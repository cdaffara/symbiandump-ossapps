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
* Description:  Implementation of the CTelDMCommandHandler class.
*
*/



// INCLUDE FILES

#include    <mphonedevicemodeobserver.h>
#include    <PSVariables.h>
#include    <ctsydomainpskeys.h>

#include    "cteldmcommandhandler.h"
#include    "cteldmcallstatelistener.h"
#include    "mteldmaccessory.h"
#include    "cteldmdebug.h"
#include     <coreapplicationuisdomainpskeys.h>

// MODULE DATA STRUCTURES
 
// ============================ MEMBER FUNCTIONS ===============================
 
// -----------------------------------------------------------------------------
// CTelDMCommandHandler::NewL
// 
// -----------------------------------------------------------------------------
//  
CTelDMCommandHandler* CTelDMCommandHandler::NewL( 
                                    MPhoneDeviceModeObserver& aCallHandler, 
                                    MTelDMAccessory& aAccessory ) 
    {
    CTelDMCommandHandler* self =
        new ( ELeave ) CTelDMCommandHandler( aCallHandler, 
                                                aAccessory );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CTelDMCommandHandler::CTelDMCommandHandler
// 
// -----------------------------------------------------------------------------
//
CTelDMCommandHandler::CTelDMCommandHandler( 
                                    MPhoneDeviceModeObserver& aCallHandler, 
                                    MTelDMAccessory& aAccessory ):
    iCallHandler( aCallHandler ),
    iAccessory ( aAccessory ),
    iGripOpen ( EFalse ),
    iGripOpenOnRingingState ( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CTelDMCommandHandler::~CTelDMCommandHandler
// 
// -----------------------------------------------------------------------------
//
CTelDMCommandHandler::~CTelDMCommandHandler()
    {
    iKeyLock.Close();
    }

// -----------------------------------------------------------------------------
// CTelDMCommandHandler::ConstructL
// 
// -----------------------------------------------------------------------------
//    
void CTelDMCommandHandler::ConstructL()
    { 
    User::LeaveIfError( iKeyLock.Connect() );                             
    }

// -----------------------------------------------------------------------------
// CTelDMCommandHandler::CallStateChanged()
// 
// -----------------------------------------------------------------------------
//    
void CTelDMCommandHandler::CallStateChangedL( TInt aCallState )
    {
    FLOG( _L( "CTelDMCommandHandler::CallStateChanged<" ) )
    FTRACE( FPrint( _L( "CTelDMCommandHandler::CallStateChangedL.aCallState=%d"), 
                                                                aCallState ))
    iCallState = aCallState;                                                            
    switch( aCallState )
        {  
        // Arriving call                    
        case EPSCTsyCallStateRinging: 
            FLOG( _L( "CTelDMCommandHandler::CallStateChanged.Ringing" ) )
            RProperty::Set( KPSUidCoreApplicationUIs,
                            KCoreAppUIsSoftReject,
                            ECoreAppUIsSoftRejectUninitialized );
            iGripOpenOnRingingState = iGripOpen;
            break;
        // Answered
        case EPSCTsyCallStateConnected:  
            {   
            FLOG( _L( "CTelDMCommandHandler::CallStateChanged.Connected" ) )
            if ( iGripOpenOnRingingState )
                {
                OfferKeyLock(); 
                }
            
            // Clear the flag.
            iGripOpenOnRingingState = EFalse;
            }
            break;
        // Disconnected 
        case EPSCTsyCallStateNone:
            {
            FLOG( _L( "CTelDMCommandHandler::CallStateChanged.None" ) )
            if ( iGripOpenOnRingingState )
                {
                OfferKeyLock(); 
                }           
            }
            break; 
        default:
            break;
        }     
    FLOG( _L( "CTelDMCommandHandler::CallStateChanged>" ) )  
    }
// -----------------------------------------------------------------------------
// CTelDMCommandHandler::HandleEvent
// 
// -----------------------------------------------------------------------------
//    
void CTelDMCommandHandler::HandleCommand( TCommands aCommand )
    {           
    FLOG( _L( "CTelDMCommandHandler::HandleCommand<" ) ) 
    switch( aCommand )
        {                      
        case EGripOpen:
            {
            FLOG( _L( "CTelDMCommandHandler::HandleCommand.Open" ) ) 
            iGripOpen = ETrue;
            if ( !IsSoftRejectOngoing() )
                {
                iCallHandler.Answer();
                }
            }
            break;
        // Do not end calls if accessory is connected. 
        case EGripClose:
            {
            FLOG( _L( "CTelDMCommandHandler::HandleCommand.Close" ) ) 
            iGripOpen = EFalse;
            EndCalls();
            break;
            }
        default:
            break;
        }  
    FLOG( _L( "CTelDMCommandHandler::HandleCommand>" ) )      
    }
    
// -----------------------------------------------------------------------------
// CTelDMCommandHandler::EndCalls
// 
// -----------------------------------------------------------------------------
//    
void CTelDMCommandHandler::EndCalls()

    {
    if ( IsEmergencyCall() )
        {
        // None
        }
    else if ( iAccessory.IsAccessoryAttached()  &&
              !iAccessory.IsAnyActiveAccessory() &&
              iCallState == EPSCTsyCallStateConnected )
        {
        FLOG( _L( "CTelDMCommandHandler::EndCalls#1" ) )
        iCallHandler.EndVoiceCalls();
        }
    else if ( !iAccessory.IsAccessoryAttached() )
        {
        FLOG( _L( "CTelDMCommandHandler::EndCalls#2" ) )
        iCallHandler.EndVoiceCalls();
        }
    }
// -----------------------------------------------------------------------------
// CTelDMCommandHandler::OfferKeyLock
//
// -----------------------------------------------------------------------------
//
void CTelDMCommandHandler::OfferKeyLock()

    {
    //- AudioAccessory attached (BT headset, Wired headset etc.)
    //- Arriving call and grip open.  
    //- Call terminated or answered  
    //-> KeyLock query is shown if not locked       
    if ( iAccessory.IsAccessoryAttached() && 
                                       !iGripOpen && 
                                       !iKeyLock.IsKeyLockEnabled() )
        {
        FLOG( _L( "CTelDMCommandHandler::CallStateChanged -lock?" ) ) 
        //Lock keypad ?
        iKeyLock.OfferKeyLock();
        }  
    }

// ---------------------------------------------------------
// CTelDMCommandHandler::IsEmergencyCall
// ---------------------------------------------------------
//
TBool CTelDMCommandHandler::IsEmergencyCall() const
    {
    FLOG( _L( "CTelDMCommandHandler::IsEmergencyCall" ) ) 
    TBool retVal( EFalse );
    TInt err( KErrNone );
    TInt state( 0 );
    
    err = RProperty::Get(
            KPSUidCtsyEmergencyCallInfo,
            KCTSYEmergencyCallInfo,
            state );
    
    if ( err == KErrNone && state )
        {
        retVal = ETrue;            
        }
    return retVal;
    }

// ---------------------------------------------------------
// CTelDMCommandHandler::IsSoftRejectOngoing
// ---------------------------------------------------------
//
TBool CTelDMCommandHandler::IsSoftRejectOngoing() const
    {
    FLOG( _L( "CTelDMCommandHandler::IsSoftRejectOngoing" ) )
    TBool retVal( EFalse );
    TInt err( KErrNone );
    TInt state( 0 );
        
    err = RProperty::Get(
            KPSUidCoreApplicationUIs,
            KCoreAppUIsSoftReject,
            state );
    
    if ( err == KErrNone && state == ECoreAppUIsSoftReject )
        {
        retVal = ETrue;            
        }
    return retVal;
    }

// End of File
