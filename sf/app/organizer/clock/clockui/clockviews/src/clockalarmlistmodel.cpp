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
* Definition file for class ClockAlarmListModel.
*
*/

// System includes
#include <QTimer>

#include <QStandardItemModel>
#include <QStandardItem>
#include <HbIcon>
#include <HbParameterLengthLimiter>

// User includes
#include "clockalarmlistmodel.h"
#include "alarmclient.h"
#include "timezoneclient.h"
#include "clockalarmlistitemprototype.h"
#include "clockappcontrollerif.h"
#include "settingsutility.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clockalarmlistmodelTraces.h"
#endif


// Constants
const int KOneHourInMinute(60);
const int KOneMinuteInSecons(60);
const int KSecondsInOneDay(24 * 60 * 60);

/*!
	\class ClockAlarmListModel

	This is the source model class for the clock main view. This owns
	a QStandardItemModel which is a model for the data containing alarms.
 */

/*!
	Constructor.

	\param parent Parent of type QObject.
 */
ClockAlarmListModel::ClockAlarmListModel(
		ClockAppControllerIf &controllerIf, QObject *parent)
:QObject(parent),
 mSourceModel(0),
 mAppControllerIf(controllerIf)
{
	OstTraceFunctionEntry0( CLOCKALARMLISTMODEL_CLOCKALARMLISTMODEL_ENTRY );
	// Construct the source model.
	mSourceModel = new QStandardItemModel(0, 1, this);
	
	// Register to listen to the alarm state changes.
	mAppControllerIf.alarmClient()->startListener();
	connect(
			mAppControllerIf.alarmClient(), SIGNAL(alarmStateChanged(int)),
			this, SLOT(updateSourceModel(int)));
	
	connect(
			mAppControllerIf.timezoneClient(), SIGNAL(timechanged()),
			this, SLOT(updateRemainingTime()));
	// Start a timer. For updating the remaining alarm time.
	mTickTimer = new QTimer(this);
	connect(
			mTickTimer, SIGNAL(timeout()),
			this, SLOT(updateRemainingTime()));
	OstTraceFunctionExit0( CLOCKALARMLISTMODEL_CLOCKALARMLISTMODEL_EXIT );
}

/*!
	Destructor.
 */
ClockAlarmListModel::~ClockAlarmListModel()
{
	OstTraceFunctionEntry0( DUP1_CLOCKALARMLISTMODEL_CLOCKALARMLISTMODEL_ENTRY );
	if (mSourceModel) {
		delete mSourceModel;
		mSourceModel = 0;
	}
	OstTraceFunctionExit0( DUP1_CLOCKALARMLISTMODEL_CLOCKALARMLISTMODEL_EXIT );
}

/*!
	Populates the source model & starts the timer for 1 minute.
 */
void ClockAlarmListModel::populateModel()
{
	OstTraceFunctionEntry0( CLOCKALARMLISTMODEL_POPULATEMODEL_ENTRY );
	// Populate the model in a different thread.
	QTimer::singleShot(1, this, SLOT(populateSourceModel()));
	OstTraceFunctionExit0( CLOCKALARMLISTMODEL_POPULATEMODEL_EXIT );
}
/*!
	Returns the source model to be used with clock main view.

	\return QAbstractItemModel
	\sa QAbstractItemModel, HbListView.
 */
QAbstractItemModel *ClockAlarmListModel::sourceModel()
{
	OstTraceFunctionEntry0( CLOCKALARMLISTMODEL_SOURCEMODEL_ENTRY );
	OstTraceFunctionExit0( CLOCKALARMLISTMODEL_SOURCEMODEL_EXIT );
	return mSourceModel;
}

/*!
	Populates the source model.
 */
void ClockAlarmListModel::populateSourceModel()
{
	OstTraceFunctionEntry0( CLOCKALARMLISTMODEL_POPULATESOURCEMODEL_ENTRY );
	// Clear the model if it has any data already.
	mSourceModel->clear();
	mSourceModel->setColumnCount(1);
	
	QList<AlarmInfo> alarmInfoList;
	
	// Get the list of pending clock alarms.
	mAppControllerIf.alarmClient()->getAlarmList(alarmInfoList);
	
	// First create a list of alarms with the order of active & inactive alarms.
	AlarmInfo alarminfo;
	for (int index = 0; index < alarmInfoList.count(); index++) {
		alarminfo = alarmInfoList[index];
		if (Enabled == alarminfo.alarmStatus) {
			// Append alarm into the source model.
			appendAlarmToModel(alarminfo);
		}
	}
	for (int index = 0; index < alarmInfoList.count(); index++) {
		alarminfo = alarmInfoList[index];
		if (Disabled == alarminfo.alarmStatus) {
			// Append alarm into the source model.
			appendAlarmToModel(alarminfo);
		}
	}
	
	if (0 < getActiveAlarmCount() && !mTickTimer->isActive()) {
		// Start the Timer for 1 minute.
		mTickTimer->start(60000 - 1000 * QTime::currentTime().second());
	}
	OstTraceFunctionExit0( CLOCKALARMLISTMODEL_POPULATESOURCEMODEL_EXIT );
}

