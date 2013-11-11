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
* Description:  Works as a proxy together with RAknAlarmClient and CAknAlarmWrapper between EikAlert and CAlmAlarmControl. 
*
*/



// INCLUDES
#include "pim_trace.h"
#include "AlarmService.h"
#include "AlmAlarmControl.h"
#include <asshdalarm.h>

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
EXPORT_C MAknAlarmServiceStartup* Entry()
    {
    TRACE_ENTRY_POINT;
    CAknAlarmService* alarmService = new CAknAlarmService;
    TRACE_EXIT_POINT;
    return alarmService;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
CAknAlarmService::CAknAlarmService()
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
CAknAlarmService::~CAknAlarmService()
    {
    TRACE_ENTRY_POINT;
    if( iAlarmAlert )
        {
        iAlarmAlert->Release();
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
void CAknAlarmService::StartupNewSessionL(TAny*)
    {
    TRACE_ENTRY_POINT;
    iAlarmAlert=new (ELeave) CAlmAlarmControl();
    iAlarmAlert->ConstructL(this);
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
void CAknAlarmService::ServiceL(const RMessage2 &aMessage)
    {
    TRACE_ENTRY_POINT;
    TBool completeMessage( ETrue );

    switch( aMessage.Function() )
        {
        case EASAltOpCodeNotify:
            if ( iMessage.IsNull() )
                {
                iMessage = aMessage;
                completeMessage = EFalse;
                }
            break;

        case EASAltOpCodeNotifyCancel:
            {
            if( !iMessage.IsNull() )
                {
                iMessage.Complete( KErrCancel );
                }
            }
            break;

        case EASAltOpCodeVisible:
            {
            TInt value( 0 );
            TPckg<TInt> pckg( value );
            aMessage.ReadL( 0, pckg );

            if( value )
                {
                iAlarmAlert->ShowAlarm();
                }
            else
                {
                iAlarmAlert->HideAlarm();
                }
            }
            break;

        case EASAltOpCodeSetState:
            {
            TInt value( 0 );
            TPckg<TInt> pckg( value );
            aMessage.ReadL( 0, pckg );
            SetAlertState( value );
            }
            break;

        case EASAltOpCodeSetAlarm:
            UpdateAlarmInfo( aMessage );
            break;

        case EAknSAltOpCodeAskServerState:
            {
            TPckgC<TInt> pckgC( iAlarmAlert->CurrentServerState() );
            aMessage.WriteL( 0, pckgC );
            }
            break;

        default:
            aMessage.Complete( KErrNotSupported );
            break;
        }

    if( completeMessage && !aMessage.IsNull() )
        {
        aMessage.Complete( KErrNone );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
void CAknAlarmService::SetAlertState(TInt aAlarmServerStateFlags)
    {
    TRACE_ENTRY_POINT;
    iAlarmAlert->UpdateForAlarmServerState(aAlarmServerStateFlags);
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
void CAknAlarmService::UpdateAlarmInfo(const RMessage2& aMessage)
    {
    TRACE_ENTRY_POINT;
    TASShdAlarm alarm;
    TPckg<TASShdAlarm> pAlarm(alarm);
    PIM_TRAPD_ASSERT( aMessage.ReadL(0,pAlarm); )
    TFullName owner;
    PIM_TRAPD_ASSERT( aMessage.ReadL(1,owner); )
    iAlarmAlert->UpdateAlarmInfo(alarm,owner);
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
void CAknAlarmService::CmdAcknowledgeAlarm()
    {
    TRACE_ENTRY_POINT;
    if( !iMessage.IsNull() )
        {
        iMessage.Complete( ECmdAcknowledgedAlarm );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
TBool CAknAlarmService::CmdTaskAwayFromAlarmL()
    {
    TRACE_ENTRY_POINT;
    if( !iMessage.IsNull() )
        {
        iMessage.Complete( ECmdTaskAwayFromAlarm );
        }
    TRACE_EXIT_POINT;
    return ETrue;
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
TBool CAknAlarmService::CmdTaskAwayFromAlarmL(TInt aMinutesToSnooze)
    {
    TRACE_ENTRY_POINT;
    if( !iMessage.IsNull() )
        {
        TPckgC<TInt> pckgC( aMinutesToSnooze );
        iMessage.WriteL( 0, pckgC );
        iMessage.Complete( ECmdTaskAwayFromAlarmWTime );
        }
    TRACE_EXIT_POINT;
    return ETrue;
    }


// End of File
