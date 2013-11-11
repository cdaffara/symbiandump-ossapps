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
* Definition file for class DateTimeSettingsView.
*
*/

// System includes
#include <QTimer>
#include <QDateTime>
#include <HbExtendedLocale>
#include <cpitemdatahelper.h>

// User includes
#include "datetimesettingsview.h"
#include "clocksettingsview.h"
#include "timezoneclient.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "datetimesettingsviewTraces.h"
#endif


/*!
	\class DateTimeSettingsView

	This class launches the clock settings view from control panel.
 */

/*!
	Constructor.

	\param itemDataHelper CpItemDataHelper object.
	\param text text to be displayed in first line.
	\param description test to be displayed in second line.
	\param icon to be displayed.
	\param parent Parent of type HbDataFormModelItem
 */
DateTimeSettingsView::DateTimeSettingsView(
		CpItemDataHelper &itemDataHelper, const QString &text,
		const QString &description, const HbIcon &icon,
		const HbDataFormModelItem *parent, HbTranslator *translator):
		CpSettingFormEntryItemData(
				itemDataHelper, text, description, icon, parent)
{
	OstTraceFunctionEntry0( DATETIMESETTINGSVIEW_DATETIMESETTINGSVIEW_ENTRY );
	// Construct the timezone client.
	mTranslator = translator;
	mTimezoneClient = TimezoneClient::getInstance();
	connect(
			mTimezoneClient, SIGNAL(timechanged()),
			this, SLOT(updateDisplayTime()));

	// Start a timer. For updating the displayed time.
	mTickTimer = new QTimer(this);
	// Start the Timer for 1 minute.
	mTickTimer->start(60000 - 1000 * QTime::currentTime().second());
	connect(
			mTickTimer, SIGNAL(timeout()),
			this, SLOT(updateDisplayTime()));
	OstTraceFunctionExit0( DATETIMESETTINGSVIEW_DATETIMESETTINGSVIEW_EXIT );
}

/*!
	Destructor.
 */
DateTimeSettingsView::~DateTimeSettingsView()
{
	OstTraceFunctionEntry0( DUP1_DATETIMESETTINGSVIEW_DATETIMESETTINGSVIEW_ENTRY );
	if (mTickTimer) {
		mTickTimer->stop();
		delete mTickTimer;
		mTickTimer = 0;
	}
	
	if (!mTimezoneClient->isNull()) {
		mTimezoneClient->deleteInstance();
	}
	OstTraceFunctionExit0( DUP1_DATETIMESETTINGSVIEW_DATETIMESETTINGSVIEW_EXIT );
}

/*!
	Launches the clock settings view.
 */
void DateTimeSettingsView::onLaunchView()
{
	OstTraceFunctionEntry0( DATETIMESETTINGSVIEW_ONLAUNCHVIEW_ENTRY );
	ClockSettingsView *settingsView = new ClockSettingsView(this, mTranslator, false);	
	settingsView->loadSettingsView();
	OstTraceFunctionExit0( DATETIMESETTINGSVIEW_ONLAUNCHVIEW_EXIT );
}

/*!
	Updates the second line i.e date & time.
 */
void DateTimeSettingsView::updateDisplayTime()
{
	OstTraceFunctionEntry0( DATETIMESETTINGSVIEW_UPDATEDISPLAYTIME_ENTRY );
	HbExtendedLocale locale = HbExtendedLocale::system();
	QString timeInfo = locale.format(
			QTime::currentTime(), r_qtn_time_usual_with_zero);
	QString dateinfo = locale.format(
			QDate::currentDate(), r_qtn_date_usual_with_zero);
	QString displayString;
	displayString.append(timeInfo);
	displayString.append(" ");
	displayString.append(dateinfo);
	setDescription(displayString);
	// Start the Timer for 1 minute.
	mTickTimer->start(60000);
	OstTraceFunctionExit0( DATETIMESETTINGSVIEW_UPDATEDISPLAYTIME_EXIT );
}

/*!
	createSettingView()
 */
CpBaseSettingView *DateTimeSettingsView::createSettingView() const
{
	OstTraceFunctionEntry0( DATETIMESETTINGSVIEW_CREATESETTINGVIEW_ENTRY );
	OstTraceFunctionExit0( DATETIMESETTINGSVIEW_CREATESETTINGVIEW_EXIT );
	return 0;
}

// End of file	--Don't remove this