/*!
	Calculates the remaining time for the given alarm.

	\param alarmInfo AlarmInfo containing the active alarm.
 */
QString ClockAlarmListModel::calculateRemainingTime(AlarmInfo alarmInfo)
{
	OstTraceFunctionEntry0( CLOCKALARMLISTMODEL_CALCULATEREMAININGTIME_ENTRY );
	QDateTime currentDateTime = QDateTime::currentDateTime();
	QDateTime alarmTime = QDateTime(
			alarmInfo.alarmDateTime, alarmInfo.nextDueTime);
	QString timeNote;

	// TODO: method getRemainingSeconds is used since secsTo API is
	// not returning the proper value.
	// int remianSecs = currentDateTime.secsTo(alarmTime);
	int remainSecs = getRemainingSeconds(alarmTime);
	QDateTime timeLeft(QDate(2009,1,1), QTime(0,0)); //do -1 for day always.

	QDateTime temp = timeLeft.addSecs(remainSecs);

	int hourleft = temp.time().hour();
	int minleft = temp.time().minute();
	int secleft = temp.time().second();
	int dayleft = temp.date().day() - 1;

	if (dayleft >= 1) {
		hourleft += (dayleft * 24);
	}
	QString formatTimeNote;
	
	if (hourleft <= 48) {
		switch (hourleft) {
			case 1: {
				if (1 == minleft) {
					formatTimeNote =
							HbParameterLengthLimiter(hbTrId("txt_clock_main_view_setlabel_in_1hr_2min"))
							.arg(QString::number(hourleft)).arg(QString::number(minleft));
				} else {
					formatTimeNote =
							HbParameterLengthLimiter(hbTrId("txt_clock_main_view_setlabel_in_1hr_2mins"))
							.arg(QString::number(hourleft)).arg(QString::number(minleft));
				}
			}
			break;

			default: {
				if (1 == minleft) {
					formatTimeNote =
						HbParameterLengthLimiter(hbTrId("txt_clock_main_view_setlabel_in_1hrs_2min"))
						.arg(QString::number(hourleft)).arg(QString::number(minleft));
				} else {
					formatTimeNote =
						HbParameterLengthLimiter(hbTrId("txt_clock_main_view_setlabel_in_1hrs_2mins"))
						.arg(QString::number(hourleft)).arg(QString::number(minleft));
				}
			}
		}
	} else {
		formatTimeNote = HbParameterLengthLimiter(hbTrId("txt_clock_main_view_setlabel_in_1days"))
				         .arg(QString::number(dayleft));
	}
	OstTraceFunctionExit0( CLOCKALARMLISTMODEL_CALCULATEREMAININGTIME_EXIT );
	return formatTimeNote;
}

/*!
	Gets remaining seconds to the alarm time from current date time.

	\param alarmDateTime reference to the alarm date time.
	\return int returns the remaining seconds calculated.
 */
int ClockAlarmListModel::getRemainingSeconds(QDateTime &alarmDateTime)
{
	OstTraceFunctionEntry0( CLOCKALARMLISTMODEL_GETREMAININGSECONDS_ENTRY );
	int remainingSeconds;
	QDateTime currentDateTime = QDateTime::currentDateTime();

	int currentTimeInSec =
			(currentDateTime.time().hour()
					* KOneHourInMinute * KOneMinuteInSecons)
			+ (currentDateTime.time().minute() * KOneMinuteInSecons)
			+ currentDateTime.time().second();

	int alarmTimeInSec =
			alarmDateTime.time().hour() * KOneHourInMinute * KOneMinuteInSecons
			+ alarmDateTime.time().minute() * KOneMinuteInSecons
			+ alarmDateTime.time().second();

	int dayInterval =
			alarmDateTime.date().day() - currentDateTime.date().day();

	if (dayInterval) {
		alarmTimeInSec += (dayInterval*KSecondsInOneDay);
	}

	remainingSeconds = alarmTimeInSec - currentTimeInSec;
	OstTraceFunctionExit0( CLOCKALARMLISTMODEL_GETREMAININGSECONDS_EXIT );
	return remainingSeconds;
}

