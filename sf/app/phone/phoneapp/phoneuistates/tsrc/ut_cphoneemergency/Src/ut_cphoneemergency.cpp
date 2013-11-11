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
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <featmgr.h>
#include "ut_cphoneemergency.h"
#include "CPhoneMainResourceResolver.h"
#include "CPhoneStateMachineGSM.h"
#include "mock_cphoneviewcommandhandle.h"
#include "PEVirtualEngine.h"
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "TPhoneCmdParamCallHeaderData.h"
#include "TPhoneCmdParamAudioOutput.h"
#include "cpeengineinfoimpl.h"
#include "phonerssbase.h"
#include "phonestatedefinitionsgsm.h"
#include "TPhoneCmdParamInteger.h"
#include "CPhoneEmergency.h"

// needed for mocking CPhoneMainResourceResolver::Instance()
class CPhoneMainResourceResolverMock : public CPhoneMainResourceResolver
    {
    };

// -----------------------------------------------------------------------------
// operator for comparing parameter expected and result value.
// -
// -----------------------------------------------------------------------------
//
TBool operator==(const TPhoneCommandParam& param1, const TPhoneCommandParam& param2 )
    {
    TBool ret( EFalse );

    if ( param1.ParamId() == param2.ParamId() )
        {
        if ( TPhoneCommandParam::EPhoneParamIdBoolean == param1.ParamId() )
            {
            const TPhoneCmdParamBoolean  &value1 = static_cast<const TPhoneCmdParamBoolean&> (param1);
            const TPhoneCmdParamBoolean  &value2 = static_cast<const TPhoneCmdParamBoolean&> (param2);
            if (  value1.Boolean() == value2.Boolean() )
                {
                ret = ETrue;
                }
            }
        else if ( TPhoneCommandParam::EPhoneParamIdCallHeaderData == param1.ParamId() )
            {
            const TPhoneCmdParamCallHeaderData  &value1 = static_cast<const TPhoneCmdParamCallHeaderData&> (param1);
            const TPhoneCmdParamCallHeaderData  &value2 = static_cast<const TPhoneCmdParamCallHeaderData&> (param2);
            if ( value1.CallState() == value2.CallState() )
                {
                ret = ETrue;
                }
            }
        else if ( TPhoneCommandParam::EPhoneParamIdInteger == param1.ParamId() )
            {
            const TPhoneCmdParamInteger  &value1 = static_cast<const TPhoneCmdParamInteger&> (param1);
            const TPhoneCmdParamInteger  &value2 = static_cast<const TPhoneCmdParamInteger&> (param2);
            if ( value1.Integer() == value2.Integer() )
                {
                ret = ETrue;
                }
            }
        else if ( TPhoneCommandParam::EPhoneParamIdAudioOutput == param1.ParamId() )
            {
            const TPhoneCmdParamAudioOutput  &value1 = static_cast<const TPhoneCmdParamAudioOutput&> (param1);
            const TPhoneCmdParamAudioOutput  &value2 = static_cast<const TPhoneCmdParamAudioOutput&> (param2);
            if ( value1.AudioOutput() == value2.AudioOutput() )
                {
                ret = ETrue;
                }
            }
        else
            {
            ret = ETrue;  // just paramiId compared
            }
        }
    return ret;
    }

// - Construction -----------------------------------------------------------

ut_cphoneemergency* ut_cphoneemergency::NewL()
    {
    ut_cphoneemergency* self = ut_cphoneemergency::NewLC();
    CleanupStack::Pop();
    return self;
    }

    ut_cphoneemergency* ut_cphoneemergency::NewLC()
    {
    ut_cphoneemergency* self = new( ELeave ) ut_cphoneemergency();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphoneemergency::~ut_cphoneemergency()
    {
    CSmcMockContext::Release();
    }

ut_cphoneemergency::ut_cphoneemergency()
    {
    }

void ut_cphoneemergency::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphoneemergency::SetupL(  )
    {
    iMockContext->InitializeL();

    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
          SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );

    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();

    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock );

    iStateEmergency  = CPhoneEmergency::NewL(
            iStateMachineMock, iViewCmdHandleMock, NULL );

    iEngineInfoMock = CPEEngineInfoImpl::NewL();
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );

    SmcDefaultValue<TPESimState>::SetL( EPESimUsable );
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPENotActive );

    }


