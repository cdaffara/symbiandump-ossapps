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
 *
 */

#include <QModelIndex>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hbdataformviewitem.h>
#include <hbabstractbutton.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <HbExtendedLocale.h>

#include "calensettings.h"
#include "calenservices.h"
#include "calendarprivatecrkeys.h"

// Constants
const int KValueZero( 0 );
const int KValueOne( 1 );


CalenSettings::CalenSettings(MCalenServices& services, HbDataForm *form, QObject *parent)
:QObject(parent),
 mServices(services),
 mSettingsForm(form),
 mSettingsModel(NULL),
 mAlarmSnoozeTimeItem(NULL),
 mShowWeekNumberItem(NULL),		 
 mShowRegionalInfoItem(NULL),
 mShowWeekStartOnInfoItem(NULL),
 mStartOfWeekItem(NULL),
 mWeekNumberItemAdded(false),
 mRegionalpluginActive(false)
{   
	mSettingsManager = new XQSettingsManager(this);
	mAlarmSnoozeCenrepKey = new XQSettingsKey(
			XQSettingsKey::TargetCentralRepository,
			KCRUidCalendar, KCalendarSnoozeTime);
	mWeekNumberCenrepKey = new XQSettingsKey(
			XQSettingsKey::TargetCentralRepository,
			KCRUidCalendar, KCalendarShowWeekNum);
	mShowRegionalInfoKey = new XQSettingsKey(
			XQSettingsKey::TargetCentralRepository,
			KCRUidCalendar, KCalendarShowRegionalInfo);
	
	// Enable the pixmap cache for better scrolling performance
	mSettingsForm->setItemPixmapCacheEnabled(true);
}

/*!
	Destructor
 */
CalenSettings::~CalenSettings()
{
	if (mSettingsManager) {
		delete mSettingsManager;
	}
	// Delete the cenrep keys
	delete mAlarmSnoozeCenrepKey;
	delete mWeekNumberCenrepKey;
	delete mShowRegionalInfoKey;
	
	if (mSettingsModel) {
		delete mSettingsModel;		
	}
	if (mAlarmSnoozeTimeHash.count()) {
		// Clear the contents
		mAlarmSnoozeTimeHash.clear();
	}
}

/*!
	Creates the settings model
 */
void CalenSettings::createModel()
{
	// Remove the model.
	if (mSettingsForm->model()) {
		delete mSettingsForm->model();
		mSettingsForm->setModel(0);
		mSettingsModel = 0;
	}

	// Create a model and set it.
	mSettingsModel = new HbDataFormModel(mSettingsForm);
	
	// Create the alarm snooze choices
	QStringList alarmSnoozeChoices;
	alarmSnoozeChoices << hbTrId(
						"txt_calendar_setlabel_reminder_snooze_time_val_5_m")
				<< hbTrId("txt_calendar_setlabel_reminder_snooze_time_val_10")
				<< hbTrId("txt_calendar_setlabel_reminder_snooze_time_val_15")
				<< hbTrId("txt_calendar_setlabel_reminder_snooze_time_val_30");
	// Build the hash map for the alarm snooze time.
	// The values are five, ten, fifteen and thirty minutes
	mAlarmSnoozeTimeHash[0] = 5;
	mAlarmSnoozeTimeHash[1] = 10;
	mAlarmSnoozeTimeHash[2] = 15;
	mAlarmSnoozeTimeHash[3] = 30;
	
	// Append Alarm Snooze Time Setting item
	mAlarmSnoozeTimeItem = new HbDataFormModelItem();
	mAlarmSnoozeTimeItem->setType(HbDataFormModelItem::ComboBoxItem);
	mAlarmSnoozeTimeItem->setData(HbDataFormModelItem::LabelRole, 
					QString(hbTrId("txt_calendar_setlabel_reminder_snooze_time")));

	mAlarmSnoozeTimeItem->setContentWidgetData(QString("items"), 
												alarmSnoozeChoices);
	mAlarmSnoozeTimeItem->setContentWidgetData(QString("currentIndex"), 0);
	mSettingsForm->addConnection(mAlarmSnoozeTimeItem, 
								SIGNAL(currentIndexChanged(int)),
								this, SLOT(handleAlarmSnoozeTimeChange(int)));
	mAlarmSnoozeTimeItem->setContentWidgetData("objectName", "alarmSnoozeTime");
	mSettingsModel->appendDataFormItem(mAlarmSnoozeTimeItem);
	
	// Create the weekday list based on start of week.
	QStringList weekdaysList;
	weekdaysList
			<< hbTrId("txt_calendar_setlabel_week_starts_on_val_monday")
			<< hbTrId("txt_calendar_setlabel_week_starts_on_val_tuesday")
			<< hbTrId("txt_calendar_setlabel_week_starts_on_val_wednesday")
			<< hbTrId("txt_calendar_setlabel_week_starts_on_val_thursday")
			<< hbTrId("txt_calendar_setlabel_week_starts_on_val_friday")
			<< hbTrId("txt_calendar_setlabel_week_starts_on_val_saturday")
			<< hbTrId("txt_calendar_setlabel_week_starts_on_val_sunday");
	// Start of week item.
	mShowWeekStartOnInfoItem = mSettingsModel->appendDataFormItem(
								HbDataFormModelItem::ComboBoxItem, 
								hbTrId("txt_calendar_setlabel_week_starts_on"));
	
	// Get start of week from the locale.
    HbExtendedLocale locale = HbExtendedLocale::system();
    mStartOfWeek = locale.startOfWeek();
	mShowWeekStartOnInfoItem->setContentWidgetData("items", weekdaysList);
	mShowWeekStartOnInfoItem->setContentWidgetData(
			"currentIndex", mStartOfWeek);
	mShowWeekStartOnInfoItem->setContentWidgetData("objectName", "startOfWeek");
	mSettingsForm->addConnection(mShowWeekStartOnInfoItem, 
									SIGNAL(currentIndexChanged(int)),
									this, SLOT(setStartDayOfWeek(int)));
	// Add the show week number item in the model
	addWeekNumberItem();

	if (isPluginEnabled()) {
		addRegionalInfoItem();
	}
}

