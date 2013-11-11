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
 *
 * Description:   Definition file for the class SettingsUtility
 *
 */

// System includes
#include <e32std.h>
#include <QTime>
#include <QDateTime>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <HbGlobal>
#include <clockdomaincrkeys.h>

// User includes
#include "settingsutility.h"
#include "timezoneclient.h"
#include "clockdatatypes.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "settingsutilityTraces.h"
#endif


/*!
	\class SettingsUtility.
 */

/*!
	Default constructor.
 */
SettingsUtility::SettingsUtility(QObject *parent)
:QObject(parent)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_SETTINGSUTILITY_ENTRY );
	mTimeSeparatorList << tr(".") << tr(":");
	mClockTypeList << hbTrId("txt_clock_button_analog") << hbTrId("txt_clock_button_digital");
	mTimeFormatList << hbTrId("txt_clk_setlabel_val_24_hour") << hbTrId("txt_clk_setlabel_val_12_hour");
	mDisplayDateFormatList << hbTrId("txt_clk_setlabel_val_dd_mm_yyyy") << hbTrId("txt_clk_setlabel_val_mm_dd_yyyy") << hbTrId("txt_clk_setlabel_val_yyyy_mm_dd");
	mDateFormatList << tr("dd mm yyyy") << tr("mm dd yyyy") << tr("yyyy mm dd");
	mDateSeparatorList << tr(".") << tr(":") << tr("/") << tr("-");
	mAutoUpdateValueList << tr("ON") << tr("OFF");
	mSnoozeValueList << tr("5 minutes") << tr("15 minutes") << tr(" 30 minutes") << tr("1 hour");
	
	mSettingsManager = new XQSettingsManager(this);
	mClockTypeSettingsKey = new XQSettingsKey(
							XQSettingsKey::TargetCentralRepository,
							KCRUidClockApp,
							KClockType);
		
	OstTraceFunctionExit0( SETTINGSUTILITY_SETTINGSUTILITY_EXIT );
}

/*!
	Destructor.
 */
SettingsUtility::~SettingsUtility()
{
	OstTraceFunctionEntry0( DUP1_SETTINGSUTILITY_SETTINGSUTILITY_ENTRY );
	if(mSettingsManager) {
		delete mSettingsManager;
	}
	// Delete the cenrep key
	delete mClockTypeSettingsKey;
	OstTraceFunctionExit0( DUP1_SETTINGSUTILITY_SETTINGSUTILITY_EXIT );
}

/*!
	Returns the current time in the device.
 */
QString SettingsUtility::time()
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_TIME_ENTRY );
	return QTime::currentTime().toString(timeFormatString());
}

/*!

 */
void SettingsUtility::setTimeFormat(const QString& format)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_SETTIMEFORMAT_ENTRY );
	TLocale locale;

	if (format == mTimeFormatList.at(0)) {
		locale.SetTimeFormat(ETime24);
	} else if (format == mTimeFormatList.at(1)) {
		locale.SetTimeFormat(ETime12);
	} else {
		// Nothing to do.
	}
	locale.Set();
	OstTraceFunctionExit0( SETTINGSUTILITY_SETTIMEFORMAT_EXIT );
}

/*!

 */
int SettingsUtility::timeFormat(QStringList &format)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_TIMEFORMAT_ENTRY );
	TLocale locale;
	int value = -1;

	if (ETime24 == locale.TimeFormat()) {
		value = 0;
	} else if (ETime12 == locale.TimeFormat()) {
		value = 1;
	}

	format = mTimeFormatList;

	OstTraceFunctionExit0( SETTINGSUTILITY_TIMEFORMAT_EXIT );
	return value;
}

/*!

 */
void SettingsUtility::setClockType(const QString &type)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_SETCLOCKTYPE_ENTRY );
	// Check the type and set the cenrep value
	if (type == mClockTypeList.at(0)) {
		// The value set for Analogue type is 0
		mSettingsManager->writeItemValue(*mClockTypeSettingsKey, 0);
	} else if (type == mClockTypeList.at(1)) {
		// The value set for Digital type is 1
		mSettingsManager->writeItemValue(*mClockTypeSettingsKey, 1);
	} else {
    	// Nothing to do.
    }
	OstTraceFunctionExit0( SETTINGSUTILITY_SETCLOCKTYPE_EXIT );
}

