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
* Definition file for class DateTimeSettingsPlugin.
*
*/
// System includes
#include <QDateTime>
#include <cpsettingformentryitemdataimpl.h>
#include <HbExtendedLocale>
#include <HbTranslator>

// User includes
#include "datetimesettingsplugin.h"
#include "datetimesettingsview.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "datetimesettingspluginTraces.h"
#endif


/*!
	Constructor.
 */
DateTimeSettingsPlugin::DateTimeSettingsPlugin()
{
	OstTraceFunctionEntry0( DATETIMESETTINGSPLUGIN_DATETIMESETTINGSPLUGIN_ENTRY );
	mTranslator = new HbTranslator("clocksettingsview");
	mTranslator->loadCommon();
	OstTraceFunctionExit0( DATETIMESETTINGSPLUGIN_DATETIMESETTINGSPLUGIN_EXIT );
}

/*!
	Destructor.
 */
DateTimeSettingsPlugin::~DateTimeSettingsPlugin()
{
	OstTraceFunctionEntry0( DUP1_DATETIMESETTINGSPLUGIN_DATETIMESETTINGSPLUGIN_ENTRY );
	if(mTranslator) {
	    delete mTranslator;
	    mTranslator = 0;
	}
	OstTraceFunctionExit0( DUP1_DATETIMESETTINGSPLUGIN_DATETIMESETTINGSPLUGIN_EXIT );
}

/*!
	Returns the uid of the plugin.
 */
int DateTimeSettingsPlugin::uid() const
{
	OstTraceFunctionEntry0( DATETIMESETTINGSPLUGIN_UID_ENTRY );
	// UID of the plugin.
	OstTraceFunctionExit0( DATETIMESETTINGSPLUGIN_UID_EXIT );
	return 0x102818E9;
}

/*!
	Creates the object of DateTimeSettingsView class.
 */
QList<CpSettingFormItemData*> DateTimeSettingsPlugin:: 
						      createSettingFormItemData(
							  CpItemDataHelper &itemDataHelper) const
{
	OstTraceFunctionEntry0( DATETIMESETTINGSPLUGIN_CREATESETTINGFORMITEMDATA_ENTRY );
	
	QString datetimeIconString = "qtg_large_date_time";
	HbIcon datetimeIcon(datetimeIconString) ;
	
	HbExtendedLocale locale = HbExtendedLocale::system();
	QString timeInfo = locale.format(
			QTime::currentTime(), r_qtn_time_usual_with_zero);
	QString dateinfo = locale.format(
			QDate::currentDate(), r_qtn_date_usual_with_zero);
	QString displayString;
	displayString.append(timeInfo);
	displayString.append(" ");
	displayString.append(dateinfo);
	
	QList<CpSettingFormItemData*> entryItemList;
	CpSettingFormItemData *entryItem = new DateTimeSettingsView(
			itemDataHelper, hbTrId("txt_cp_main_view_list_time_date"),
			displayString,datetimeIcon, 0, mTranslator);
	entryItemList.append(entryItem);
	OstTraceFunctionExit0( DATETIMESETTINGSPLUGIN_CREATESETTINGFORMITEMDATA_EXIT );
	return entryItemList;
}

Q_EXPORT_PLUGIN2(DateTimeSettingsPlugin, DateTimeSettingsPlugin)

// End of file	--Don't remove this
