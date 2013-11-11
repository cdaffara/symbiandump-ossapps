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
* The header file for CalenEditorReminderField class.
*
*/

#ifndef CALENEDITORREMINDERFIELD_H
#define CALENEDITORREMINDERFIELD_H

// System Includes
#include <QObject>
#include <qglobal.h>

// User Includes
#include "caleneditor_p.h"

// FORWARD DECLARATIONS
class HbDataFormModelItem;
class HbDataForm;
class HbDataFormModelItem;
class HbDateTimePicker;
class CalenEditorDataHandler;

class CalenEditorReminderField : public QObject
{
	Q_OBJECT
	
public:
	enum AllDayReminderIndex 
		{
		ReminderOff = 0,
		ReminderOnEventDay,
		ReminderOneDayBefore,
		ReminderTwoDaysBefore
		};
	enum MeetingReminderIndex
		{
		//Starting the enum from 1 bcoz ReminderOff of AllDayReminderIndex can be used.
		ReminderAtStart = 1,
		Reminder15MinsBefore,
		Reminder30MinsBefore,
		Reminder1HourBefore
		};
public:
	CalenEditorReminderField(CalenEditorPrivate* calenEditor,
	                         HbDataForm* form, HbDataFormModel* model,
	                         QObject *parent = NULL);
	virtual ~CalenEditorReminderField();

public:
	void setReminderChoices();
	void addItemToModel();
	void removeItemFromModel();
	void populateReminderItem(bool newEntry);
	QModelIndex modelIndex();
	void setReminderOff();
	void setDefaultAlarmForAllDay();
	void updateReminderChoicesForAllDay(QDate repeatUntilDate);
	void insertReminderTimeField();
	void setDisplayTime();
	void removeReminderTimeField();
	bool isReminderFieldEnabled();
	int reminderItemsCount();
	int currentReminderIndex();
	void setCurrentIndex(int index);
	void disableReminderTimeField();
	bool isReminderTimeForAllDayAdded();
	void UpdateReminderChoicesForSameDay(QTime startTime);
	void setSavedMeetingReminderIndex();
	int getReminderIndexBasedOnEntryAlarm();
	
private slots:
	void handleReminderIndexChanged(int index);
	void launchReminderTimePicker();
	void setReminderTimeForAllDay();
private:
	CalenEditorPrivate* mCalenEditor;
	HbDataForm* mEditorForm;
	HbDataFormModel* mCalenEditorModel;
	HbDataFormModelItem* mReminderItem;
	HbDataFormModelItem *mCustomReminderTimeItem;
	QPointer<HbDateTimePicker> mTimePicker;
	QTime mReminderTimeForAllDay;
	QHash<int, int> mReminderHash;
	bool mReminderTimeAdded;
};

#endif // CALENEDITORREMINDERFIELD_H

// End of file	--Don't remove this.
