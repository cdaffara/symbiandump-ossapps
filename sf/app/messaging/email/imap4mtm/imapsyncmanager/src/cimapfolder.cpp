// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cimapfolder.h"
#include "cimapsyncmanager.h"
#include "cimapsession.h"
#include "cimaprfc822headerfields.h"
#include "cimapbodystructure.h"
#include "cimapfolderinfo.h"
#include "cimapfolderindex.h"
#include "cimaplogger.h"
#include <numberconversion.h>
#include <imcvcodc.h>
#include <miutconv.h>
#include "cimapmimeheaderfields.h"
#include "cimapcharconv.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "cimconvertheader.h"
#include "cimconvertcharconv.h"
#endif

// For first stage download (to view in folder list)
_LIT8(KImapFetchHeaderToEnd, "%d:*");
_LIT8(KImapFetchHeaderRange,"%d:%d");
_LIT8(KImapFetchHeaderUIDRange,"UID %d:%d");
_LIT8(KImapFetchHeaderRangeSearch,"%d:%d %S");
_LIT8(KImapSmallHeaderFields,"Received Date Subject From Priority X-MSMail-Priority X-Priority Importance");
//_LIT8(KImapLargeHeaderFields,"Received Date Subject From Reply-to To Cc Bcc Message-ID Priority %S");

_LIT(KQuoteChar, "\\");
_LIT(KIMAP_INBOX, "INBOX");

_LIT8(KImapTxtImage, "IMAGE");
_LIT8(KImapTxtAudio, "AUDIO");
_LIT8(KImapTxtVideo, "VIDEO");
_LIT8(KImapTxtApplication, "APPLICATION");
_LIT8(KImapTxtRelated, "RELATED");
_LIT8(KImapTxtAlternative, "ALTERNATIVE");
_LIT8(KImapTxtMixed, "MIXED");
_LIT8(KImapTxtHtml, "HTML");
_LIT8(KImapTxtAttachment, "ATTACHMENT");
_LIT8(KImapTxtCalendar, "CALENDAR");
_LIT8(KImapTxtXVCalendar, "X-VCALENDAR");
_LIT8(KImapTxtDeliveryStatus, "DELIVERY-STATUS");
_LIT8(KImapTxtBase64, "BASE64");

// IMAP Priority fields
//_LIT8(KImapPriorityFieldsSearch, "Priority Priority X-MSMail-Priority Precedence Importance");

// Constants
const TInt KImapUidSearchSize = 400;
const TInt KImapMaxIntChars = 11; // the maximum number of characters needed to represent a 32-bit unsigned integer as a string.


// Efficient and SAFE way of comparing TBools which might have different integers representing TRUE
inline TBool BoolsAreEqual( TBool aA, TBool aB )
	{
	return ((aA && aB) || (!aA && !aB));
	}
	
inline TBool BoolsAreNotEqual( TBool aA, TBool aB )
	{
	return ((!aA || !aB) && (aA || aB));
	}

/**
Constructor
*/
CImapFolder::CImapFolder(CImapSyncManager& aSyncMan, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings)
: CMsgActive(EPriorityStandard), 
iSyncManager(aSyncMan),
iServerEntry(aServerEntry),
iImapSettings(aImapSettings),
iHeaderConverter(CImapUtils::GetRef().Charconv().HeaderConverter())
	{
	}

/**
Destructor
*/
CImapFolder::~CImapFolder()
	{
	iMatchingMessageIds.Reset();
	iDeletedMessageIds.Reset();
	iMissingMessageIds.Reset();
	
	if(iCachedEntryData)
		{
	    iCachedEntryData->Close();
		}
		
	delete iSelection;
	iFolderIndex->Reset();
	iMessageFlagInfoArray.Reset();
	delete iFolderIndex;
	delete iCachedEntryData;
	delete iClearSeenList;
	delete iSetSeenList;
	iFullFolderPath.Close();
	}

/**
Static factory constructor

@param aSyncMan
Reference to the owning Sync Manager (for access to offline control object)
@param aEmailServerEntry
The folder entry on the Message server that will be use to construct this folder object
@return The constructed CImapFolder object pushed onto the cleanup stack
*/
EXPORT_C CImapFolder* CImapFolder::NewLC(CImapSyncManager& aSyncMan, CMsvServerEntry& aServerEntry, TMsvEmailEntry& aEmailEntry, CImapSettings& aImapSettings, const TDesC& aFullFolderPath)
	{
	CImapFolder* self=new (ELeave) CImapFolder(aSyncMan, aServerEntry, aImapSettings);
	CleanupStack::PushL(self);

	// Non-trivial constructor
	self->ConstructL(aServerEntry, aEmailEntry, aFullFolderPath);
	return self;
	}

/**
Static factory constructor with cleanup

@param aSyncMan
Reference to the owning Sync Manager (for access to offline control object)
@param aEmailServerEntry
The folder entry on the Message server that will be use to construct this folder object
@return The constructed CImapFolder object
*/
EXPORT_C CImapFolder* CImapFolder::NewL(CImapSyncManager& aSyncMan, CMsvServerEntry& aServerEntry, TMsvEmailEntry& aEmailEntry, CImapSettings& aImapSettings, const TDesC& aFullFolderPath)
	{
	CImapFolder* self=NewLC(aSyncMan, aServerEntry, aEmailEntry, aImapSettings, aFullFolderPath);
	CleanupStack::Pop();
	return self;
	}

/**
The non-trival constructor

@param aEmailServerEntry
The folder entry on the Message server that will be use to construct this folder object
*/
void CImapFolder::ConstructL(CMsvServerEntry& /*aServerEntry*/, TMsvEmailEntry& aEmailEntry, const TDesC& aFullFolderPath)
	{
	iSessionFolderInfo = NULL;
	iMailboxId = aEmailEntry.Id();
	iMailboxSize = aEmailEntry.RemoteFolderEntries();
	iFolderIndex = new (ELeave) CImapFolderIndex();
	iSelection   = new (ELeave) CMsvEntrySelection();
	iSyncLimit = KImImapSynchroniseAll;

	// Set the Path of folder object
	SetFullFolderPathL(aFullFolderPath) ;
	
	// We're an active object...
	CActiveScheduler::Add(this);
	}

/**
Sets the full path name of the folder from the service entry downwards. Layout as on the remote IMAP Server.
Should only be use by the CImapSyncManager.

@param aFullFolderPath
The new path to use as the full path of the folder.
*/
void CImapFolder::SetFullFolderPathL(const TDesC& aFullFolderPath)
	{
	iFullFolderPath.Close(); // discard any previous data
	iFullFolderPath.CreateL(aFullFolderPath);
	}