/*!
	Slot which Updates the source model with the given alarm `id'.
	The alarm could be newly added or deleted or have undergone a status change
	or have just been modified.

	\param id of alarm, identifies the alarm which was changed.
 */
void ClockAlarmListModel::updateSourceModel(int alarmId)
{
	OstTraceFunctionEntry0( CLOCKALARMLISTMODEL_UPDATESOURCEMODEL_ENTRY );
	Q_UNUSED(alarmId)
	int alarmInfoCount;
	int modelCount;
	bool newAlarm = false;
	bool alarmDeleted = false;
	
	// Get the list of pending clock alarms from server.
	QList<AlarmInfo> alarmInfoList;
	mAppControllerIf.alarmClient()->getAlarmList(alarmInfoList);
	modelCount = mSourceModel->rowCount();
	
	// First create a list of alarms with the order of active & inactive alarms.
	QList<AlarmInfo> alarmDisplayList;
	AlarmInfo alarminfo;
	for (int index = 0; index < alarmInfoList.count(); index++) {
		alarminfo = alarmInfoList[index];
		if (Enabled == alarminfo.alarmStatus
				&& Notified != alarminfo.alarmState) {
			// Append only active alarms to the alarmDisplayList.
			alarmDisplayList.append(alarminfo);
		}
	}
	for (int index = 0; index < alarmInfoList.count(); index++) {
		alarminfo = alarmInfoList[index];
		if (Disabled == alarminfo.alarmStatus) {
			// Append only active alarms to the alarmDisplayList.
			alarmDisplayList.append(alarminfo);
		}
	}
	alarmInfoCount = alarmDisplayList.count();
	
	AlarmInfo alarmInfo;
	int index = 0;
	
	if (alarmInfoCount > modelCount) {
		// If alarm count from server is more than in model then, a new alarm
		// has been queued into server.
		newAlarm = true;
	} else if (alarmInfoCount < modelCount) {
		// If alarm count from server is less than in model then, an alarm
		// has been deleted from server.
		alarmDeleted = true;
	}
	if (0 < modelCount) {
		int maxCount = 0;
		if (newAlarm) {
			maxCount = modelCount;
		} else if (alarmDeleted) {
			maxCount = alarmInfoCount;
		} else {
			maxCount = alarmInfoCount;
		}
		// Update the alarm details.
		for (; index < maxCount; index++) {
			alarmInfo = alarmDisplayList[index];
			QModelIndex modelIndex = mSourceModel->index(index,0);
			if (modelIndex.isValid()) {
				updateAlarmDetails(modelIndex, alarmInfo);
			}
		}
	}
	if (newAlarm) {
		emit changeAlarmListDisplay();
		alarmInfo = alarmDisplayList[index];
		appendAlarmToModel(alarmInfo);
	}
	if (alarmDeleted) {
		QModelIndex modelIndex = mSourceModel->index(index, 0);
		if (modelIndex.isValid()) {
			mSourceModel->removeRow(modelIndex.row());
		}
		// If alarm count is "0" then hide the alarmlist in the view.
		if (0 == mSourceModel->rowCount()) {
			emit changeAlarmListDisplay();
		}
	}
	if (0 < getActiveAlarmCount()) {
		if (!mTickTimer->isActive()) {
			// Start the Timer for 1 minute.
			mTickTimer->start(60000 - 1000 * QTime::currentTime().second());
		}
	} else {
		if (mTickTimer->isActive()) {
			// Stop the timer if no more active alarms are present.
			mTickTimer->stop();
		}
	}
	OstTraceFunctionExit0( CLOCKALARMLISTMODEL_UPDATESOURCEMODEL_EXIT );
}

/*!
	Slot which Updates the remaining time field in the alarms list for all the
	alarms.
	
 */
