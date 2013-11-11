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
* This file contains the ClockAppControllerIfImpl class declaration.
*
*/

#ifndef CLOCKAPPCONTROLLERIFIMPL_H
#define CLOCKAPPCONTROLLERIFIMPL_H

// System includes
#include <QObject>

// User includes
#include "clockappcontrollerif.h"
#include "clockdatatypes.h"

// Forward declarations
class ClockAppController;
class TimezoneClient;
class SettingsUtility;
class AlarmClient;

class ClockAppControllerIfImpl : public ClockAppControllerIf, public QObject
{
public:
	ClockAppControllerIfImpl(
			ClockAppController *controller, QObject *parent = 0);
	virtual ~ClockAppControllerIfImpl();

public:
	virtual TimezoneClient *timezoneClient();
	virtual SettingsUtility *settingsUtility();
	virtual AlarmClient *alarmClient();
	virtual void switchToView(ClockViews viewId);

private:
	ClockAppController *mAppController;
	TimezoneClient *mTimeZoneClient;
	AlarmClient *mAlarmClient;
	SettingsUtility *mSettingsUtility;
};

#endif // CLOCKAPPCONTROLLERIFIMPL_H

// End of file	--Don't remove this.