/**
Processing message loop for the active object. Called by the active scheduler.
*/
void CImapFolder::DoRunL()
	{
	// Finish if any server errors.
	if(iStatus.Int()!=KErrNone)
		{
		Complete(iStatus.Int());
		return;
		}

	switch(iNextAction)
		{
	case CImapSyncManager::ESyncNew:
		{
		__LOG_TEXT(iSavedSession->LogId(), "ImapFolder: Starting SynchroniseNewL");				
		SynchroniseNewL();
		return;
		}
	case CImapSyncManager::ESyncFlags:
		{
		__LOG_TEXT(iSavedSession->LogId(), "ImapFolder: Syncing flags for local messages");

		// Whenever iSavedSession->FetchBodyStructureAndHeadersL() is called, this is the next step 
		// (although this step can be reached without calling FetchBodyStructureAndHeadersL())
		// This means that OnFetchLD() may have been called, in which case there are 
		// outstanding BULK entry operations that need to be completed here.
		iServerEntry.CompleteBulk();

		// If there were message flags that has been changed locally and need upadting on the remote server
		// then do those first.
		if(iSetSeenList)
			{
			if(ProcessSeenFlagsL(ESetSeenFlag))
				{
				return;
				}
			}

		if(iClearSeenList)
			{
			if(ProcessSeenFlagsL(EClearSeenFlag))
				{
				return;
				}
			}

		__LOG_TEXT(iSavedSession->LogId(), "ImapFolder: Syncing flags for remote messages");
		// Creates the list of old messages then update the flags for them
		GetMessageChildrenL(iMailboxId, iSelection);
		MakeSortedFolderIndexL(ETrue);
	
		if(iSelection->Count() > 0)
			{
			iHighestUid = (*iFolderIndex)[iSelection->Count()-1].iUid;
			}
		else
			{
			iHighestUid = 0;
			}

		TInt localcount = iFolderIndex->Size();
		iMatchingMessageIds.Reset();
		for(TInt localloop = 0; localloop < localcount; ++localloop)
			{
			TMsvId localid = (*iFolderIndex)[localloop].iUid;
			iMatchingMessageIds.AppendL(localid);
			}
			
		iMessageFlagInfoArray.Reset();
		
		HBufC8* sequenceSetOnHeap = CImapSession::CreateSequenceSetLC(iMatchingMessageIds);
		
		// Call the FETCHFLAGS function in the session
		iNextAction = CImapSyncManager::EEndSyncFlags;
		iSavedSession->FetchFlagsL(iStatus, *sequenceSetOnHeap, iMessageFlagInfoArray);
		SetActive();
		CleanupStack::PopAndDestroy(sequenceSetOnHeap);
		}
		break;		
	case CImapSyncManager::EEndSyncFlags:
		{
		__LOG_TEXT(iSavedSession->LogId(), "ImapFolder: Call to get the remote flags completed");
		// Loop through the messages array and update the flags for each UID returned
		// Get a clean version of the folder children
		TInt resultscount = iMessageFlagInfoArray.Count();
		for(TInt resultsloop = 0; resultsloop < resultscount; ++resultsloop)
			{
			TMessageFlagInfo *messageinfo = &(iMessageFlagInfoArray[resultsloop]);

			TInt localmsgcount = iFolderIndex->Size();
			// For each returned Uid go through the list of local index to find the matching TMsvId.
			for(TInt localloop = 0; localloop < localmsgcount; ++localloop)
				{
				if((*iFolderIndex)[localloop].iUid == messageinfo->MessageUid())
					{
					SetEntryL((*iFolderIndex)[localloop].iMsvId);
					TMsvEmailEntry entry = iServerEntry.Entry();

					TBool messageInfoSeen = messageinfo->QueryFlag(TMessageFlagInfo::ESeen);
					TBool oldEntrySeen = entry.SeenIMAP4Flag();

					// Are we configured to update the \seen flag on the server?
					// If so then we need to translate "read\unread" flags into "\\seen" or not "\\seen"
					if (iImapSettings.UpdatingSeenFlags())
						{
						// Make a note to update the servers \Seen flag if "read\unread" has CHANGED on the client
						// and is different to the server's version
						if (BoolsAreEqual(entry.Unread(), messageInfoSeen) && BoolsAreEqual(oldEntrySeen, messageInfoSeen))
							{
							if (entry.Unread())
								{
								AppendClearSeenL(entry.UID());
								}
							else
								{
								AppendSetSeenL(entry.UID());
								}								
							}
						}

					entry.SetSeenIMAP4Flag(messageInfoSeen);
					entry.SetFlaggedIMAP4Flag(messageinfo->QueryFlag(TMessageFlagInfo::EFlagged));
					entry.SetDeletedIMAP4Flag(messageinfo->QueryFlag(TMessageFlagInfo::EDeleted));
					entry.SetDraftIMAP4Flag(messageinfo->QueryFlag(TMessageFlagInfo::EDraft));
					entry.SetRecentIMAP4Flag(messageinfo->QueryFlag(TMessageFlagInfo::ERecent));
					
					// Are we configured to update the \seen flag on the server?
					if (iImapSettings.UpdatingSeenFlags())
						{
						// Now copy the inverse of the \Seen flag down to the phone's Unread flag 
						//  except when LastSyncSeen is set (ie when the client version is more up to date)
						//  This means that the client Read status ALWAYS reflects the IMAP \Seen state
						if (BoolsAreEqual(entry.Unread(), messageInfoSeen) 		&& BoolsAreNotEqual(oldEntrySeen, messageInfoSeen))
							{
							// Phone and server disagree on whether the message has been read.
							// And the seen flag on the server is different to that on the phone.
							// Which means that a change has happened on the server that the phone was not aware of.
							// So server is more up to date, and we take its value.
							entry.SetUnread(!messageInfoSeen);
							}
						}
					
					ChangeEntryBulkL(entry); // completed after the outer for loop
					break;
					}
				} // end of inner for loop
			}
			
		iServerEntry.CompleteBulk();
		
		iNextAction = CImapSyncManager::ESyncProcessSeenFlagsAndComplete;
		SetActive();
		CompleteSelf();
		}
		break;		
	case CImapSyncManager::ESyncProcessSeenFlagsAndComplete:
		{
		// If there were message flags that has been changed locally and need upadting on the remote server
		// then do those now.
		if(iSetSeenList)
			{
			if(ProcessSeenFlagsL(ESetSeenFlag))
				{
				return;
				}
			}

		if(iClearSeenList)
			{
			if(ProcessSeenFlagsL(EClearSeenFlag))
				{
				return;
				}
			}
			
		SyncCompleteL();
		Complete(iStatus.Int());
		}
		break;
	case CImapSyncManager::ESyncSearch:
		{
		if(iStatus == KErrNone) 
			{
			if(iSyncState == CImapSyncManager::ESyncSearch)
				{
				// Process the seach results to see if any messages ids are left.
				if(CheckAndProgressMessageSyncL())
					{
					// More messages and the search command has been sent so just return.
					return;
					}
				}
				
			// If the sync action is syncing old messages then go through the list of found ids
			// match it with the folder index and build a list of messages that are no longer on the server.
			if(iSyncState == CImapSyncManager::ESyncOld)
				{
				// At this point the remote command to search out folder UIDs has completed. This
				// list will contain all UIDs for messages in the remote folder. This may be too
				// many, and if so, the list is truncated to only include the N most recent 
				// messages in accordance with the synchronisation limit.

				TInt syncThresh = GetSyncThreshold();
				__LOG_FORMAT((iSavedSession->LogId(), "ImapFolder: Syncing old. syncThresh = %d", syncThresh));

				// Perform the check on the message ids to make sure that only messages that are within
				// the sync limit will be left in iDeletedMessageIds.
				CheckMessagesInRangeL(syncThresh);
				
				// Check the delete ids list and delete messages according to what's in the list	
				TInt deletedcount = iDeletedMessageIds.Count();
				// Remove message from the local server
				for(TInt deleteloop = 0; deleteloop < deletedcount; ++deleteloop)
					{
					__LOG_FORMAT((iSavedSession->LogId(), "ImapFolder: Local message (%d) deleting.", (*iFolderIndex)[iDeletedMessageIds[deleteloop]].iMsvId));
					DeleteMessageL((*iFolderIndex)[iDeletedMessageIds[deleteloop]].iMsvId);
					}

				// Trim the list down to the most recent UIDs consistant with the sync limit.
				if(iMatchingMessageIds.Count() > syncThresh)
					{
					for(TInt i = 0; i < syncThresh; ++i)
						{
						iMatchingMessageIds.Remove(0);
						}
					}

				// Get a clean version of the folder children
				GetMessageChildrenL(iMailboxId, iSelection);
				
				//update the progress object
				iMsgsToDo = iMatchingMessageIds.Count();
				
				MakeSortedFolderIndexL(ETrue);
			
				if(iSelection->Count() > 0)
					{
					iHighestUid = (*iFolderIndex)[iSelection->Count()-1].iUid;
					}
				else
					{
					iHighestUid = 0;
					}

				// At this point, the remote command to fetch all old messages has completed. 
				// Now we can look at fetching all new messages. 'iHighestUid' will contain the
				// highest UID of the old messages. The top entry in 'iSearchList' will contain
				// the highest UID in the remote folder. This gives us the range of UID to fetch
				// for new messages.

				// First check are there any new messages to fetch? If 'iHighestUid' is the highest
				// UID locally and remotely, then we finished sync'ing when we completed the old
				// sync.
				if(iMatchingMessageIds.Count() == 0)
					{
					__LOG_TEXT(iSavedSession->LogId(), "ImapFolder: Search List is empty");				
					}
				else if(iHighestUid < iMatchingMessageIds[iMatchingMessageIds.Count()-1])
					{
					TUint32 uidLow = iHighestUid;
					TUint32 uidHigh = iMatchingMessageIds[iMatchingMessageIds.Count()-1];

					// Only want new messages.
					uidLow++;

					// Are there only new messages (and no old)?
					if(iHighestUid == 0)
						{
						// Set this to ensure range is correct.
						uidLow = iMatchingMessageIds[0];
						}
					
					// Perform the new sync.
					__LOG_FORMAT((iSavedSession->LogId(), "ImapFolder: Synchronising new messages with range %d:%d", uidLow, uidHigh));
					SynchroniseRangeL(uidLow, uidHigh);
					return;
					}

				iSyncState = CImapSyncManager::ESyncNew;	
				}

			if((iSyncState == CImapSyncManager::ESyncNew) && (iImapSettings.SearchString().Length() == 0))
				{
				// Check for missing UIDs, ie. holes in the messages that we should have.
				CheckForMissingMessagesUidsL();

				// If there were any "missing" messages found during the sync that we should have
				// mirrored previously, get these now.
				if(iMissingMessageIds.Count() != 0)
					{
					__LOG_FORMAT((iSavedSession->LogId(), "ImapFolder: Missing messages detected %d", iMissingMessageIds.Count()));
					HBufC8* sequenceSetOnHeap = CImapSession::CreateSequenceSetLC(iMissingMessageIds);
					// Call the FETCH function in the session
					iSavedSession->FetchBodyStructureAndHeadersL(iStatus, sequenceSetOnHeap->Des(), KImapSmallHeaderFields(), *this);
					CleanupStack::PopAndDestroy(sequenceSetOnHeap);
					SetActive();

				 	iSyncState = CImapSyncManager::EFolderSynchronise;
				 	iNextAction = CImapSyncManager::ESyncFlags;
					return;
					}

				if(iSyncLimit <= KImImapSynchroniseAll)
					{
					__LOG_TEXT(iSavedSession->LogId(), "ImapFolder: Starting SynchroniseNewL");				
					SynchroniseNewL();
					return;
					}
				
				// If we haven't returned yet, then no new messages to sync
				// so just sync the flags of existing messages.
				iSyncState = CImapSyncManager::EFolderSynchronise;
			 	iNextAction = CImapSyncManager::ESyncFlags;
			 	SetActive();
			 	CompleteSelf();
				return;
				}
			else
				{
				SyncCompleteL();
				Complete(iStatus.Int());
				}
			}
		}
		break;
	case CImapSyncManager::EFolderExpunge:
		{
		// Call expunge to delete messages marked for delete
		__LOG_FORMAT((iSavedSession->LogId(), "ImapFolder: calling EXPUNGE in folder %S", &iFullFolderPath));

		iSyncState = CImapSyncManager::EFolderExpunge;
		iNextAction = CImapSyncManager::EFolderLocalExpunge;
		iSavedSession->ExpungeL(iStatus);
		SetActive();
		}
		break;
	case CImapSyncManager::EFolderClose:
		{
		//Permanently removes all messages that have the deleted flag set
		//from the currently selected mailbox
		iSyncState = CImapSyncManager::EFolderClose;
		iNextAction = CImapSyncManager::EFolderReSelect;
		iSavedSession->CloseL(iStatus);
		SetActive();
		}
		break;
	case CImapSyncManager::EFolderReSelect:
		{
		//Selecting a mailbox so that messages in the mailbox can be accessed. 
		iSyncState = CImapSyncManager::EFolderReSelect;
		iNextAction = CImapSyncManager::EFolderLocalExpunge;
		SelectL(iStatus, *iSavedSession);
		SetActive();
		}
		break;
	case CImapSyncManager::EFolderLocalExpunge:
		{
		// delete local messages locally
		__LOG_FORMAT((iSavedSession->LogId(), "ImapFolder: local message delete in folder %S", &iFullFolderPath));
		
		// Check the delete ids list and delete messages according to what's in the list	
		TInt deletedcount = iDeletedMessageIds.Count();
		// Remove message from the local server
		for(TInt deleteloop = 0; deleteloop < deletedcount; ++deleteloop)
			{
			__LOG_FORMAT((iSavedSession->LogId(), "ImapFolder: Local message (%d) deleting.", (*iFolderIndex)[iDeletedMessageIds[deleteloop]].iMsvId));
			DeleteMessageL((*iFolderIndex)[iDeletedMessageIds[deleteloop]].iMsvId);
			}
			
		// clear the count of expunged messages to prevent a sync triggering
		CImapFolderInfo* folderInfo = iSavedSession->SelectedFolderInfo();
		if (folderInfo->ExpungedMessages().Count()==deletedcount)
			{
			folderInfo->ResetExpungedMessages();

			// store updated remote mailbox size.
			iMailboxSize = folderInfo->Exists();
			SetEntryL(iMailboxId);
			TMsvEmailEntry entry = iServerEntry.Entry();
			entry.SetRemoteFolderEntries(folderInfo->Exists());
			ChangeEntryL(entry);
			}

		iSyncState = CImapSyncManager::EFolderLocalExpunge;
		iNextAction = CImapSyncManager::ENotSyncing;
		}
		// fall through...
	case CImapSyncManager::ENotSyncing:
		{
		iSyncState = CImapSyncManager::ENotSyncing;
		Complete(iStatus.Int());
		}
		break;
	default:
		break;
		}
	}

