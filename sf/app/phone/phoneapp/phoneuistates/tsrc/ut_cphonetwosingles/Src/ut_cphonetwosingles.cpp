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

#include "ut_cphonetwosingles.h"
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
#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>
#include <smcmacros.h>
#include <msmcmockaction.h>
#include "phonestatedefinitions.h"

#define protected public
#include "CPhoneTwoSingles.h"

// - Construction -----------------------------------------------------------

ut_cphonetwosingles* ut_cphonetwosingles::NewL()
    {
    ut_cphonetwosingles* self = ut_cphonetwosingles::NewLC();
    CleanupStack::Pop();
    return self;
    }
    
    ut_cphonetwosingles* ut_cphonetwosingles::NewLC()
    {
    ut_cphonetwosingles* self = new( ELeave ) ut_cphonetwosingles();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonetwosingles::~ut_cphonetwosingles()
    {
    CSmcMockContext::Release();
    }

ut_cphonetwosingles::ut_cphonetwosingles()
    {
    }

void ut_cphonetwosingles::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphonetwosingles::SetupL(  )
    {
    iMockContext->InitializeL();
    
    iViewCommandHandle = CPhoneViewController::NewL();
    iStateMachine = CPhoneStateMachineStub::NewL(); 
    iViewCommandHandle->iCallId = 2; 
    
    iStateTwoSingles  = CPhoneTwoSingles::NewL(
           iStateMachine, iViewCommandHandle, NULL );
    
    }
    

void ut_cphonetwosingles::Teardown(  )
    {
    delete CPhoneMainResourceResolver::Instance();
    delete iStateTwoSingles;
    delete iViewCommandHandle;
    delete iStateMachine;
    
    iMockContext->Reset();
    }
   
// -----------------------------------------------------------------------------
// Test  conference idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphonetwosingles::T_HandleIdleL( )
    {
    
    iViewCommandHandle->SetViewGetCountOfActiveCalls(1);

    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::SetVisibilityIfNumberEntryUsedL" ).
            WithL( ETrue );
    iStateTwoSingles->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );

    EUNIT_ASSERT_EQUALS( EPhoneStateSingle, iStateMachine->iState ); 
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
        ut_cphonetwosingles,
    "Add test suite description here.",
    "UNIT" )
    
EUNIT_TEST(
    "1- CPhoneTwoSingles: Test idle",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleL, Teardown)    
    
EUNIT_END_TEST_TABLE