/*!

 */
int SettingsUtility::clockType(QStringList &list)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_CLOCKTYPE_ENTRY );

	int clockType = -1;
	// Read the clocktype value from the cenrep
	// 0 is for Analogue type and 1 for Digital
	QVariant value = mSettingsManager->readItemValue(*mClockTypeSettingsKey);
	clockType = value.toInt();
	
	list = mClockTypeList;
	OstTraceFunctionExit0( SETTINGSUTILITY_CLOCKTYPE_EXIT );
	
	return clockType;
}

/*!

 */
void SettingsUtility::setTimeSeparator(const QString &separator)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_SETTIMESEPARATOR_ENTRY );
	if (mTimeSeparatorList.contains(separator)) {
	    TLocale locale;

		locale.SetTimeSeparator(TChar(separator.unicode()->unicode()), 1);
		locale.SetTimeSeparator(TChar(separator.unicode()->unicode()), 2);

		locale.Set();
    }
	OstTraceFunctionExit0( SETTINGSUTILITY_SETTIMESEPARATOR_EXIT );
}

/*!

 */
int SettingsUtility::timeSeparator(QStringList &list)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_TIMESEPARATOR_ENTRY );
	TLocale locale;
	TChar separatorChar = locale.TimeSeparator(1);
	int value = -1;

	if (separatorChar == (mTimeSeparatorList.at(0).unicode())->unicode()) {
		value = 0;
	} else if (separatorChar == (mTimeSeparatorList.at(1).unicode())->unicode()) {
		value = 1;
	} else {
		// Nothing to do.
	}

	list = mTimeSeparatorList;
	OstTraceFunctionExit0( SETTINGSUTILITY_TIMESEPARATOR_EXIT );
	return value;
}

/*!

 */
QString SettingsUtility::date()
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_DATE_ENTRY );
	return QDate::currentDate().toString(dateFormatString());
}

/*!

 */
void SettingsUtility::setDateFormat(const QString &format)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_SETDATEFORMAT_ENTRY );
	TLocale locale;
	int index;

	for (index = 0; index < mDisplayDateFormatList.count(); ++index) {
		if (format == mDisplayDateFormatList.at(index)) {
			break;
		}
	}

	switch (index) {
		case 0:
			locale.SetDateFormat(EDateEuropean);
			break;
		case 1:
			locale.SetDateFormat(EDateAmerican);
			break;
		case 2:
			locale.SetDateFormat(EDateJapanese);
			break;
		default:
			//Nothing to do.
			break;
	}

	locale.Set();
	OstTraceFunctionExit0( SETTINGSUTILITY_SETDATEFORMAT_EXIT );
}

/*!

 */
int SettingsUtility::dateFormat(QStringList &format)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_DATEFORMAT_ENTRY );
	TLocale locale;
	int index = -1;

	switch (locale.DateFormat()) {
		case EDateEuropean:
			index = 0;
			break;
		case EDateAmerican:
			index = 1;
			break;
		case EDateJapanese:
			index = 2;
			break;
		default:
			// Nothing yet.
			break;
	}

	format = mDisplayDateFormatList;

	OstTraceFunctionExit0( SETTINGSUTILITY_DATEFORMAT_EXIT );
	return index;
}

/*!

 */
void SettingsUtility::setDateSeparator(const QString &separator)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_SETDATESEPARATOR_ENTRY );
	TLocale locale;
	locale.SetDateSeparator(
			TChar(separator.unicode()->unicode()), 1);
	locale.SetDateSeparator(
			TChar(separator.unicode()->unicode()), 2);
	locale.Set();
	OstTraceFunctionExit0( SETTINGSUTILITY_SETDATESEPARATOR_EXIT );
}

/*!

 */
