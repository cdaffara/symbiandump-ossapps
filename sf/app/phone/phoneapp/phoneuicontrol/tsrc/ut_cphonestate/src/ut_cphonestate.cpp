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
* Description:
*
*/

#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>
#include <smcmacros.h>
#include <msmcmockaction.h>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <featmgr.h>
#include <eikmenup.h>
#include <eikmenub.h> 
#include <HWRMDomainPSKeys.h>
#include <PhoneUI.rsg>
#include <StartupDomainPSKeys.h>
#include <coreapplicationuisdomainpskeys.h>

#include "ut_cphonestate.h"
#include "CPhoneMainResourceResolver.h"
#include "CPhoneStateMachineGSM.h"
#include "PEVirtualEngine.h"
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "TPhoneCmdParamCallHeaderData.h"
#include "TPhoneCmdParamAudioOutput.h"
#include "cpeengineinfo.h"
#include "cpeengineinfoimpl.h"
#include "phonerssbase.h"
#include "phonestatedefinitionsgsm.h"
#include "TPhoneCmdParamInteger.h"
#include "TPhoneCmdParamKeyEvent.h"
#include "TPhoneCmdParamGlobalNote.h"
#include "TPhoneCmdParamBoolean.h"
#include "TPhoneCmdParamString.h"
#include "TPhoneCmdParamCallStateData.h"
#include "mphonecustomization.h"
#include "telephonyvariant.hrh"
#include "phoneappcommands.hrh"
#include "phoneconstants.h"
#include "cphonecustomizationvoip.h"
#include "tphonecmdparamglobalnote.h"

#define private friend class ut_cphonestate;private
#include "cphonestate_tester.h"

// needed for mocking CPhoneMainResourceResolver::Instance()
class CPhoneMainResourceResolverMock : public CPhoneMainResourceResolver
    {
    };

TPEPhoneAudioRouteParameters routeParameters;

// this is set true if there is no need check param id value.
TBool doNotCheckParamId;

// -----------------------------------------------------------------------------
// operator for comparing parameter expected and result value.
// -
// -----------------------------------------------------------------------------
//
TBool operator==(const TPhoneCommandParam& param1, const TPhoneCommandParam& param2 )
    {
    TBool ret( ETrue );
    
    if ( param1.ParamId() == param2.ParamId() )
        {
    
        if ( TPhoneCommandParam::EPhoneParamIdInteger == param1.ParamId() )
            {
            const TPhoneCmdParamInteger  &value1 = static_cast<const TPhoneCmdParamInteger&> (param1);
            const TPhoneCmdParamInteger  &value2 = static_cast<const TPhoneCmdParamInteger&> (param2);
            if ( value1.Integer() != value2.Integer() )
                {
                ret = EFalse;
                }
            }
        else if ( TPhoneCommandParam::EPhoneParamIdKeyEvent == param1.ParamId() )
            {
            const TPhoneCmdParamKeyEvent  &value1 = static_cast<const TPhoneCmdParamKeyEvent&> (param1);
            const TPhoneCmdParamKeyEvent  &value2 = static_cast<const TPhoneCmdParamKeyEvent&> (param2);
            if ( value1.EventCode() != value2.EventCode() )
                {
                ret = EFalse;
                }
            }
        else if ( TPhoneCommandParam::EPhoneParamIdGlobalNote == param1.ParamId() )
            {
            const TPhoneCmdParamGlobalNote  &value1 = static_cast<const TPhoneCmdParamGlobalNote&> (param1);
            const TPhoneCmdParamGlobalNote  &value2 = static_cast<const TPhoneCmdParamGlobalNote&> (param2);
            if ( value1.Type() != value2.Type() )
                {
                ret = EFalse;
                }
            }
        else if ( TPhoneCommandParam::EPhoneParamIdBoolean == param1.ParamId() && !doNotCheckParamId )
            {
            const TPhoneCmdParamBoolean  &value1 = static_cast<const TPhoneCmdParamBoolean&> (param1);
            const TPhoneCmdParamBoolean  &value2 = static_cast<const TPhoneCmdParamBoolean&> (param2);
            if ( value1.Boolean() != value2.Boolean() )
                {
                ret = EFalse;
                }
            } 
        else
            {
            ret = ETrue;  // just paramiId compared
            }
        }
    return ret;
    }

class CPhoneViewController : public CBase, public MPhoneViewCommandHandle
{
public:
    CPhoneViewController() {}
    ~CPhoneViewController() {}

    void ExecuteCommandL( 
        TPhoneViewCommandId aCmdId )
    {
        SMC_MOCK_METHOD1(void, TPhoneViewCommandId, aCmdId);
    }


