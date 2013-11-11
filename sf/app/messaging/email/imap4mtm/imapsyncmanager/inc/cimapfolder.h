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
// CImapFolder header
// 
//

#if !defined(__CIMAPFOLDER_H__)
#define __CIMAPFOLDER_H__

#include <f32file.h>
#include <mentact.h>
#include <msventry.h>
#include "cimapsyncmanager.h"	// forward declaration is not possible because of embedded enum
#include "cimapfetchresponse.h"

// Forward declarations
class CImapSession;
class CImapFolderIndex;
class CImapFolderInfo;
class CImConvertCharconv;
class CCnvCharacterSetConverter;
class CImConvertHeader;

// Modes updating Seen flag
enum TSeenFlagUpdateMode
	{
	EClearSeenFlag=0,
	ESetSeenFlag
	};

//cached TMsvEntry data
class TMsvCacheData
/**
@internalComponent
@prototype
*/
	{
public:
	TBool iOrphan;
	TUint32 iUid;
	};
	
class CImapFolder : public CMsgActive, public MImapFetchStructureObserver
/**
The IMAP Folder Component shall encapsulate a folder on the IMAP service. 
Folder objects shall be created for subscribed folders, and will contain information about the remote folder 
and the local mirror of that folder that allows the IMAP Server MTM to determine if the folders require synchronisation.
@internalComponent
@prototype
*/
	{
public:
	~CImapFolder();
	// Class contructors
	IMPORT_C static CImapFolder* NewLC(CImapSyncManager& aSyncMan, CMsvServerEntry& aServerEntry, TMsvEmailEntry& aEmailEntry, CImapSettings& aImapSettings, const TDesC& aFullFolderPath);
	IMPORT_C static CImapFolder* NewL(CImapSyncManager& aSyncMan, CMsvServerEntry& aServerEntry, TMsvEmailEntry& aEmailEntry, CImapSettings& aImapSettings, const TDesC& aFullFolderPath);
	IMPORT_C static void DoQuoteL(HBufC16*& aBuffer);

	// Mirror folder tree of remote server
	IMPORT_C TBool Changed(CImapSession& aSession);
	IMPORT_C void SynchroniseL(TRequestStatus& aStatus, CImapSession& aSession, TBool aNewOnly, TInt  aDeleteOption);
	IMPORT_C void SyncDeletesL(TRequestStatus& aStatus, CImapSession& aSession);
	IMPORT_C void SelectL(TRequestStatus& aStatus, CImapSession& aSession, TBool aSelectInbox = ETrue );
	IMPORT_C void ExamineL(TRequestStatus& aStatus, CImapSession& aSession);
	IMPORT_C void UpdateSessionFolderInfoL(CImapSession& aSession);
	
	// Get Mailbox name and Id
	IMPORT_C TDesC& FullFolderPathL();
	IMPORT_C TMsvId MailboxId();

	IMPORT_C void DeleteMessageL(const TMsvId aMessage);

	IMPORT_C void AppendSetSeenL(const TUint32 aMessage);
	IMPORT_C void AppendClearSeenL(const TUint32 aMessage);

	void SetFullFolderPathL(const TDesC& aFullFolderPath);
	void SetParentId(TMsvId aParentId);

	void OnFetchLD(CImapFetchResponse* aImapFetchResponse);
	void OnFetchCommit();
	
	IMPORT_C HBufC16* MakePathL(const TBool aIncludeLeaf);
	
	// Getting the progress updates
	IMPORT_C void Progress(TImap4SyncProgress& aProgress);
	
	// Returns selection of messages to auto-fetch
	IMPORT_C TInt GetFetchMessageChildrenL(CMsvEntrySelection& aSelection);

	void SetFolderMatched(TBool aFolderMatched);
	TBool FolderMatched();

	static TInt CompareByFolderName(const CImapFolder& aFirst, const CImapFolder& aSecond);

private:
	CImapFolder(CImapSyncManager& aSyncMan,CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings);
	void ConstructL(CMsvServerEntry& aServerEntry, TMsvEmailEntry& aEmailEntry, const TDesC& aFullFolderPath);
	void DoRunL();
	void DoCancel();
	void DoComplete(TInt& aStatus);
	void CompleteSelf();

	TBool CheckAndProgressMessageSyncL();
	TInt  GetSyncThreshold();
	void  CheckMessagesInRangeL(TInt aSyncThreshold);
	TBool CreateAndSendUIDSearchStringL(TRequestStatus& aStatus);
	void CheckForMissingMessagesUidsL();
	
	void SetEntryL(const TMsvId aId);
	void ChangeEntryL(const TMsvEntry& aEntry);
	void ChangeEntryBulkL(const TMsvEntry& aEntry);

	void SynchroniseNewL();
	void SynchroniseRangeL(const TUint32 aLowUid, const TUint32 aHighUid);
	void ClearChangeCounts();

	void GetChildrenL(CMsvEntrySelection& aSelection);
	void GetMessageChildrenL(const TMsvId aFolder, CMsvEntrySelection* aChildren);
	void SyncCompleteL();
	void ResetSubscriptionFlagsL(const TMsvId aFolder);
	void MakeSortedFolderIndexL(TBool aUseCachedEntryData);
	TBool ProcessSeenFlagsL(TSeenFlagUpdateMode aUpdateMode);
	void SetMessageFlagsL(TMsvEmailEntry& aEntry, CImapBodyStructure* aBodyStructure);
	void GetFlagsForBodyStructurePart(CImapBodyStructure* aBodyStructure,
	                                  TBool& aHasAttachments, TBool& aHasHtml,
	                                  TBool& aAfterRelated, TBool& aAfterAlternative,
	                                  TBool& aHtmlAfterAltRel, TBool& aHasICalendar,
	                                  TBool& aHasVCalendar, TInt& aSize);
	void ClearNewFlagsIfRequiredL();

private:
	// External data objects owned by the object creator
	CImapSyncManager& 	iSyncManager; 
	CMsvServerEntry& 	iServerEntry;
	CImapSettings& 		iImapSettings;
	CImapSession*		iSavedSession; 
	
	// Folder data members
	TMsvId 		iMailboxId;
	TInt 		iMailboxSize;
	TInt		iMailboxRecent;
	RBuf	 	iFullFolderPath;

	// Progress and Sync states
	CImapSyncManager::TSynchroniseState	iSyncState;
	CImapSyncManager::TSynchroniseState	iNextAction;
	TBool 								iSyncDoSingleStep;
	
	// Synchronisation process objects
	RArray<TMsvCacheData>* iCachedEntryData;
	CMsvEntrySelection*		iSelection;
	CImapFolderInfo* 		iSessionFolderInfo;
	CImapFolderIndex* 		iFolderIndex;
	RArray<TUint> 			iMatchingMessageIds;
	RArray<TUint> 			iDeletedMessageIds;
	RArray<TUint> 			iMissingMessageIds;

	// List of messages to have Seen FLAG set or clear
	RArray<TUint>*		 	iSetSeenList;
	RArray<TUint>*			iClearSeenList;

	// Flags and data members to manage the sync process
	RArrayMessageFlagInfo	iMessageFlagInfoArray;
	TBool					iSomeUnread;
	TInt					iHeadersFetched;
	TInt					iOrphanedMessages;
	TInt					iRemoteMessagesDeleteTagged;
	TInt					iMsgsDone;
	TInt 					iMsgsToDo;
	TInt					iFolderPosition;
	TInt					iSyncLimit;
	TInt					iHighestUid;
	TInt					iMissingUidLow;
	TInt					iMissingUidHigh;
	TInt					iDeleteOption;
	TBool					iFlagChangedOnly;

	// Character set conversion
	CImConvertHeader& iHeaderConverter;

	// General flag used by operations to indicate that this folder has been
	// matched against some other list of folders
	TBool iFolderMatched;
	};

#endif // __CIMAPFOLDER_H__
