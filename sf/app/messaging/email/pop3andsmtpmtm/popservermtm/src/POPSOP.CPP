// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <e32std.h>
#include "POPS.H"		//CImPop3Session
#include "POPSOP.H"		//CImPop3Operation
#include "POPS.PAN"		//IMPP panic codes

#include <imcvrecv.h>	//CImRecvConvert
#include <imcvutil.h>   //ImCheckDiskSpace utility class
#include <msvapi.h>     //MessageServer utility class

const TInt KTOPRequestSize = 18; // i.e. 'TOP nnnn 1\r\n' where nnnn can be a 32 bit integer
const TInt KTOPBatchLimit = 100; // Pipelining limit of 100 TOP requests at a time

// string constants used whilst parsing response from POP3 email server
_LIT8(KAuthMatchString,"*SASL *");
_LIT8(KAuthPlainMatchString,"* PLAIN*");
_LIT8(KAuthLoginMatchString,"* LOGIN*");
_LIT8(KAuthCramMD5MatchString,"* CRAM-MD5*");

// Special case for non-compliant Domino servers 
_LIT8(KAuthDominoMatchString,"*AUTH=*"); 
_LIT8(KAuthDominoPlainMatchString,"*=PLAIN*"); 
_LIT8(KAuthDominoLoginMatchString,"*=LOGIN*"); 
_LIT8(KAuthDominoCramMD5MatchString,"*=CRAM-MD5*"); 

// Utility functions declared in IMPP.CPP
GLREF_C TBool CommandAccepted(TDesC8& aResponse);

// IMPP Panic function
GLREF_C void Panic(TPopsPanic aPanic);
//
// CImPop3Operation - an Abstract base for all the specific POP3 operations
//
CImPop3Operation::CImPop3Operation()
	: CMsgActive(KImPopSessionPriority),iFullStopTerminator(_S8(".\r\n"))
	{
	_LIT(KPop3Operation,"CImPop3Operation");
	__DECLARE_NAME(KPop3Operation());
	}

CImPop3Operation::~CImPop3Operation()
	{
	Cancel();
	delete iImId;
	}

//
// 2nd stage of construction
//
void CImPop3Operation::Construct(CImPop3Session *aPopSession)
	{
	iPopSession=aPopSession;
	iTextServer=aPopSession->TextServerSession();
	CActiveScheduler::Add(this); 
	}

void CImPop3Operation::Construct(CImPop3Session *aPopSession, CImRecvConvert* aRecvConverter)
	{
	Construct(aPopSession);
	iRecvConverter=aRecvConverter;
	}

void CImPop3Operation::DoRunL()
	{
	iPopSession->SetOpNotPending();

	__ASSERT_ALWAYS(iTextServer->GetCurrentTextLine(iTextServerResponse)==ECRLFTerminated,User::Leave(EImppBufferNotTerminated));
	if(CommandAccepted(iTextServerResponse)==EFalse)
		{
		// Message has disapeared already
		}
	}

void CImPop3Operation::DoCancel()
	{
	iTextServer->Cancel();
	if (iRecvConverter != NULL)
		{
		iRecvConverter->Cancel();
		}
	CMsgActive::DoCancel();
	}
//
// Queue a read operation to get next line of text
//
void CImPop3Operation::QueueReadNext()
	{
	iTextServer->QueueReceiveNextTextLine(iStatus);
	SetActive();
	}

//
// Return the Pop3 mailbox index no of a message with give Babel msg id
//
TInt CImPop3Operation::MessageNo(TMsvId aMsgId)
	{
	TInt msgNo=iPopSession->MessageNo(aMsgId);
	return msgNo;
	}

//
// Check if there's a poperation outstainding if so panic
//
void CImPop3Operation::CheckIfPending()
	{
	iPopSession->IsPending() ? Panic(EImppOperationAlreadyPending) : iPopSession->SetPending();
	}

