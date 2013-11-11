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

#ifndef __IMPSMTM_H__
#define __IMPSMTM_H__

#include <mtsr.h>
#include <msvstd.h>

#include <imapset.h>
#include <imapcmds.h>
#include "mimapsessionobserver.h"

#include <offop.h>

// Forward declarations
class CImImap4Session;
class CImImap4FolderSync;
class CImImap4Synchronise;
class CImap4ServerMtm;
class CImImap4Compound;
class CImap4OffLineControl;
class CImap4Utils;

//******************
// MActiveWrapperObserver - A Mixin class which provides mechanism to communicate to CActiveOwner
//******************
class MActiveWrapperObserver
/**
@internalComponent
@released
*/
	{
public:	
	virtual void RequestCompleted(TInt aChildActiveObjectID, TInt aError)=0;
	virtual void Activate()=0;
	virtual void NonCompletedFailureOnSession(TInt aId)=0;
	};

//*******************
// CActiveWrapper - The wrapper class which owns a CmsgActive and signals its Cactive parent
//*******************

// CActiveWrapper is the 'thin' CActive object which wraps each of your CMsgActive objects
class CActiveWrapper : public CActive, public MImapSessionObserver
/**
@internalComponent
@released
*/
	{
public:
	static CActiveWrapper* NewL(TInt aID);
	void StartL(MActiveWrapperObserver* aManager);
	~CActiveWrapper();
	TRequestStatus* SessionStatus();
	void SetEntry(CMsvServerEntry* aEntry);
	
	// Return of current imap4session
	CImImap4Session* GetImap4Session();
	
protected:
	void RunL();
	void DoCancel();
	void DoComplete(TInt aStatus);

private:
	CActiveWrapper(TInt aID);
	void ConstructL();

	// From MImapSessionObserver
	void NonCompletedFailure();
private:
	TInt iID;							// my unique ID which my parent uses to identify me
	MActiveWrapperObserver* iParentPtr;	// handle to my parent (and hence its Completed() function)

protected:
	// The session which CActiveWrapper owns
	CImImap4Session*	iSession;

	// Class for compound operations
	CImImap4Compound*	iCompound;

	// Full synchronise class (only ever instantiated on primary session)
	CImImap4Synchronise*	iFullSync;

private:
	friend class CImap4ServerMtm;
	};

// package buffer for progress info.
typedef TPckgBuf<TImap4CompoundProgress> TImap4ProgressBuf;

// States that MTM can be in
/**
@internalComponent
@released
*/
enum TMtmState
	{
	EMtmStateIdle=0,
	EMtmStateCopyToLocal,
	EMtmStateCopyFromLocal,
	EMtmStateCopyWithinService,
	EMtmStateMoveToLocal,
	EMtmStateMoveFromLocal,
	EMtmStateMoveWithinService,
	EMtmStateDeleteAfterMove,
	EMtmStateDelete,
	EMtmStateDeleteFolder,
	EMtmStateCreateFolder,
	EMtmStateRenameFolder,
	EMtmStatePrimaryConnect,
	EMtmStateSecondaryConnect,
	EMtmStateForegroundSync,
	EMtmStateMiscCommand,
	EMtmStateWaitingForBackgroundToFinish,
	EMtmStateSyncCompleted,
	EMtmStatePopulate,
	EMtmStateLogging,
	EMtmStateOffLineDelete,
	EMtmStateOffLineUndelete,
	EMtmStateOffLineCopyToLocal,
	EMtmStateOffLineMoveToLocal,
	EMtmStateOffLineCopyFromLocal,
	EMtmStateOffLineMoveFromLocal,
	EMtmStateOffLineCopyWithinService,
	EMtmStateOffLineMoveWithinService,
	EMtmStateOffLinePopulate,
	EMtmStateSecondarySessionIdle
	};

class CImap4ServerMtm : public CBaseServerMtm, public MActiveWrapperObserver
/**
@internalTechnology
@released
*/
	{
public:
	IMPORT_C static CImap4ServerMtm* NewL(CRegisteredMtmDll& aRegisteredMtmDll,CMsvServerEntry* aEntry);
	~CImap4ServerMtm();

	void CopyToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void CopyFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void CopyWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveToLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void MoveWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	void DeleteAllL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);
	void CreateL(TMsvEntry aNewEntry, TRequestStatus& aStatus);
	void ChangeL(TMsvEntry aNewEntry, TRequestStatus& aStatus);
	//
	void StartCommandL(CMsvEntrySelection& aSelection, TInt aCommand, const TDesC8& aParameter, TRequestStatus& aStatus);
	//
	TBool CommandExpected();
	//
	const TDesC8& Progress();

	// What state we're in
	enum
		{
		EBothIdle=0,
		EPrimaryBusy,
		EBothBusy,
		ESecondaryBusy
		};
