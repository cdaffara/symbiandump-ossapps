/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Alarm UI panic reason codes
 *
*/



#ifndef __ALARMUI_PAN__
#define __ALARMUI_PAN__


enum TAlarmUIPanicReason
{
    EAlarmUIInvalidState   = 1,  // invalid state for attempted state machine signal (see documentation)
    EAlarmUIUnknownState   = 2,  // unknown state
    EAlarmUIUnknownReason  = 4,  // unknown reason
    EAlarmUIUnknownCommand = 8,  // unknown command
};

enum TAlarmUIFunctionInfo
{
    EAlarmUIUpdateAlarmInfo =   16,
    EAlarmUIShowAlarm       =   32,
    EAlarmUIHideAlarm       =   64,
    EAlarmUIDisplayDialog   =  128,
    EAlarmUICancelDialog    =  256,
    EAlarmUINoteCompleted   =  512,
    EAlarmUIKSysHideAlarm   = 1024,
    EAlarmUIAutoSnooze      = 2048,
    EAlarmUIHandleInterrupt = 4096
};


#endif  // __ALARMUI_PAN__


// End of File
