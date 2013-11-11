/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Utility class for Voip-states.
*
*/

#include <cphcltemergencycall.h>
#include <mpeengineinfo.h>
#include "cphonestateutilsvoip.h"
#include "cphonelogger.h"
#include "phoneconstants.h"
#include "mphonestatemachine.h"
#include "mphoneviewcommandhandle.h"
#include "phoneui.pan"
#include "tphonecmdparamstring.h"
#include "cphonekeys.h"
#include "tphonecmdparamselector.h"
#include "phonestatedefinitionsvoip.h"

const TInt KServiceId1 = 1;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPhoneStateUtilsVoip::CPhoneStateUtilsVoip
// C++ constructor.
// ---------------------------------------------------------------------------
//
CPhoneStateUtilsVoip::CPhoneStateUtilsVoip( MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle )
        : 
        iStateMachine( aStateMachine ), 
        iViewCommandHandle( aViewCommandHandle )
    {
    
    }


// ---------------------------------------------------------------------------
// CPhoneStateUtilsVoip::ConstructL
// ---------------------------------------------------------------------------
//
void CPhoneStateUtilsVoip::ConstructL()
    {
    iBuffer = HBufC::NewL( KPhoneNumberEntryBufferSize );
    iEmergencyCall = CPhCltEmergencyCall::NewL( NULL );
    }


// ---------------------------------------------------------------------------
// CPhoneStateUtilsVoip::NewL
// ---------------------------------------------------------------------------
//
CPhoneStateUtilsVoip* CPhoneStateUtilsVoip::NewL( 
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle )
    {
    CPhoneStateUtilsVoip* self = CPhoneStateUtilsVoip::NewLC( 
        aStateMachine, aViewCommandHandle );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CPhoneStateUtilsVoip::NewLC
// ---------------------------------------------------------------------------
//
CPhoneStateUtilsVoip* CPhoneStateUtilsVoip::NewLC( 
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle )
    {
    CPhoneStateUtilsVoip* self = new( ELeave ) CPhoneStateUtilsVoip( 
        aStateMachine, aViewCommandHandle );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CPhoneStateUtilsVoip::~CPhoneStateUtilsVoip
// Destructor.
// ---------------------------------------------------------------------------
//
CPhoneStateUtilsVoip::~CPhoneStateUtilsVoip()
    {
    delete iBuffer;
    delete iEmergencyCall;
    }


// ---------------------------------------------------------------------------
// CPhoneStateUtilsVoip::DialVoIPL
// ---------------------------------------------------------------------------
//
void CPhoneStateUtilsVoip::DialVoIPL( const TDesC& aNumber, TUint aServiceId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateUtilsVoip::DialVoIPL" );
    
    __ASSERT_DEBUG( 
        iStateMachine.PhoneEngineInfo(), Panic( EPhoneCtrlInvariant ) );
    
    if ( 0 == aNumber.Compare( KNullDesC ) )
        {
        // Get the number entry contents
        HBufC *phoneNumber = HBufC::NewLC( KPhoneNumberEntryBufferSize );
        TPtr ptr( phoneNumber->Des() );
        TPhoneCmdParamString stringParam;
        stringParam.SetString( &ptr );
        iViewCommandHandle.ExecuteCommand(
            EPhoneViewGetNumberFromEntry,
            &stringParam );
    
        // Set the phone number
        iStateMachine.PhoneEngineInfo()->SetPhoneNumber( ptr );
        
        CleanupStack::PopAndDestroy( phoneNumber );
        }
    else
        {
        iStateMachine.PhoneEngineInfo()->SetPhoneNumber( aNumber );
        }
    
    // Make a call
    iStateMachine.PhoneEngineInfo()->SetServiceIdCommand( aServiceId );
    iStateMachine.PhoneEngineInfo()->SetCallTypeCommand( EPECallTypeVoIP );
    iStateMachine.SendPhoneEngineMessage( MPEPhoneModel::EPEMessageDial );
    }


// ---------------------------------------------------------------------------
// CPhoneStateUtilsVoip::IsVoipPreferredCall
// ---------------------------------------------------------------------------
//
TBool CPhoneStateUtilsVoip::IsVoipPreferredCall( TUint& /*aServiceId*/ )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateUtilsVoip::IsVoipPreferredCall" );
    TBool isVoipPrefered( EFalse );
    return isVoipPrefered;
    }


// ---------------------------------------------------------------------------
// CPhoneStateUtilsVoip::IsVoipNumberL
// ---------------------------------------------------------------------------
//
TBool CPhoneStateUtilsVoip::IsVoipNumber()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateUtilsVoip::IsVoipNumberL" );
    
    TBool isVoipNumber = EFalse;
    
    TPtr ptr( iBuffer->Des() );
    TPhoneCmdParamString stringParam;
    stringParam.SetString( &ptr );
    iViewCommandHandle.ExecuteCommand( 
        EPhoneViewGetNumberFromEntry, &stringParam );
    
    isVoipNumber = !CPhoneKeys::Validate( ptr );
    
    return isVoipNumber;
    }


// ---------------------------------------------------------------------------
// CPhoneStateUtilsVoip::SelectServiceAndDialL
// ---------------------------------------------------------------------------
//
void CPhoneStateUtilsVoip::SelectServiceAndDialL( 
        const TDesC& aNumber,
        TUint aServiceId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateIdleVoIP::SelectServiceAndDialL" );
    
    TPhoneCmdParamSelector params;
    
    // Get the number entry contents
    TPtr ptr( iBuffer->Des() );
    TPhoneCmdParamString stringParam;
    stringParam.SetString( &ptr );
    iViewCommandHandle.ExecuteCommand(
        EPhoneViewGetNumberFromEntry,
        &stringParam );
    
    // Check the string 
    TBool emergencyNumber = ( 0 == aNumber.Compare( KNullDesC ) ?
        IsEmergencyNumber( ptr ) :
        IsEmergencyNumber( aNumber ) );
    
    if ( emergencyNumber )
        {
        if ( 0 == aNumber.Compare( KNullDesC ) )
            {
            // Set the phone number from number entry
            iStateMachine.PhoneEngineInfo()->SetPhoneNumber( ptr );
            }
        else
            {
            iStateMachine.PhoneEngineInfo()->SetPhoneNumber( aNumber );
            }
        
        // Make a emergency call
        iStateMachine.PhoneEngineInfo()->SetServiceIdCommand( KServiceId1 );
        iStateMachine.PhoneEngineInfo()->SetCallTypeCommand( 
            EPECallTypeCSVoice );
        iStateMachine.SendPhoneEngineMessage( MPEPhoneModel::EPEMessageDial );
        }
    else if ( 0 != aServiceId )
        {
        // Service id is known
        DialVoIPL( aNumber, aServiceId );
        }
    else if ( EPhoneViewResponseSuccess == iViewCommandHandle.HandleCommandL( 
            EPhoneViewServiceSelection, &params ) )
        {
        DialVoIPL( aNumber, params.ServiceId() );
        }
    }


// ---------------------------------------------------------------------------
// CPhoneStateUtilsVoip::IsEmergencyNumber
// ---------------------------------------------------------------------------
//
TBool CPhoneStateUtilsVoip::IsEmergencyNumber( const TDesC& aNumber  ) const
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateUtilsVoip::IsEmergencyNumber" );
    
    TBool emergencyNumber( EFalse );
    TInt error = iEmergencyCall->IsEmergencyPhoneNumber( aNumber, emergencyNumber );
    
    return ( KErrNone == error && emergencyNumber );  
    }
