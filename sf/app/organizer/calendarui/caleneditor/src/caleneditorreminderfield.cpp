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
* Definition of CalenEditorReminderField class.
*
*/


// System Includes
#include <QDate>

#include <hbdataformmodelitem.h>
#include <hbdataformmodel.h>
#include <hbdataform.h>
#include <hbdatetimepicker.h>
#include <hbextendedlocale.h>
#include <hbi18ndef.h>
#include <hbdialog.h>
#include <hblabel.h>
#include <hbaction.h>

// User Includes
#include "calenagendautils.h"
#include "caleneditorreminderfield.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "caleneditorreminderfieldTraces.h"
#endif


#define numberOfMinutesInADay 1440
/*!
	\class CalenEditorReminderField
 */
/*!
	Constructor.

	\param parent QObject pointer
 */

CalenEditorReminderField::CalenEditorReminderField(
										CalenEditorPrivate* calenEditor,
										HbDataForm* form, 
										HbDataFormModel* model,
										QObject *parent)
	:QObject(parent),
	 mCalenEditor(calenEditor), 
	 mEditorForm(form), 
	 mCalenEditorModel(model),
	 mCustomReminderTimeItem(0),
	 mTimePicker(0),
	 mReminderTimeAdded(false)
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_CALENEDITORREMINDERFIELD_ENTRY );
	mReminderItem = new HbDataFormModelItem();
	mReminderItem->setType(HbDataFormModelItem::ComboBoxItem);
	mReminderItem->setData(HbDataFormModelItem::LabelRole,
						   hbTrId("txt_calendar_setlabel_reminder"));	
	// Add it to the model
	mCalenEditorModel->appendDataFormItem(mReminderItem,
										mCalenEditorModel->invisibleRootItem());
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_CALENEDITORREMINDERFIELD_EXIT );
}

/*!
	 Destructor
 */
CalenEditorReminderField::~CalenEditorReminderField()
{
	OstTraceFunctionEntry0( DUP1_CALENEDITORREMINDERFIELD_CALENEDITORREMINDERFIELD_ENTRY );
	if(mReminderHash.count()) {
		mReminderHash.clear();
	}
	OstTraceFunctionExit0( DUP1_CALENEDITORREMINDERFIELD_CALENEDITORREMINDERFIELD_EXIT );
}

/*!
	 Set the reminder choices.
 */
void CalenEditorReminderField::setReminderChoices()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_SETREMINDERCHOICES_ENTRY );
	
	// Disconnect the slot and connect it back again at end to avoid unnecessary
	// calls to handleReminderIndexChanged slot. Or else the slot gets called 
	// when we add all of items to the repeat combobox.
	mEditorForm->removeConnection(mReminderItem,
							SIGNAL(currentIndexChanged(int)), this,
							SLOT(handleReminderIndexChanged(int)));
	// Create the reminder choices
	QStringList reminderChoices;
	reminderChoices << hbTrId("txt_calendar_setlabel_reminder_val_off")
				<< hbTrId("txt_calendar_setlabel_reminder_val_at_the_start")
				<< hbTrId("txt_calendar_setlabel_reminder_val_15_minutes_befo")
				<< hbTrId("txt_calendar_setlabel_reminder_val_30_minutes_befo")
				<< hbTrId("txt_calendar_setlabel_reminder_val_1_hour_before");

	mReminderItem->setContentWidgetData("items", reminderChoices);
	mReminderItem->setEnabled(true);
	
	mEditorForm->addConnection(mReminderItem,
							SIGNAL(currentIndexChanged(int)), this,
							SLOT(handleReminderIndexChanged(int)));
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_SETREMINDERCHOICES_EXIT );
}

/*!
	 Adds reminder item to the model
 */
void CalenEditorReminderField::addItemToModel()
{	
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_ADDITEMTOMODEL_ENTRY );
	// Add reminder to the model
	mCalenEditorModel->appendDataFormItem( mReminderItem,
				   mCalenEditorModel->invisibleRootItem());
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_ADDITEMTOMODEL_EXIT );
}

/*!
	 Removes reminder item from the model
 */
void CalenEditorReminderField::removeItemFromModel()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_REMOVEITEMFROMMODEL_ENTRY );
	mCalenEditorModel->removeItem(modelIndex());
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_REMOVEITEMFROMMODEL_EXIT );
}

/*!
	 Set the current reminder index with the value which was saved 
	 before, if it is valid or else default the value to 15MinsBefore
	 or AtTheStart whichever is appropriate
 */
