/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Source file for the class AlarmListener
*
*/

// User includes
#include "alarmlistener.h"
#include "alarmclient.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "alarmlistenerTraces.h"
#endif


AlarmListener::AlarmListener(AlarmClient* client, RASCliSession& session)
:CActive(EPriorityStandard),
mClient(client),
mSession(session)
{
		OstTraceFunctionEntry0( ALARMLISTENER_ALARMLISTENER_ENTRY );
		// Nothing to do.
OstTraceFunctionExit0( ALARMLISTENER_ALARMLISTENER_EXIT );
}

AlarmListener::~AlarmListener()
{
	OstTraceFunctionEntry0( DUP1_ALARMLISTENER_ALARMLISTENER_ENTRY );
	if (IsActive()) {
		Cancel();
	}

OstTraceFunctionExit0( DUP1_ALARMLISTENER_ALARMLISTENER_EXIT );
}

void AlarmListener::start()
{
	OstTraceFunctionEntry0( ALARMLISTENER_START_ENTRY );
	// Add the AO to the scheduler.
	CActiveScheduler::Add(this);

	// Subscrive for async notification from alarm server.
	mSession.NotifyChange(iStatus, mAlarmId);

	// Set the AO active.
	SetActive();

OstTraceFunctionExit0( ALARMLISTENER_START_EXIT );
}

void AlarmListener::stop()
{
	OstTraceFunctionEntry0( ALARMLISTENER_STOP_ENTRY );
	if (IsActive()) {
		Cancel();
	}
OstTraceFunctionExit0( ALARMLISTENER_STOP_EXIT );
}

void AlarmListener::RunL()
{
	OstTraceFunctionEntry0( ALARMLISTENER_RUNL_ENTRY );
	if (iStatus != KRequestPending) {
		// We get notification for various changes with the alarm server.
		// Only the required Events are used to emit a signal.
		if (iStatus == EAlarmChangeEventAlarmDeletion
				|| iStatus == EAlarmChangeEventTimerExpired
				|| iStatus == EAlarmChangeEventState
				|| iStatus == EAlarmChangeEventStatus) {
			// Notify the client.
			mClient->notifyChange(mAlarmId);
		}

		// Cancel and outstanding requests.
		mSession.NotifyChangeCancel();
		// Re-subscribe for change.
		iStatus = KRequestPending;
		mSession.NotifyChange(iStatus, mAlarmId);

		SetActive();
	}
OstTraceFunctionExit0( ALARMLISTENER_RUNL_EXIT );
}

void AlarmListener::DoCancel()
{
	OstTraceFunctionEntry0( ALARMLISTENER_DOCANCEL_ENTRY );
	// Cancel async request.
	mSession.NotifyChangeCancel();

OstTraceFunctionExit0( ALARMLISTENER_DOCANCEL_EXIT );
}

// End of file
