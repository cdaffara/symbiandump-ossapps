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
* Description:  Functional implementation of key event service
*
*/


// INCLUDE FILES
#include "HtiKeyEventServicePlugin.h"
#include "KeyEventHandler.h"

#include <apgtask.h>
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>


// CONSTANTS
_LIT8( KErrorServiceNotReady, "Service is busy - possibly executing long running typetext or long key press functionality" );
_LIT8( KErrorUnrecognizedCommand, "Unrecognized command" );
_LIT8( KErrorEmptyUnicodeArray, "Unicode char array was empty" );
_LIT8( KErrorInvalidUnicodeCharArray, "Invalid unicode character array" );
_LIT8( KErrorInvalidScancode, "Invalid scancode. Wrong length" );
_LIT8( KErrorTypeTextFailure, "Internal typetext failure" );
_LIT8( KErrorLongKeyPressFailure, "Internal long key press failure" );

const TInt KKeyboardFuncDataLength = 2;  // scancode with 2 bytes
const TInt KLongKeyPressDataLength = 4;  // scancode 2 bytes + time 2 bytes
const TInt KPressKeySequenceMinDataLength = 6;  // hold time 2 bytes + key press interval 2 bytes + at least one scancode 2 bytes
const TInt KTypeTextDelayBetweenCharsInMicroSeconds = 50000; // 50 milliseconds
const TInt KTypeTextStartPausing = 200; // start pausing after this many characters
const TInt KTypeTextPauseInterval = 50; // how many chars to type before pausing
const TInt KTypeTextPauseInMicroSeconds = 800000; // 0.8 sec