void CalenEditorReminderField::setSavedMeetingReminderIndex()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_SETSAVEDMEETINGREMINDERINDEX_ENTRY );
	// Get the reminder offset value.
	int reminderOffset =
	mCalenEditor->editedEntry()->alarm().timeOffset();
	// Get the index value for the reminder combo box from the hash 
	// table. 2nd argument is defaultKey if the hash contains no item mapped to value
	//TODO: Need to confirm the default reminder to set if none of the choices are met like in case of synch from outlook.
	int index = mReminderHash.key(reminderOffset, Reminder15MinsBefore); 
	
	if (index < reminderItemsCount())
		setCurrentIndex(index);
	else if (reminderItemsCount() == (ReminderAtStart + 1)) //+1 because enum starts from 0.
		setCurrentIndex(ReminderAtStart); 
	else
		setCurrentIndex(Reminder15MinsBefore);
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_SETSAVEDMEETINGREMINDERINDEX_EXIT );
}

/*!
	 Populates reminder item with available choices to the user
	 \param newEntry bool value to indicate if its a new entry
 */
void CalenEditorReminderField::populateReminderItem(bool newEntry)
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_POPULATEREMINDERITEM_ENTRY );
	AgendaAlarm reminder;
	bool pastEvent =  false;
	bool repeatingEntry = false;
	bool sameDay = false;
	
	mReminderItem->setContentWidgetData("objectName", "remainderItem");
	// Build the hash map for the reminder.
	mReminderHash[ReminderOff] = -1; // OFF.
	mReminderHash[ReminderAtStart] = 0;
	mReminderHash[Reminder15MinsBefore] = 15;
	mReminderHash[Reminder30MinsBefore] = 30;
	mReminderHash[Reminder1HourBefore] = 60;
	    
	// Set reference date to start date or repeat until date accordingly to 
	// decide whether its a past event or not.
	QDate referenceDate;
	if (!mCalenEditor->editedEntry()->isRepeating() || 
					(mCalenEditor->isEditRangeThisOnly())) {
		referenceDate = mCalenEditor->editedEntry()->startTime().date();
	} else {
		referenceDate = mCalenEditor->editedEntry()->repeatRule().until().date();
		repeatingEntry = true;
	}
	
	if ((referenceDate < QDate::currentDate()) || 
					(referenceDate == QDate::currentDate()
					&& (mCalenEditor->editedEntry()->startTime().time() < 
					QTime::currentTime()))) {
		pastEvent = true;
	}
	
	if (mCalenEditor->isAllDayEvent()) {
		updateReminderChoicesForAllDay(referenceDate);
	} else {
		if((referenceDate == QDate::currentDate())
					&& !pastEvent) {
			UpdateReminderChoicesForSameDay(mCalenEditor->editedEntry()->startTime().time());
			sameDay = true;
		}
		else {
		setReminderChoices();
		}
	}
	// Set the default reminder value to 15 minutes 
	if (newEntry) {
		if (!pastEvent) {
			int defaultIndex = Reminder15MinsBefore;
			if (reminderItemsCount() == (ReminderAtStart + 1))  //If 15MinsBefore option is not available
				defaultIndex = ReminderAtStart;
		
			setCurrentIndex(defaultIndex);
			// Save the reminder alarm for the entry
			reminder.setTimeOffset(mReminderHash.value(defaultIndex));
			reminder.setAlarmSoundName(QString(" "));
			// Set the reminder to the entry as well as original entry.
			mCalenEditor->editedEntry()->setAlarm(reminder);
			mCalenEditor->originalEntry()->setAlarm(reminder);
		} else {
			setReminderOff();
		}
	} else {
        int offsetInMins = mCalenEditor->editedEntry()->alarm().timeOffset();
		// If the alarm is not null,
		// Check if all day event or not and then set the choices accordingly.
		if (mCalenEditor->editedEntry()->alarm().isNull()) {
			// Alarm is set off
			setCurrentIndex(ReminderOff);
			if(mReminderTimeAdded) {
				removeReminderTimeField();
			}
		} else if (!mCalenEditor->isAllDayEvent()) {
			QTime currentTime = QTime::currentTime();

			//TODO: Still need confirmation for proper behaviour when entry is edited after alarm has expired.
			if (!repeatingEntry && sameDay && (currentTime.addSecs(offsetInMins * 60) >= mCalenEditor->editedEntry()->startTime().time())) {
				setCurrentIndex(ReminderOff); //Alarm has expired already, so making it off.
				//The slot for index 0 is not called, since after UpdateReminderChoicesForSameDay()
				//index is 0 itself and there is no change. So explicitly setting the reminder here.
				reminder.setTimeOffset(mReminderHash.value(ReminderOff));
				reminder.setAlarmSoundName(QString(" "));
				mCalenEditor->editedEntry()->setAlarm(reminder);
			}
			else {
				setSavedMeetingReminderIndex();
			}
		} else {
			// Insert reminder time field and display entry's reminder time.
			// If past then disable the field.
			if (!mReminderTimeAdded) {
				insertReminderTimeField();
			}
			if (pastEvent && mReminderTimeAdded) {
				mCustomReminderTimeItem->setEnabled(false);
			}
			// Get the appropriate reminder index depending on the value of time offset.
			int index = getReminderIndexBasedOnEntryAlarm();
			mReminderItem->setContentWidgetData("currentIndex", index);
			setDisplayTime();
		}
		if (pastEvent) {
			mReminderItem->setEnabled(false);
		}
	}
	mEditorForm->addConnection(mReminderItem,
							SIGNAL(currentIndexChanged(int)), this,
							SLOT(handleReminderIndexChanged(int)));
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_POPULATEREMINDERITEM_EXIT );
}

