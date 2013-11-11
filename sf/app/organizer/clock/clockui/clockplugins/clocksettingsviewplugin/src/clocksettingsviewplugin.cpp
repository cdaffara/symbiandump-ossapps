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
* Definition file for class ClockSettingsViewPlugin.
*
*/
// System includes
#include <QtPlugin>

// User includes
#include "clockregionalsettingsview.h"
#include "clocksettingsviewplugin.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "clocksettingsviewpluginTraces.h"
#endif


/*!
	\class ClockSettingsViewPlugin

	\brief The class ClockSettingsViewPlugin is a plugin to show
	clock settings View 
*/

/*!
	Constructor.
 */
ClockSettingsViewPlugin::ClockSettingsViewPlugin()
{
	OstTraceFunctionEntry0( CLOCKSETTINGSVIEWPLUGIN_CLOCKSETTINGSVIEWPLUGIN_ENTRY );
	OstTraceFunctionExit0( CLOCKSETTINGSVIEWPLUGIN_CLOCKSETTINGSVIEWPLUGIN_EXIT );
}

/*!
	Destructor.
 */
ClockSettingsViewPlugin::~ClockSettingsViewPlugin()
{
	OstTraceFunctionEntry0( DUP1_CLOCKSETTINGSVIEWPLUGIN_CLOCKSETTINGSVIEWPLUGIN_ENTRY );
	OstTraceFunctionExit0( DUP1_CLOCKSETTINGSVIEWPLUGIN_CLOCKSETTINGSVIEWPLUGIN_EXIT );
}

/*!
	Launch the clock  regional settings view.
 */
void ClockSettingsViewPlugin::launchRegionalSettingsView()
{
	OstTraceFunctionEntry0( CLOCKSETTINGSVIEWPLUGIN_LAUNCHREGIONALSETTINGSVIEW_ENTRY );
	//The view is removed from main window and deleted once you select back on view
	ClockRegionalSettingsView *view = new ClockRegionalSettingsView();
	view->showView();
	OstTraceFunctionExit0( CLOCKSETTINGSVIEWPLUGIN_LAUNCHREGIONALSETTINGSVIEW_EXIT );
}

Q_EXPORT_PLUGIN2(clocksettingsviewplugin, ClockSettingsViewPlugin)

// End of file	--Don't remove this
