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

#include "ut_cphoneconferenceandwaiting.h"
#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <featmgr.h>
#include "CPhoneConferenceAndWaiting.h"
#include "PEVirtualEngine.h" 
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "CPhoneMainResourceResolver.h"
#include "CPhoneStateMachineGSM.h"
#include "mock_cphoneviewcommandhandle.h"
#include "TPhoneCmdParamCallHeaderData.h"
#include "TPhoneCmdParamAudioOutput.h"
#include "phonerssbase.h"
#include "phonestatedefinitionsgsm.h"
#include "TPhoneCmdParamInteger.h"
#include "TPhoneCmdParamCallStateData.h"

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
    return ETrue;
    }

// - Construction -----------------------------------------------------------

ut_cphoneconferenceandwaiting* ut_cphoneconferenceandwaiting::NewL()
    {
    ut_cphoneconferenceandwaiting* self = ut_cphoneconferenceandwaiting::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphoneconferenceandwaiting* ut_cphoneconferenceandwaiting::NewLC()
    {
    ut_cphoneconferenceandwaiting* self = new( ELeave ) ut_cphoneconferenceandwaiting();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphoneconferenceandwaiting::~ut_cphoneconferenceandwaiting()
    {
    CSmcMockContext::Release();
    }

ut_cphoneconferenceandwaiting::ut_cphoneconferenceandwaiting()
    {
    }

void ut_cphoneconferenceandwaiting::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphoneconferenceandwaiting::SetupL(  )
    {
    iMockContext->InitializeL();
    
    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
        SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );
    
    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();
    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock );
    
    iMockContext->ExpectCallL( "TPhoneCmdParamCallStateData::CallId" ).
        ReturnsL( 2 );
    iStateConferenceAndWaiting = CPhoneConferenceAndWaiting::NewL(
            iStateMachineMock, iViewCmdHandleMock, NULL );

    }

void ut_cphoneconferenceandwaiting::Setup_NoRingingCallIdL(  )
    {
    iMockContext->InitializeL();
    
    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
        SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );
    
    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();
    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock );
    
    iMockContext->ExpectCallL( "TPhoneCmdParamCallStateData::CallId" ).
        ReturnsL( -1 );
    iStateConferenceAndWaiting = CPhoneConferenceAndWaiting::NewL(
            iStateMachineMock, iViewCmdHandleMock, NULL );
    }
    

void ut_cphoneconferenceandwaiting::Teardown(  )
    {

    delete iStateConferenceAndWaiting;
        iStateConferenceAndWaiting = NULL;
    delete iStateMachineMock;
        iStateMachineMock = NULL;
    delete iViewCmdHandleMock;
        iViewCmdHandleMock = NULL;
    delete iCPhoneMainResourceResolverMock;
        iCPhoneMainResourceResolverMock = NULL;
        
    SmcDefaultValue<CPhoneMainResourceResolver*>::Reset();    
    
    iMockContext->Reset();
    }
    
// -----------------------------------------------------------------------------
// Test construction
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandwaiting::T_ConstructionL()
    {
    // SetupL and Teardown does the test.
    }

// -----------------------------------------------------------------------------
// Precontition: Conference and waiting calls
// Use case: User answer waiting call, waiting call connected
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandwaiting::T_HandleConnected_1L(  )
    {           
    iMockContext->ExpectCallL( "CPhoneState::UpdateCallHeaderAndUiCommandsL" ).
        WithL<TInt> ( 2 );
    
    iMockContext->ExpectCallL( "CPhoneStateMachine::ChangeState" ).
        WithL<TInt> ( EPhoneStateConferenceAndSingle );

    iStateConferenceAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnected, 2 );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// Precontition: Conference(with 3 calls) and waiting calls
// Use case: Go one to one with conference member
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandwaiting::T_HandleConnected_2L(  )
    {
    iMockContext->ExpectCallL( "CPhoneState::UpdateCallHeader" ).
        WithL<TInt> ( 0 );

    iStateConferenceAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnected, 0 );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// Precontition: Conference and waiting calls
