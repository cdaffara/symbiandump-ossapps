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
* Description:  MessagesServicePlugin implementation
*
*/


// INCLUDE FILES
#include "HtiMessagesServicePlugin.h"
#include "MessageMgmntHandler.h"
#include "HtiIAPHandler.h"
#include "HtiMailboxHandler.h"
#include "HtiMsgSettingsHandler.h"
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
#include "HtiNotificationHandler.h"
#endif

#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

// CONSTANTS
_LIT8( KErrorMissingCommand, "Command was not given - message was empty" );
_LIT8( KErrorUnrecognizedCommand, "Unrecognized command" );

// ----------------------------------------------------------------------------
// Create instance of concrete ECOM interface implementation
CHtiMessagesServicePlugin* CHtiMessagesServicePlugin::NewL()
    {
    CHtiMessagesServicePlugin* self = new (ELeave) CHtiMessagesServicePlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
CHtiMessagesServicePlugin::CHtiMessagesServicePlugin()
    {
    }

// ----------------------------------------------------------------------------
CHtiMessagesServicePlugin::~CHtiMessagesServicePlugin()
    {
    HTI_LOG_TEXT( "CHtiMessagesServicePlugin destroy" );
    delete iMessageHandler;
    delete iIAPHandler;
    delete iMailboxHandler;
    delete iMsgSettingsHandler;
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    delete iNotificationHandler;
#endif
    }

// ----------------------------------------------------------------------------
void CHtiMessagesServicePlugin::ConstructL()
    {
    HTI_LOG_TEXT( "CHtiMessagesServicePlugin::ConstructL" );

    }

// ----------------------------------------------------------------------------
void CHtiMessagesServicePlugin::ProcessMessageL( const TDesC8& aMessage,
    THtiMessagePriority aPriority)
    {
    HTI_LOG_FUNC_IN( "CHtiMessagesServicePlugin::ProcessMessageL" );
    HTI_LOG_FORMAT( "Msg len: %d.", aMessage.Length() );

    if ( aMessage.Length() == 0 )
        {
        User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument, KErrorMissingCommand, KHtiMessagesServiceUid ) );
        return;
        }

    TUint8 command = aMessage.Ptr()[0];

    if ( command >= EAddSms && command <= EDeleteFolderContent )
        {
        if ( !iMessageHandler )
            {
            iMessageHandler = CMessageMgmntHandler::NewL();
            iMessageHandler->SetDispatcher( iDispatcher );
            }
        iMessageHandler->ProcessMessageL( aMessage, aPriority );
        }
    else if ( command >= ECreateMailBox && command <= EDeleteMailBox  )
        {
        if ( !iMailboxHandler )
            {
            iMailboxHandler = CHtiMailboxHandler::NewL();
            iMailboxHandler->SetDispatcher( iDispatcher );
            }
        iMailboxHandler->ProcessMessageL( aMessage, aPriority );
        }
    else if ( command >= ECreateIAP && command <= ESetDefaultConnection )
        {
        if ( !iIAPHandler )
            {
            iIAPHandler = CHtiIAPHandler::NewL();
            iIAPHandler->SetDispatcher( iDispatcher );
            }
        iIAPHandler->ProcessMessageL( aMessage, aPriority );
        }

    else if ( command >= ESetDefaultSmsCenter && command <= ESetMmsSettings )
        {
        if ( !iMsgSettingsHandler )
            {
            iMsgSettingsHandler = CHtiMsgSettingsHandler::NewL();
            iMsgSettingsHandler->SetDispatcher( iDispatcher );
            }
        iMsgSettingsHandler->ProcessMessageL( aMessage, aPriority );
        }

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    else if ( command >= ECreateVoiceMessageNotification && command <= EClearAllNotifications)
        {
        if( !iNotificationHandler )
            {
            iNotificationHandler = CHtiNotificationHandler::NewL();
            iNotificationHandler->SetDispatcher(iDispatcher);
            }
        iNotificationHandler->ProcessMessageL( aMessage, aPriority );
        }
#endif

    else
        {
        User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument, KErrorUnrecognizedCommand, KHtiMessagesServiceUid ) );
        }

    HTI_LOG_FUNC_OUT( "CHtiMessagesServicePlugin::ProcessMessageL: Done" );
    }
