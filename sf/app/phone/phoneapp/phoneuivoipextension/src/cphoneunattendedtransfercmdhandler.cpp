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
* Description: Implementation of CPhoneUnattendedTransferCmdHandler class.
*
*/

#include <featmgr.h>
#include <mpeengineinfo.h>
#include "cphoneunattendedtransfercmdhandler.h"

#include "cphonelogger.h"
#include "mphonestatemachine.h"
#include "cphonetransferdialercontroller.h"
#include "phonerssvoip.h"
#include "phoneappvoipcommands.hrh"
#include "cphonestate.h"
#include "cphonestateincall.h"
#include "tphonecmdparamcustomdialer.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamquery.h"
#include "tphonecmdparamglobalnote.h"
#include "cphonemainresourceresolver.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPhoneUnattendedTransferCmdHandler::CPhoneUnattendedTransferCmdHandler
// C++ constructor.
// ---------------------------------------------------------------------------
//
CPhoneUnattendedTransferCmdHandler::CPhoneUnattendedTransferCmdHandler( 
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle )
    : iStateMachine( aStateMachine ), iViewCommandHandle( aViewCommandHandle )
    {

    }


// ---------------------------------------------------------------------------
// CPhoneUnattendedTransferCmdHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CPhoneUnattendedTransferCmdHandler::ConstructL()
    {
    iTransferDialerController = CPhoneTransferDialerController::NewL();
    }


// ---------------------------------------------------------------------------
// CPhoneUnattendedTransferCmdHandler::NewL
// ---------------------------------------------------------------------------
//
CPhoneUnattendedTransferCmdHandler* CPhoneUnattendedTransferCmdHandler::NewL(
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle )
    {
    CPhoneUnattendedTransferCmdHandler* self = 
        CPhoneUnattendedTransferCmdHandler::NewLC( 
            aStateMachine, aViewCommandHandle );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CPhoneUnattendedTransferCmdHandler::NewLC
// ---------------------------------------------------------------------------
//
CPhoneUnattendedTransferCmdHandler* CPhoneUnattendedTransferCmdHandler::NewLC(
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle )
    {
    CPhoneUnattendedTransferCmdHandler* self = 
        new( ELeave ) CPhoneUnattendedTransferCmdHandler( 
            aStateMachine, aViewCommandHandle );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CPhoneUnattendedTransferCmdHandler::~CPhoneUnattendedTransferCmdHandler
// Destructor.
// ---------------------------------------------------------------------------
//
CPhoneUnattendedTransferCmdHandler::~CPhoneUnattendedTransferCmdHandler()
    {
    delete iTransferDialerController;
    }


// ---------------------------------------------------------------------------
// CPhoneUnattendedTransferCmdHandler::HandleCommandL
// ---------------------------------------------------------------------------
//
void CPhoneUnattendedTransferCmdHandler::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EPhoneInCallCmdUnattendedTransfer:
            ShowTransferDialerL();               
            break;
        
        case EPhoneCmdTransferDialerOk:
            DoUnattendedTransferL();     
            break;
        
        case EPhoneCmdTransferDialerExit:
            CloseTransferDialerL();
            break;
        
        case EPhoneCmdTransferDialerSearch:
            OpenSingleItemContactFetchL( 
                EPhoneCmdTransferDialerContactFetch ); 
            break;
        
        case EPhoneCmdTransferDialerContactFetch:
            {
            TBuf<KPhoneNumberEntryBufferSize> fetchContent;    
            fetchContent = iViewCommandHandle.FetchContent();
            
            // Set transfer to address to the phoneengine
            iStateMachine.PhoneEngineInfo()->
                SetTransferTargetCommand( fetchContent );
            iStateMachine.SendPhoneEngineMessage( 
                MPEPhoneModel::EPEMessageDoUnattendedTransfer );
            
            ShowWaitingAcceptanceNoteL();
            }
            break;
        
        default:
            __PHONELOG( EOnlyFatal, PhoneUIVoIPExtension, 
                "CPhoneUnattendedTransferCmdHandler::HandleCommandL, DEFAULT" );    
            ASSERT( EFalse );
        }
    }


