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
* Description:  -
*
*/


#include <StringLoader.h>
#include <featmgr.h>
#include <eikenv.h>

#include "cphonenumberentrymanager.h"
#include "mpeengineinfo.h"
#include "mphoneviewcommandhandle.h"
#include "mphonestatemachine.h"
#include "mphonecustomization.h"
#include "cphonestate.h"

#include "cphoneuicommandmanager.h"
#include "cphonepubsubproxy.h"
#include "cphonekeys.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamcallheaderdata.h"
#include "cphonemainresourceresolver.h"
#include "phonerssbase.h"
#include "phoneui.pan"
#include "phoneviewcommanddefinitions.h"
#include "phoneappcommands.hrh"
#include "phonelogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPhoneNumberEntryManager::CPhoneNumberEntryManager
// ---------------------------------------------------------------------------
//
CPhoneNumberEntryManager::CPhoneNumberEntryManager(
         CPhoneState* aState,
         MPhoneViewCommandHandle& aViewCommandHandle,
         MPhoneStateMachine& aStateMachine,
         MPhoneCustomization* aCustomization,
         CPhoneUiCommandManager& aUiCommandManager )
   : iState ( aState ),
     iViewCommandHandle ( aViewCommandHandle ),
     iStateMachine ( aStateMachine ),
     iCustomization ( aCustomization ),
     iUiCommandManager ( aUiCommandManager ),
     iEnv( *CEikonEnv::Static() )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneNumberEntryManager::CPhoneNumberEntryManager() ");
    }

// ---------------------------------------------------------------------------
// CPhoneNumberEntryManager::NewL
// ---------------------------------------------------------------------------
//
CPhoneNumberEntryManager* CPhoneNumberEntryManager::NewL( 
        CPhoneState* aState,
        MPhoneViewCommandHandle& aViewCommandHandle,
        MPhoneStateMachine& aStateMachine,
        MPhoneCustomization* aCustomization,
        CPhoneUiCommandManager& aUiCommandManager )
     {
     __LOGMETHODSTARTEND( EPhoneControl, "CPhoneNumberEntryManager::NewL() ");
     CPhoneNumberEntryManager* self = new (ELeave) CPhoneNumberEntryManager(
             aState,
             aViewCommandHandle,
             aStateMachine,
             aCustomization,
             aUiCommandManager );
     
     CleanupStack::PushL( self );
     self->ConstructL();
     CleanupStack::Pop( self );
     
     return self;
     }

// ---------------------------------------------------------------------------
// CPhoneNumberEntryManager::ConstructL
// ---------------------------------------------------------------------------
//
void CPhoneNumberEntryManager::ConstructL()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneNumberEntryManager::ConstructL() ");
    }

// ---------------------------------------------------------------------------
//  CPhoneNumberEntryManager::~CPhoneNumberEntryManager
// ---------------------------------------------------------------------------
//
CPhoneNumberEntryManager::~CPhoneNumberEntryManager()
     {
     __LOGMETHODSTARTEND( EPhoneControl, "CPhoneNumberEntryManager::~CPhoneNumberEntryManager() ");
     delete iNumberEntryContent;  
     }
 
// ======== LOCAL FUNCTIONS ========

// -----------------------------------------------------------
// CPhoneNumberEntryManager::StoreNumberEntryContentL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneNumberEntryManager::StoreNumberEntryContentL()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneNumberEntryManager::StoreNumberEntryContentL( ) ");
    // Store the number entry content to cache
    if ( !iNumberEntryContent )
        {
        iNumberEntryContent = HBufC::NewL( KPhoneNumberEntryBufferSize );
        }    
    TPtr ptr( iNumberEntryContent->Des() );
    ptr.Zero();   
      
    TPhoneCmdParamInteger numberEntryCountParam;
    iViewCommandHandle.ExecuteCommandL( 
            EPhoneViewGetNumberEntryCount,
            &numberEntryCountParam );
    
    TInt neLength( numberEntryCountParam.Integer() );
    if ( neLength > 0 )
        {                    
        TPhoneCmdParamString stringParam;
        stringParam.SetString( &ptr );
        iViewCommandHandle.ExecuteCommandL(
                EPhoneViewGetNumberFromEntry,
                &stringParam );  
        }
    }

// -----------------------------------------------------------
// CPhoneNumberEntryManager::IsNumberEntryContentStored
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneNumberEntryManager::IsNumberEntryContentStored()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneNumberEntryManager::IsNumberEntryContentStored( ) ");
    return iNumberEntryContent ? ETrue : EFalse;
    }

// -----------------------------------------------------------
// CPhoneNumberEntryManager::ClearNumberEntryContentCache
// -----------------------------------------------------------
//
EXPORT_C void CPhoneNumberEntryManager::ClearNumberEntryContentCache()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneNumberEntryManager::ClearNumberEntryContentCache( ) ");
    delete iNumberEntryContent;
    iNumberEntryContent = NULL;
    }
 
// -----------------------------------------------------------
// CPhoneNumberEntryManager::SetNumberEntryVisibilityL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneNumberEntryManager::SetNumberEntryVisibilityL( TBool aVisible )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneNumberEntryManager::SetNumberEntryVisibilityL( ) ");
    __PHONELOG1( EBasic, EPhoneControl, 
            "CPhoneNumberEntryManager::SetNumberEntryVisibilityL : aVisible =%d", 
            aVisible );
    TPhoneCmdParamBoolean booleanParam;
    booleanParam.SetBoolean( ETrue );
    if ( !aVisible )
        {
        booleanParam.SetBoolean( EFalse );
        }
    iViewCommandHandle.ExecuteCommandL( EPhoneViewSetNumberEntryVisible, &booleanParam );
    }

