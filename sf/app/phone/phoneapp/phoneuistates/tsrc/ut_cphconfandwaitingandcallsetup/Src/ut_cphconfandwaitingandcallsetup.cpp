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

#include "ut_cphconfandwaitingandcallsetup.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <featmgr.h>
#include "CPhoneConferenceAndWaitingAndCallSetup.h"
#include "PEVirtualEngine.h" 
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "CPhoneMainResourceResolver.h"

#include <coemain.h>
#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>

#include "cphonestatemachinegsm.h"
#include "mock_cphoneviewcommandhandle.h"

#include "cpeengineinfo.h"
#include "cpeengineinfoimpl.h"

#include "tphonecmdparamcallheaderdata.h"

// needed for mocking CPhoneMainResourceResolver::Instance()
class CPhoneMainResourceResolverMock : public CPhoneMainResourceResolver
    {
    };

// - Construction -----------------------------------------------------------

ut_cphconfandwaitingandcallsetup* ut_cphconfandwaitingandcallsetup::NewL()
    {
    ut_cphconfandwaitingandcallsetup* self = ut_cphconfandwaitingandcallsetup::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphconfandwaitingandcallsetup* ut_cphconfandwaitingandcallsetup::NewLC()
    {
    ut_cphconfandwaitingandcallsetup* self = new( ELeave ) ut_cphconfandwaitingandcallsetup();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphconfandwaitingandcallsetup::~ut_cphconfandwaitingandcallsetup()
    {
    CSmcMockContext::Release();
    }

ut_cphconfandwaitingandcallsetup::ut_cphconfandwaitingandcallsetup()
    {
    }

void ut_cphconfandwaitingandcallsetup::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphconfandwaitingandcallsetup::SetupL(  )
    {
    iMockContext->InitializeL();
    
    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
    SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );
            
    iViewCommandHandleMock = new CPhoneViewCommandHandleMock();
    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCommandHandleMock ); 
    iStateConferenceAndCallSetupAndWaiting  = CPhoneConferenceAndWaitingAndCallSetup::NewL(
           iStateMachineMock, iViewCommandHandleMock, NULL );
    
    iCoeEnvMock = new (ELeave ) CCoeEnv();
    SmcDefaultValue<CCoeEnv*>::SetL( iCoeEnvMock );    
    
    iEngineInfoMock = CPEEngineInfo::NewL();
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );
    }
    

void ut_cphconfandwaitingandcallsetup::Teardown(  )
    {
    delete iCPhoneMainResourceResolverMock;
    iCPhoneMainResourceResolverMock = NULL;
    SmcDefaultValue<CPhoneMainResourceResolver*>::Reset();
    
    delete iStateConferenceAndCallSetupAndWaiting;
    delete iViewCommandHandleMock;
    delete iStateMachineMock;
    
    delete iCoeEnvMock;
    iCoeEnvMock = NULL;
    SmcDefaultValue<CCoeEnv*>::Reset();
    
    delete iEngineInfoMock;
    iEngineInfoMock = NULL;
    SmcDefaultValue<MPEEngineInfo*>::Reset();
    
    iMockContext->Reset();
    }

// -----------------------------------------------------------------------------
// Test  conference and waiting state in connecting 
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphconfandwaitingandcallsetup::T_HandleConnectedL(  )
    {
    TInt callId(2);
    TPhoneCmdParamCallHeaderData callHeaderParam;
    callHeaderParam.SetCallState( EPEStateConnected );
    iMockContext->ExpectCallL( "CPhoneState::UpdateCallHeader" ).
        WithL<TInt> ( callId );
          
    iStateConferenceAndCallSetupAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnected, callId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// Test  idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphconfandwaitingandcallsetup::T_HandleIdleL()
    {
    TInt callId(0);
    TPhoneCmdParamBoolean conferenceExistsForCallId;
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL<TPhoneViewCommandId, TInt, TPhoneCommandParam&> ( 
                EPhoneViewGetCallExistsInConference, 
                callId,
                conferenceExistsForCallId );
               
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
       WithL<TPhoneViewCommandId, TInt>( EPhoneViewRemoveCallHeader, callId );
    
    iStateConferenceAndCallSetupAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

// -----------------------------------------------------------------------------
// Test  conference idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphconfandwaitingandcallsetup::T_HandleConferenceIdleL( )
    {
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
       WithL( EPhoneViewRemoveConferenceBubble );
    
    iStateConferenceAndCallSetupAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConferenceIdle, 0 );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }


// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
        ut_cphconfandwaitingandcallsetup,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "1- Test handle connecting",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleConnectedL, Teardown)
    
EUNIT_TEST(
    "2- Test handle idle",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleL, Teardown)
    
EUNIT_TEST(
    "3- Test handle conference idle",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleConferenceIdleL, Teardown)
    

EUNIT_END_TEST_TABLE
