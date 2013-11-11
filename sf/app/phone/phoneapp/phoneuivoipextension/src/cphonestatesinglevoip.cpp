/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneStateSingleVoIP class.
*
*/


// INCLUDES
#include <StringLoader.h>
#include <avkon.rsg>
#include <mpeengineinfo.h>
#include "cphonestatesinglevoip.h"
#include "phonelogger.h"
#include "cphonecustomizationvoip.h"

#include "phoneappvoipcommands.hrh"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamstring.h"
#include "cphonemainresourceresolver.h"
#include "phonerssvoip.h"

#include "tphonecmdparamselector.h"
#include "mphonestatemachine.h"
#include "tphonecmdparamcustomdialer.h"
#include "cphonetransferdialercontroller.h"
#include "tphonecmdparamquery.h"
#include "tphonecmdparamglobalnote.h"
#include "cphonestateutilsvoip.h"
#include "cphonestatemachinevoip.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneStateSingleVoIP::CPhoneStateSingleVoIP( 
    MPhoneStateMachine& aStateMachine, 
    MPhoneViewCommandHandle& aViewCommandHandle,
    MPhoneCustomization& aCustomization ) : 
    CPhoneSingleCall( &aStateMachine, &aViewCommandHandle, &aCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneStateSingleVoIP::~CPhoneStateSingleVoIP()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------

CPhoneStateSingleVoIP::~CPhoneStateSingleVoIP()
    { 
    }

// -----------------------------------------------------------
// CPhoneStateSingleVoIP::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneStateSingleVoIP::ConstructL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateSingleVoIP::ConstructL()" )
    CPhoneSingleCall::ConstructL(); 
    }

// -----------------------------------------------------------
// CPhoneStateSingleVoIP::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneStateSingleVoIP* CPhoneStateSingleVoIP::NewL( 
    MPhoneStateMachine& aStateMachine, 
    MPhoneViewCommandHandle& aViewCommandHandle,
    MPhoneCustomization& aCustomization )
    {
    CPhoneStateSingleVoIP* self = new (ELeave) 
        CPhoneStateSingleVoIP( aStateMachine, aViewCommandHandle, aCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneStateSingleVoIP::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneStateSingleVoIP::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateSingleVoIP::HandlePhoneEngineMessageL() ")
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageUnattendedTransferRequest:
            LaunchUnattendedTransferAcceptanceQueryL();
            break;
        default:
            CPhoneSingleCall::HandlePhoneEngineMessageL( 
                aMessage, aCallId );
            break;
        }
    }
 
// -----------------------------------------------------------
// CPhoneStateSingleVoIP::HandleCommandL
// -----------------------------------------------------------
//
TBool CPhoneStateSingleVoIP::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateSingleVoIP::HandleCommandL()" )

    TBool commandStatus( ETrue );

    switch( aCommand )
        {
        case EPhoneNumberAcqCmdInternetCall:
            StateUtils().SelectServiceAndDialL();
            break;
        
        case EPhoneNumberAcqCmdSendCommand:
            StartCallingL();
            break;
         
        case EPhoneCmdAcceptUnattendedTransfer:
            HandleUnattendedTransferRequestResponseL( ETrue );
            break;
        
        case EPhoneCmdRejectUnattendedTransfer:
            HandleUnattendedTransferRequestResponseL( EFalse );
            break;
        default:
            commandStatus = CPhoneSingleCall::HandleCommandL( aCommand );
            break;
        }

    return commandStatus;
    } 
  
