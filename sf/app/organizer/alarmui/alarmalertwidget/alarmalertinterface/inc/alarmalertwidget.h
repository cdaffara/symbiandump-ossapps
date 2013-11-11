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
// alarmalertwidget.h

#ifndef __ALARM_ALERT_WIDGET__
#define __ALARM_ALERT_WIDGET__

// System includes
#include <QObject>
#include <hbdevicedialogsymbian.h>

// User includes
#include "alarmcommon.h"

// Forward declarations
class CHbDeviceDialogSymbian;
class AlarmAlertObserver;
class CHbSymbianVariantMap;
class CHbSymbianVariant;

// Class declaration
/**
 * @class AlarmAlertWidget
 * @brief This is the client side of the device dialog plugin.
 * Contains API's to show, update and dismiss alarm dialogs
 */
class AlarmAlertWidget : public MHbDeviceDialogObserver
{
public:

    /**
     * @brief Constructor
     * @param observer This is the observer of all user events
     */
	AlarmAlertWidget(AlarmAlertObserver *observer);

	/**
	 * @brief Destructor
	 */
	~AlarmAlertWidget();

	/**
	 * @brief Shows the alarm dialog
	 * @param The alarm information to display
	 * @return true on success, false otherwise
	 */
	bool showAlarmDialog(SAlarmInfo *alarmInfo);

	/**
	 * @brief Dismisses any alarm dialog that is being shown
	 * @return true on success, false otherwise
	 */
	void dismissAlarmDialog();

	/**
	 * @brief Updates the alarm dialog with new information
	 * @param The alarm information to update
	 * @return true on success, false otherwise
	 */
	bool updateAlarmDialog(SAlarmInfo *alarmInfo);

public:
	
    /**
     * @brief Inform the client about user interaction on dialog.
     * @param List of aruguemnts the dialog has sent.
     */
	void DataReceived(CHbSymbianVariantMap& aData);

	/**
	 * @brief Updates the alarm dialog with new information
	 * @param The alarm information to update
	 */
	void DeviceDialogClosed(TInt aCompletionCode);

private:
    /**
     * @brief Handle the events sent by dialog.
     * @param params List of arguments the dialog has sent
     */
    void triggerAction(const CHbSymbianVariant* source);

private:

    /**
     * @var mVariantMap
     * @brief Contain the parameters.
     */
     CHbSymbianVariantMap* mVariantMap;

	/**
	 * @var mSubject
	 * @brief Holds the alarm subject
	 */
    CHbSymbianVariant* mAlarmSubject;

    /**
     * @var mLocation
     * @brief Holds the alarm location (for calendar alerts only)
     */
    CHbSymbianVariant* mLocation;

    /**
     * @var mAlarmTime
     * @brief Holds the alarm expiry time
     */
    CHbSymbianVariant* mAlarmTime;
    
    /**
     * @var mAlarmDate
     * @brief Holds the alarm expiry date
     */
    CHbSymbianVariant* mAlarmDate;

    /**
     * @var mDeviceDialog
     * @brief The interface to the device dialog server
     */
    CHbDeviceDialogSymbian *mDeviceDialog;

    /**
     * @var mAlarmAlertType
     * @brief Tells if the type of alert being displayed
     * is a clock /calendar/to-do alert
     */
    CHbSymbianVariant* mAlarmAlertType;

    /**
     * @var mCanSnooze
     * @brief Tells whether the current alert can be snoozed.
     * true->alarm can be snoozed
     * false->alarm cannot be snoozed
     */
    CHbSymbianVariant* mCanSnooze;

    /**
     * @var mIsSilent
     * @brief Indicates if the alarm is silent or not
     */
    CHbSymbianVariant* mIsSilent;

    /**
     * @var mIsTimedAlarm
     * @brief Indicates if the alarm has time info or not
     */
    CHbSymbianVariant* mIsTimedAlarm;

    /**
     * @var mObserver
     * @brief The observer for user responses
     */
    AlarmAlertObserver *mObserver;
};


#endif
