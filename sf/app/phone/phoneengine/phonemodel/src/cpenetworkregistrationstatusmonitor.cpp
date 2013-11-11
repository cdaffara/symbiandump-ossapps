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
* Description:  This module contains the implementation of CPENetworkRegistrationStatusMonitor class 
*
*/


// INCLUDES
#include "cpenetworkregistrationstatusmonitor.h"
#include "cpepubsubmonitor.h"
#include "mpephonemodelinternal.h"
#include <e32property.h>
#include <mpedatastore.h>
#include <networkhandlingdomainpskeys.h>
#include <talogger.h>


// EXTERNAL DATA STRUCTURES
// None.

// EXTERNAL FUNCTION PROTOTYPES  
// None.

// CONSTANTS
// None.

// MACROS
// None.

// LOCAL CONSTANTS AND MACROS
// None.

// MODULE DATA STRUCTURES
// None.

// LOCAL FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
// None. 

// ==================== LOCAL FUNCTIONS ====================
// None.

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPENetworkRegistrationStatusMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CPENetworkRegistrationStatusMonitor* CPENetworkRegistrationStatusMonitor::NewL(
        MPEPhoneModelInternal& aModel
        ) 
    {
    TEFLOGSTRING(KTAOBJECT, "CPENetworkRegistrationStatusMonitor::NewL");
    CPENetworkRegistrationStatusMonitor* self = new (ELeave) CPENetworkRegistrationStatusMonitor( 
        aModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return( self );
    }
                                                                            
// -----------------------------------------------------------------------------
// CPENetworkRegistrationStatusMonitor::CPENetworkRegistrationStatusMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPENetworkRegistrationStatusMonitor::CPENetworkRegistrationStatusMonitor(
        MPEPhoneModelInternal& aModel
        ): CPEPubSubMonitor( aModel )
    {
    }

// -----------------------------------------------------------------------------
// CPENetworkRegistrationStatusMonitor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPENetworkRegistrationStatusMonitor::ConstructL(
        // None
        )
        {
    TEFLOGSTRING(KTAOBJECT, "CPENetworkRegistrationStatusMonitor::ConstructL");
    
    BaseConstructL( KPSUidNetworkInfo, KNWRegistrationStatus, RProperty::EInt );
       
    //  Now retrieve the value
    TInt value;
    TInt error = Get( value );
    
    // if not able to retrieve the value, set it to the default value of ENWStatusRegistrationUnknown
    if ( error != KErrNone )
        {
        value = ENWStatusRegistrationUnknown; 
        }
        
    iModel.DataStore()->SetNetworkRegistrationStatus( 
        static_cast< TNWNetworkRegistrationStatus > ( value ) );
    }

// -----------------------------------------------------------------------------
// CPECenRepMonitor::UpdateL
// -----------------------------------------------------------------------------
//
void CPENetworkRegistrationStatusMonitor::UpdateL(
        // None
        )
    {
    TEFLOGSTRING(KTAINT, "CPENetworkRegistrationStatusMonitor::UpdateL" );
   
    //  Now retrieve the value
    TInt value;
    User::LeaveIfError(Get(value)); 
        
    iModel.DataStore()->SetNetworkRegistrationStatus( 
        static_cast< TNWNetworkRegistrationStatus > ( value ) );
    iModel.SendMessage( MEngineMonitor::EPEMessageNetworkRegistrationStatusChange );
    }
    
// End of file
