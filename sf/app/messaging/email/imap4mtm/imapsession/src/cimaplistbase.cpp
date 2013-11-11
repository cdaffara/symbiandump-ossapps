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


#include "cimaplistbase.h"
#include "moutputstream.h"
#include "cimapsessionconsts.h"
#include "cimapatomparser.h"
#include "cimapatomwalker.h"
#include "cimaputils.h"
#include "cimapcharconv.h"
#include "cimapsettings.h"

// Inbox mailbox name
_LIT16(KIMAP_INBOX, "INBOX");

const TUint KNullTerminator = '\0';

void CImapListBase::ConstructL(const TDesC& aReferenceName, const TDesC& aMailboxName)
	{
	iReferenceName = EncodeMailboxNameForSendL(aReferenceName);
	iMailboxName = EncodeMailboxNameForSendL(aMailboxName);
	iAtomWalker = CImapAtomWalker::NewL(iLogId);
	}

CImapListBase::CImapListBase(CImapFolderInfo* aSelectedFolderData, TInt aLogId, RArrayImapListFolderInfo& aFolderList, const CImapSettings& aImapSettings, const TDesC8& aSendMessageFormat, const TDesC8& aResponseIdentifier)
	: CImapCommandEx(aSelectedFolderData, aLogId)
	, iFolderList(aFolderList)
	, iSendMessageFormat(aSendMessageFormat)
	, iResponseIdentifier(aResponseIdentifier)
	, iListParseState(EWaitingForResponse)
	, iImapSettings(aImapSettings)
	{		
	}
	
CImapListBase::~CImapListBase()
	{
	delete iAtomWalker;
	delete iAtomParser;
	delete iReferenceName;
	delete iMailboxName;
	}

/** 
Formats and sends the IMAP LIST command.
@param aTagId Command sequence id which will be send along with the IMAP command.
@param aStream A wrapper for the outbound stream of a connected socket, using which 
the command will be send to the server
*/
void CImapListBase::SendMessageL(TInt aTagId, MOutputStream& aStream)
	{
	iTagId = aTagId;
	
	TInt bufLength = iSendMessageFormat.Length();	
	bufLength += TagLength(aTagId);
	bufLength += iReferenceName->Length();
	bufLength += iMailboxName->Length();
	
	__ASSERT_DEBUG(iOutputBuffer==NULL, TImapServerPanic::ImapPanic(TImapServerPanic::ECommandOutputBufferNotNull));
	iOutputBuffer = HBufC8::NewL(bufLength);	
	iOutputBuffer->Des().Format(iSendMessageFormat, aTagId, iReferenceName, iMailboxName);
	
	//send the command to the server
	aStream.SendDataReq(*iOutputBuffer);	
	}

/**
If this is a LIST/LSUB response, then begin parsing it.
Parsing of LIST/LSUB responses is a two phase process.
First, build an atom tree using the atom parser.  
The atom parser determines whether extra literal data is expected on the stream
Once the atom parser has enough data (either here or in ParseLineFollowingLiteralL()) 
then the second phase is to process the atom tree by calling ProcessMailboxListL().
Otherwise return not recognised
@return ENotRecognised (if not a LIST/LSUB response)
		ECompleteUntagged (if a LIST/LSUB response with no literal data)
		EResponseIncomplete (if a LIST/LSUB response wiht literal data expected)		
*/
CImapCommand::TParseBlockResult CImapListBase::ParseUntaggedResponseL()
	{
	TParseBlockResult result = ENotRecognised;
		
	// Is this a list response?
	TPtrC8 part = GetNextPart(); // returns KNullDesC8 if there is no part available
				
	if(part.CompareF(iResponseIdentifier) == 0)
		{
		__ASSERT_DEBUG(iAtomParser == NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EListBaseAtomParserAlreadyExists));
		iAtomParser = CImapAtomParser::NewL(ETrue, iLogId);
		
		TBool wantMore = iAtomParser->ProcessLineL(iUnparsedData);
		if (wantMore)
			{
			result = EResponseIncomplete;
			}
		else
			{
			// All the data was received in the fist line.  Process it now.
			ProcessMailboxListL();
			result = ECompleteUntagged;
			}
		}
	else
		{
		result = ENotRecognised;
		}			
   		
	return result;
	}

