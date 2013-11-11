/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CPEParserEmergencyNumberHandler class methods implementation
*
*/



// INCLUDE FILES
#include "cpeclientcallrequestmonitor.h"
#include "cpemessagehandler.h"
#include "cpeparseremergencynumberhandler.h"
#include <mpecallhandling.h>
#include <mpedatastore.h>
#include <talogger.h>


// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CPEParserEmergencyNumberHandler::CPEParserEmergencyNumberHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEParserEmergencyNumberHandler::CPEParserEmergencyNumberHandler( 
        CPEMessageHandler& aOwner, 
        MPECallHandling& aCallHandling,
        MPEDataStore& aDataStore
        ) : iOwner( aOwner ), 
            iCallHandling( aCallHandling ),
            iDataStore( aDataStore )
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEParserEmergencyNumberHandler::CPEParserEmergencyNumberHandler" )
    }

// -----------------------------------------------------------------------------
// CPEParserEmergencyNumberHandler::ProcessDialToEmergencyNumberL
// Process dial to number.
// -----------------------------------------------------------------------------
//
void CPEParserEmergencyNumberHandler::ProcessDialToEmergencyNumberL( 
        const TDesC& aNumber
        )
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEParserEmergencyNumberHandler::ProcessDialToEmergencyNumberL, number: %S", 
        &aNumber);
    /* 
    * DTMF string is nulled here because if this not done here it is possible that
    * some old DTMF string is send when emergency call connects(for more information
    * about connect handling see CPEMessageHandler class HandleConnectedState method ).
    */
    iDataStore.SetDtmfStringCommand( KNullDesC() );
    
    if ( aNumber.Length() == 0 )
        {
        User::Leave( ECCPErrorInvalidPhoneNumber );
        }

    if ( iDataStore.CallOriginCommand() != EPECallOriginPhone )
        {
        iOwner.SetClientInformation( KPEEmergencyCallId, aNumber );
        }

    iDataStore.SetPhoneNumber( aNumber );
    iOwner.HandleDialEmergencyCall( EFalse );

    iOwner.SetGsmParserErrorCode( KErrNone );
    }
    
//  End of File
