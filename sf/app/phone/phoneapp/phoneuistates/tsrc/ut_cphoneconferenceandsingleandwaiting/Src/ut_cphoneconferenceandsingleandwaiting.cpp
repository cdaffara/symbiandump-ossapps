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

#include "ut_cphoneconferenceandsingleandwaiting.h"
#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <featmgr.h>
#include "CPhoneConferenceAndSingleAndWaiting.h"
#include "PEVirtualEngine.h" 
#include "CPhoneViewControllerStub.h"
#include "CPhoneStateMachineStub.h"
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "CPhoneMainResourceResolver.h"

// - Construction -----------------------------------------------------------

ut_cphoneconferenceandsingleandwaiting* ut_cphoneconferenceandsingleandwaiting::NewL()
    {
    ut_cphoneconferenceandsingleandwaiting* self = ut_cphoneconferenceandsingleandwaiting::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphoneconferenceandsingleandwaiting* ut_cphoneconferenceandsingleandwaiting::NewLC()
    {
    ut_cphoneconferenceandsingleandwaiting* self = new( ELeave ) ut_cphoneconferenceandsingleandwaiting();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphoneconferenceandsingleandwaiting::~ut_cphoneconferenceandsingleandwaiting()
    {
    CSmcMockContext::Release();
    }

ut_cphoneconferenceandsingleandwaiting::ut_cphoneconferenceandsingleandwaiting()
    {
    }

void ut_cphoneconferenceandsingleandwaiting::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphoneconferenceandsingleandwaiting::SetupL(  )
    {
    iMockContext->InitializeL();
    iViewCommandHandle = CPhoneViewController::NewL();
    iStateMachine = CPhoneStateMachineStub::NewL(); 
    iViewCommandHandle->iCallId = 2; 
    iStateConferenceAndSingleAndWaiting  = CPhoneConferenceAndSingleAndWaiting::NewL(
           iStateMachine, iViewCommandHandle, NULL );
    
    }
    

void ut_cphoneconferenceandsingleandwaiting::Teardown(  )
    {
    delete CPhoneMainResourceResolver::Instance();
    delete iStateConferenceAndSingleAndWaiting;
    delete iViewCommandHandle;
    delete iStateMachine;
    iMockContext->Reset();
    }
    

// -----------------------------------------------------------------------------
// Test  conference and waiting state in connecting 
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandsingleandwaiting::T_HandleConnectedL(  )
    {
    iMockContext->Reset();
    iMockContext->ExpectCallL( "CPhoneState::UpdateCallHeaderAndUiCommandsL" ).
             WithL<TInt> ( 2 );
    iStateConferenceAndSingleAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnected, 2 );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// Test  idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandsingleandwaiting::T_HandleIdleL()
    {
    
    iViewCommandHandle->SetViewGetCountOfActiveCalls(1);
    
    iStateConferenceAndSingleAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );
    
    iViewCommandHandle->VerifyViewCommand( EPhoneViewRemoveCallHeader );
        
    
    }

// -----------------------------------------------------------------------------
// Test  conference idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandsingleandwaiting::T_HandleConferenceIdleL( )
    {
          
    iViewCommandHandle->SetViewGetCountOfActiveCalls(1);
       
    iStateConferenceAndSingleAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConferenceIdle, 0 );
    
    iViewCommandHandle->VerifyViewCommand( EPhoneViewRemoveConferenceBubble );
    
    
    }


// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
        ut_cphoneconferenceandsingleandwaiting,
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