/**
Processing of the returned message ids after the completion of the IMAP SEARCH command.
If there are more messages ids to be look at then the SEARCH commands will be call again.
This call could set this folder object active

@return ETrue if the call has resulted in the folder object being made Active.
*/
TBool CImapFolder::CheckAndProgressMessageSyncL()
	{
	iFolderPosition += KImapUidSearchSize;

	__LOG_FORMAT((iSavedSession->LogId(), "ImapFolder: Session Search command completed: %d", iFolderPosition));

	// Check to see if we have all the messages.
	if(iFolderPosition >= iMailboxSize)
		{
		__LOG_TEXT(iSavedSession->LogId(), "ImapFolder: UID search complete");

		//update the progress object
		iMsgsToDo = iMatchingMessageIds.Count();

		iSyncState = CImapSyncManager::ESyncOld;
		// We have the full list of remote UIDs - fall through.
		}
	else
		{
		// Should be able to hit this code if KImapUidSearchSize is reduced to < the size
		// of the remote mailbox (iMailboxSize)
		// SearchString non 0 means a refined UID SEARCH is required
		
		if(iImapSettings.SearchString().Length() != 0)
			{
			// Refined search required
			// Still uses the indexes but appends user specified search criteria
			RBuf8 tempstr;
			tempstr.CleanupClosePushL();
					
			TPtrC8 searchstr = iImapSettings.SearchString();
			
			TInt tempstrLen = KImapFetchHeaderRangeSearch().Length() + KImapMaxIntChars + KImapMaxIntChars + searchstr.Length();
 			tempstr.CreateL(tempstrLen);			
			tempstr.Format(KImapFetchHeaderRangeSearch, iFolderPosition+1, Min(iMailboxSize,iFolderPosition+KImapUidSearchSize), &searchstr);
			
			__LOG_FORMAT((iSavedSession->LogId(), "ImapFolder: Refined UID search - get next manageable block of UIDs: %S", &tempstr));
			iSavedSession->SearchL(iStatus, tempstr, iMatchingMessageIds);
			
			// Go active
			SetActive();
			
			CleanupStack::PopAndDestroy(&tempstr);
			return ETrue;
			}
		else
			{
			// Normal unrefined SEARCH. Will pull back all the UIDs between indexes
			// Perform a UID search on this folder.
			RBuf8 tempstr;
			tempstr.CleanupClosePushL();
				
			TInt tempstrLen = KImapFetchHeaderRange().Length() + KImapMaxIntChars + KImapMaxIntChars;
			tempstr.CreateL(tempstrLen);
			tempstr.Format(KImapFetchHeaderRange, iFolderPosition+1, Min(iMailboxSize,iFolderPosition+KImapUidSearchSize));
			
			__LOG_FORMAT((iSavedSession->LogId(), "ImapFolder: Unrefined UID search - get next manageable block of UIDs: %S", &tempstr));
			iSavedSession->SearchL(iStatus, tempstr, iMatchingMessageIds);
			
			// Go active
			SetActive();
			
			CleanupStack::PopAndDestroy(&tempstr);
			return ETrue;
			}
		}
	return EFalse;
	}

/**
Sets number of messages to be sync. If there is a search string or the sync limit is outside
the mailbox size then everything will be sync. i.e. the sync limit will be set to 0.

*/
TInt CImapFolder::GetSyncThreshold()
	{
	// If no UID Search string defined then the old logic is used
	if((iImapSettings.SearchString().Length() == 0) && (iSyncLimit > KImImapSynchroniseAll))
		{	
		if(iSyncLimit < iMailboxSize)
			{
			return (iMailboxSize - iSyncLimit);
			}
		}

	return 0;
	}

/**
Go through the list of found message ids and work out if each message is within the sync limit
as defined by the user. If a message isn't within the limit it will be removed if it hasn't been fetched.

@param aSyncThreshold
The number of messages that will define the sync limit
*/
void CImapFolder::CheckMessagesInRangeL(TInt aSyncThreshold)
	{
	// At this stage (prior to truncation), we have a full list of all the message
	// UIDs in the remote folder. We also have a list of all UIDs for messages in
	// the local folder. With these lists, we can establish which local messages
	// have been orphaned and which have not using the following rules.

	// * If the remote message is no longer there:
	//		(1) Then the local message is orphaned (always).
	// * If the remote message is still there and it is not one of the N most recent:
	//		(2) If the local message has body parts do nothing. - This Check has been removed as per new CMail UI.
	//		(3) If the local message does not have body parts, then it is orphaned
	//		    unless is is a message we have selected for download.
	// * If the remote message is still there and it is one of the N most recent:
	//		(4) Do nothing.

	// Search through the local folder list while checking the remote folder list.

	TInt localcount = iFolderIndex->Size();
	TInt resultscount = iMatchingMessageIds.Count();

	iFolderPosition = 0;

	iDeletedMessageIds.Reset();
	
	for(TInt localloop = 0; localloop < localcount; ++localloop)
		{
		// Go through the list of index, if can't find matching in found ids then mark for delete.
		TInt resultsloop = 0;//listprogress;
		TBool removeThis = EFalse;
		TBool folderPositionFound = EFalse;

		while(resultsloop < resultscount)
			{
			TUint remoteUid = iMatchingMessageIds[resultsloop];
			TUint localUid = (*iFolderIndex)[localloop].iUid;

			// Check the synclimit
			TBool inSyncRange = (resultsloop >= aSyncThreshold);
			if((remoteUid > localUid) || (remoteUid > iHighestUid))
				{
				break;
				}
			if(remoteUid == localUid)
				{
				// Found id, sets new lower limit then exit loop
				folderPositionFound = ETrue;
				
				// Check for if message is in the syncrange
				if(!inSyncRange)
					{
					// Here the remote uid matches the local uid, but the message falls outside
					// of the N most recent messages. See cases (2) & (3).
					SetEntryL((*iFolderIndex)[localloop].iMsvId);
					TMsvEmailEntry message(iServerEntry.Entry());
					// Does message have any downloaded parts?
					// The local message does not have any body parts and
					// is not selected for download, so it is orphaned.
					// See case (3) above.
					__LOG_FORMAT((iSavedSession->LogId(), "ImapFolder: Local message (%d) is only header and not selected for download, deleting", (*iFolderIndex)[localloop].iMsvId));
					removeThis = ETrue;
					iOrphanedMessages++;
					}
				break;
				}

			++resultsloop;
			}
		if(!folderPositionFound || removeThis)
			{
			// Saved the index position of the message to be deleted from the local view
			__LOG_FORMAT((iSavedSession->LogId(), "ImapFolder: Local message (%d) marked for deleting, loop = %d", (*iFolderIndex)[localloop].iMsvId, localloop));
			iDeletedMessageIds.AppendL(localloop);
			}
		else
			{
				iMsgsDone++;
			}
			
		}
	
	}
	

/**
Go through the list of found remote message ids and work out if each message header has been fetched or not.
Messages that are missing from the local index will be added to a list of messages to be fetch.
*/
void CImapFolder::CheckForMissingMessagesUidsL()
	{
	iMissingMessageIds.Reset();

	if(iFolderIndex->Size() == 0)
		{
		return;
		}
		
	TInt remotecount = iMatchingMessageIds.Count();

	for(TInt remoteloop = 0; remoteloop < remotecount; ++remoteloop)
		{
		TUint remoteUid = iMatchingMessageIds[remoteloop];

		if(iFolderIndex->FindMsg(remoteUid) == 0)
			{
			iMissingMessageIds.AppendL(remoteUid);
			}
		}
	}
/**
Cancels any outstanding asynchronous service requests.
*/
void CImapFolder::DoCancel()
	{
	iSavedSession->Cancel();
	CMsgActive::DoCancel();
	}

/**
Called when the requested operation is completed.
Cleans up data members used only during synchronisation
*/
void CImapFolder::DoComplete(TInt& /*aStatus*/)
	{
	iCachedEntryData->Reset();
	iSelection->Reset();
	iFolderIndex->Reset();
	iMatchingMessageIds.Reset();
	iDeletedMessageIds.Reset();
	iMessageFlagInfoArray.Reset();

	// iSessionFolderInfo is not owned by this class,
	// Set to NULL as it should never be assumed to exist.
	iSessionFolderInfo = NULL;
	}

/**
Returns the full mailbox path description from the Message server

@return Pointer to the descriptor object
*/
EXPORT_C TDesC& CImapFolder::FullFolderPathL()
	{
	if(iFullFolderPath.Length() == 0)
		{
		HBufC16* temp = MakePathL(ETrue);
		iFullFolderPath.Close();
		iFullFolderPath.Assign(temp);
		}

	return iFullFolderPath;
	}

/**
Returns the mailbox uid from the Message server

@return The mailbox UID;
*/
EXPORT_C TMsvId CImapFolder::MailboxId()
	{
	return iMailboxId;
	}

/**
Issue the IMAP select command to select this folder using the supplied session.
The session must already have been opened and connected.

NOTE:  The client TRequestStatus is passed through to the CImapSession.
       Therefore a CImapSession::Cancel() must be called to cancel the
       SELECT operation.
NOTE:  UpdateSessionFolderInfoL() should be called following completion
	   of the select operation. And definitely prior to synchronising
	   the folder (see below)

@param aStatus 
The status request object to be use to send the completion signal. 
This is passed through to the session object.
@param aSession 
The session to be use for the SELECT command
*/
EXPORT_C void CImapFolder::SelectL(TRequestStatus& aStatus, CImapSession& aSession, TBool aSelectInbox)
	{
	// Construct the info object for the select command then just pass it on.
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	folderInfo->SetMsvId(iMailboxId);
	folderInfo->SetNameL(FullFolderPathL());
	
	// Issue the SELECT - the session takes immediate 
	// ownership of the folder info object.
	CleanupStack::Pop(folderInfo);
	aSession.SelectL(aStatus, folderInfo, aSelectInbox);
	}


