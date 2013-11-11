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
* Description:  Acessory and Bluetooth handler.
*
*/


// INCLUDE FILES
#include    <featmgr.h>
#include    <StringLoader.h>
#include    <btengdomainpskeys.h>
#include    <btengdomaincrkeys.h>
#include    <mpeengineinfo.h>

#include    "cphoneaccessorybthandler.h"
#include    "cphonecenrepproxy.h"
#include    "cphonemainresourceresolver.h"
#include    "mphoneneclearedhandler.h"
#include    "mphonestatemachine.h"
#include    "mphoneviewcommandhandle.h"
#include    "phoneconstants.h"
#include    "phonelogger.h"
#include    "phoneui.pan"
#include    "phonerssbase.h"
#include    "tphonecmdparamnote.h"

// CONSTANTS

const TInt KPhoneBtAddressTextLength = 64;

// ============================ MEMBER FUNCTIONS ===============================

CPhoneAccessoryBTHandler::CPhoneAccessoryBTHandler( 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneStateMachine* aStateMachine ) :
    iViewCommandHandle( aViewCommandHandle ),
    iStateMachine( aStateMachine )
    {
    }

// -----------------------------------------------------------
// CPhoneAccessoryBTHandler::NewLC()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneAccessoryBTHandler* CPhoneAccessoryBTHandler::NewLC( 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneStateMachine* aStateMachine )
    {
    CPhoneAccessoryBTHandler* self = new (ELeave) CPhoneAccessoryBTHandler( 
        aViewCommandHandle, aStateMachine );
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneAccessoryBTHandler::~CPhoneAccessoryBTHandler
// -----------------------------------------------------------------------------
//
CPhoneAccessoryBTHandler::~CPhoneAccessoryBTHandler()
    {
    }

// ---------------------------------------------------------
// CPhoneAccessoryBTHandler::SetBTHandsfreeModeL
// ---------------------------------------------------------
//
TBool CPhoneAccessoryBTHandler::SetBTHandsfreeModeL( TBool aHandsfreeMode )
    {
    return DoSetHandsfreeModeL( aHandsfreeMode, aHandsfreeMode ? EPEBTAudioAccessory : EPEHandset );
    }

// ---------------------------------------------------------
// CPhoneAccessoryBTHandler::SetHandsfreeModeL
// ---------------------------------------------------------
//
TBool CPhoneAccessoryBTHandler::SetHandsfreeModeL( TBool aHandsfreeMode )
    {
    return DoSetHandsfreeModeL( aHandsfreeMode, aHandsfreeMode ? EPELoudspeaker : EPEHandset );
    }

// ---------------------------------------------------------
// CPhoneAccessoryBTHandler::ShowBTAddressL
// ---------------------------------------------------------
//
void CPhoneAccessoryBTHandler::ShowBTAddressL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneAccessoryBTHandler::ShowBTAddressL( ) ");
    iViewCommandHandle->ExecuteCommandL( EPhoneViewClearNumberEntryContent );
    
    TBuf<KPhoneBtAddressTextLength> addressBuffer;
    CPhoneCenRepProxy::Instance()->GetString( 
        KCRUidBluetoothLocalDeviceAddress,
        KBTLocalDeviceAddress,
        addressBuffer );
    // BT address was empty. BT is not turned on.
    if ( addressBuffer.Length() == 0 )
        {
        __PHONELOG( EBasic, 
                EPhoneControl, 
                "CPhoneAccessoryBTHandler::ShowBTAddressL.NoAddress" );
        }
    // So we got the address. Now we need the localised text:
    HBufC* buf = StringLoader::LoadLC( 
        CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( EPhonePhoneBtDevAddress ) , 
        addressBuffer );
    TPhoneCmdParamNote noteParam;
    noteParam.SetType( EPhoneNoteCustom );
    noteParam.SetResourceId( CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( EPhoneInformationWaitNote ) );
    noteParam.SetText( *buf );
    
    iViewCommandHandle->ExecuteCommandL( EPhoneViewShowNote, &noteParam );
    CleanupStack::PopAndDestroy( buf );
    }    

// -----------------------------------------------------------
// CPhoneAccessoryBTHandler::ShowBTLoopbackL
// Handling for message EPEMessageShowBTLoopback.
// Shows BT loopback status note.
// -----------------------------------------------------------
//
void CPhoneAccessoryBTHandler::ShowBTLoopbackL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneAccessoryBTHandler::ShowBTLoopbackL( ) ");
    iViewCommandHandle->ExecuteCommandL( EPhoneViewClearNumberEntryContent );
    // Get current status of BT loopback
    TInt btLoopback( EBTDutOff );
    TInt err = RProperty::Get( 
            KPSUidBluetoothDutMode, 
            KBTDutEnabled, 
            btLoopback );
    // If loopback was disabled enable it. According to current knowledge
    // disabling BT test loop is not possible. If error getting the value
    // assume that it is disabled.
    if ( btLoopback == EBTDutOff || err != KErrNone )
        {
        // Safe to overwrite the previous error code since it is nolonger needed
        err = RProperty::Set( KPSUidBluetoothDutMode,
                  KBTDutEnabled,
                  EBTDutOn );
        if( !err )
            {
            btLoopback = EBTDutOn;
            }
        }
    // Get localised text according to setting status
    HBufC* btLoopbackString( NULL );
    btLoopbackString = StringLoader::LoadLC( 
        CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( btLoopback == EBTDutOn ? 
        EPhoneBtLoopbackEnabled : EPhoneBtLoopbackDisabled ) );
    TPhoneCmdParamNote noteParam;
    noteParam.SetType( EPhoneNoteCustom );
    noteParam.SetResourceId( CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( EPhoneInformationWaitNote ) );
    noteParam.SetText( *btLoopbackString );
    noteParam.SetTone( CAknNoteDialog::EConfirmationTone );

    iViewCommandHandle->ExecuteCommandL( EPhoneViewShowNote, &noteParam );
    CleanupStack::PopAndDestroy( btLoopbackString );
    }

// -----------------------------------------------------------
// CPhoneAccessoryBTHandler::ShowBTActivatedL
// Shows BT activated note.
// -----------------------------------------------------------
//
void CPhoneAccessoryBTHandler::ShowBTActivatedL()
    {
    // Get localised text 
    HBufC* buf = StringLoader::LoadLC( 
            CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( EPhoneInfoBTAccActivated ) );
    TPhoneCmdParamNote noteParam;
    noteParam.SetType( EPhoneNoteConfirmation );
    noteParam.SetText( *buf );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewShowNote, &noteParam );
    CleanupStack::PopAndDestroy( buf );
    }

// ---------------------------------------------------------
// CPhoneAccessoryBTHandler::DoSetHandsfreeModeL
// ---------------------------------------------------------
//
TBool CPhoneAccessoryBTHandler::DoSetHandsfreeModeL( 
        TBool aHandsfreeMode, TPEAudioOutput aAudioOutput )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneAccessoryBTHandler::DoSetHandsfreeModeL( ) ");
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(), Panic( EPhoneCtrlInvariant ));
    TBool allowed(EFalse);
    // Only toggle the handsfree mode if there is no wired accessory connected
    if( iStateMachine->PhoneEngineInfo()->AudioOutput() != EPEWiredAudioAccessory )
        {
        iStateMachine->PhoneEngineInfo()->
                SetAudioOutputCommand( aAudioOutput, aHandsfreeMode );
        iStateMachine->
                SendPhoneEngineMessage( MPEPhoneModel::EPEMessageSetAudioOutput );
        allowed = ETrue; 
        }
    return allowed;
    }

//  End of File  
