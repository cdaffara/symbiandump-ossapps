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

#include "ut_cphonestateidle.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include <featmgr.h>
#include <coemain.h>
#include "cpeengineinfo.h"
#include "cphonestatemachinegsm.h"
#include "PEVirtualEngine.h"
#include "mock_cphoneviewcommandhandle.h"
#include "CPhoneStateMachine.h"
#include "MPhoneKeyEvents.h"
#include "CPhoneMainResourceResolver.h"
#include "phoneappcommands.hrh"
#include "cphonecustomizationvoip.h"

#include "CPhoneStateIdle.h"
#include "CPhoneStateIdle_Inherited.h"

#include <csmcmockcontext.h>
#include <msmcmockspecbuilder.h>
#include <MProfileEngine.h>

#include "phonerssbase.h"
#include <PhoneUI.rsg>
#include "TPhoneCmdParamInteger.h"
#include "tphonecmdparamkeycapture.h"


MProfileEngine * CreateProfileEngineL() {}

#include <w32std.h>
void RWsSession::SimulateKeyEvent(TKeyEvent) {}
void RWsSession::Flush() {}
RWsSession::RWsSession() {}

RWindowGroup::RWindowGroup() {}

MWsClientClass::MWsClientClass() {}
RWindowTreeNode::RWindowTreeNode() {}

#include <APGTASK.h>
TApaTaskList::TApaTaskList(RWsSession &aSession ) : iWsSession( aSession ) {}
TApaTask TApaTaskList::FindApp(TUid) {}
TInt TApaTask::Exists() const {return EFalse;}

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
        else
            {
            ret = ETrue;  // just paramiId compared
            }
        }
    return ret;
    }

// - Construction -----------------------------------------------------------

ut_cphonestateidle* ut_cphonestateidle::NewL()
    {
    ut_cphonestateidle* self = ut_cphonestateidle::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphonestateidle* ut_cphonestateidle::NewLC()
    {
    ut_cphonestateidle* self = new( ELeave ) ut_cphonestateidle();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonestateidle::~ut_cphonestateidle()
    {
    CSmcMockContext::Release();
    }

ut_cphonestateidle::ut_cphonestateidle()
    {
    }

void ut_cphonestateidle::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphonestateidle::SetupL(  )
    {
    iMockContext->InitializeL();

    iCoeEnvMock = new (ELeave ) CCoeEnv();
    SmcDefaultValue<CCoeEnv*>::SetL( iCoeEnvMock );

    iEngineInfoMock = CPEEngineInfo::NewL();
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );

    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
    SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );

    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();

    iStateMachineGsmMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock );

    iCustomizationMock =
        CPhoneCustomizationVoip::NewL( *iStateMachineGsmMock, *iViewCmdHandleMock );

    iStateIdle = CPhoneStateIdle_Inherited::NewL(
        iStateMachineGsmMock, iViewCmdHandleMock, iCustomizationMock);
    }


void ut_cphonestateidle::Teardown(  )
    {
    delete iStateIdle;
    iStateIdle = NULL;
    delete iStateMachineGsmMock;
    iStateMachineGsmMock = NULL;
    delete iViewCmdHandleMock;
    iViewCmdHandleMock = NULL;
    delete iCustomizationMock;
    iCustomizationMock = NULL;

    delete iCPhoneMainResourceResolverMock;
    iCPhoneMainResourceResolverMock = NULL;
    SmcDefaultValue<CPhoneMainResourceResolver*>::Reset();

    delete iEngineInfoMock;
    iEngineInfoMock = NULL;
    SmcDefaultValue<MPEEngineInfo*>::Reset();
    iMockContext->Reset();
    delete iCoeEnvMock;
    iCoeEnvMock = NULL;
    SmcDefaultValue<CCoeEnv*>::Reset();
    }


