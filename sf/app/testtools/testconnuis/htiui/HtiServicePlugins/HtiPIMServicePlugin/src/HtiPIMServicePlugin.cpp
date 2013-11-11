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
* Description:  PIMServicePlugin implementation
*
*/


// INCLUDE FILES
#include "HtiPIMServicePlugin.h"
#include "PIMHandler.h"

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
#include "HtiBookmarkHandler.h"
#endif

#include "HtiSimDirHandler.h"
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

// CONSTANTS
_LIT8( KErrorMissingCommand, "Missing command" );
_LIT8( KErrorUnrecognizedCommand, "Unrecognized command" );
// ----------------------------------------------------------------------------
// Create instance of concrete ECOM interface implementation
CHtiPIMServicePlugin* CHtiPIMServicePlugin::NewL()
    {
    CHtiPIMServicePlugin* self = new (ELeave) CHtiPIMServicePlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
CHtiPIMServicePlugin::CHtiPIMServicePlugin()
    {
    }

// ----------------------------------------------------------------------------
CHtiPIMServicePlugin::~CHtiPIMServicePlugin()
    {
    HTI_LOG_TEXT("CHtiPIMServicePlugin destroy");
    delete iPimHandler;
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    delete iBookmarkHandler;
#endif
    delete iSimDirHandler;
    }

// ----------------------------------------------------------------------------
void CHtiPIMServicePlugin::ConstructL()
    {
    HTI_LOG_TEXT("CHtiPIMServicePlugin::ConstructL");
    }

// ----------------------------------------------------------------------------
void CHtiPIMServicePlugin::ProcessMessageL( const TDesC8& aMessage,
    THtiMessagePriority aPriority )
    {
    HTI_LOG_FUNC_IN( "CHtiPIMServicePlugin::ProcessMessageL" );
    HTI_LOG_FORMAT( "Msg len: %d.", aMessage.Length() );

    if ( aMessage.Length() == 0 )
        {
        User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument, KErrorMissingCommand, KPIMServiceUid ) );
        return;
        }

    TUint8 aCommand = aMessage.Ptr()[0];
    if ( aCommand < ESimCardInfo )
        {
        if ( iPimHandler == NULL )
            {
            iPimHandler = CPIMHandler::NewL();
            iPimHandler->SetDispatcher( iDispatcher );
            }
        iPimHandler->ProcessMessageL( aMessage, aPriority );
        }
    else if ( aCommand < ECreateBookmark )
        {
        if ( iSimDirHandler == NULL )
            {
            iSimDirHandler = CHtiSimDirHandler::NewL();
            iSimDirHandler->SetDispatcher( iDispatcher );
            }
        iSimDirHandler->ProcessMessageL( aMessage, aPriority );
        }
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    else if ( aCommand <= EDeleteBookmark)
        {
        if ( iBookmarkHandler == NULL )
            {
            iBookmarkHandler = CHtiBookmarkHandler::NewL();
            iBookmarkHandler->SetDispatcher( iDispatcher );
            }
        iBookmarkHandler->ProcessMessageL( aMessage, aPriority );
        }
#endif
    else
        {
        User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument, KErrorUnrecognizedCommand, KPIMServiceUid ) );
        }

    HTI_LOG_FUNC_OUT( "CHtiPIMServicePlugin::ProcessMessageL" );
    }


// ----------------------------------------------------------------------------
TBool CHtiPIMServicePlugin::IsBusy()
    {
    if ( iPimHandler )
        {
        return iPimHandler->IsBusy();
        }
    
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    if ( iBookmarkHandler )
        {
        return iBookmarkHandler->IsBusy();
        }
#endif
    
    if( iSimDirHandler)
        {
        return iSimDirHandler->IsBusy();
        }
    return EFalse;
    }
