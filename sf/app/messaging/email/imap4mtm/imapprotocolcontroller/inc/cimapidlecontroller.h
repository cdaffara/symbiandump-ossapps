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

#ifndef __CIMAPIDLECONTROLLER_H__
#define __CIMAPIDLECONTROLLER_H__

#include <mentact.h>
#include <msvstd.h>
#include <imapset.h>
#include "cimapobservabletimer.h"


class CImapSyncManager;
class CImapFolder;
class CImapSettings;
class CMsvServerEntry;
class CImapSession;
class CImapCompoundCopyToLocal;
class CImapMailStore;

/**
Implemented by anything that wants to use a CImapIdleController.
Provides an interface that can be called if an unexpected error
occurs while in IDLE.

@internalTechnology
@prototype
*/
class MImapIdleControllerObserver
	{
public:
	virtual void OnIdleError(TInt aError) = 0;
	};


/**
Provides management of IMAP IDLE as a background task.
The Protocol Controller is free to complete a client requested operation
immediately after calling StartIdle().

@internalTechnology
@prototype
*/
class CImapIdleController : public CActive, public MImapTimerObserver
	{
public:
	~CImapIdleController();
	
	static CImapIdleController* NewL(MImapIdleControllerObserver& aObserver, CImapSession*& aSession, CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore);

	void StartIdle();
	void StopIdle(TRequestStatus& aStatus);
	
	void Progress(TImap4CompoundProgress& aCompoundProgress);

private:
	enum TIdleControllerState
		{
		EFinished,					// 00 No Command Issued,     NOT active in this state
		EStartingIdle,				// 01 StartIdle() requested, Active in this state
		ESelectingInbox,			// 02 SELECT command issued. Active in this state
		EWaitingForContinuation,	// 03 IDLE command issued.   Active in this state
		EIdling,					// 04 wait-for-event issued  Active in this state
		EWaitingForDone,			// 05 DONE command issued    Active in this state
		EWaitingForServerEvent,		// 06 Dummy read issued	  Active in this state 
		EWaitingForFlush,			// 07 read cancelled and flushed Active in this state
		ESynchronising,			    // 08 CImapFolder::Synchronise issued Active in this state.
		ESyncFetching				// 09 CImapCompoundCopyToLocal Active in this state...
		};

	CImapIdleController(MImapIdleControllerObserver& aObserver, CImapSession*& aSession, CImapSyncManager& aSyncManager, CMsvServerEntry& aServerEntry, CImapSettings& aImapSettings, CImapMailStore& aImapMailStore);
	void ConstructL();

	void RunContinueL();
	void RunStopL();
	void GoIdleL();
	TBool DoBodyDownloadL();
	TBool SyncRequired();

	// From CActive
	virtual void RunL();
	
	// mimic CMsgActive	
	void Queue(TRequestStatus& aStatus);
	virtual void DoRunL();
	virtual void DoCancel();
	virtual void Complete(TInt aErr);
	virtual void DoComplete(TInt& aErr);
	
	// From MImapTimerObserver
	virtual void OnTimerL(const CImapObservableTimer& aSourceTimer);

	void EnterIdlingState();
	void InternalCancelAndSetState(TIdleControllerState aNextState);

private:
	MImapIdleControllerObserver& iObserver;
	CImapSession*& iSession;
	CImapSyncManager& iSyncManager;
	CMsvServerEntry& iServerEntry;
	CImapSettings& iImapSettings;
	CImapMailStore& iImapMailStore;
	CImapCompoundCopyToLocal* iCopyToLocal;
	
	// report request status
	TRequestStatus* iReport;
	
	// State Information
	TBool iUsingIdle;
	TIdleControllerState iState;
	TBool iStoppingIdle;
	TBool iInternalCancel;

	CImapObservableTimer* iReissueTimer;
	};

#endif	// __CIMAPIDLECONTROLLER_H__