//
// Retrieve a full message or just the specified no of lines
//
void CImPop3Operation::RetrieveMessageL()
	{
	iPopSession->SetOpNotPending();
	
	iTextServer->GetCurrentTextLine(iTextServerResponse);

	if(iFirstLine)
		{ 
		iParsedHeader=EFalse;
		iCommandAccepted=CommandAccepted(iTextServerResponse);
		iFirstLine=EFalse;
		if(!iCommandAccepted)
			{
			return;
			}
		}
	else
		{ 
		TBool fakeDot=EFalse;
		if(iTextServerResponse.Compare(iFullStopTerminator)!=0)  // header + first n lines last line is a full stop
			{
			RemoveFirstDot();
			if (iRecvConverter->ParseNextField( iTextServerResponse ) == KErrNoMemory)
				{
				User::Leave(KErrNoMemory);
				}
			}
		else if(iParsedHeader==EFalse)
			{
			RemoveFirstDot();
			_LIT8(KCrLfStr,"\r\n");
			if (iRecvConverter->ParseNextField( KCrLfStr ) == KErrNoMemory)
				{
				User::Leave(KErrNoMemory);
				}    
			fakeDot=ETrue;
			}
		else
			return;
  
		// test for comp header
		if(iRecvConverter->ValidCompleteHeader())
			{
			iParsedHeader=ETrue;
			// test for valid headers
			const CImHeader& header = iRecvConverter->Header();
			iEmptyHeaders=(   (header.From().Length()==0)
							&&(header.ReplyTo().Length()==0)
							&&(header.ToRecipients().Count()==0)
							&&(header.CcRecipients().Count()==0)
							&&(header.BccRecipients().Count()==0));
		
			// store id of new entry created by IMCV
			iNewEntryId = iRecvConverter->EntryId();
			// store internet msg id
			HBufC8* imId = header.ImMsgId().AllocL();
			delete iImId;
			iImId = NULL;
			iImId = imId;
			}
 
		if(fakeDot!=EFalse)
			{
			return;
			}
		}
 
	QueueReadNext();
	}

//
// What to do on completion
//
void CImPop3Operation::DoComplete(TInt& /*aCompleteRequest*/)
	{
	}

TBool CImPop3Operation::PopCommandAccepted()
	{
	return iCommandAccepted;
	}

//
// Remove first dot of a pair
//
void CImPop3Operation::RemoveFirstDot()
	{
	if(iTextServerResponse.Length()>2)
		{
		if( (iTextServerResponse[0]=='.') && (iTextServerResponse[1]=='.'))
			{
			iTextServerResponse.Delete(0,1);
			}
		}
	}

TPtrC8 CImPop3Operation::ImMsgId()
	{ 
	return (iImId ? TPtrC8(*iImId) : TPtrC8()); 
	}

TBool CImPop3Operation::SetMessage(TMsvId anId)
	{
	iMessageNo=MessageNo(anId);
	return iMessageNo == KErrNotFound ? EFalse: ETrue;
	}


//
// CImPop3Stat return no of messages and the mailbox size
//
CImPop3Stat* CImPop3Stat::NewL(CImPop3Session *aPopSession)
	{
	CImPop3Stat* self = new (ELeave) CImPop3Stat();
	self->Construct(aPopSession);
	return self;
	}

void CImPop3Stat::Start(TRequestStatus& aStatus, TUint &aNoMsg, TUint &aMboxSize)
	{
	CheckIfPending();

	iNoMessages=&aNoMsg;
	iMailboxSize=&aMboxSize;

	_LIT8(KStatCrLfStr,"STAT\r\n");
	iTextServer->SendQueueReceive(iStatus, KStatCrLfStr());
	SetActive();
	Queue(aStatus);
	}

// Got a response from the text server

