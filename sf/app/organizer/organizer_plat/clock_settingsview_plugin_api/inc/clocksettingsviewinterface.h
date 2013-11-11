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
* Description: Header file for clock settings view Interface
*
*/

#ifndef CLOCKSETTINGSVIEWINTERFACE_H
#define CLOCKSETTINGSVIEWINTERFACE_H

// System includes
#include <qglobal.h>

/*!
	\class ClockSettingsViewInterface

	\brief The class ClockSettingsViewInterface is an interface for the plugin to show clock settings view
 */


#define CLOCK_SETTINGSVIEW_PLUGIN_PATH QString("z:/resource/qt/plugins/clock")
#define CLOCK_SETTINGSVIEW_PLUGIN_NAME QString("clocksettingsviewplugin.qtplugin")

class ClockSettingsViewInterface
{
	
public:

	/*!
	Destructor
	*/
	virtual ~ClockSettingsViewInterface() {}
	
	/*!
	To launch the Clock Regional Settings View
	*/
	virtual void launchRegionalSettingsView() = 0;

};

Q_DECLARE_INTERFACE(ClockSettingsViewInterface, 
        "org.nokia.clock.ClockSettingsViewInterface/1.0")

#endif // CLOCKSETTINGSVIEWINTERFACE_H

// End of file