/*!
	Triggered from tapping on reminder item.
	Handles the reminder time change and updates the same in the event.
	\param index The new index chosen in the reminder list.
 */
void CalenEditorReminderField::handleReminderIndexChanged(int index)
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_HANDLEREMINDERINDEXCHANGED_ENTRY );
	AgendaAlarm reminder;
	if (!mCalenEditor->editedEntry()->alarm().isNull()) {
		reminder = mCalenEditor->editedEntry()->alarm();
	}
	// Check whether all day event or not and store appropriately.
	if (!mCalenEditor->isAllDayEvent()) {
		// If value for the index in hash table is -1 i.e reminder is "OFF",
		// then set the default constructed reminder to
		// the entry which is Null.
		if (mReminderHash.value(index) < 0) {
			// Construct the default alarm which is NULL
			reminder = AgendaAlarm();
		} else {
			// If not zero then set the reminder offset
			// value to the entry.
			reminder.setTimeOffset(mReminderHash.value(index));
			reminder.setAlarmSoundName(QString(" "));
		}
	} else {
		QDateTime reminderDateTimeForAllDay;
		QDateTime
		        startDateTimeForAllDay(
		                               mCalenEditor->editedEntry()->startTime().date(),
		                               QTime(0, 0, 0));
		if (mReminderItem->contentWidgetData("currentIndex") == ReminderOff) {
			reminder = AgendaAlarm();
			removeReminderTimeField();
		} else {
			int offset = 0;
			if (mReminderItem->contentWidgetData("currentIndex") == ReminderOneDayBefore) {
				offset = 1;
			} else if (mReminderItem->contentWidgetData("currentIndex") == ReminderTwoDaysBefore) {
				offset = 2;
			}
			if(!mReminderTimeAdded) {
				insertReminderTimeField();
			}
			// If on same day as that of the event then check if time has been 
			// changed , if changed retain that else set default time.
			if (offset == 0) {
				if(mReminderTimeForAllDay == QTime(18, 0, 0, 0)) {
					mReminderTimeForAllDay.setHMS(8, 0, 0);
				}
			}else {
				// For the reminder options other than ReminderOnEventDay
				// reset the default values since the time set for one option 
				// may not be valid for the other option
				mReminderTimeForAllDay.setHMS(18, 0, 0, 0);
			}
			setDisplayTime();
			reminderDateTimeForAllDay.setDate(
					mCalenEditor->editedEntry()->startTime().date().addDays(
																	-offset));
			reminderDateTimeForAllDay.setTime(mReminderTimeForAllDay);
			int seconds =
					reminderDateTimeForAllDay.secsTo(startDateTimeForAllDay);
			int timeOffset = seconds / 60;
			mCustomReminderTimeItem->setEnabled(true);
			reminder.setTimeOffset(timeOffset);
			reminder.setAlarmSoundName(QString(" "));
		}
	}
	// Set the reminder to the entry.
	mCalenEditor->editedEntry()->setAlarm(reminder);
	if(!mCalenEditor->isNewEntry()) {
	mCalenEditor->addDiscardAction();
	}
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_HANDLEREMINDERINDEXCHANGED_EXIT );
}

/*!
	 Returns the mode index of the reminder item
	 \return Mode index of the reminder item
 */
QModelIndex CalenEditorReminderField::modelIndex()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_MODELINDEX_ENTRY );
	return mCalenEditorModel->indexFromItem(mReminderItem);
}

/*!
	 Set reminder off.
 */
void CalenEditorReminderField::setReminderOff()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_SETREMINDEROFF_ENTRY );
	// Create the remindar choices
	QStringList reminderChoices;
	reminderChoices << hbTrId("txt_calendar_setlabel_reminder_val_off");
	mReminderItem->setContentWidgetData(QString("items"), reminderChoices);
	mReminderItem->setEnabled(false); 
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_SETREMINDEROFF_EXIT );
}