void ClockAlarmListModel::updateRemainingTime()
{
	OstTraceFunctionEntry0( CLOCKALARMLISTMODEL_UPDATEREMAININGTIME_ENTRY );
	// Get the list of pending clock alarms from server.
	AlarmInfo alarmInfo;
	for (int row = 0; row < mSourceModel->rowCount(); row++) {
		// Get the data for the alarm.
		QList<QVariant> alarmData =
				mSourceModel->index(row, 0).data(AlarmDetails).toList();
		int alarmId = alarmData.at(0).value<int>();

		int error = mAppControllerIf.alarmClient()->getAlarmInfo(
				alarmId, alarmInfo);
		if (-1 != error) {
			if (Enabled == alarmInfo.alarmStatus) {
				// Get the model index.
				QModelIndex mdlIndex = mSourceModel->index(row, 0);
				if (Notified != alarmInfo.alarmState) {
					QStringList displayStringList =
							getDisplayStringListforAlarmItem(alarmInfo);
					mSourceModel->setData(
							mdlIndex, displayStringList, Qt::DisplayRole);
				}
			}
		}
	}
	// Start the Timer for 1 minute.
	mTickTimer->start(60000);
	OstTraceFunctionExit0( CLOCKALARMLISTMODEL_UPDATEREMAININGTIME_EXIT );
}

/*!
	Returns the stringList to be set for "displayRole" for the alarm item.

	\param alarmInfo AlarmInfo containing the alarm.
	
	\return QStringList contains the stringList to be set.
 */
QStringList ClockAlarmListModel::getDisplayStringListforAlarmItem(
		AlarmInfo alarmInfo)
{
	OstTraceFunctionEntry0( CLOCKALARMLISTMODEL_GETDISPLAYSTRINGLISTFORALARMITEM_ENTRY );
	QStringList displayStringList;
	QString timeString;
	if (Snoozed == alarmInfo.alarmState) {
		timeString = alarmInfo.nextDueTime.toString(
				mAppControllerIf.settingsUtility()->timeFormatString());
	} else {
		timeString = alarmInfo.origAlarmTime.toString(
				mAppControllerIf.settingsUtility()->timeFormatString());
	}
	
	QStringList stringList = timeString.split(" ");
	QString time;
	time.append(stringList[0]);
	if (stringList.count() > 1) {
		time.append("\n");
		time.append(stringList[1]);
	}
	displayStringList.append(time);
	
	if (Enabled == alarmInfo.alarmStatus) {
		QString remainingTime = calculateRemainingTime(alarmInfo);
		displayStringList.append(remainingTime);
	} else {
		displayStringList.append(
		        hbTrId("txt_clock_main_view_setlabel_inactive"));
	}

	QString alarmDescription = alarmInfo.alarmDesc;
	displayStringList.append(alarmDescription);

	QString repeatTypeString;
	// The repeat type string.
	if (Daily == alarmInfo.repeatType) {
		repeatTypeString.append(hbTrId("txt_clock_main_view_dblist_daily"));
	} else if (Workday == alarmInfo.repeatType) {
		repeatTypeString.append(
		    hbTrId("txt_clock_main_view_dblist_daily_val_workdays"));
	} else if (Weekly == alarmInfo.repeatType) {
		repeatTypeString.append( HbParameterLengthLimiter(hbTrId("txt_clock_main_view_setlabel_every_1"))
					.arg(alarmInfo.alarmDateTime.toString("dddd")));
	} else {
		if (QDate::currentDate() == alarmInfo.alarmDateTime) {
			repeatTypeString.append(
			    hbTrId("txt_clock_main_view_dblist_daily_val_today"));
		} else {
			repeatTypeString.append(
					alarmInfo.alarmDateTime.toString("dddd"));
		}
	}
	displayStringList.append(repeatTypeString);
	OstTraceFunctionExit0( CLOCKALARMLISTMODEL_GETDISPLAYSTRINGLISTFORALARMITEM_EXIT );
	return displayStringList;
}

/*!
	Appends an alarm to the source model.

	\param alarmInfo AlarmInfo containing the alarm.
 */