int SettingsUtility::dateSeparator(QStringList &separator)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_DATESEPARATOR_ENTRY );
	TLocale locale;
	TChar separatorChar = locale.DateSeparator(1);

	const int at0 = (mDateSeparatorList.at(0).unicode())->unicode();
	int at1 = (mDateSeparatorList.at(1).unicode())->unicode();
	int at2 = (mDateSeparatorList.at(2).unicode())->unicode();
	int at3 = (mDateSeparatorList.at(3).unicode())->unicode();
	int value = 0;

	if (separatorChar == (mDateSeparatorList.at(0).unicode())->unicode()) {
		value = 0;
	} else if (separatorChar ==
			(mDateSeparatorList.at(1).unicode())->unicode()) {
		value = 1;
	} else if (separatorChar ==
			(mDateSeparatorList.at(2).unicode())->unicode()) {
		value = 2;
	} else if (separatorChar ==
			(mDateSeparatorList.at(3).unicode())->unicode()) {
		value = 3;
	}

	separator = mDateSeparatorList;

	OstTraceFunctionExit0( SETTINGSUTILITY_DATESEPARATOR_EXIT );
	return value;
}

/*!
	Sets a new list of workdays.

	\param workdays The new list of workdays selected.
 */
void SettingsUtility::setWorkdays(const QString &workdays)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_SETWORKDAYS_ENTRY );
	Q_UNUSED(workdays)
	OstTraceFunctionExit0( SETTINGSUTILITY_SETWORKDAYS_EXIT );
}

/*!
	Returns the workdays list selected by the user.

	\param workdaysList A string list of workdays selected in the locale.
	\return int Index of the item selected.
 */
int SettingsUtility::workdays(QStringList &workdaysList)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_WORKDAYS_ENTRY );
	Q_UNUSED(workdaysList)

	return 0;
}

/*!
	Returns a string based on the locale set, to format time.

	\return QString String to be used to format time.
 */
QString SettingsUtility::timeFormatString()
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_TIMEFORMATSTRING_ENTRY );
	QString format;
	TLocale locale;
	QStringList dummyList;

	if (ETime24 == locale.TimeFormat()) {
		format = QString("hh:mm");
	} else if (ETime12 == locale.TimeFormat()) {
		format = QString("hh:mm ap");
	}

	QString separator = mTimeSeparatorList.at(timeSeparator(dummyList));

	format.replace(QString(":"), separator);

	OstTraceFunctionExit0( SETTINGSUTILITY_TIMEFORMATSTRING_EXIT );
	return format;
}

/*!
	Returns a string based on the locale set, to format date.

	\return QString String to be used to format date.
 */
QString SettingsUtility::dateFormatString()
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_DATEFORMATSTRING_ENTRY );
	QStringList dummyList;
	QString format(mDateFormatList.at(dateFormat(dummyList)));
	QString separator = mDateSeparatorList.at(dateSeparator(dummyList));

	format.replace(QString(" "), separator);
	format.replace(QString("mm"), QString("MM"));

	OstTraceFunctionExit0( SETTINGSUTILITY_DATEFORMATSTRING_EXIT );
	return format;
}

/*!
	Sets the snooze time value selected by the user.

	\param index The index of the preset snooze time values.
 */
void SettingsUtility::setSnoozeTime(int index)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_SETSNOOZETIME_ENTRY );
	Q_UNUSED(index)
	OstTraceFunctionExit0( SETTINGSUTILITY_SETSNOOZETIME_EXIT );
}

/*!
	Returns a list of default snooze time values.

	\param snoozeValueList A string list of preset values.
	\return int The index of the item in the list to be focused.
 */
int SettingsUtility::snoozeTime(QStringList &snoozeValueList)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_SNOOZETIME_ENTRY );
	int index = -1;

	snoozeValueList = mSnoozeValueList;
	OstTraceFunctionExit0( SETTINGSUTILITY_SNOOZETIME_EXIT );
	return index;
}

/*!
	Sets the start of week value selected by the user.

	\param index The index of the selected value.
 */
void SettingsUtility::setStartOfWeek(int index)
{
	OstTraceFunctionEntry0( SETTINGSUTILITY_SETSTARTOFWEEK_ENTRY );
	TLocale locale;
	TDay day = (TDay)index;
	locale.SetStartOfWeek(day);
	locale.Set();
	OstTraceFunctionExit0( SETTINGSUTILITY_SETSTARTOFWEEK_EXIT );
}
// End of file	--Don't remove this.