/*!
	 Set the default alarm for a new all day event.
 */
void CalenEditorReminderField::setDefaultAlarmForAllDay()
{	
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_SETDEFAULTALARMFORALLDAY_ENTRY );
	// Set default alarm if its a new entry.
	if (mCalenEditor->isNewEntry()) {
		AgendaAlarm reminder;
		QDate defaultDate
				(mCalenEditor->editedEntry()->startTime().date().addDays(-1));
		// Set default time as 6pm of the previous day.
		mReminderTimeForAllDay.setHMS(18, 0, 0, 0);
		setDisplayTime();
		QDateTime startDateTimeForAllDay(
				mCalenEditor->editedEntry()->startTime().date(), QTime(0, 0));
		QDateTime defaultReminderDateTimeForAllDay
				(defaultDate, mReminderTimeForAllDay);
		int offsetInSecs = 
				defaultReminderDateTimeForAllDay.secsTo(startDateTimeForAllDay);
		int offsetInMins = offsetInSecs/60;
		reminder.setTimeOffset(offsetInMins);
		reminder.setAlarmSoundName(QString(" "));
		// Set the reminder to the entry as well as original entry.
		mCalenEditor->editedEntry()->setAlarm(reminder);
		mCalenEditor->originalEntry()->setAlarm(reminder);
	}
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_SETDEFAULTALARMFORALLDAY_EXIT );
}

/*!
	 Update the reminder choices when the meeting is on same day, 
	 based on the time available from current time to start time of the event.
 */
void CalenEditorReminderField::UpdateReminderChoicesForSameDay(QTime startTime)
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_UPDATEREMINDERCHOICESFORSAMEDAY_ENTRY );
	QTime currentTime = QTime::currentTime();
	int offset = currentTime.secsTo(startTime);

	// Disconnect the slot and connect it back again at end to avoid unnecessary
	// calls to handleReminderIndexChanged slot. Or else the slot gets called 
	// when we add all of items to the repeat combobox.
	mEditorForm->removeConnection(mReminderItem,
							SIGNAL(currentIndexChanged(int)), this,
							SLOT(handleReminderIndexChanged(int)));

	//The event start time is within 15mins from current time of today,
	//then we cannot have alarms "15 mins before", "30mins before" and "1 hour before"
	if (offset <= SecsIn15Mins)
	{
		QStringList reminderChoices;
		//First to clear all items, we just set items with empty list.
		mReminderItem->setContentWidgetData("items", reminderChoices); 
		reminderChoices << hbTrId("txt_calendar_setlabel_reminder_val_off")
				<< hbTrId("txt_calendar_setlabel_reminder_val_at_the_start");
		mReminderItem->setContentWidgetData("items", reminderChoices);
	}
	//The event start time is within half and hour from current time of today
	//then we cannot have alarms "30mins before" and "1 hour before"
	else if (offset <= SecsIn30Mins)
	{
		QStringList reminderChoices;
		//First to clear all items, we just set items with empty list.
		mReminderItem->setContentWidgetData("items", reminderChoices);
		reminderChoices << hbTrId("txt_calendar_setlabel_reminder_val_off")
				<< hbTrId("txt_calendar_setlabel_reminder_val_at_the_start")
				<< hbTrId("txt_calendar_setlabel_reminder_val_15_minutes_befo");
		mReminderItem->setContentWidgetData("items", reminderChoices);
	}
	//The event start time is within an hour of the current time of today,
	//then cannot have alarm "1 hour before".
	else if (offset <= SecsInOneHour)
	{
		QStringList reminderChoices;
		//First to clear all items, we just set items with empty list.
		mReminderItem->setContentWidgetData("items", reminderChoices);
		reminderChoices << hbTrId("txt_calendar_setlabel_reminder_val_off")
				<< hbTrId("txt_calendar_setlabel_reminder_val_at_the_start")
				<< hbTrId("txt_calendar_setlabel_reminder_val_15_minutes_befo")
				<< hbTrId("txt_calendar_setlabel_reminder_val_30_minutes_befo");
		mReminderItem->setContentWidgetData("items", reminderChoices);
	}
	// The event start time is more than one hour from current time 
	// then we can have all choices for alarm.
	else
	{
		QStringList reminderChoices;
		//First to clear all items, we just set items with empty list.
		mReminderItem->setContentWidgetData("items", reminderChoices);
		reminderChoices << hbTrId("txt_calendar_setlabel_reminder_val_off")
				<< hbTrId("txt_calendar_setlabel_reminder_val_at_the_start")
				<< hbTrId("txt_calendar_setlabel_reminder_val_15_minutes_befo")
				<< hbTrId("txt_calendar_setlabel_reminder_val_30_minutes_befo")
				<< hbTrId("txt_calendar_setlabel_reminder_val_1_hour_before");
		mReminderItem->setContentWidgetData("items", reminderChoices);
	}
	mReminderItem->setEnabled(true);
	mEditorForm->addConnection(mReminderItem,
							SIGNAL(currentIndexChanged(int)), this,
							SLOT(handleReminderIndexChanged(int)));
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_UPDATEREMINDERCHOICESFORSAMEDAY_EXIT );
}

