// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include <e32math.h>
#include <txtrich.h>
#include <bautils.h>
#include <miut_err.h>

#include "IMUTDLL.H"
#include "IMCVSEND.H"

#include <miuthdr.h>
#include <miutpars.h>

#include <imcm.rsg>	
#include <barsread.h>	// TResourceReader

#include <cmsvattachment.h> 
#include <mmsvattachmentmanager.h>
#include <cmsvplainbodytext.h>		

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "miut_errconsts.h"
#include "timrfc822datefield.h"
#include "cimconvertheader.h"
#endif


#define SENDLOG(x)				\
	if (iLogFileExists)			\
		iLogFile.Write(x);	


#define SENDLOG2(x,y)			\
	if (iLogFileExists)			\
		{						\
		iLogFile.Write(x);		\
		iLogFile.Write(y);		\
		}

_LIT(KLogFilePath, "c:\\logs\\mailtext\\");
_LIT(KLogFileName, "out.txt");
_LIT8(KNewLogHeader, "\r\n------ New Send Session ------\r\n");
_LIT8(KSent, "Sent : ");
_LIT8(KSetBody, "Body length = %d\r\n");

const TInt KArrayGranularity = 3;
const TInt KChunkSize = 1024;


//*********************************************************************************
//              Class CImSendMessage Functions
//*********************************************************************************

