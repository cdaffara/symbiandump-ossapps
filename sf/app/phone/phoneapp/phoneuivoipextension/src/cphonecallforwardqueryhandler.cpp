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
* Description: Implementation of CPhoneCallForwardQueryHandler class.
*
*/

#include <featmgr.h>
#include <StringLoader.h>
#include <mpeengineinfo.h>

#include "cphonecallforwardqueryhandler.h"
#include "cphonelogger.h"
#include "mphonestatemachine.h"
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
#include "cphonestatemachinevoip.h"
#include "tphonecmdparamstring.h"
#include "phoneviewcommanddefinitions.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::CPhoneCallForwardQueryHandler
// C++ constructor.
// ---------------------------------------------------------------------------
//
CPhoneCallForwardQueryHandler::CPhoneCallForwardQueryHandler( 
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle )
    : iStateMachine( aStateMachine ), 
      iViewCommandHandle( aViewCommandHandle ),
      iMultipleChoicesArray( NULL )
    {

    }


// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CPhoneCallForwardQueryHandler::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::NewL
// ---------------------------------------------------------------------------
//
CPhoneCallForwardQueryHandler* CPhoneCallForwardQueryHandler::NewL(
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle )
    {
    CPhoneCallForwardQueryHandler* self = 
        CPhoneCallForwardQueryHandler::NewLC( 
            aStateMachine, aViewCommandHandle );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::NewLC
// ---------------------------------------------------------------------------
//
CPhoneCallForwardQueryHandler* CPhoneCallForwardQueryHandler::NewLC(
        MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle )
    {
    CPhoneCallForwardQueryHandler* self = 
        new( ELeave ) CPhoneCallForwardQueryHandler( 
            aStateMachine, aViewCommandHandle );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::~CPhoneCallForwardQueryHandler
// Destructor.
// ---------------------------------------------------------------------------
//
CPhoneCallForwardQueryHandler::~CPhoneCallForwardQueryHandler()
    {
    delete iMultipleChoicesArray;
    }

// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::ShowMovedPermanentlyQueryL
// ---------------------------------------------------------------------------
//
void CPhoneCallForwardQueryHandler::ShowMovedPermanentlyQueryL(
        TInt aCallId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneCallForwardQueryHandler::ShowMovedPermanentlyQueryL" )
        
    iCallId = aCallId;
    iAddressArray = iStateMachine.PhoneEngineInfo()->
        ForwardAddressChoices( aCallId );
    
    if ( NULL == iAddressArray )
        {
        User::Leave( KErrNotSupported );
        }
    
    iQueryType = EPhoneMovedPermanently;
    TPhoneCmdParamQuery queryParam;
    queryParam.SetQueryCommandObserver( *this );
    queryParam.SetTone( EAvkonSIDInformationTone );
    queryParam.SetQueryType( EPhoneGlobalQueryDialog );
  
    HBufC* queryPrompt = StringLoader::LoadLC( 
            CPhoneMainResourceResolver::Instance()->ResolveResourceID( 
                    EPhoneVoIPMovedPermanentlyQueryHeader ));
   
    queryParam.SetQueryPrompt( *queryPrompt );

    queryParam.SetDefaultCba(             
        CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( EPhoneVoIPMovedPermanentlySoftkeys ) );
  
    iCommandProcessed = EFalse;
    
    iViewCommandHandle.ExecuteCommandL( EPhoneViewShowQuery, &queryParam );
    
    CleanupStack::PopAndDestroy( queryPrompt );
    }

// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::ShowMultipleChoicesQueryL
// ---------------------------------------------------------------------------
//
void CPhoneCallForwardQueryHandler::ShowMultipleChoicesQueryL(
        TInt aCallId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneCallForwardQueryHandler::ShowMultipleChoicesQueryL" )
        
    iCallId = aCallId;
    iAddressArray = iStateMachine.PhoneEngineInfo()->
        ForwardAddressChoices( aCallId );
    
    if ( NULL == iAddressArray )
        {
        User::Leave( KErrNotSupported );
        }
    
    ResetMultipleChoicesArrayL( *iAddressArray );
    
    iQueryType = EPhoneMultipleChoices;
    TPhoneCmdParamQuery queryParam;
    queryParam.SetQueryCommandObserver( *this );
    queryParam.SetTone( EAvkonSIDInformationTone );
    queryParam.SetQueryType( EPhoneCustomListQueryDialog );
    queryParam.SetItemTextArrayForListQuery( 
            static_cast<MDesCArray&> ( *iMultipleChoicesArray ) );

    queryParam.SetQueryResourceId( 
            CPhoneMainResourceResolver::Instance()->ResolveResourceID( 
                    EPhoneVoIPMultipleChoicesListQuery ) );

    iCommandProcessed = EFalse;
    
    iViewCommandHandle.ExecuteCommandL( EPhoneViewShowQuery, &queryParam );
    
    }

// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::AddToContacsL
// ---------------------------------------------------------------------------
//
void CPhoneCallForwardQueryHandler::AddToContacsL(
        TBool aCreateNew )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneCallForwardQueryHandler::AddToContacsL" )
    
    if ( !iAddressArray ||
          0 == iAddressArray->Count() )
        {
        User::Leave( KErrNotFound );
        }
        
    HBufC* buf = HBufC::NewLC( iAddressArray->MdcaPoint(0).Length() );
    buf->Des().Copy( iAddressArray->MdcaPoint(0) );
    TPtr ptr( buf->Des() );

    iCommandProcessed = EFalse;
    
    TPhoneCmdParamString stringParam;
    stringParam.SetString( &ptr );
    iViewCommandHandle.ExecuteCommandL(
        aCreateNew ? EPhoneViewAddContactByString :
                     EPhoneViewUpdateContactByString,
        &stringParam );   
    
    CleanupStack::PopAndDestroy( buf );
    
    // Forward call to array index 0
    ForwardCallToAddressL( 0 );
    }


// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::SaveAddressToContactsL
// ---------------------------------------------------------------------------
//
void CPhoneCallForwardQueryHandler::SaveAddressToContactsL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
       "CPhoneCallForwardQueryHandler::SaveAddressToContactsL" )
    
    TPhoneCmdParamQuery queryDialogParam;
    iQueryType = EPhoneQueryAddToContacts;
    queryDialogParam.SetQueryCommandObserver( *this );
    queryDialogParam.SetQueryType( EPhoneContactsListQuery );
    queryDialogParam.SetQueryResourceId(
           CPhoneMainResourceResolver::Instance()->
               ResolveResourceID( EPhoneAddtoContactsListQuery )  );
    
    iCommandProcessed = EFalse;
    
    // Display dialog        
    iViewCommandHandle.ExecuteCommandL( EPhoneViewShowQuery,
       &queryDialogParam );
       
    }

// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CPhoneCallForwardQueryHandler::ProcessCommandL(
        TInt aCommandId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
       "CPhoneCallForwardQueryHandler::ProcessCommandL" )
    
    if ( iCommandProcessed )
        {
        return;
        }
    
    iCommandProcessed = ETrue;
    
    if ( iQueryType == EPhoneQueryAddToContacts )
        {
        switch ( aCommandId )
            {          
            case EPhoneNumberAcqCmdSave:
                AddToContacsL( ETrue );     
                break;
                
            case EPhoneNumberAcqCmdAddToName:
                AddToContacsL( EFalse );     
                break;
            case EAknSoftkeyCancel:
                iStateMachine.SendPhoneEngineMessage( 
                        MPEPhoneModel::EPEMessageRelease );     
                break;
            }
        }
    else if ( iQueryType == EPhoneMovedPermanently )
        {
        switch ( aCommandId )
            {          
            case EAknSoftkeyOk:
                SaveAddressToContactsL();     
                break;
                
            default:
                iStateMachine.SendPhoneEngineMessage( 
                        MPEPhoneModel::EPEMessageRelease );  
                break;
            }
        }
    else if ( iQueryType == EPhoneMultipleChoices )
        {
        if ( iAddressArray && aCommandId >= 0 && 
             aCommandId < iAddressArray->MdcaCount() )
            {
            ForwardCallToAddressL( aCommandId );
            }
        else
            {
            iStateMachine.SendPhoneEngineMessage( 
                    MPEPhoneModel::EPEMessageRelease );
            }
        }
    }

// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::ShowCallForwardedNoteL
// ---------------------------------------------------------------------------
//
void CPhoneCallForwardQueryHandler::ShowCallForwardedNoteL()
    {
    __LOGMETHODSTARTEND(PhoneUIVoIPExtension, 
            "CPhoneCallForwardQueryHandler::ShowCallForwardedNoteL( ) ");
   
    CPhoneState* phoneState = 
        static_cast< CPhoneState* >( iStateMachine.State() );       
            
    phoneState->SendGlobalInfoNoteL( EPhoneRemoteForwarding );
    }

// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::ResetMultipleChoicesArrayL
// ---------------------------------------------------------------------------
//
void CPhoneCallForwardQueryHandler::ResetMultipleChoicesArrayL(
        const MDesC8Array& aAddressArray )
    {
    __LOGMETHODSTARTEND(PhoneUIVoIPExtension, 
            "CPhoneCallForwardQueryHandler::ResetMultipleChoicesArrayL( ) ");
   
    delete iMultipleChoicesArray;
    iMultipleChoicesArray = NULL;
    
    iMultipleChoicesArray = 
        new(ELeave) CDesC16ArrayFlat( aAddressArray.MdcaCount() );
    
    for ( TInt i=0; i< aAddressArray.MdcaCount() ; i++ )
        {
        HBufC* buf = HBufC::NewLC( aAddressArray.MdcaPoint(i).Length() );
        buf->Des().Copy( aAddressArray.MdcaPoint(i) );
        iMultipleChoicesArray->AppendL( *buf );
        CleanupStack::PopAndDestroy( buf );
        }
    }

// ---------------------------------------------------------------------------
// CPhoneCallForwardQueryHandler::ForwardCallToAddressL
// ---------------------------------------------------------------------------
//
void CPhoneCallForwardQueryHandler::ForwardCallToAddressL(
        TInt aIndex )
    {
    __LOGMETHODSTARTEND(PhoneUIVoIPExtension, 
            "CPhoneCallForwardQueryHandler::ForwardCallToAddressL( ) ");
   
    ShowCallForwardedNoteL();
    iStateMachine.PhoneEngineInfo()->SetForwardAddressIndex( aIndex );
    iStateMachine.SendPhoneEngineMessage( 
            MPEPhoneModel::EPEMessageForwardCallToAddress );
    }

// End of File