/*!
	 Updates the reminder choices for an all day event.
	 \param referenceDate to indicate past or not.
 */
void CalenEditorReminderField::updateReminderChoicesForAllDay(QDate referenceDate)
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_UPDATEREMINDERCHOICESFORALLDAY_ENTRY );
	if (!mReminderTimeAdded){
		insertReminderTimeField();
	}
	// This functions rearranges the reminder options based on the date and time
	// To avoid the unecessary call to handleReminderIndexChanged 
	// the connection is removed.
	mEditorForm->removeConnection(mReminderItem,
								SIGNAL(currentIndexChanged(int)), this,
								SLOT(handleReminderIndexChanged(int)));
	
	// Since the combox box is not updated as per the current datetime
	// there can be a conflict between the previousIndex set on the combobox
	// and the actual index. This comes while populating an existing entry.
	// So the index has to be taken from the entry alarm 
	
	AgendaAlarm actualAlarm = mCalenEditor->editedEntry()->alarm();
	// Get the actual alarm index from the entry alarm which is been set
	int actualIndex = getReminderIndexBasedOnEntryAlarm();
	int offsetInMins = actualAlarm.timeOffset();

	// Calculate the alarm time
	QDateTime alarmDateTime;
	alarmDateTime.setDate(referenceDate);
	alarmDateTime.setTime(mCalenEditor->editedEntry()->startTime().time());
	QTime checkTime = alarmDateTime.time().addSecs(-(offsetInMins * 60));
	alarmDateTime.setTime(checkTime);
	
	// The combox is updated based on the reference date which is beed passed.
	// And the reminder time has to be set acoordingly
	QStringList reminderChoicesForAllDay;
	QDate tomorrow = QDate::currentDate().addDays(1);
	QDate theDayAfterTomorrow = QDate::currentDate().addDays(2);

	// If the event is on a past date the default alarm will be off.
	if (referenceDate < QDate::currentDate() || 
			referenceDate == QDate::currentDate()) {
		// Set reminder off for past event.
		reminderChoicesForAllDay << hbTrId("txt_calendar_setlabel_reminder_val_off");
		mReminderItem->setContentWidgetData(QString("items"), 
		                                    reminderChoicesForAllDay);
		mReminderItem->setEnabled(false);		
		// Remove the reminder field if it was added
		if(mReminderTimeAdded) {
			removeReminderTimeField();
		}
	} else if (theDayAfterTomorrow < referenceDate) {
		// If the event is on a future date which is two days after the current date
		// The options are off, on event day, 1 day before and 2 days before
		reminderChoicesForAllDay 
			<< hbTrId("txt_calendar_setlabel_reminder_val_off")
			<< hbTrId("txt_calendar_setlabel_reminder_val_on_event_day")
			<< hbTrId("txt_calendar_setlabel_reminder_val_1_day_before")
			<< hbTrId("txt_calendar_setlabel_reminder_val_2_days_before");
		mReminderItem->setEnabled(true);
		mCustomReminderTimeItem->setEnabled(true);
		mReminderItem->setContentWidgetData(QString("items"), 
		                                    reminderChoicesForAllDay);
	} else if (theDayAfterTomorrow == referenceDate) {
		// If the event is on a future date which is two days after the current date
		// If the current time is before 6.00 pm (default for 2 days before)
		// The options are off, on event day, 1 day before and 2 days before
		if (QTime::currentTime() < QTime(18, 0, 0, 0)) {
			reminderChoicesForAllDay 
				<< hbTrId("txt_calendar_setlabel_reminder_val_off")
				<< hbTrId("txt_calendar_setlabel_reminder_val_on_event_day")
				<< hbTrId("txt_calendar_setlabel_reminder_val_1_day_before")
				<< hbTrId("txt_calendar_setlabel_reminder_val_2_days_before");
		}else {
			// If the event is on a future date which is two days after the current date
			// the current time is after 6.00 pm (default for 2 days before)
			// If its after the default time don't show the 2 days before option
			reminderChoicesForAllDay 
				<< hbTrId("txt_calendar_setlabel_reminder_val_off")
				<< hbTrId("txt_calendar_setlabel_reminder_val_on_event_day")
				<< hbTrId("txt_calendar_setlabel_reminder_val_1_day_before");
		}
		mReminderItem->setEnabled(true);
		mCustomReminderTimeItem->setEnabled(true);
		mReminderItem->setContentWidgetData(QString("items"), 
		                                    reminderChoicesForAllDay);
	}else if (QTime::currentTime() < QTime(18, 0, 0, 0)) {
		// If the event is on a future date which is one day after the current date
		// and current time is before 6.00 pm.
		// The options are off, on event day and 1 day before
		reminderChoicesForAllDay 
			<< hbTrId("txt_calendar_setlabel_reminder_val_off")
			<< hbTrId("txt_calendar_setlabel_reminder_val_on_event_day")
			<< hbTrId("txt_calendar_setlabel_reminder_val_1_day_before");
		mReminderItem->setEnabled(true);
		mCustomReminderTimeItem->setEnabled(true);
		mReminderItem->setContentWidgetData(QString("items"), 
		                                    reminderChoicesForAllDay);
	}else {
		// If the event is on a future date which is one day after the current date
		// and current time is after 6.00 pm.
		// The options are off and on event day
		reminderChoicesForAllDay 
			<< hbTrId("txt_calendar_setlabel_reminder_val_off")
			<< hbTrId("txt_calendar_setlabel_reminder_val_on_event_day");
		mReminderItem->setEnabled(true);
		mCustomReminderTimeItem->setEnabled(true);
		mReminderItem->setContentWidgetData(QString("items"), 
		                                    reminderChoicesForAllDay);
	}
	// Get the count of the reminder options after the rearrangement 
	// based on the date and time 
	int count = reminderItemsCount();
	// Set the reminder index
	
	if (count <= actualIndex || 
			alarmDateTime < QDateTime::currentDateTime() || 
			actualIndex == ReminderOff) {
		// This will be executed when the actual index is not 
		// available in the combobox or the alarm time is already passed
		// or the actual alarm is off
		// So set it as off
		mReminderItem->setContentWidgetData("currentIndex", ReminderOff);
		actualAlarm = AgendaAlarm();
		mCalenEditor->editedEntry()->setAlarm(actualAlarm);
		removeReminderTimeField();
	}else {
		// Set the actual reminder index
		mReminderItem->setContentWidgetData("currentIndex", actualIndex);
		if( actualIndex != ReminderOff) {
			if(!mReminderTimeAdded) {
				insertReminderTimeField();
			}
			mCalenEditor->editedEntry()->setAlarm(actualAlarm);
			// Get the alarm time from the offset
			QTime alarmTime = alarmDateTime.time();
			// Set the alarm time and display it on the button
			mReminderTimeForAllDay.setHMS(
					alarmTime.hour(),alarmTime.minute(),alarmTime.second());
			setDisplayTime();
		}else {
			actualAlarm = AgendaAlarm();
			removeReminderTimeField();
		}
	}
	// Connect the slot for the index change
	mEditorForm->addConnection(mReminderItem,
								SIGNAL(currentIndexChanged(int)), this,
								SLOT(handleReminderIndexChanged(int)));

	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_UPDATEREMINDERCHOICESFORALLDAY_EXIT );
}