// Use case: idle message and no confecence and no active calls
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandwaiting::T_HandleIdleL_1()
    {
    //No conference call
    iMockContext->ExpectCallL( "TPhoneCmdParamBoolean::Boolean" ).
        ReturnsL( EFalse );
    
    //Active call count
    iMockContext->ExpectCallL( "TPhoneCmdParamInteger::Integer" ).
        ReturnsL( 0 );
    
    iMockContext->ExpectCallL( "CPhoneStateMachine::ChangeState" ).
        WithL<TInt> ( EPhoneStateIncoming );
    
    iStateConferenceAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// Precontition: Conference and waiting calls
// Use case: idle message and no confecence and one active call
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandwaiting::T_HandleIdleL_2()
    {
    //No conference call
    iMockContext->ExpectCallL( "TPhoneCmdParamBoolean::Boolean" ).
        ReturnsL( EFalse );
    
    //Active call count
    iMockContext->ExpectCallL( "TPhoneCmdParamInteger::Integer" ).
        ReturnsL( 1 );
    
    iMockContext->ExpectCallL( "CPhoneStateMachine::ChangeState" ).
        WithL<TInt> ( EPhoneStateWaitingInSingle );
    
    iStateConferenceAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// Precontition: Conference and waiting calls
// Use case: idle message and no confecence and two active call
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandwaiting::T_HandleIdleL_3()
    {
    //No conference call
    iMockContext->ExpectCallL( "TPhoneCmdParamBoolean::Boolean" ).
        ReturnsL( EFalse );
    
    //Active call count
    iMockContext->ExpectCallL( "TPhoneCmdParamInteger::Integer" ).
        ReturnsL( 2 );
    
    iMockContext->ExpectCallL( "CPhoneStateMachine::ChangeState" ).
        WithL<TInt> ( EPhoneStateTwoSinglesAndWaiting );
    
    iStateConferenceAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }


// -----------------------------------------------------------------------------
// Precontition: Conference and waiting calls
// Use case: idle message for waiting call
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandwaiting::T_HandleIdleL_4()
    {
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL<TPhoneViewCommandId, TInt> ( EPhoneViewRemoveCallHeader, 2 );
    
    iMockContext->ExpectCallL( "CPhoneStateMachine::ChangeState" ).
        WithL<TInt> ( EPhoneStateConference );
    
    iStateConferenceAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 2 );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// Precontition: Conference and waiting calls
// Use case: User give command private
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandwaiting::T_HandleWentOneToOne_1L()
    {
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL<TPhoneViewCommandId, TInt> ( EPhoneViewPrivateFromConference, 0 );
    
    //Conference call exsist
    iMockContext->ExpectCallL( "TPhoneCmdParamBoolean::Boolean" ).
        ReturnsL( ETrue );
    
    iMockContext->ExpectCallL( "CPhoneStateMachine::ChangeState" ).
        WithL<TInt> ( EPhoneStateConferenceAndSingleAndWaiting );
    
    iStateConferenceAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageWentOneToOne, 0 );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphoneconferenceandwaiting,
    "Add test suite description here.",
    "UNIT" )
    
EUNIT_TEST(
    "0- Test construction",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_ConstructionL, Teardown)
       
EUNIT_TEST(
    "1- Test handle connecting",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleConnected_1L, Teardown)

EUNIT_TEST(
    "2- Test handle connecting",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleConnected_2L, Teardown)
    
EUNIT_TEST(
    "3- Test handle idle",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleL_1, Teardown)
    
EUNIT_TEST(
    "4- Test handle idle",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleL_2, Teardown)
    
EUNIT_TEST(
    "5- Test handle idle",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleL_3, Teardown)

EUNIT_TEST(
    "6- Test handle idle",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleL_4, Teardown)
    
EUNIT_TEST(
    "7- Test handle went one to one",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleWentOneToOne_1L, Teardown)

EUNIT_END_TEST_TABLE
