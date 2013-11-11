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
* Description:  Message Preview Plugin main class
 *
*/


// SYSTEM INCLUDES
#include <mcsplugineventobserver.h>

// USER INCLUDES
#include "ccspreviewplugin.h"
#include "ccspreviewpluginhandler.h"

// ============================== MEMBER FUNCTIONS ============================
// ----------------------------------------------------------------------------
// CCsPreviewPlugin::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
//
CCsPreviewPlugin* CCsPreviewPlugin::NewL(MCsPluginEventObserver* aObserver)
    {
    PRINT ( _L("Enter CCsPreviewPlugin::NewL") );

    CCsPreviewPlugin* self = new ( ELeave ) CCsPreviewPlugin(aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    PRINT ( _L("End CCsPreviewPlugin::NewL") );

    return self;
    }

// ----------------------------------------------------------------------------
// CCsPreviewPlugin::CCsPreviewPlugin
// Two Phase Construction
// ----------------------------------------------------------------------------
//
CCsPreviewPlugin::CCsPreviewPlugin(MCsPluginEventObserver* aObserver):iPluginEventObserver (aObserver)
            {
            }

// ----------------------------------------------------------------------------
// CCsPreviewPlugin::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
//
void CCsPreviewPlugin::ConstructL()
    {
    PRINT ( _L("Enter CCsPreviewPlugin::ConstructL") );
    iPreviewPluginHandler=CCsPreviewPluginHandler::NewL(this);
    PRINT ( _L("Exit CCsPreviewPlugin::ConstructL") );
    }
// ----------------------------------------------------------------------------
// CCsPreviewPlugin::~CCsPreviewPlugin
// Destructor
// ----------------------------------------------------------------------------
//
CCsPreviewPlugin::~CCsPreviewPlugin()
    {
    PRINT ( _L("Enter CCsPreviewPlugin::~CCsPreviewPlugin") );

    if(iPreviewPluginHandler)
        delete iPreviewPluginHandler;
    iPreviewPluginHandler = NULL;

    PRINT ( _L("End CCsPreviewPlugin::~CCsPreviewPlugin") );
    }


// -----------------------------------------------------------------------------
// CCsPreviewPlugin::HandleCachingCompleted
// Sends the cache completed status to server
// -----------------------------------------------------------------------------
//
void CCsPreviewPlugin::HandleCachingCompleted()
    {
    iPluginEventObserver->CachingCompleted();
    }

// -----------------------------------------------------------------------------
// CCsPreviewPlugin::HandleCachingError
// Sends the error occured during the caching to server
// -----------------------------------------------------------------------------
//
void CCsPreviewPlugin::HandleCachingError(const TInt aError)
    {
    iPluginEventObserver->CachingError(aError);
    }

// -----------------------------------------------------------------------------
// CCsPreviewPlugin::GetConversationsL()
// This function starts the state machine to fetch msg data from msvserver
// -----------------------------------------------------------------------------
//
void CCsPreviewPlugin::GetConversationsL()
    {
    PRINT ( _L("Enter CCsPreviewPlugin::GetConversationsL") );
    //THE initial caching is yet to be handled
    HandleCachingCompleted();
    }


// End of file