void CImPop3Stat::DoRunL()
	{
	iPopSession->SetOpNotPending();
	__ASSERT_ALWAYS(iTextServer->GetCurrentTextLine(iTextServerResponse)==ECRLFTerminated,User::Leave(EImppBufferNotTerminated));
		
	if(CommandAccepted(iTextServerResponse))
		{
		TLex8 parseResponse(iTextServerResponse);
		parseResponse.SkipCharacters();
		parseResponse.SkipSpace();

		if(parseResponse.Val(*iNoMessages)==KErrNone)
			{
			parseResponse.SkipSpace();
			parseResponse.Val(*iMailboxSize);
			}
		else
			*iNoMessages=0;
		}
	}

//
// CImPop3List list a specific message no  and size or all messages and sizes
//
CImPop3List* CImPop3List::NewL(CImPop3Session *aPopSession)
	{
	CImPop3List* self = new (ELeave) CImPop3List();
	self->Construct(aPopSession);
	return self;
	}

void CImPop3List::Start(TRequestStatus& aStatus, TUint* aSize)
	{
	CheckIfPending();
	iSize=aSize;
	iArrayPos=0;

	if(iMessageNo!=0)
		{
		_LIT8(KListArgCrLfStr,"LIST %d\r\n");
		iTextServer->SendQueueReceive(iStatus, KListArgCrLfStr, iMessageNo);
		}
	else
		{
		_LIT8(KListCrLfStr,"LIST\r\n");
		iTextServer->SendQueueReceive(iStatus,KListCrLfStr());
		iFirstLine=ETrue;
		}

	SetActive();
	Queue(aStatus);
	}

void CImPop3List::DoRunL()
	{
	iPopSession->SetOpNotPending();
	
	__ASSERT_ALWAYS(iTextServer->GetCurrentTextLine(iTextServerResponse)==ECRLFTerminated,User::Leave(EImppBufferNotTerminated));
	TLex8 parseResponse(iTextServerResponse);

	if(iNoResponses > 0)
		{
		if(CommandAccepted(iTextServerResponse))
		User::Leave(KErrNotFound);
		}
		
	if(iMessageNo!=0)
		{
		if(CommandAccepted(iTextServerResponse))
			{
			parseResponse.SkipCharacters();
			parseResponse.SkipSpace();
			GetIndexAndSize(&parseResponse);
			}
		else
			Panic(EImppBabelMsgDoesNotExist);
		}
	else
		{
		if(iFirstLine)
			{
			if(CommandAccepted(iTextServerResponse))
				{
				iFirstLine=EFalse;
				}
			else
				{
				return;
				}
			}
		else
			{
			if(iTextServerResponse.Compare(iFullStopTerminator)==0)  // list of message info last line is a full stop
				{
				return;
				}
			else
				{
				if(GetIndexAndSize(&parseResponse)==KErrNone)
					{
					iArrayPos++;
					iNoResponses++;
					}
				else
					{
					iSize[iArrayPos] = 0;
					}
				}
			}
		QueueReadNext();
		}
	}

//
// Get size of a specific message (folder view displays message sizes)
//
TInt CImPop3List::GetIndexAndSize(TLex8 *aLex)
	{
	aLex->SkipCharacters();
	aLex->SkipSpace();
	return(aLex->Val(iSize[iArrayPos]));
	}

//
// CImPop3Retr Retrieve a specific message
//
CImPop3Retr* CImPop3Retr::NewL(CImPop3Session *aPopSession, CImRecvConvert* aRecvConverter, RFs& aFs)
	{
	CImPop3Retr* self = new (ELeave) CImPop3Retr(aFs);
	CleanupStack::PushL(self);
	self->ConstructL(aPopSession, aRecvConverter);
	CleanupStack::Pop(self);
	return self;
	}
void CImPop3Retr::ConstructL(CImPop3Session *aPopSession, CImRecvConvert* aRecvConverter)
	{
	Construct(aPopSession,aRecvConverter);
	iCurrentDrive = MessageServer::CurrentDriveL(iFs);
	}

