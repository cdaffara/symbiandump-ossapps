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
 * Header file for the class AlarmClient
 *
 */

#ifndef ALARMCLIENT_H
#define ALARMCLIENT_H

// System includes
#include <qglobal.h>
#include <QObject>
#include <QList>
#include <QTime>
#include <asshddefs.h>
#include <asshdalarm.h>
#include <asclisession.h>

// User includes
#include "clockdatatypes.h"

#ifdef  ALARMCLIENT_LIBRARY
#define ALARMCLIENT_EXPORT Q_DECL_EXPORT
#else
#define ALARMCLIENT_EXPORT Q_DECL_IMPORT
#endif

// Constants
// Clock alarm category ID (copied from T32alm.cpp!)
const TUid KAlarmClockOne = { 0x101F793A };  // Clock alarm category

// Forward declarations
class AlarmListener;

class ALARMCLIENT_EXPORT AlarmClient : public QObject
{
	Q_OBJECT

public:
	AlarmClient(QObject* parent);
	~AlarmClient();

public:
	void getAlarmList(QList<AlarmInfo>& alarmList);
	void setAlarm(AlarmInfo& alarmInfo);
	void deleteAlarm(int alarmId);
	int deleteSnoozedAlarm(int alarmId);
	int getAlarmInfo(int alarmId, AlarmInfo& alarmInfo);
	void toggleAlarmStatus(int alarmId, int alarmStatus);

	void startListener();
	void stopListener();

public:
	void notifyChange(int alarmId = 0);

signals:
	void alarmStateChanged(int alarmId = 0);

private:
	void setAlarmState(
			TAlarmState state, AlarmState& alarmState);
	void setAlarmState(
			AlarmState state, TAlarmState& alarmState);
	void setAlarmRepeatType(
			TAlarmRepeatDefinition repeat, AlarmRepeatType& repeatType);
	void setAlarmRepeatType(
			AlarmRepeatType repeat, TAlarmRepeatDefinition& repeatType);

private:
	AlarmListener *mListener;

	RASCliSession mAlarmSrvSession;
	TASShdAlarm mAlmObj;
	TAlarmId mAlarmIdDetail;
};

#endif // ALARMCLIENT_H

// End of file	--Don't remove this.
