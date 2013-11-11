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
* Header file for ClockSettingsViewPlugin class.
*
*/


#ifndef CLOCKSETTINGSVIEWPLUGIN_H_
#define CLOCKSETTINGSVIEWPLUGIN_H_

// System includes
#include <QObject>
#include <clocksettingsviewinterface.h>

class ClockSettingsViewPlugin : public QObject, public ClockSettingsViewInterface
{
	Q_OBJECT
	Q_INTERFACES(ClockSettingsViewInterface)

public:
	ClockSettingsViewPlugin();
	~ClockSettingsViewPlugin();
	
	void launchRegionalSettingsView();

};

#endif /* CLOCKSETTINGSVIEWPLUGIN_H_ */

// End of file	--Don't remove this
