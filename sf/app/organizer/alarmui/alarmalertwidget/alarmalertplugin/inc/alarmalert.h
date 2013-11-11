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
// alarmalert.h

#ifndef __ALARM_ALERT_H__
#define __ALARM_ALERT_H__

// The plugin name. Do not remove the "com.nokia.hb" part.
const QString ALARM_ALERT_PLUGIN("com.nokia.hb.alarmalertdialog");
_LIT(ALARM_ALERT_PLUGIN_SYMBIAN, "com.nokia.hb.alarmalertdialog");

// The string identifier for command
const QString alarmCommand("command");
_LIT(alarmCommandSymbian, "command");

// The string identifier for alarm subject
const QString alarmSubject("subject");
_LIT(alarmSubjectSymbian, "subject");

// The string identifier for alarm location
const QString alarmLocation("location");
_LIT(alarmLocationSymbian, "location");

// The string identifier for alarm time
const QString alarmTime("time");
_LIT(alarmTimeSymbian, "time");

// The string identifier for alarm date.
const QString alarmDate("date");
_LIT(alarmDateSymbian, "date");

// The string identifier for alarm time

// The string identifier for alarm type
const QString alarmType("type");
_LIT(alarmTypeSymbian, "type");

// The string identifier for alarm ringing type
const QString alarmRingingType("ringingType");
_LIT(alarmRingingTypeSymbian, "ringingType");

// The string identifier for alarm snoozability
const QString alarmCanSnooze("canSnooze");
_LIT(alarmCanSnoozeSymbain, "canSnooze");

// The string identifier for checking the nontimed alarms
const QString alarmIsTimed("isTimed");
_LIT(alarmIsTimedSymbian, "isTimed");


/**
 * @enum Commands
 * @brief Some common commands that can be sent
 * back by the alarm dialog
 */
enum Commands {
	Stop,
	Silence,
	Snooze,
	Shown,
	Other
};

/**
 * @enum AlarmType
 * @brief Alarm types to be sent
 * to the alarm dialog
 */
enum AlarmType {
    ClockAlarm = 0,
	CalendarAlarm,
    TodoAlarm,
    OtherAlarm
    };

#endif		// __ALARM_ALERT_H__