    void ExecuteCommandL( 
        TPhoneViewCommandId aCmdId, 
        TInt aCallId )
    {
        SMC_MOCK_METHOD2(void, TPhoneViewCommandId, aCmdId, TInt, aCallId);
    }


    void ExecuteCommandL( 
        TPhoneViewCommandId aCmdId, 
        TPhoneCommandParam* aCommandParam )
    {
        SMC_MOCK_METHOD2(void, 
            TPhoneViewCommandId, aCmdId, 
            TPhoneCommandParam*, aCommandParam);
    }

    void ExecuteCommandL( 
        TPhoneViewCommandId aCmdId, 
        TInt aCallId, 
        TPhoneCommandParam* aCommandParam )
    {
        SMC_MOCK_METHOD3(void, 
            TPhoneViewCommandId, aCmdId,
            TInt, aCallId, 
            TPhoneCommandParam*, aCommandParam);
    }

    void ExecuteCommandL( 
        TPhoneViewCommandId aCmdId, 
        TInt aCallId, 
        TDesC& aMessage )
    {
        SMC_MOCK_METHOD3(void, 
            TPhoneViewCommandId, aCmdId,
            TInt, aCallId, 
            TDesC&, aMessage);
    }

    TPhoneViewResponseId HandleCommandL( 
        TPhoneViewCommandId aCmdId )
    {
        SMC_MOCK_METHOD1(TPhoneViewResponseId, 
            TPhoneViewCommandId, aCmdId);
    }

    TPhoneViewResponseId HandleCommandL( 
        TPhoneViewCommandId aCmdId, 
        TPhoneCommandParam* aCommandParam )
    {
        SMC_MOCK_METHOD2(TPhoneViewResponseId, 
            TPhoneViewCommandId, aCmdId,
            TPhoneCommandParam*, aCommandParam);
    }

    void ExecuteCommand( 
        TPhoneViewCommandId aCmdId )
    {
        SMC_MOCK_METHOD1(void, 
            TPhoneViewCommandId, aCmdId);
    }
        
    void ExecuteCommand( 
        TPhoneViewCommandId aCmdId, 
        TPhoneCommandParam* aCommandParam )
    {
        SMC_MOCK_METHOD2(void, 
            TPhoneViewCommandId, aCmdId,
            TPhoneCommandParam*, aCommandParam);
    }
    
    const TDesC& FetchContent() 
    {
        SMC_MOCK_METHOD0(const TDesC&);
    };
};

// - Construction -----------------------------------------------------------

ut_cphonestate* ut_cphonestate::NewL()
    {
    ut_cphonestate* self = ut_cphonestate::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphonestate* ut_cphonestate::NewLC()
    {
    ut_cphonestate* self = new( ELeave ) ut_cphonestate();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonestate::~ut_cphonestate()
    {
    CSmcMockContext::Release();
    }

ut_cphonestate::ut_cphonestate()
    {
    }

void ut_cphonestate::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------

void ut_cphonestate::SetupL()
    {
    iMockContext->InitializeL();
    
    SmcDefaultValue<const TPESimState&>::SetL( EPESimUsable );
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPENotActive );
    SmcDefaultValue<const TPEPhoneAudioRouteParameters&>::SetL( routeParameters );
    SmcDefaultValue<const TBool&>::SetL( EFalse );
    SmcDefaultValue<const TProfileRingingType&>::SetL( EProfileRingingTypeRinging );
     
      
    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
        SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );

    iViewCmdHandleMock = new (ELeave) CPhoneViewController();

    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock );

    iEngineInfoMock = CPEEngineInfo::NewL();
        SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );
        
    iPhoneState = new( ELeave ) cphonestate_tester( iStateMachineMock, iViewCmdHandleMock, NULL );
    iPhoneState->BaseConstructL();
       
    doNotCheckParamId = EFalse;
    }

void ut_cphonestate::Teardown(  )
    {
    delete iViewCmdHandleMock;
        iViewCmdHandleMock = NULL;
        
    delete iCustomizationMock;
        iCustomizationMock = NULL;
            
    delete iPhoneState;
        iPhoneState = NULL;
        
    delete iStateMachineMock;
        iStateMachineMock = NULL;
        
    delete iEngineInfoMock;
        iEngineInfoMock = NULL;
        
    delete iCPhoneMainResourceResolverMock;
        iCPhoneMainResourceResolverMock = NULL;
        
    SmcDefaultValue<const TPESimState&>::Reset();
    SmcDefaultValue<const TPEAudioOutput&>::Reset();
    SmcDefaultValue<const TPEPhoneAudioRouteParameters&>::Reset();
    SmcDefaultValue<const TBool&>::Reset();
    SmcDefaultValue<const TProfileRingingType&>::Reset();

    iMockContext->Reset();
    }
    