/**
Issue the IMAP select command to select this folder using the supplied session.
The session must already have been opened and connected.

NOTE:  The client TRequestStatus is passed through to the CImapSession.
       Therefore a CImapSession::Cancel() must be called to cancel the 
       EXAMINE operation.
NOTE:  UpdateSessionFolderInfoL() should be called following completion
	   of the EXAMINE operation.

@param aStatus 
The status request object to be use to send the completion signal. 
This is passed through to the session object.
@param aSession 
The session to be use for the EXAMINE command
*/
EXPORT_C void CImapFolder::ExamineL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	// Construct the info object for the select command then just pass it on.
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	folderInfo->SetMsvId(iMailboxId);
	folderInfo->SetNameL(FullFolderPathL());
	
	// Issue the EXAMINE - the session takes immediate 
	// ownership of the folder info object.
	CleanupStack::Pop(folderInfo);
	aSession.ExamineL(aStatus, folderInfo);
	}

/**
Updates the remote folder mirror with status information about the folder 
on the remote server. Called at the start of the synchronisation process.

Panics if the folder is not selected folder on the passed session.

NOTE: MUST be called prior to synchronising (done internally)

NOTE: updates the context of the CMsvServerEntry to the folder.

@param aSession - the session that has SELECTed this folder
*/
EXPORT_C void CImapFolder::UpdateSessionFolderInfoL(CImapSession& aSession)
	{
	iSessionFolderInfo = aSession.SelectedFolderInfo();
	if(iSessionFolderInfo)
		{
		__ASSERT_ALWAYS(iSessionFolderInfo->MsvId()==iMailboxId, User::Invariant());
		
		iMailboxSize = iSessionFolderInfo->Exists();
		// Need to be save away. NOT Done on Session.
		SetEntryL(iMailboxId);
		TMsvEmailEntry entry = iServerEntry.Entry();
		entry.SetRemoteFolderEntries(iSessionFolderInfo->Exists());
		if(entry.UID() != iSessionFolderInfo->UidValidity())
			{
			entry.SetValidUID(EFalse);
			}
		entry.SetRecentIMAP4Flag(iSessionFolderInfo->Recent());
		entry.SetUnreadIMAP4Flag(iSessionFolderInfo->Unseen());
		ChangeEntryL(entry);
		}
	}

/**
This can be called at any time with a given IMAP session to find out if the folder has 
changed in any way since the last sync.

Panics if the folder is not selected folder on the passed session.

@return
Returns true if any of Exists, Recent or Expunged counts are non-zero.
*/
EXPORT_C TBool CImapFolder::Changed(CImapSession& aSession)
	{
	iSessionFolderInfo = aSession.SelectedFolderInfo();
	if(iSessionFolderInfo)
		{
		__ASSERT_ALWAYS(iSessionFolderInfo->MsvId()==iMailboxId, User::Invariant());

		// True if the exists count has changed
		// or if recent or expunged counts are non-zero
		TBool existChanged = (iMailboxSize != iSessionFolderInfo->Exists());
		TBool flagChanged = iSessionFolderInfo->MessageFlagsChanged();
		TBool otherChanged = (existChanged || 
			     				iSessionFolderInfo->Recent() || 
			     				iSessionFolderInfo->ExpungedMessages().Count());
			     				
		iFlagChangedOnly = (flagChanged && !(otherChanged));
		
		return ( otherChanged || flagChanged );
		}
	return EFalse;
	}

/**
Clears the counts that indicate that an event has occurred on the remote server
for the selected folder since the last sync operation.

This method is called during synchronisation such that subsequent changes are
identified.
*/
void CImapFolder::ClearChangeCounts()
	{
	if(iSessionFolderInfo)
		{
		// Do not clear exists count 
		iSessionFolderInfo->SetRecent(0);
		iSessionFolderInfo->ResetExpungedMessages();
		iSessionFolderInfo->SetMessageFlagsChanged(EFalse);
		}
	iFlagChangedOnly = EFalse;
	}

/** 
Set the current entry context on the Message server

@param aId
The entry id to set in the Message server
@leave KErrNotFound if the entry does not exist or KErrLocked if the entry is locked.
*/
void CImapFolder::SetEntryL(const TMsvId aId)
	{
	User::LeaveIfError(iServerEntry.SetEntry(aId));
	}

/**
Sets the context's index entry to the specified values on the Message server

@param  aEntry
The new details for the entry.
@leave KErrAccessDenied - the entry is read only (deleted entry, standard folder, or locked); 
KErrNotSupported - aEntry is invalid or the ID specified in aEntry is not the same as the context ID
or no context has been set for the object; 
KErrNoMemory - a memory allocation failed. 
*/
void CImapFolder::ChangeEntryL(const TMsvEntry& aEntry)
	{
	User::LeaveIfError(iServerEntry.ChangeEntry(aEntry));
	}

/**
Change entry in bulk mode (i.e no index file commit. no notify)

@param  aEntry
The new details for the entry.
@leave KErrAccessDenied - the entry is read only (deleted 
entry, standard folder, or locked); 
KErrNotSupported - aEntry is invalid or the 
ID specified in aEntry is not the same as the context ID or no context has been 
set for the object; 
KErrNoMemory - a memory allocation failed.
*/
void CImapFolder::ChangeEntryBulkL(const TMsvEntry& aEntry)
	{
	User::LeaveIfError(iServerEntry.ChangeEntryBulk(aEntry));
	}

/**
Get a list of ids of the children from the current context of the message server.

@param aSelection
A reference to the email entries selection object which will be filled with the ids of the child entries.
@leave Error from CMsvServerEntry::GetChildren.
*/
void CImapFolder::GetChildrenL(CMsvEntrySelection& aSelection)
	{
	User::LeaveIfError(iServerEntry.GetChildren(aSelection));
	}

/**
Delete a local message.

@param aMessage
The TMsvId of the message to be removed from the message server.
*/
EXPORT_C void CImapFolder::DeleteMessageL(const TMsvId aMessage)
	{
 	if(aMessage == KMsvNullIndexEntryId)
		{
		// Attempted delete of null entry
 		return;
		}
	// Delete message and all subparts: first, move to parent
	SetEntryL(aMessage);

	SetEntryL(iServerEntry.Entry().Parent());

	// Do not leave when entry is in use 
	TInt err (iServerEntry.DeleteEntry(aMessage));
	if(err == KErrInUse)
		{
		// Dont leave if err = KErrInUse
		}
	else
		{
		User::LeaveIfError(err);
		}
	}

/**
Add a message UID to the list of messages that should have the SEEN flag sets during the SYNC FLAGS stage.

@param aMessage
The UID of the message to be added to the list of messages to have the SEEN flag set.
*/
EXPORT_C void CImapFolder::AppendSetSeenL(const TUint32 aMessage)
	{
	if(iSetSeenList)
		{
		iSetSeenList->AppendL(aMessage);
		}
	else
		{
		iSetSeenList = new(ELeave) RArray<TUint>(4);
		iSetSeenList->AppendL(aMessage);
		}
	}
	
/**
Add a message UID to the list of messages that should have the SEEN flag cleared during the SYNC FLAGS stage.

@param aMessage
The UID of the message to be added to the list of messages to have the SEEN flag cleared.
*/
EXPORT_C void CImapFolder::AppendClearSeenL(const TUint32 aMessage)
	{
	if(iClearSeenList)
		{
		iClearSeenList->AppendL(aMessage);
		}
	else
		{
		iClearSeenList = new(ELeave) RArray<TUint>(4);
		iClearSeenList->AppendL(aMessage);
		}
	}

/**
Get MESSAGE ONLY children of a folder. Ignore shadows as they are not going to be synced against the server
*/
void CImapFolder::GetMessageChildrenL(const TMsvId aFolder, CMsvEntrySelection* aChildren)
	{	
	aChildren->Reset();
	// Get *all* the children
	SetEntryL(aFolder);
	GetChildrenL(*aChildren);

	if(iCachedEntryData)
	    {
        iCachedEntryData->Close();
	    }
	delete iCachedEntryData;
	iCachedEntryData = NULL;
	iCachedEntryData = new(ELeave) RArray<TMsvCacheData>(5);

	// Go through them, checking to see if they're messages and removing ones that aren't
	TInt pos = 0;
	while(pos < aChildren->Count())
		{
		TMsvEntry* entryPtr;
		TMsvId id = (*aChildren)[pos];
		User::LeaveIfError(iServerEntry.GetEntryFromId(id, entryPtr));

		// Is it a message? And is it real (not shadow)
		if(entryPtr->iType != KUidMsvMessageEntry ||
			entryPtr->iRelatedId != KMsvNullIndexEntryId )
			{
			// No, remove it
			aChildren->Delete(pos,1);
			}
		else
			{
			//cache two parts of the TMsvEntry data to avoid having to refind it later
			TMsvCacheData data;
			data.iOrphan = ((TMsvEmailEntry)(*entryPtr)).Orphan();
			data.iUid = ((TMsvEmailEntry)(*entryPtr)).UID();
			iCachedEntryData->AppendL(data);
			// Next entry
			pos++;
			}
		}
	}

/**
Populates the entry selection with messages that are eligible for auto-fetch.
Auto fetch is performed as a second synchronisation stage, following the header
synchronisation. Eligible messages are synchronised according to defined 
download rules. Note that this method does not filter the returned entry
selection according to these rules.

@return TInt - number of meesages in the updated selection
*/
EXPORT_C TInt CImapFolder::GetFetchMessageChildrenL(CMsvEntrySelection& aSelection)
	{	
	aSelection.Reset();
	// Get *all* the children
	SetEntryL(iMailboxId);
	GetChildrenL(aSelection);

	// Go through them, checking to see if they're messages and removing ones that aren't
	TInt pos = 0;
	TMsvEntry* entryPtr;
	while(pos < aSelection.Count())
		{
		TMsvId id = (aSelection)[pos];
		User::LeaveIfError(iServerEntry.GetEntryFromId(id, entryPtr));

		// Remove entry from the selection if:
		//   It is not a message
		//   It is not real (ie it is a shadow entry)
		//   If has been previously fetched.
		TBool previouslyFetched = ((TMsvEmailEntry*)entryPtr)->ValidUID();
		if(entryPtr->iType != KUidMsvMessageEntry ||
			entryPtr->iRelatedId != KMsvNullIndexEntryId ||
			previouslyFetched)
			{
			aSelection.Delete(pos,1);
			}
		else
			{
			++pos;
			}
		}
	
	return aSelection.Count();
	}

