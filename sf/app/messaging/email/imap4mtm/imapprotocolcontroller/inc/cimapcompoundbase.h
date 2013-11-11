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

#ifndef __CIMAPCOMPOUNDBASE_H__
#define __CIMAPCOMPOUNDBASE_H__

#include <mentact.h>
#include <msvstd.h>
#include <imapset.h>

class CImapSession;
class CImapSettings;
class CImapSyncManager;
class CMsvServerEntry;
class TImap4CompoundProgress;

// Operation step identifiers
enum TCompoundStep
	{
	EFinished						=  0,
	ESelectSourceMailboxRW			=  1,
	ESelectSourceMailboxRO			=  2,
	ESelectDestinationMailboxRW		=  3,
	ESelectDestinationMailboxRO		=  4,
	EFetchMessage					=  5,
	ECopyMessage					=  6,
	EAppendMessage					=  7,
	EDeleteMessage					=  8,
	EDeleteLocalMessage				=  9,
	EDeleteAllMessages				= 10,
	ENewSyncFolder					= 11,
	ESyncFolder						= 12,
	EInboxDuplicateMove				= 13,
	EInboxDuplicateCopy				= 14,
	ECloseFolder					= 15,
	EDeleteFolder					= 16,
	ECreate							= 17,
	ERename							= 18,
	ECreateSession					= 19,
	EStartBackgroundSyncOp			= 20,
	ESelectInboxRW					= 21,
	EFetchMessageInitialise			= 22,
	ELocalCopyComplete				= 23,
	EExpunge						= 24,
	ESetSourceMailbox				= 25,
	ESyncTree						= 26,
	ESyncService					= 27,
	EDeleteLocalFolder				= 28,
	EDisconnect						= 29,
	ESetSource						= 30,
	EProcessRenameResponse			= 31,
	ECreateLocal					= 32,
	ESyncSubscriptions				= 33,
	EInboxDoneOps					= 34,
	EFolderDoneOps					= 35,
	EFolderCheckPendingOPs			= 36,
	EInboxCheckPendingOps			= 37,
	EInboxSync						= 38,
	EInboxEarlyDeletes				= 39,
	EFolderEarlyDeletes				= 40,
	EInboxEarlyExpunge				= 41,
	EFolderEarlyExpunge				= 42,
	EInboxLateDeletes				= 43,
	EFolderLateDeletes				= 44,
	EInboxLateExpunge				= 45,
	EFolderLateExpunge				= 46,
	EProcessFetchFlags				= 47,
	ESelectSyncFolder				= 48,
	ECheckDestinationMailbox		= 49,
	ESelectFolderAfterClose			= 50,
	ESuspendedForMigrate			= 51,
	ESyncInboxAutoFetch				= 52,
	ESyncFolderAutoFetchCheck		= 53,
	ESyncFolderAutoFetch			= 54,
	ECheckFolderExists				= 55,
	EProcessFolderCheck				= 56,
	EUpdateFlag						= 57
	};
	
/**
@internalTechnology
@prototype
*/
class CImapCompoundBase : public CMsgActive
	{
			
public:
	~CImapCompoundBase();

	virtual void StartOperation(TRequestStatus& aStatus, CImapSession& aSession) = 0;
	virtual void Progress(TImap4CompoundProgress& aCompoundProgress) = 0;
	virtual void StopForMigrate();
	void CancelForMigrate();
	virtual void ResumeOperationL(TRequestStatus& aStatus, CImapSession& aSession) = 0;
	TBool Suspended();
	

	
protected:
	CImapCompoundBase(CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings);
	void DoRunL();
	virtual TBool DoRunLoopL() = 0;
	virtual void DoComplete(TInt& aErr);

	TInt ProcessSessionError();
	virtual TInt ProcessNegativeServerResponse() = 0;
	
	// utility methods:	
	TInt  CheckSelectionL(const CMsvEntrySelection& aSelection, CMsvEntrySelection* aLocalCopy, const TBool aMessages, const TBool aParts, const TBool aFolders, const TBool aIsInService);
	TInt  CalculateDownloadSizeL(const CMsvEntrySelection& aSelection);
	TMsvId FindFolderL(const TMsvId aMessage);
	void SetEntryL(const TMsvId aId);
	void CompleteSelf();
	void SetCurrentStep();
	void MessageErrorL(const TMsvId aMessageId, const TInt aError);
	
protected:
	CImapSession* iSession; 		// not owned by this class
	CImapSyncManager& iSyncManager;

	CMsvServerEntry& iServerEntry;
	CImapSettings& iImapSettings;
	
	TCompoundStep  	iCurrentStep;
	TCompoundStep  	iNextStep;
	
	TInt	iProgressErrorCode;		// Is set when an error occur.
	TBool   iStopForMigrate;        // set if stop for migrate requested
	TBool   iCancelForMigrate;      // set if cancelling for migrate
	
	TImap4GenericProgress::TImap4GenericProgressState iProgressState;
	TImap4SyncProgress::TImap4SyncProgressState iSyncProgressState;
	};

#endif // __CIMAPCOMPOUNDBASE_H__
