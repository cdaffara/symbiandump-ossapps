/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  implementation of CSPSsSettingsHandler class.
*
*/


// INCLUDES
#include "cspsssettingshandler.h"
#include "csplogger.h"
#include "csppanic.pan"

#include <etelmm.h>
#include <mccpssobserver.h>


// -----------------------------------------------------------------------------
// CSPSsSettingsHandler::CSPSsSettingsHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSPSsSettingsHandler::CSPSsSettingsHandler( const MCCPSsObserver& aObs ) : 
        iObserver( const_cast<MCCPSsObserver&>(aObs) )
    {
    CSPLOGSTRING( CSPOBJECT, "CSPSsSettingsHandler::CSPSsSettingsHandler" );
    } 

// -----------------------------------------------------------------------------
// CSPSsSettingsHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSPSsSettingsHandler::ConstructL()
    {
    CSPLOGSTRING( CSPOBJECT, "CSPSsSettingsHandler::ConstructL <" );
    User::LeaveIfError( iSSSettings.Open() );
    CSPLOGSTRING( CSPOBJECT, "CSPSsSettingsHandler::ConstructL Reg " );
    TInt errorCode = iSSSettings.Register( ESSSettingsAls, *this );
    CSPLOGSTRING2( CSPINT, 
            "CSPSsSettingsHandler::ConstructL > iSSSettings.Register, errorCode: %d",
            errorCode );
    if ( errorCode == KErrNotSupported )
        {
        CSPLOGSTRING( CSPINT, "CSPSsSettingsHandler::ConstructL, ALS not supported" );
        }
    else if ( errorCode == KErrNotReady )
        {
        // Try again after idle
        iRegisterAlsNofify = CIdle::NewL( CActive::EPriorityHigh );
        RegisterAfterIdle();
        }    
      
    RMobilePhone::TMobilePhoneALSLine alsLine;
    CSPLOGSTRING( CSPOBJECT, "CSPSsSettingsHandler::ConstructL GetALS" );
    GetALSLineL( alsLine );
    CSPLOGSTRING( CSPOBJECT, "CSPSsSettingsHandler::ConstructL Notify " );
    iObserver.NotifyCurrentActiveALSLine( (TInt) alsLine );
       
    CSPLOGSTRING( CSPOBJECT, "CSPSsSettingsHandler::ConstructL >" );
    } 

// -----------------------------------------------------------------------------
// CSPSsSettingsHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSPSsSettingsHandler* CSPSsSettingsHandler::NewL( const MCCPSsObserver& aObs )
    {
    CSPSsSettingsHandler* self = new ( ELeave ) CSPSsSettingsHandler( aObs );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    CSPLOGSTRING( CSPOBJECT, "CSPSsSettingsHandler::NewL" );
    return self;
    } 

// Destructor
CSPSsSettingsHandler::~CSPSsSettingsHandler()
    {
    CSPLOGSTRING( CSPOBJECT, "CSPSsSettingsHandler::~CSPSsSettingsHandler <" );
    iSSSettings.Close();
    CSPLOGSTRING( CSPOBJECT, "CSPSsSettingsHandler::~CSPSsSettingsHandler >" );
    if ( iRegisterAlsNofify )
        {
        delete iRegisterAlsNofify;
        }
    }

// -----------------------------------------------------------------------------
// CSPSsSettingsHandler::GetALSLineL
// Sends synchronous request to get the ALS line and returns handle to access 
// the status.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSPSsSettingsHandler::GetALSLineL(
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
    }

// -----------------------------------------------------------------------------
// CSPSsSettingsHandler::SetALSLineL
// Sends synchronous request to set the ALS line and returns handle to access 
// the status.
// -----------------------------------------------------------------------------
//
void CSPSsSettingsHandler::SetALSLineL(
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
            CSPLOGSTRING( CSPINT,
                "CSPSsSettingsHandler::SetALSLineL: Not valid value" );
            __ASSERT_DEBUG( EFalse, Panic( ECSPPanicInvalidRequest ) );
            break;
        }
    User::LeaveIfError( iSSSettings.Set( ESSSettingsAls, alsValue ) );
    } 

