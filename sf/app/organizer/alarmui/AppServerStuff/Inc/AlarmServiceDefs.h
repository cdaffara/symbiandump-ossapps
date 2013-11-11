/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file contains Alarm Service Commands. 
*
*/



#ifndef __ALARM_SERVICE_DEFS__
#define __ALARM_SERVICE_DEFS__

#include <asaltdefs.h>
#include <AknCapServerDefs.h>

enum TAknAlarmServiceAdditionalOpCodes
    {
    EAknSAltOpCodeAskServerState = EASAltOpCodeStopPlayingSound+1 // currently last of TASAltOpCode, future proofing to do..
    };

enum TAknAlarmServiceCallbackCmds
    {
    ECmdAcknowledgedAlarm = 0,
    ECmdTaskAwayFromAlarm,
    ECmdTaskAwayFromAlarmWTime
    };

#endif // __ALARM_SERVICE_DEFS__


// End of File
