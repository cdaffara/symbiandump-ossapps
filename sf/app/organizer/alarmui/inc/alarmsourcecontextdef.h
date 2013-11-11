/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Alarm related source context definitions.
*
*/




#ifndef ALARMSOURCECONTEXTDEF_H
#define ALARMSOURCECONTEXTDEF_H

#include <cfdefinitions.h>

//-----------------------------------------------------------------------------
// Source:  'AlarmUI'
//-----------------------------------------------------------------------------
//
_LIT( KAlarmUISource, "AlarmUI" );


//-----------------------------------------------------------------------------
// Source:  'AlarmUI'
// Type:    'State'
//
// Describes AlarmUI activation and deactivation.
// State transitions:
//     [Inactive] -> [Calendar|Clock|Unknown] -> [Stopped|Snoozed] -> [Inactive]
//
//-----------------------------------------------------------------------------
//
_LIT( KAlarmUISourceState, "State" );

// Context values
_LIT_CONTEXT_VALUES( KAlarmUISourceStateValues ) =
    {
    // alarm ui is not visible, no active alarms
    _S( "Inactive" ),

    // calendar alarm expired
    _S( "Calendar" ),

    // clock alarm expired
    _S( "Clock" ),

    // unknown alarm expired
    _S( "Unknown" )
    };

// Enumerations for accessing values array
enum TAlarmSourceStateValues
    {
    EStateAlarmInactive,
    EStateCalendarAlarm,
    EStateClockAlarm,
    EStateOtherAlarm
    };

//-----------------------------------------------------------------------------
// Capabilities
_LIT_SECURITY_POLICY_PASS( KAlarmUISourceStateSec );


//-----------------------------------------------------------------------------
// Source:  'AlarmUI'
// Type:    'Command'
//
// Describes commands available for AlarmUI.
//-----------------------------------------------------------------------------
//
_LIT( KAlarmUISourceCommand, "Command" );

// Context values
_LIT_CONTEXT_VALUES( KAlarmUISourceCommandValues ) =
    {
    // command to stop an active alarm
    _S( "Stop" ),

    // command to snooze an active alarm
    _S( "Snooze" )
    };

// Enumerations for accessing values array
enum TAlarmSourceCommandValues
    {
    ECommandAlarmStop,
    ECommandAlarmSnooze
    };

//-----------------------------------------------------------------------------
// Capabilities
_LIT_SECURITY_POLICY_PASS( KAlarmUISourceCommandSec );


//-----------------------------------------------------------------------------
// Source:  'AlarmUI'
// Type:    'Result'
//
// Describes executed action (result) in AlarmUI.
//-----------------------------------------------------------------------------
//
_LIT( KAlarmUISourceResult, "Result" );

// Context values
_LIT_CONTEXT_VALUES( KAlarmUISourceResultValues ) =
    {
    // alarm was stopped
    _S( "Stopped" ),

    // alarm was snoozed
    _S( "Snoozed" ),

    // alarm was silenced
    _S( "Silenced" ),
    
    // alarm was Stopped.The sender(Calendar) can exit if required.
    _S( "StoppedAndExit" )
    };

// Enumerations for accessing values array
enum TAlarmSourceResultValues
    {
    EResultAlarmStopped,
    EResultAlarmSnoozed,
    EResultAlarmSilenced,
    EResultAlarmStoppedAndExit
    };

//-----------------------------------------------------------------------------
// Capabilities
_LIT_SECURITY_POLICY_PASS( KAlarmUISourceResultSec );


//-----------------------------------------------------------------------------
// Source:  'AlarmUI'
// Type:    'Action'
//
// Action id for snoozing the active alarm.
//-----------------------------------------------------------------------------
//
_LIT_CONTEXT_VALUES( KAlarmUIActions ) =
    {
    // snooze the active alarm
    _S( "AlarmUI.Snooze" ),

    // stop the active alarm
    _S( "AlarmUI.Stop" ),

    // print a debug string
    _S( "AlarmUI.Debug" )

    };

// Enumerations for accessing values array
enum TAlarmUIActionValues
    {
    EActionSnooze,
    EActionStop,
    EDebug
    };

//-----------------------------------------------------------------------------
// Capabilities
_LIT_SECURITY_POLICY_PASS( KAlarmUIActionSec );


#endif // ALARMSOURCECONTEXTDEF_H


// End of File
