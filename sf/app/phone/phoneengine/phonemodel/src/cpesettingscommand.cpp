/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  implementation of CPESettingsCommand class.
*
*/


// INCLUDES
#include "cpesettingscommand.h"
#include "mpephonemodelinternal.h"
#include <mpedatastore.h>
#include <talogger.h>


// LOCAL CONSTANTS
// None.

// ==================== LOCAL FUNCTIONS =====================================
//None.

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPESettingsCommand::CPESettingsCommand
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPESettingsCommand::CPESettingsCommand(
        MPEPhoneModelInternal& aModel ) : iModel( aModel )
    {
    TEFLOGSTRING( KTAOBJECT, "PE: CPESettingsCommand::CPESettingsCommand" );
    } //CPESettingsCommand

// -----------------------------------------------------------------------------
// CPESettingsCommand::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPESettingsCommand::ConstructL()
    {
    User::LeaveIfError( iSSSettings.Open() );
    TInt errorCode = iSSSettings.RegisterAll( *this );
    if ( errorCode == KErrNotSupported )
        {
        errorCode = KErrNone;
        }
    User::LeaveIfError( errorCode );

    TEFLOGSTRING( KTAOBJECT, "PE CPESettingsCommand::ConstructL" );
    } //ConstructLb

// -----------------------------------------------------------------------------
// CPESettingsCommand::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPESettingsCommand* CPESettingsCommand::NewL(
        MPEPhoneModelInternal& aModel )
    {
    CPESettingsCommand* self = new ( ELeave ) CPESettingsCommand( aModel );

    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );

    TEFLOGSTRING( KTAOBJECT, "PE CPESettingsCommand::NewL" );

    return self;

    } //NewL

// Destructor
CPESettingsCommand::~CPESettingsCommand()
    {
    iSSSettings.Close();
    TEFLOGSTRING( KTAOBJECT, "PE CPESettingsCommand::~CPESettingsCommand" );
    } //~CPESettingsCommand

// -----------------------------------------------------------------------------
// CPESettingsCommand::GetALSLineL
// Sends synchronous request to get the ALS line and returns handle to access 
// the status.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPESettingsCommand::GetALSLineL(
        RMobilePhone::TMobilePhoneALSLine& aALSLine )
    {
    TInt value;
    User::LeaveIfError( iSSSettings.Get( ESSSettingsAls, value ) );

    TSSSettingsAlsValue alsValue = 
        static_cast< TSSSettingsAlsValue > ( value );

    switch( alsValue )
        {
        case ESSSettingsAlsNotSupported:
            aALSLine = RMobilePhone::EAlternateLineNotAvailable;
            break;
        case ESSSettingsAlsPrimary: 
            aALSLine = RMobilePhone::EAlternateLinePrimary;       
            break;
        case ESSSettingsAlsAlternate:
            aALSLine = RMobilePhone::EAlternateLineAuxiliary;
            break;
        default:
            break;
        }
    } //GetALSLineL

// -----------------------------------------------------------------------------
// CPESettingsCommand::SetALSLineL
// Sends synchronous request to set the ALS line and returns handle to access 
// the status.
// -----------------------------------------------------------------------------
//
void CPESettingsCommand::SetALSLineL(
        RMobilePhone::TMobilePhoneALSLine& aALSLine )
    {
    TSSSettingsAlsValue alsValue = 
        static_cast<TSSSettingsAlsValue> ( aALSLine );
        
    switch( aALSLine )
        {
        case RMobilePhone::EAlternateLineUnknown:
        case RMobilePhone::EAlternateLineNotAvailable:
            alsValue = ESSSettingsAlsNotSupported;
            break;
        case RMobilePhone::EAlternateLinePrimary: 
            alsValue = ESSSettingsAlsPrimary;       
            break;
        case RMobilePhone::EAlternateLineAuxiliary:
            alsValue = ESSSettingsAlsAlternate;
            break;
        default:
            TEFLOGSTRING( KTAINT,
                "PE CPESettingsCommand::SetALSLineL: Not valid value" );
            __ASSERT_DEBUG( EFalse, Panic( EPEPanicInvalidRequest ) );
            break;
        }
    User::LeaveIfError( iSSSettings.Set( ESSSettingsAls, alsValue ) );
    } 

// -----------------------------------------------------------------------------
// CPESettingsCommand::GetClirL
// Sends synchronous request to get the CLIR value and returns handle to access 
// the status.
// -----------------------------------------------------------------------------
//
void CPESettingsCommand::GetClirL(
        RMobileCall::TMobileCallIdRestriction& aClir )
    {
    TInt value;
    User::LeaveIfError( iSSSettings.Get( ESSSettingsClir, value ) );

    TSSSettingsClirValue clir = 
        static_cast<TSSSettingsClirValue> ( value );
    TEFLOGSTRING2( KTAERROR,
        "PE CPESettingsCommand::GetClirL, clir: %d", clir );
        
    switch( clir )
        {
        case ESSSettingsClirNetworkDefault:
            aClir = RMobileCall::EIdRestrictDefault;
            break;
        case ESSSettingsClirExplicitInvoke: 
            aClir = RMobileCall::EDontSendMyId;
            break;
        case ESSSettingsClirExplicitSuppress:
            aClir = RMobileCall::ESendMyId; 
            break;
        default:
            TEFLOGSTRING2( KTAERROR,
                "PE CPESettingsCommand::GetClirL, NOT VALID aClir: %d", aClir );
            __ASSERT_DEBUG( EFalse, Panic( EPEPanicInvalidRequest ) );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CPESettingsCommand::GetCugL
// Sends synchronous request to get the CUG index and returns handle to access 
// the status.
// -----------------------------------------------------------------------------
//
void CPESettingsCommand::GetCugL(
        TInt aCugIndex )
    {
    TEFLOGSTRING2( KTAINT,
        "PE CPESettingsCommand::GetCugL, aCugIndex: %d", aCugIndex );
    User::LeaveIfError( iSSSettings.Get( ESSSettingsCug, aCugIndex ) );
    }
    
// ---------------------------------------------------------
// CPESettingsCommand::PhoneSettingChanged
// ---------------------------------------------------------
//
void CPESettingsCommand::PhoneSettingChanged( 
        TSSSettingsSetting aSetting, 
        TInt aNewValue )
    {
    if ( aSetting == ESSSettingsAls )
        {
        TSSSettingsAlsValue alsValue = 
            static_cast< TSSSettingsAlsValue > ( aNewValue );
    
        RMobilePhone::TMobilePhoneALSLine 
            alsLine( RMobilePhone::EAlternateLineUnknown );

        switch( alsValue )
            {
            case ESSSettingsAlsNotSupported:
                alsLine = RMobilePhone::EAlternateLineNotAvailable;
                break;
            case ESSSettingsAlsPrimary: 
                alsLine = RMobilePhone::EAlternateLinePrimary;       
                break;
            case ESSSettingsAlsAlternate:
                alsLine = RMobilePhone::EAlternateLineAuxiliary;
                break;
            default:
                break;
            }
        iModel.DataStore()->SetALSLine( CCCECallParameters::TCCELineType( alsLine ) );
        iModel.SendMessage( MEngineMonitor::EPEMessageALSLineChanged );
        }
    } // PhoneSettingChanged

//  End of File