/*!
	 Gets the reminder index for all day events based on the alarm
	 which is saved for the entry
 */
int CalenEditorReminderField::getReminderIndexBasedOnEntryAlarm()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_GETREMINDERINDEXBASEDONENTRYALARM_ENTRY );
	// Get the appropriate reminder index depending on the value of time offset.
	AgendaAlarm actualAlarm = mCalenEditor->editedEntry()->alarm();
	QTime referenceTime(0, 0, 0);
	int index = ReminderOff;
	int offsetInMins = actualAlarm.timeOffset();
	if(offsetInMins == -1) {
		index = ReminderOff;
	}else if (offsetInMins < 0 || offsetInMins == 0) {
		index = ReminderOnEventDay;
		mReminderTimeForAllDay = referenceTime.addSecs(-(offsetInMins
				* 60));
	} else if (offsetInMins <= numberOfMinutesInADay) {
		index = ReminderOneDayBefore;
		mReminderTimeForAllDay = referenceTime.addSecs(-(offsetInMins
				* 60));
	} else {
		index = ReminderTwoDaysBefore;
		offsetInMins %= (24 * 60);
		mReminderTimeForAllDay = referenceTime.addSecs(-(offsetInMins
				* 60));
	}
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_GETREMINDERINDEXBASEDONENTRYALARM_EXIT );
	return index;
}