// -----------------------------------------------------------------------------
// CSPSsSettingsHandler::GetClirL
// Sends synchronous request to get the CLIR value and returns handle to access 
// the status.
// -----------------------------------------------------------------------------
//
void CSPSsSettingsHandler::GetClirL(
        RMobileCall::TMobileCallIdRestriction& aClir )
    {
    TInt value;
    User::LeaveIfError( iSSSettings.Get( ESSSettingsClir, value ) );

    TSSSettingsClirValue clir = 
        static_cast<TSSSettingsClirValue> ( value );
    CSPLOGSTRING2( CSPERROR,
        "CSPSsSettingsHandler::GetClirL, clir: %d", clir );
        
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
            CSPLOGSTRING2( CSPERROR,
                "CSPSsSettingsHandler::GetClirL, NOT VALID aClir: %d", aClir );
            __ASSERT_DEBUG( EFalse, Panic( ECSPPanicInvalidRequest ) );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CSPSsSettingsHandler::GetCugL
// Sends synchronous request to get the CUG index and returns handle to access 
// the status.
// -----------------------------------------------------------------------------
//
void CSPSsSettingsHandler::GetCugL( TInt& aCugIndex )
    {
    CSPLOGSTRING2( CSPINT,
        "CSPSsSettingsHandler::GetCugL, aCugIndex: %d", aCugIndex );
    User::LeaveIfError( iSSSettings.Get( ESSSettingsCug, aCugIndex ) );
    }
    
// -----------------------------------------------------------------------------
// CSPSsSettingsHandler::GetDefaultCugL
// Sends synchronous request to get the default CUG index 
// -----------------------------------------------------------------------------
//
void CSPSsSettingsHandler::GetDefaultCugL( TInt& aDefCug )
    {
    User::LeaveIfError( iSSSettings.Get( ESSSettingsDefaultCug, aDefCug ) );
    }

// -----------------------------------------------------------------------------
// CSPSsSettingsHandler::IsValueValidCugIndex
// Sends synchronous request to check the CUG index 
// -----------------------------------------------------------------------------
//
TBool CSPSsSettingsHandler::IsValueValidCugIndex( const TInt& aValue ) const
    {
    return iSSSettings.IsValueValidCugIndex( aValue );
    }

// ---------------------------------------------------------
// CSPSsSettingsHandler::PhoneSettingChanged
// ---------------------------------------------------------
//
void CSPSsSettingsHandler::PhoneSettingChanged( 
        TSSSettingsSetting aSetting, 
        TInt aNewValue )
    {
    CSPLOGSTRING2( CSPINT,
        "CSPSsSettingsHandler::PhoneSettingChanged, aNewValue : %d",
        aNewValue );
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
        
        iObserver.NotifyCurrentActiveALSLine( (TInt) alsLine );
        }
    }

// -----------------------------------------------------------------------------
// CSPSsSettingsHandler::RegisterAfterIdle
// -----------------------------------------------------------------------------
//
void CSPSsSettingsHandler::RegisterAfterIdle()
    {
    CSPLOGSTRING( CSPINT, "CSPSsSettingsHandler::RegisterAfterIdle" );
    if ( !iRegisterAlsNofify->IsActive() )
        {
        CSPLOGSTRING( CSPINT, "CSPSsSettingsHandler::RegisterAfterIdle > iRegisterAlsNofify->Star" );
        iRegisterAlsNofify->Start( 
            TCallBack( CIdleCallBack, this ) );
        }
    }
    
// -----------------------------------------------------------------------------
// CSProvider::CIdleCallBack
// -----------------------------------------------------------------------------
//
TInt CSPSsSettingsHandler::CIdleCallBack( TAny* aAny )
    {
    CSPLOGSTRING( CSPINT, "CSPSsSettingsHandler::CIdleCallBack" );
    static_cast< CSPSsSettingsHandler* >( aAny )->Register();

    return KErrNone;    
    }   

// -----------------------------------------------------------------------------
// CSProvider::Register
// -----------------------------------------------------------------------------
//
void CSPSsSettingsHandler::Register()
    {
    CSPLOGSTRING( CSPINT, "CSPSsSettingsHandler::Register" );
    TInt errorCode( KErrNotReady );
    
    errorCode = iSSSettings.Register( ESSSettingsAls, *this );
    CSPLOGSTRING2( CSPINT, 
        "CSPSsSettingsHandler::Register > iSSSettings.Register, errorCode: %d",
        errorCode );

    if ( errorCode == KErrNotReady )
        {
        // Try again
        RegisterAfterIdle();
        }
    }  

//  End of file