/**
Transfers the current selection into the iFolderIndex, and sorts it by UID.
*/
void CImapFolder::MakeSortedFolderIndexL(TBool aUseCachedEntryData)
	{
		
	TInt noofchildren = iSelection->Count();
	
	// Reset folder index
	iFolderIndex->SetSizeL(noofchildren);
	TInt acounter = 0;

	if(!aUseCachedEntryData)
		{ //can't rely on iCachedEntryData
		TMsvEntry* entryPtr;
		TMsvId id;
		for(acounter = 0; acounter < noofchildren; acounter++)
			{
			// Save UID/TMsvId of this entry
			id = (*iSelection)[acounter];
			User::LeaveIfError(iServerEntry.GetEntryFromId(id, entryPtr));
			(*iFolderIndex)[acounter].iUid = ((TMsvEmailEntry)(*entryPtr)).UID();
			(*iFolderIndex)[acounter].iMsvId = id;
			}
		}
	else
		{
		for(acounter = 0; acounter < noofchildren; acounter++)
			{
			// Save UID/TMsvId of this entry
			(*iFolderIndex)[acounter].iUid = (*iCachedEntryData)[acounter].iUid;
			(*iFolderIndex)[acounter].iMsvId = (*iSelection)[acounter];
			}
		}

	// Sort it by UID
	iFolderIndex->Sort();

	// Check for any duplicate UIDs (ie, a dud netscape server)
	TMsvEntry* entryPtr;
	TMsvEntry* nextEntryPtr;
	for(acounter = 1; acounter < noofchildren; acounter++)
		{
		if((*iFolderIndex)[acounter].iUid != 0 && (*iFolderIndex)[acounter].iUid == (*iFolderIndex)[acounter-1].iUid)
			{
			if(!aUseCachedEntryData)
				{
				// Get the TMsvEntry for the message/folder
				User::LeaveIfError(iServerEntry.GetEntryFromId((*iFolderIndex)[acounter].iMsvId,entryPtr));
				User::LeaveIfError(iServerEntry.GetEntryFromId((*iFolderIndex)[acounter-1].iMsvId,nextEntryPtr));
				// Check if type of TMsvEntry and type of next TMsvEntry are both Messages
				if( entryPtr->iType.iUid == nextEntryPtr->iType.iUid && entryPtr->iType.iUid == KUidMsvMessageEntryValue)
					{
					User::Leave(KErrCorrupt);
					}
				}
			else
				{
				User::Leave(KErrCorrupt);
				}
			}
			
		}
	}

/**
Synchronise the local view of the contents with that of the remote folder. 
If the folder is not the currently in SELECTED state on the CImapSesssion, 
the first step should be to issue a SELECT command via the IMAP Session
*/
EXPORT_C void CImapFolder::SynchroniseL(TRequestStatus& aStatus, CImapSession& aSession, TBool aNewOnly, TInt aDeleteOption)
	{
	__LOG_TEXT(aSession.LogId(), "SyncMan: Starting full IMAP Sync");
	
	//initialise counters
	iMsgsDone=0;
	iMsgsToDo=0;
	
	iHeadersFetched=0;
	iOrphanedMessages=0;
	iRemoteMessagesDeleteTagged=0;
	
	// Saved the calling status request
	iSavedSession = &aSession;
	iDeleteOption = aDeleteOption;
	
	// Set the Synchronisation states
	iSyncState = CImapSyncManager::ESynchronise;
	iNextAction = CImapSyncManager::ESynchronise;
	iMatchingMessageIds.Reset();
	
	// Update the remote folder info and clear the change indication counts
	UpdateSessionFolderInfoL(*iSavedSession);
	ClearChangeCounts();

	// Some pre-bits that need doing - get the children & count them
	// Get the folder info for the selected folder
	SetEntryL(iMailboxId);
	TMsvEmailEntry message = iServerEntry.Entry();
	GetMessageChildrenL(iMailboxId, iSelection);
	TInt noofchildren = iSelection->Count();

	// Check if new flags for the current selection should be cleared.
	ClearNewFlagsIfRequiredL();
		
	/*
	First of all check the UIDVALIDITY of the mirror and the 
	remote folder match - this is indicated by message.ValidUID() returning true.
	If not, we have to delete everything in the local mirror and start again.
	We also do this if there are 0 messages in the remote mailbox (0 EXISTS)
	and there are messages locally
	*/
	if(!message.ValidUID() || iMailboxSize == 0)
		{
		/*
		They don't match: do we have local children?
		If we were doing a new-only sync, change this to a full sync as the 
		UIDVALIDITY shows major changes.
		*/
		aNewOnly = EFalse;

		if(noofchildren)
			{
		    // We've got local children then delete them
			for(TInt a = 0; a < noofchildren; a++)
				{
				// We should be skipping locally generated messages. i.e. Offline operations
				DeleteMessageL((*iSelection)[a]);
				}

			// Reset the number of children as this may have changed.
			GetMessageChildrenL(iMailboxId, iSelection);
			noofchildren = iSelection->Count();
			}

		// Match the remote's UIDVALIDITY: 
		// reset the context as it may have been used by the deletion process.
		SetEntryL(iMailboxId);
		if(!message.ValidUID())
			{
			// Do the change if necessary
			message.SetUID(iSessionFolderInfo->UidValidity());
			message.SetValidUID(ETrue);
			ChangeEntryL(message);
			}
		}

	// Any remote messages? If not, complete now as there's nothing else to do
	if(iMailboxSize == 0)
		{
		// This folder is now sync'ed
		// No need to set seen flags as no messages in remote mailbox
		SyncCompleteL(); 
		Queue(aStatus);
		Complete(KErrNone);
		return;
		}

	// Start the synchronise with sync'ing old messages: are there any
	// messages in our mirror folder?
 	iSyncState = CImapSyncManager::ESyncOld; //EImapStateSynchroniseWait;
	iNextAction = CImapSyncManager::ESyncOld;

	iSomeUnread = EFalse;
	iHighestUid = 0;

	// Any children?
	iFolderIndex->Reset();
	if(noofchildren > 0) 
		{
		// Children exist, we need to do an old-sync to check all the messages
		// are still there.
		
		// Build an index of UIDs/TMsvIds currently in the mirror folder, and
		// sort this by UID: this is the order in which we expect the fetch to
		// return UIDs - any missing have been deleted on the server. They may
		// well not be in UID order in the index because locally-appended
		// messages will not have been added to the index in UID order.
		TRAPD(err,MakeSortedFolderIndexL(ETrue));
		if(err != KErrNone)
			{
			// Children exist, need to do old sync
			Queue(aStatus);
			Complete(err);
			return;
			}

		// Find the highest UID in the index
		iHighestUid = (*iFolderIndex)[noofchildren-1].iUid;
		}

	// Retrieve folder synchronisation limit.
	if(iSyncManager.EntryIsInbox(iServerEntry.Entry()))
		{
		// Leave iSyncLimit at the maximum if a Search String is set
		// If no Search String is set and this is the inbox, then use the inbox sync limit.
		if(iImapSettings.SearchString().Length() == 0)
			{
			iSyncLimit = iImapSettings.InboxSynchronisationLimit();	
			}
		}
	else
		{
		// Otherwise use the folder sync limit.
		// Leave iSyncLimit at the maximum if a Search String is set
		if(iImapSettings.SearchString().Length() == 0)
			{
			iSyncLimit = iImapSettings.MailboxSynchronisationLimit();
			}
		}
	
	// Call function to create and send the search command for the remote server message ids
	// if there wasn't a command sent then a full sync is needed.
	if(CreateAndSendUIDSearchStringL(aStatus))
		{
		return;
		}
		
	if(noofchildren > 0) 
		{
		// A complete list of the message ids on the remote server is needed.
		if(!aNewOnly && iHighestUid > 0)
			{
			// Do old sync
			iSyncState = CImapSyncManager::ESyncOld;
			iNextAction = CImapSyncManager::ESyncSearch;
			
			iFolderPosition = 0;
			// If a UID Search String is used it looks like this is FULL sync only 
			// so leave as is
			RBuf8 tempstr;
			tempstr.CleanupClosePushL();
			
			TInt tempstrLen = KImapFetchHeaderUIDRange().Length() + KImapMaxIntChars; // KImapFetchHeaderUIDRange provides enough room for "1"
			tempstr.CreateL(tempstrLen);
			tempstr.Format(KImapFetchHeaderUIDRange, 1, iHighestUid);

			__LOG_FORMAT((iSavedSession->LogId(), "CImapFolder SynchroniseL : Sending Search for old messages with no synclimits: %S", &tempstr));
			
			iSavedSession->SearchL(iStatus, tempstr, iMatchingMessageIds);
			// Go active and note that a send has been queued
			Queue(aStatus);
			SetActive();
			
			CleanupStack::PopAndDestroy(&tempstr);
			return;
			}
		}

	// There was no need to search for old ids hence do new sync
	SynchroniseNewL();
	Queue(aStatus);
	}