/*!
	 Insert the reminder time field for an all day event.
 */
void CalenEditorReminderField::insertReminderTimeField()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_INSERTREMINDERTIMEFIELD_ENTRY );
	HbDataFormModelItem::DataItemType itemType =
			static_cast<HbDataFormModelItem::DataItemType> (ReminderTimeOffset);
	
	// If all day item is not added then insert at one level before.
	int index = CalenEditorPrivate::ReminderTimeForAllDayItem;
	if(!mCalenEditor->isAllDayFieldAdded()) {
		index = CalenEditorPrivate::ReminderTimeForAllDayItem - 1;
	} 
	mCustomReminderTimeItem = mCalenEditorModel->insertDataFormItem(
							index,
							itemType,
							QString(hbTrId("txt_calendar_setlabel_reminder_time")),
							mCalenEditorModel->invisibleRootItem());
	if (currentReminderIndex() != 1){
		mReminderTimeForAllDay.setHMS(18,0,0,0);
	} else {
		mReminderTimeForAllDay.setHMS(8,0,0,0);
	}
	
	mEditorForm->addConnection(mCustomReminderTimeItem, SIGNAL(clicked()),
	                           this, SLOT(launchReminderTimePicker()));
	setDisplayTime();
	mReminderTimeAdded = true;
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_INSERTREMINDERTIMEFIELD_EXIT );
}

/*!
	 Set the reminder time selected by the user.
 */
void CalenEditorReminderField::setDisplayTime()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_SETDISPLAYTIME_ENTRY );
	HbExtendedLocale locale = HbExtendedLocale::system();
	QString timeString = locale.format(
			mReminderTimeForAllDay,
			r_qtn_time_usual_with_zero);
	mCustomReminderTimeItem->setContentWidgetData("text", timeString);
	
	// If the reminder time picker is open and locale changes happen,
	// we need to refresh them dynamically with proper time formats
	if(!(mTimePicker.isNull())) {
		if(locale.timeStyle() == HbExtendedLocale::Time12) {
			mTimePicker->setDisplayFormat("hh:mm ap");	
		}else {
			mTimePicker->setDisplayFormat("hh:mm");
		}
		mTimePicker->setTime(mReminderTimeForAllDay);
	}
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_SETDISPLAYTIME_EXIT );
}

/*!
	Remove the reminder time field if its not an all day event.
 */
void CalenEditorReminderField::removeReminderTimeField()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_REMOVEREMINDERTIMEFIELD_ENTRY );
	mReminderTimeAdded = false;
	mEditorForm->removeConnection(mCustomReminderTimeItem, SIGNAL(clicked()),
		                           this, SLOT(launchReminderTimePicker()));
	if (mCustomReminderTimeItem) {
		QModelIndex reminderIndex =
				mCalenEditorModel->indexFromItem(mCustomReminderTimeItem);
		mCalenEditorModel->removeItem(
				mCalenEditorModel->index(
						reminderIndex.row(), 0));
		mCustomReminderTimeItem = 0;
	}
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_REMOVEREMINDERTIMEFIELD_EXIT );
}

/*!
	 Launches time picker to select time.
 */
void CalenEditorReminderField::launchReminderTimePicker()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_LAUNCHREMINDERTIMEPICKER_ENTRY );
	
	HbDialog *popUp = new HbDialog();
	// Set the parent for the dialog
	// Once the parent object is deleted the dialog will also be deleted
	popUp->setParent(this);
	popUp->setDismissPolicy(HbDialog::NoDismiss);
	popUp->setTimeout(HbDialog::NoTimeout);
	popUp->setAttribute( Qt::WA_DeleteOnClose, true );
	popUp->setHeadingWidget( new HbLabel(
				hbTrId("Reminder Time")));
	
	HbExtendedLocale locale = HbExtendedLocale::system();
	mTimePicker = new HbDateTimePicker(mReminderTimeForAllDay);
	// Set the display format
	if(locale.timeStyle() == HbExtendedLocale::Time12) {
		mTimePicker->setDisplayFormat("hh:mm ap");	
	}else {
		mTimePicker->setDisplayFormat("hh:mm");
	}
	
	// Check if the entry is repeating based on the repeatuntil item
	if(!mCalenEditor->isRepeatUntilItemAdded()) {
		int index = currentReminderIndex();
		QDate checkDate = mCalenEditor->editedEntry()->startTime().date();
		// Restrict the time picker to show the valid time 
		// depending on the reminder options
		// Take an offset of 5 mins
		QTime minTime = QTime::currentTime().addSecs(5 * 60);
		if(index == ReminderTwoDaysBefore &&  checkDate.addDays(-2) == QDate::currentDate()) {
				mTimePicker->setMinimumTime(minTime);
		}else if(index == ReminderOneDayBefore && checkDate.addDays(-1) == QDate::currentDate()) {
				mTimePicker->setMinimumTime(minTime);
		}else if(index == ReminderOnEventDay && checkDate == QDate::currentDate()) {
				mTimePicker->setMinimumTime(minTime);
		}
	}
	mTimePicker->setTime(mReminderTimeForAllDay);
	popUp->setContentWidget(mTimePicker);
	
	HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"), popUp);
	popUp->addAction(okAction);
	connect(okAction, SIGNAL(triggered()), this, SLOT(setReminderTimeForAllDay()));
	popUp->addAction(new HbAction(hbTrId("txt_common_button_cancel"),
	                                      popUp));
	popUp->open();
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_LAUNCHREMINDERTIMEPICKER_EXIT );
}

