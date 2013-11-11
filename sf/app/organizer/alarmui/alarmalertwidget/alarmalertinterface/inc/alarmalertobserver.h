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
*
*/
#ifndef __ALARM_ALERT_OBSERVER_H__
#define __ALARM_ALERT_OBSERVER_H__

// User includes
#include "alarmcommon.h"

// Class declaration
/**
 * @class AlarmAlertObserver
 * 
 * @brief Interface class for observing to alarm alert events
 */
class AlarmAlertObserver
{
public:
	virtual void alertCompleted(AlarmCommand command) = 0;
    virtual void alertDisplayed(AlarmCommand command) = 0;
    virtual void alertCancelled(AlarmCommand command) = 0;
};

#endif		// __ALARM_ALERT_OBSERVER_H__