void CImPop3Retr::StartL(TRequestStatus& aStatus)
	{
	CheckIfPending();
	iFirstLine=ETrue;
	iNoBytesRetrieved=0;
	iEmptyHeaders = EFalse;
	ImCheckDiskSpace::LeaveIfLowDiskL(iFs, iCurrentDrive);

	iRecvConverter->ResetL();
	
	_LIT8(KRetrArgCrLfStr,"RETR %d\r\n");
	iTextServer->SendQueueReceive(iStatus, KRetrArgCrLfStr, iMessageNo);
	SetActive();
	Queue(aStatus);
	}

void CImPop3Retr::DoRunL()
	{
	RetrieveMessageL();
	if(iFirstLine==EFalse)
		{
		iNoBytesRetrieved+=iTextServerResponse.Length();
		}
	}

TUint CImPop3Retr::Progress()
	{
	return iNoBytesRetrieved;
	}

void CImPop3Retr::SetMessageIndex(TInt anIndex)
	{
	iMessageNo=anIndex;
	}
//
// CImPop3Dele Delete a specified message
//
CImPop3Dele* CImPop3Dele::NewL(CImPop3Session *aPopSession)
	{
	CImPop3Dele* self = new (ELeave) CImPop3Dele();
	self->Construct(aPopSession);
	return self;
	}

void CImPop3Dele::Start(TRequestStatus& aStatus)
	{
	CheckIfPending();

	_LIT8(KDeleArgCrLfStr,"DELE %d\r\n");
	iTextServer->SendQueueReceive(iStatus, KDeleArgCrLfStr, iMessageNo);
	SetActive();
	Queue(aStatus);
	}


//
// CImPop3Noop No operation (but I'm still here)
//
CImPop3Noop* CImPop3Noop::NewL(CImPop3Session *aPopSession)
	{
	CImPop3Noop* self = new (ELeave) CImPop3Noop();
	self->Construct(aPopSession);
	return self;
	}

void CImPop3Noop::Start(TRequestStatus& aStatus)
	{
	CheckIfPending();

	_LIT8(KNoopCrLfStr,"NOOP\r\n");
	iTextServer->SendQueueReceive(iStatus, KNoopCrLfStr());
	SetActive();
	Queue(aStatus);
	}

//
// CImPop3Rset Reset
//
CImPop3Rset* CImPop3Rset::NewL(CImPop3Session *aPopSession)
	{
	CImPop3Rset* self = new (ELeave) CImPop3Rset();
	self->Construct(aPopSession);
	return self;
	}

void CImPop3Rset::Start(TRequestStatus& aStatus)
	{
	CheckIfPending();
	_LIT8(KRsetCrLfStr,"RSET\r\n");
	iTextServer->SendQueueReceive(iStatus, KRsetCrLfStr());
	SetActive();
	Queue(aStatus);
	}

//
// Return specified header and top n lines of message.
// If the POP server supports pipelining, TOP requests will be submitted in
// batches. The batch size limit is 100 requests.
//
CImPop3Top* CImPop3Top::NewL(CImPop3Session *aPopSession, CImRecvConvert* aRecvConverter,TBool aHeaderOnly)
	{
	CImPop3Top* self = new (ELeave) CImPop3Top();
	self->ConstructL(aPopSession,aRecvConverter,aHeaderOnly);
	return self;
	}

void CImPop3Top::ConstructL(CImPop3Session *aPopSession, CImRecvConvert* aRecvConverter,TBool aHeaderOnly)
	{
	CImPop3Operation::Construct(aPopSession,aRecvConverter);
	iHeadersOnly = aHeaderOnly;
	}

