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

#include "ut_cphonestateincall.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include "CPhoneStateMachineGSM.h"
#include "mock_cphoneviewcommandhandle.h"
#include "CPEEngineInfo.h"
#include "cphonestateincall.h"
#include "CPhoneCenRepProxy.h"
#include "CPhoneMainResourceResolver.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include "phoneui.hrh"
#include <csmcmockcontext.h>
#include <msmcmockspecbuilder.h>
#include <coemain.h>
#include <PhoneUI.rsg>
#include "phoneresourceids.h"
#include "phonerssbase.h"
#include "CPhoneTerminateAllConnectionsCommand.h"

// needed for mocking CPhoneMainResourceResolver::Instance()
class CPhoneMainResourceResolverMock : public CPhoneMainResourceResolver{};
CPhoneCenRepProxy* GlobalCPhoneCenRepProxy;
HBufC* GlobalHBufC;

// - Construction -----------------------------------------------------------

ut_cphonestateincall* ut_cphonestateincall::NewL()
    {
    ut_cphonestateincall* self = ut_cphonestateincall::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphonestateincall* ut_cphonestateincall::NewLC()
    {
    ut_cphonestateincall* self = new( ELeave ) ut_cphonestateincall();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonestateincall::~ut_cphonestateincall()
    {
    CSmcMockContext::Release();
    }

ut_cphonestateincall::ut_cphonestateincall()
    {
    }

void ut_cphonestateincall::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------

void ut_cphonestateincall::SetupL(  )
    {
    GlobalCPhoneCenRepProxy = NULL;
    GlobalHBufC = NULL;
    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
    SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );
    
    iCoeEnvMock = new (ELeave ) CCoeEnv();
    SmcDefaultValue<CCoeEnv*>::SetL( iCoeEnvMock );
    
    iViewCommandHandle = new CPhoneViewCommandHandleMock();

    iStateMachine = CPhoneStateMachineGSM::NewL( iViewCommandHandle );
    
    iEngineInfoMock = CPEEngineInfo::NewL();
        SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );
        
    iStateInCall =  CPhoneStateInCall::NewL( iStateMachine, iViewCommandHandle, NULL );
    }

void ut_cphonestateincall::Teardown(  )
    {
    delete iStateInCall;
    iStateInCall = NULL;
    delete iStateMachine;
    iStateMachine = NULL;
    delete iViewCommandHandle;
    iViewCommandHandle = NULL;
    
    delete iEngineInfoMock;
    iEngineInfoMock = NULL;
    SmcDefaultValue<MPEEngineInfo*>::Reset();
    
    delete iCoeEnvMock;
    iCoeEnvMock = NULL;
    SmcDefaultValue<CCoeEnv*>::Reset();
    
    delete GlobalCPhoneCenRepProxy;
    GlobalCPhoneCenRepProxy = NULL;

    delete GlobalHBufC;
    GlobalHBufC = NULL;
    delete iCPhoneMainResourceResolverMock;
    iCPhoneMainResourceResolverMock = NULL;
    SmcDefaultValue<CPhoneMainResourceResolver*>::Reset();
    
    iMockContext->Reset();
    }
    
void ut_cphonestateincall::T_ConstructionL(  )
    {
    // SetupL and Teardown does the test.
    }

void ut_cphonestateincall::T_NumberAcqCmdVideoCall(  )
    {
    _LIT( KPhoneNumber, "12345" );
    HBufC* phoneNumber = KPhoneNumber().AllocL();
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
                  ReturnsL(phoneNumber);
    
    iStateInCall->HandleCommandL( EPhoneNumberAcqCmdVideoCall );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonestateincall::T_HandleStoppedDTMFL(  )
    {         
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::DtmfString" ).
                      ReturnsL(KNullDesC);
    
    iMockContext->ExpectCallL( "CPhoneMainResourceResolver::ResolveResourceID" ).
               WithL( EPhoneDtmfNumberQuery ).
               ReturnsL(R_PHONEUI_SEND_DTMF_QUERY);
                
    iMockContext->ExpectCallL( "CPhoneMainResourceResolver::ResolveResourceID" ).
                   WithL( EPhoneSendDtmfEmptyEditBoxCBA ).
                   ReturnsL(R_PHONEUI_SEND_DTMF_EMPTY_EDIT_BOX_CBA);
   
    iMockContext->ExpectCallL( "CPhoneMainResourceResolver::ResolveResourceID" ).
                   WithL( EPhoneSendDtmfNormalEditBoxCBA ).
                   ReturnsL(R_PHONEUI_SEND_DTMF_NORMAL_EDIT_BOX_CBA);
            
    iStateInCall->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageStoppedDTMF, -1 );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphonestateincall,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "Test0",
    "T_ConstructionL",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_ConstructionL, Teardown)

EUNIT_TEST(
    "Test1",
    "EPhoneNumberAcqCmdVideoCall",
    "Test1",
    "FUNCTIONALITY",
    SetupL, T_NumberAcqCmdVideoCall, Teardown)
    
EUNIT_TEST(
    "Test2",
    "T_HandleStoppedDTMFL",
    "Test2",
    "FUNCTIONALITY",
    SetupL, T_HandleStoppedDTMFL, Teardown)
    
EUNIT_END_TEST_TABLE