void ut_cphoneemergency::Teardown(  )
    {
    delete iViewCmdHandleMock;
        iViewCmdHandleMock = NULL;
    delete iStateEmergency;
        iStateEmergency = NULL;
    delete iStateMachineMock;
        iStateMachineMock = NULL;
    delete iEngineInfoMock;
        iEngineInfoMock = NULL;
    delete iCPhoneMainResourceResolverMock;
        iCPhoneMainResourceResolverMock = NULL;

    SmcDefaultValue<TPESimState>::Reset();
    SmcDefaultValue<const TPEAudioOutput&>::Reset();

    iMockContext->Reset();
    }


// -----------------------------------------------------------------------------
// Test emergency state in connecting
// -check commands
// -----------------------------------------------------------------------------
//
void ut_cphoneemergency::T_HandleConnectingL(  )
    {
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL( EPhoneViewRemoveGlobalNote );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL( EPhoneViewRemoveNote );

    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
       WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewSetGlobalNotifiersDisabled, globalNotifierParam );
    
    TInt callID(0);
    iMockContext->ExpectCallL( "CPhoneState::UpdateCallHeaderAndUiCommandsL" ).
        WithL<TInt> ( callID );

    iStateEmergency->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnecting, 0 );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );

    }
// -----------------------------------------------------------------------------
// ut_cphoneemergency::T_HandleDialingL
// -check commands
// -----------------------------------------------------------------------------
//
void ut_cphoneemergency::T_HandleDialingL()
    {
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( ETrue );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewSetGlobalNotifiersDisabled, globalNotifierParam );

    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL<TPhoneViewCommandId> ( EPhoneViewBringPhoneAppToForeground );
    
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL( EPhoneViewRemoveQuery );

    iStateEmergency->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageDialing, KEmergencyCallId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// ut_cphoneemergency::T_HandleConnectedL
// -check commands
// -----------------------------------------------------------------------------
//
void ut_cphoneemergency::T_HandleConnectedL(  )
    {
    
    TInt callID(0);
    iMockContext->ExpectCallL( "CPhoneState::UpdateCallHeaderAndUiCommandsL" ).
        WithL<TInt> ( callID );

    //first callsetup runs connecting before connected
    iStateEmergency->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnected, 0 );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// ut_cphoneemergency::T_HandleIdleEmergencyL
// -check commands
// -----------------------------------------------------------------------------
//
void ut_cphoneemergency::T_HandleIdleEmergencyL(  )
    {
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewSetGlobalNotifiersDisabled, globalNotifierParam );

   iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL( EPhoneViewRemoveCallHeader, KEmergencyCallId );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL( EPhoneViewRemoveNote );

    iMockContext->ExpectCallL( "CPhoneState::IsSimStateNotPresentWithSecurityModeEnabled" ).
            TimesL(2).
            ReturnsL(EFalse);

    iMockContext->ExpectCallL( "CPhoneState::RemoveDialogsAndSendPhoneToBackgroundL" ).
        TimesL(1); // Set this zero if not expected to call   


    iStateEmergency->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, KEmergencyCallId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );

    }

// -----------------------------------------------------------------------------
// ut_cphoneemergency::T_HandleAudioOutputChangedL
// -check commands
// -----------------------------------------------------------------------------
//
void ut_cphoneemergency::T_HandleAudioOutputChangedL(  )
    {
    TPhoneCmdParamAudioOutput outputParam;
    outputParam.SetAudioOutput( EPEHandset );

    iMockContext->ExpectCallL( "CPEEngineInfoImpl::AudioOutput" ).
        ReturnsL( EPEHandset );

    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewActivateAudioPathUIChanges, outputParam );

    iStateEmergency->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageAudioOutputChanged, KEmergencyCallId );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }
 
