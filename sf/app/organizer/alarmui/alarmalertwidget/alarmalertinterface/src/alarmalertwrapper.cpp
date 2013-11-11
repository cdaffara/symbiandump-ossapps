/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
// alarmalertwrapper.h

// User includes
#include "alarmalertwrapper.h"
#include "alarmalertwidget.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "alarmalertwrapperTraces.h"
#endif

// ---------------------------------------------------------
// AlarmAlert::AlarmAlert
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C AlarmAlert::AlarmAlert(AlarmAlertObserver *observer)
{
	OstTraceFunctionEntry0( ALARMALERT_ALARMALERT_ENTRY );
	// Construct the client interface
    mAlarmWidget = new AlarmAlertWidget(observer);
    OstTraceFunctionExit0( ALARMALERT_ALARMALERT_EXIT );
}

// ---------------------------------------------------------
// AlarmAlert::~AlarmAlert
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C AlarmAlert::~AlarmAlert()
{
    OstTraceFunctionEntry0( DUP1_ALARMALERT_ALARMALERT_ENTRY );
    // Cleanup
	if (mAlarmWidget) {
		delete mAlarmWidget;
	}
	OstTraceFunctionExit0( DUP1_ALARMALERT_ALARMALERT_EXIT );
}

// ---------------------------------------------------------
// AlarmAlert::showAlarmAlert
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C bool AlarmAlert::showAlarmAlert(SAlarmInfo *alarmInfo)
{
    OstTraceFunctionEntry0( ALARMALERT_SHOWALARMALERT_ENTRY );
    // Forward the call to the client
	return mAlarmWidget->showAlarmDialog(alarmInfo);
}

// ---------------------------------------------------------
// AlarmAlert::dismissAlarmAlert
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C void AlarmAlert::dismissAlarmAlert()
{
    OstTraceFunctionEntry0( ALARMALERT_DISMISSALARMALERT_ENTRY );
    // Forward the call to the client
	mAlarmWidget->dismissAlarmDialog();
	OstTraceFunctionExit0( ALARMALERT_DISMISSALARMALERT_EXIT );
}

// ---------------------------------------------------------
// AlarmAlert::updateAlarmAlert
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C bool AlarmAlert::updateAlarmAlert(SAlarmInfo *alarmInfo)
{
    OstTraceFunctionEntry0( ALARMALERT_UPDATEALARMALERT_ENTRY );
    // Forward the call to the client
	return mAlarmWidget->updateAlarmDialog(alarmInfo);
}
