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

#include "ut_cphonetwosinglesandwaiting.h"
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
#include "CPhoneTwoSinglesAndWaiting.h"

// - Construction -----------------------------------------------------------

ut_cphonetwosinglesandwaiting* ut_cphonetwosinglesandwaiting::NewL()
    {
    ut_cphonetwosinglesandwaiting* self = ut_cphonetwosinglesandwaiting::NewLC();
    CleanupStack::Pop();
    return self;
    }
    
    ut_cphonetwosinglesandwaiting* ut_cphonetwosinglesandwaiting::NewLC()
    {
    ut_cphonetwosinglesandwaiting* self = new( ELeave ) ut_cphonetwosinglesandwaiting();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonetwosinglesandwaiting::~ut_cphonetwosinglesandwaiting()
    {
    }

ut_cphonetwosinglesandwaiting::ut_cphonetwosinglesandwaiting()
    {
    }

void ut_cphonetwosinglesandwaiting::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------

void ut_cphonetwosinglesandwaiting::SetupL(  )
    {
    iViewCommandHandle = CPhoneViewController::NewL();
    iStateMachine = CPhoneStateMachineStub::NewL(); 
     
    iStateTwoSinglesAndWaiting  = CPhoneTwoSinglesAndWaiting::NewL(
           iStateMachine, iViewCommandHandle, NULL );
    
    }
    

void ut_cphonetwosinglesandwaiting::Teardown(  )
    {
    delete CPhoneMainResourceResolver::Instance();
    delete iStateTwoSinglesAndWaiting;
    delete iViewCommandHandle;
    delete iStateMachine;
    }
    
// -----------------------------------------------------------------------------
// Test  conference idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphonetwosinglesandwaiting::T_HandleIdleL( )
    {
    
    iViewCommandHandle->SetViewGetCountOfActiveCalls(1);
    
    iStateTwoSinglesAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );
   
    iViewCommandHandle->VerifyViewCommand( EPhoneViewRemoveCallHeader );
          
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
        ut_cphonetwosinglesandwaiting,
    "Add test suite description here.",
    "UNIT" )
    
EUNIT_TEST(
    "1- CPhoneTwoSinglesAndWaiting: Test idle",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleL, Teardown)    
    
EUNIT_END_TEST_TABLE
