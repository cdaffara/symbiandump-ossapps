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
* This file contains the ClockAppControllerIf abstract class declaration.
*
*/

#ifndef CLOCKAPPCONTROLLERIF_H
#define CLOCKAPPCONTROLLERIF_H

// User includes
#include "clockdatatypes.h"

// Forward declarations
class TimezoneClient;
class SettingsUtility;
class AlarmClient;

class ClockAppControllerIf
{
public:
	virtual TimezoneClient *timezoneClient() = 0;
	virtual SettingsUtility *settingsUtility() = 0;
	virtual AlarmClient *alarmClient() = 0;
	virtual void switchToView(ClockViews viewId) = 0;
};

#endif // CLOCKAPPCONTROLLERIF_H

// End of file	--Don't remove this.
