/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     This class connects telephony server and requests information from it.  
*
*/


   
// INCLUDE FILES
#include <mmtsy_names.h> // KMmTsyModuleName
#include <etel.h> // RTelServer

// USERINCLUDE FILES
#include "MmsConnInitLogging.h"
#include "MmsPhoneClient.H"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// ============================== LOCAL FUNCTIONS ==============================
// ============================== MEMBER FUNCTIONS =============================

    /*****************************************************
    *   Series 60 Customer / ETel
    *   Series 60  ETel API
    *****************************************************/
    /*****************************************************
    *   Series 60 Customer / TSY
    *   Needs customer TSY implementation
    *****************************************************/

// -----------------------------------------------------------------------------
// CMmsPhoneClient
// -----------------------------------------------------------------------------
//
CMmsPhoneClient::CMmsPhoneClient() : 
    CActive( EPriorityStandard ),
    iRequestId( 0 )
    {   
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CMmsPhoneClient::ConstructL()
    {
    // Connecting RTelServer and then RMobilePhone
    User::LeaveIfError( iTelServer.Connect() );
    User::LeaveIfError( iTelServer.LoadPhoneModule( KMmTsyModuleName ) );
    User::LeaveIfError( iMobilePhone.Open( iTelServer, KMmTsyPhoneName ) );
    #ifdef _MMSCONNINIT_LOGGING_
    TMmsConnInitLogger::Log( _L("CMmsPhoneClient::ConstructL OK") );
    #endif
    }

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CMmsPhoneClient* CMmsPhoneClient::NewL()
    {
    CMmsPhoneClient* self = new( ELeave ) CMmsPhoneClient;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
// -----------------------------------------------------------------------------
// ~CMmsPhoneClient
// -----------------------------------------------------------------------------
//
CMmsPhoneClient::~CMmsPhoneClient()
    {
    Cancel();
    
    iMobilePhone.Close();
    iTelServer.UnloadPhoneModule( KMmTsyModuleName );
    iTelServer.Close();
    }

// -----------------------------------------------------------------------------
// Roaming
// -----------------------------------------------------------------------------
//
EXPORT_C void CMmsPhoneClient::Roaming( TRequestStatus& aStatus )
    {
    iClientStatus = &aStatus;
    aStatus = KRequestPending;
    iMobilePhone.GetNetworkRegistrationStatus( iStatus, iRegistrationStatus );
    iRequestId = EMobilePhoneGetNetworkRegistrationStatus;
    SetActive();
    }

// -----------------------------------------------------------------------------
// NetworkMode
// -----------------------------------------------------------------------------
//
EXPORT_C RMobilePhone::TMobilePhoneNetworkMode CMmsPhoneClient::NetworkMode()
    {
    TInt error = KErrNone;
    RMobilePhone::TMobilePhoneNetworkMode networkMode = RMobilePhone::ENetworkModeGsm;
    error = iMobilePhone.GetCurrentMode( networkMode );
    if( error != KErrNone )
        {
#ifdef _MMSCONNINIT_LOGGING_
        TMmsConnInitLogger::Log( _L("- ERROR: iMobilePhone.GetCurrentMode: %d"), error );
#endif
        return RMobilePhone::ENetworkModeUnknown;
        }
    return networkMode;
    }

// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
//
void CMmsPhoneClient::RunL()
    {
    
    // Error case
    // "not supported" case must be handled separately for each request.
    // There may be a default value that must be handled in these cases
    
    if( iStatus != KErrNone && iStatus != KErrNotSupported )
        {
#ifdef _MMSCONNINIT_LOGGING_
        TMmsConnInitLogger::Log( _L("- ERROR: CMmsPhoneClient::RunL completed with %d"), iStatus.Int() );
#endif
        User::RequestComplete( iClientStatus, iStatus.Int() );
        // Reset the request Id
        iRequestId = 0;
        return;
        }

    //
    // Currently only one asynch call (roaming check):
    //
    if( iRequestId == EMobilePhoneGetNetworkRegistrationStatus )
    // Summary:
    // - if query not supported, we say "home"
    // - if registered home, we say "home"
    // - otherwise we say "roaming" (includes all non-registered and searching states)
        {
        if ( iStatus == KErrNotSupported )
            {
            // If network registration status query is not supported,
            // we say "home network", return 0.
            User::RequestComplete( iClientStatus, 0 );
            }
        else
            {
            switch( iRegistrationStatus )
                {
                case RMobilePhone::ERegisteredOnHomeNetwork:
#ifdef _MMSCONNINIT_LOGGING_
                    TMmsConnInitLogger::Log( _L("- In Home Network") );
#endif
                    // in Home Network, returning 0
                    User::RequestComplete( iClientStatus, 0 );
                    break;
                default:
#ifdef _MMSCONNINIT_LOGGING_
                    TMmsConnInitLogger::Log( _L("- Roaming, in state %d"), iRegistrationStatus );
#endif
                    // Default to roaming -> returning positive value (i.e. true)
                    // If we are not sure we are at home, we say "roaming"
                    // This includes all possible "searching" and "not registered" states
                    User::RequestComplete( iClientStatus, 1 );
                    break;
                }
            }
        } // requestId
            
    // Reset the request Id
    iRequestId = 0;
    } 

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CMmsPhoneClient::DoCancel()
    {
    if( iRequestId )
        {
        iMobilePhone.CancelAsyncRequest( iRequestId );
        iRequestId = 0;
        }
    User::RequestComplete( iClientStatus, KErrCancel );
    }

// -----------------------------------------------------------------------------

// =========================== OTHER EXPORTED FUNCTIONS ========================

//  End of File  