// ----------------------------------------------------------------------------
// UintValue()
// Helper: aPtr must point to 16 bits area where the value will be extracted
// ----------------------------------------------------------------------------
TUint UintValue( const TUint8* aPtr )
    {
    // This is short way to do it..
    //return (aPtr[1]<<8) + aPtr[0];

    TUint8 uInt8Low = *aPtr;
    TUint8 uInt8High = *(aPtr+1);

    HTI_LOG_FORMAT( "UintValue:: Low: %d", uInt8Low );
    HTI_LOG_FORMAT( "UintValue:: High: %d", uInt8High );

    TUint16 uInt16 = (TUint16)uInt8High;
    uInt16 = (uInt16 << 8);
    uInt16 = uInt16 + (TUint)uInt8Low;

    HTI_LOG_FORMAT( "UintValue:: 16-bit value: %d", uInt16 );

    TUint uInt = (TUint)uInt16;
    HTI_LOG_FORMAT( "UintValue:: 32-bit value: %d", uInt );

    return uInt;
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::NewL()
// ----------------------------------------------------------------------------
CKeyEventHandler* CKeyEventHandler::NewL()
    {
    HTI_LOG_FUNC_IN( "CKeyEventHandler::NewL" );
    CKeyEventHandler* self = new (ELeave) CKeyEventHandler();
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CKeyEventHandler::Done" );
    return self;
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::CKeyEventHandler()
// ----------------------------------------------------------------------------
CKeyEventHandler::CKeyEventHandler()
    : CActive( CActive::EPriorityStandard ), iReady( ETrue ), iCommand( 0 ),
      iHoldingScanCode( 0 )

    {
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::~CKeyEventHandler()
// ----------------------------------------------------------------------------
CKeyEventHandler::~CKeyEventHandler()
    {
    HTI_LOG_TEXT( "CKeyEventHandler destroy" );
    Cancel();
    iTimer.Close();
    iWsSession.Close();
    delete iUnicodeChars;
    iUnicodeChars = NULL;
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::ConstructL()
// ----------------------------------------------------------------------------
void CKeyEventHandler::ConstructL()
    {
    HTI_LOG_TEXT( "CKeyEventHandler::ConstructL" );
    User::LeaveIfError( iWsSession.Connect() );
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::SetDispatcher()
// ----------------------------------------------------------------------------
void CKeyEventHandler::SetDispatcher( MHtiDispatcher* aDispatcher )
    {
    iDispatcher = aDispatcher;
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::RunL()
// ----------------------------------------------------------------------------
void CKeyEventHandler::RunL()
    {
    HTI_LOG_FUNC_IN( "CKeyEventHandler::RunL" );

    if ( iCommand == ELongKeyPress )
        {
        KeyUp( iHoldingScanCode );
        SendOkMsgL();
        iReady = ETrue;
        HTI_LOG_TEXT( "CKeyEventHandler::RunL: LongKeyPress has finished" );
        }

    else if ( iCommand == ETypeText || iCommand == ETypeTextPassword )
        {
        SimulateKeyEventL( *iUnicodeChars, iUnicodeCharIndex );

        if ( iUnicodeCharIndex == iUnicodeChars->Length() / 2 - 1 )
            {
            delete iUnicodeChars;
            iUnicodeChars = NULL;
            SendOkMsgL();
            iReady = ETrue;
            HTI_LOG_TEXT( "CKeyEventHandler::RunL: TypeText has finished: OK" );
            }
        else
            {
            iUnicodeCharIndex++;
            TypeTextWithDelay( iUnicodeCharIndex );
            }
        }

    else if ( iCommand == EPressKeySequence )
        {
        PressKeySequenceKeyL();
        }

    HTI_LOG_FUNC_OUT( "CKeyEventHandler::RunL" );
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::RunError()
// ----------------------------------------------------------------------------
TInt CKeyEventHandler::RunError( TInt aError )
    {
    TInt dispatchResult = KErrNone;

    if ( iCommand == ELongKeyPress )
        {
        HTI_LOG_FORMAT( "CKeyEventHandler::RunError: LongKeyPress failure: %d.", aError );
        dispatchResult = iDispatcher->DispatchOutgoingErrorMessage( aError,
                KErrorLongKeyPressFailure,  KKeyEventServiceUid );
        }

    else // it's TypeText
        {
        HTI_LOG_FORMAT( "CKeyEventHandler::RunError: TypeText failure: %d.", aError );
        dispatchResult = iDispatcher->DispatchOutgoingErrorMessage( aError,
                KErrorTypeTextFailure,  KKeyEventServiceUid );
        }

    if ( dispatchResult != KErrNone )
        {
        HTI_LOG_FORMAT( "CKeyEventHandler::RunError: Failed to send error report to framework: %d.", dispatchResult );
        }
    iReady = ETrue;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::DoCancel()
// ----------------------------------------------------------------------------
void CKeyEventHandler::DoCancel()
    {
    iTimer.Cancel();
    delete iUnicodeChars;
    iUnicodeChars = NULL;
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::ProcessMessageL()
// ----------------------------------------------------------------------------
void CKeyEventHandler::ProcessMessageL( const TDesC8& aMessage,
    THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN("CKeyEventHandler::ProcessMessageL");
    HTI_LOG_FORMAT("Msg len: %d.", aMessage.Length());

    if ( !iReady )
        {
        SendErrorMessageL( ENotReadyCommand, KErrorServiceNotReady );
        return;
        }

    iCommand = aMessage.Ptr()[0];
    switch ( iCommand )
        {
        case EPressKey:
        case EKeyDown:
        case EKeyUp:
            HandleKeyboardFuncL( aMessage.Right( aMessage.Length() - 1 ) );
            break;
        case ETypeText:
        case ETypeTextPassword:
            HandleTypeTextFuncL( aMessage.Right( aMessage.Length() - 1 ) );
            break;
        case ELongKeyPress:
            HandleLongKeyPressFuncL( aMessage.Right( aMessage.Length() - 1 ) );
            break;
        case EPressKeySequence:
            HandlePressKeySequenceFuncL( aMessage.Right( aMessage.Length() - 1 ) );
            break;
        default:
            SendErrorMessageL( EUnrecognizedCommand,
                KErrorUnrecognizedCommand );
        break;
        }

    HTI_LOG_FUNC_OUT( "CKeyEventHandler::ProcessMessageL: Done" );
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::HandleTypeTextFuncL()
// Handle request to send one or more unicode characters to top most
// focused UI application.
// ----------------------------------------------------------------------------
void CKeyEventHandler::HandleTypeTextFuncL(
    const TDesC8& aUnicodeChars )
    {
    HTI_LOG_FUNC_IN( "CKeyEventHandler::HandleTypeTextFunc" );

    // For UCS-2 and common unicode key code areas, see
    // http://czyborra.com/unicode/characters.html
    // "How are the Unicode characters numbered?"

    if ( aUnicodeChars.Size() == 0 )
        {
        SendErrorMessageL( EEmptyUnicodeCharArray, KErrorEmptyUnicodeArray );
        return;
        }

#ifdef __ENABLE_LOGGING__
    CArrayFixFlat<TInt> *windowGroups = new (ELeave) CArrayFixFlat<TInt>(5);
    iWsSession.WindowGroupList(windowGroups);
    HTI_LOG_FORMAT("WindowGroupList, %d groups:",windowGroups->Count());
    for ( TInt i = 0; i < windowGroups->Count(); i++ )
    {
        TBuf<125> wgname; // max length for wg name?
        iWsSession.GetWindowGroupNameFromIdentifier((*windowGroups)[i], wgname);

        TBuf<145> logmsg;
        logmsg.AppendFormat(_L("WgId %3d - \""), (*windowGroups)[i]);
        logmsg.Append(wgname);
        logmsg.Append(_L("\""));
        HTI_LOG_DES(logmsg);
    }
    delete windowGroups;
#endif

    // Get the front most focused window group. Note that this might
    // not necessarily be the front-most window group, as
    // window groups can disable keyboard focus.
    iFocusedWindowGroup = iWsSession.GetFocusWindowGroup();

    HTI_LOG_TEXT("FocusWindowGroup name :");
    TBuf<125> focusWindowGroupName; // max length for wg name?
    iWsSession.GetWindowGroupNameFromIdentifier(iFocusedWindowGroup, focusWindowGroupName);
    HTI_LOG_DES(focusWindowGroupName);

    // ...exception to above! (in EKA2) :
    // When we are in idle state the text must be sent to 'Telephone' task's
    // windowgroup because 'idle' windowgroup does not handle keyevents.
    //
    // Also when typing in idle state we need EKeyUp event to prevent the key
    // beeing stuck in the pressed position.
    //
    // Window group name contains UID - we are using UID to find the
    // 'Idle' window group so it will be found also with different
    // localizations. The name 'Idle' cannot be used since the name is
    // localized.

    if ( ( focusWindowGroupName.Find( _L( "101fd64c" ) ) != KErrNotFound ) ||
         ( focusWindowGroupName.Find( _L( "102750f0" ) ) != KErrNotFound ) )
        {
        HTI_LOG_TEXT( "FocusWindowGroup is \"Idle\". Sending text to \"Telephone\" task." );
        TApaTaskList taskList( iWsSession );
        TApaTask telephoneTask = taskList.FindApp( TUid::Uid( 0x100058b3 ) );
        if ( !telephoneTask.Exists() )
            {
            HTI_LOG_FUNC_OUT( "CKeyEventHandler::SimulateKeyEventL: \"Telephone\" task does not exist" );
            User::Leave( KErrNotFound );
            }

        iFocusedWindowGroup = telephoneTask.WgId();

        // We must also get focus to 'Telephone' task for it
        // to receive our text.
        TWsEvent event;
        event.SetType( EEventFocusGained );
        iWsSession.SendEventToWindowGroup( telephoneTask.WgId(), event );
        }

    if ( aUnicodeChars.Size() % 2 == 1 )
        {
        // aUnicodeChars descriptor must be divisible by two,
        // because expecting two-byte characters
        SendErrorMessageL( EInvalidUnicodeCharArray,
            KErrorInvalidUnicodeCharArray );
        return;
        }

    // Start work
    if ( iUnicodeChars )
        {
        delete iUnicodeChars;
        iUnicodeChars = NULL;
        }

    // If the string begins with unicode byte order mark 0xfffe, strip it away
    if ( ( aUnicodeChars[0] == 0xff && aUnicodeChars[1] == 0xfe ) )
        {
        HTI_LOG_TEXT( "Ignoring Unicode BOM (first 2 bytes) from the string" );
        iUnicodeChars = aUnicodeChars.Mid( 2 ).AllocL();
        }
    else // take the string as it is
        {
        iUnicodeChars = aUnicodeChars.AllocL();
        }
    iReady = EFalse;

    TypeTextWithDelay( 0 );

    HTI_LOG_FUNC_OUT("CKeyEventHandler::HandleTypeTextFunc: Done (operation started)");
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::TypeTextWithDelay()
// ----------------------------------------------------------------------------
void CKeyEventHandler::TypeTextWithDelay( TInt aNextIndex )
    {
    iUnicodeCharIndex = aNextIndex;
    // Keep a longer pause in some character intervals if the given text
    // is very long to avoid losing characters.
    if ( iUnicodeCharIndex > KTypeTextStartPausing &&
        iUnicodeCharIndex % KTypeTextPauseInterval == 0 )
        {
        iTimer.After( iStatus, KTypeTextPauseInMicroSeconds );
        }
    else
        {
        iTimer.After( iStatus, KTypeTextDelayBetweenCharsInMicroSeconds );
        }
    SetActive();
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::SimulateKeyEventL()
// ----------------------------------------------------------------------------
void CKeyEventHandler::SimulateKeyEventL( const TDesC8& aUnicodeChars,
    TInt aIndex )
    {

    // Expect aUnicodeChars to be Unicode characters (each two bytes)
    // Note: aIndex is within range of 0 to aUnicodeChars/2

    HTI_LOG_FUNC_IN("CKeyEventHandler::SimulateKeyEventL");
    TKeyEvent keyEvent;
    const TUint8* rawDataStart = aUnicodeChars.Ptr();
    keyEvent.iCode = *( (TUint16*) ( rawDataStart + aIndex * 2 ) );
    // Set modifier to treat key events as events from external keyboard
    // so that AknFep won't interfere with numbers and * and # characters.
    keyEvent.iModifiers = EModifierKeyboardExtend;
    keyEvent.iRepeats = 0;
    //keyEvent.iScanCode = EStdKeyNull;
    // change because FEP got confused about EStdKeyNull in key-up event and
    // opened the edit menu
    keyEvent.iScanCode = 255;

    TWsEvent event;
    *event.Key() = keyEvent;

    if ( iCommand == ETypeTextPassword )
        {
        // Typing a password needs only EEventKey.
        // Otherwise it will appear to be pressed twice.
        HTI_LOG_FORMAT( "EEventKey 0x%x", keyEvent.iCode );
        event.SetType( EEventKey );
        iWsSession.SendEventToWindowGroup( iFocusedWindowGroup, event );
        }
    else
        {
        // EEventKeyUp is needed when typing a SMS or MMS. Otherwise the
        // counters will not be updated and message will be corrupt.
        // Also typing in idle state requires EEventKeyUp for the key to be
        // released.
        HTI_LOG_FORMAT( "EEventKey&EEventKeyUp 0x%x", keyEvent.iCode );
        event.SetType( EEventKey );
        iWsSession.SendEventToWindowGroup( iFocusedWindowGroup, event );

        event.SetType( EEventKeyUp );
        iWsSession.SendEventToWindowGroup( iFocusedWindowGroup, event );
        }

    HTI_LOG_FUNC_OUT( "CKeyEventHandler::SimulateKeyEventL" );
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::KeyUp()
// ----------------------------------------------------------------------------
void CKeyEventHandler::KeyUp( TUint aCode )
    {
    HTI_LOG_FUNC_IN( "CKeyEventHandler::KeyUp: Starting" );
    TRawEvent rawEvent;
    rawEvent.Set( TRawEvent::EKeyUp, aCode );
    iWsSession.SimulateRawEvent( rawEvent );
    iWsSession.Flush();
    HTI_LOG_FUNC_OUT( "CKeyEventHandler::KeyUp: Done" );
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::KeyDown()
// ----------------------------------------------------------------------------
void CKeyEventHandler::KeyDown( TUint aCode )
    {
    HTI_LOG_FUNC_IN( "CKeyEventHandler::KeyDown: Starting" );
    User::ResetInactivityTime();
    TRawEvent rawEvent;
    rawEvent.Set( TRawEvent::EKeyDown, aCode );
    iWsSession.SimulateRawEvent( rawEvent );
    iWsSession.Flush();
    HTI_LOG_FUNC_OUT( "CKeyEventHandler::KeyDown: Done" );
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::HandleKeyboardFuncL()
// Handle request to emulate pressing keyboard keys in sequence.
// ----------------------------------------------------------------------------
void CKeyEventHandler::HandleKeyboardFuncL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CKeyEventHandler::HandleKeyboardFuncL" );

    HTI_LOG_FORMAT( "CKeyEventHandler::HandleKeyboardFuncL: Size of data: %d", aData.Length() );
    HTI_LOG_TEXT( "CKeyEventHandler::HandleKeyboardFuncL: Data: " );
    HTI_LOG_DES( aData );

//  <scancode> is unsigned short integer, a scancode of
//  keyboard key. Theses are defined in E32KEYS.H. It depends on
//  device how these are mapped to actual keyboard keys in the
//  device.
//
//  Client note: each call to KEYDOWN should be followed by KEYUP
//               with the same scancode at some point.

    if ( aData.Length() != KKeyboardFuncDataLength )
        {
//        HTI_LOG_FORMAT( "PRESSKEY: Invalid scancode length: %d (expecting 2 bytes).", ptr.Length() - 1);
        SendErrorMessageL( EInvalidScancode, KErrorInvalidScancode );
        return;
        }

    iReady = EFalse;

    TUint scanCode = UintValue( aData.Ptr() );

    switch ( iCommand )
        {
        case EPressKey:
            KeyDown( scanCode );
            KeyUp( scanCode );
            break;

        case EKeyDown:
            KeyDown( scanCode );
            break;

        case EKeyUp:
            KeyUp( scanCode );
            break;

        default:
        break;
        }
    SendOkMsgL();
    iReady = ETrue;
    HTI_LOG_FUNC_OUT("CKeyEventHandler::HandleKeyboardFuncL: Done");
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::HandleLongKeyPressFuncL()
// Handle request to emulate holding a key down for specified time.
// ----------------------------------------------------------------------------
void CKeyEventHandler::HandleLongKeyPressFuncL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CKeyEventHandler::HanldeLongKeyPressFuncL" );

    if ( aData.Length() != KLongKeyPressDataLength )
        {
        SendErrorMessageL( EInvalidScancode, KErrorInvalidScancode );
        return;
        }

    iReady = EFalse;
    iHoldingScanCode = UintValue( aData.Ptr() );
    TTimeIntervalMicroSeconds32 holdTime( UintValue( aData.Ptr() + 2 ) * 1000 );
    KeyDown( iHoldingScanCode );
    iTimer.After( iStatus, holdTime );
    SetActive();

    HTI_LOG_FUNC_OUT( "CKeyEventHandler::HanldeLongKeyPressFuncL" );
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::HandlePressKeySequenceFuncL()
// Handle request to press keys in sequence.
// ----------------------------------------------------------------------------
void CKeyEventHandler::HandlePressKeySequenceFuncL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CKeyEventHandler::HandlePressKeySequenceFuncL" );

    if ( aData.Length() % 2 != 0 )
        {
        HTI_LOG_FORMAT( "Invalid data length: %d", aData.Length() );
        SendErrorMessageL( EInvalidUnicodeCharArray, KErrorInvalidUnicodeCharArray );
        return;
        }

    if ( aData.Length() < KPressKeySequenceMinDataLength )
        {
        HTI_LOG_FORMAT( "Too short data: %d", aData.Length() );
        SendErrorMessageL( EEmptyUnicodeCharArray, KErrorEmptyUnicodeArray );
        return;
        }

    iReady = EFalse;

    iKeySequenceHoldTime = UintValue( aData.Ptr() ) * 1000;
    iKeySequenceInterval = UintValue( aData.Ptr() + 2 ) * 1000;
    HTI_LOG_FORMAT( "Parsed key sequence hold time: %d", iKeySequenceHoldTime.Int() / 1000 );
    HTI_LOG_FORMAT( "Parsed key sequence interval: %d", iKeySequenceInterval.Int() / 1000 );
    if ( iKeySequenceInterval.Int() < KTypeTextDelayBetweenCharsInMicroSeconds )
        {
        HTI_LOG_TEXT( "Given key sequence interval too low, using the default value" );
        iKeySequenceInterval = KTypeTextDelayBetweenCharsInMicroSeconds;
        }

    if(iUnicodeChars)
		{
			delete iUnicodeChars;
			iUnicodeChars = NULL;
		}
	
	iUnicodeChars = aData.Mid( 4 ).AllocL();
    iUnicodeCharIndex = 0;
    iHoldingKey = EFalse;

    PressKeySequenceKeyL();

    HTI_LOG_FUNC_OUT( "CKeyEventHandler::HandlePressKeySequenceFuncL" );
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::PressKeySequenceKeyL()
// Handle a single key press from press keys in sequence command.
// ----------------------------------------------------------------------------
void CKeyEventHandler::PressKeySequenceKeyL()
    {
    HTI_LOG_FUNC_IN( "CKeyEventHandler::PressKeySequenceKeyL" );

    if ( iHoldingKey )
        {
        KeyUp( iHoldingScanCode );
        iHoldingKey = EFalse;

        HTI_LOG_FORMAT( "Released key: %d", iHoldingScanCode );

        if ( iUnicodeChars->Length() > iUnicodeCharIndex )
            {
            iTimer.After( iStatus, iKeySequenceInterval );
            SetActive();
            }
        else
            {
            HTI_LOG_TEXT( "PressKeySequence has finished: OK" );
            SendOkMsgL();
            iReady = ETrue;
            }
        }
    else
        {
        iHoldingScanCode = UintValue( iUnicodeChars->Ptr() + iUnicodeCharIndex );
        iUnicodeCharIndex += 2;

        KeyDown( iHoldingScanCode );
        iHoldingKey = ETrue;

        HTI_LOG_FORMAT( "Holding down key: %d", iHoldingScanCode );

        iTimer.After( iStatus, iKeySequenceHoldTime );
        SetActive();
        }

    HTI_LOG_FUNC_OUT( "CKeyEventHandler::HandlePressKeySequenceFuncL" );
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::SendOkMsgL()
// ----------------------------------------------------------------------------
void CKeyEventHandler::SendOkMsgL()
    {
    HTI_LOG_FUNC_IN("CKeyEventHandler::SendOkMsgL: Starting");

    User::LeaveIfNull( iDispatcher );
    TBuf8<1> response;
    response.Append( EResultOk );
    HBufC8* respH8 = response.AllocL();
    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
        respH8, KKeyEventServiceUid ) );

    HTI_LOG_FUNC_OUT("CKeyEventHandler::SendOkMsgL: Done");
    }

// ----------------------------------------------------------------------------
// CKeyEventHandler::SendErrorMessageL()
// ----------------------------------------------------------------------------
void CKeyEventHandler::SendErrorMessageL( TInt aError,
        const TDesC8& aDescription )
    {
    HTI_LOG_FUNC_IN("CKeyEventHandler::SendErrorMessageL: Starting");
    User::LeaveIfNull( iDispatcher );
    User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
        aError, aDescription, KKeyEventServiceUid ) );
    HTI_LOG_FUNC_OUT("CKeyEventHandler::SendErrorMessageL: Done");
    }