void CImPop3Top::StartL(TRequestStatus& aStatus)
	{
	CheckIfPending();
	iFirstLine=ETrue;
	iNoBytesRetrieved=0;
	iEmptyHeaders = EFalse;

	if (iHeadersOnly)
		{
		iRecvConverter->ResetForHeadersL();
		}
	else
		{
		iRecvConverter->ResetL();
		}

	if(iPipeliningSupported)
		{
		if(iMessageNo <= iLastMulti)
			{
			TInt numberToGet = iLastMulti - iStartMsg;
			TInt tempStartPos(iStartMsg);
			if (numberToGet > KTOPBatchLimit)
				{
				tempStartPos = iLastMulti - KTOPBatchLimit;
				numberToGet = KTOPBatchLimit;
				}
			HBufC8* buffer = HBufC8::NewLC(numberToGet * KTOPRequestSize);
			TPtr8 bufferPtr = buffer->Des();
			for(; iLastMulti > tempStartPos; --iLastMulti)
				{
 				if(iPopSession->MessageArray()[iLastMulti-1]==0)
					{
					_LIT8(KTopArgArgCrLfStr,"TOP %d %d\r\n");
 					bufferPtr.AppendFormat( KTopArgArgCrLfStr, iLastMulti, iNoLines );
 					}
				}
			iTextServer->SendQueueReceive(iStatus, bufferPtr);
			CleanupStack::PopAndDestroy(buffer);
			}
		else
			{
			// These headers should be on their way already, queue another read
            QueueReadNext(); // This calls SetActive() for us
            Queue(aStatus);
            return;
			}
		}
	else // pipelining not supported by server
		{
		_LIT8(KTopArgArgCrLfStr,"TOP %d %d\r\n");
		iTextServer->SendQueueReceive(iStatus, KTopArgArgCrLfStr, iMessageNo, iNoLines);
		}
	SetActive();
	Queue(aStatus);
	}

void CImPop3Top::DoRunL()
	{
	RetrieveMessageL();
	if(iFirstLine==EFalse)
		{
		iNoBytesRetrieved+=iTextServerResponse.Length();
		}
	}

TBool CImPop3Top::SetMessageAndLines(TMsvId aId, TInt aNoLines)
	{
	iNoLines=aNoLines;
	return SetMessage(aId);
	}

void CImPop3Top::SetMessageIndexAndLines(TInt anIndex, TInt aNoLines)
	{
	iMessageNo=anIndex;
	iNoLines=aNoLines;
	}
	
void CImPop3Top::SetStartAndEndMessageIndex(TInt aStart, TInt aEnd)
	{
	iPipeliningSupported = ETrue;
	iStartMsg = aStart;
	iLastMulti = aEnd + 1; // off by one 
	}

TUint CImPop3Top::Progress()
	{
	return iNoBytesRetrieved;
	}

// CImPop3UidlMap
//
CImPop3UidlMap* CImPop3UidlMap::NewL(TInt aMsgCount)
	{
	return new(ELeave) CImPop3UidlMap(aMsgCount);
	}

CImPop3UidlMap::~CImPop3UidlMap()
	{}

void CImPop3UidlMap::BindL(TInt aMsgIx,const TDesC8& aUidl)
	{
	// Insert mail Uidls into Uidl array including duplicated uidls.
	TInt pos = iUid.InsertIsqAllowDuplicatesL(aUidl,ECmpNormal); 
	iMsgNo.InsertL(pos,aMsgIx);
	}


TInt CImPop3UidlMap::MsgNo(const TDesC8& aUidl) const
	{
	TInt pos;
	if (iUid.FindIsq(aUidl,pos,ECmpNormal)==0)
		{
		return iMsgNo[pos];
		}
	return -1;
	}

TPtrC8 CImPop3UidlMap::At(TInt aMsgIx) const
	{
	if (iMsgNo.Count())
		{
		const TInt* p=&iMsgNo[0];
		for (TInt ii=iMsgNo.Count();--ii>=0;)
			{
			if (p[ii]==aMsgIx)
				{
				return iUid[ii];
				}
			}
		}
	return TPtrC8();
	}

CImPop3UidlMap::CImPop3UidlMap(TInt aMsgCount)
	:iUid(aMsgCount),iMsgNo(aMsgCount)
	{
	__DECLARE_NAME(_S("CImPop3UidlMap"));
	}

//
// CImPop3 Uidl return unique id for a specified message or all the messages
//
CImPop3Uidl* CImPop3Uidl::NewL(CImPop3Session *aPopSession)
	{
	CImPop3Uidl* self = new (ELeave) CImPop3Uidl();
	self->Construct(aPopSession);
	return self;
	}

