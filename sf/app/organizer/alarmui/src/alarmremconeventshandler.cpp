/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class takes care of accessory events that should be 
*                handled in alarm.  I.e. stopping alarm using accessory end key.
*
*/



#include "alarmremconeventshandler.h"

#include "AlmAlarmControl.h"
#include <remconinterfaceselector.h>
#include <RemConCallHandlingTarget.h>
#include <RemConExtensionApi.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CAlarmRemConEventsHandler* CAlarmRemConEventsHandler::NewL(CAlmAlarmControl& aAlarmUI)
    {
    TRACE_ENTRY_POINT;
    CAlarmRemConEventsHandler* self = new( ELeave )CAlarmRemConEventsHandler( aAlarmUI );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_EXIT_POINT;
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CAlarmRemConEventsHandler::~CAlarmRemConEventsHandler()
    {
    TRACE_ENTRY_POINT;
    // cancel the AO and delete all data
    Cancel();
    Cleanup();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// from base class CActive
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::DoCancel()
    {
    TRACE_ENTRY_POINT;
    Cleanup();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// from base class CActive
// ---------------------------------------------------------------------------
//
TInt CAlarmRemConEventsHandler::RunError(TInt /*aError*/)
    {
    TRACE_ENTRY_POINT;
    Cleanup();
    TRACE_EXIT_POINT;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// from base class CActive
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::RunL()
    {
    TRACE_ENTRY_POINT;
    TBool sendResponse( EFalse );

    switch( iState )
        {
        case EStateIdle:
        case EStateWaitingCommand:
            // do nothing
            break;

        case EStateHandleEndCallCommand:
            {
            // stop the alarm after sending the response
            iState = EStateStopAlarm;
            sendResponse = ETrue;
            }
            break;

        case EStateIgnoreCommand:
            {
            // command was ignored, keep waiting for more commands
            iState = EStateWaitingCommand;
            sendResponse = ETrue;
            }
            break;

        case EStateStopAlarm:
            {
            // stop the alarm and keep waiting for more commands
            iState = EStateWaitingCommand;
            iAlarmUI.ExternalStopAlarm();
            }
            break;

        default:
            {
            ASSERT( EFalse );
            }
        }

    if( sendResponse )
        {
        // send the response
        TInt status = iStatus.Int();
        SetActive();
        iRemConCallHandlingTarget->SendResponse( iStatus, iRemConOperationID, status );
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// From MRemConCallHandlingTargetObserver.
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::EndCall()
    {
    TRACE_ENTRY_POINT;
    HandleRemConCommand( ERemConExtEndCall );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// From MRemConCallHandlingTargetObserver.
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::AnswerEndCall()
    {
    TRACE_ENTRY_POINT;
    HandleRemConCommand( ERemConExtAnswerEnd );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// From MRemConCallHandlingTargetObserver.
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::AnswerCall()
    {
    TRACE_ENTRY_POINT;
    HandleRemConCommand( ERemConExtAnswerCall );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// From MRemConCallHandlingTargetObserver.
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::VoiceDial(const TBool /*aActivate*/)
    {
    TRACE_ENTRY_POINT;
    HandleRemConCommand( ERemConExtVoiceDial );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// From MRemConCallHandlingTargetObserver.
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::LastNumberRedial()
    {
    TRACE_ENTRY_POINT;
    HandleRemConCommand( ERemConExtLastNumberRedial );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// From MRemConCallHandlingTargetObserver.
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::DialCall(const TDesC8& /*aTelNumber*/)
    {
    TRACE_ENTRY_POINT;
    HandleRemConCommand( ERemConExtDialCall );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// From MRemConCallHandlingTargetObserver.
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::MultipartyCalling(const TDesC8& /*aData*/)
    {
    TRACE_ENTRY_POINT;
    HandleRemConCommand( ERemConExt3WaysCalling );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// From MRemConCallHandlingTargetObserver.
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::GenerateDTMF(const TChar /*aChar*/)
    {
    TRACE_ENTRY_POINT;
    HandleRemConCommand( ERemConExtGenerateDTMF );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// From MRemConCallHandlingTargetObserver.
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::SpeedDial(const TInt /*aIndex*/)
    {
    TRACE_ENTRY_POINT;
    HandleRemConCommand( ERemConExtSpeedDial );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CAlarmRemConEventsHandler::CAlarmRemConEventsHandler(CAlmAlarmControl& aAlarmUI)
: CActive( CActive::EPriorityStandard ),
  iState( EStateIdle ),
  iRemConOperationID( -1 ),
  iAlarmUI( aAlarmUI )
    {
    TRACE_ENTRY_POINT;
    /** no implementation **/
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::ConstructL()
    {
    TRACE_ENTRY_POINT;
    CActiveScheduler::Add( this );
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::HandleRemConCommand(TInt aCommand)
    {
    TRACE_ENTRY_POINT;
    ASSERT( iState == EStateWaitingCommand && IsAdded() && !IsActive() );

    // there shouldn't be new commands while we're still handling the previous one
    if( iState == EStateWaitingCommand )
        {
        iRemConOperationID = aCommand;

        switch( iRemConOperationID )
            {
            case ERemConExtEndCall:
            case ERemConExtAnswerEnd:
                // handle message (stop the alarm)
                iState = EStateHandleEndCallCommand;
                break;

            case ERemConExtAnswerCall:
            case ERemConExtVoiceDial:
            case ERemConExtLastNumberRedial:
            case ERemConExtDialCall:
            case ERemConExt3WaysCalling:
            case ERemConExtGenerateDTMF:
            case ERemConExtSpeedDial:
                // no event handling...just send the response
                iState = EStateIgnoreCommand;
                break;

            default:
                ASSERT( EFalse );
            }
        // start the AO
        CompleteSelf();
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::CompleteSelf()
    {
    TRACE_ENTRY_POINT;
    TRequestStatus* pStat = &iStatus;
    User::RequestComplete( pStat, KErrNone );

    if( IsAdded() && !IsActive() )
        {
        SetActive();
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::StartL()
    {
    TRACE_ENTRY_POINT;
    if( !iRemConInterfaceSelector && !iRemConCallHandlingTarget )
        {
        // create the RemCon interface selector
        CRemConInterfaceSelector* selector = CRemConInterfaceSelector::NewL();
        CleanupStack::PushL( selector );

        // create the RemCon target
        CRemConCallHandlingTarget* target = CRemConCallHandlingTarget::NewL( *selector, *this );
        CleanupStack::PushL( target );

        // open a target session to RemCon
        selector->OpenTargetL();
        CleanupStack::Pop( 2, selector );

        // RemCon initialisation ok
        iRemConCallHandlingTarget = target;
        iRemConInterfaceSelector = selector;

        iState = EStateWaitingCommand;
        }
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::Stop()
    {
    TRACE_ENTRY_POINT;
    Cancel();
    Cleanup();
    TRACE_EXIT_POINT;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CAlarmRemConEventsHandler::Cleanup()
    {
    TRACE_ENTRY_POINT;
    iState = EStateIdle;
    iRemConOperationID = -1;
    iRemConCallHandlingTarget = NULL; // owned by iRemConInterfaceSelector
    delete iRemConInterfaceSelector;
    iRemConInterfaceSelector = NULL;
    TRACE_EXIT_POINT;
    }


// End of File
