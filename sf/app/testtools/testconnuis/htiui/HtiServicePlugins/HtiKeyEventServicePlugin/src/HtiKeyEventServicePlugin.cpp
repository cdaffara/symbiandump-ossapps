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
* Description:  KeyEventPlugin entry implementation
*
*/


// INCLUDE FILES
#include "HtiKeyEventServicePlugin.h"
#include "KeyEventHandler.h"
#include "PointerEventHandler.h"

#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>
#include <hwrmlightdomaincrkeys.h>

// CONSTANTS
_LIT8( KErrorMissingCommand, "Command was not given - message was empty" );
const TInt KDefaultMinLightTimeout = 5;
const TInt KWakeupWaitTime = 200000; // 0.2 sec

// ----------------------------------------------------------------------------
// CHtiKeyEventServicePlugin::NewL()
// Create instance of concrete ECOM interface implementation
// ----------------------------------------------------------------------------
CHtiKeyEventServicePlugin* CHtiKeyEventServicePlugin::NewL()
    {
    CHtiKeyEventServicePlugin* self = new (ELeave) CHtiKeyEventServicePlugin;
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CHtiKeyEventServicePlugin::CHtiKeyEventServicePlugin()
// ----------------------------------------------------------------------------
CHtiKeyEventServicePlugin::CHtiKeyEventServicePlugin()
        : iLightTimeout( KDefaultMinLightTimeout ),
          iCenRepSession( NULL ),
          iLightTimeoutSettingHandler( NULL )
    {
    }

// ----------------------------------------------------------------------------
// CHtiKeyEventServicePlugin::~CHtiKeyEventServicePlugin()
// ----------------------------------------------------------------------------
CHtiKeyEventServicePlugin::~CHtiKeyEventServicePlugin()
    {
    HTI_LOG_TEXT( "CHtiKeyEventServicePlugin destroy" );
    delete iKeyHandler;
    delete iPointerHandler;

    if ( iLightTimeoutSettingHandler )
        {
        iLightTimeoutSettingHandler->StopListening();
        }
    delete iLightTimeoutSettingHandler;
    delete iCenRepSession;
    }

// ----------------------------------------------------------------------------
// CHtiKeyEventServicePlugin::ConstructL()
// ----------------------------------------------------------------------------
void CHtiKeyEventServicePlugin::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiKeyEventServicePlugin::ConstructL" );
    iCenRepSession = CRepository::NewL( KCRUidLightSettings );
    iLightTimeoutSettingHandler = CCenRepNotifyHandler::NewL( *this,
            *iCenRepSession, CCenRepNotifyHandler::EIntKey,
            KDisplayLightsTimeout );
    iLightTimeoutSettingHandler->StartListeningL();
    HTI_LOG_FUNC_OUT( "CHtiKeyEventServicePlugin::ConstructL" );
    }

// ----------------------------------------------------------------------------
// CHtiKeyEventServicePlugin::ProcessMessageL()
// ----------------------------------------------------------------------------
void CHtiKeyEventServicePlugin::ProcessMessageL( const TDesC8& aMessage,
    THtiMessagePriority aPriority)
    {
    HTI_LOG_FUNC_IN( "CHtiKeyEventServicePlugin::ProcessMessageL" );

    if ( aMessage.Length() == 0 )
        {
        User::LeaveIfNull( iDispatcher );
        User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
            CKeyEventHandler::EMissingCommand,
            KErrorMissingCommand, KKeyEventServiceUid ) );
        return;
        }

    HTI_LOG_FORMAT( "Light timeout = %d", iLightTimeout );
    TTimeIntervalSeconds inactivityTime = User::InactivityTime();
    HTI_LOG_FORMAT( "Inactivity time = %d", inactivityTime.Int() );
    if ( inactivityTime.Int() >= iLightTimeout )
        {
        HTI_LOG_TEXT( "Waking up" );
        User::ResetInactivityTime();
        User::After( KWakeupWaitTime );
        }

    if ( aMessage[0] <= CKeyEventHandler::EPressKeySequence )
        {
        if ( iKeyHandler == NULL )
            {
            iKeyHandler = CKeyEventHandler::NewL();
            iKeyHandler->SetDispatcher( iDispatcher );
            }
        iKeyHandler->ProcessMessageL( aMessage, aPriority );
        }

    else 
        {
        if ( iPointerHandler == NULL )
            {
            iPointerHandler = CPointerEventHandler::NewL();
            iPointerHandler->SetDispatcher( iDispatcher );
            }
        iPointerHandler->ProcessMessageL( aMessage, aPriority );
        }   


    HTI_LOG_FUNC_OUT( "CHtiKeyEventServicePlugin::ProcessMessageL: Done" );
    }

// ---------------------------------------------------------------------------
// From MCenRepNotifyHandlerCallback.
// CHtiKeyEventServicePlugin::HandleNotifyInt
// ---------------------------------------------------------------------------
//
void CHtiKeyEventServicePlugin::HandleNotifyInt( TUint32 aId, TInt aNewValue )
    {
    HTI_LOG_FUNC_IN( "CHtiKeyEventServicePlugin::HandleNotifyInt" );
    if ( aId == KDisplayLightsTimeout )
        {
        HTI_LOG_FORMAT( "New light timeout value %d", aNewValue );
        iLightTimeout = aNewValue;
        }
    HTI_LOG_FUNC_OUT( "CHtiKeyEventServicePlugin::HandleNotifyInt" );
    }

// ---------------------------------------------------------------------------
// From MCenRepNotifyHandlerCallback.
// CHtiKeyEventServicePlugin::HandleNotifyError
// ---------------------------------------------------------------------------
//
void CHtiKeyEventServicePlugin::HandleNotifyError( TUint32 /*aId*/,
        TInt /*error*/, CCenRepNotifyHandler* /*aHandler*/ )
    {
    HTI_LOG_TEXT( "CHtiKeyEventServicePlugin::HandleNotifyError()" );
    iLightTimeout = KDefaultMinLightTimeout;
    TRAP_IGNORE( iLightTimeoutSettingHandler->StartListeningL() );
    }

// End of file
