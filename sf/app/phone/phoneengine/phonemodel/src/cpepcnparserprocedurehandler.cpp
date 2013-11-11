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
* Description:  implementation of CPEPcnParserProcedureHandler class.
*
*/


//  ==================== INCLUDE FILES  ==================== 
#include "cpepcnparserprocedurehandler.h"
#include "mpephonemodelinternal.h"
#include <mpedatastore.h>
#include <secui.h>
#include <secuimanualsecuritysettings.h>
#include <talogger.h>


// ==================== LOCAL FUNCTIONS ====================
// None.

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPEPcnParserProcedureHandler::CPEPcnParserProcedureHandler 
        ( 
        CPEMessageHandler& aOwner,              // The owner of this object.
        MPEPhoneModelInternal& aModel
        ) : iOwner( aOwner ),
            iModel( aModel )
    {
    iSupplementaryServicesCommandInfo.action = EPESSActionUnspecified;
    iSupplementaryServicesCommandInfo.type = EPESSTypeUnspecified;
    iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
    TFLOGSTRING("PhoneEngine: PEPcnParserProcedureHandler::CPEPcnParserProcedureHandler complete");
    }

// Destructor
CPEPcnParserProcedureHandler::~CPEPcnParserProcedureHandler
        (
        // None.
        )
    {
    if( iSecurityModel != NULL )
        {
        iSecurityModel->CancelChangePin();
        iSecurityModel->CancelUnblockPin();
        delete iSecurityModel;
        TSecUi::UnInitializeLib();  
        }
    TFLOGSTRING("PhoneEngine: PEPcnParserProcedureHandler::~CPEPcnParserProcedureHandler complete");
    }

// -----------------------------------------------------------------------------
// CPEPcnParserProcedureHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEPcnParserProcedureHandler* CPEPcnParserProcedureHandler::NewL
        (
        CPEMessageHandler& aOwner,              // The owner of this object.
        MPEPhoneModelInternal& aModel
        )
    {
    CPEPcnParserProcedureHandler* self = new ( ELeave ) CPEPcnParserProcedureHandler( 
        aOwner, 
        aModel );

    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );

    TEFLOGSTRING( KTAOBJECT, "CPEPcnParserProcedureHandler::NewL" );

    return self;
    } //NewL

// -----------------------------------------------------------------------------
// CPEPcnParserProcedureHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEPcnParserProcedureHandler::ConstructL
        (
        //None
        )
    {
    TEFLOGSTRING( KTAOBJECT, "tape: CPEPcnParserProcedureHandler::ConstructL" );
    }

/*
-----------------------------------------------------------------------------

    Class: CPEPcnParserProcedureHandler

    Method: ProcessSimLock

    Description: Process SIM locking.

    Type: -

    Parameters: TDesC& aPassword, in coming value, accepted values are number strings, 
                Password for change Sim lock status.
                TDesC& aType, in coming value, accepted values are 1-6, 
                tell what Sim lock will be changed.
    
    Return Values: -

    Errors/Exceptions: -

    Status: Approved
    
-----------------------------------------------------------------------------
*/
void CPEPcnParserProcedureHandler::ProcessSimLockL( 
            const TDesC& aPassword, 
            const TDesC& aType )
    {
    if( iSecurityModel == NULL )
        {
        TSecUi::InitializeLibL();
        iSecurityModel = CManualSecuritySettings::NewL();
        TEFLOGSTRING( KTAOBJECT,
            "PE CPEPcnParserProcedureHandler::ProcessSimLockL > CManualSecuritySettings::NewL()" );
        }
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );
    if( iSecurityModel->LockSimL( aPassword, aType ) )
        {
        iSupplementaryServicesCommandInfo.action = EPESSActivation;
        iSupplementaryServicesCommandInfo.type = EPESimLock;
        iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
        iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
        }
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    }

/*
-----------------------------------------------------------------------------

    Class: CPEPcnParserProcedureHandler

    Method: ProcessSimUnlock

    Description: Process SIM locking.

    Type: -

    Parameters: TDesC& aPassword, in coming value, accepted values are number strings, 
                Password for change Sim lock status.
                TDesC& aType, in coming value, accepted values are 1-6, 
                tell what Sim lock will be changed.
    
    Return Values: -

    Errors/Exceptions: -

    Status: Approved
    
-----------------------------------------------------------------------------
*/
void CPEPcnParserProcedureHandler::ProcessSimUnlockL( 
            const TDesC& aPassword, 
            const TDesC& aType )
    { 
    if( iSecurityModel == NULL )
        {
        TSecUi::InitializeLibL();
        iSecurityModel = CManualSecuritySettings::NewL();
        TEFLOGSTRING( KTAOBJECT,
            "PE CPEPcnParserProcedureHandler::ProcessSimUnlockL > CManualSecuritySettings::NewL()" );
        }
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );
    if( iSecurityModel->UnlockSimL( aPassword, aType ) )
        {
        iSupplementaryServicesCommandInfo.action = EPESSDeactivation;
        iSupplementaryServicesCommandInfo.type = EPESimLock;
        iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
        iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
        }
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    }

// End of File