/**
Delegates parsing of incoming literal data to the atom parser.
*/
void CImapListBase::ParseLiteralBlockL()
	{
	__ASSERT_DEBUG(iAtomParser != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EListBaseAtomParserIsNull));
	iAtomParser->ProcessLiteralBlockL(iUnparsedData);
	}
	
/**
Delegates parsing of the incoming line of data to the atom parser.
If the atom parser has all the data it needs, then processing of the mailbox data
can begin.
@return whether further literal data is expected
*/
TBool CImapListBase::ParseLineFollowingLiteralL()
	{
	__ASSERT_DEBUG(iAtomParser != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EListBaseAtomParserIsNull));
	
	TBool wantMoreData = iAtomParser->ProcessLineL(iUnparsedData);
 	if(!wantMoreData)
 		{
 		// All the data for this LIST/LSUB response has been recieved.  Process it now.
		ProcessMailboxListL();
		}
		
	return wantMoreData;
	}

/**
Having received all the data for a LIST/LSUB response, this method processes the atom tree
representation of the response.
A CImapListFolderInfo object representing the response is populated and then inserted 
in alphabetic order (based on the mailbox name) into the output array of CImapListFolderInfo objects
that was passed into this object on construction.
*/
void CImapListBase::ProcessMailboxListL()
	{
	// mailbox-list = "(" [mbx-list-flags] ")" SP (DQUOTE QUOTED-CHAR DQUOTE / nil) SP mailbox
	//
	// mailbox = "INBOX" / astring

	__ASSERT_DEBUG(iAtomParser != NULL, TImapServerPanic::ImapPanic(TImapServerPanic::EListBaseAtomParserIsNull));
	
	iAtomWalker->SetRootL(iAtomParser->RootAtom());
	iAtomWalker->WalkDownL();
	
	CImapListFolderInfo* folderInfo = CImapListFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
		
	ProcessMailboxListFlagsL(*folderInfo);
	ProcessHeirarchySeparatorL(*folderInfo);	
	ProcessMailboxL(*folderInfo);

	TLinearOrder<CImapListFolderInfo> sorter(CImapListFolderInfo::CompareByFolderName);
	iFolderList.InsertInOrderL(folderInfo, sorter);

	// Finished with the atom tree for this response.
	delete iAtomParser;
	iAtomParser = NULL;

	CleanupStack::Pop(folderInfo);
	}

/**
Process the mbx-list-flags portion of the LIST/LSUB response
@param aFolderInfo recieves the flags data
*/
void CImapListBase::ProcessMailboxListFlagsL(CImapListFolderInfo& aFolderInfo)
	{
	// mbx-list-flags =	*(mbx-list-oflag SP) mbx-list-sflag *(SP mbx-list-oflag) /
	//					mbx-list-oflag *(SP mbx-list-oflag)
	//
	// mbx-list-oflag = "\Noinferiors" / flag-extension
	//		; Other flags; multiple possible per LIST response
	//
	// flag-extension = "\" atom
	//
	// mbx-list-sflag = "\Noselect" / "\Marked" / "\Unmarked"
	//		; Selectability flags; only one per LIST response

	// mbx-list-flags cannot contain brackets, so it is safe to search for its delimiting brackets

	// There may be no flags, so check for this first
	if (iAtomWalker->PeekDown())
		{
		iAtomWalker->WalkDownL();
		do
			{
			// mbx-list-oflag = "\Noinferiors" / flag-extension
			if (iAtomWalker->CurrentMatch(KImapTxtNoInferiors))
				{
				aFolderInfo.SetFlag(CImapListFolderInfo::ENoinferiors, ETrue);
				}
		
			// mbx-list-sflag = "\Noselect" / "\Marked" / "\Unmarked"
			// Server should only send one of these (but must send one)
			else if (iAtomWalker->CurrentMatch(KImapTxtNoSelect))
				{
				aFolderInfo.SetFlag(CImapListFolderInfo::ENoselect, ETrue);
				}
			else if (iAtomWalker->CurrentMatch(KImapTxtMarked))
				{
				aFolderInfo.SetFlag(CImapListFolderInfo::EMarked, ETrue);
				}
			else if (iAtomWalker->CurrentMatch(KImapTxtUnmarked))
				{
				aFolderInfo.SetFlag(CImapListFolderInfo::EUnmarked, ETrue);
				}
		
			} while (iAtomWalker->WalkAcrossL(EFalse));
	
		iAtomWalker->WalkUpL();
		}
	}

