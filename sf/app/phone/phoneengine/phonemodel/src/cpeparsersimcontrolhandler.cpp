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
* Description:  implementation of CPEParserSimControlHandler class.
*
*/


//  1.0 INCLUDE FILES
#include "cpeparsersimcontrolhandler.h"
#include "mpephonemodelinternal.h"
#include <mpecontacthandling.h>
#include <mpedatastore.h>
#include <pepanic.pan>
#include <secui.h>
#include <secuimanualsecuritysettings.h>
#include <talogger.h>


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEParserSimControlHandler::CPEParserSimControlHandler( 
        MPEPhoneModelInternal& aModel,        
        MPEContactHandling& aContactHandling
        ) : iModel( aModel ),
            iContactHandling( aContactHandling )
    {
    iSupplementaryServicesCommandInfo.action = EPESSActionUnspecified;
    iSupplementaryServicesCommandInfo.type = EPESSTypeUnspecified;
    iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
    TEFLOGSTRING( KTAOBJECT,
        "PE CPEParserSimControlHandler::CPEParserSimControlHandler: complete");
    }

// Destructor
CPEParserSimControlHandler::~CPEParserSimControlHandler()
    {
    if( iSecurityModel != NULL )
        {
        iSecurityModel->CancelChangePin();
        iSecurityModel->CancelUnblockPin();
        delete iSecurityModel;
        TSecUi::UnInitializeLib();  
        }
    TEFLOGSTRING( KTAOBJECT,
        "PE CPEParserSimControlHandler::~CPEParserSimControlHandler: complete");
    }

// -----------------------------------------------------------------------------
// CPEParserSimControlHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEParserSimControlHandler* CPEParserSimControlHandler::NewL(
        MPEPhoneModelInternal& aModel,      
        MPEContactHandling& aContactHandling )
    {
    CPEParserSimControlHandler* self = new ( ELeave ) CPEParserSimControlHandler( 
        aModel, 
        aContactHandling );

    TEFLOGSTRING( KTAOBJECT,
        "PE CPEParserSimControlHandler::NewL complete" );

    return self;
    } //NewL

// -----------------------------------------------------------------------------
// CPEExternalDataHandler::ProcessChangePin
// Change pin code.
// -----------------------------------------------------------------------------
//
void CPEParserSimControlHandler::ProcessChangePinL( 
        TPinCode aType,         // It is type of pin.
        const TDesC& aOldPin,   // It is old pin.
        const TDesC& aNewPin,   // It is new pin.
        const TDesC& aVerifyNew )  // It is new pin(confirmation).
    {
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSimControlHandler::ProcessChangePinL ");
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );
    CManualSecuritySettings::TPin pin= CManualSecuritySettings::EPin1;

    switch ( aType )
        {
        case EPinCode:
            pin = CManualSecuritySettings::EPin1;
            break;
        case EPin2Code:
            pin = CManualSecuritySettings::EPin2;
            break;
        default:
            // Case not handled, panic.
            User::Panic( KPEPhoneEnginePanic, EPEPanicStupidUser );
            break;
        }
        
    if( iSecurityModel == NULL )
        {
        TSecUi::InitializeLibL();
        iSecurityModel = CManualSecuritySettings::NewL();
        TEFLOGSTRING( KTAOBJECT,
            "PE CPEParserSimControlHandler::ConstructL" );
        }
        
    if( iSecurityModel->ChangePinL( pin, aOldPin, aNewPin, aVerifyNew ) )
        {
        iSupplementaryServicesCommandInfo.action = EPEPasswordOperation;
        iSupplementaryServicesCommandInfo.type = EPEPin;
        iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
        iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
        TEFLOGSTRING( KTAINT,
            "PE CPEParserSimControlHandler::ProcessChangePinL: iSecurityModel->ChangePinL called ");
        }
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    }

// -----------------------------------------------------------------------------
// CPEGsmExternalDataHandler::ProcessUnblockPin
// Unblock pin code.
// -----------------------------------------------------------------------------
//
void CPEParserSimControlHandler::ProcessUnblockPinL( 
        TPinCode aType,         // It is type of pin to unblock.
        const TDesC& aPuk,      // It is unblocking code.
        const TDesC& aNewPin,   // It is new pin.
        const TDesC& aVerifyNew ) // It is new pin.
    {
       
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );
    CManualSecuritySettings::TPin pin= CManualSecuritySettings::EPin1;

    switch ( aType )
        {
        case EPinCode:
            pin = CManualSecuritySettings::EPin1;
            break;
        case EPin2Code:
            pin = CManualSecuritySettings::EPin2;
            break;
        default:
            // Case not handled, panic.
            User::Panic( KPEPhoneEnginePanic, EPEPanicStupidUser );
            break;
        }

    if( iSecurityModel == NULL )
        {
        TSecUi::InitializeLibL();
        iSecurityModel = CManualSecuritySettings::NewL();
        TEFLOGSTRING( KTAOBJECT,
            "PE CPEParserSimControlHandler::ConstructL" );
        }
        
    if( iSecurityModel->UnblockPinL( pin, aPuk, aNewPin, aVerifyNew ) )
        {
        iSupplementaryServicesCommandInfo.action = EPEPasswordOperation;
        iSupplementaryServicesCommandInfo.type = EPEUnblockPin;
        iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
        iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
        TEFLOGSTRING( KTAINT,
            "PE CPEParserSimControlHandler::ProcessUnblockPinL: iSecurityModel->UnblockPinL called ");
        }
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );        
    }

// End of File
