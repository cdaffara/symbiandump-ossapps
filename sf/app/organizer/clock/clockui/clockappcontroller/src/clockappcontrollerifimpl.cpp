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
* This file contains the ClockAppControllerIfImpl class definition.
*
*/

// User includes
#include "clockappcontrollerifimpl.h"
#include "clockappcontroller.h"
#include "clockviewmanager.h"
#include "timezoneclient.h"
#include "settingsutility.h"
#include "alarmclient.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockappcontrollerifimplTraces.h"
#endif


/*!
	\class ClockAppControllerIfImpl

	This class implements the ClockAppControllerIf interface which is used
	by the views and other componets to access the services of
	clockviewmanager.
 */

/*!
	Constructor.

	\param controller Pointer to an object of ClockAppController.
	\param parent QObject pointer.
 */
ClockAppControllerIfImpl::ClockAppControllerIfImpl(
		ClockAppController *controller, QObject *parent)
:QObject(parent),
 mAppController(controller)
{
	OstTraceFunctionEntry0( CLOCKAPPCONTROLLERIFIMPL_CLOCKAPPCONTROLLERIFIMPL_ENTRY );
	// Construct the timezone client.
	mTimeZoneClient = TimezoneClient::getInstance();
	// Construct the settings utility object.
	mSettingsUtility = new SettingsUtility(this);
	// Construct the alarm client object.
	mAlarmClient = new AlarmClient(this);
	OstTraceFunctionExit0( CLOCKAPPCONTROLLERIFIMPL_CLOCKAPPCONTROLLERIFIMPL_EXIT );
}

/*!
	Destructor.
 */
ClockAppControllerIfImpl::~ClockAppControllerIfImpl()
{
	OstTraceFunctionEntry0( DUP1_CLOCKAPPCONTROLLERIFIMPL_CLOCKAPPCONTROLLERIFIMPL_ENTRY );
	if (!mTimeZoneClient->isNull()) {
		mTimeZoneClient->deleteInstance();
	}
	if (mSettingsUtility) {
		delete mSettingsUtility;
		mSettingsUtility = 0;
	}
	if(mAlarmClient) {
		delete mAlarmClient;
		mAlarmClient = 0;
	}
	OstTraceFunctionExit0( DUP1_CLOCKAPPCONTROLLERIFIMPL_CLOCKAPPCONTROLLERIFIMPL_EXIT );
}


/*!
	From ClockAppControllerIf.
	Returns a pointer to timezoneClient.

	\return TimezoneClient* Pointer to TimezoneClient object.
	\sa ClockAppControllerIf
 */
TimezoneClient* ClockAppControllerIfImpl::timezoneClient()
{
	OstTraceFunctionEntry0( CLOCKAPPCONTROLLERIFIMPL_TIMEZONECLIENT_ENTRY );
	OstTraceFunctionExit0( CLOCKAPPCONTROLLERIFIMPL_TIMEZONECLIENT_EXIT );
	return mTimeZoneClient;
}

/*!
	From ClockAppControllerIf.
	Returns a pointer to SettingsUtility.

	\return SettingsUtility* Pointer to SettingsUtility object.
	\sa ClockAppControllerIf
 */
SettingsUtility* ClockAppControllerIfImpl::settingsUtility()
{
	OstTraceFunctionEntry0( CLOCKAPPCONTROLLERIFIMPL_SETTINGSUTILITY_ENTRY );
	OstTraceFunctionExit0( CLOCKAPPCONTROLLERIFIMPL_SETTINGSUTILITY_EXIT );
	return mSettingsUtility;
}

/*!
	From ClockAppControllerIf.
	Returns a pointer to AlarmClient.

	\return AlarmClient* Pointer to AlarmClient object.
	\sa ClockAppControllerIf
 */
AlarmClient* ClockAppControllerIfImpl::alarmClient()
{
	OstTraceFunctionEntry0( CLOCKAPPCONTROLLERIFIMPL_ALARMCLIENT_ENTRY );
	OstTraceFunctionExit0( CLOCKAPPCONTROLLERIFIMPL_ALARMCLIENT_EXIT );
	return mAlarmClient;
}

/*!
	From ClockAppControllerIf.
	Issues a request to ClockViewManager to switch to a given view.

	\param viewId The id of the view to be switched to.
	\sa ClockAppControllerIf
 */
void ClockAppControllerIfImpl::switchToView(ClockViews viewId)
{
	OstTraceFunctionEntry0( CLOCKAPPCONTROLLERIFIMPL_SWITCHTOVIEW_ENTRY );
	mAppController->mViewManager->showView(viewId);
	OstTraceFunctionExit0( CLOCKAPPCONTROLLERIFIMPL_SWITCHTOVIEW_EXIT );
}

// End of file	--Don't remove this.