// ---------------------------------------------------------------------------
// CPhoneUnattendedTransferCmdHandler::ShowTransferDialerL
// ---------------------------------------------------------------------------
//
void CPhoneUnattendedTransferCmdHandler::ShowTransferDialerL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneUnattendedTransferCmdHandler::ShowTransferDialerL" )

    }


// ---------------------------------------------------------------------------
// CPhoneUnattendedTransferCmdHandler::DoUnattendedTransferL
// ---------------------------------------------------------------------------
//
void CPhoneUnattendedTransferCmdHandler::DoUnattendedTransferL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneUnattendedTransferCmdHandler::DoUnattendedTransferL" )
    
    // First get the transfer to address from dialog
    TPhoneCmdParamString transferTarget;
    HBufC* content = HBufC::NewLC( KPEPhoneNumberMaxLength );
    TPtr ptr( content->Des() );
    transferTarget.SetString( &ptr );
    CPhoneState* phoneState = 
        static_cast< CPhoneState* >( iStateMachine.State() );    
    if ( FeatureManager::FeatureSupported( KFeatureIdOnScreenDialer ) && 
         phoneState->IsCustomizedDialerVisibleL() )
        {
        iViewCommandHandle.ExecuteCommand(
            EPhoneViewGetNumberFromEntry,
            &transferTarget );  
        }
    
    // Set transfer to address to the phoneengine
    iStateMachine.PhoneEngineInfo()->SetTransferTargetCommand( ptr );
    iStateMachine.SendPhoneEngineMessage( 
        MPEPhoneModel::EPEMessageDoUnattendedTransfer );
    
    ShowWaitingAcceptanceNoteL();
    CleanupStack::PopAndDestroy( content );
    }


// ---------------------------------------------------------------------------
// CPhoneUnattendedTransferCmdHandler::CloseTransferDialerL
// ---------------------------------------------------------------------------
//
void CPhoneUnattendedTransferCmdHandler::CloseTransferDialerL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneUnattendedTransferCmdHandler::CloseTransferDialerL" )
    }


// ---------------------------------------------------------------------------
// CPhoneUnattendedTransferCmdHandler::OpenSingleItemContactFetchL
// ---------------------------------------------------------------------------
//
void CPhoneUnattendedTransferCmdHandler::OpenSingleItemContactFetchL( 
        TInt aFetchType )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
       "CPhoneUnattendedTransferCmdHandler::OpenSingleItemContactFetchL" )
    
    TPhoneCmdParamInteger integerParam;
    integerParam.SetInteger( aFetchType );
    
    /*iViewCommandHandle.HandleCommandL( 
        EPhoneViewOpenSingleItemFetchDialog, 
        &integerParam );*/
    }

// ---------------------------------------------------------------------------
// CPhoneUnattendedTransferCmdHandler::ShowWaitingAcceptanceNoteL
// ---------------------------------------------------------------------------
//
void CPhoneUnattendedTransferCmdHandler::ShowWaitingAcceptanceNoteL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneUnattendedTransferCmdHandler::ShowWaitingAcceptanceNoteL" )
    //TODO: Show "Waiting acceptance..." note
//    TPhoneCmdParamGlobalNote globalNoteParam;
//    globalNoteParam.SetType( EAknGlobalWaitNote );
//    globalNoteParam.SetTone( EAvkonSIDNoSound );
//    globalNoteParam.SetSoftkeys( R_AVKON_SOFTKEYS_EMPTY );
//    globalNoteParam.SetTextResourceId( CPhoneMainResourceResolver::Instance()->
//        ResolveResourceID( EPhoneVoIPWaitingAcceptanceNote ) );
//    iViewCommandHandle.ExecuteCommandL( 
//        EPhoneViewShowGlobalNote, &globalNoteParam );
    }