/**
 *  Test name: Construction and destruct.
 *  Use case: Construction and destruct.
 *  Motivation: None.
 *  stif ref: None.
 */
void ut_cphonestate::T_ConstructionL(  )
    {
    // SetupL and Teardown does the test.
    }

/**
 *  Test name: Test method DisplayCallTerminationNoteL.
 *  Use case: Test method DisplayCallTerminationNoteL.
 *  Motivation: BDJR-7JLF76
 *  stif ref: None.
 */
void ut_cphonestate::T_DisplayCallTerminationNoteL( )
    {
   
    iMockContext->ExpectCallL( "PhoneNoteUtil::SendGlobalNoteL" );
      
    iPhoneState->T_DisplayCallTerminationNoteL();
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

/**
 *  Test name: T_HandleKeyEventUpArrowNENotVisibleL
 *  Use case: NumberEntry is not in use and "->" key is pressed. -> volume is increased  
 *  Motivation: TTEN-7LYF76
 *  stif ref: None.
 * */
void ut_cphonestate::T_HandleKeyEventUpArrowNENotVisibleL( )
    {
    TInt volume( KPhoneDefaultVolume );
    TKeyEvent keyEvent;
    keyEvent.iScanCode = EStdKeyUpArrow; //
    keyEvent.iCode = EKeyUpArrow;
    keyEvent.iRepeats = 0;
    TEventCode eventCode(EEventKey);
    iMockContext->ExpectCallL( "PhoneAudioUtil::IncreaseAudioVolumeL" );
         
    iPhoneState->HandleKeyEventL( keyEvent, eventCode );
     
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

/**
 *  Test name: T_HandleKeyEventDownArrowNENotVisibleL
 *  Use case: NumberEntry is not in use and "<-" key is pressed. -> volume is decreased  
 *  Motivation: test coverage.
 *  stif ref: None.
 * */
void ut_cphonestate::T_HandleKeyEventDownArrowNENotVisibleL( )
    {
    TInt volume( KPhoneDefaultVolume );
    TKeyEvent keyEvent;
    keyEvent.iScanCode = EStdKeyDownArrow; //
    keyEvent.iCode = EKeyDownArrow;
    keyEvent.iRepeats = 0;
    TEventCode eventCode(EEventKey);
    iMockContext->ExpectCallL( "PhoneAudioUtil::DecreaseAudioVolumeL" );
         
    iPhoneState->HandleKeyEventL( keyEvent, eventCode );
     
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

/**
 *  Test name: T_HandleDtmfKeyToneStarkeyL
 *  Use case: send event to phoneengine that DTMF tone for "*" key is played
 *  Motivation: TTEN-7LYF76
 *  stif ref: None.
 */
void ut_cphonestate::T_HandleDtmfKeyToneStarkeyL()
    {
    TKeyEvent keyEvent;
    keyEvent.iScanCode = EStdKeyNkpAsterisk; //key:'*'
    keyEvent.iCode = KPhoneDtmfStarCharacter;
    keyEvent.iRepeats = 0;
    keyEvent.iModifiers = 0;
    TEventCode eventCode(EEventKey);
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
        ReturnsL( ETrue );
    _LIT( KPhoneNumber, "12345" );
    HBufC* phoneNumber = KPhoneNumber().AllocL();
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
         ReturnsL(phoneNumber);
           
    //phone-engine receives message
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetPhoneNumber" );
     
    iMockContext->ExpectCallL( "CPhoneKeys::IsDtmfTone" ).
        WithL( keyEvent, eventCode ).
        ReturnsL( ETrue );
       
    iPhoneState->HandleDtmfKeyToneL( keyEvent, eventCode );
       
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

/**
 *  Test name: T_HandleDtmfKeyTone1KeyL 
 *  Use case: do not send event to phoneengine 
 *  Motivation: TTEN-7LYF76
 *  stif ref: None.
 */
void ut_cphonestate::T_HandleDtmfKeyTone1KeyL()
    {
     
    TKeyEvent keyEvent;
    keyEvent.iScanCode = EStdKeyNkp1; //key:'1'
    keyEvent.iCode = KPhoneDtmf1Character;
    keyEvent.iRepeats = 0;
    keyEvent.iModifiers = 0;
    TEventCode eventCode(EEventKey);
    TChar expChar(keyEvent.iCode);
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
            ReturnsL( ETrue );
    _LIT( KPhoneNumber, "12345" );
    HBufC* phoneNumber = KPhoneNumber().AllocL();
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
         ReturnsL(phoneNumber);
    //phone-engine receives message
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetPhoneNumber" );
     
    iMockContext->ExpectCallL( "CPhoneKeys::IsDtmfTone" ).
        WithL( keyEvent, eventCode ).
        ReturnsL( ETrue );
      
    iPhoneState->HandleDtmfKeyToneL( keyEvent, eventCode );
      
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    
    }
 
/**
 *  Test name: T_HandleAudioOutputChangedL 
 *  Use case: Call audio route output has been changed
 *  Motivation: EBNI-7KM8N4
 *  stif ref: -
 */
void ut_cphonestate::T_HandleAudioOutputChangedL()
    {        
    iMockContext->ExpectCallL( "CPhoneUiCommandManager::UpdateUiCommands" );
    
  
    iPhoneState->HandlePhoneEngineMessageL(
        MEngineMonitor::EPEMessageAudioOutputChanged, 
        0 );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

/**
 *  Test name:Test method StartShowSecurityNoteL 
 *  Use case: StartShowSecurityNoteL called EPhoneViewSetSecurityMode gets called.
 *  Motivation: TRLM-7Q3HVZ 
 */
void ut_cphonestate::T_StartShowSecurityNoteL()
    {
    TPhoneCmdParamBoolean securityMode;
    securityMode.SetBoolean( ETrue );
    iMockContext->ExpectCallL( "CPhoneViewController::ExecuteCommandL" ).
        WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( 
                EPhoneViewSetSecurityMode, 
                securityMode );
        
    iPhoneState->T_StartShowSecurityNoteL();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }


/**
 *  Test name: T_HandleDtmfKeyToneL_ModeChange
 *  Use case: DTMF tone is played when number entry in numeric mode and not played in alpha mode 
 *  Motivation: EDXU-7T69N4
 *  stif ref: None.
 **/
void ut_cphonestate::T_HandleDtmfKeyToneL_ModeChange()
    {
    TKeyEvent keyEvent;
    keyEvent.iScanCode = EStdKeyNkp1; //key:'1'
    keyEvent.iCode = KPhoneDtmf1Character;  
    keyEvent.iRepeats = 0;
    keyEvent.iModifiers = 0;
    TEventCode eventCode( EEventKey );
    
    iMockContext->ExpectCallL( "CPhoneKeys::IsDtmfTone" ).
           WithL( keyEvent, eventCode ).
           ReturnsL( ETrue );
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
            ReturnsL( ETrue );
    _LIT( KPhoneNumber, "12345" );
    HBufC* phoneNumber = KPhoneNumber().AllocL();
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
         ReturnsL(phoneNumber);
    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage" ).
        WithL( MPEPhoneModel::EPEMessagePlayDTMF ).
        TimesL( 1 );   
  
    iPhoneState->HandleDtmfKeyToneL( keyEvent, eventCode ); 
    
    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage" ).
        TimesL( 0 );
    
    iPhoneState->HandleDtmfKeyToneL( keyEvent, eventCode ); 
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

/**
 *  Test name: T_HandleDisconnectingL
 *  Use case: 
 *  Motivation: 
 *  stif ref: None.
 **/
void ut_cphonestate::T_HandleDisconnectingL()
    {
    iMockContext->ExpectCallL( "CPhoneViewController::ExecuteCommandL" ).
        WithL( EPhoneViewStopRingTone );
    iMockContext->ExpectCallL( "CPhoneClearBlacklist::ClearBlackListOnNextKey" );
    TInt callId = 0;
    iPhoneState->T_HandleDisconnectingL(callId);
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

/**
 *  Test name: T_HandleBTActivatedL 
 *  Use case: Call audio route output has been changed to bt 
 *  Motivation: ou1cimx1#218924
 *  stif ref: -
 */
void ut_cphonestate::T_HandleBTActivatedL()
    {        
    
    iMockContext->ExpectCallL( "PhoneAudioUtil::HandleAudioOutputChangedL()" );
    
    iPhoneState->HandlePhoneEngineMessageL(
        MEngineMonitor::EPEMessageAudioOutputChanged, 
        0 );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }
/**
 * Test name: T_OnlySideVolumeKeySupported_VolumeKeysFALSEandSideVolumeKeysTRUE_L
 * PreConditions set in this case:
 *          KTelephonyLVFlagScrollVolumeKeys = EFalse
 *          KFeatureIdSideVolumeKeys         = ETRue
 *          
 * Motivation: REQ 417-4164
 */
void ut_cphonestate::T_OnlySideVolumeKeySupported_VolumeKeysFALSEandSideVolumeKeysTRUE_L()
    {
    iMockContext->ExpectCallL( "CPhoneCenRepProxy::IsTelephonyFeatureSupported" ).
        WithL(KTelephonyLVFlagScrollVolumeKeys).
        ReturnsL(EFalse);
    iMockContext->ExpectCallL( "FeatureManager::FeatureSupported" ).
        WithL(KFeatureIdSideVolumeKeys).
        ReturnsL(ETrue);
    EUNIT_ASSERT_EQUALS( ETrue, iPhoneState->OnlySideVolumeKeySupported() );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

/**
 * Test name: T_OnlySideVolumeKeySupported_VolumeKeysFALSEandSideVolumeKeysFALSE_L * 
 * PreConditions set in this case:
 *          KTelephonyLVFlagScrollVolumeKeys = EFalse
 *          KFeatureIdSideVolumeKeys         = EFalse
 *          
 * Motivation: REQ 417-4164
 */
void ut_cphonestate::T_OnlySideVolumeKeySupported_VolumeKeysFALSEandSideVolumeKeysFALSE_L()
    {
    iMockContext->ExpectCallL( "CPhoneCenRepProxy::IsTelephonyFeatureSupported" ).
        WithL(KTelephonyLVFlagScrollVolumeKeys).
        ReturnsL(EFalse);
    iMockContext->ExpectCallL( "FeatureManager::FeatureSupported" ).
        WithL(KFeatureIdSideVolumeKeys).
        ReturnsL(EFalse);
    EUNIT_ASSERT_EQUALS( EFalse, iPhoneState->OnlySideVolumeKeySupported() );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

/**
 * Test name: T_OnlySideVolumeKeySupported_VolumeKeysTRUEandSideVolumeKeysFALSE_L
 * PreConditions set in this case:
 *          KTelephonyLVFlagScrollVolumeKeys = ETrue
 *          KFeatureIdSideVolumeKeys         = EFalse
 *          
 * Motivation: REQ 417-4164
 */
void ut_cphonestate::T_OnlySideVolumeKeySupported_VolumeKeysTRUEandSideVolumeKeysFALSE_L()
    {    
    iMockContext->ExpectCallL( "CPhoneCenRepProxy::IsTelephonyFeatureSupported" ).
        WithL(KTelephonyLVFlagScrollVolumeKeys).
        ReturnsL(ETrue);
    EUNIT_ASSERT_EQUALS( EFalse, iPhoneState->OnlySideVolumeKeySupported() );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

/**
 * Test name: T_DisplayCallSetup_SetNEVisibilityFalseL
 * Motivation: Refactor & Test Coverage
 */
void ut_cphonestate::T_DisplayCallSetup_SetNEVisibilityFalseL()
    {
    TInt magicCallId(1);
    iMockContext->ExpectCallL( 
            "CPhoneViewController::ExecuteCommand" ).
            WithL( EPhoneViewBeginUpdate );
    iMockContext->ExpectCallL( 
            "CPhoneNumberEntryManager::SetNumberEntryVisibilityL" ).
            WithL( EFalse );
    iMockContext->ExpectCallL( 
            "CPhoneViewController::ExecuteCommandL" ).
            WithL( EPhoneViewRemovePhoneDialogs );
    iMockContext->ExpectCallL( 
            "CPhoneViewController::ExecuteCommandL" ).
            WithL( EPhoneViewBringPhoneAppToForeground );
    
    iMockContext->ExpectCallL( 
            "CPhoneViewController::ExecuteCommandL" ).
            WithL<TPhoneViewCommandId, TInt> 
                ( EPhoneViewCreateCallHeader, magicCallId );
    
    iMockContext->ExpectCallL( 
            "CPhoneUiCommandManager::UpdateUiCommands" );
    
    iPhoneState->DisplayCallHeaderL(magicCallId, CPhoneState::ESetNEVisibilityFalse);
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

/**
 * Test name: T_DisplayCallSetup_CheckIfNEUsedBeforeSettingVisibilityFalseL
 * Motivation: Refactor & Test Coverage
 */
void ut_cphonestate::T_DisplayCallSetup_CheckIfNEUsedBeforeSettingVisibilityFalseL()
    {
    TInt magicCallId(1);
    iMockContext->ExpectCallL( 
            "CPhoneViewController::ExecuteCommand" ).
            WithL( EPhoneViewBeginUpdate );
    iMockContext->ExpectCallL( 
            "CPhoneNumberEntryManager::SetVisibilityIfNumberEntryUsedL" ).
            WithL( EFalse );
    iMockContext->ExpectCallL( 
            "CPhoneViewController::ExecuteCommandL" ).
            WithL( EPhoneViewRemovePhoneDialogs );
    iMockContext->ExpectCallL( 
            "CPhoneViewController::ExecuteCommandL" ).
            WithL( EPhoneViewBringPhoneAppToForeground );
    
    iMockContext->ExpectCallL( 
            "CPhoneViewController::ExecuteCommandL" ).
            WithL<TPhoneViewCommandId, TInt> 
                ( EPhoneViewCreateCallHeader, magicCallId );
    
    iMockContext->ExpectCallL( 
            "CPhoneUiCommandManager::UpdateUiCommands" );
    
    iPhoneState->DisplayCallHeaderL(
            magicCallId, 
            CPhoneState::ECheckIfNEUsedBeforeSettingVisibilityFalse);
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// Old VOIP related test cases.
/*void ut_cphonestate::SetupWithVoipL(  )
    {
    iMockContext->InitializeL();
        
    SmcDefaultValue<const TPESimState&>::SetL( EPESimUsable );
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPENotActive );
    SmcDefaultValue<const TPEPhoneAudioRouteParameters&>::SetL( routeParameters );
    SmcDefaultValue<const TBool&>::SetL( EFalse );
    SmcDefaultValue<const TProfileRingingType&>::SetL( EProfileRingingTypeRinging );
      
    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
        SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );

    iViewCmdHandleMock = new (ELeave) CPhoneViewController();

    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock );

    iEngineInfoMock = CPEEngineInfo::NewL();
        SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );
    
    iCustomizationMock = 
          CPhoneCustomizationVoip::NewL( *iStateMachineMock, *iViewCmdHandleMock );
        
    iPhoneState = new( ELeave ) cphonestate_tester( iStateMachineMock, iViewCmdHandleMock, iCustomizationMock );
    
    doNotCheckParamId = EFalse;
    }*/
/**
 * Test name: T_HandleCommandL_CallsPhoneCustomization_L
 * Use case: Command is handled in phone customization
 * Motivation: 
 */
/*void ut_cphonestate::T_HandleCommandL_CallsPhoneCustomization_L()
    {
    const TInt KCommand = 1000000;
    iMockContext->ExpectCallL( "CPhoneCustomizationVoip::HandleCommandL" ).
        WithL( KCommand );
    iPhoneState->HandleCommandL( KCommand ); 
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }*/
/**
 * Test name: T_HandlePhoneEngineMessage_EPEMessageCallSecureStatusChanged_L
 * Use case: Secure status of call changes 
 * Motivation: 
 */
/*void ut_cphonestate::T_HandlePhoneEngineMessage_EPEMessageCallSecureStatusChanged_L()
    {
    const TInt KCallId = 1;
    iMockContext->ExpectCallL( "CPhoneCustomizationVoip::HandlePhoneEngineMessageL" ).
        WithL( MEngineMonitor::EPEMessageCallSecureStatusChanged, KCallId );
    iPhoneState->HandlePhoneEngineMessageL( 
        MEngineMonitor::EPEMessageCallSecureStatusChanged, KCallId ); 
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }*/
/**
 * Test name: T_HandlePhoneEngineMessage_CallsPhoneCustomization_L
 * Use case: Phoneengine message is handled by phone customization
 * Motivation: 
 */
/*void ut_cphonestate::T_HandlePhoneEngineMessage_CallsPhoneCustomization_L()
    {
    const TInt KMessage = 1000000;
    const TInt KCallId = 1;
    iMockContext->ExpectCallL( "CPhoneCustomizationVoip::HandlePhoneEngineMessageL" ).
        WithL( KMessage, KCallId ).
        ReturnsL( ETrue );
    iPhoneState->HandlePhoneEngineMessageL( KMessage,KCallId ); 
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }*/
/**
 * Test name: T_ShowNumberBusyNoteL_CallsCustomizeBusyNoteText_L
 * Use case: Number busy note text gets customized
 * Motivation: 
 */
/*void ut_cphonestate::T_ShowNumberBusyNoteL_CallsCustomizeBusyNoteText_L()
    {
    iMockContext->ExpectCallL( "CPhoneCustomizationVoip::CustomizeBusyNoteText" );
    iPhoneState->T_ShowNumberBusyNoteL(); 
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }*/
/**
 * Test name: T_PhoneNumberFromEntryLC
 * Use case:
 * Motivation: 
 */
/*void ut_cphonestate::T_PhoneNumberFromEntryLC()
    {
    TPhoneCmdParamString stringParam;
      
    iMockContext->ExpectCallL( "CPhoneViewController::ExecuteCommand" ).
        WithL<TPhoneViewCommandId, TPhoneCommandParam&>(
                   EPhoneViewGetNumberFromEntry,
                   stringParam );
    
    HBufC* number = iPhoneState->T_PhoneNumberFromEntryLC();
      
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    CleanupStack::PopAndDestroy( number );
    }*/
/**
 * Test name: T_UpdateRemoteInfoDataL
 * Use case: 
 * Motivation: 
 */
/*void ut_cphonestate::T_UpdateRemoteInfoDataL()
    {
    const TInt KCallId = 1;
    TPhoneCmdParamCallHeaderData callHeaderData;
    iMockContext->ExpectCallL( "CPhoneCallHeaderManager::UpdateCallHeaderInfoL" ).
        WithL( KCallId ).
        ReturnsL( callHeaderData );
    iMockContext->ExpectCallL( "CPhoneCallHeaderManager::GetInCallNumberTextL" ).
        WithL( KCallId, KNullDesC() );
        
    iMockContext->ExpectCallL( "CPhoneCustomizationVoip::ModifyCallHeaderTexts" );
    
    iPhoneState->T_UpdateRemoteInfoDataL( KCallId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }*/
// CaptureKeysDuringCallNotificationL( it callsAknLayoutUtils::PenEnabled(), which crash some reason)
// call crash this case.
/*void ut_cphonestate::T_UpdateSingleActiveCallL_VoIPcallL( )
    {
    SmcDefaultValue<TPECallType>::SetL( EPECallTypeVoIP );
    TInt callId = 0;
    TPhoneCmdParamCallHeaderData callHeaderData;
    
    iMockContext->ExpectCallL( "CPhoneCallHeaderManager::UpdateCallHeaderInfoL" ).
          WithL<TInt>( callId ).
          ReturnsL( callHeaderData );
    
    iMockContext->ExpectCallL( "CPhoneCallHeaderManager::GetInCallNumberTextL" );
  //      WithL( callId, KNullDesC() );
    
    iMockContext->ExpectCallL( "CPhoneCustomizationVoip::ModifyCallHeaderTexts" );
      
    iPhoneState->T_UpdateSingleActiveCallL( callId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );   
    
    SmcDefaultValue<TPECallType>::Reset();
    }*/

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphonestate,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "T_ConstructionL",
    "",
    "T_ConstructionL",
    "FUNCTIONALITY",
    SetupL, T_ConstructionL, Teardown)

EUNIT_TEST(
    "DisplayCallTerminationNoteL",
    "",
    "DisplayCallTerminationNoteL",
    "FUNCTIONALITY",
    SetupL, T_DisplayCallTerminationNoteL, Teardown)

EUNIT_TEST(
    "T_HandleKeyEventUpArrowNENotVisibleL",
    "",
    "T_HandleKeyEventUpArrowNENotVisibleL",
    "FUNCTIONALITY",
    SetupL,  T_HandleKeyEventUpArrowNENotVisibleL, Teardown)
   
EUNIT_TEST(
    "T_HandleKeyEventDownArrowNENotVisibleL",
    "",
    "T_HandleKeyEventDownArrowNENotVisibleL",
    "FUNCTIONALITY",
    SetupL,  T_HandleKeyEventDownArrowNENotVisibleL, Teardown)

EUNIT_TEST(
    "T_HandleDtmfKeyToneStarkeyL",
    "",
    "T_HandleDtmfKeyToneStarkeyL",
    "FUNCTIONALITY",
    SetupL, T_HandleDtmfKeyToneStarkeyL, Teardown)

EUNIT_TEST(
    "T_HandleDtmfKeyTone1KeyL",
    "",
    "T_HandleDtmfKeyTone1KeyL",
    "FUNCTIONALITY",
    SetupL, T_HandleDtmfKeyTone1KeyL, Teardown)

EUNIT_TEST(
    "T_HandleAudioOutputChangedL",
    "",
    "T_HandleAudioOutputChangedL",
    "FUNCTIONALITY",
    SetupL, T_HandleAudioOutputChangedL, Teardown)

EUNIT_TEST(
    "T_OnlySideVolumeKeySupported_VolumeKeysFALSEandSideVolumeKeysTRUE_L",
    "",
    "T_OnlySideVolumeKeySupported_VolumeKeysFALSEandSideVolumeKeysTRUE_L",
    "FUNCTIONALITY",
    SetupL, T_OnlySideVolumeKeySupported_VolumeKeysFALSEandSideVolumeKeysTRUE_L, Teardown)
    
EUNIT_TEST(
    "T_OnlySideVolumeKeySupported_VolumeKeysFALSEandSideVolumeKeysFALSE_L",
    "",
    "T_OnlySideVolumeKeySupported_VolumeKeysFALSEandSideVolumeKeysFALSE_L",
    "FUNCTIONALITY",
    SetupL, T_OnlySideVolumeKeySupported_VolumeKeysFALSEandSideVolumeKeysFALSE_L, Teardown)
    
EUNIT_TEST(
    "T_OnlySideVolumeKeySupported_VolumeKeysTRUEandSideVolumeKeysFALSE_L",
    "",
    "T_OnlySideVolumeKeySupported_VolumeKeysTRUEandSideVolumeKeysFALSE_L",
    "FUNCTIONALITY",
    SetupL, T_OnlySideVolumeKeySupported_VolumeKeysTRUEandSideVolumeKeysFALSE_L, Teardown)

EUNIT_TEST(
    "T_StartShowSecurityNoteL",
    "",
    "T_StartShowSecurityNoteL",
    "FUNCTIONALITY",
    SetupL, T_StartShowSecurityNoteL, Teardown)

EUNIT_TEST(
    "T_HandleDtmfKeyToneL_ModeChange",
    "",
    "T_HandleDtmfKeyToneL_ModeChange",
    "FUNCTIONALITY",
    SetupL, T_HandleDtmfKeyToneL_ModeChange, Teardown)

EUNIT_TEST(
    "T_HandleDisconnectingL",
    "",
    "T_HandleDisconnectingL",
    "FUNCTIONALITY",
    SetupL, T_HandleDisconnectingL, Teardown)

EUNIT_TEST(
    "T_HandleBTActivatedL",
    "",
    "T_HandleBTActivatedL",
    "FUNCTIONALITY",
    SetupL, T_HandleBTActivatedL, Teardown)
    
EUNIT_TEST(
    "T_DisplayCallSetup_SetNEVisibilityFalseL",
    "",
    "T_DisplayCallSetup_SetNEVisibilityFalseL",
    "FUNCTIONALITY",
    SetupL, T_DisplayCallSetup_SetNEVisibilityFalseL, Teardown)
    
EUNIT_TEST(
    "T_DisplayCallSetup_CheckIfNEUsedBeforeSettingVisibilityFalseL",
    "",
    "T_DisplayCallSetup_CheckIfNEUsedBeforeSettingVisibilityFalseL",
    "FUNCTIONALITY",
    SetupL, T_DisplayCallSetup_CheckIfNEUsedBeforeSettingVisibilityFalseL, Teardown)

/*    
// Cannot run because call AknLayoutUtils::PenEnabled()( called by CPhoneState::CaptureKeysDuringCallNotificationL)
// call crash this case.
/*EUNIT_TEST(
    "T_UpdateSingleActiveCallL_VoIPcallL",
    "",
    "T_UpdateSingleActiveCallL_VoIPcallL",
    "FUNCTIONALITY",
    SetupWithVoipL, T_UpdateSingleActiveCallL_VoIPcallL, Teardown) */
/*
EUNIT_TEST(
    "T_HandleCommandL_CallsPhoneCustomization_L",
    "",
    "T_HandleCommandL_CallsPhoneCustomization_L",
    "FUNCTIONALITY",
    SetupWithVoipL, T_HandleCommandL_CallsPhoneCustomization_L, Teardown)
    
EUNIT_TEST(
    "T_HandlePhoneEngineMessage_EPEMessageCallSecureStatusChanged_L",
    "",
    "T_HandlePhoneEngineMessage_EPEMessageCallSecureStatusChanged_L",
    "FUNCTIONALITY",
    SetupWithVoipL, T_HandlePhoneEngineMessage_EPEMessageCallSecureStatusChanged_L, Teardown)

EUNIT_TEST(
    "T_HandlePhoneEngineMessage_CallsPhoneCustomization_L",
    "",
    "T_HandlePhoneEngineMessage_CallsPhoneCustomization_L",
    "FUNCTIONALITY",
    SetupWithVoipL, T_HandlePhoneEngineMessage_CallsPhoneCustomization_L, Teardown)

EUNIT_TEST(
    "T_ShowNumberBusyNoteL_CallsCustomizeBusyNoteText_L",
    "",
    "T_ShowNumberBusyNoteL_CallsCustomizeBusyNoteText_L",
    "FUNCTIONALITY",
    SetupWithVoipL, T_ShowNumberBusyNoteL_CallsCustomizeBusyNoteText_L, Teardown)
    
EUNIT_TEST(
    "T_PhoneNumberFromEntryLC",
    "",
    "T_PhoneNumberFromEntryLC",
    "FUNCTIONALITY",
    SetupWithVoipL, T_PhoneNumberFromEntryLC, Teardown)
    
EUNIT_TEST(
    "T_UpdateRemoteInfoDataL",
    "",
    "T_UpdateRemoteInfoDataL",
    "FUNCTIONALITY",
    SetupWithVoipL, T_UpdateRemoteInfoDataL, Teardown)
*/

EUNIT_END_TEST_TABLE