/**
Find the messages available on the remote server by creating and send the string of messages
to be search for by the session SearchL command.

@param aStatus
Reference to the request status object to be use for the Search command.
@return ETrue if the search command has been sent and the folder object has been made Active.
*/
TBool CImapFolder::CreateAndSendUIDSearchStringL(TRequestStatus& aStatus)
	{
	// Get the user defined UID SEARCH string and if there is one
	// do a refined search.
	if(iImapSettings.SearchString().Length() != 0)
		{
		iSyncState = CImapSyncManager::ESyncSearch;
		iNextAction = CImapSyncManager::ESyncSearch;
		
		iFolderPosition = 0;

		// Refined search
		RBuf8 tempstr;
		tempstr.CleanupClosePushL();
		
		TPtrC8 searchstr = iImapSettings.SearchString();
		
		TInt tempstrLen = KImapFetchHeaderRangeSearch().Length() + KImapMaxIntChars + searchstr.Length(); // KImapFetchHeaderRangeSearch provides enough room for "1"
		tempstr.CreateL(tempstrLen);		
		tempstr.Format(KImapFetchHeaderRangeSearch, 1, Min(iMailboxSize,KImapUidSearchSize), &searchstr);
		
		__LOG_FORMAT((iSavedSession->LogId(), "CImapFolder SynchroniseL : Sending Search for old messages with search string: %S", &searchstr));
		iSavedSession->SearchL(iStatus, tempstr, iMatchingMessageIds);
		
		// Go active and note that a send has been queued
		Queue(aStatus);
		SetActive();
		
		CleanupStack::PopAndDestroy(&tempstr);
		return ETrue;
		}
	else
		{
		// if no search string we use the old behaviour
		// Check the folder synchronisation limit.
		if(iSyncLimit > KImImapSynchroniseNone)
			{
			// Limited folder synchronisation, perform a UID search.
			iSyncState = CImapSyncManager::ESyncSearch;
			iNextAction = CImapSyncManager::ESyncSearch;
			
			iFolderPosition = 0;

			// Perform a UID search on this folder.
			RBuf8 tempstr;
			tempstr.CleanupClosePushL();
						
			TInt tempstrLen = KImapFetchHeaderRange().Length() + KImapMaxIntChars; // KImapFetchHeaderRange provides enough room for "1"
			tempstr.CreateL(tempstrLen);			
			tempstr.Format(KImapFetchHeaderRange, 1, Min(iMailboxSize,KImapUidSearchSize));
			
			__LOG_FORMAT((iSavedSession->LogId(), "CImapFolder SynchroniseL : Sending Search for old messages with synclimits: %S", &tempstr));
			iSavedSession->SearchL(iStatus, tempstr, iMatchingMessageIds);
			
			// Go active and note that a send has been queued
			Queue(aStatus);
			SetActive();
			
			CleanupStack::PopAndDestroy(&tempstr);			
			return ETrue;
			}
		else if(iSyncLimit == KImImapSynchroniseNone)
			{
			// No synchronisation required.
			// This folder is now sync'ed
			SyncCompleteL();
			Queue(aStatus);
			iSyncState = CImapSyncManager::ENotSyncing;
			// iSyncLimit=KImImapSynchroniseNone, no sync required
			Complete(KErrNone);
			return ETrue;
			}
		// iSyncLimit <= KImImapSynchroniseAll so Full synchronisation required - fall through.
		}
	return EFalse;
	}
	
/**
Synchronise new messages for a given range.
*/
void CImapFolder::SynchroniseRangeL(const TUint32 aLowUid,const TUint32 aHighUid)
	{
  
 	iSyncState = CImapSyncManager::EFolderSynchronise;
 	iNextAction = CImapSyncManager::ESyncFlags;
	iFolderPosition = 0;

	// First, resize folder index to hold all messages in the folder,
	// as opposed to the old sync list. This will preserve the old
	// contents of the index, which is what we want as it's up-to-date
	// and correct.
	iFolderIndex->SetSizeL(iMailboxSize);
	
	// Create list of priority fields to request
	// If a UID search string has been specified, the we should create the UID FETCH
	// string from the UID integer list.
	if(iImapSettings.SearchString().Length() != 0)
		{
		HBufC8* sequenceSetOnHeap = CImapSession::CreateSequenceSetLC(iMatchingMessageIds);
		iSavedSession->FetchBodyStructureAndHeadersL(iStatus, *sequenceSetOnHeap, KImapSmallHeaderFields(), *this);
		CleanupStack::PopAndDestroy(sequenceSetOnHeap);
		}
	else
		{
		RBuf8 tempstr;
		tempstr.CleanupClosePushL();
		
		TInt tempstrLen = KImapFetchHeaderRange().Length() + KImapMaxIntChars + KImapMaxIntChars;
		tempstr.CreateL(tempstrLen);		
		tempstr.Format(KImapFetchHeaderRange, aLowUid, aHighUid);
		
		iSavedSession->FetchBodyStructureAndHeadersL(iStatus, tempstr, KImapSmallHeaderFields(), *this);
		
		CleanupStack::PopAndDestroy(&tempstr);
		}
	SetActive();
	}

/**
Synchronise new mesasges from current highest UID to end.
*/
void CImapFolder::SynchroniseNewL()
	{
	iSyncState = CImapSyncManager::ESyncNew;
	iNextAction = CImapSyncManager::ESyncFlags;
	iFolderPosition = 0;

	// First, resize folder index to hold all messages in the folder,
	// as opposed to the old sync list. This will preserve the old
	// contents of the index, which is what we want as it's up-to-date
	// and correct.
	iFolderIndex->SetSizeL(iMailboxSize);

	// Fetch just the header of the new mails
	RBuf8 tempstr;
	tempstr.CleanupClosePushL();
	
	TInt tempstrLen = KImapFetchHeaderToEnd().Length() + KImapMaxIntChars;
	tempstr.CreateL(tempstrLen);		
	tempstr.Format(KImapFetchHeaderToEnd, iHighestUid + 1);
	
	iSavedSession->FetchBodyStructureAndHeadersL(iStatus, tempstr, KImapSmallHeaderFields(), *this);
	SetActive();
	
	CleanupStack::PopAndDestroy(&tempstr);
	}

/**
Update iDate in iMailboxId to show the time now (last sync time)
*/
void CImapFolder::SyncCompleteL()
	{
	__LOG_TEXT(iSavedSession->LogId(), "ImapFolder: Starting SyncCompleteL");
	// Find entry
	SetEntryL(iMailboxId);
	TMsvEmailEntry message = iServerEntry.Entry();

	// Find 'now'
	message.iDate.UniversalTime();

	// Check to see if there has been a change in the number of messages in the remote folder.
	TBool folderSizeChanged = (message.RemoteFolderEntries()!=iMailboxSize);

	// Set 'unread' flag on folder if there are any unread messages within it
	if(message.Unread() != iSomeUnread || !message.Visible() || folderSizeChanged)
		{
		// Update flags
		message.SetUnread(iSomeUnread);
		message.SetVisible(ETrue);
		message.SetRemoteFolderEntries(iMailboxSize);
 		ChangeEntryBulkL(message); // completed at the end of this method
		}

	// we need to ensure the hierarchy of folders containing this one
	// is now visible. Note previously this incorrectly only did this
	// when we were not in DisconncetedUserMode
	do
		{
		// Move up one
		SetEntryL(message.Parent());
		message=iServerEntry.Entry();

		// Ensure visibility
		if(!message.Visible())
			{
			message.SetVisible(ETrue);
			ChangeEntryBulkL(message); // completed at the end of this method
			}
		}
	while(message.iType!=KUidMsvServiceEntry);
	

	// commit any outstanding entries to the index file to complete the bulk
	// synchronization operation
	iServerEntry.CompleteBulk();
	
	// Set the current id to null so that we aren't locking any folders
	SetEntryL(KMsvNullIndexEntryId);
	}

/**
Reset subscription flags for all children, and recurse into folders
*/
void CImapFolder::ResetSubscriptionFlagsL(const TMsvId aFolder)
	{
	// Do this one
	SetEntryL(aFolder);
	TMsvEmailEntry entry = iServerEntry.Entry();

	// A folder or service? If not, return
	if(entry.iType != KUidMsvServiceEntry &&
		entry.iType != KUidMsvFolderEntry)
		{
		return;
		}

	// Reset flag if needed
	if(entry.Subscribed())
		{
		// Reset flag and save
		entry.SetSubscribed(EFalse);
		ChangeEntryL(entry);
		}

	// Any children?
	CMsvEntrySelection *children = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(children);
	GetChildrenL(*children);
	if(children->Count())
		{
		// Do each in turn
		for(TInt child = 0; child < children->Count(); child++)
			ResetSubscriptionFlagsL((*children)[child]);
		}
	CleanupStack::PopAndDestroy();
	}

/**
Performs any outstanding offline delete operations

deletes from the remote server any messages marked /deleted locally.
*/
EXPORT_C void CImapFolder::SyncDeletesL(TRequestStatus& aStatus, CImapSession& aSession)
	{
	iSavedSession = &aSession;

	SetEntryL(iMailboxId);
	GetMessageChildrenL(iMailboxId, iSelection);
	TRAPD(err,MakeSortedFolderIndexL(ETrue));
	if(err!=KErrNone)
		{
		Queue(aStatus);
		Complete(err);
		return;
		}
	TInt pos = 0;
	TInt deleted = 0;

	// Build command
	HBufC8* command=HBufC8::NewLC(256);
	RArray<TUint>	deletingMessageIds;
	
	// Start command
	//command->Des().Append(_L8("UID STORE "));
	
	iDeletedMessageIds.Reset();
	iMessageFlagInfoArray.Reset();

	while(pos < iFolderIndex->Size())
		{
		// Look for messages with deleted flag set
		SetEntryL((*iFolderIndex)[pos].iMsvId);
		if(((TMsvEmailEntry)iServerEntry.Entry()).DeletedIMAP4Flag())
			{
			__LOG_FORMAT((aSession.LogId(), "Message id %x marked as deleted",iServerEntry.Entry().Id()));
			++iRemoteMessagesDeleteTagged;
			
			// Append to the delete list
			TInt64 uid=(TUint)((TMsvEmailEntry)iServerEntry.Entry()).UID();
			deletingMessageIds.AppendL(uid);
			// index of local message in iFolderIndex to be deleted
			iDeletedMessageIds.AppendL(pos);
			++deleted;
			}

		// Next message
		pos++;
		}

	// Anything deleted?
	if(deleted)
		{
		// Append flags & send command
		_LIT8(KDeleteFlag,"+FLAGS");
		_LIT8(KDeleteItem,"(\\Deleted)");
		command->Des().Append(KDeleteFlag);
		
		HBufC8* sequenceSetOnHeap = CImapSession::CreateSequenceSetLC(deletingMessageIds);
		// Call the STORE function in the session
		aSession.StoreL(iStatus, *sequenceSetOnHeap, KDeleteFlag(), KDeleteItem(), ETrue, iMessageFlagInfoArray);
		CleanupStack::PopAndDestroy(sequenceSetOnHeap);
		
		Queue(aStatus);
		SetActive();
		iSyncState = CImapSyncManager::EFolderEarlyExpunge;
				
		if (iImapSettings.UseExpunge())
			{
			iNextAction = CImapSyncManager::EFolderExpunge;
			}
		else
			{
			iNextAction = CImapSyncManager::EFolderClose;
			}
		}
	else
		{
		// Nothing to do just complete
		Queue(aStatus);
		Complete(KErrNone);
		}

	// Get rid of command buffer
	CleanupStack::PopAndDestroy(command);
	}