// -----------------------------------------------------------
// CPhoneStateIdleVoIP::HandleKeyMessageL
// -----------------------------------------------------------
//
void CPhoneStateSingleVoIP::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aKeyCode  )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateSingleVoIP::HandleKeyMessageL()" )
    switch ( aKeyCode )
        {
        // send-key
        case EKeyYes:
            {
            TPhoneCmdParamInteger numberEntryCountParam;
            iViewCommandHandle->ExecuteCommandL( EPhoneViewGetNumberEntryCount,
            &numberEntryCountParam );
            TInt neLength( numberEntryCountParam.Integer() );
                 
            if( iNumberEntryManager->IsNumberEntryVisibleL() && 
                    neLength )
                {
                if ( iNumberEntryManager->IsCustomizedDialerVisibleL()  )
                    {
                    return;
                    }
                    
                StartCallingL();
                }
            else
                {
                //Calls ToggleHoldL, if number entry isn't used.
                CPhoneSingleCall::HandleKeyMessageL( aMessage, aKeyCode );
                }
            }
            break;
            
        default:
            CPhoneSingleCall::HandleKeyMessageL( aMessage, aKeyCode );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneStateSingleVoIP::StartCallingL
// -----------------------------------------------------------
//
void CPhoneStateSingleVoIP::StartCallingL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateSingleVoIP::StartCallingL()" )
    
    TUint serviceId( 0 );
    CPhoneStateUtilsVoip& utililty = StateUtils();
    if ( utililty.IsVoipPreferredCall( serviceId ) )
        {
        utililty.SelectServiceAndDialL( KNullDesC, serviceId );         
        }
    else
        {
        if ( utililty.IsVoipNumber() )
            {
            utililty.SelectServiceAndDialL();
            }
        else
            {
            if ( iNumberEntryManager->IsNumberEntryUsedL() )
                {
                // handle send key short press
                HBufC* phoneNumber = iNumberEntryManager->PhoneNumberFromEntryLC();
                
                if ( !IsSimOk() )
                    {
                    iStateMachine->PhoneEngineInfo()->SetPhoneNumber( 
                            *phoneNumber ) ;
                    iStateMachine->SendPhoneEngineMessage( 
                            MPEPhoneModel::EPEMessageCheckEmergencyNumber );
                    CleanupStack::PopAndDestroy( phoneNumber );
                    return;
                    }
                
                // call the number
                iStateMachine->PhoneEngineInfo()->SetPhoneNumber( 
                        *phoneNumber );
                DialVoiceCallL();

                CleanupStack::PopAndDestroy( phoneNumber );
                }
            }
        }
    }

// -----------------------------------------------------------
// CPhoneStateSingleVoIP::LaunchUnattendedTransferAcceptanceQueryL
// Dialog is constructed based on S60 In-Call Operations UI Specification, 
// Section 2.26.
// -----------------------------------------------------------
//
void CPhoneStateSingleVoIP::LaunchUnattendedTransferAcceptanceQueryL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateSingleVoIP::LaunchUnattendedTransferAcceptanceQueryL() ")
    
    TPhoneCmdParamQuery queryParam;
    queryParam.SetQueryType( EPhoneGlobalMsgQuery );
    queryParam.SetCommandParamId( 
        TPhoneCommandParam::EPhoneParamIdUnattendedTransferAcceptanceQuery );
    
    HBufC* msgHeader = 
        StringLoader::LoadLC( CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( 
                EPhoneVoIPUnattendedTransferAcceptanceQueryHeader ) );
    queryParam.SetQueryPrompt( *msgHeader );
    
    RBuf queryText;
    CleanupClosePushL( queryText );
    ResolveUnattendedTransferAcceptanceQueryDialogTextL( queryText );
    queryParam.SetDataText( &queryText );
    
    queryParam.SetDefaultCba( R_AVKON_SOFTKEYS_ACCEPT_REJECT );
    
    const TInt KAcceptTransferQueryTimeoutInSec = 7;
    queryParam.SetTimeOut( KAcceptTransferQueryTimeoutInSec );
    
    queryParam.SetTone( EAvkonSIDDefaultSound );
    
    // configure custom command mappings for user responses
    queryParam.SetCbaCommandMapping( 
        EAknSoftkeyYes, EPhoneCmdAcceptUnattendedTransfer );
    queryParam.SetCbaCommandMapping( 
        EAknSoftkeyOk, EPhoneCmdAcceptUnattendedTransfer );
    queryParam.SetCbaCommandMapping( 
        EAknSoftkeyNo, EPhoneCmdRejectUnattendedTransfer );
    queryParam.SetCbaCommandMapping( 
        EAknSoftkeyCancel, EPhoneCmdRejectUnattendedTransfer );
    queryParam.SetCustomCommandForTimeOut( EPhoneCmdRejectUnattendedTransfer );
    
    iViewCommandHandle->ExecuteCommandL( EPhoneViewShowQuery, &queryParam );
    
    CleanupStack::PopAndDestroy( &queryText );
    CleanupStack::PopAndDestroy( msgHeader );
    }