// -----------------------------------------------------------------------------
// ut_cphoneemergency::EmergencyCallSetup_HandleEPEMessageConnectedL
// -check commands
// -----------------------------------------------------------------------------
// 
void ut_cphoneemergency::T_EmergencyCallSetup_HandleEPEMessageConnectedL()
    {
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL<TPhoneViewCommandId> ( EPhoneViewRemoveGlobalNote );
    
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewSetGlobalNotifiersDisabled, globalNotifierParam );
    
    iMockContext->ExpectCallL( "CPhoneState::UpdateCallHeaderAndUiCommandsL" ).
        WithL<TInt> ( 0 );
        
    iMockContext->ExpectCallL( "CPhoneState::SetToolbarButtonLoudspeakerEnabled" ).
            TimesL(1);

    
    //first callsetup runs connecting before connected
    iStateEmergency->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnected, 0 );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// ut_cphoneemergency::T_HandleIdleEmergencyWithNeOpenL
// Handle idle when number entry is open
// Expected results: - Number entry is shown
//                   - State is changed to idle
// -----------------------------------------------------------------------------
//
void ut_cphoneemergency::T_HandleIdleEmergencyWithNeOpenL(  )
    {
    iMockContext->ExpectCallL( "CPhoneState::IsSimOk" ).
        ReturnsL( ETrue );    
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
                ReturnsL( ETrue );
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::SetNumberEntryVisibilityL" ).
                WithL(ETrue);
            
    iMockContext->ExpectCallL( "CPhoneState::RemoveDialogsAndSendPhoneToBackgroundL" );
    
    iMockContext->ExpectCallL( "CPhoneStateMachine::ChangeState" ).
        WithL( EPhoneStateIdle );
    
    iStateEmergency->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, KEmergencyCallId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );

    }

// -----------------------------------------------------------------------------
// ut_cphoneemergency::T_HandleKeyEventSimNotUsableL
// Use case: Handle key event during emergency call when sim not inserted
// Expected results:  Number entry and keys are shown 
// Motivation: EMQN-7Q883L Can not input any numbers to number entry during an 
//                         active emergency call in offline mode
// -----------------------------------------------------------------------------
 
void ut_cphoneemergency::T_HandleKeyEventSimNotUsableL()
    {
    SmcDefaultValue<TPESimState>::SetL( EPESimNotPresent );
    TKeyEvent aKeyEvent;
    TEventCode aEventCode;
    iMockContext->ExpectCallL( "CPhoneState::HandleKeyEventL" ).
        WithL( aKeyEvent, aEventCode );
    MPhoneKeyEvents* keyEmergency  = iStateEmergency;
    
    keyEmergency->HandleKeyEventL( aKeyEvent,aEventCode );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );

    }

// -----------------------------------------------------------------------------
// ut_cphoneemergency::T_HandleOtherIdleThanEmergency
// Use case: Emergency call created from active call which idle comes to 
//           Emergency state
// Expected results:  No idle screen 
// Motivation: ELVW-7Q85X5 Emergency call view is not displayed 
//             when making an Emergency call in Log application with active call.
// -----------------------------------------------------------------------------
 
void ut_cphoneemergency::T_HandleOtherIdleThanEmergency ()
    {
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
           WithL( EPhoneViewRemoveAllCallHeaders );
    iMockContext->ExpectCallL("CPhoneState::SetupIdleScreenInBackgroundL")
        .TimesL(0); // not expected to call
    
    iStateEmergency->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 1 );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// ut_cphoneemergency::T_HandleIdleEmergencySecurityNoteShownSimlessOfflineOffL