// -----------------------------------------------------------
// CPhoneNumberEntryManager::IsNumberEntryUsedL
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneNumberEntryManager::IsNumberEntryUsedL() const
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneNumberEntryManager::IsNumberEntryUsedL( ) ");
    return iViewCommandHandle.HandleCommandL(
        EPhoneViewGetNumberEntryIsUsedStatus ) ==
        EPhoneViewResponseSuccess;
    }

// -----------------------------------------------------------
// CPhoneNumberEntryManager::IsNumberEntryVisibleL
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneNumberEntryManager::IsNumberEntryVisibleL() const
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneNumberEntryManager::IsNumberEntryVisibleL( ) ");
    return iViewCommandHandle.HandleCommandL(
        EPhoneViewGetNumberEntryIsVisibleStatus ) ==
        EPhoneViewResponseSuccess;
    }

// -----------------------------------------------------------
// CPhoneNumberEntryManager::PhoneNumberFromEntryLC()
// -----------------------------------------------------------
//
EXPORT_C HBufC* CPhoneNumberEntryManager::PhoneNumberFromEntryLC() const
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneNumberEntryManager::PhoneNumberFromEntryLC( ) ");
    HBufC* phoneNumber = HBufC::NewLC( KPhoneNumberEntryBufferSize );
    TPtr ptr( phoneNumber->Des() );
    TPhoneCmdParamString stringParam;
    stringParam.SetString( &ptr );
    iViewCommandHandle.ExecuteCommand(
        EPhoneViewGetNumberFromEntry,
        &stringParam );
    return phoneNumber;
    }

// ---------------------------------------------------------
// CPhoneNumberEntryManager::NumberEntryClearL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneNumberEntryManager::NumberEntryClearL() const
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneNumberEntryManager::NumberEntryClearL( ) ");
    iViewCommandHandle.ExecuteCommandL( EPhoneViewClearNumberEntryContent );
    }

// -----------------------------------------------------------
// CPhoneNumberEntryManager::SetVisibilityIfNumberEntryUsedL
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneNumberEntryManager::SetVisibilityIfNumberEntryUsedL(
        TBool aVisibility )
    {
    __LOGMETHODSTARTEND(EPhoneControl, 
            "CPhoneGsmInCall::SetVisibilityIfNumberEntryUsedL() ");
    TBool retValue(EFalse);
    if ( IsNumberEntryUsedL() )
        {
        TPhoneCmdParamBoolean booleanParam;
        booleanParam.SetBoolean( EFalse );
        if ( aVisibility )
            {
            booleanParam.SetBoolean( ETrue );
            }
        iViewCommandHandle.ExecuteCommandL( EPhoneViewSetNumberEntryVisible, &booleanParam );
        __PHONELOG1( EBasic, EPhoneControl, 
                    "SetVisibilityIfNumberEntryUsedL: %d", 
                    aVisibility );
        retValue = ETrue;
        }
    return retValue;
    }

// -----------------------------------------------------------
// CPhoneNumberEntryManager::RemoveNumberEntryIfVisibilityIsFalseL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneNumberEntryManager::RemoveNumberEntryIfVisibilityIsFalseL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, 
                "CPhoneNumberEntryManager::RemoveNumberEntryIfVisibilityIsFalseL() ");
    // Remove the number entry if it isn't DTMF dialer
    if ( !IsNumberEntryVisibleL() )
        {
        iViewCommandHandle.ExecuteCommandL( EPhoneViewRemoveNumberEntry );
        }
    }

// -----------------------------------------------------------
// CPhoneNumberEntryManager::RemoveNumberEntryIfVisibilityIsFalseL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneNumberEntryManager::ClearNumberEntryContentCacheIfContentStored()
    {
    __LOGMETHODSTARTEND(EPhoneControl, 
                    "CPhoneNumberEntryManager::ClearNumberEntryContentCacheIfContentStored() ");
    if ( IsNumberEntryContentStored() )
        {
        ClearNumberEntryContentCache();
        }
    }

// ---------------------------------------------------------
// CPhoneNumberEntryManager::CallFromNumberEntryL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneNumberEntryManager::CallFromNumberEntryL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneNumberEntryManager::CallFromNumberEntryL( ) ");
    __ASSERT_DEBUG( iStateMachine.PhoneEngineInfo(), Panic( EPhoneCtrlInvariant ) );
    if ( IsCustomizedDialerVisibleL() )
        {
        return;
        }
    HBufC* phoneNumber = PhoneNumberFromEntryLC();
    iStateMachine.PhoneEngineInfo()->SetPhoneNumber( *phoneNumber );
    if ( phoneNumber->Des().Length() < KPhoneValidPhoneNumberLength )
        {
        iViewCommandHandle.ExecuteCommandL( EPhoneViewRemoveNumberEntry );
        iState->HandleNumberEntryClearedL();
        }
    CleanupStack::PopAndDestroy( phoneNumber );

    if ( !iCustomization ||
         !iCustomization->HandleCallFromNumberEntryL() )
        {
        // Customization didn't handle call. Dial voice call
        // as normally
        iState->DialVoiceCallL();
        }
    }

// -----------------------------------------------------------
// CPhoneState::IsCustomizedDialerVisibleL()
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneNumberEntryManager::IsCustomizedDialerVisibleL() const
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneNumberEntryManager::IsCustomizedDialerVisibleL( ) ");
    return iViewCommandHandle.HandleCommandL(
        EPhoneIsCustomizedDialerVisible ) == EPhoneViewResponseSuccess;
    }