void CImPop3Uidl::Start(TRequestStatus& aStatus, CImPop3UidlMap& anArray)
	{
	CheckIfPending();
	iMsgUid=&anArray;
	if(iMessageNo!=0)
		{
		_LIT8(KUidlArgCrLfStr,"UIDL %d\r\n");
		iTextServer->SendQueueReceive(iStatus, KUidlArgCrLfStr, iMessageNo);
		}
	else
		{
		_LIT8(KUidlCrLfStr,"UIDL\r\n");
		iTextServer->SendQueueReceive(iStatus, KUidlCrLfStr());
		iFirstLine=ETrue;
		}
	SetActive();
	Queue(aStatus);
	}

void CImPop3Uidl::DoRunL()
	{
	iPopSession->SetOpNotPending();
	__ASSERT_ALWAYS(iTextServer->GetCurrentTextLine(iTextServerResponse)==ECRLFTerminated,User::Leave(EImppBufferNotTerminated));
	TLex8 parseResponse(iTextServerResponse);

	if(iMessageNo!=0) // one specific Uid
		{
		iCommandAccepted=CommandAccepted(iTextServerResponse);
		if(iCommandAccepted)
			{
			parseResponse.SkipCharacters();
			parseResponse.SkipSpace();
			GetIndexAndUidL(&parseResponse);
			}
		else
			Panic(EImppBabelMsgDoesNotExist);
		}
	else // list all Uids
		{
		if(iFirstLine)
			{
			iCommandAccepted=CommandAccepted(iTextServerResponse);
			if(iCommandAccepted)
				{
				iFirstLine=EFalse;
				}
			else
				{
				return;
				}
			}
		else
			{
			if(iTextServerResponse.Compare(iFullStopTerminator)==0) // list of message info last line is a full stop
				{
				return;
				}
			else
				{
				GetIndexAndUidL(&parseResponse);
				}
			}
		QueueReadNext();
		}
	}

//
// Get index no and Pop id of a message
//
void CImPop3Uidl::GetIndexAndUidL(TLex8* aLex)
	{
	TInt msgNo;
	TInt lexErr=aLex->Val(msgNo);
	TLexMark8 uidMarker;

	if(lexErr==KErrNone)
		{
		aLex->SkipSpace();
		aLex->Mark(uidMarker);
		aLex->SkipCharacters();
		iMsgUid->BindL(msgNo-1,aLex->MarkedToken( uidMarker ));
		}
	}

//
// CImPop3Capa returns the list of capabilities supported by the server
//
CImPop3Capa* CImPop3Capa::NewL(CImPop3Session *aPopSession)
	{
	CImPop3Capa* self = new (ELeave) CImPop3Capa();
	self->Construct(aPopSession);
	return self;
	}

void CImPop3Capa::Start(TRequestStatus& aStatus)
	{
	CheckIfPending();
	_LIT8(KCapaCrLfStr,"CAPA\r\n");
	iTextServer->SendQueueReceive(iStatus, KCapaCrLfStr());
	iFirstLine = ETrue;
	SetActive();
	Queue(aStatus);
	}

// Got a response from the text server
void CImPop3Capa::DoRunL()
	{
	iPopSession->SetOpNotPending();

	iTextServer->GetCurrentTextLine(iTextServerResponse);
	if(iFirstLine)
		{ 
		iCommandAccepted = CommandAccepted(iTextServerResponse);
		iFirstLine = EFalse;
		if(!iCommandAccepted)
			{
			return;
			}
		}
	else
		{
		if(iTextServerResponse.Compare(iFullStopTerminator) != 0)
			{
			TInt crPos = iTextServerResponse.Find(KImcvCRLF);
			if(crPos>0)
				{
				TPtrC8 ptr;
				ptr.Set(iTextServerResponse.Left(crPos));
				_LIT8(KPipelining, "PIPELINING");
				
				if(ptr.CompareF(KPipelining) == 0)
					{
					iPipeliningSupported = ETrue;
					}
					
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)				
				if (iTextServerResponse.Match(KAuthMatchString) != KErrNotFound || 
    					iTextServerResponse.Match(KAuthDominoMatchString) != KErrNotFound)
					{
					SetSaslAuthExtensionFlag();
					}
#endif					
				}
			else
				return;
			}
		else
			return;
		}
	QueueReadNext();
	}

