// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "cimapstatus.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "cimapatomwalker.h"
#include "cimapatom.h"
#include "cimaputils.h"
#include "cimapcharconv.h"

// IMAP STATUS command
_LIT8(KCommandLogin, "%d STATUS %S %S\r\n");

/**
The factory constructor. Part of two phased construction.
*/
CImapStatus* CImapStatus::NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, const TDesC& aMailboxName, const TDesC8& aStatusDataItemNames, CImapFolderInfo& aFolderInfo)
// static method first phase construction
	{		
	CImapStatus* self = new (ELeave) CImapStatus(aSelectedFolderData, aLogId, aFolderInfo);
	CleanupStack::PushL(self);
	self->ConstructL(aMailboxName, aStatusDataItemNames);
	CleanupStack::Pop();
	return self;
	}
	
CImapStatus::CImapStatus(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImapFolderInfo& aFolderInfo) 
	: CImapCommandEx(aSelectedFolderData, aLogId)
	, iFolderInfo(aFolderInfo)
	{	
	}


/**
Second phase constructor.
*/		
void CImapStatus::ConstructL(const TDesC& aMailboxName, const TDesC8& aStatusDataItemNames)
	{
	// Converting the mailbox name from 16 bit to imap-UTF7 charset..
	iMailboxName = CImapUtils::GetRef().Charconv().ConvertFromUnicodeToImapUtf7L(aMailboxName);
	
	iStatusDataItemNames = aStatusDataItemNames.AllocL();
	iAtomParser = CImapAtomParser::NewL(EFalse, iLogId);
	}
/**
Destructor.
*/
CImapStatus::~CImapStatus()
	{
	delete iAtomParser;
	delete iMailboxName;
	delete iStatusDataItemNames;
	}

/** 
Formats and sends the IMAP STATUS command.
@param aTagId Command sequence id which will be sent along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapStatus::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;

	TInt buffLength = KCommandLogin().Length();	
	buffLength += iMailboxName->Length();
	buffLength += iStatusDataItemNames->Length();
	
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(buffLength + TagLength(aTagId));	
	iOutputBuffer->Des().Format(KCommandLogin, aTagId, iMailboxName, iStatusDataItemNames);	
	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);
	}

/**
Checks whether this is a STATUS response.
If it is, then it starts buiding an atom tree.  
If the tree is completed then the data is parsed straight away.  Otherwise, more data is requested.
If the atom tree is completed by this line, then tree is parsed to decode the response.
@return ENotRecognised if this is not a STATUS response
		EResponseIncomplete if this is a STATUS resposne, but more data is required.
		ECompleteUntagged if this is a STATUS response that has been fully parsed.
*/
CImapCommand::TParseBlockResult CImapStatus::ParseUntaggedResponseL()
	{
	TParseBlockResult result = ENotRecognised;
	
	TPtrC8 response = GetNextPart();
	if(response.CompareF(KImapTxtStatus) == 0)
		{
		TPtrC8 remainingData = Remainder();
   		TBool wantMoreData = iAtomParser->ProcessLineL(remainingData);
   		if (wantMoreData)
   			{
   			result = EResponseIncomplete;
   			}
   		else
   			{
   			ParseStatusResponseL();
   			result = ECompleteUntagged;
   			}
		}
	
	return result;
	}

/**
Adds the block of literal data to the atom tree.
And waits for the following line.
*/
void CImapStatus::ParseLiteralBlockL()
	{
	iAtomParser->ProcessLiteralBlockL(iUnparsedData);
	}

/**
Adds the line of data to the atom tree.
If the atom tree is completed by this line, then tree is parsed to decode the response.
@return Whether more data (i.e. another literal block) is expected.
*/
TBool CImapStatus::ParseLineFollowingLiteralL()
	{
	TBool wantMoreData = iAtomParser->ProcessLineL(iUnparsedData);
	
	if(!wantMoreData)
		{
		ParseStatusResponseL();
		}
		
	return wantMoreData;
	}

void CImapStatus::ParseStatusResponseL()
 	{
 	// Traverse the atom tree
 	CImapAtomWalker* atomWalker = CImapAtomWalker::NewL(iLogId);
 	CleanupStack::PushL(atomWalker);
 	
 	atomWalker->SetRootL(iAtomParser->RootAtom());
 	
 	// Move to the first item of the atom tree...
 	atomWalker->WalkDownL();
 	
 	// Skip over the mailbox name
 	atomWalker->WalkAcrossL(ETrue);
 	
  	// We should now be at the opening bracket for the status-att-list
  	atomWalker->WalkDownL();
  	
  	// status-att-list = status-att SP number *(SP status-att SP number)
 	do
 		{
 		// Get the status-att
 		TPtrC8 statusAtt = atomWalker->CurrentDes(EFalse);
 		
 		// Get the number value corrosponding to a status data item.
 		atomWalker->WalkAcrossL(ETrue);
 		TPtrC8 value = atomWalker->CurrentDes(EFalse);
 		
 		ParseStatusAttributeL(statusAtt, value);
 		} 
  		while (atomWalker->WalkAcrossL(EFalse));
 		 	
 	CleanupStack::PopAndDestroy(atomWalker);
 	}

/**
Parses the status response.
@param aAttribute.The status data item.(MESSAGES/UIDNEXT/RECENT/UIDVALIDITY/UNSEEN)
@param aValue. The value corrosponding a particular status data item.
*/
void CImapStatus::ParseStatusAttributeL(const TDesC8& aAttribute, const TDesC8& aValue )
// Parse response of the form (MESSAGES 899 UIDNEXT 8874)...
	{
	TPtrC8 nextPart(aAttribute);
	TLex8 desToInt(aValue);
	TInt tempVal = 0;
	
	if(desToInt.Val(tempVal) != KErrNone)
		{
		CorruptDataL();
		}
	
	// From RFC3501 section 9
	//	
	// status-att = "MESSAGES" / "RECENT" / "UIDNEXT" / "UIDVALIDITY" / "UNSEEN"
		
	if(nextPart.CompareF(KImapTxtMessages) == 0)
		{					
		iFolderInfo.SetMessages(tempVal);
		}
	else if(nextPart.CompareF(KImapTxtRecent) == 0)
		{
		iFolderInfo.SetRecent(tempVal);					
		}
	else if(nextPart.CompareF(KImapTxtUidNext) == 0)
		{
		iFolderInfo.SetUidNext(tempVal);					
		}
	else if(nextPart.CompareF(KImapTxtUidValidity) == 0)
		{					
		iFolderInfo.SetUidValidity(tempVal);
		}
	else if(nextPart.CompareF(KImapTxtUnseen) == 0)
		{					
		iFolderInfo.SetUnseen(tempVal);
		}			
	}