void ClockAlarmListModel::appendAlarmToModel(AlarmInfo alarmInfo)
{
	OstTraceFunctionEntry0( CLOCKALARMLISTMODEL_APPENDALARMTOMODEL_ENTRY );
	// Append only pending alarms to the model.
	if (Notified != alarmInfo.alarmState) {
		QStandardItem *item = new QStandardItem();
		QStringList displayStringList =
				getDisplayStringListforAlarmItem(alarmInfo);
		
		item->setData(displayStringList, Qt::DisplayRole);
		
		// The repeat icon.
		QList<QVariant> displayiconList;
		if (Weekly == alarmInfo.repeatType ||
				Daily == alarmInfo.repeatType ||
				Workday == alarmInfo.repeatType) {
			displayiconList.append(HbIcon("qtg_mono_repeat"));
		} else {
			displayiconList.append(QVariant());
		}
		item->setData(displayiconList, Qt::DecorationRole);
		

		// Set the icon for active/in-active/snoozed alarm.
		if (Disabled == alarmInfo.alarmStatus) {
			item->setData(QString("qtg_mono_alarm_inactive"), AlarmIcon);
		} else if (Snoozed == alarmInfo.alarmState) {
			// Change the alarm icon also to alarm snooze icon
			item->setData(QString("qtg_mono_alarm_snooze"), AlarmIcon);
		} else {
			item->setData(QString("qtg_mono_alarm"), AlarmIcon);
		}

		// The model for the list item will have:
		// 1. Id of the alarm that the list item represents.
		// 2. The repeat type of the alarm (Recurrence type).
		// 3. TAlarmStatus value.
		QList<QVariant> alarmData;
		alarmData.append(alarmInfo.alarmId);
		alarmData.append(alarmInfo.repeatType);
		alarmData.append(alarmInfo.alarmStatus);
		item->setData(alarmData, AlarmDetails);
		
		mSourceModel->appendRow(item);
	}

	OstTraceFunctionExit0( CLOCKALARMLISTMODEL_APPENDALARMTOMODEL_EXIT );
}

/*!
	Updates the alarm details in the source model with updated 
	details from server.
	
	\param item Contains the alarm to be updated.
	\param alarmInfo AlarmInfo containing the alarm.
 */
void ClockAlarmListModel::updateAlarmDetails(
		QModelIndex modelIndex, AlarmInfo alarmInfo)
{
	OstTraceFunctionEntry0( CLOCKALARMLISTMODEL_UPDATEALARMDETAILS_ENTRY );
	if (Notified != alarmInfo.alarmState) {
		QStringList displayStringList =
				getDisplayStringListforAlarmItem(alarmInfo);
		
		mSourceModel->setData(
				modelIndex, displayStringList, Qt::DisplayRole);
		
		// The repeat icon.
		QList<QVariant> displayiconList;
		if (Weekly == alarmInfo.repeatType ||
				Daily == alarmInfo.repeatType ||
				Workday == alarmInfo.repeatType) {
			displayiconList.append(HbIcon("qtg_mono_repeat"));
		} else {
			displayiconList.append(QVariant());
		}
		mSourceModel->setData(
				modelIndex, displayiconList, Qt::DecorationRole);
		
		// Set the icon for active/in-active/snoozed alarm.
		if (Disabled == alarmInfo.alarmStatus) {
			mSourceModel->setData(modelIndex, 
			    QString("qtg_mono_alarm_inactive"), AlarmIcon);
		} else if (Snoozed == alarmInfo.alarmState) {
			// Change the alarm icon also to alarm snooze icon
			mSourceModel->setData(modelIndex, 
			    QString("qtg_mono_alarm_snooze"), AlarmIcon);
		} else {
			mSourceModel->setData(modelIndex, QString("qtg_mono_alarm"), AlarmIcon);
		}

		// The model for the list item will have:
		// 1. Id of the alarm that the list item represents.
		// 2. The repeat type of the alarm (Recurrence type).
		// 3. TAlarmStatus value.
		QList<QVariant> alarmData;
		alarmData.append(alarmInfo.alarmId);
		alarmData.append(alarmInfo.repeatType);
		alarmData.append(alarmInfo.alarmStatus);
		mSourceModel->setData(modelIndex, alarmData, AlarmDetails);
	}
	OstTraceFunctionExit0( CLOCKALARMLISTMODEL_UPDATEALARMDETAILS_EXIT );
}

/*!
	Returns the number of active alarms present in the model.

	\return int number of active alarms present in the model.
 */
int ClockAlarmListModel::getActiveAlarmCount()
{
	OstTraceFunctionEntry0( CLOCKALARMLISTMODEL_GETACTIVEALARMCOUNT_ENTRY );
	int activeAlarmCount = 0;
	for (int index = 0; index < mSourceModel->rowCount(); index++) {
		// Get the data for the alarm.
		QList<QVariant> alarmData = mSourceModel->
				index(index, 0).data(AlarmDetails).toList();
		if (Enabled == alarmData.at(2).value<int>()) {
			activeAlarmCount++;
		}
	}
	OstTraceFunctionExit0( CLOCKALARMLISTMODEL_GETACTIVEALARMCOUNT_EXIT );
	return activeAlarmCount;
}
// End of file	--Don't remove this.
