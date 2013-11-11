/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A plug-in for silencing the rigning tones.
*
*/


// INCLUDE FILES
#include <badesca.h>
#include <cfactionindication.h>

#include "silenceactionplugin.h"
#include "logger.h"
#include "silenceaction.h"

// LOCAL CONSTANTS AND MACROS

// Supported action identifiers.
_LIT( KPhoneId,     "Phone" );

_LIT( KActionIdCommand,     "Command" );

_LIT( KActionCommand,     "Silence" );

// Capabilities
_LIT_SECURITY_POLICY_PASS( KSecurityPolicy );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSilenceActionPlugIn::CSilenceActionPlugIn
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSilenceActionPlugIn::CSilenceActionPlugIn( ) 
                                : CCFActionPlugIn()                                  
    {
    }

// -----------------------------------------------------------------------------
// CSilenceActionPlugIn::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSilenceActionPlugIn* CSilenceActionPlugIn::NewL( )
    {
    CSilenceActionPlugIn* self = CSilenceActionPlugIn::NewLC();
    CleanupStack::Pop();
    return self;
    }
  
// -----------------------------------------------------------------------------
// CSilenceActionPlugIn::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSilenceActionPlugIn* CSilenceActionPlugIn::NewLC( )
    {
    CSilenceActionPlugIn* self = new( ELeave ) CSilenceActionPlugIn();
    CleanupStack::PushL( self );
    return self;
    }
  

// -----------------------------------------------------------------------------
// CSilenceActionPlugIn::~CSilenceActionPlugIn
// Destructor
// -----------------------------------------------------------------------------
//
CSilenceActionPlugIn::~CSilenceActionPlugIn()
    {
    if ( iSilence )
        {
        delete iSilence;
        iSilence = NULL;   
        }
    }

// METHODS

//-----------------------------------------------------------------------------
// CSilenceActionPlugIn::InitializeL
//-----------------------------------------------------------------------------
//
void CSilenceActionPlugIn::InitializeL()
    {
    LOG_1( _L("CSilenceActionPlugIn::InitializeL: START") );
    iSilence = CSilenceAction::NewL( *this );
    LOG_1( _L("CSilenceActionPlugIn::InitializeL: END") );
    }

//-----------------------------------------------------------------------------
// CSilenceActionPlugIn::ExecuteL
// Triggers an action.
//-----------------------------------------------------------------------------
//
CCFActionPlugIn::TExecutionTime CSilenceActionPlugIn::ExecuteL(
    CCFActionIndication* aActionIndication )
    {
    LOG_1( _L("CSilenceActionPlugIn::ExecuteL: START") );
    
    CCFActionPlugIn::TExecutionTime time = CCFActionPlugIn::ENone;
    const RKeyValueArray& parameters = aActionIndication->Parameters();
    
    TInt count = parameters.Count();
    
    for( TInt i = 0; i < count; i++ )
        {
        TPtrC actionId( KNullDesC );
        TPtrC commandId( KNullDesC );
        
        actionId.Set( parameters[i]->Key() );
        commandId.Set( parameters[i]->Value() );

        if( ( actionId.CompareF( KActionIdCommand ) == KErrNone ) && 
            ( commandId.CompareF( KActionCommand ) == KErrNone ) )
            {
            LOG_1( _L("CSilenceActionPlugIn::ExecuteL->ActionMuteRingingTone") );
            iSilence->ActionMuteRingingTone();
            time = CCFActionPlugIn::ESmall;
            }
        else
            {
            LOG_1( _L("CSilenceActionPlugIn::ExecuteL->Do nothing") );
            time = CCFActionPlugIn::ENone;
            }
        }
        
    LOG_1( _L("CSilenceActionPlugIn::ExecuteL: END") );
    return time;
    }

//-----------------------------------------------------------------------------
// CSilenceActionPlugIn::MuteActionCompleted
// Notifies CCFActionPlugIn base class that async request has been completed. 
//-----------------------------------------------------------------------------
//
void CSilenceActionPlugIn::MuteActionCompleted()
    {
    AsyncExecutionCompleted();
    }

//-----------------------------------------------------------------------------
// CSilenceActionPlugIn::GetActionsL
// Gets all the actions the plugin can perform.
//-----------------------------------------------------------------------------
//
void CSilenceActionPlugIn::GetActionsL( CDesCArray& aActionList ) const
    {
    LOG_1( _L("CSilenceActionPlugIn::GetActionsL: START") );
    aActionList.AppendL( KPhoneId );
    LOG_1( _L("CSilenceActionPlugIn::GetActionsL: END") );
    }

//-----------------------------------------------------------------------------
// CSilenceActionPlugIn::SecurityPolicy
// Returns the capabilities required by the plugin.
//-----------------------------------------------------------------------------
//
const TSecurityPolicy& CSilenceActionPlugIn::SecurityPolicy() const
    {
    LOG_1( _L("CSilenceActionPlugIn::SecurityPolicy") );
    return KSecurityPolicy;
    }
    
// End of File
