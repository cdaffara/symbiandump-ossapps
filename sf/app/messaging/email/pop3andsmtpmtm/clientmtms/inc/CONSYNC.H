// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CONSYNC_H__
#define __CONSYNC_H__

#include <e32base.h>
#include <msvapi.h>

#include "ImapConnectCompletionStates.H"

class CClientMtmRegistry;
class CBaseMtm;
class CMtmUiRegistry;
class CBaseMtmUi;
class CImapConnectAndSyncOp;
class MMsvImapConnectionObserver;


class CProgressTimer : public CTimer
/**
@internalComponent
@released
*/
	{
public:
	static CProgressTimer* NewL(CImapConnectAndSyncOp& aOperation);

private:	// methods from CActive, via CTimer
	virtual void RunL();
	virtual TInt RunError(TInt /*aError*/);
private:
	CProgressTimer(CImapConnectAndSyncOp& aOperation);
private:
	CImapConnectAndSyncOp& iOperation;

	};

class CRefreshTimer : public CTimer
	{
public:
	static CRefreshTimer* NewL(CImapConnectAndSyncOp& aOperation);
private:	// methods from CActive, via CTimer
	virtual void RunL();
	virtual TInt RunError(TInt /*aError*/);
private:
	CRefreshTimer(CImapConnectAndSyncOp& aOperation);
private:
	CImapConnectAndSyncOp& iOperation;
	};


class CImapConnectAndSyncOp : public CMsvOperation, public MMsvEntryObserver
/**
	The CImapConnectAndSyncOp is an operation handles connection and 
	synchronisation to an IMAP account.

	The operation has three different completion stages. After connection has 
	occurred with the IMAP server, after the full sync has completed, or after 
	disconnecting from the IMAP server. With the last option the operation does 
	intermitent refreshes until connection timeout is exceeded. The sync is done
	as a background task.

	The refresh rate is set by the CImImap4Settings class. It is obtained from 
	the settings on creation of the object and that will persist for the lifetime
	of the object. The timeout value is provided by iMtmData1 component of the 
	IMAP service entry index. 

	NOTE - this value is obtained from the commdb by the server MTM.

	This class contains two timers - a refresh and a progress timer. The refresh
	timer is used to refresh the inbox as described earlier. The progress timer
	has two roles. Whilst the sync takes place the progress timer is fired every
	KImapDefaultProgressRate microseconds - this is currently set to approx 1sec.
	When fired it calls the ProgressL() function. This updates the state and the
	operation observer as necessary. Once the sync has completed the progress 
	timer acts as the timeout timer. If the timeout value is zero or negative 
	then no timeout actually occurs even though the timer is used.

	This class sets itself as an observer to the store entry for the IMAP service.
	It uses the notification of changes to the entry to push the state machine 
	through the sync states. These sync states are concerned with the first
	sync to the IMAP acconut and NOT any subsequent inbox refresh syncs. The 
	first sync is broken into three stages - syncing the inbox then the folder
	list and finally the folders themselves.

	The state machine starts in the NotStarted state. Here it invokes an async 
	command on the server MTM - KIMAP4MTMConnectAndSynchronise. The class sets
	itself active and moves to the Connecting state.

	In the Connecting state, the RunL() checks the completion criteria. If 
	completion is when the connection to the IMAP server is established then the
	observer is notified (completed) and the operation ends. The sync continues
	in the background. If the completion criteria is for the other options then 
	the state moves to the FirstSyncingUpdatingInbox state. The progress timer is
	reset. 

	NOTE - the class is SetActive() even though no async request has been made. 
	In order for the scheduler not to call its RunL() the iStatus is set to
	KRequestPending. This is done in order to make the operation appear as if it
	is not complete. Therefore care must be taken when changing code not to call
	SetActive() again.

	During the first sync to the IMAP account the state machine is driven by 
	calls to ProgressL(). This may be either by the progress timer or by the 
	owner/observer of this operation.

	In state FirstSyncingUpdatingInbox the sync progress is obtained from the 
	server MTM. If the progress has moved on from ESyncInbox then the state is
	changed to FirstSyncingUpdatingFolderList. The observer is notified and the 
	state machine moves onto handling this next state.

	In state FirstSyncingUpdatingFolderList the sync progress is again obtained
	from the server MTM. If the progress has moved on from ESyncFolderTree then
	the state is changed to FirstSyncingUpdatingFolders. The observer is notified
	and the state machine moves onto handling this next state.
	
	In state FirstSyncingUpdatingFolders the sync progress is again obtained
	from the server MTM. If the progress is not Idle then nothing happens. 
	Otherwise if the completion criteria is for after sync with the IMAP server
	then the state moves to CompletingSelf and the active object self-completes.
	This ensures that its RunL() is called asap. If the completion criteria is
	for when disconnection from the IMAP server occurs then the state moves to 
	Waiting and the refresh timer is started and the observer is notified.

	The behaviour in the Waiting state depends on which function has been called.
	In ProgressL() the progress timer changes its role to the idle timeoout timer.
	If the timeout time has not been set then it is set, although if the interval
	is zero or negative it cannot be set. If the timeout time has been set then
	a check is made to see if the current time has passed it. If so the class
	invokes the async function KIMAP4MTMDisconnect on the server MTM and the state
	moves to DisconnectingOnTimeout. The observer is notified. If the current time
	has not exceeded the timeout time or the timeout time has been set then the 
	progress timer is started - the value depending on whether a timeout time
	exists or not.

	When the refresh timer fires the DoRefreshInboxL() function is called. The state
	will be Waiting. If the server MTM is active then the refresh timer is re-started.
	If the server MTM is not active then the async function KIMAP4MTMInboxNewSync 
	is called and the state moves to the ForcedSyncing state.

	In the ForcedSyncing state the RunL() re-starts the refresh timer and the
	state is moved back to Waiting. 

	NOTE - the active object is SetActive() again as described before.

	In the DisconnectingOnTimeout state the RunL() completes the observer. The
	operation is finished and the final progress value is set to KErrTimeOut.

	In the CompletingSelf state the RunL() completes the observer. The operation
	is finished.

	@internalComponent
	@released
*/
	{
public:
	static CImapConnectAndSyncOp* NewL(
									  CMsvSession&						aSession, 
									  const CMsvEntrySelection&			aSelection, 
									  CBaseMtm& 						aBaseMtm, 
									  TInt 								aPriority, 
									  TRequestStatus&					aStatus, 
									  TImapConnectionCompletionState	aCompletionState, 
									  MMsvImapConnectionObserver* 		aConnectionObserver=NULL
									  );

	virtual ~CImapConnectAndSyncOp();

	void DoRefreshInboxL();

public:	// methods from CMsvOperation

	virtual const TDesC8& ProgressL();
	
public:	// methods from MMsvEntryObserver

	void HandleEntryEventL(TMsvEntryEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/);

private:	// methods from CActive

	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);

