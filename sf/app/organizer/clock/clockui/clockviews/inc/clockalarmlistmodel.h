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
* Header file for class ClockAlarmListModel.
*
*/

#ifndef CLOCKALARMLISTMODEL_H
#define CLOCKALARMLISTMODEL_H

// System includes
#include <QObject>

// User includes
#include "clockdatatypes.h"

// Forward declarations
class QAbstractItemModel;
class QStandardItemModel;
class QModelIndex;
class QTimer;
class ClockAppControllerIf;

class ClockAlarmListModel : public QObject
{
	Q_OBJECT

public:
	ClockAlarmListModel(
			ClockAppControllerIf &controllerIf, QObject *parent = 0);
	virtual ~ClockAlarmListModel();
	
public:
	QAbstractItemModel *sourceModel();
	void populateModel();

signals:
	void changeAlarmListDisplay();

private slots:
	void populateSourceModel();
	void updateSourceModel(int alarmId);
	void updateRemainingTime();

private:
	QString calculateRemainingTime(AlarmInfo alarmInfo);
	int getRemainingSeconds(QDateTime &alarmDateTime);
	QStringList getDisplayStringListforAlarmItem(AlarmInfo alarmInfo);
	void appendAlarmToModel(AlarmInfo alarmInfo);
	void updateAlarmDetails(QModelIndex modelIndex, AlarmInfo alarminfo);
	int getActiveAlarmCount();
	
private:
	QTimer *mTickTimer;
	QStandardItemModel *mSourceModel;
	ClockAppControllerIf &mAppControllerIf;
};

#endif // CLOCKALARMLISTMODEL_H

// End of file	--Don't remove this