TBool CImPop3Capa::PipeliningSupport()
	{
	return iPipeliningSupported;
	}
	
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)

/**
To check the Server Response for CAPABILITY command and set iSupportedAuthProfiles accordingly.
*/
void CImPop3Capa::SetSaslAuthExtensionFlag()
	{
	// check for each supported profile
	if (iTextServerResponse.Match(KAuthMatchString) != KErrNotFound)
		{
		if (iTextServerResponse.Match(KAuthPlainMatchString) != KErrNotFound)
			{
			iSupportedAuthProfiles|=CPopAuthMechanismHelper::EPlain;
			}
			
		if (iTextServerResponse.Match(KAuthLoginMatchString) != KErrNotFound)
			{
			iSupportedAuthProfiles|=CPopAuthMechanismHelper::ELogin;
			}
			
		if (iTextServerResponse.Match(KAuthCramMD5MatchString) != KErrNotFound)
			{
			iSupportedAuthProfiles|=CPopAuthMechanismHelper::ECramMD5;
			}
		}
		
	// Special case for Domino servers tagging "=" after "AUTH" 
	if (iTextServerResponse.Match(KAuthDominoMatchString) != KErrNotFound)	
		{
		if (iTextServerResponse.Match(KAuthDominoPlainMatchString) != KErrNotFound) 
			{
			iSupportedAuthProfiles|=CPopAuthMechanismHelper::EPlain; 
			}
    		
   		if (iTextServerResponse.Match(KAuthDominoLoginMatchString) != KErrNotFound) 
   			{
   			iSupportedAuthProfiles|=CPopAuthMechanismHelper::ELogin; 
   			}
    		
   		if (iTextServerResponse.Match(KAuthDominoCramMD5MatchString) != KErrNotFound) 
   			{
   			iSupportedAuthProfiles|=CPopAuthMechanismHelper::ECramMD5;
   			}
   		}
   	}

/**
To get iSupportedAuthProfiles value.
*/	
TUint CImPop3Capa::SaslAuthExtensionFlag()
	{
	return iSupportedAuthProfiles;
	}
#endif

// Function will return Index of message(if unique UIDL) Or KErrAlreadyExists(if Duplicate UIdl)
//Or KErrNotFound if UIdl not present..
TInt CImPop3UidlMap::MsgUidlNo(const TDesC8& aUidl,TInt& aPos) const
	{
	if (iUid.FindIsq(aUidl,aPos,ECmpNormal) == 0)
		{
		if(aPos > 0 )
			{
			//To Check that the index does not exceed size of the iUid array.
		 	if(aPos == (iMsgNo.Count() - 1))
		 		{
		  		if ((iUid[aPos-1]) == aUidl) 
		 			{
					return KErrAlreadyExists;
					}
				 }
			 else
			 	{
			 	if ((iUid[aPos-1]) == aUidl || (iUid[aPos+1]) == aUidl) 
			 		{
			 		return KErrAlreadyExists;
			 		}
			 		
			 	}	 
			}
			return iMsgNo[aPos];
			
		}
	return KErrNotFound;
	}


// To return the Index of the UIDL corresponding to a UIDL position.
TInt CImPop3UidlMap::MsgIndex(TInt aPos) const
	{
	return iMsgNo[aPos];
	}

// To get the no. of UIDLs in the server..
TInt CImPop3UidlMap::MsgCount() const
	{
	return iMsgNo.Count();
	}

//Return the UIDL at aPos.
TPtrC8  CImPop3UidlMap::MsgUidl(TInt aPos) const
	{
	return iUid[aPos];
	}
