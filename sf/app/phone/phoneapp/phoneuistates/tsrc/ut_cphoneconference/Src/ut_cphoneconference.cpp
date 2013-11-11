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

#include "ut_cphoneconference.h"

#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <featmgr.h>
#include "PEVirtualEngine.h" 
#include "CPhoneViewControllerStub.h"
#include "CPhoneStateMachineStub.h"
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "CPhoneMainResourceResolver.h"
#include "tphonecmdparamcallheaderdata.h"

#define protected public
#include "CPhoneConference.h"

// - Construction -----------------------------------------------------------

ut_cphoneconference* ut_cphoneconference::NewL()
    {
    ut_cphoneconference* self = ut_cphoneconference::NewLC();
    CleanupStack::Pop();
    return self;
    }
    
    ut_cphoneconference* ut_cphoneconference::NewLC()
    {
    ut_cphoneconference* self = new( ELeave ) ut_cphoneconference();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphoneconference::~ut_cphoneconference()
    {
    CSmcMockContext::Release();
    }

ut_cphoneconference::ut_cphoneconference()
    {
    }

void ut_cphoneconference::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphoneconference::SetupL(  )
    {
    iMockContext->InitializeL();
    iViewCommandHandle = CPhoneViewController::NewL();
    iStateMachine = CPhoneStateMachineStub::NewL(); 
    iViewCommandHandle->iCallId = 2; 
    iStateConference  = CPhoneConference::NewL(
           iStateMachine, iViewCommandHandle, NULL );
    
    }
    

void ut_cphoneconference::Teardown(  )
    {
    delete CPhoneMainResourceResolver::Instance();
    delete iStateConference;
    delete iViewCommandHandle;
    delete iStateMachine;
    iMockContext->Reset();
    }
    

// -----------------------------------------------------------------------------
// Test  conference and waiting state in connecting 
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconference::T_HandleConnectedL(  )
    {
    iMockContext->Reset();
    iMockContext->ExpectCallL( "CPhoneState::UpdateCallHeader" ).
             WithL<TInt> ( 2 );
    iStateConference->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnected, 2 );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// Test  idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconference::T_HandleIdleL()
    {
    
    iViewCommandHandle->SetViewGetCountOfActiveCalls(1);
    
    iStateConference->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );
    
    iViewCommandHandle->VerifyViewCommandBoolean( EPhoneViewGetIsConference, EFalse );
     
    }

// -----------------------------------------------------------------------------
// Test  conference idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconference::T_HandleConferenceIdleL( )
    {
      
    iViewCommandHandle->SetViewGetCountOfActiveCalls(2);
       
    iStateConference->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConferenceIdle, 0 );
    
    iViewCommandHandle->VerifyViewCommand( EPhoneViewRemoveConferenceBubble );
    
    iViewCommandHandle->SetViewGetCountOfActiveCalls(0);
    
    iStateConference->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConferenceIdle, 0 );
            
     
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphoneconference,
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
