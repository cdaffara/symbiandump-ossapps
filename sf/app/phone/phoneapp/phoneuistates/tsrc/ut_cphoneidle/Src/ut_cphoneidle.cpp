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

#include "ut_cphoneidle.h"
#include "cphoneidle_inherited.h"

#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include "CPhoneIdle.h"
#include <featmgr.h>
#include "PEVirtualEngine.h" 
#include "CPhoneViewControllerStub.h"
#include "CPhoneStateMachineStub.h"
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "CPhoneMainResourceResolver.h"
#include "phonerssbase.h"


// - Construction -----------------------------------------------------------

ut_cphoneidle* ut_cphoneidle::NewL()
    {
    ut_cphoneidle* self = ut_cphoneidle::NewLC();
    CleanupStack::Pop();
    return self;
    }
    
ut_cphoneidle* ut_cphoneidle::NewLC()
    {
    ut_cphoneidle* self = new( ELeave ) ut_cphoneidle();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphoneidle::~ut_cphoneidle()
    {
    CSmcMockContext::Release();
    }

ut_cphoneidle::ut_cphoneidle()
    {
    }

void ut_cphoneidle::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphoneidle::SetupL(  )
    {
    iMockContext->InitializeL();
        
    iViewCommandHandle = CPhoneViewController::NewL();//TRect() );
    iStateMachineMock = CPhoneStateMachineStub::NewL();
    
    iStateIdle  = CPhoneIdle::NewL(
            iStateMachineMock, iViewCommandHandle, NULL );
    
    iIdleInherited = CPhoneIdle_Inherited::NewL(
        iStateMachineMock, iViewCommandHandle, NULL);
    
    }
    

void ut_cphoneidle::Teardown(  )
    {
    delete CPhoneMainResourceResolver::Instance();
    delete iStateIdle;
    delete iIdleInherited;
    delete iViewCommandHandle;
    delete iStateMachineMock;
    
    iMockContext->Reset();
    }
    

void ut_cphoneidle::T_ConstructionL(  )
    {
    //empty
    }


void ut_cphoneidle::T_HandlePhoneEngineMessageL(  )
    {
    TInt callId(1);
    iMockContext->ExpectCallL( "CPhoneState::SendGlobalInfoNoteL" ).
            WithL<TPhoneViewCommandId, TBool> ( EPhoneInformationRemotePutOnHoldNote, ETrue );
    iStateIdle->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageRemoteHeld, callId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    
    iMockContext->ExpectCallL( "CPhoneState::SendGlobalInfoNoteL" ).
            WithL<TPhoneViewCommandId, TBool> ( EPhoneInformationConnectedNote, ETrue );
    iStateIdle->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageRemoteResumed, callId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    
    iMockContext->ExpectCallL( "CPhoneState::HandleAudioOutputChangedL" ).TimesL(1);
    iStateIdle->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageAudioOutputChanged, callId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
        
    iMockContext->ExpectCallL( "CPhoneGeneralGsmMessagesHandler::NewL" );
    iMockContext->ExpectCallL( "CPhoneGeneralGsmMessagesHandler::HandlePhoneEngineMessageL" );
    iStateIdle->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIssuedSSRequest, callId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    
    iMockContext->ExpectCallL( "CPhoneStateIdle::HandlePhoneEngineMessageL" ).
            WithL<TInt, TInt> ( MEngineMonitor::EPEMessageSecureNotSpecified, callId );
    iStateIdle->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageSecureNotSpecified, callId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphoneidle::T_HandleErrorL(  )
    {
    TPEErrorInfo aErrorInfo;
    aErrorInfo.iErrorCode = ECCPErrorCCUserAlertingNoAnswer;
    iMockContext->ExpectCallL( "CPhoneState::HandleErrorL" ).WithL<TPEErrorInfo>( aErrorInfo );
    iIdleInherited->User_HandleErrorL( aErrorInfo );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphoneidle,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "Construction",
    "ut_cphoneidle",
    "test1",
    "FUNCTIONALITY",
    SetupL, T_ConstructionL, Teardown)
    
EUNIT_TEST(
    "T_HandlePhoneEngineMessageL",
    "ut_cphoneidle",
    "test2",
    "FUNCTIONALITY",
    SetupL, T_HandlePhoneEngineMessageL, Teardown)
    
EUNIT_TEST(
    "T_HandleErrorL",
    "ut_cphoneidle",
    "test3",
    "FUNCTIONALITY",
    SetupL, T_HandleErrorL, Teardown)

EUNIT_END_TEST_TABLE