void ut_cphonestateidle::T_IdlePhoneFocusLost_DiallerNotUsedL(  )
    {
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
            ReturnsL( EFalse );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL( EPhoneViewUpdateFSW );
    iStateIdle->HandlePhoneFocusLostEventL();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonestateidle::T_IdlePhoneFocusLost_DiallerUsedL(  )
    {
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
            ReturnsL( ETrue );
    // check that T_IdlePhoneFocusLost_DiallerUsedL is not called.
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL( T_IdlePhoneFocusLost_DiallerUsedL ).
            TimesL(0);
    iStateIdle->HandlePhoneFocusLostEventL();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonestateidle::T_IdleEndKeyPressGeneratesCancelNotificationsL(  )
    {
    iMockContext->ExpectCallL( "CPhoneState::IsAutoLockOn" ).
        ReturnsL( EFalse );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL( EPhoneViewCancelAllNotications ).
        TimesL(1);
    iMockContext->ExpectCallL( "CPhoneState::IsSimOk" ).
        ReturnsL( ETrue ).
        TimesL(1);
    iStateIdle->HandleKeyMessageL( MPhoneKeyEvents::EPhoneKeyLongPress, EKeyNo );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonestateidle::T_HandleKeyMessageL(  )
    {
    _LIT( KPhoneNumber, "12345" );
    HBufC* phoneNumber = KPhoneNumber().AllocLC();
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
        ReturnsL( ETrue ).
        TimesL(2);
    iMockContext->ExpectCallL( "TPhoneCmdParamBoolean::Boolean" ).
        ReturnsL( ETrue );
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
        ReturnsL( phoneNumber );
    iMockContext->ExpectCallL( "CPhoneState::IsSimOk" ).
        ReturnsL( ETrue );

    iStateIdle->HandleKeyMessageL( MPhoneKeyEvents::EPhoneKeyShortPress, EKeyYes );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    CleanupStack::Pop( phoneNumber );
    }

void ut_cphonestateidle::T_HandleKeyMessageL_AppKeyL(  )
    {
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
        ReturnsL( EFalse );
    iMockContext->ExpectCallL( "CPhoneState::HandleCommandL" ).
        WithL( EPhoneNumberAcqSecurityDialer );

    iStateIdle->HandleKeyMessageL( MPhoneKeyEvents::EPhoneKeyShortPress, EKeyApplication0 );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }


void ut_cphonestateidle::T_HandleKeyMessageL_AppKeyL_NEOpen(  )
    {
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
        ReturnsL( ETrue );
    iMockContext->ExpectCallL( "CPhoneState::HandleCommandL" ).
            TimesL( 0 );

    iStateIdle->HandleKeyMessageL( MPhoneKeyEvents::EPhoneKeyShortPress, EKeyApplication0 );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonestateidle::T_HandleCommand_NumberAcqCmdCallL()
    {
    _LIT( KPhoneNumber, "12345" );
    HBufC* phoneNumber = KPhoneNumber().AllocLC();
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
        ReturnsL( ETrue );
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
        ReturnsL( phoneNumber );
    // SIM NOK
    iMockContext->ExpectCallL( "CPhoneState::IsSimOk" ).
        ReturnsL( EFalse );
    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage" ).
        WithL( MPEPhoneModel::EPEMessageCheckEmergencyNumber );
    iStateIdle->HandleCommandL( EPhoneNumberAcqCmdCall );
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
        ReturnsL( ETrue );
    HBufC* phoneNumber2 = KPhoneNumber().AllocLC();
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
        ReturnsL( phoneNumber2 );
    // SIM OK
    iMockContext->ExpectCallL( "CPhoneState::IsSimOk" ).
            ReturnsL( ETrue );
    iMockContext->ExpectCallL( "CPhoneState::DialVoiceCallL" );

    iStateIdle->HandleCommandL( EPhoneNumberAcqCmdCall );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    CleanupStack::Pop( phoneNumber2 );
    CleanupStack::Pop( phoneNumber );
    }

void ut_cphonestateidle::T_HandleCommand_NumberAcqCmdSendCommandL()
    {
    _LIT( KPhoneNumber, "12345" );
    HBufC* phoneNumber = KPhoneNumber().AllocLC();
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
        ReturnsL( ETrue );
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
        ReturnsL( phoneNumber );
    // SIM NOK
    iMockContext->ExpectCallL( "CPhoneState::IsSimOk" ).
        ReturnsL( EFalse );
    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage" ).
        WithL( MPEPhoneModel::EPEMessageCheckEmergencyNumber );
    iStateIdle->HandleCommandL( EPhoneNumberAcqCmdCall );
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
        ReturnsL( ETrue );
    HBufC* phoneNumber2 = KPhoneNumber().AllocLC();
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
        ReturnsL( phoneNumber2 );
    // SIM OK
    iMockContext->ExpectCallL( "CPhoneState::IsSimOk" ).
            ReturnsL( ETrue );
    iMockContext->ExpectCallL( "CPhoneState::DialVoiceCallL" );

    iStateIdle->HandleCommandL( EPhoneNumberAcqCmdSendCommand );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    CleanupStack::Pop( phoneNumber2 );
    CleanupStack::Pop( phoneNumber );
    }

void ut_cphonestateidle::T_DialL()
    {
    _LIT( KPhoneNumber, "23322" );
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetPhoneNumber" ).
        WithL( KPhoneNumber() );
    iMockContext->ExpectCallL( "FeatureManager::FeatureSupported" ).
        WithL( KFeatureIdCsVideoTelephony ).
        ReturnsL( ETrue );

    iStateIdle->DialL_UserL( 
            KPhoneNumber, 
            CPhoneStateIdle::EPhoneNumberTypeVideo,
            CPhoneStateIdle::EDialMethodUnknown );

    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetPhoneNumber" ).
        WithL( KPhoneNumber() );

    iStateIdle->DialL_UserL( 
            KPhoneNumber, 
            CPhoneStateIdle::EPhoneNumberTypeCS,
            CPhoneStateIdle::EDialMethodUnknown );

    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetPhoneNumber" ).
        WithL( KPhoneNumber() );
    iMockContext->ExpectCallL( "CPhoneCustomizationVoip::HandleDialL" ).
        WithL( KPhoneNumber() );

    iStateIdle->DialL_UserL( 
            KPhoneNumber, 
            CPhoneStateIdle::EPhoneNumberTypeVoip,
            CPhoneStateIdle::EDialMethodUnknown );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonestateidle::T_DialVideoCallL()
    {
    _LIT( KPhoneNumber, "123456" );
    HBufC* phoneNumber = KPhoneNumber().AllocLC();
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
            ReturnsL( ETrue );
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
            ReturnsL( phoneNumber );
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetPhoneNumber" ).
            WithL( KPhoneNumber() );
    iMockContext->ExpectCallL( "CPhoneState::DialMultimediaCallL" );

    //DialVideoCallL is used via handlecommand because it's private method.
    iStateIdle->HandleCommandL( EPhoneNumberAcqCmdVideoCall );

    _LIT( KSpeedDialNumber, "2" );
    HBufC* phoneNumber2 = KSpeedDialNumber().AllocLC();
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
           ReturnsL( ETrue );
   iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
           ReturnsL( phoneNumber2 );

    //DialVideoCallL is used via handlecommand because it's private method.
    iStateIdle->HandleCommandL( EPhoneNumberAcqCmdVideoCall );

    iMockContext->ExpectCallL( "CPhoneState::DialMultimediaCallL" ).
        TimesL(0);

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    CleanupStack::Pop( phoneNumber2 );
    CleanupStack::Pop( phoneNumber );
    }

// -----------------------------------------------------------------------------
// ut_cphonestateidle::T_HandleEndKeyPressSecurityNoteShownSimlessOfflineUndefL
// -check commands
// TRLM-7Q3HVZ
// -----------------------------------------------------------------------------
//
void ut_cphonestateidle::T_HandleEndKeyPressSecurityNoteShownSimlessOfflineUndefL(  )
    {
    iMockContext->ExpectCallL( "CPhoneState::IsSimStateNotPresentWithSecurityModeEnabled" ).
            TimesL(1).
            ReturnsL(ETrue);
    iMockContext->ExpectCallL( "FeatureManager::FeatureSupported" ).
            TimesL(1).
            ReturnsL(EFalse);
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryUsedL" ).
               TimesL(1).
               ReturnsL(ETrue);
    // ExecuteCommandL is expected to be called only 2 times because SecutiryNote is shown and
    // SimlessOffline is Undefined.
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            TimesL(2);
    iMockContext->ExpectCallL( "CPhoneState::IsSimOk" ).
            ReturnsL( ETrue ).
            TimesL(1);

    iStateIdle->HandleKeyMessageL( MPhoneKeyEvents::EPhoneKeyLongPress, EKeyNo );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// ut_cphonestateidle::T_HandleEndKeyPressSecurityNoteShownSimlessOfflineDefinedL
// -check commands
// TRLM-7Q3HVZ
// -----------------------------------------------------------------------------
//
void ut_cphonestateidle::T_HandleEndKeyPressSecurityNoteShownSimlessOfflineDefinedL(  )
    {
    iMockContext->ExpectCallL( "CPhoneState::IsSimStateNotPresentWithSecurityModeEnabled" ).
            TimesL(1).
            ReturnsL(ETrue);
    iMockContext->ExpectCallL( "FeatureManager::FeatureSupported" ).
            TimesL(1).
            ReturnsL(ETrue);
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL(EPhoneViewRemovePhoneDialogs);
    iMockContext->ExpectCallL( "CPhoneState::IsSimOk" ).
            ReturnsL( ETrue ).
            TimesL(1);

    iStateIdle->HandleKeyMessageL( MPhoneKeyEvents::EPhoneKeyLongPress, EKeyNo );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// ut_cphonestateidle::T_HandleNumberEntryClearedL
// -check commands
// EAYG-7N4BGE
// -----------------------------------------------------------------------------
//
void ut_cphonestateidle::T_HandleNumberEntryClearedL()
    {
    iMockContext->ExpectCallL( "CPhoneState::IsSimOk" ).
            ReturnsL( ETrue );
    iMockContext->ExpectCallL( "CPhoneStateMachineGSM::State" ).
            ReturnsL<MPhoneState*>( iStateIdle );
    iMockContext->ExpectCallL( "CPhoneState::RemoveDialogsAndSendPhoneToBackgroundL" );

    iStateIdle->HandleNumberEntryClearedL_UserL();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );

    }

// - EUnit test table -------------------------------------------------------
EUNIT_BEGIN_TEST_TABLE(
     ut_cphonestateidle,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "Test0 - T_IdlePhoneFocusLost_DiallerNotUsedL",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_IdlePhoneFocusLost_DiallerNotUsedL, Teardown)
    
EUNIT_TEST(
    "Test1 - T_IdlePhoneFocusLost_DiallerUsedL",
    "",
    "Test1",
    "FUNCTIONALITY",
    SetupL, T_IdlePhoneFocusLost_DiallerUsedL, Teardown)

EUNIT_TEST(
    "Test2 - End key generates cancel notification message",
    "CPhoneIdleState",
    "Test2",
    "FUNCTIONALITY",
    SetupL, T_IdleEndKeyPressGeneratesCancelNotificationsL, Teardown)
    
EUNIT_TEST(
    "Test7 - HandleKeyMessageL",
    "CPhoneIdleState",
    "Test7",
    "FUNCTIONALITY",
    SetupL, T_HandleKeyMessageL, Teardown)

EUNIT_TEST(
    "Test8 - HandleKeyMessageL_AppKeyL",
    "CPhoneIdleState",
    "Test8",
    "FUNCTIONALITY",
    SetupL, T_HandleKeyMessageL_AppKeyL, Teardown )

EUNIT_TEST(
    "Test9 - HandleKeyMessageL_AppKeyL_NEOpen",
    "CPhoneIdleState",
    "Test9",
    "FUNCTIONALITY",
    SetupL, T_HandleKeyMessageL_AppKeyL_NEOpen, Teardown )

EUNIT_TEST(
    "Test10 - HandleCommand_NumberAcqCmdCall",
    "CPhoneIdleState",
    "Test10",
    "FUNCTIONALITY",
    SetupL, T_HandleCommand_NumberAcqCmdCallL, Teardown)

EUNIT_TEST(
    "Test11 - HandleCommand_NumberAcqCmdSendCommand",
    "CPhoneIdleState",
    "Test11",
    "FUNCTIONALITY",
    SetupL, T_HandleCommand_NumberAcqCmdSendCommandL, Teardown)

EUNIT_TEST(
    "Test12 - DialL",
    "CPhoneIdleState",
    "Test12",
    "FUNCTIONALITY",
    SetupL, T_DialL, Teardown)

EUNIT_TEST(
    "Test13 - DialVideoCallL",
    "CPhoneIdleState",
    "Test13",
    "FUNCTIONALITY",
    SetupL, T_DialVideoCallL, Teardown)

EUNIT_TEST(
    "Test14 - HandleEndKeyPressSecurityNoteShownSimlessOfflineUndefL",
    "CPhoneIdleState",
    "Test14",
    "FUNCTIONALITY",
    SetupL, T_HandleEndKeyPressSecurityNoteShownSimlessOfflineUndefL, Teardown)

EUNIT_TEST(
    "Test15 - HandleEndKeyPressSecurityNoteShownSimlessOfflineDefinedL",
    "CPhoneIdleState",
    "Test15",
    "FUNCTIONALITY",
    SetupL, T_HandleEndKeyPressSecurityNoteShownSimlessOfflineDefinedL, Teardown)

EUNIT_TEST(
    "Test16 - HandleNumberEntryClearedL",
    "CPhoneIdleState",
    "Test16",
    "FUNCTIONALITY",
    SetupL, T_HandleNumberEntryClearedL, Teardown)

EUNIT_END_TEST_TABLE