/*!
	Adds the regional info item to the model
 */
void CalenSettings::addRegionalInfoItem()
{
	mRegionalpluginActive = true;
	mShowRegionalInfoItem = new HbDataFormModelItem();
	mShowRegionalInfoItem->setType(HbDataFormModelItem::ToggleValueItem);
	mShowRegionalInfoItem->setData(HbDataFormModelItem::LabelRole, 
				QString(hbTrId("txt_calendar_setlabel_show_lunar_calendar")));

	// For HbPushButton type properties -- to be used for toggle value item
	mShowRegionalInfoItem->setContentWidgetData(QString("text"), 
								QString(hbTrId("txt_calendar_button_no")));
	mShowRegionalInfoItem->setContentWidgetData(QString("additionalText"), 
								QString(hbTrId("txt_calendar_button_yes")));
	mShowRegionalInfoItem->setContentWidgetData(
			"objectName", "showRegionalInfo");
	mSettingsForm->addConnection(mShowRegionalInfoItem, SIGNAL(clicked()),
								this, SLOT(handleRegionalInfoChange()));
	mSettingsModel->appendDataFormItem(mShowRegionalInfoItem);
}

/*!
	Removes the regional info item from the model
 */
void CalenSettings::removeRegionalInfoItem()
{
	mSettingsForm->removeConnection(mShowRegionalInfoItem, SIGNAL(clicked()),
									this, SLOT(handleRegionalInfoChange()));
	mSettingsModel->removeItem(mShowRegionalInfoItem);
	mRegionalpluginActive = false;
}

/*!
	Handles the alarm snooze time change
 */
void CalenSettings::handleAlarmSnoozeTimeChange(int index)
{
	if(mAlarmSnoozeTimeHash.value(index) > KValueZero) {
		mSettingsManager->writeItemValue(*mAlarmSnoozeCenrepKey, mAlarmSnoozeTimeHash[index]);
	}
}

/*!
	Handles the week number change
 */
void CalenSettings::handleWeekNumberChange()
{
	QVariant data = mShowWeekNumberItem->contentWidgetData(QString("text"));
	QString value = data.toString();
	if (!value.compare(hbTrId("txt_calendar_button_no"))) {
		// User has made it OFF.
		mSettingsManager->writeItemValue(*mWeekNumberCenrepKey, KValueZero);
	} else if (!value.compare(hbTrId("txt_calendar_button_yes"))) {
		// User has made it ON.
		mSettingsManager->writeItemValue(*mWeekNumberCenrepKey, KValueOne);
	}
}

/*!
	Handles the regional info option change
 */
void CalenSettings::handleRegionalInfoChange()
{
	QVariant data = mShowRegionalInfoItem->contentWidgetData(QString("text"));
	QString value = data.toString();
	if (!value.compare(hbTrId("txt_calendar_button_no"))) {
		// User has made it OFF
		mSettingsManager->writeItemValue(*mShowRegionalInfoKey, KValueZero);
	} else if (!value.compare(hbTrId("txt_calendar_button_yes"))) {
		// User has made it ON.
		mSettingsManager->writeItemValue(*mShowRegionalInfoKey, KValueOne);
	}
}

/*!
	Populates the settings model
 */
