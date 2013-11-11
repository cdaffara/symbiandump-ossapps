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
// alarmcommon.h

#ifndef __ALARM_COMMON_H__
#define __ALARM_COMMON_H__

#include <e32cmn.h>
#include <e32std.h>

/**
 * @struct SAlarmInfo
 * @brief Structure to hold alarm information
 */
struct SAlarmInfo
{
    HBufC* iSubject;
    TTime iDate;
    TTime iTime;
    HBufC* iLocation;
    TBool iCanSnooze;
    TInt iAlarmAlertType;
    TBool iIsSilent;
    TBool iIsTimed;
};

/**
 * @enum AlarmCommand
 * @brief Common alarm commands
 */
enum AlarmCommand {
	AlarmStop = 0x0,
	AlarmSilence,
	AlarmSnooze,
	AlarmShown,
	AlarmCmdLast     
};

#endif		// __ALARM_COMMON_H__
