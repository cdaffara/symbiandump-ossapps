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

#include "ut_cphonecallsetupandwaiting.h"

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
#include "phoneappcommands.hrh"

#define protected public
#include "CPhoneCallSetupAndWaiting.h"

// - Construction -----------------------------------------------------------

ut_cphonecallsetupandwaiting* ut_cphonecallsetupandwaiting::NewL()
    {
    ut_cphonecallsetupandwaiting* self = ut_cphonecallsetupandwaiting::NewLC();
    CleanupStack::Pop();
    return self;
    }
    
    ut_cphonecallsetupandwaiting* ut_cphonecallsetupandwaiting::NewLC()
    {
    ut_cphonecallsetupandwaiting* self = new( ELeave ) ut_cphonecallsetupandwaiting();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonecallsetupandwaiting::~ut_cphonecallsetupandwaiting()
    {
    CSmcMockContext::Release();
    }

ut_cphonecallsetupandwaiting::ut_cphonecallsetupandwaiting()
    {
    }

void ut_cphonecallsetupandwaiting::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphonecallsetupandwaiting::SetupL(  )
    {
    iMockContext->InitializeL();
    iViewCommandHandle = CPhoneViewController::NewL();
    iStateMachine = CPhoneStateMachineStub::NewL(); 
     
    iStateCallSetupAndWaiting  = CPhoneCallSetupAndWaiting::NewL(
           iStateMachine, iViewCommandHandle, NULL );
    }
    

void ut_cphonecallsetupandwaiting::Teardown(  )
    {
    delete CPhoneMainResourceResolver::Instance();
    delete iStateCallSetupAndWaiting;
    delete iViewCommandHandle;
    delete iStateMachine;
    iMockContext->Reset();
    }

// -----------------------------------------------------------------------------
// Test  conference idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphonecallsetupandwaiting::T_HandleIdleL( )
    {
    
    iViewCommandHandle->SetViewGetCountOfActiveCalls(1);
    
    iStateCallSetupAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );
   
    iViewCommandHandle->VerifyViewCommand( EPhoneViewRemoveCallHeader );
          
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
        ut_cphonecallsetupandwaiting,
    "Add test suite description here.",
    "UNIT" )
    
EUNIT_TEST(
    "2- CPhoneTwoSinglesAndWaiting: Test idle",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleL, Teardown)    
    
EUNIT_END_TEST_TABLE