private:
	CImapConnectAndSyncOp(
						 CMsvSession& aSession,
						 const CMsvEntrySelection& aSelection,
						 CBaseMtm& aBaseMtm, 
						 TInt aPriority, 
						 TRequestStatus& aStatus, 
						 TImapConnectionCompletionState aCompletionState,
						 MMsvImapConnectionObserver* aConnectionObserver
						 );
	void ConstructL(const CMsvEntrySelection& aSelection);

	void Completed(TInt aError);
	TInt GetServiceProgress();
	void UpdateObserver() const;
	void ResetProgressTimer();
	void ResetRefreshTimer();

private:
	enum TImapConnState 
		{
		ENotStarted=0, 
		EConnecting, 
		EFirstSyncingUpdatingInbox,
		EFirstSyncingUpdatingFolderList,
		EFirstSyncingUpdatingFolders,
		EWaiting, 
		EForcedSyncing, 
		EDisconnectingOnTimeout, 
		ECompletingSelf,
		ECompleted
		};

	TImapConnState iState;
	CBaseMtm& iBaseMtm;
	CMsvOperation* iOperation;
	CMsvEntrySelection* iSelection;
	TPckgBuf<TImap4CompoundProgress> iProgress;
	TPckgBuf<TImap4SyncProgress> iSyncProgress;
	CRefreshTimer* 	iRefreshTimer;
	CProgressTimer* iProgressTimer;
	MMsvImapConnectionObserver*		iConnectionObserver;
	TBool 							iForcedCancel;
	TImapConnectionCompletionState	iCompletionState;
	CMsvEntry* iServiceEntry;
	
	//	Following member used for timeout and refresh functionality
	TBool				 	iTimeout;
	TTime				 	iTimeoutAt;
	TTimeIntervalSeconds	iIdleTimeout;
	TTimeIntervalSeconds	iRefreshRate;
	};

#endif	// __CONSYNC_H__
