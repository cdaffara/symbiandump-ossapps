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

#include "ut_cphoneconferenceandsingle.h"
#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <featmgr.h>
#include "CPhoneConferenceAndSingle.h"
#include "PEVirtualEngine.h" 
#include "CPhoneViewControllerStub.h"
#include "CPhoneStateMachineStub.h"
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "CPhoneMainResourceResolver.h"
#include "cphonestate.h"

// - Construction -----------------------------------------------------------

ut_cphoneconferenceandsingle* ut_cphoneconferenceandsingle::NewL()
    {
    ut_cphoneconferenceandsingle* self = ut_cphoneconferenceandsingle::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphoneconferenceandsingle* ut_cphoneconferenceandsingle::NewLC()
    {
    ut_cphoneconferenceandsingle* self = new( ELeave ) ut_cphoneconferenceandsingle();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphoneconferenceandsingle::~ut_cphoneconferenceandsingle()
    {
    CSmcMockContext::Release();
    }

ut_cphoneconferenceandsingle::ut_cphoneconferenceandsingle()
    {
    }

void ut_cphoneconferenceandsingle::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphoneconferenceandsingle::SetupL(  )
    {
    iMockContext->InitializeL();
    iViewCommandHandle = CPhoneViewController::NewL();
    iStateMachine = CPhoneStateMachineStub::NewL(); 
    iViewCommandHandle->iCallId = 2; 
    iStateConferenceAndSingle  = CPhoneConferenceAndSingle::NewL(
           iStateMachine, iViewCommandHandle, NULL );
    
    }
    

void ut_cphoneconferenceandsingle::Teardown(  )
    {
    delete CPhoneMainResourceResolver::Instance();
    delete iStateConferenceAndSingle;
    delete iViewCommandHandle;
    delete iStateMachine;
    iMockContext->Reset();
    }
    

// -----------------------------------------------------------------------------
// Test  conference and waiting state in connecting 
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandsingle::T_HandleConnectedL(  )
    {
    iMockContext->Reset();
    iMockContext->ExpectCallL( "CPhoneState::UpdateCallHeaderAndUiCommandsL" ).
             WithL<TInt> ( 2 );
    iStateConferenceAndSingle->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnected, 2 );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// Test  idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandsingle::T_HandleIdleL()
    {
    
    iViewCommandHandle->SetViewGetCountOfActiveCalls(1);
    
    iStateConferenceAndSingle->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );
    
    iViewCommandHandle->VerifyViewCommand( EPhoneViewRemoveCallHeader );
        
    
    }

// -----------------------------------------------------------------------------
// Test  conference idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandsingle::T_HandleConferenceIdleL( )
    {
          
    iViewCommandHandle->SetViewGetCountOfActiveCalls(1);
       
    iStateConferenceAndSingle->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConferenceIdle, 0 );
    
    iViewCommandHandle->VerifyViewCommand( EPhoneViewRemoveConferenceBubble );
    
    
    }


// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphoneconferenceandsingle,
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