protected:
	void DoCancel();
	void DoRunL();
	void DoComplete(TInt aError);
	
	// From MActiveWrapperObserver
	void RequestCompleted(TInt aId, TInt aCode);
	void Activate();
	void NonCompletedFailureOnSession(TInt aId);

private:
	CImap4ServerMtm(CRegisteredMtmDll& aRegisteredMtmDll,CMsvServerEntry* aEntry);
	void ConstructL();
	TInt CheckSelectionL(const CMsvEntrySelection& aSelection,
					 	 CMsvEntrySelection* aLocalCopy,
				 		 const TBool aMessages,
				 		 const TBool aParts,
						 const TBool aFolders,
						 const TBool aIsInService);
//	void SaveOperationL(CImOffLineOperation& aOperation);
	void MessageErrorL(const TMsvId aMessageId, const TInt aError);
	TInt SetLocalSubscription(const TMsvId aFolder, const TBool aSubscribed);
	void MtmCommandL(const TInt aType,const CMsvEntrySelection& aSelection, TMsvId aDestination, TRequestStatus& aStatus);
	void MarkOnOrOfflineL(const TBool aOnline);
	void ChangeVisibilityL(TMsvId aParent, TBool aInvisible);
	void ChangeVisibilityL(TMsvId aParent, TBool aInvisible, TBool aRecurse, TUid aType);
	void PropagateInvisibleFlagL(TMsvId aId);
	void GoneOffline();

	// Set entry, leave if error (not related to setentry() which sets iEntry)
	void SetEntryL(const TMsvId aId);

	// Change entry, leave if error
	void ChangeEntryL(const TMsvEntry& aEntry);

	// Get children, leave if error
	void GetChildrenL(CMsvEntrySelection& aSelection);

	void DeleteEntryL(TMsvId aId);
	void UndeleteAllL(const CMsvEntrySelection& aSelection, TRequestStatus& aStatus);

	// now used instead of ChangeL
	TBool RenameFolderL(TMsvId aId, const TImap4RenameFolder& aRename);

	// load or reload service settings
	void LoadSettingsL(TMsvId aId);

	void ClearNewFlagL(TMsvId aParent);

	TBool PruneMessages(const CMsvEntrySelection& aSelection);
	TBool IsPartialPopulate(TDes8& aParameter);

private:
	TInt						iProgressMsgsToDo;
	TInt						iProgressMsgsDone;
	TInt						iProgressErrorCode;
	TImap4ProgressBuf			iProgressBuf;

	TImap4GenericProgress::TImap4GenericProgressState	iLastSessionState;
	TImap4GenericProgress::TImap4GenericProgressOp		iRequestedOperation;

	TMsvId						iServiceId;

	// Copy of the service's settings
	CImImap4Settings*			iServiceSettings;

	// Internal state for multipart operations
	TMtmState					iState;
	TMtmState					iSavedState;
	TInt						iLastCommand;
	CMsvEntrySelection*			iSelection;
	CMsvEntrySelection*			iOneSelection;
	TMsvId						iDestination;

	// The two sessions that we can deal with
	CActiveWrapper*				iPrimarySession;
	CActiveWrapper*				iSecondarySession;

	// the object that manages our offline operations
	CImap4OffLineControl*		iOffLineControl;
	
	// The session the last command was issued to
	CActiveWrapper*				iCurrentSession;
	TInt						iId;
	TInt						iCode;

	// State of sessions
	//TInt						iSessionState;
	TBool						iBackgroundSyncInProgress;
	TBool						iBatchInProgress;

	// Can we be deleted now?
	TBool						iCanBeDeletedNow;

	// Should we clear New flag on messages?
	TBool						iClearNewFlagOnNextSync;

	TRequestStatus*				iRequest;

	TImap4GetMailOptions		iGetMailOptions;

	CImap4Utils*				iUtils;

	TInt						iTotalSize;
	TImImap4GetPartialMailInfo	iPartialMailInfo;
	};
#endif