// -----------------------------------------------------------
// CPhoneStateSingleVoIP::ResolveUnattendedTransferAcceptanceQueryDialogTextL
// -----------------------------------------------------------
//
void CPhoneStateSingleVoIP::ResolveUnattendedTransferAcceptanceQueryDialogTextL( 
        RBuf& aQueryText ) const
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
    "CPhoneStateSingleVoIP::ResolveUnattendedTransferAcceptanceQueryDialogTextL")
    
    aQueryText.Close();
    
    MPEEngineInfo* info = iStateMachine->PhoneEngineInfo();
    const TPEPhoneNumber& transferorAddr = info->RemotePhoneNumber( CallId() );
    const TPEPhoneNumber& transferTarget = 
        info->UnattendedTransferTarget( CallId() );
    
    if ( KNullDesC() == transferorAddr )
        {
        // transfer initiator is anonymous
        aQueryText.Assign( StringLoader::LoadL( 
            CPhoneMainResourceResolver::Instance()->ResolveResourceID( 
                EPhoneVoIPUnattendedTransferAcceptanceQueryAnonymous ), 
            transferTarget ) );
        }
    else
        {
        const TInt KGranularity = 2;
        CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( KGranularity );
        CleanupStack::PushL( array );
        array->AppendL( transferorAddr );
        array->AppendL( transferTarget );
        aQueryText.Assign( StringLoader::LoadL( 
            CPhoneMainResourceResolver::Instance()->ResolveResourceID( 
                EPhoneVoIPUnattendedTransferAcceptanceQueryIdentified ), 
            *array ) );
        CleanupStack::PopAndDestroy( array );
        }
    }


// -----------------------------------------------------------
// CPhoneStateSingleVoIP::HandleUnattendedTransferRequestResponseL
// -----------------------------------------------------------
//
void CPhoneStateSingleVoIP::HandleUnattendedTransferRequestResponseL( 
        TBool aRequestAccepted )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateSingleVoIP::HandleUnattendedTransferRequestResponseL" )
    
    iStateMachine->SetCallId( CallId() );
    
    if ( aRequestAccepted )
        {        
        iStateMachine->SendPhoneEngineMessage( 
            MPEPhoneModel::EPEMessageAcceptUnattendedTransfer );
        MPEEngineInfo* info = iStateMachine->PhoneEngineInfo();
        const TPEPhoneNumber& transferTarget = 
            info->UnattendedTransferTarget( CallId() );
 
        TUint32 serviceId = iStateMachine->PhoneEngineInfo()->
            ServiceId( CallId() );
        StateUtils().SelectServiceAndDialL( transferTarget, serviceId );
        }
    else
        {
        iStateMachine->SendPhoneEngineMessage( 
            MPEPhoneModel::EPEMessageRejectUnattendedTransfer );
        }
    }


// -----------------------------------------------------------
// CPhoneStateSingleVoIP::StateUtils
// -----------------------------------------------------------
//
CPhoneStateUtilsVoip& CPhoneStateSingleVoIP::StateUtils()
    {
    return static_cast<CPhoneStateMachineVoIP&>( 
        *iStateMachine ).StateUtils();
    }

// End of File
