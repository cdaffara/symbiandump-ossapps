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
* Description:  Haptics OBEX connection plugin.
*
*/


#include "hapticsconnplugin.h"
#include "hapticsmsghandler.h"
#include "hapticsconntrace.h"

// -----------------------------------------------------------------------------
// CHapticsConnPlugin::NewL()
// Two-phase constructing
// -----------------------------------------------------------------------------
//
CHapticsConnPlugin* CHapticsConnPlugin::NewL()
    {
    COMPONENT_TRACE( ( _L( "CHapticsConnPlugin::NewL - Begin" ) ) );
    CHapticsConnPlugin* self = new ( ELeave ) CHapticsConnPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    COMPONENT_TRACE( ( _L( "CHapticsConnPlugin::NewL - End" ) ) );
    return( self );
    }

// -----------------------------------------------------------------------------
// CHapticsConnPlugin::ConstructL()
// 
// -----------------------------------------------------------------------------
//
void CHapticsConnPlugin::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "CHapticsConnPlugin::ConstructL() - Begin" ) ) );
    iMsgHandler = CHapticsMsgHandler::NewL();
    iMsgHandler->StartNotifier();
    COMPONENT_TRACE( ( _L( "CHapticsConnPlugin::ConstructL() - End" )));
    }

// -----------------------------------------------------------------------------
// CHapticsConnPlugin::~CHapticsConnPlugin()
// Destructor
// -----------------------------------------------------------------------------
//
CHapticsConnPlugin::~CHapticsConnPlugin()
    {
    COMPONENT_TRACE( ( _L( "CHapticsConnPlugin::~CHapticsConnPlugin() - Begin" ) ) );
    delete iMsgHandler;
    COMPONENT_TRACE( ( _L( "CHapticsConnPlugin::~CHapticsConnPlugin() - End" ) ) );
    }

// -----------------------------------------------------------------------------
// CHapticsConnPlugin::CHapticsConnPlugin()
// Constructor
// -----------------------------------------------------------------------------
//
CHapticsConnPlugin::CHapticsConnPlugin()
    {    
    }

// -----------------------------------------------------------------------------
// CHapticsConnPlugin::IsOBEXActive()
//
// -----------------------------------------------------------------------------
//    
TBool CHapticsConnPlugin::IsOBEXActive()   
    {
    COMPONENT_TRACE( ( _L( "CHapticsConnPlugin::IsOBEXActive" ) ));
    return ETrue;
    }

// End of file