/**
Enquote a string (being sent as a string literal) if required
*/
EXPORT_C void CImapFolder::DoQuoteL(HBufC16*& aBuffer)
	{
	// Null string? Nothing to do
	if(!aBuffer->Length() || !aBuffer->Des().Length())
		{
		return;
		}

	// Anything needing quoting in there?
	if(aBuffer->Des().Locate('\\') == KErrNotFound &&
		aBuffer->Des().Locate('\"') == KErrNotFound)
		{
		return;
		}

	// Run through string, inserting quote characters as needed
	for(TInt a = 0; a<aBuffer->Des().Length(); a++)
		{
		if(aBuffer->Des()[a] == '\\' || aBuffer->Des()[a] == '\"')
			{
			HBufC16 *newbuf = aBuffer->ReAllocL(aBuffer->Des().Length()+1);

			// Been moved due to realloc?
			if(newbuf != aBuffer)
				{
				// In all cases when DoQuoteL() is called, the buffer is on the top of
				// the cleanup stack: change this to indicate the correct entry
				CleanupStack::Pop();
				CleanupStack::PushL(aBuffer = newbuf);
				}

			aBuffer->Des().Insert(a, KQuoteChar);
			a++;
			}
		}
	}


/**
Implementation of the observer function for the session fetch command. For each call
creates the required entry tree.
*/
void CImapFolder::OnFetchLD(CImapFetchResponse* aImapFetchResponse)
	{
	// Take ownership of parameter
	CleanupStack::PushL(aImapFetchResponse);
	
	CImapRfc822HeaderFields* headerinfo = aImapFetchResponse->HeaderFields();
	CImapBodyStructure* bodystructure = aImapFetchResponse->BodyStructure();
	
	//update the progress object
	++iMsgsDone;
	++iHeadersFetched;

	if(headerinfo)
		{
		TUint remoteUid = aImapFetchResponse->MessageUid();
		if(iFolderIndex->Size() > 0)
			{
			if(iFolderIndex->FindMsg(remoteUid) != 0)
				{
				CleanupStack::PopAndDestroy(aImapFetchResponse);
				return;
				}
			}
		// Create an email entry in this folder.
		SetEntryL(iMailboxId);

		// Skeleton for new entry
		TMsvEmailEntry entry;
		TFileName attachmentFilename;	//	Attachment filename

		entry.iSize = 0;
		entry.iType = KUidMsvMessageEntry;
		entry.iMtm = KUidMsgTypeIMAP4;
		entry.iServiceId = iImapSettings.ServiceId();
		entry.SetValidUID(EFalse);		// reuse ValidUID Flag record if the message has ever been fetched
		entry.SetComplete(EFalse);
		entry.SetUnread(ETrue);
		entry.SetNew(ETrue);
		entry.SetUID(aImapFetchResponse->MessageUid());

		// Set from line in TMsvEntry
		const TDesC8& temp2 = headerinfo->FieldValue(CImapRfc822HeaderFields::EImapFrom);
		HBufC* decodedFromBuffer = HBufC::NewLC(temp2.Length());
		TPtr decodedFromPtr(decodedFromBuffer->Des());

		iHeaderConverter.DecodeHeaderFieldL(temp2, decodedFromPtr);
		entry.iDetails.Set(decodedFromPtr);
		
		// Set subject in TMsvEntry
		const TDesC8& temp3 = headerinfo->FieldValue(CImapRfc822HeaderFields::EImapSubject); 
		HBufC* decodedSubjectBuffer = HBufC::NewLC(temp3.Length());
		TPtr decodedSubjectPtr(decodedSubjectBuffer->Des());

		iHeaderConverter.DecodeHeaderFieldL(temp3, decodedSubjectPtr);
		entry.iDescription.Set(decodedSubjectPtr);

		// Set the Date
		entry.iDate = headerinfo->Date();

		// Set the priority field
		entry.SetPriority(headerinfo->PriorityL());

		if (bodystructure)
			{
			SetMessageFlagsL(entry, bodystructure);
			}

		// Set the flags
		TBool messageInfoSeen = aImapFetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::ESeen);

		entry.SetSeenIMAP4Flag(messageInfoSeen);
		entry.SetAnsweredIMAP4Flag(aImapFetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EAnswered));
		entry.SetFlaggedIMAP4Flag(aImapFetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EFlagged));
		entry.SetDeletedIMAP4Flag(aImapFetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EDeleted));
		entry.SetDraftIMAP4Flag(aImapFetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EDraft));
		entry.SetRecentIMAP4Flag(aImapFetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::ERecent));
		
		// Are we configured to update the \seen flag on the server?
		if (iImapSettings.UpdatingSeenFlags())
			{
			// Now copy the inverse of the \Seen flag down to the clients Unread flag
			if (messageInfoSeen)
				{
				entry.SetUnread(EFalse);
				}
			}
		
		// note that an unread message has been spotted.
		if(!entry.SeenIMAP4Flag() || entry.RecentIMAP4Flag())
			{
			iSomeUnread	= ETrue;
			}
		
		// If sync'ing to download rules is disabled, mark that message as
		// "fetched"  using the re-used ValidUID flag. This prevents the
		// message from being sync'd according to download rules if rules
		// are subsequently enabled.
		if(!iImapSettings.UseSyncDownloadRules())
			{
			entry.SetValidUID(ETrue);
			}

		// Create message
		User::LeaveIfError(iServerEntry.CreateEntryBulk(entry));
		// The matching CompleteBulk() is called in OnFetchCommit() and the ESyncFlags section of the DoRunL()
		// that is called after FetchBodyStructureAndHeadersL() has completed.
		__ASSERT_DEBUG(iNextAction == CImapSyncManager::ESyncFlags, User::Invariant());
		CleanupStack::PopAndDestroy(decodedSubjectBuffer);
		CleanupStack::PopAndDestroy(decodedFromBuffer);
		}
		
	CleanupStack::PopAndDestroy(aImapFetchResponse);
	}
	
void CImapFolder::OnFetchCommit()
	{
	iServerEntry.CompleteBulk();
	}

/**
Set or clear the \\Seen flags on the server

@param aUpdateMode
ETrue -> Sets the flag
@return False if no messages need to be processed
*/
TBool CImapFolder::ProcessSeenFlagsL(TSeenFlagUpdateMode aUpdateMode)
	{
	RArray<TUint>* pendingList;
	TBool settingFlag = (aUpdateMode == ESetSeenFlag);

	// Point pendingList to the correct list
	pendingList = (settingFlag ? iSetSeenList: iClearSeenList);
	
	// Exit if nothing to process
	if(!pendingList->Count())
		{
		return EFalse;
		}
	
	__LOG_FORMAT((iSavedSession->LogId(), "CImapFolder : ProcessSeenFlags(%d)", aUpdateMode));

	_LIT8(KStoreFlagsSetCommand, "+FLAGS");
	_LIT8(KStoreFlagsClearCommand, "-FLAGS");
	_LIT8(KStoreFlagsSeenCommand,"(\\Seen)");
	
	iMessageFlagInfoArray.Reset();

	HBufC8* sequenceSetOnHeap = CImapSession::CreateSequenceSetLC(*pendingList);

	// Call the STORE function in the session
	if(settingFlag)
		{
		iSavedSession->StoreL(iStatus, *sequenceSetOnHeap, KStoreFlagsSetCommand(), KStoreFlagsSeenCommand(), ETrue, iMessageFlagInfoArray);
		}
	else
		{
		iSavedSession->StoreL(iStatus, *sequenceSetOnHeap, KStoreFlagsClearCommand(), KStoreFlagsSeenCommand(), ETrue, iMessageFlagInfoArray);
		}
	
	CleanupStack::PopAndDestroy(sequenceSetOnHeap);
	
	SetActive();

	// Reset the list
	pendingList->Reset();

	return ETrue;
	}

/**
Construct a full mailbox path for the folder object
This is expensive in memory movement terms, as it works UP the path,
inserting new data at the start. This is based on the principle that it's
more expensive to find an entry in the index with SetEntryL() than it is to
move some bytes about, otherwise we'd find the path upwards then create the
string downwards.
*/
EXPORT_C HBufC16* CImapFolder::MakePathL(const TBool aIncludeLeaf)
	{
	// Making a path: we start with nothing
	HBufC16 *path = HBufC16::NewLC(256);
	TBool skipfirst = ETrue;
	TMsvId traverse = iMailboxId;

	// Move to the entry
	User::LeaveIfError(iServerEntry.SetEntry(traverse));

	// Skipping the leaf?
	if(!aIncludeLeaf && iServerEntry.Entry().iType != KUidMsvServiceEntry)
		{
		// Up a level before we generate the path
		traverse = iServerEntry.Entry().Parent();
		User::LeaveIfError(iServerEntry.SetEntry(traverse));
		}

	// Check and see if we are dealing with the INBOX, in which case return immediately
	if (iSyncManager.EntryIsInbox(iServerEntry.Entry()))
		{
		path->Des().Insert(0,KIMAP_INBOX);
		CleanupStack::Pop(path);
		return path;
		}

	// While we can still go up within this service...
	while(iServerEntry.Entry().iType != KUidMsvServiceEntry)
		{
		// Make sure the path maxlength is still ok with the added folder name and an extra separator character
		if((path->Length() + iServerEntry.Entry().iDetails.Length() + 1) > path->Des().MaxLength())
			{
			HBufC16* newpath = path->ReAllocL(path->Length() + iServerEntry.Entry().iDetails.Length() + 1);
			if(path != newpath)
				{
				CleanupStack::Pop(path);
				path = newpath;
				CleanupStack::PushL(path);
				}
			}

		// Add the name of this component to the path
		if(!skipfirst)
			{
			path->Des().Insert(0,iImapSettings.PathSeparator());
			}
		else
			{
			skipfirst = EFalse;
			}

		// Ensure uppercase 'INBOX' is used in folder name. This allows case
		// sensitive searches to be used later.
		if (iSyncManager.EntryIsInbox(iServerEntry.Entry()))
			{
			path->Des().Insert(0,KIMAP_INBOX);
			}
		else
			{
			path->Des().Insert(0,iServerEntry.Entry().iDetails);
			}

		// Go up a level
		SetEntryL(traverse = iServerEntry.Entry().Parent());
		}
	
	// Add the path at the very start, if it exists
	if(iImapSettings.FolderPath().Length())
		{
		// Make sure the path maxlength is still ok with the added folder path and an extra separator character
		if((path->Length() + iImapSettings.FolderPath().Length() + 1) > path->Des().MaxLength())
			{
			HBufC16* newpath = path->ReAllocL(path->Length() + iImapSettings.FolderPath().Length() + 1);
			if(path != newpath)
				{
				CleanupStack::Pop(path);
				path = newpath;
				CleanupStack::PushL(path);
				}
			}

		// Anything there already? If not, don't bother with the separator
		if(path->Des().Length()) 
			{
			path->Des().Insert(0,iImapSettings.PathSeparator());	
			}
		
		RBuf tempstr;
		tempstr.CleanupClosePushL();
		tempstr.CreateL(iImapSettings.FolderPath().Length());
		
		tempstr.Copy(iImapSettings.FolderPath());
		path->Des().Insert(0, tempstr);
		
		CleanupStack::PopAndDestroy(&tempstr);
		}

	// Pop it off cleanup stack
	CleanupStack::Pop(path);

	// Return the path
	return(path);
	}

