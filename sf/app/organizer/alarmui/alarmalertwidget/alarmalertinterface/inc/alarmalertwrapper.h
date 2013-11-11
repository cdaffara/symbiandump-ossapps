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

#ifndef __ALARM_ALERT_WRAPPER_H__
#define __ALARM_ALERT_WRAPPER_H__

// System includes
#include <e32def.h>

// User includes
#include "alarmcommon.h"

// Forward declarations
class AlarmAlertWidget;
class AlarmAlertObserver;

// Class declaration
/**
 * @class AlarmAlert
 * @brief Wrapper class that clients need to use to
 * launch alarm alerts
 */
class AlarmAlert
{
public:
    
    /**
     * @brief Constructor
     * @param observer The observer for alarm events
     */
	IMPORT_C AlarmAlert(AlarmAlertObserver *observer);
	
	/**
	 * @brief Destructor
	 */
	IMPORT_C ~AlarmAlert();
	
	/**
	 * @brief Shows the alarm alert. This is merely a request
	 * and it returns immediately. When the dialog actually
	 * gets shown is decided by the device dialog server.
	 * 
	 * AlarmAlertObserver::alertDisplayed is called once the
	 * dialog actually gets shown.
	 * 
	 * AlarmAlertObserver::alertCompleted is called if the dialog
	 * is closed by user action
	 * 
	 * AlarmAlertObserver::alertCancelled is called in case the
	 * dialog closes unexpectedly or is cancelled in between
	 * 
	 * @return true on success, false otherwise
	 */
	IMPORT_C bool showAlarmAlert(SAlarmInfo *alarmInfo);
	
	/**
	 * @brief Dismisses the alarm alert being shown. This is 
	 * merely a request and it returns immediately. 
	 * AlarmAlertObserver::alertCancelled is called once the
	 * alert is cancelled successfully
	 * 
	 */
	IMPORT_C void dismissAlarmAlert();

	/**
	 * @brief Updates the alarm alert with the information
	 * provided
	 */
	IMPORT_C bool updateAlarmAlert(SAlarmInfo *alarmInfo);
	
private:
	
	/**
	 * @var mAlarmWidget
	 * @brief The client side interface for the alarm alert
	 */
	AlarmAlertWidget *mAlarmWidget;
};

#endif		// __ALARM_ALERT_WRAPPER_H__
