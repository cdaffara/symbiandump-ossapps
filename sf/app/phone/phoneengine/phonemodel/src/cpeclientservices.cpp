/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CPEClientServices class 
*
*/


// INCLUDE FILES
#include "cpeclientcallrequestmonitor.h"
#include "cpeclientcommandhandlermonitor.h"
#include "cpeclientemergencycallmonitor.h"
#include "cpeclientservices.h"
#include "cpedevicemodehandler.h"
#include "cpemessagehandler.h"
#include "mpecallhandling.h"
#include "mpephonemodelinternal.h"
#include <apgtask.h>
#include <cphcltcallnotify.h>
#include <cphcltemergencycall.h>
#include <cphcltussd.h>
#include <mpedatastore.h>
#include <talogger.h>


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEClientServices::CPEClientServices
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEClientServices::CPEClientServices( 
        MPEPhoneModelInternal& aModel,
        CPEMessageHandler& aMessageHandler,
        MPECallHandling& aCallHandling,
        CPEManualCallControlHandler& aManualCallControlHandler
        ) : iModel( aModel ),
            iMessageHandler( aMessageHandler ),
            iCallHandling( aCallHandling ),
            iManualCallControlHandler( aManualCallControlHandler )
    {
    }

// -----------------------------------------------------------------------------
// CPEClientServices::ConstructL
// Symbian OS default constructor may leave.
// -----------------------------------------------------------------------------
//
void CPEClientServices::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEClientServices::ConstructL()" );

    User::LeaveIfError( iPhoneServer.Connect( ) );

    // Need to call CreateAll so that Phone Server creates all managers
    // (f.ex. USSD manager). Otherwise USSD requests/notifications don't
    // get through unless USSD request is sent from Mobile Terminal first.
    TRequestStatus phoneServerStatus;
    iPhoneServer.CreateAll( phoneServerStatus );
    User::WaitForRequest( phoneServerStatus );
    User::LeaveIfError( phoneServerStatus.Int() );
        
    iEmergency = CPhCltEmergencyCall::NewL( NULL );
    
    iCallNotifier = CPhCltCallNotify::NewL();
    User::LeaveIfError( iCallNotifier->Open( iPhoneServer ) );
    
    iClientCallRequestMonitor = CPEClientCallRequestMonitor::NewL( 
        iModel, 
        *iCallNotifier ); 

    iClientEmergencyMonitor = CPEClientEmergencyCallMonitor::NewL(
        iModel, 
        *iCallNotifier ); 

    iClientCommandHandlerMonitor = CPEClientCommandHandlerMonitor::NewL( 
        iCallHandling, 
        iMessageHandler, 
        iModel, 
        iPhoneServer,
        iManualCallControlHandler );
        
    iDeviceModeHandler = CPEDeviceModeHandler::NewL( 
        iCallHandling, 
        iMessageHandler );      
    }

// -----------------------------------------------------------------------------
// CPEClientServices::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEClientServices* CPEClientServices::NewL(
    MPEPhoneModelInternal& aModel,
        CPEMessageHandler& aMessageHandler,
        MPECallHandling& aCallHandling,
        CPEManualCallControlHandler& aManualCallControlHandler )
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEClientServices::NewL()" );
    
    CPEClientServices* self = new ( ELeave ) CPEClientServices( 
        aModel, 
        aMessageHandler, 
        aCallHandling,
        aManualCallControlHandler );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return (self);
    }


// Destructor
CPEClientServices::~CPEClientServices ()
    {
    delete iClientCommandHandlerMonitor;
    delete iClientCallRequestMonitor;
    delete iDeviceModeHandler;

    // Cancel any outstanding USSD requests
    if ( iUssdClient )
        {
        iUssdClient->SendUssdCancel();
        }
    delete iUssdClient;
    delete iClientEmergencyMonitor;
    iCallNotifier->Close();
    delete iCallNotifier;
    delete iEmergency;
    iPhoneServer.Close();  
    }

// -----------------------------------------------------------------------------
// CPEClientServices::IsEmergencyPhoneNumber
// Check if the given telephone number is an emergency number.
// -----------------------------------------------------------------------------
//
TInt CPEClientServices::IsEmergencyPhoneNumber( 
    TPhCltTelephoneNumber& aNumber, 
    TBool& aIsEmergencyNumber )
    {
    TInt errorCode = iEmergency->IsEmergencyPhoneNumber( aNumber, 
                                                         aIsEmergencyNumber );
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEClientServices::SendUssd
// Process USSD request
// -----------------------------------------------------------------------------
//
TInt CPEClientServices::SendUssd( 
        const TDesC& aString )   // USSD string to be sent.
    {
    TInt errorCode( KErrNone );

    TRAP( errorCode, ( iUssdClient = CPhCltUssd::NewL() ) );
    if( errorCode == KErrNone )
        {
        iString.Set( aString );
        iModel.SendMessage( MEngineMonitor::EPEMessageIssuingUSSDRequest );
        errorCode = iUssdClient->SendUssd( iString );
        iModel.SendMessage( MEngineMonitor::EPEMessageIssuedUSSDRequest );
        TEFLOGSTRING2( KTAINT, 
            "PE CPEClientServices::SendUssd, error code: %d", errorCode );

        delete iUssdClient;
        iUssdClient = NULL;
        }
    else
        {
        TEFLOGSTRING( KTAERROR, 
            "PE CPECLIENTSERVICES::SENDUSSD > CPHCLTUSSD::NEWL FAILED" );
        }

    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEClientServices::StartMonitoring
// Start monitoring
// -----------------------------------------------------------------------------
//
void CPEClientServices::StartMonitoring()
    {
    iClientCallRequestMonitor->StartMonitoring();
    iClientEmergencyMonitor->StartMonitoring();
    iClientCommandHandlerMonitor->Start();
    }

// -----------------------------------------------------------------------------
// CPEClientServices::CallRequestMonitor
// Returns MPEClientCallRequestMonitor object
// -----------------------------------------------------------------------------
//
MPEClientCallRequestMonitor* CPEClientServices::CallRequestMonitor()
    {
    // Both monitors are accessed when in RunL, only in Sat call case call 
    // request monitor's SendRespond method is called later when monitoring 
    // already started.
    if ( !iClientEmergencyMonitor->IsActive() )
        {
        return iClientEmergencyMonitor;
        }
    return iClientCallRequestMonitor;
    }

// -----------------------------------------------------------------------------
// CPEClientServices::CommandHandlerMonitor
// Returns CPEClientCommandHandlerMonitor object
// -----------------------------------------------------------------------------
//
CPEClientCommandHandlerMonitor* CPEClientServices::CommandHandlerMonitor()
    {
    return iClientCommandHandlerMonitor;
    }
    
// End of File
