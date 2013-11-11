/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Defines some common data structures used by clock
*
*/



#ifndef CLOCKDATATYPES_H
#define CLOCKDATATYPES_H

#include <QTime>

enum ClockViews {
	MainView = 1,
	AlarmEditor,
	WorldClock,
	ClockSettings
};

struct LocationInfo {
	QString countryName;
	QString cityName;
	QString listImageName;
	int zoneOffset;
	int timezoneId;
	int cityGroupId;
	bool dstOn;
};

enum AlarmRepeatType {
	Once = 0,
	Daily,
	Weekly,
	Workday
};

enum AlarmState {
	InPreparation = -1,
	Queued,
	Snoozed,
	WaitingToNotify,
	Notifying,
	Notified
};

enum AlarmStatus {
	Enabled = 0,
	Disabled
};

enum TimerState {
	Initial,
	Running,
	Stopped,
	Reset
};

enum TimerType {
	HourTimer,
	MinuteTimer,
	SecondTimer,
	None
};

enum AlarmVolumeStatus {
	AlarmVolumeOff = 0,
	AlarmVolumeOn
};

struct AlarmInfo {
	QTime origAlarmTime;
	QTime nextDueTime;
	QString alarmDesc;
	QDate alarmDateTime;
	AlarmStatus alarmStatus;
	AlarmState alarmState;
	AlarmRepeatType repeatType;
	AlarmVolumeStatus volumeStatus;
	int alarmId;
	int alarmDay;
};

enum AlarmEditorState {
	NewAlarm = 0,
	ResetAlarm
};

enum SettingsItemChanged {
	AutoTimeUpdate,
	AlarmTone,
	AlarmSnoozeTime
};

enum AlarmListUserRole {
	AlarmIcon = Qt::UserRole + 100,
	AlarmDetails
};

#endif // CLOCKDATATYPES_H

// End of file	--Don't remove this.