void CImapFolder::CompleteSelf()
	{
// Complete self.
	TRequestStatus* status = &iStatus;
	iStatus = KRequestPending;
	User::RequestComplete(status, KErrNone);
	}

/**
Returns updated progress information on outstanding synchronisation operations.
*/
EXPORT_C void CImapFolder::Progress(TImap4SyncProgress& aProgress)
	{
	//copy values from member progress ob into aProgress
	aProgress.iMsgsDone=iMsgsDone;
	aProgress.iMsgsToDo=iMsgsToDo;
	
	aProgress.iHeadersFetched = iHeadersFetched;	
	aProgress.iOrphanedMessages = iOrphanedMessages;
	aProgress.iRemoteMessagesDeleteTagged = iRemoteMessagesDeleteTagged;
	
	if(iImapSettings.SearchString().Length() != 0)
		{
		aProgress.iMsgsToDo=iMailboxSize;	
		}
	else
		{
		aProgress.iMsgsToDo=(iSyncLimit<=0)?iMailboxSize:Min(iMailboxSize,iSyncLimit);	
		}
	aProgress.iMsgsDone = Min(iMsgsDone,aProgress.iMsgsToDo);	
		
	}

/**
Sets various flags in the message entry by doing a quick scan of the
bodystructure. This routine is intended to be relatively straightforward
so as not to impact performance during the sync phase too much.

@param aEntry Entry to set flags for
@param aBodyStructure Message body structure 
*/
void CImapFolder::SetMessageFlagsL(TMsvEmailEntry& aEntry, CImapBodyStructure* aBodyStructure)
	{
	TBool hasAttachments(EFalse);
	TBool hasHtml(EFalse);
	TBool afterRelated(EFalse);
	TBool afterAlternative(EFalse);
	TBool htmlAfterAltRel(EFalse);
	TBool hasICalendar(EFalse);
	TBool hasVCalendar(EFalse);
	TInt size(0);

	// Check if top level of message contains attachment type
	if ((aBodyStructure->BodyStructureType() == CImapBodyStructure::ETypeBasic) &&
	    ((aBodyStructure->Type().CompareF(KImapTxtImage) == 0) ||
	     (aBodyStructure->Type().CompareF(KImapTxtAudio) == 0) ||
	     (aBodyStructure->Type().CompareF(KImapTxtVideo) == 0) ||
	     (aBodyStructure->Type().CompareF(KImapTxtApplication) == 0)))
		{
		hasAttachments = ETrue;
		}

	RPointerArray<CImapBodyStructure> bodyStructureStack;
	bodyStructureStack.AppendL(aBodyStructure);

	TInt count;

	// A body structure stack is maintained. This avoids the use of recursion
	// when processing the embedded body structure list at each level, and also
	// maintains the order of processing that the old IMAP implementation used.
	// As nobody could explain exactly why the old code used this order, it was
	// felt that it was safer that the new implementation matched the old.
	while (bodyStructureStack.Count() > 0)
		{
		GetFlagsForBodyStructurePart(bodyStructureStack[0], hasAttachments, hasHtml, afterRelated, afterAlternative, htmlAfterAltRel, hasICalendar, hasVCalendar, size);

		for (count = 0; count < bodyStructureStack[0]->EmbeddedBodyStructureList().Count(); ++count)
			{
			bodyStructureStack.AppendL(bodyStructureStack[0]->EmbeddedBodyStructureList()[count]);
			}

		bodyStructureStack.Remove(0);
		}

	aEntry.SetAttachment(hasAttachments);
	aEntry.SetMHTMLEmail(hasHtml || htmlAfterAltRel);
	aEntry.SetICalendar(hasICalendar);
	aEntry.SetVCalendar(hasVCalendar);
	aEntry.iSize = size;
	bodyStructureStack.Reset();
	}

/**
Gets a set of flags for a body structure part

@param aBodyStructure Body structure part
@param aHasAttachments Flag to indicate if message contains attachments
@param aHasHtml Flag to indicate if message has HTML part
@param aAfterRelated Flag to indicate a multipart/related part has been found
@param aAfterAlternative Flag to indicate a multipart/alternative part has been found
@param aHtmlAfterAltRel Flag to indicate a HTML part has been found after a multipart/related or multipart/alternative part
@param aHasICalendar Flag to indicate message contains ICalendar
@param aHasVCalendar Flag to indicate message contains VCalendar
@param aSize Running total of size of message
*/
void CImapFolder::GetFlagsForBodyStructurePart(CImapBodyStructure* aBodyStructure,
                                               TBool& aHasAttachments, TBool& aHasHtml,
                                               TBool& aAfterRelated, TBool& aAfterAlternative,
                                               TBool& aHtmlAfterAltRel, TBool& aHasICalendar,
                                               TBool& aHasVCalendar, TInt& aSize)
	{
	switch (aBodyStructure->BodyStructureType())
		{
		case CImapBodyStructure::ETypeMultipart:
			{
			if (aBodyStructure->SubType().CompareF(KImapTxtRelated) == 0)
				{
				aAfterRelated = ETrue;
				}
			else if (aBodyStructure->SubType().CompareF(KImapTxtAlternative) == 0)
				{
				aAfterAlternative = ETrue;
				}
			else if (aBodyStructure->SubType().CompareF(KImapTxtMixed) == 0)
				{
				aHasAttachments = ETrue;
				}
			
			break;
			}

		case CImapBodyStructure::ETypeText:
			{
			if (aBodyStructure->SubType().CompareF(KImapTxtHtml) == 0)
				{
				if (aBodyStructure->ExtDispositionName().CompareF(KImapTxtAttachment) != 0)
					{
					aHasHtml = ETrue;
					}

				if (aAfterRelated || aAfterAlternative)
					{
					aHtmlAfterAltRel = ETrue;
					}
				}
			else if (aBodyStructure->SubType().CompareF(KImapTxtCalendar) == 0)
				{
				aHasICalendar = ETrue;
				}
			else if (aBodyStructure->SubType().CompareF(KImapTxtXVCalendar) == 0)
				{
				aHasVCalendar = ETrue;
				}

			break;
			}

		case CImapBodyStructure::ETypeMessageRfc822:
			{
			return;
			}

		default:
			{
			if (aBodyStructure->SubType().CompareF(KImapTxtDeliveryStatus) == 0)
				{
				aHasAttachments = ETrue;
				}

			break;
			}
		}

	// Add size of this body part to the running total
	TInt size(0);
	TLex8 lex(aBodyStructure->BodySizeOctets());
	lex.Val(size);

	// For Base64, use the pre encoding data size
	if (aBodyStructure->BodyEncoding().CompareF(KImapTxtBase64) == 0)
		{
		size = (size * 3) / 4;
		}

	aSize += size;
	}

/**
Checks if we need to clear the new flags on the messages in the folder,
and if so clears them.
@pre Current selection (iSelection) contains all the messages in the folder.
*/
void CImapFolder::ClearNewFlagsIfRequiredL()
	{
	if (iSyncManager.EntryIsInbox(iServerEntry.Entry()))
		{
		if (iSyncManager.InboxClearNewFlags())
			{
			__LOG_TEXT(iSavedSession->LogId(), "CImapFolder: Clearing new flags (inbox)");
			
			// Change attributes on the current selection
			User::LeaveIfError(iServerEntry.ChangeAttributes(*iSelection, 0, KMsvNewAttribute));

			// Set the flag to False to indicate that we have cleared the flags
			// on the inbox, and so any subsequent synchronise of the inbox
			// will not clear them again.
			iSyncManager.ResetInboxClearNewFlags();
			}
		}
	else
		{
		if (iSyncManager.NonInboxClearNewFlags())
			{
			__LOG_TEXT(iSavedSession->LogId(), "CImapFolder: Clearing new flags (non inbox)");

			// Change attributes on the current selection
			User::LeaveIfError(iServerEntry.ChangeAttributes(*iSelection, 0, KMsvNewAttribute));
			
			// Note that we do not clear the flag here as it will be required
			// for any subsequent non inbox folders that are yet to be synced.
			}
		}
	}

/**
Sets the folder matched flag

@param aFolderMatched Value to set flag to
*/
void CImapFolder::SetFolderMatched(TBool aFolderMatched)
	{
	iFolderMatched = aFolderMatched;
	}

/**
Gets the folder matched flag

@return Folder matched flag value
*/
TBool CImapFolder::FolderMatched()
	{
	return iFolderMatched;
	}

/**
Performs comparison between two folders by comparing their folder names.

@param aFirst The first folder to compare
@param aSecond The second folder to compare

@return The result of calling Compare on the folder names
*/
TInt CImapFolder::CompareByFolderName(const CImapFolder& aFirst, const CImapFolder& aSecond)
// static method
	{
	return aFirst.iFullFolderPath.Compare(aSecond.iFullFolderPath);
	}