/*!
	 Set the reminder time chosen.
 */
void CalenEditorReminderField::setReminderTimeForAllDay()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_SETREMINDERTIMEFORALLDAY_ENTRY );
	mReminderTimeForAllDay = mTimePicker->time();
	if (mReminderTimeForAllDay.isValid()) {
		// Change the time displayed to that selected by the user.
		setDisplayTime();
		int offset = 0;
		if (currentReminderIndex() == ReminderOneDayBefore) {
			offset = 1;
		} else if (currentReminderIndex() == ReminderTwoDaysBefore) {
			offset = 2;
		}
		QDateTime startDateTimeForAllDay(
			mCalenEditor->editedEntry()->startTime().date(), QTime(0, 0, 0));
		QDateTime reminderDateTimeForAllDay; 
		reminderDateTimeForAllDay.setDate(
				mCalenEditor->editedEntry()->startTime().date().addDays(
						-offset));
		reminderDateTimeForAllDay.setTime(mReminderTimeForAllDay);
		int seconds =
				reminderDateTimeForAllDay.secsTo(startDateTimeForAllDay);
		int timeOffset = seconds / 60;
		mCustomReminderTimeItem->setEnabled(true);
		AgendaAlarm reminder;
		reminder.setTimeOffset(timeOffset);
		reminder.setAlarmSoundName(QString(" "));
		// Set the reminder to the entry.
		mCalenEditor->editedEntry()->setAlarm(reminder);
	}	
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_SETREMINDERTIMEFORALLDAY_EXIT );
}

/*!
	 Checks if reminder field is enabled or not.
 */
bool CalenEditorReminderField::isReminderFieldEnabled()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_ISREMINDERFIELDENABLED_ENTRY );
	return mReminderItem->isEnabled();
}

/*!
	 Returns the number of items present in the reminder option.
 */
int CalenEditorReminderField::reminderItemsCount()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_REMINDERITEMSCOUNT_ENTRY );
	QVariant strings = mReminderItem->contentWidgetData("items");
	QStringList stringList(strings.toStringList());
	int count = stringList.count();
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_REMINDERITEMSCOUNT_EXIT );
	return count;
}

/*!
	 Returns the current chosen index.
 */
int CalenEditorReminderField::currentReminderIndex()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_CURRENTREMINDERINDEX_ENTRY );
	QVariant countVariant = mReminderItem->contentWidgetData("currentIndex");
	int index = countVariant.toInt();
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_CURRENTREMINDERINDEX_EXIT );
	return index;
}

/*!
	 Sets the chosen index as current index.
	 /param index indicates the idex value to be set.
 */
void CalenEditorReminderField::setCurrentIndex(int index)
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_SETCURRENTINDEX_ENTRY );
	mReminderItem->setContentWidgetData("currentIndex", index);
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_SETCURRENTINDEX_EXIT );
}

/*!
	 Disables the reminder time field.
 */
void CalenEditorReminderField::disableReminderTimeField()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_DISABLEREMINDERTIMEFIELD_ENTRY );
	if (mReminderTimeAdded) {
		mCustomReminderTimeItem->setEnabled(false);
	}
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_DISABLEREMINDERTIMEFIELD_EXIT );
}

/*!
	 Checks if reminder time field is added or not.
 */
bool CalenEditorReminderField::isReminderTimeForAllDayAdded()
{
	OstTraceFunctionEntry0( CALENEDITORREMINDERFIELD_ISREMINDERTIMEFORALLDAYADDED_ENTRY );
	OstTraceFunctionExit0( CALENEDITORREMINDERFIELD_ISREMINDERTIMEFORALLDAYADDED_EXIT );
	return mReminderTimeAdded;
}

// End of file	--Don't remove this.