//---------------------------------------------------------------------------------
EXPORT_C CImSendMessage* CImSendMessage::NewLC(CMsvServerEntry& aServerEntry)
//---------------------------------------------------------------------------------
	{
	CImSendMessage* self = new (ELeave) CImSendMessage(aServerEntry);
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

//---------------------------------------------------------------------------------
EXPORT_C CImSendMessage* CImSendMessage::NewL(CMsvServerEntry& aServerEntry)
//---------------------------------------------------------------------------------
	{
	CImSendMessage* self = CImSendMessage::NewLC(aServerEntry);
	CleanupStack::Pop( );
	return self;
	}

//---------------------------------------------------------------------------------
CImSendMessage::CImSendMessage(CMsvServerEntry& aServerEntry) : iServerEntry(aServerEntry)
//---------------------------------------------------------------------------------
	{	
	}


//---------------------------------------------------------------------------------
void CImSendMessage::ConstructL()
//---------------------------------------------------------------------------------
	{
	// logfile stuff
	TInt ret=0;
	TParse logFileName;
	logFileName.Set(KLogFileName, &KLogFilePath, NULL);
	TFileName filename(logFileName.FullName());
	ret=iLogFile.Replace(iServerEntry.FileSession(), filename, EFileShareExclusive|EFileWrite );
	iLogFileExists = (ret==KErrNone);
	SENDLOG( KNewLogHeader )

	}

//---------------------------------------------------------------------------------
EXPORT_C void CImSendMessage::InitialiseL(TMsvId aMessageId, TImSendMethod aSendMethod, 
					const TTime aTimeDate, const TDesC& aDomainName, 
					TUint aCharset, const TImSMTPSendCopyToSelf aCopyToSelf)
//---------------------------------------------------------------------------------
	{
	TImEmailTransformingInfo info;
	info.SetToDefault(aSendMethod); // Sets defaults
	info.SetHeaderAndBodyCharset(aCharset) ; // Sets charset for header/body

	InitialiseL(aMessageId, aTimeDate, aDomainName, info, aCopyToSelf);
	}


//---------------------------------------------------------------------------------
EXPORT_C void CImSendMessage::InitialiseL(TMsvId aMessageId, const TTime aTimeDate,
							const TDesC& aDomainName,	const TImEmailTransformingInfo& aInfo,
							const TImSMTPSendCopyToSelf aCopyToSelf)
//---------------------------------------------------------------------------------
	{
	User::LeaveIfError(iServerEntry.SetEntry(aMessageId));
	__ASSERT_ALWAYS(iServerEntry.Entry().iType==KUidMsvMessageEntry, gPanic(KPanicEntryIsNotMessage));

	SetSendMethodL(aInfo.SendMethod());
	iSendEmail->InitialiseL(aTimeDate, aDomainName, aCopyToSelf, aInfo);
	Reset();

	}

//---------------------------------------------------------------------------------
EXPORT_C void CImSendMessage::Reset()
//---------------------------------------------------------------------------------
	{
	iSendEmail->Reset();
	}


// Creates the required email sending object, either Mime or non MIME.
//---------------------------------------------------------------------------------
void CImSendMessage::SetSendMethodL(TImSendMethod aSendMethod)
//---------------------------------------------------------------------------------
	{
	switch (aSendMethod)
		{
		case ESendAsSimpleEmail:
			iSendEmail = CImSendPlainEmail::NewL(iServerEntry);
			break;
		case ESendAsMimeEmail:
			iSendEmail = CImSendMimeEmail::NewL(iServerEntry);
			break;
		default:
			gPanic(KPanicUnknownSendingMethod);
		}
	}


//---------------------------------------------------------------------------------
EXPORT_C TInt CImSendMessage::NextLineL( TDes8& rOutputLine, TInt& rPaddingCount  )
//---------------------------------------------------------------------------------
	{
	// Format the next line of output into rOutputLine.
	// If the output line contains more characters than the size of the source data cosumed 
	// in preparing it, then the difference is passed back in aPaddingCount.
	
	TInt localPaddingCount=0;

	rOutputLine.Zero();
	TInt ret = iSendEmail->NextLineL( rOutputLine, localPaddingCount );

	AddCRLFAtEndOfLine(rOutputLine, localPaddingCount);

	SENDLOG(KSent);
	SENDLOG(rOutputLine);

	rPaddingCount = localPaddingCount;
	return ret;
	}


//---------------------------------------------------------------------------------
EXPORT_C CImSendMessage::~CImSendMessage( )
//---------------------------------------------------------------------------------
	{
	if (iLogFileExists)
		iLogFile.Close();
	delete iSendEmail;
	}

//---------------------------------------------------------------------------------
EXPORT_C TInt CImSendMessage::HeaderSize()
//---------------------------------------------------------------------------------
	{
	TInt size = iSendEmail->HeaderSize();
	iSendEmail->Reset();
	return size;
	}


//---------------------------------------------------------------------------------
EXPORT_C TInt CImSendMessage::BodySizeL()
//---------------------------------------------------------------------------------
	{
	return iSendEmail->BodySizeL();
	}


//*********************************************************************************
//              Class CImSendEmail Functions
//*********************************************************************************

//---------------------------------------------------------------------------------
CImSendEmail::CImSendEmail(CMsvServerEntry& aServerEntry) : 
							iServerEntry(aServerEntry), iDomainName(TPtrC())
//---------------------------------------------------------------------------------
	{
	}


//---------------------------------------------------------------------------------
CImSendEmail::~CImSendEmail( )
//---------------------------------------------------------------------------------
	{
	delete iCharConv;
	delete iCharacterConverter;
	delete iHeaderConverter;

	delete iSendFile;
	delete iEmailTraverser;

	delete iRfc822Header;
	delete iSendRichText;
	}

//---------------------------------------------------------------------------------
void CImSendEmail::ConstructL()
//---------------------------------------------------------------------------------
	{
	iCharacterConverter = CCnvCharacterSetConverter::NewL();
	iCharConv = CImConvertCharconv::NewL(*iCharacterConverter, iServerEntry.FileSession());

	iSendFile =	CImSendFile::NewL(iServerEntry.FileSession(), *iCharConv);

	iEmailTraverser = CImEmailTraverser::NewL(iServerEntry);

	iHeaderConverter = CImConvertHeader::NewL(*iCharConv);
	iRfc822Header = CImSendRfc822Header::NewL(iServerEntry.FileSession(), *iHeaderConverter);
	iSendRichText = CImSendRichText::NewL(*iCharConv);
	}

//---------------------------------------------------------------------------------
void CImSendEmail::InitialiseL( const TTime aTimeDate, const TDesC& aDomainName,
							   const TImSMTPSendCopyToSelf aCopyToSelf,
							   const TImEmailTransformingInfo& aTransformingInfo)
//---------------------------------------------------------------------------------
	{
	// get a list of all attachments
	iDomainName.Set(aDomainName);
	iDefaultTransformingInfo=aTransformingInfo;
	iTimeDate=aTimeDate;
	
	iRfc822Header->InitialiseL(iTimeDate,	iDomainName, iServerEntry, 
					iServerEntry.Entry().Priority(), aCopyToSelf, iDefaultTransformingInfo);

	iEmailTraverser->InitialiseL( iServerEntry.Entry().Id() );

	}


//---------------------------------------------------------------------------------
TBool CImSendEmail::InitBodyObjectL(TMsvEntry& rEntry)
//---------------------------------------------------------------------------------
	{
	return ( rEntry.iType==KUidMsvEmailTextEntry
		  && iSendRichText->InitialiseL( iServerEntry, 
							Encoder(EEncodingTypeNone), EEncodingTypeNone,
							iRfc822Header->TransformingInfo().BodyTextCharset()) );
	}


// calculates & returns the total size of the header info, including the RFC822 tags, 
// comma & space separation, the CRLFs and the blank line after the header itself

//---------------------------------------------------------------------------------
TInt CImSendEmail::HeaderSize()
//---------------------------------------------------------------------------------
	{
	return iRfc822Header->Size();
	}

// Sum of body parts and file attachments.
//---------------------------------------------------------------------------------
TInt CImSendEmail::BodySizeL()
//---------------------------------------------------------------------------------
	{
	TMsvEntry entry;
	TInt size=0;
	TBool isEndOfEmail=EFalse;

	entry = iEmailTraverser->ThisEntry();
	do {
		if (InitBodyObjectL(entry))
			size += iSendRichText->Size();
		else if (InitAttachmentObjectL(entry))
			size += iSendFile->Size();

		if ( iEmailTraverser->DownLevelL() )
			entry = iEmailTraverser->ThisEntry();
		else
			while ( !iEmailTraverser->NextEntryL(entry) )
				{
				if ( !iEmailTraverser->UpLevelL() || iEmailTraverser->IsBaseLevel() )
					{
					// end of traversal, reached message entry.
					isEndOfEmail=ETrue;
					break;
					}
				}

	} while (!isEndOfEmail);

	return size;
	}

//---------------------------------------------------------------------------------
void CImSendEmail::Reset()
//---------------------------------------------------------------------------------
	{
	if(iRfc822Header)
		{		
		iRfc822Header->Reset();
		}
	iState = 0;
	}

//---------------------------------------------------------------------------------
TImFileCodec&  CImSendEmail::Encoder(TImEncodingType aType)
//---------------------------------------------------------------------------------
	{
	switch (aType)
		{
	case EEncodingTypeUU: 
		iUUEncoder.Initialise();
		return iUUEncoder;
	case EEncodingTypeBASE64: 
		iB64Encoder.Initialise();
		return iB64Encoder;
	case EEncodingTypeQP: 
		return iQPEncoder;
	case EEncodingTypeNone:
		return iNULLEncoder;
	default:
		__ASSERT_DEBUG( ETrue, gPanic(KPanicInvalidEncodingType) );
		return iNULLEncoder;
		}
	}


//---------------------------------------------------------------------------------
TBool CImSendEmail::InitAttachmentObjectL(TMsvEntry& rEntry)
//---------------------------------------------------------------------------------
	{
	if ( !(rEntry.iType==KUidMsvAttachmentEntry || rEntry.iType==KUidMsvEmailHtmlEntry) )
		return EFalse;
		
	iSendFile->InitialiseL(iServerEntry, Encoder(iRfc822Header->TransformingInfo().AttachmentEncoding())); 
	return ETrue;
	}



//*********************************************************************************
//              Class CImEmailTraverser Functions
//*********************************************************************************


//---------------------------------------------------------------------------------
CImEmailTraverser* CImEmailTraverser::NewL( CMsvServerEntry& aServerEntry)
//---------------------------------------------------------------------------------
	{
	CImEmailTraverser* self = new (ELeave) CImEmailTraverser( aServerEntry );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( );
	return self;
	}

//---------------------------------------------------------------------------------
CImEmailTraverser::CImEmailTraverser( CMsvServerEntry& aServerEntry ) : 
				iServerEntry(aServerEntry), iCurrentEntryList(KArrayGranularity)
//---------------------------------------------------------------------------------
	{
	iBaseId = iServerEntry.Entry().Id();
	}

//---------------------------------------------------------------------------------
void CImEmailTraverser::ConstructL()
//---------------------------------------------------------------------------------
	{
	iSelectionList = new (ELeave) CArrayFixFlat<CMsvEntrySelection*>(KArrayGranularity);
	}

//---------------------------------------------------------------------------------
CImEmailTraverser::~CImEmailTraverser()
//---------------------------------------------------------------------------------
	{
	Reset();
	delete iSelectionList;
	}


//---------------------------------------------------------------------------------
void CImEmailTraverser::Reset()
//---------------------------------------------------------------------------------
	{
	while (DeleteCurrentList())
		;
	}

//---------------------------------------------------------------------------------
void CImEmailTraverser::InitialiseL( TMsvId aId)
//---------------------------------------------------------------------------------
	{
	iBaseId = aId;
	User::LeaveIfError(iServerEntry.SetEntry(iBaseId));
	Reset();
	}


//---------------------------------------------------------------------------------
TBool CImEmailTraverser::DownLevelL()
//---------------------------------------------------------------------------------
	{
	TMsvSelectionOrdering ordering(KMsvNoGrouping,EMsvSortById,ETrue);
	iServerEntry.SetSort(ordering);
	CMsvEntrySelection*	children = new(ELeave) CMsvEntrySelection;
	CleanupStack::PushL(children);
	User::LeaveIfError(iServerEntry.GetChildren(*children));

	if (children->Count())
		{
		AddList(*children);
		CleanupStack::Pop(children);
		User::LeaveIfError(iServerEntry.SetEntry( CurrentList()[0]));
		return ETrue;
		}
	
	CleanupStack::PopAndDestroy(children);
	return EFalse;
	}

//---------------------------------------------------------------------------------
TBool CImEmailTraverser::UpLevelL()
//---------------------------------------------------------------------------------
	{
	if (!DeleteCurrentList())
		return EFalse;

	if (!LevelExists())
		User::LeaveIfError( iServerEntry.SetEntry(iBaseId) );
	else
		User::LeaveIfError( iServerEntry.SetEntry( CurrentList()[CurrentEntry()]) );
	return ETrue;
	}

//---------------------------------------------------------------------------------
TBool CImEmailTraverser::NextEntryL(TMsvEntry& rEntry)
//---------------------------------------------------------------------------------
	{	
	if (IsBaseLevel())
		return EFalse;

	iCurrentEntryList[0] += 1;
	if ( CurrentEntry()>=CurrentList().Count() )
		return EFalse; // last entry on this level.

	if (!LevelExists())
		User::LeaveIfError( iServerEntry.SetEntry(iBaseId) );
	else
		User::LeaveIfError( iServerEntry.SetEntry( CurrentList()[CurrentEntry()]) );

	rEntry = ThisEntry();
	return ETrue;
	}


//*********************************************************************************
//              Class CImSendPlainEmail Functions
//*********************************************************************************

//---------------------------------------------------------------------------------
CImSendPlainEmail* CImSendPlainEmail::NewLC(CMsvServerEntry& aServerEntry)
//---------------------------------------------------------------------------------
	{
	CImSendPlainEmail* self = new (ELeave) CImSendPlainEmail(aServerEntry);
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

//---------------------------------------------------------------------------------
CImSendPlainEmail* CImSendPlainEmail::NewL(CMsvServerEntry& aServerEntry)
//---------------------------------------------------------------------------------
	{
	CImSendPlainEmail* self = CImSendPlainEmail::NewLC(aServerEntry);
	CleanupStack::Pop( );
	return self;
	}

//---------------------------------------------------------------------------------
CImSendPlainEmail::CImSendPlainEmail(CMsvServerEntry& aServerEntry) : CImSendEmail(aServerEntry)
//---------------------------------------------------------------------------------
	{
	}

// Sending header followed by body and attachment parts, in the order in which they are
// arranged in the TMsventry structure.
//---------------------------------------------------------------------------------
TInt CImSendPlainEmail::NextLineL( TDes8& rOutputLine, TInt& rPaddingCount )
//---------------------------------------------------------------------------------
	{
	__ASSERT_DEBUG( iRfc822Header!=NULL, gPanic(KPanicNoRfc822Header) );
	__ASSERT_DEBUG( iSendRichText!=NULL, gPanic(KPanicNoRichText) );
	
	TInt advance = 0;
	TInt length = 0;
	TInt localPaddingCount=0;
	
	rOutputLine.Zero();
	do 
		{
		switch (iState)
			{
			case ERfc822Header:
				advance = iRfc822Header->NextLineL(rOutputLine, rPaddingCount);
				if (advance)
					AppendCRLF(rOutputLine, localPaddingCount);
				break;
			case EBody:
				advance = iSendRichText->NextLineL(rOutputLine, rPaddingCount);
				break;
			case EAttachmentFile:
				advance = iSendFile->NextLineL(rOutputLine, rPaddingCount);
				break;
			}

		length = rOutputLine.Length();
		if ( !length || advance )
			{
			if ( iState==EBody )
				AppendCRLF(rOutputLine, localPaddingCount);
			while (!ChangeStateL())
				;
			}

		} while ( !length && iState != KImCvFinished);
		
	rPaddingCount = localPaddingCount;
	return (iState==KImCvFinished ? KImCvFinished : KErrNone);
	}


//---------------------------------------------------------------------------------
TBool CImSendPlainEmail::ChangeStateL()
//---------------------------------------------------------------------------------
	{
	TMsvEmailEntry entry = iEmailTraverser->ThisEntry();

	if ( iEmailTraverser->DownLevelL() )
		entry = iEmailTraverser->ThisEntry();
	else
		while ( !iEmailTraverser->NextEntryL(entry) )
			{
			if ( !iEmailTraverser->UpLevelL() || iEmailTraverser->IsBaseLevel() )
				{
				iState=KImCvFinished;
				return ETrue;
				}
			}

	// Is this entry an attachment entry?
	if (InitBodyObjectL(entry))
		iState=EBody;
	else if (InitAttachmentObjectL(entry))
		iState=EAttachmentFile;	
	else
		return EFalse;
		
	return ETrue;
	}


//*********************************************************************************
//              Class CImSendMimeEmail Functions
//*********************************************************************************

//---------------------------------------------------------------------------------
CImSendMimeEmail* CImSendMimeEmail::NewLC(CMsvServerEntry& aServerEntry )
//---------------------------------------------------------------------------------
	{
	CImSendMimeEmail* self = new (ELeave) CImSendMimeEmail(aServerEntry);
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

//---------------------------------------------------------------------------------
CImSendMimeEmail* CImSendMimeEmail::NewL(CMsvServerEntry& aServerEntry )
//---------------------------------------------------------------------------------
	{
	CImSendMimeEmail* self = CImSendMimeEmail::NewLC(aServerEntry);
	CleanupStack::Pop( );
	return self;
	}

//---------------------------------------------------------------------------------
CImSendMimeEmail::CImSendMimeEmail(CMsvServerEntry& aServerEntry ) : CImSendEmail(aServerEntry)
//---------------------------------------------------------------------------------
	{
	}

//---------------------------------------------------------------------------------
CImSendMimeEmail::~CImSendMimeEmail()
//---------------------------------------------------------------------------------
	{
	Reset();
	delete iSendMimeHeader;
	delete iBoundaryArray;
	delete iMimeHeader;
	}

//---------------------------------------------------------------------------------
void CImSendMimeEmail::ConstructL()
//---------------------------------------------------------------------------------
	{
	TTime theTime;
	theTime.UniversalTime(); // Gets the current universal time to use as a random number seed
	iRandSeed = theTime.Int64();

	iBoundaryArray = new (ELeave) CDesC8ArrayFlat(KArrayGranularity);
	CImSendEmail::ConstructL();
	}

//---------------------------------------------------------------------------------
void CImSendMimeEmail::Reset()
//---------------------------------------------------------------------------------
	{
	iTextDisplayed=EFalse;
	iBoundaryArray->Reset();
	CImSendEmail::Reset();
	}


// Sending a MIME message consisting of a header, main MIME header followed by one or more
// MIME parts. If more than one, the parts are seperated by a boundary string.

//---------------------------------------------------------------------------------
TInt CImSendMimeEmail::NextLineL( TDes8& rOutputLine, TInt& rPaddingCount )
//---------------------------------------------------------------------------------
	{	
	__ASSERT_DEBUG( iRfc822Header!=NULL, gPanic(KPanicNoRfc822Header) );
	__ASSERT_DEBUG( iSendRichText!=NULL, gPanic(KPanicNoRichText) );

	TInt advance = 0;
	TInt length = 0;
	TBool addBlankLine=EFalse;

	rOutputLine.Zero();
	do 
	{
	addBlankLine=EFalse;
	switch (iState)
		{
		case ERfc822Header:
			advance = iRfc822Header->NextLineL( rOutputLine, rPaddingCount );
			break;
		case EMimeMainHeader:
			advance = iSendMimeHeader->NextLine( rOutputLine, rPaddingCount );
			break;
		case EMimeText:
			if ( iBoundaryArray->Count() && !iTextDisplayed )
				{
				rOutputLine.Append(KImcvMimeText);
				rPaddingCount=rOutputLine.Length();
				AppendCRLF(rOutputLine, rPaddingCount);
				addBlankLine=ETrue;
				iTextDisplayed=ETrue;
				}
			advance=1;
			break;
		case EBoundary:
			if ( iBoundaryArray->Count() )
				{
				SendBoundaryLine( rOutputLine, rPaddingCount );
				AppendCRLF(rOutputLine, rPaddingCount);
				}
			advance = 1;
			break;
		case EMimePartHeader:
			advance = iSendMimeHeader->NextLine( rOutputLine, rPaddingCount );
			break;
		case EBody:
			advance = iSendRichText->NextLineL(rOutputLine, rPaddingCount);
			break;
		case EAttachmentFile:
			advance = iSendFile->NextLineL(rOutputLine, rPaddingCount);
			break;
		case EEndBoundary:
			if ( iBoundaryArray->Count() )
				{
				SendBoundaryLine( rOutputLine, rPaddingCount );
				rOutputLine.Append(KImcvMimeBoundaryStartEnd);
				DeleteBoundary();
				rPaddingCount=rOutputLine.Length();
				}
			advance = 1;
			break;
		case ELineAfterEndBoundary:
			AppendCRLF(rOutputLine, rPaddingCount);
			advance = 1;
			break;
			}
			
		length = rOutputLine.Length();

		if ( !length || advance )
			ChangeStateL();

		if (addBlankLine)
			AppendCRLF(rOutputLine, rPaddingCount);

		} while ( !length && iState != KImCvFinished);

	return (iState==KImCvFinished ? KImCvFinished : KErrNone);
	}


//---------------------------------------------------------------------------------
TBool CImSendMimeEmail::ChangeStateL()
//---------------------------------------------------------------------------------
	{
	TInt ret=ETrue;
	TMsvEmailEntry entry = iEmailTraverser->ThisEntry();
	TImEmailFolderType folderType;

	switch (iState)
		{
		case ERfc822Header:
			InitSendMimeHeaderL(entry, EMainMimeHeader);
			iState=EMimeMainHeader;
			break;
		case EMimeMainHeader:
			iState=EMimeText;
			break;
		case EMimeText:
			if ( !CheckForMultipartEmailL(entry, folderType) )
				{
				// NOT a multipart email.
				// Get next entry and see what else it could be..
				GetNextEntryL(entry);

				if (iState!=EBoundary)
					break; // Simple MIME message, only one part.

				if ( InitBodyObjectL(entry) )
					iState=EBody;	// Rich text
				else if ( iEmbeddedEmail )
					// An embeddef RFC822 message, change state to RGC822 header
					InitEmbeddedEmailL(); 
				else if ( InitAttachmentObjectL(entry) )
					iState=EAttachmentFile; // Attachment part
				else
					GetNextEntryL(entry);
				}
			else
				{
				// A Multipart message, get next entry to see what first part is.
				GetNextEntryL(entry);

				// An multipart inside a multipart.
				if ( iState==EBoundary && entry.iType==KUidMsvFolderEntry )
					GetNextEntryL(entry); 
				}
			break;
		case EBoundary:
			iEmbeddedEmail = CheckForEmbeddedEmailL(entry);
			InitSendMimeHeaderL(entry, EPartMimeHeader);
			iState=EMimePartHeader;
			break;	
		case EMimePartHeader:
			if ( entry.iType==KUidMsvFolderEntry )
				GetNextEntryL(entry);
			else if ( InitBodyObjectL(entry) )
				iState=EBody;
			else if ( iEmbeddedEmail )
				InitEmbeddedEmailL();
			else if ( InitAttachmentObjectL(entry) )
				iState=EAttachmentFile;
			else
				GetNextEntryL(entry);
			break;
		case EBody:
		case EAttachmentFile:
			GetNextEntryL(entry);
			break;
		case EEndBoundary:
			iState=ELineAfterEndBoundary;
			break;
		case ELineAfterEndBoundary:
			GetNextEntryL(entry);
			break;
		} // end Switch	

	return ret;
	}


// If mimeheader in store, crates relevant mimeHeader object.
//---------------------------------------------------------------------------------
TBool CImSendMimeEmail::CreateMimeHeaderL()
//---------------------------------------------------------------------------------
	{
	TBool restored=EFalse;
	delete iMimeHeader;
	iMimeHeader=NULL;
	iMimeHeader = CImMimeHeader::NewL();

	if (iServerEntry.HasStoreL())
		{
		CMsvStore* entryStore = iServerEntry.ReadStoreL();
		CleanupStack::PushL(entryStore);
		if( entryStore->IsPresentL(KUidMsgFileMimeHeader) )
			{
			iMimeHeader->RestoreL(*entryStore);
			restored=ETrue;
			}
		else
		/* The MIME header store does not exist, but MIME header info will be
		 * sent with this email, so the Sent message should reflect this.
		 * So we create a KUidMsgFileMimeHeader in the store */
			{
			// open store for edit
			CleanupStack::PopAndDestroy(entryStore);
			entryStore = NULL;
			entryStore = iServerEntry.EditStoreL();
			CleanupStack::PushL(entryStore);
			
			// store the MIMEheader
			iMimeHeader->StoreL(*entryStore);
			entryStore->CommitL();
			}

		CleanupStack::PopAndDestroy(entryStore);
		}
	return restored;
	}

//---------------------------------------------------------------------------------
void CImSendMimeEmail::InitMimeHeaderL( TMsvEntry& aEntry, TPtrC8& rBoundary, TPtrC& rFilename)
//---------------------------------------------------------------------------------
	{
	TBool multipart=EFalse;
	
	TImEmailFolderType folderType;
	if (!iEmbeddedEmail && CheckForMultipartEmailL(aEntry, folderType))
		{
		//iMimeHeader->Reset();
		iMimeHeader->SetContentTypeL(KImcvMultipart);
		switch(folderType)
			{
			case EFolderTypeRelated:
				iMimeHeader->SetContentSubTypeL(KImcvRelated);
				break;
			case EFolderTypeMixed:
				iMimeHeader->SetContentSubTypeL(KImcvMixed);
				break;
			case EFolderTypeParallel:
				iMimeHeader->SetContentSubTypeL(KImcvParallel);
				break;
			case EFolderTypeAlternative:
				iMimeHeader->SetContentSubTypeL(KImcvAlternative);
				break;
			case EFolderTypeDigest:
				iMimeHeader->SetContentSubTypeL(KImcvDigest);
				break;
			default:
				// Should really default to mixed rather than unknown.
				iMimeHeader->SetContentSubTypeL(KImcvMixed);		
			}
		multipart=ETrue;
		}

	rBoundary.Set(SetBoundaryL(multipart));
	
	//Storing header information(ContentType and SubContentType) for e-mail with attachment
	if ((aEntry.Attachment()) && (iMimeHeader->ContentType().Length() == 0) && (iServerEntry.HasStoreL()))
		{
		CMsvStore* store = iServerEntry.ReadStoreL(); 
		CleanupStack::PushL(store);
		MMsvAttachmentManager& manager=store->AttachmentManagerL();
		if(manager.AttachmentCount())
			{
			CMsvAttachment* attachment = manager.GetAttachmentInfoL(0);
			CleanupStack::PushL(attachment);
			TInt length = attachment->MimeType().Length();
			TInt location = attachment->MimeType().Locate('/');
			if ((length > 0) && (location != KErrNotFound))
				{
				iMimeHeader->SetContentTypeL(attachment->MimeType().Left(location++));
				iMimeHeader->SetContentSubTypeL(attachment->MimeType().Right((length-location)));	
				}
			CleanupStack::PopAndDestroy(attachment);	
			}
		CleanupStack::PopAndDestroy(store);	
		}	

	if(InitAttachmentObjectL(aEntry))
		rFilename.Set(static_cast<TPtrC>(iSendFile->Filename()));
	else
		rFilename.Set(KNullDesC);	
	}

//---------------------------------------------------------------------------------
void CImSendMimeEmail::InitSendMimeHeaderL( TMsvEntry& aEntry, TMimeHeaderType aMimeHeaderType )
//---------------------------------------------------------------------------------
	{
	// When we are dealing with the main Mime header and we don't have a 
	// multipart we want to check if the entry is a text entry and if it 
	// has a Mime header in its store: this means we are probably dealing 
	// with a scheduling message (i.e.meeting invite). In this case we take
	// the Mime header from the text entry.

	TImEmailFolderType folderType;
	TBool found = EFalse;

	if ( (aMimeHeaderType==EMainMimeHeader) && !CheckForMultipartEmailL(aEntry, folderType) )
		{
		if ( Down(aEntry) )
			{
			if ( aEntry.iType==KUidMsvEmailTextEntry )
				{
				if ( CreateMimeHeaderL() )
					{
					found = ETrue;
					}
				}

			// We move back to the previous entry
			iEmailTraverser->UpLevelL();
			aEntry = iEmailTraverser->ThisEntry();
			}
		}

	// We read the Mimeheader stream if we haven't done it already
	if ( !found )
		{
		CreateMimeHeaderL();		
		}
	
	TPtrC8 boundary;  
	TPtrC filename;

	InitMimeHeaderL(aEntry, boundary, filename);

	delete iSendMimeHeader;
	iSendMimeHeader = NULL;
 	iSendMimeHeader = CImSendMimeHeader::NewL(*iHeaderConverter, aMimeHeaderType);	
 	if (( aEntry.iType == KUidMsvMessageEntry )
 		&& ( !iEmailTraverser->IsBaseLevel() )
 		&& ( aMimeHeaderType == EPartMimeHeader ))
 		{
 		iSendMimeHeader->InitialiseL( *iMimeHeader, filename, boundary,iEmbeddedEmail, 
 										aEntry, iRfc822Header->TransformingInfo(), ETrue);
 		}
 	else
 		{
 		iSendMimeHeader->InitialiseL( *iMimeHeader, filename, boundary,iEmbeddedEmail, 
   										aEntry, iRfc822Header->TransformingInfo());
 		}

	}


//---------------------------------------------------------------------------------
void CImSendMimeEmail::InitEmbeddedEmailL()
//---------------------------------------------------------------------------------
	{
	iRfc822Header->InitialiseL(iServerEntry.Entry().iDate, iDomainName, iServerEntry,
					iServerEntry.Entry().Priority(), ESendNoCopy, iDefaultTransformingInfo);

	iState=ERfc822Header;
	iEmbeddedEmail=0;
	}

//---------------------------------------------------------------------------------
void CImSendMimeEmail::CreateBoundaryL()
//---------------------------------------------------------------------------------
	{
	TBuf8<57> validBoundaryBuffer(KBasicAsciiChars);
	TBuf8<KBoundaryStringLength> boundaryString; 
	TUint result = 0;

	boundaryString.Append(KImcvEpoc32); 
	boundaryString.Append(KImcvHyphen); 

	while( boundaryString.Length()<KBoundaryStringLength )
		{
		result=(Math::Rand(iRandSeed));
		// gets the mod of Rand() which is a number between 0 to KMaxTInt
		result%=57;
		boundaryString.AppendFormat(KPrintChar, validBoundaryBuffer[result]);
		result=0;
		}

	iBoundaryArray->InsertL(0, boundaryString);
	}

//---------------------------------------------------------------------------------
void CImSendMimeEmail::DeleteBoundary()
//---------------------------------------------------------------------------------
	{
	if (iBoundaryArray->Count())
		iBoundaryArray->Delete(0);
	}

//---------------------------------------------------------------------------------
TInt CImSendMimeEmail::HeaderSize()
//---------------------------------------------------------------------------------
	{
	TInt size = CImSendEmail::HeaderSize();

	TMsvId id = iServerEntry.Entry().Id();
	iEmailTraverser->SetBaseEntry();

	TMsvEntry aId;

	// DEF023106. Can't let Leaves pass into public non-leave function, so Trap.
	TRAPD(initialiseError, InitSendMimeHeaderL(aId, EMainMimeHeader));

	if (initialiseError != KErrNone)
		{
		// DEF023106. Return double the rfc822 header size on Leave.
		size += size;
		}
	else
		{
		size +=  iSendMimeHeader->Size();
		}

	iServerEntry.SetEntry(id);
	return size;
	}


//---------------------------------------------------------------------------------
TBool CImSendMimeEmail::InitBodyObjectL(TMsvEntry& rEntry)
//---------------------------------------------------------------------------------
	{
	TImEncodingType type=iRfc822Header->TransformingInfo().BodyTextEncoding();

	return ( rEntry.iType==KUidMsvEmailTextEntry
		  && iSendRichText->InitialiseL( iServerEntry, 
							Encoder(type), type,
							iSendMimeHeader->CharsetUid()) );
	}


//*********************************************************************************
//              Class CImSendRfc822Header Functions
//*********************************************************************************

//---------------------------------------------------------------------------------
CImSendRfc822Header* CImSendRfc822Header::NewLC( RFs& anFs, CImConvertHeader& aConverter )
//---------------------------------------------------------------------------------
	{
	CImSendRfc822Header* self = new (ELeave) CImSendRfc822Header(anFs, aConverter);
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

//---------------------------------------------------------------------------------
CImSendRfc822Header* CImSendRfc822Header::NewL( RFs& anFs, CImConvertHeader& aConverter )
//---------------------------------------------------------------------------------
	{
	CImSendRfc822Header* self = CImSendRfc822Header::NewLC(anFs, aConverter);
	CleanupStack::Pop( );
	return self;
	}

//---------------------------------------------------------------------------------
CImSendRfc822Header::CImSendRfc822Header( RFs& anFs, CImConvertHeader& aConverter ) 
				: iFs(anFs), iHeaderConverter(aConverter)
//---------------------------------------------------------------------------------
	{
	}

//---------------------------------------------------------------------------------
CImSendRfc822Header::~CImSendRfc822Header() 
//---------------------------------------------------------------------------------
	{
	delete iHeader;
	delete iDomainName;

	delete iOutputBuffer;
	delete iProductName;
	delete iImcvUtils;
	delete iPriorityFields;
	delete iImportanceFields;
	delete iReceiptFields;
	}

//---------------------------------------------------------------------------------
void CImSendRfc822Header::ConstructL()
//---------------------------------------------------------------------------------
	{
	RResourceFile resFile;
	OpenResourceFileL(resFile,iFs);	// leaves if file not found

	// make sure the resource file will be closed if anything goes wrong
	TCleanupItem close( CloseResourceFile, &resFile );
	CleanupStack::PushL( close );
	HBufC8* buf = resFile.AllocReadLC( PRODUCT_NAME );

	TResourceReader reader;
	reader.SetBuffer(buf);
	iProductName = reader.ReadTPtrC().AllocL();
	CleanupStack::PopAndDestroy(2); // close , buf
	resFile.Close();

	}

//---------------------------------------------------------------------------------
TBool CImSendRfc822Header::InitialiseL( const TTime aTimeDate , const TDesC& aDomainName,
							CMsvServerEntry& aServerEntry,const TMsvPriority aPriority,
							const TImSMTPSendCopyToSelf aCopyToSelf,
							TImEmailTransformingInfo& aInfo)
//---------------------------------------------------------------------------------
	{
	Reset();
	iTimeDate = aTimeDate;

	delete iDomainName;
	iDomainName=NULL;
	iDomainName=aDomainName.AllocL();

	delete iHeader;
	iHeader=NULL;
	iHeader = CImHeader::NewLC();
	CleanupStack::Pop(); // header

	delete iImcvUtils;
	iImcvUtils=NULL;
	iImcvUtils=CImcvUtils::NewL();
	
	delete iPriorityFields;
	iPriorityFields=NULL;
	delete iImportanceFields;
	iImportanceFields=NULL;
	delete iReceiptFields;
	iReceiptFields=NULL;
	
	iPriorityFields=new (ELeave)CDesC8ArrayFlat(KArrayAllocationNumber);
	iImcvUtils->SendPriorityFieldsL(*iPriorityFields);
	
	iImportanceFields=new (ELeave)CDesC8ArrayFlat(KArrayAllocationNumber);
	iImcvUtils->SendImportanceFieldsL(*iImportanceFields);

	TMsvEmailEntry entry = aServerEntry.Entry();
	if (entry.Receipt())
		{
		iReceiptFields = new (ELeave)CDesC8ArrayFlat(KArrayAllocationNumber);
		iImcvUtils->SendReturnReceiptFieldsL(*iReceiptFields);
		}
	iPriority = aPriority;

	TBool validEmailMsg=EFalse;
	if (aServerEntry.HasStoreL())
		{
		CMsvStore* store = aServerEntry.ReadStoreL();
		CleanupStack::PushL(store);

		if (store->IsPresentL( KUidMsgFileTransformingInfo))
			aInfo.RestoreL(*store);
		
		// Must have an rfc822 header.
		if (store->IsPresentL( KUidMsgFileIMailHeader) )
			{
			iHeader->RestoreL(*store);
			validEmailMsg=ETrue;
			}

		CleanupStack::PopAndDestroy(store); 
		}
	
	iTransformingInfo=aInfo;
	if(aInfo.HeaderCharset() == 0)
		aInfo.SetHeaderCharset(iHeaderConverter.CharConv().DefaultCharset());

	iHeaderCharset=aInfo.HeaderCharset();
	iHeaderEncoding=aInfo.HeaderEncoding();
	
	iCopyToSelf=aCopyToSelf;	

	return validEmailMsg;
	}

//---------------------------------------------------------------------------------
void CImSendRfc822Header::HandleSpecialCharacters(TPtr8& aAddressPtr)
//---------------------------------------------------------------------------------
	{
	// Single address passed in. Make sure the address is of the format
	// "quoted name" <name@domain>
	// quoted, name <name@domain>
	// quoted "name" <name@domain>
	// name <name@domain>
	// quoted "name <name@domain>
	// Comments are not supported.
	// Remove any leading and trailing white space.
	aAddressPtr.Trim();
	
	// If the address isn't the correct format.
	if (aAddressPtr[aAddressPtr.Length() - 1] != '>')
		return;

	TInt leftChevron = aAddressPtr.LocateReverse(TChar('<'));
	if (leftChevron == KErrNotFound)
		return;

	//If the address has allias
	if(leftChevron > 0)
		{
			
		TInt  firstDQuote = aAddressPtr.Locate(TChar('"'));
		TInt lastDQuote = aAddressPtr.Left(leftChevron).LocateReverse(TChar('"'));
		TInt aliasLastChar = leftChevron-1;
		
		//If alias has ',' in between
		if(aAddressPtr.Left(leftChevron).LocateReverse(TChar(',')) != KErrNotFound)
			{
			//get the last character of alias
			for (TInt index = aliasLastChar ; aAddressPtr[index] == ' ' ; --index )
				{
				aliasLastChar--;	
				}
			//if alias not with in quotes then add quotes
			if ( firstDQuote != 0 || lastDQuote != aliasLastChar)
				{
				aAddressPtr.Insert(0,KImcvQuoteString);	
				firstDQuote = 0;
				//because alias length has been increased by one character 
				aliasLastChar+=1;
				aAddressPtr.Insert(aliasLastChar+1 , KImcvQuoteString);
				lastDQuote = aliasLastChar+1;
				}
			}	
		if (lastDQuote == KErrNotFound || lastDQuote == firstDQuote)
		 		 return;
		
		//add 	KBackslash before all spacial characters in alias
		_LIT8(KBackslash,"\\");
		for (TInt index = lastDQuote - 1; index > firstDQuote; --index)
			{
			if (IsSpecialCharacter(aAddressPtr[index]))
				{
				// Character inserted before the current character.
				aAddressPtr.Insert(index, KBackslash);
				}
			}	
		}
	}

//---------------------------------------------------------------------------------
TBool CImSendRfc822Header::IsSpecialCharacter(const TUint aChar)
//---------------------------------------------------------------------------------
	{
	return (aChar == '(' || aChar == ')' || aChar == '<' || aChar == '>' || aChar == '@' 
		 || aChar == '"' || aChar == ';' || aChar == ':' || aChar == '/' || aChar == ']' 
		 || aChar == '[');
	}

//---------------------------------------------------------------------------------
void CImSendRfc822Header::Reset()
//---------------------------------------------------------------------------------
	{
	iState = 0;
	}

//---------------------------------------------------------------------------------
TInt CImSendRfc822Header::NextLineL( TDes8& rOutputLine, TInt& rPaddingCount )
//---------------------------------------------------------------------------------
	{
	// Format the next line of output into rOutputLine.
	// If the output line contains more characters than the size of the source data consumed 
	// in preparing it, then the difference is passed back in aPaddingCount.

	__ASSERT_DEBUG( iHeader!=NULL, gPanic(KPanicNoRfc822Header) );
	
	TInt advance = 0;
	TInt length = 0;
	TInt localPaddingCount=0;

	rOutputLine.Zero();
	do 
		{
		switch (iState)
			{
			case EFrom: // top of header, generate 'From'
				advance = FromL( rOutputLine ); // returns 1 if we've finished the field, 0 otherwise
				break;
			case EReplyTo: // generate 'Reply-To'
				advance = ReplyToL( rOutputLine );
				break;
			case ETo: // generate 'To'
				advance = ToL( rOutputLine );
				break;
			case ECc: // generate 'Cc'
				advance = CcL( rOutputLine  );
				break;
			case EBcc: // generate 'Bcc'
				advance = BccL( rOutputLine );
				break;
			case ESubject: // generate 'Subject'
				advance = SubjectL( rOutputLine );
				break;
			case EDate: // generate date
				rOutputLine = KImcvDatePrompt;
				iRfc822Date.SetDate( iTimeDate, rOutputLine );
				advance = 1;
				break;
			case EReturnReceiptTo:
				if (iReceiptFields)
					advance = ReturnReceiptsL( rOutputLine );
				break;
			case EPriority:
				advance = PriorityL(rOutputLine );
				break;
			case EImportance:
				advance = ImportanceL(rOutputLine );
				break;
			case EMessageID:  // iDomainName should be called somewhere before this is called!
				// (use SetDomainNameL(aDomainName))
				SetMessageIdL();
				advance = MessageIdL(rOutputLine);
				break;
			case EXMailer: // quick advert for ourselves
				advance = XMailerL( rOutputLine );
				break;
			}

		length = rOutputLine.Length();
		if (!length || advance)
			iState ++;

		} while ( !length && (iState < EEndOfRfc822Header) );

	if ( !IsLineCRLF( rOutputLine ) )
		// only write something if there is any data!
		AppendCRLF(rOutputLine, localPaddingCount);
		
	rPaddingCount = localPaddingCount;
	return (iState>=EEndOfRfc822Header ? KImCvAdvance : KErrNone);
	}

// Pass in 16 bit descriptor, encode before outputting

//---------------------------------------------------------------------------------
void CImSendRfc822Header::PrepareBufferL( const TPtrC8& aPrompt, const TDesC& aData)
//---------------------------------------------------------------------------------
	{
	HBufC8* buf = HBufC8::NewL(aData.Length());
	RBuf8 bufPtr(buf);
	bufPtr.CleanupClosePushL();
	EncodeHeaderFieldL(aData, bufPtr, 0);
	PrepareBufferL(aPrompt,bufPtr);
	CleanupStack::PopAndDestroy(&bufPtr);

	}

// data is 8 bit, no encoding.

//---------------------------------------------------------------------------------
void CImSendRfc822Header::PrepareBufferL( const TPtrC8& aPrompt, const TDesC8& aData)
//---------------------------------------------------------------------------------
	{
	if (iOutputBuffer)
		return;

	iOutputBuffer = HBufC8::NewL( aPrompt.Length() + aData.Length() + 1 );
	*iOutputBuffer = aPrompt;

	// need to buffer up some output
	iOutputBuffer->Des().Append( KImcvSpace );
	iOutputBuffer->Des().Append( aData );
	ipOutputBuffer = iOutputBuffer->Ptr();
	}


//---------------------------------------------------------------------------------
void CImSendRfc822Header::AddCopyToSelfL(TImSMTPSendCopyToSelf aCopyToSelf, CDesCArray& aList)
//---------------------------------------------------------------------------------
	{
	if (iCopyToSelf==aCopyToSelf)
		{
		if ( iHeader->ReceiptAddress().Length() )
			aList.AppendL(iHeader->ReceiptAddress());
		else if ( iHeader->ReplyTo().Length() )
			aList.AppendL(iHeader->ReplyTo());
		}
	}


//---------------------------------------------------------------------------------
TInt CImSendRfc822Header::DoRecipientsL( TDes8& rOutputLine, const TPtrC8& aPrompt, 
										 CDesCArray& aList)
//---------------------------------------------------------------------------------
	{
	if (iOutputBuffer)
		return SendOutput(rOutputLine);

	// first call into this function, so build the output buffer
	// first establish the length (this is faster than guessing & having to re-allocate)
	TInt32 totalLength = aPrompt.Length()+1;
	TInt i=aList.Count(); // loop counter
	while (i--)
		// allow 2 chars for comma-separation
		totalLength += aList[i].Length() + 2;

	iOutputBuffer = HBufC8::NewL( totalLength );
	// now build the combined list
	TInt count=aList.Count();
	for ( i=0; i<count; i++ )
		{
		if (!i) // first recipient in list needs to be preceeded by the field identifier
			{
			*iOutputBuffer = aPrompt;
			iOutputBuffer->Des().Append( KImcvSpace );
			}
		else // subsequent ones need comma-separation
			iOutputBuffer->Des().Append( KImcvCommaSpace );
		// ...and of course the data
		HBufC8* buf = HBufC8::NewL((aList[i]).Length());
		RBuf8 bufPtr(buf);
		bufPtr.CleanupClosePushL();

		EncodeHeaderFieldL(aList[i] , bufPtr, i);

		HBufC8* addrBuffer = HBufC8::NewLC(bufPtr.Length() * 2);
		TPtr8 addrPtr(addrBuffer->Des());
		addrPtr.Copy(bufPtr);
		HandleSpecialCharacters(addrPtr);

		// May have to increase size of iOutputBuf
		if (iOutputBuffer->Des().MaxLength() < iOutputBuffer->Des().Length() + addrPtr.Length() + 2)
				iOutputBuffer = iOutputBuffer->ReAllocL( iOutputBuffer->Des().MaxLength() + addrPtr.Length());
		iOutputBuffer->Des().Append( addrPtr);
		bufPtr.Close();
		CleanupStack::PopAndDestroy(2, &bufPtr); // addrBuffer, buf

		} // for (...)
	ipOutputBuffer = iOutputBuffer->Ptr();
	return SendOutput( rOutputLine );
	}

void CImSendRfc822Header::PackRemainingData( TDes8& rOutputLine, TPtrC8& aBuf )
	{

	if (iAddSpaceToNewLine)
		{
		// wrapped lines start with a space to indicate their wrappiness.
		rOutputLine = KImcvSpace;
		rOutputLine.Append( aBuf.Ptr(), aBuf.Length() );
		// we can add this extra byte since MKaxIMailLineLength allows for it 
		rOutputLine.SetLength( aBuf.Length() + 1 );
		}
	else
		{
		rOutputLine = aBuf;
		}
	}


//---------------------------------------------------------------------------------
TInt CImSendRfc822Header::SendOutput( TDes8& rOutputLine )
//---------------------------------------------------------------------------------
	{
	// we have buffered output - first check whether we can bung the rest out on one line.
	// the size of the remaining data is the whole length less the distance we've already got through

	TInt offset = (ipOutputBuffer-iOutputBuffer->Ptr());

	TInt dataRemaining = iOutputBuffer->Length() - offset;
	if ( dataRemaining < KMaxIMailHeaderWriteLineLength )
		{
		// copy this into the line-output buffer
		if ( !offset )
			{
			// this is the first line for this field since we're still pointing at the start of the buffer
			rOutputLine = *iOutputBuffer;
			}
		else
			{
			TPtrC8 data( ipOutputBuffer, dataRemaining );
			PackRemainingData( rOutputLine, data);
			}

		delete iOutputBuffer;
		iOutputBuffer 		= NULL;
		ipOutputBuffer 		= NULL;
		iAddSpaceToNewLine 	= EFalse;
		// we can now advance to the next field
		return 1;
		}
	else // we need to send out the data in line-long segments
		{
		// only the first line gets the whole length, cos subsequent ones are prepended with a space.
		TInt lineLength = (offset ? KMaxIMailHeaderWriteLineLength : KMaxIMailHeaderWriteLineLength-1 );

		TInt pos 	= 0;
		TInt left 	= 0;
		TInt right 	= 0;
		TBool splitAddress = EFalse;
		iAddSpaceToNewLine = EFalse;

		// if currently handling subject field, then do not attempt to parse for email addresses.
		// This causes encoded words to be split accross line breaks, resulting in garbled text. 
		if (FieldIsEmailAddress())
			{
			TPtrC8 addrData(ipOutputBuffer, dataRemaining);
			while ( ValidEmailAddress( addrData, pos, left, right) )
				{
				if (left+pos < lineLength)
					{
					if (right+pos >= lineLength)
						{
						// Current Email address won't fit into line
						splitAddress = ETrue;
						iAddSpaceToNewLine = ETrue;
						
						if (right - left < lineLength)
							{
							// Break line just before Email address
							lineLength = left+pos;
							break;
							}	
						}
					}
				else
					{
					//length of alias is less than KMaxIMailHeaderWriteLineLength
					 if(left+pos < KMaxIMailHeaderLineLength)
						{
						splitAddress = ETrue;
						iAddSpaceToNewLine = ETrue;
						// Break line just before Email address
						lineLength = left+pos;
						}
					else
						{
						User::Leave(KErrSmtpBufferOverFlow);
						}
					break;
					 }		
			    if (right+pos > lineLength)
					{
					//length of email address is less than KMaxIMailHeaderWriteLineLength
					if(right+pos < KMaxIMailHeaderLineLength)
						{
						splitAddress = ETrue;
						iAddSpaceToNewLine = ETrue;
						// Break line just before Email address
						lineLength = right+pos;	
						}
					else
						{
						User::Leave(KErrSmtpBufferOverFlow);	
						}
					break;
					}	
				// Move pos to end of Email address
				pos += right;
				}
			}
					
		if ( !splitAddress )
			{
			TPtrC8 encData(ipOutputBuffer, dataRemaining);			
			while ( iHeaderConverter.FindEncodedWord(encData, pos, left, right) )
				{
				if (left+pos < lineLength)
					{
					// Second part of condition checks for too long encoded-words 
					if ((right+pos >= lineLength) && (right - left < lineLength))
						{
						// Break line just before encoded word
						lineLength = left+pos;
						iAddSpaceToNewLine = ETrue;
						break;
						}
					}
				else if (right+pos > lineLength)
					break;
				// Move pos to end of the encoded word
				pos += right;
				}
			}


		if ( !FieldIsEmailAddress() && !iAddSpaceToNewLine )
			{
				// We have a subject field and no wrapping set at an Encoded Word
				_LIT(KInvalidDataSize, "Data Remaining, not suitable for more than a line");
				__ASSERT_DEBUG( dataRemaining >= KMaxIMailHeaderWriteLineLength, User::Panic(KInvalidDataSize, 1 ));
				
				// Find space from a resonable length onwards in ascending order
				TInt minLineLength = KMaxIMailHeaderWriteLineLength/2;
				TPtrC8 buf(ipOutputBuffer+minLineLength, KMaxIMailHeaderWriteLineLength);
				lineLength = buf.Locate(' ');
				
				if ( lineLength != KErrNotFound )
					{
					// We have found a space
					lineLength += minLineLength;
					// only the first line gets the whole length, cos subsequent ones are prepended with a space.
					TInt maxLineLength = (offset ? KMaxIMailHeaderWriteLineLength : KMaxIMailHeaderWriteLineLength-1 );
					if (left+pos < maxLineLength)
					    {
					    // Second part of condition checks for too long encoded-words 
					    if ((right+pos >= maxLineLength) && pos>0)
					        {
					        rOutputLine = KImcvSpace;
					        }
					    }
					rOutputLine.Append( ipOutputBuffer, lineLength );
					ipOutputBuffer += lineLength;
					}
				else
					{
					// Line length limit is 1000 chars per line including CRLF (RFC2822, Section 2.1.1)
                    // 1000 chars including "Field name: "+"Field body"+CRLF (here "Resent-Message-ID: " is largest field)
					if (dataRemaining > KSmtpMaxBufferExcludingCRLF)
						User::Leave(KErrSmtpBufferOverFlow);
	
					// No space found, so send the full remaining buffer as it is
					rOutputLine.Append( ipOutputBuffer, dataRemaining );
					delete iOutputBuffer;
					iOutputBuffer = NULL;
					ipOutputBuffer = NULL;
					return 1;
					}
				
				return 0;
			}
		// create a descriptor for the next line-length of characters
		TPtrC8 data( ipOutputBuffer, lineLength );

		if ( !offset )
			{
			// this is the first line for this field
			rOutputLine = data;	
			}
		else
			{
			PackRemainingData( rOutputLine, data );
			}
		// advance the data pointer to the next line-length of data
		ipOutputBuffer += lineLength;

		// prevent ourselves advancing to the next field
		return 0;
		}
	}

// Assumption:
// data string coming in has already been formatted.
// address1@dd.com, address2@dd.com, addreass3@dd
// 
//---------------------------------------------------------------------------------
TBool CImSendRfc822Header::ValidEmailAddress(const TPtrC8& aData, const TInt aPos, 
															TInt& rStart, TInt& rEnd)
//---------------------------------------------------------------------------------
	{
	TBool found = EFalse;

	TPtrC8 data = aData.Mid(aPos);
	TInt len = data.Length();
	if (len < 3)
		return found;
	//Assuming that alias does not contain '@'
	TInt at = data.Locate(KImcvAt);
	if ( at!=KErrNotFound )
		{
		found = ETrue;

		TPtrC8 findEnd(data.Right(len-at));
		TPtrC8 findStart(data.Left(at));

		rEnd = findEnd.Locate(KImcvComma);
		rStart = findStart.LocateReverse(KImcvSpaceChar);
		} 

	rEnd = (rEnd==KErrNotFound) ? data.Length() : rEnd+at;

	if (rStart==KErrNotFound)
		rStart = 0;

	return found;
	}

//---------------------------------------------------------------------------------
void CImSendRfc822Header::SetMessageIdL()
//---------------------------------------------------------------------------------
	{
	TInt domainNameLength= iDomainName && iDomainName->Length() ? iDomainName->Length():0;

	// list of Valid Characters
	TBuf8<52> validCharacterBuffer(KValidCharacters);
	TBuf8<20> randomString;
	HBufC8* messageId = HBufC8::NewL(domainNameLength+24); 
	CleanupStack::PushL(messageId);

	TPtr8 ptr(messageId->Des());
	TUint result = 0;
	TInt64 randSeed;
	TTime theTime;

	theTime.UniversalTime(); 
	// Gets the current universal time to use as a random number seed
	randSeed = theTime.Int64();

	while(randomString.Length()<20)
		{
		result= (Math::Rand(randSeed)%10)*10; 
		result+= Math::Rand(randSeed)%10;
		result%=52;
		randomString.AppendFormat(KImcvCharacterFormat, validCharacterBuffer[result]);
		result=0;
		}

	ptr.Append(KImcvLeftChevron);
	ptr.Append(randomString.Left(12));
	ptr.Append(KImcvFullStop);
	ptr.Append(randomString.Right(8));
	if (domainNameLength)
		{
		ptr.Append(KImcvAt);
		ptr.Append(iDomainName->Des());
		}
	ptr.Append(KImcvRightChevron);
	iHeader->SetImMsgIdL(messageId->Des()); 
	
	CleanupStack::PopAndDestroy(messageId);
	}


//---------------------------------------------------------------------------------
TInt CImSendRfc822Header::PriorityL(TDes8& rOutputLine)
//---------------------------------------------------------------------------------
	{
	if (iPriority==EMsvMediumPriority)
		return 0;
	if (iPriority==EMsvHighPriority)
		PrepareBufferL((*iPriorityFields)[0], KImPrioritySendHigh);
	else if (iPriority==EMsvLowPriority)
		PrepareBufferL((*iPriorityFields)[0], KImPrioritySendLow);
	iPriorityFields->Delete(0);
	SendOutput( rOutputLine );
	
	return (iPriorityFields->Count()) ? 0:1; // Return 1 if array is empty
	}

//---------------------------------------------------------------------------------
TInt CImSendRfc822Header::ImportanceL(TDes8& rOutputLine)
//---------------------------------------------------------------------------------
	{
	// Although importance means priority, it is a different field and 
	// had a different format. When we send the importance field we send
	// it based on the priority of the e-mail hense the usage of iPriority

	if (iPriority==EMsvMediumPriority)
		return 0;
	if (iPriority==EMsvHighPriority)
		PrepareBufferL((*iImportanceFields)[0], KImImportanceSendHigh);
	else if (iPriority==EMsvLowPriority)
		PrepareBufferL((*iImportanceFields)[0], KImImportanceSendLow);
	iImportanceFields->Delete(0);
	SendOutput( rOutputLine );
	
	return (iImportanceFields->Count()) ? 0:1; // Return 1 if array is empty
	}

// calculates & returns the total size of the header info, including the RFC822 tags, 
// comma & space separation, the CRLFs and the blank line after the header itself

//---------------------------------------------------------------------------------
TInt CImSendRfc822Header::Size() const
//---------------------------------------------------------------------------------
	{
	__ASSERT_DEBUG( iHeader!=NULL, gPanic(KPanicNoRfc822Header) );

	TInt size = iHeader->DataSize();

	size+= KImcvFromPrompt().Length() + 3;
	if (iHeader->ReplyTo().
		Length()>0)	//test for existence of replyTo field 
		{
		size+= KImcvReplyToPrompt().Length() + 3;
		}
	size+= KImcvSubjectPrompt().Length() + 3;
	size+= KImcvDateStringLength + KImcvDatePrompt().Length() + 2;
	size+= KImcvXMailer().Length() + iProductName->Length() + 2;

	
	if (iHeader->ToRecipients().Length())
		size+= KImcvToPrompt().Length()+3 
						+ iHeader->ToRecipients().Count()*2;
	
	if (iHeader->CcRecipients().Length())
		size+= KImcvCcPrompt().Length()+3 
							+ iHeader->CcRecipients().Count()*2;

	if (iHeader->BccRecipients().Length())
		size+= KImcvBccPrompt().Length()+3 
							+ iHeader->BccRecipients().Count()*2;

	if (iHeader->ReceiptAddress().Length())  
		// for now there will always be one return receipt field in send.  In future if we implement more than one 
		//field then the size should include other fields. 
		size+= KImcvMsgDispositionTo().Length()+3 
							+ iHeader->ReceiptAddress().Length()+2;
	return size;
	}


//---------------------------------------------------------------------------------
TUint CImSendRfc822Header::HeaderCharset() const
//---------------------------------------------------------------------------------
	{
	return iHeader->Charset();
	}

//---------------------------------------------------------------------------------
void CImSendRfc822Header::EncodeHeaderFieldL(const TDesC& aBufIn, RBuf8& rBufOut, const TInt aArrayVal)
//---------------------------------------------------------------------------------
	{
	CArrayFix<TImHeaderEncodingInfo>* infoArray = &(iHeader->EncodingInfo());
	TImHeaderEncodingInfo::TEncodingType type;
	switch (iHeaderEncoding)
		{
		case EEncodingTypeQP: 
			type=TImHeaderEncodingInfo::EQP;
			break;
		case EEncodingTypeBASE64: 
			type=TImHeaderEncodingInfo::EBase64;
			break;
		case EEncodingTypeUU: 
			type=TImHeaderEncodingInfo::EUU;
			break;
		case EEncodingTypeNone: 
			type=TImHeaderEncodingInfo::ENoEncoding;
			break;
		default:
			__ASSERT_DEBUG( ETrue, gPanic(KPanicInvalidEncodingType) );
			type=TImHeaderEncodingInfo::ENoEncoding;
		}

	if (iTransformingInfo.SendMethod() == ESendAsSimpleEmail)
		{
		// Don't encode, just convert 
		iHeaderConverter.ConvertHeaderFieldL(aBufIn, rBufOut, FieldIsEmailAddress());
		}
	else if (iHeaderCharset && iHeaderCharset!=KUidMsvCharsetNone)
		{
		iHeaderConverter.EncodeHeaderFieldL(aBufIn, rBufOut, iHeaderCharset, type, FieldIsEmailAddress());
		}
	else if (infoArray->Count())
		{
		// Use encoding information stored from incoming message.
		iHeaderConverter.EncodeHeaderFieldL(aBufIn, rBufOut, infoArray, iState, aArrayVal);
		}
	else
		{
		// Dont encode
		rBufOut.Copy(aBufIn);
		}
	}

//*********************************************************************************
//              Class CImSendMimeHeader Functions
//*********************************************************************************

//---------------------------------------------------------------------------------
CImSendMimeHeader* CImSendMimeHeader::NewLC(CImConvertHeader& aConverter, TMimeHeaderType aType)
//---------------------------------------------------------------------------------
	{
	CImSendMimeHeader* self = new (ELeave) CImSendMimeHeader(aConverter, aType);
	CleanupStack::PushL( self );
	return self;
	}

//---------------------------------------------------------------------------------
CImSendMimeHeader* CImSendMimeHeader::NewL(CImConvertHeader& aConverter, TMimeHeaderType aType)
//---------------------------------------------------------------------------------
	{
	CImSendMimeHeader* self = CImSendMimeHeader::NewLC(aConverter, aType);
	CleanupStack::Pop();
	return self;
	}

//---------------------------------------------------------------------------------
CImSendMimeHeader::CImSendMimeHeader(CImConvertHeader& aConverter, TMimeHeaderType aType) :
					iConverter(aConverter), iBoundaryString(TPtrC8()), iMimeHeaderType(aType)
//---------------------------------------------------------------------------------
	{
	}

//---------------------------------------------------------------------------------
CImSendMimeHeader::~CImSendMimeHeader()
//---------------------------------------------------------------------------------
	{
	delete iCharsetString;
	}
				
//---------------------------------------------------------------------------------
void CImSendMimeHeader::InitialiseL( CImMimeHeader& aMimeHeader, const TPtrC aFilename, 
					const TPtrC8& aBoundary,  const TBool aEmbedded, 
					const TMsvEntry& aEntry,  const TImEmailTransformingInfo& aInfo,
					TBool aEmbeddedEmail)
//---------------------------------------------------------------------------------
	{
	iMimeHeader=&aMimeHeader;
	iBoundaryString.Set(aBoundary);
	iFilename = aFilename;
	iHeaderState=0;
	iDealingWithAnEmbeddedEmail = aEmbeddedEmail;
	iEntry=aEntry;

	TPtrC8 contentType = iMimeHeader->ContentType(); 
	iIsMultipart = (contentType.Length() && contentType==KImcvMultipart) ? ETrue:EFalse;

	// Set the EncodingType of whatever MIME part it is.
	if (aEmbedded)
		{
		// Deals with the case of a message/rfc22 which is 7 bit.
		iEncodingType=EEncodingTypeNone;
		}
	else if (aEntry.iType==KUidMsvMessageEntry && iMimeHeader->ContentType()!=KImcvMultipart)
		{
		// Special case: for non multipart HTML message.
		if ( ((TMsvEmailEntry)aEntry).MHTMLEmail() )
			{
			if ( aEntry.Attachment())
				{
				iEncodingType=aInfo.AttachmentEncoding();
				SetCharsetUidL(aInfo.HeaderCharset());
				}
			else
				{
				iEncodingType=aInfo.HTMLEncoding();
				SetCharsetUidL(aInfo.HTMLCharset());
				}
			}
		else if (contentType.CompareF(KImcvApplication)==0)
			{
			// So we know it is binary..
			iEncodingType=aInfo.AttachmentEncoding();
			SetCharsetUidL(aInfo.HeaderCharset());
			}
		else
			{
			iEncodingType=aInfo.BodyTextEncoding();
			SetCharsetUidL(aInfo.BodyTextCharset());
			}
		}
	else 
		{
		if  (aEntry.iType == KUidMsvAttachmentEntry)
			{
			iEncodingType=aInfo.AttachmentEncoding();
			SetCharsetUidL(aInfo.HeaderCharset());
			}
		else if (aEntry.iType == KUidMsvEmailHtmlEntry)
			{
			iEncodingType=aInfo.HTMLEncoding();
			SetCharsetUidL(aInfo.HTMLCharset());
			}
		else
			{
			iEncodingType=aInfo.BodyTextEncoding();
			SetCharsetUidL(aInfo.BodyTextCharset());
			}
		}

	TUint mimeCharset = iMimeHeader->MimeCharset();
	if (mimeCharset)
		SetCharsetUidL(mimeCharset); // Ovverriding the info value passed in.

	if (aEmbedded)
		{
		iMimeHeader->SetContentTypeL(KImcvMessage);
		iMimeHeader->SetContentSubTypeL(KImcvRfc822);
		}

	if (iDealingWithAnEmbeddedEmail)
		{
 		CImSendFile::EmbeddedEmailFilename(this->iEntry, this->iFilename);
 		}

	}

//---------------------------------------------------------------------------------
TInt CImSendMimeHeader::NextLine( TDes8& rOutputLine, TInt& rPaddingCount )
//---------------------------------------------------------------------------------
	{
	TInt advance = 0;
	rOutputLine.Zero();

	switch (iHeaderState)
		{
		case EMimeVersion: // Main only
			if (iMimeHeaderType==EMainMimeHeader)
				{
				advance = MimeVersion(rOutputLine);
				break;
				}
			advance++;  // else continue..
		case EContentLanguage: // Main only
			if (iMimeHeaderType==EMainMimeHeader)
				{
				advance = ContentLanguage( rOutputLine );
				break;
				}
			advance++;  // else continue..
		case EDescription: // Part only
			advance++;
			if ( Description(rOutputLine) )
				break;
			// else continue on to..
		case EContentType:
			advance++;
			if ( ContentType(rOutputLine) )
				break;
			// else continue..
		case EBoundaryString:
			if( iBoundaryString.Length() )
				{
				advance = Boundary(rOutputLine);
				break;
				}
			advance++;  // else continue..
		case EDisposition:
			if (iIsMultipart && !iDealingWithAnEmbeddedEmail)
				advance++;
			else if (Disposition(rOutputLine, advance))
				break;
			// else continue..
 		case ETransferEncoding:
			advance++; 
			if ( !iIsMultipart && TransferEncoding(rOutputLine) )
				break;
			// else continue..
		case ECRLF:
			advance += AddCRLF(rOutputLine);
			break;
		default:
			__ASSERT_DEBUG( ETrue, gPanic(EPanicInvalidHeaderState) );
		}

	iHeaderState += advance;
	rPaddingCount = rOutputLine.Length();
	return (iHeaderState >= EEndOfMimeHeader ? KImAttFinished : KErrNone);
	}


//---------------------------------------------------------------------------------
void CImSendMimeHeader::AppendFilenameL( TDes8& rOutputLine ) const
//---------------------------------------------------------------------------------
	{
    HBufC8* buf = HBufC8::NewL( iFilename.Length() );
	RBuf8 bufPtr(buf);
	CleanupClosePushL(bufPtr);
	iConverter.EncodeHeaderFieldL( iFilename, bufPtr, iCharsetUid, KDefaultSendingHeaderEncoding, EFalse);
	rOutputLine.Append(KImcvEqualsQuote);
	rOutputLine.Append( bufPtr );	
	rOutputLine.Append(KImcvQuoteString);
	CleanupStack::PopAndDestroy(&bufPtr);
	}

//---------------------------------------------------------------------------------
void CImSendMimeHeader::SetCharsetUidL(const TUint aId)
//---------------------------------------------------------------------------------
	{
	delete iCharsetString;
	iCharsetString=NULL;

	iCharsetUid=aId;

	if (iCharsetUid==KUidMsvCharsetNone)
		// Unknown charset, set string to original	
		iCharsetString = (iMimeHeader->GetContentTypeValue(KImcvCharset)).AllocL();
	else
		{
		if (!iCharsetUid)
			iCharsetUid=iConverter.CharConv().DefaultCharset();

		iCharsetString = iConverter.CharConv().GetMimeCharsetTextStringL(iCharsetUid);
		}
	}

//---------------------------------------------------------------------------------
TPtrC8	CImSendMimeHeader::GetCharsetString() const
//---------------------------------------------------------------------------------
	{
	return *iCharsetString;
	}


// Only calculates main header size
//---------------------------------------------------------------------------------
TInt CImSendMimeHeader::Size() const
//---------------------------------------------------------------------------------
	{
	TInt size=0;

	// MIME VERSION
	size+=KImcvMimePrompt().Length() + 
					KImcvSpMimeVersion().Length() + 2;

	// CONTENT LANGUAGE
	size+=KImcvContentLanguage().Length() + 
					KImcvDefaultLanguage().Length() + 2;

	// CONTENT TYPE
	TPtrC8 contentType = iMimeHeader->ContentType();
	if (contentType.Length()!=0)
		{
		size += KImcvContentType().Length() +
					contentType.Length() + 2;

		TPtrC8 contentSubType = iMimeHeader->ContentSubType();
		if ( contentSubType.Length() )
			{
			size += KImcvForwardSlash().Length() + contentSubType.Length() + 2;
			}

		if ( contentType.CompareF(KImcvText)==0 )
			{
			size += 3 ; // space, =, semi colon
			size += KImcvCharset().Length() + (GetCharsetString()).Length() + 2;
			}
		}

	// BOUNDARY
	size+= CImSendMimeEmail::KBoundaryStringLength + GetCharsetString().Length() + 2;

	// TRANSFER ENCODING
	TPtrC8 encodingType = EncodingType();
	if (encodingType.Length())
		size+= KImcvContentTransferEncoding().Length() + encodingType.Length() + 2;

	// CRLF
	size+= 2;
	
	return size;
	}


//*********************************************************************************
//              Class CImSendRichText Functions
//*********************************************************************************

//---------------------------------------------------------------------------------
CImSendRichText* CImSendRichText::NewLC(CImConvertCharconv& aConv)
//---------------------------------------------------------------------------------
	{
	CImSendRichText* self = new (ELeave) CImSendRichText(aConv);
	CleanupStack::PushL( self );
	return self;
	}

//---------------------------------------------------------------------------------
CImSendRichText* CImSendRichText::NewL(CImConvertCharconv& aConv)
//---------------------------------------------------------------------------------
	{
	CImSendRichText* self = CImSendRichText::NewLC(aConv);
	CleanupStack::Pop();
	return self;
	}

//---------------------------------------------------------------------------------
CImSendRichText::CImSendRichText(CImConvertCharconv& aConv) : iConverter(aConv)
//---------------------------------------------------------------------------------
	{
	}


//---------------------------------------------------------------------------------
CImSendRichText::~CImSendRichText() 
//---------------------------------------------------------------------------------
	{
	delete iLeftOver;
	delete iPlainBodyText;	
	iCurrentChunk.Close();
	}

//---------------------------------------------------------------------------------
TBool CImSendRichText::InitialiseL( CMsvServerEntry& aServerEntry,  TImCodec& anEncoder,
								    TImEncodingType aType, TUint aCharset)
//---------------------------------------------------------------------------------
	{
	TBool ret = 0;
	iPos = 0;
	delete iPlainBodyText;	
	iPlainBodyText = NULL;
	iIsNewChunk = ETrue;

	iCurrentChunk.Close();
	iCurrentChunk.CreateL(KChunkSize);	
	
	if (aServerEntry.HasStoreL())
		{
		CMsvStore* store = aServerEntry.ReadStoreL();
		CleanupStack::PushL(store);
		
		ret = store->HasBodyTextL();
		if (ret)
			{
			// Initialize the plainbodytext message store for reading the bodytext.
			iPlainBodyText = store->InitialisePlainBodyTextForReadL(KChunkSize);					
			iBodySize = iPlainBodyText->Size(); // Size() returns in no of bytes and the data is 16 bit formatted.
			}	
		
		CleanupStack::PopAndDestroy(store); 
		}

	if (aCharset ==KCharacterSetIdentifierIso88591 || aCharset== KCharacterSetIdentifierAscii)
		{
		aCharset=KCharacterSetIdentifierCodePage1252;
		}		

	iPreparedToConvert= iConverter.PrepareToConvertToFromOurCharsetL(aCharset);

	iBodyEncoding = aType;
	iEncoder=&anEncoder;

	return ret;
	}



//---------------------------------------------------------------------------------
TInt CImSendRichText::NextLineL( TDes8& rOutputLine, TInt& rPaddingCount )
//---------------------------------------------------------------------------------
	{
	__ASSERT_DEBUG( iPlainBodyText != NULL, gPanic(KPanicNoRichText) );

	TInt noMoreChars= 0;
	
	switch (iBodyEncoding)
		{
	case EEncodingTypeQP:
		noMoreChars = EncodeQPNextLineL(rOutputLine, rPaddingCount);
		break;
	case EEncodingTypeUU:
	case EEncodingTypeBASE64:
		noMoreChars = EncodeNextLineL(rOutputLine, rPaddingCount);
		break;
	case EEncodingTypeNone:
	default:
		NoEncodeNextLineL(rOutputLine, rPaddingCount);
		}
	
	return noMoreChars;
	}

/**
Extract line from the chunk restored from Message Store
@param rOutputLine The output line to be sent.
@return void.
*/
void CImSendRichText::ExtractLineFromChunkL(TDes16& aOutputLine)
	{
	// Check if there is enough data in the chunk to extract a line from it.
	if((iPos + iMaxLength) > iCurrentChunk.Length())
		{
		iIsNewChunk = ETrue;		
		}
	
	// Restore the newchunk from the messagestore.
	if(iIsNewChunk)
		{
		iIsNewChunk = EFalse;
		
		// Rearrange the remaining data from current chunk.
		iCurrentChunk.Delete(0, iPos);		
		TInt currentChunkLength = iCurrentChunk.Length();
		
		// Resize iCurrentChunk as per data left for send operation.
		iCurrentChunk.ReAllocL(KChunkSize + currentChunkLength);		
		iCurrentChunk.SetMax();
		
		TPtr16 restoredChunkPtr =  iCurrentChunk.MidTPtr(currentChunkLength);
		
		// Restore the chunk from Message Store.	
		iPlainBodyText->NextChunkL(restoredChunkPtr);
		// if NextChunkL did not return a full size chunk, then need to update the size of iCurrentChunk	
		if(restoredChunkPtr.Length() != KChunkSize)
			{
			iCurrentChunk.SetLength(currentChunkLength + restoredChunkPtr.Length());
			}
		iPos = 0;
		}

	// Check whether chunk length is not greater than (iPos+iMaxlength).
	if( (iPos + iMaxLength) >= iCurrentChunk.Length() )	
		{
		aOutputLine.Copy(iCurrentChunk.MidTPtr(iPos));	
		}
	else
		{
		aOutputLine.Copy(iCurrentChunk.MidTPtr(iPos, iMaxLength));			
		}
	}


// Convert character set of line	
//---------------------------------------------------------------------------------
TInt CImSendRichText::ConvertNextLineL( const TDesC& aInput, TDes8& rOutput)
//---------------------------------------------------------------------------------
	{
	TInt unconvertedChars=0;
	TInt pos=0;
	if (iPreparedToConvert)
		iConverter.ConvertFromOurCharsetL(aInput, rOutput, unconvertedChars, pos);
	else
		rOutput.Copy(aInput);

	return  pos;
	}

//---------------------------------------------------------------------------------
TInt CImSendRichText::NoEncodeNextLineL( TDes8& rOutputLine, TInt& rPaddingCount )
//---------------------------------------------------------------------------------
	{ 
	rOutputLine.Zero();

	// grab a few characters from the buffer 
	// (making sure this is more than we will ever need for the current line).

	TBuf<2*KMaxIMailBodyLineLength> source;

	iMaxLength = iPos+2*KMaxIMailBodyLineLength > iBodySize ? 
								iBodySize-iPos : 2*KMaxIMailBodyLineLength;
	
	// Extract line from chunk restored from store.
	ExtractLineFromChunkL(source);
	HBufC* output = HBufC::NewLC(rOutputLine.MaxLength());

	TPtr outPtr(output->Des());
	TInt ret = RemoveControlCharacters( source, outPtr, rPaddingCount );

	ConvertNextLineL( outPtr, rOutputLine);

	CleanupStack::PopAndDestroy(output); 
	return ret;
	}


//---------------------------------------------------------------------------------  
TInt CImSendRichText::RemoveControlCharacters( const TDesC& aInputLine, TDes& rOutputLine, TInt& rPaddingCount )
//---------------------------------------------------------------------------------  
	{ 
	TInt written=0;				// number of characters written to the current line

	TLex lexSource(aInputLine);
	TDes& ptr = rOutputLine;

	while ( (written<KMaxIMailBodyLineLength) && !lexSource.Eos() )
		{
		// first check whether we're at a line break
		if ( IsEOL(lexSource.Peek()) )
			{
			Append( ptr, (TDesC8&) KImcvCRLF);
			written+=2;

			// we just added an extra character
			rPaddingCount+=1;
			lexSource.Inc();
			break;
			}
		else if ( SmartBreak(written, lexSource.Remainder()) )
			{
			ptr.Append( lexSource.Peek() );
			Append( ptr, (TDesC8&) KImcvCRLF);
			written+=3;
			rPaddingCount+=2;
			lexSource.Inc();
			break;
			}
		else if ( written >= KMaxIMailBodyLineLength-1 )
			// make sure we don't exceed 76 chars per line of encoded text (must allow for CRLF)
			{
			Append( ptr, (TDesC8&) KImcvCRLF);
			written+=2;
			rPaddingCount+=2;
			}
			// check whether we have a whacky control character from CEditableText
		else if ( lexSource.Peek()<KImcvSP )
			{
			TUint8 replacement = ReplacementChar( lexSource.Peek() );
			if (replacement)	// potentially ditch the character altogther
				{
				ptr.Append( replacement );
				written++;
				}
			else
				rPaddingCount-=1;
			}
		else
			{
			ptr.Append( lexSource.Peek() );
			written++;
			}
		
		// advance to next source character	
		lexSource.Inc();

		} // while

	iPos += written - rPaddingCount;
	rPaddingCount = rPaddingCount >=0 ? rPaddingCount : 0;

	// set up the output parameter & return value
	return (iPos >= iBodySize ? 1 : 0);
	}

//---------------------------------------------------------------------------------
TInt CImSendRichText::EncodeNextLineL( TDes8& rOutputLine, TInt& rPaddingCount )
//---------------------------------------------------------------------------------
	{ 		
	rOutputLine.Zero();
	TInt sourceLength=0;

	HBufC8* output = HBufC8::NewLC(rOutputLine.MaxLength());
	TPtr8 outPtr(output->Des());

	TBuf<KMaxIMailBodyLineLength+1> source;

	iMaxLength = iPos+KMaxIMailBodyLineLength > iBodySize ? 
								iBodySize-iPos : KMaxIMailBodyLineLength;
	
	TInt leftOverLength= (iLeftOver) ? (*iLeftOver).Length() : 0 ;
	if (leftOverLength < KDecodeLineLength && iPos<iBodySize )
		{
		// Extract line from chunk restored from store.
		ExtractLineFromChunkL(source);
		sourceLength=source.Length();	

		// Character conversion
		ConvertNextLineL( source, outPtr );

		ConvertLineBreaks(source, ETrue); 
		}

	if (iLeftOver)
		{
		outPtr.Insert(0, *iLeftOver);
		delete iLeftOver;
		iLeftOver=NULL;
		}
	
	TInt rem = outPtr.Length() - KDecodeLineLength;
	if (rem>0)
		{
		iLeftOver=(outPtr.Right(rem)).AllocL();
		outPtr.SetLength(KDecodeLineLength);
		}

	if (outPtr.Length())
		iEncoder->Encode(outPtr, rOutputLine); 

	if (source.Length()) // some text has bee extracted from the rich text store
		iPos +=sourceLength;
	
	CleanupStack::PopAndDestroy(output); 

	rPaddingCount=rOutputLine.Length();

	// set up the output parameter & return value
	return iLeftOver ? 0 : (iPos >= iBodySize ? 1 : 0);
	}


//---------------------------------------------------------------------------------
TInt CImSendRichText::EncodeQPNextLineL( TDes8& rOutputLine, TInt& rPaddingCount )
//---------------------------------------------------------------------------------
	{ 		
	rOutputLine.Zero();
	iUseRichText=ETrue;
	TInt sourceUsed=0;
	TInt crlfPadding=0;
	TBool unicodeChars=EFalse;
	TInt originalSource=0;

	HBufC8* output = HBufC8::NewLC(rOutputLine.MaxLength());
	TPtr8 outPtr(output->Des());

	TBuf<KMaxIMailBodyLineLength+1> source;

	iMaxLength = iPos+KMaxIMailBodyLineLength > iBodySize ? 
								iBodySize-iPos : KMaxIMailBodyLineLength;
	
	TInt leftOverLength= (iLeftOver) ? (*iLeftOver).Length() : 0 ;
	if (leftOverLength < KMaxIMailBodyLineLength && iPos<iBodySize )
		{
		// Extract line from chunk restored from store.
		ExtractLineFromChunkL(source);
		crlfPadding=ConvertLineBreaks(source, ETrue); 
		originalSource=source.Length()-crlfPadding;

		// Character conversion
		ConvertNextLineL( source, outPtr );

		unicodeChars = (outPtr.Length() > source.Length()) ? ETrue:EFalse;
		}

	if (iLeftOver)
		outPtr.Insert(0, *iLeftOver);
	
	// QP encode line 
	((TImCodecQP*) iEncoder)->AddPlainChar(KImcvPlainRichText);


	TInt written = iEncoder->Encode(outPtr, rOutputLine); 
	TInt leftover = outPtr.Length() - written;

	if (iLeftOver)
		{
		TInt rem = leftOverLength - written;
		if (rem>0)
			{
			// Not all of the leftOver buffer was used.
			iLeftOver->Des().Copy((*iLeftOver).Right(rem));
			sourceUsed=0;
			}
		else
			{
			delete iLeftOver;
			iLeftOver=NULL;
			if(leftover)
 				sourceUsed=rem*(-1);
 			else
 				sourceUsed=originalSource;
			}
		}
	else if (leftover)
			sourceUsed=written;
	else 
		sourceUsed=originalSource;

	if (unicodeChars && !iLeftOver && leftover>0)
		{
		iLeftOver=(outPtr.Right(leftover)).AllocL();
 		iPos += originalSource;
		}
	else if (source.Length()) // some text has bee extracted from the rich text store
		iPos +=sourceUsed;
	
	CleanupStack::PopAndDestroy(output);

	rPaddingCount=rOutputLine.Length();

	// set up the output parameter & return value
	return iLeftOver ? 0 : (iPos >= iBodySize ? 1 : 0);
	}



//---------------------------------------------------------------------------------
TBool CImSendRichText::SmartBreak( TInt aWritten, const TUint8* aSource, TInt aMaxLength )
//---------------------------------------------------------------------------------
	{
	// Check whether the current char is breakable.

	TBool ret = EFalse;
	TUint8 ch = *aSource;

	if ( IsBreakable(ch) )
		{
		// Scan ahead looking for the next breakable char

		const TUint8* lookAhead = aSource+1;
		TBool found = EFalse;
		while ( !found && lookAhead-aSource <= aMaxLength )
			{
			found = IsBreakable(*lookAhead);
			if (!found)
				lookAhead++;
			}

		// There's another breakable char before the end of the text - we need to break now 
		// if it's too far away and only if the non-breakable text fits on an output line.

		ret = (aWritten+(lookAhead-aSource) >= KMaxIMailBodyLineLength 
			             && (lookAhead-aSource) < KMaxIMailBodyLineLength);
		}

	return ret;
	}


//---------------------------------------------------------------------------------  
TBool CImSendRichText::SmartBreak( TInt aWritten, const TDesC& aSource )
//---------------------------------------------------------------------------------  
	{
	TBool ret = EFalse;
	TLex source( aSource );

	// only check for whether we should break if the current char is breakable
	if ( IsBreakable(source.Peek()) )
		{
 		// scan ahead looking for the next breakable char
		source.Inc();
		TBool found = EFalse;
		TInt encodingOffset=(IsPlain(source.Peek()) ? 0 : 2);
		while ( !found && !source.Eos() )
			{
			found = IsBreakable(source.Peek());
			if (!found)
				{
				encodingOffset+=(IsPlain(source.Peek()) ? 0 : 2);
				source.Inc();
				}
			}
		// there's another breakable char before the end of the text 
		// - we need to break now if it's too far away
		// but only if the non-breakable text fits on a line itself.

		ret = ((aWritten+source.Offset()+encodingOffset) > KMaxIMailBodyLineLength-4);
		}

	return ret;
	}

//*********************************************************************************
//              Class CImSendFile Functions
//*********************************************************************************

//---------------------------------------------------------------------------------
CImSendFile* CImSendFile::NewLC(RFs& anFs, CImConvertCharconv& aCharConv)
//---------------------------------------------------------------------------------
	{
	CImSendFile* self = new (ELeave) CImSendFile(anFs, aCharConv);
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

//---------------------------------------------------------------------------------
CImSendFile* CImSendFile::NewL(RFs& anFs, CImConvertCharconv& aCharConv)
//---------------------------------------------------------------------------------
	{
	CImSendFile* self = CImSendFile::NewLC(anFs, aCharConv);
	CleanupStack::Pop();
	return self;
	}

/**
	Intended Usage	:	Second phase of two-phase construction method. Does any 
						allocations required to fully construct the object.
	@since			7.0s
	@leave			KErrNoMemory.
	@pre 			First phase of construction is complete
	@post			The object is fully constructed and initialised.
 */
//---------------------------------------------------------------------------------
void CImSendFile::ConstructL()
//---------------------------------------------------------------------------------
	{
	iAttachmentFile = new(ELeave) TImAttachmentFile(CONST_CAST(RFs&,iFs));
	}

//---------------------------------------------------------------------------------
CImSendFile::CImSendFile(RFs& anFs, CImConvertCharconv& aCharConv) 
	:iFs(anFs), iConverter(aCharConv), iAttachmentFile(NULL)
//---------------------------------------------------------------------------------
	{
	}

//---------------------------------------------------------------------------------
void CImSendFile::InitialiseL( CMsvServerEntry& aServerEntry, TImFileCodec& anEncoder)
//---------------------------------------------------------------------------------
	{
	iEncoder = &anEncoder;
	iAttachmentState=ENextFile;
	Filename(aServerEntry, iAttachmentInfo);
	}

//---------------------------------------------------------------------------------
CImSendFile::~CImSendFile() 
//---------------------------------------------------------------------------------
	{
	delete iAttachmentFile;
	}

//---------------------------------------------------------------------------------
TInt CImSendFile::NextLineL( TDes8& rOutputLine, TInt& rPaddingCount )
//---------------------------------------------------------------------------------
	{
	rOutputLine.Zero();
				
	do
		{
		switch(iAttachmentState)
			{
		case ENextFile:
			iAttachmentState=EPrefixLines;
			break;
		case EPrefixLines:
			if (iEncoder->PrefixNextLineL(rOutputLine, iAttachmentInfo.iName, rPaddingCount)==KImAttFinished)
				iAttachmentState=EEncodeFile;
			break;		
		case EEncodeFile:
			if (iAttachmentFile->ReadFile( iSourceLine, KDecodeLineLength))
				{
				iAttachmentState=EPostfixLines;
				}
			else if (iSourceLine.Length()!=0)
				{
				iEncoder->Encode(iSourceLine, rOutputLine);	
				rPaddingCount=rOutputLine.Length()-(iSourceLine.Length());
				}
			else
				{
				iAttachmentState=EPostfixLines;
				}
			break;
		case EPostfixLines:
			if (iEncoder->PostfixNextLine(rOutputLine, rPaddingCount)==KImAttFinished)
				iAttachmentState=EEndOfAttachments;
			break;
		case EEndOfAttachments:
			iAttachmentFile->CloseFile();
			iAttachmentState=ECloseDelimiter;
			break;
		case ECloseDelimiter:
			// Dependent on encoding type
			iAttachmentState=ECRLFLine;
			break;				
		case ECRLFLine:
			AddCRLF(rOutputLine, rPaddingCount);
			rPaddingCount=rOutputLine.Length();
			iAttachmentState=EEnd;
			break;				
			}
	
		} while(!rOutputLine.Length());
	
		return (iAttachmentState==EEnd ? KImAttFinished : KErrNone);
	}

//---------------------------------------------------------------------------------
const TFileName& CImSendFile::Filename() const
//---------------------------------------------------------------------------------
	{
	return iAttachmentInfo.iName;
	}

TFileName& CImSendFile::Filename(CMsvServerEntry& aEntry, SAttachmentInfo& aInfo)
	{
	TMsvEntry entry = aEntry.Entry();
	aInfo.iSize=entry.iSize; 

		CMsvStore* store = aEntry.ReadStoreL(); 
		CleanupStack::PushL(store);
		MMsvAttachmentManager& manager=store->AttachmentManagerL();
		if(manager.AttachmentCount())
			{
			CMsvAttachment* attachment = manager.GetAttachmentInfoL(0);
			CleanupStack::PushL(attachment);
			TParsePtrC parse(attachment->FilePath());
			aInfo.iPath=parse.DriveAndPath();
			aInfo.iName=parse.NameAndExt();
			iAttachmentFile->CloseFile();
			RFile file=manager.GetAttachmentFileL(0);
			iAttachmentFile->SetFileHandle(file,TImAttachmentFile::EImFileRead);
			CleanupStack::PopAndDestroy(attachment);	
			}
		CleanupStack::PopAndDestroy(store); 
	return aInfo.iName;
	}

//---------------------------------------------------------------------------------
void CImSendFile::EmbeddedEmailFilename(TMsvEntry& aEntry, TFileName& aFileName)
//---------------------------------------------------------------------------------
 	{
 	if (aEntry.iDescription.Length())
 		{
 		aFileName = aEntry.iDescription;
 		if ( (aFileName.MaxLength() - aFileName.Length()) >= KImcvEmbeddedEmailFilenameExtension().Length() )
 			{
 			aFileName.Insert(aFileName.Length(), KImcvEmbeddedEmailFilenameExtension);
 			}
 		else
 			{
 			aFileName = KImcvEmbeddedEmailDefaultFilename;
 			aFileName.Insert(aFileName.Length(), KImcvEmbeddedEmailFilenameExtension);
 			}
 		}
 	}
 
 


//*********************************************************************************
//              Class CImCalculateMsgSize Functions
//*********************************************************************************

//---------------------------------------------------------------------------------
EXPORT_C CImCalculateMsgSize* CImCalculateMsgSize::NewL(RFs &anFs,CMsvServerEntry& aServerEntry)
//---------------------------------------------------------------------------------
	{
	CImCalculateMsgSize* self=new (ELeave) CImCalculateMsgSize(anFs, aServerEntry);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


//---------------------------------------------------------------------------------
CImCalculateMsgSize::CImCalculateMsgSize(RFs &anFs,CMsvServerEntry& aServerEntry)
				: 	CMsgActive(EPriorityStandard), iFs(anFs),iServerEntry(aServerEntry)
//---------------------------------------------------------------------------------
	{
	}


//---------------------------------------------------------------------------------
void CImCalculateMsgSize::ConstructL()
//---------------------------------------------------------------------------------
	{	
	CActiveScheduler::Add(this);	
	}

//---------------------------------------------------------------------------------
EXPORT_C CImCalculateMsgSize::~CImCalculateMsgSize()
//---------------------------------------------------------------------------------
	{
	Cancel();
	Reset();
	}


//---------------------------------------------------------------------------------
EXPORT_C void CImCalculateMsgSize::StartL(TRequestStatus& aStatus,TMsvId aMsvId,
			TImSendMethod aAlgorithm, TTime& aTimeDate, const TDesC& aDomainName,
			const TUint aCharset)
//---------------------------------------------------------------------------------
	{
	Reset();
	//initialise
	iMailString.Zero();
	iFinished=KErrNone;
	iSize=0;
	//
	iSendMsg=CImSendMessage::NewL(iServerEntry);
	iSendMsg->InitialiseL(aMsvId, aAlgorithm, aTimeDate, aDomainName, aCharset, ESendNoCopy);
	//
#if defined (_DEBUG)
	TBuf<25> KFileName=_L("c:\\mailtest\\imcv\\size.txt");
	TInt err=iFile.Open(iFs, KFileName, EFileStreamText|EFileWrite|EFileShareAny);
	if (err==KErrNotFound) // file does not exist - create it
		{
		err = iFile.Create(iFs, KFileName, EFileStreamText|EFileWrite|EFileShareAny);
		}
	if (err == KErrNone)
		{
		iFileOpen=ETrue;
		}
#endif
	Queue(aStatus);
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,0);
	SetActive();
	}


//---------------------------------------------------------------------------------
void CImCalculateMsgSize::Reset()
//---------------------------------------------------------------------------------
	{
	delete iSendMsg;
	iSendMsg=NULL;
#if defined (_DEBUG)
	if (iFileOpen)
		{
		iFile.Close();
		iFileOpen=EFalse;
		}
#endif
	}

//---------------------------------------------------------------------------------
void CImCalculateMsgSize::DoRunL()
//---------------------------------------------------------------------------------
	{
	if (iStatus.Int()!=KErrNone || iFinished==KImCvFinished)
		return;
	TInt count=0;
	iFinished=iSendMsg->NextLineL(iMailString,count);
	iSize+=iMailString.Length();
#if defined (_DEBUG)
	if (iFileOpen)
		{
		iFile.Write(iMailString);
		}
#endif
	SetActive();
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,0);
	}


//---------------------------------------------------------------------------------
void CImCalculateMsgSize::DoCancel()
//---------------------------------------------------------------------------------
	{
	CMsgActive::DoCancel();
	}