void CalenSettings::populateSettingList()
{   
	// Read the values from cenrep
	// Read the initial values from the cenrep
	QVariant value = mSettingsManager->readItemValue(*mAlarmSnoozeCenrepKey);
	uint alarmSnoozeTime = value.toUInt();

	// Get the value from the hash and set the value to the form
	int choiceIndex = mAlarmSnoozeTimeHash.key(alarmSnoozeTime);
	mAlarmSnoozeTimeItem->setContentWidgetData(QString("currentIndex"), 
														choiceIndex);
	// Get start of week from the locale.
	HbExtendedLocale locale = HbExtendedLocale::system();
	mStartOfWeek = locale.startOfWeek();

	mShowWeekStartOnInfoItem->setContentWidgetData("currentIndex", mStartOfWeek);
	//update the show week number option
	updateShowWeekItem();
	// Check the the regional info plugin is loaded now or not
	if (isPluginEnabled()) {
		// If the plugin was already there just populate the item
		// else add the regional info item and then populate
		if (mRegionalpluginActive) {
			populateRegionalInfo();
		}else {
			addRegionalInfoItem();
			populateRegionalInfo();
		}
	}else {
		// If the plugin is removed now and if it was loaded before
		// remove the item
		if (mRegionalpluginActive) {
			removeRegionalInfoItem();
		}
	}
	mSettingsForm->setModel(mSettingsModel);
}

/*!
	Populates the regional info item
 */
void CalenSettings::populateRegionalInfo()
{
	QVariant value  = mSettingsManager->readItemValue(*mShowRegionalInfoKey);
	uint showRegionalInfo = value.toUInt();

	if (showRegionalInfo) {
		mShowRegionalInfoItem->setContentWidgetData(
				QString("text"), QString(hbTrId("txt_calendar_button_yes")));
		mShowRegionalInfoItem->setContentWidgetData(
				QString("additionalText"), QString(
						hbTrId("txt_calendar_button_no")));
	} else {
		mShowRegionalInfoItem->setContentWidgetData(
				QString("text"), QString(hbTrId("txt_calendar_button_no")));
		mShowRegionalInfoItem->setContentWidgetData(
				QString("additionalText"), QString(
						hbTrId("txt_calendar_button_yes")));
	}
}

/*!
	Adds the week number item in the model
 */
void CalenSettings::addWeekNumberItem()
{
	mWeekNumberItemAdded = true;
	// Append Show Week Number settings item
	mShowWeekNumberItem = new HbDataFormModelItem();
	mShowWeekNumberItem->setType(HbDataFormModelItem::ToggleValueItem);
	mShowWeekNumberItem->setData(HbDataFormModelItem::LabelRole, 
					QString(hbTrId("txt_calendar_setlabel_show_week_numbers")));
	mShowWeekNumberItem->setContentWidgetData("objectName", "showWeekNumber");
	mSettingsForm->addConnection(mShowWeekNumberItem, SIGNAL(clicked()), 
									this, SLOT(handleWeekNumberChange()));
	mSettingsModel->appendDataFormItem(mShowWeekNumberItem);

}

/*!
	Returns true if the regional plugin is loaded
 */
bool CalenSettings::isPluginEnabled()
{
	QString *pluginInfo = mServices.InfobarTextL();
	if (!pluginInfo) {
		return false;
	} else {
		return true;
	}
}

/*!
	Sets the start day of the week to the locale
	Is called when user selects week day
 */
void CalenSettings::setStartDayOfWeek(const int index)
{
	TLocale locale;
	TDay day = (TDay)index;
	//set the start day of the week to locale
	locale.SetStartOfWeek(day);
	locale.Set();
}

/*!
	Updates the show  week number option
 */
void CalenSettings::updateShowWeekItem()
{
	// Get start of week from the locale.
	HbExtendedLocale locale = HbExtendedLocale::system();
	mStartOfWeek = locale.startOfWeek();
	mShowWeekStartOnInfoItem->setContentWidgetData("currentIndex", mStartOfWeek);
	// Read the value form cenrep
	QVariant value = mSettingsManager->readItemValue(*mWeekNumberCenrepKey);
	uint showWeekNumber = value.toUInt();

	//if start day is not Monday
	//removed the show week number option  ,
	//else set to proper status
	if (mStartOfWeek != HbExtendedLocale::Monday) {
	    if (mWeekNumberItemAdded) {
	       mSettingsForm->removeConnection(mShowWeekNumberItem, SIGNAL(clicked()), 
	                this, SLOT(handleWeekNumberChange()));
	        mSettingsModel->removeItem(mShowWeekNumberItem);
	        mWeekNumberItemAdded = false;
	    }
	} else {
	    //if option  "show week number is not present"
	    //add the option in view
	    if (!mWeekNumberItemAdded) {
	        addWeekNumberItem();  
	    }
	    if (showWeekNumber) {
	        mShowWeekNumberItem->setContentWidgetData(QString("text"),
	                QString(hbTrId("txt_calendar_button_yes")));
	        mShowWeekNumberItem->setContentWidgetData(QString("additionalText"),
	                QString(hbTrId("txt_calendar_button_no")));
	    } else {
	        mShowWeekNumberItem->setContentWidgetData(QString("text"), 
	                QString(hbTrId("txt_calendar_button_no")));
	        mShowWeekNumberItem->setContentWidgetData(QString("additionalText"),
	                QString(hbTrId("txt_calendar_button_yes")));
	    }
	    
	}
}
// End of file