// -check commands
// TRLM-7Q3HVZ 
// -----------------------------------------------------------------------------
// 
void ut_cphoneemergency::T_HandleIdleEmergencySecurityNoteShownSimlessOfflineOffL(  )
    {
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewSetGlobalNotifiersDisabled, globalNotifierParam );

   iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL( EPhoneViewRemoveCallHeader, KEmergencyCallId );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL( EPhoneViewRemoveNote );

    iMockContext->ExpectCallL( "CPhoneState::IsSimStateNotPresentWithSecurityModeEnabled" ).
            TimesL(2).
            ReturnsL(ETrue);
    iMockContext->ExpectCallL( "FeatureManager::FeatureSupported" ).
                TimesL(1).
                ReturnsL(EFalse);
    iMockContext->ExpectCallL( "CPhoneState::RemoveDialogsAndSendPhoneToBackgroundL" ).
        TimesL(0); // Set this zero if not expected to call

    iStateEmergency->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, KEmergencyCallId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// ut_cphoneemergency::T_HandleIdleEmergencySecurityNoteShownSimlessOfflineOnL
// -check commands
// TRLM-7Q3HVZ 
// -----------------------------------------------------------------------------
// 
void ut_cphoneemergency::T_HandleIdleEmergencySecurityNoteShownSimlessOfflineOnL(  )
    {    
        
    iMockContext->ExpectCallL( "CPhoneState::IsSimStateNotPresentWithSecurityModeEnabled" ).
            TimesL(2).
            ReturnsL(ETrue);
    
    iMockContext->ExpectCallL( "CPhoneState::StartShowSecurityNoteL" ).
            TimesL(1);
    
    iMockContext->ExpectCallL( "FeatureManager::FeatureSupported" ).
                TimesL(1).
                ReturnsL(ETrue);
    
    iMockContext->ExpectCallL( "CPhoneState::RemoveDialogsAndSendPhoneToBackgroundL" ).
        TimesL(1); 
        
    iStateEmergency->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, KEmergencyCallId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// ut_cphoneemergency::T_HandleIdleEmergencySecurityNoteNotShownSimlessOfflineOffL
// -check commands
// TRLM-7Q3HVZ 
// -----------------------------------------------------------------------------
// 
void ut_cphoneemergency::T_HandleIdleEmergencySecurityNoteNotShownSimlessOfflineOffL(  )
    {    
    iMockContext->ExpectCallL( "CPhoneState::IsSimStateNotPresentWithSecurityModeEnabled" ).
            TimesL(2).
            ReturnsL(EFalse);
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
                   ReturnsL( EFalse );
    
    iMockContext->ExpectCallL( "FeatureManager::FeatureSupported" ).  
                TimesL(0);               
    
    iMockContext->ExpectCallL( "CPhoneState::RemoveDialogsAndSendPhoneToBackgroundL" ).
        TimesL(1); 

    iStateEmergency->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, KEmergencyCallId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// ut_cphoneemergency::T_HandleIdleEmergencySecurityNoteNotShownSimlessOfflineOnL
// -check commands
// TRLM-7Q3HVZ 
// -----------------------------------------------------------------------------
// 
void ut_cphoneemergency::T_HandleIdleEmergencySecurityNoteNotShownSimlessOfflineOnL(  )
    {    
    iMockContext->ExpectCallL( "CPhoneState::IsSimStateNotPresentWithSecurityModeEnabled" ).
            TimesL(2).
            ReturnsL(ETrue);
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
                       ReturnsL( EFalse );
    
    iMockContext->ExpectCallL( "FeatureManager::FeatureSupported" ).
                    TimesL(1).
                    ReturnsL(ETrue);
    
    iMockContext->ExpectCallL( "CPhoneState::RemoveDialogsAndSendPhoneToBackgroundL" ).
        TimesL(1); 

    iStateEmergency->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, KEmergencyCallId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }


// -----------------------------------------------------------------------------
// ut_cphoneemergency::T_HandleRemConCommand_MuteL
// -check commands
// ELVW-7RTC22
// -----------------------------------------------------------------------------
// 
void ut_cphoneemergency::T_HandleRemConCommand_MuteL(  )
    {
    MPhoneState* stateEmergency = iStateEmergency;
    iMockContext->ExpectCallL( "CPhoneState::HandleRemConCommandL" ).
            TimesL(0);
    
    stateEmergency->HandleRemConCommandL( ERemConCoreApiMute, ERemConCoreApiButtonPress ); 
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
        ut_cphoneemergency,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "1- Test handle connecting",
    "CPhoneEmergency",
    "Test1",
    "FUNCTIONALITY",
    SetupL, T_HandleConnectingL, Teardown)

EUNIT_TEST(
    "2- test handle dialing ",
    "CPhoneEmergency",
    "Test2",
    "FUNCTIONALITY",
    SetupL, T_HandleDialingL, Teardown)

EUNIT_TEST(
    "3- test handle connected",
    "CPhoneEmergency",
    "Test3",
    "FUNCTIONALITY",
    SetupL, T_HandleConnectedL, Teardown)

EUNIT_TEST(
    "4- test handle idle",
    "CPhoneEmergency",
    "Test4",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleEmergencyL, Teardown)

EUNIT_TEST(
    "5- Audio output change",
    "CPhoneEmergency",
    "Test5",
    "FUNCTIONALITY",
    SetupL, T_HandleAudioOutputChangedL, Teardown)

EUNIT_TEST(
    "8- EmergencyCallSetup_HandleEPEMessageConnected",
    "CPhoneEmergency",
    "Test8",
    "FUNCTIONALITY",
    SetupL, T_EmergencyCallSetup_HandleEPEMessageConnectedL, Teardown)  

EUNIT_TEST(
    "9- To idle, NE open",
    "CPhoneEmergency",
    "Test9",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleEmergencyWithNeOpenL, Teardown)

EUNIT_TEST(
    "10- Key event, SIM not usable",
    "CPhoneEmergency",
    "Test10",
    "FUNCTIONALITY",
    SetupL, T_HandleKeyEventSimNotUsableL, Teardown)

EUNIT_TEST(
    "11- Other call idle",
    "CPhoneEmergency",
    "Test11",
    "FUNCTIONALITY",
    SetupL, T_HandleOtherIdleThanEmergency, Teardown)
    
EUNIT_TEST(
    "13- Handle Idle message when security note is SHOWN and KFeatureIdFfSimlessOfflineSupport flag is UNDEF",
    "CPhoneEmergency",
    "Test13",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleEmergencySecurityNoteShownSimlessOfflineOffL, Teardown)
    
EUNIT_TEST(
    "14- Handle Idle message when security note is SHOWN and KFeatureIdFfSimlessOfflineSupport flag is DEFINED",
    "CPhoneEmergency",
    "Test14",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleEmergencySecurityNoteShownSimlessOfflineOnL, Teardown)

EUNIT_TEST(
    "15- Handle Idle message when security note is NOT SHOWN and KFeatureIdFfSimlessOfflineSupport flag is UNDEF",
    "CPhoneEmergency",
    "Test15",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleEmergencySecurityNoteNotShownSimlessOfflineOffL, Teardown)
    
EUNIT_TEST(
    "16- Handle Idle message when security note is NOT SHOWN and KFeatureIdFfSimlessOfflineSupport flag is DEFINED",
    "CPhoneEmergency",
    "Test16",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleEmergencySecurityNoteNotShownSimlessOfflineOnL, Teardown)
    
EUNIT_TEST(
    "17- Handle Remote Control ERemConCoreApiMute operation ",
    "CPhoneEmergency",
    "Test17",
    "FUNCTIONALITY",
    SetupL, T_HandleRemConCommand_MuteL, Teardown)
    
    

EUNIT_END_TEST_TABLE

