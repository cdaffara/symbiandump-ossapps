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

#include <startupdomaindefs.h>
#include <ssm/ssmsubstates.hrh>
#include "agssystemstateobserver.h"
#include "agmdebug.h"

CCAgnSystemStateObserver* CCAgnSystemStateObserver::NewL(CAgnServFileMgr& aFileMgr)
	{
	CCAgnSystemStateObserver* self = new (ELeave) CCAgnSystemStateObserver(aFileMgr);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CCAgnSystemStateObserver::CCAgnSystemStateObserver (CAgnServFileMgr& aFileMgr) :
	CActive(CActive::EPriorityStandard), // Standard priority
	iFileMgr(aFileMgr), iShutdown(EFalse)
	{
	CActiveScheduler::Add(this); // Add to scheduler
	}

CCAgnSystemStateObserver::~CCAgnSystemStateObserver ()
	{
	Cancel (); // Cancel any request, if outstanding
	iStateAwareSession.Close();
	}


void CCAgnSystemStateObserver::ConstructL ()
	{
	iWorkAround = SubscribeToShutdownNotifications();
	if (!iWorkAround)
		{
		//Log and Fail silently if this does not connect.
		TInt err = iStateAwareSession.Connect(KSM2AppServicesDomain4);
		if (err == KErrNone)
			{
			iStateAwareSession.RequestStateNotification(iStatus);
			SetActive();
			}
		else
			{
			_DBGLOG_BASIC(AgmDebug::DebugLog("%d : Failed to connect to the state aware session.", err);)
			}
		}
	}

void CCAgnSystemStateObserver::DoCancel ()
	{
	if (iWorkAround)
		{
		iNotification.Cancel();
		}
	else
		{
		iStateAwareSession.RequestStateNotificationCancel();
		}
	}

void CCAgnSystemStateObserver::RunL ()
	{
	if (iStatus == KErrNone)
		{
		TSsmState state;
		GetState(state);

		if (state.MainState() == ESsmShutdown && state.SubState() == ESsmShutdownSubStateCritical)
			{
			iShutdown =  ETrue;
			// This component only responds to shutdown state and the first substate critical event.
			// It does nothing for the subsequent non critical event.

			//This does not require clients to have closed sessions
			iFileMgr.QueueAlarmsImmediatelyForShutdown();
			
			//This requires clients to close, however there will not be a delay while shutting down.
			iFileMgr.CloseScheduledFilesImmediately();
			}
		else if (state.MainState() == ESsmNormal && state.SubState() == ESsmNormalSubState)
			{
			iShutdown = EFalse;
			iFileMgr.RequeueAlarmsForShutdownCancellation();
			}
		AcknowledgeAndRequestStateNotification();
		}
	}

TInt CCAgnSystemStateObserver::RunError (TInt aError)
	{
	if (aError != KErrNone)
		{
		_DBGLOG_BASIC(AgmDebug::DebugLog("%d : RunError in CCAgnSystemStateObserver.", aError);)
		}
	return KErrNone; //Don't want to panic the server.
	}

TBool CCAgnSystemStateObserver::IsShutdownInProgress() const
	{
	return iShutdown;
	}

TBool CCAgnSystemStateObserver::SubscribeToShutdownNotifications()
	{
	TBool workAround;
	TInt shutdownkey = 111;
	TUid uid = {0xE6AEDA09}; //test harness uid
	TInt value;
	workAround = (iNotification.Get(uid, shutdownkey, value) != KErrNotFound);
	if (workAround)
		{
		iNotification.Attach(uid, shutdownkey);
		iNotification.Subscribe(iStatus);
		SetActive();
		}
	return workAround;
	}

void CCAgnSystemStateObserver::ConvertToSystemState(TSsmState& aState)
	{
	TUid testUid = {0xE6AEDA09};
	TInt test_key = 111;
	TInt value = 0;
	iNotification.Get(testUid, test_key, value);
	switch (value)
		{
	    case 0:
		aState.Set(ESsmNormal, ESsmNormalSubState);
		break;
	    case 1: //ESsmShutdownCritical:
		aState.Set(ESsmShutdown, ESsmShutdownSubStateCritical);
		break;
	    default: 
		;
		}
	}

void CCAgnSystemStateObserver::GetState(TSsmState& state)
	{
	if (iWorkAround)
		{
		ConvertToSystemState(state);
		}
	else
		{
		state = iStateAwareSession.State();
		}
	}

void CCAgnSystemStateObserver::AcknowledgeAndRequestStateNotification()
	{
	if (iWorkAround)
		{
		SendSystemStateAck();
		}
	else
		{
		iStateAwareSession.AcknowledgeAndRequestStateNotification(KErrNone, iStatus);
		SetActive();
		}
	}

void CCAgnSystemStateObserver::SendSystemStateAck()
	{
	TUid testUid = {0xE6AEDA09};
	TInt test_key = 111;
	
	TInt err = iNotification.Set(testUid, test_key, 2); //2 is Ack enum
	iNotification.Subscribe(iStatus);
	SetActive();
	}