/**
Processes the heirarchy separator portion of the LIST/LSUB response.
@param aFolderInfo receives the heirarchy character
*/
void CImapListBase::ProcessHeirarchySeparatorL(CImapListFolderInfo& aFolderInfo)
	{
	// now look for heirarchy separator
	// (DQUOTE QUOTED-CHAR DQUOTE / nil)
	
	iAtomWalker->WalkAcrossL(ETrue);
	TPtrC8 heirarchySeparator = iAtomWalker->CurrentDes(ETrue); // this is effectively an nstring
		
	if (heirarchySeparator.Length() == 1)
		{
		// this is the most likely scenario so start here.
		aFolderInfo.iHierarchySeperator = heirarchySeparator[0];
		}
	else if (heirarchySeparator.Length() == 0)
		{
		// The separator was NIL
		aFolderInfo.iHierarchySeperator = KNullTerminator;
		}
	else
		{
		CorruptDataL(); // we were returned a string that was too long for a heirarchySeparator.
		}
	}

/**
Extracts the mailbox name from the LIST/LSUB response.
@param aFolderInfo receives the mailbox name
*/
void CImapListBase::ProcessMailboxL(CImapListFolderInfo& aFolderInfo)
	{
	iAtomWalker->WalkAcrossL(ETrue);
	TPtrC8 mailboxNameUtf7 = iAtomWalker->CurrentDes(EFalse); // mailbox not an nstring.
	
	// Convert into ImapUtf7.
	HBufC16* mailboxNameUnicode = CImapUtils::GetRef().Charconv().ConvertToUnicodeFromImapUtf7L(mailboxNameUtf7);
	TPtr16 namePtr = mailboxNameUnicode->Des();

	// Strip leading hierarchy separators
	TPtrC16 stripName(namePtr);
	while ((stripName.Length() > 0) && (stripName.Locate(aFolderInfo.iHierarchySeperator) == 0))
		{
		stripName.Set(stripName.Mid(1));
		}

	TInt numFound = namePtr.Length() - stripName.Length();
	if (numFound > 0)
		{
		namePtr.Delete(0, numFound);
		}

	// Strip trailing hierarchy separators
	while ((namePtr.Length() > 0) && (namePtr.LocateReverse(aFolderInfo.iHierarchySeperator) == namePtr.Length() - 1))
		{
		namePtr.Delete(namePtr.Length() - 1, 1);
		}

	// If the first part of the path name is the inbox, make sure we use
	// the uppercase version INBOX. This allows for case sensitive searches
	// to be performed later.
	TInt inboxLength = KIMAP_INBOX().Length();

	// Check that the input folder name is long enough to contain INBOX and
	// the hierarchy separator.
	if (namePtr.Length() >= inboxLength + 1)
		{
		TPtrC16 inboxPart = namePtr.Left(inboxLength);

		if (inboxPart.CompareF(KIMAP_INBOX) == 0 && inboxPart.Compare(KIMAP_INBOX) != 0)
			{
			TPtrC16 separatorPart = namePtr.Mid(inboxLength, 1);

			if (separatorPart.Compare(iImapSettings.PathSeparator()) == 0)
				{
				namePtr.Replace(0, inboxLength, KIMAP_INBOX);
				}
			}
		}

	aFolderInfo.SetFolderNameL(mailboxNameUnicode);
	}
