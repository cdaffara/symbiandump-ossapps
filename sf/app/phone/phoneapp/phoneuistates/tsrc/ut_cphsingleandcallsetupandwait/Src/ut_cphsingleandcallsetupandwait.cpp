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

#include "ut_cphsingleandcallsetupandwait.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <featmgr.h>
#include "CPhoneSingleAndCallSetupAndWaiting.h"
#include "PEVirtualEngine.h" 
#include "CPhoneViewControllerStub.h"
#include "CPhoneStateMachineStub.h"
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "CPhoneMainResourceResolver.h"
#include "phoneappcommands.hrh"


// - Construction -----------------------------------------------------------

ut_cphsingleandcallsetupandwait* ut_cphsingleandcallsetupandwait::NewL()
    {
    ut_cphsingleandcallsetupandwait* self = ut_cphsingleandcallsetupandwait::NewLC();
    CleanupStack::Pop();
    return self;
    }
    
    ut_cphsingleandcallsetupandwait* ut_cphsingleandcallsetupandwait::NewLC()
    {
    ut_cphsingleandcallsetupandwait* self = new( ELeave ) ut_cphsingleandcallsetupandwait();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphsingleandcallsetupandwait::~ut_cphsingleandcallsetupandwait()
    {
    }

ut_cphsingleandcallsetupandwait::ut_cphsingleandcallsetupandwait()
    {
    }

void ut_cphsingleandcallsetupandwait::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphsingleandcallsetupandwait::SetupL(  )
    {
    iViewCommandHandle = CPhoneViewController::NewL();
    iStateMachine = CPhoneStateMachineStub::NewL(); 
     
    iStateSingleAndCallSetupAndWaiting  = CPhoneSingleAndCallSetupAndWaiting::NewL(
           iStateMachine, iViewCommandHandle, NULL );
    
    }
    

void ut_cphsingleandcallsetupandwait::Teardown(  )
    {
    delete CPhoneMainResourceResolver::Instance();
    delete iStateSingleAndCallSetupAndWaiting;
    delete iViewCommandHandle;
    delete iStateMachine;
    }
    
// -----------------------------------------------------------------------------
// Test  conference idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphsingleandcallsetupandwait::T_HandleIdleL( )
    {
    
    iViewCommandHandle->SetViewGetCountOfActiveCalls(1);
    
    iStateSingleAndCallSetupAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );
   
    iViewCommandHandle->VerifyViewCommand( EPhoneViewRemoveCallHeader );
          
    }

// -----------------------------------------------------------------------------
// Test callsetup fail with reason remote busy.
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphsingleandcallsetupandwait::T_RemoteBusyL( )
    {
    // At the moment ut_cphsingleandcallsetupandwait uses cphonestatestub and because of this
    // we cant check/verify EPhoneViewStopCapturingKey command since it is called from cphonestate
    // and because test uses cphonestatestub+cphonestate.h(original) we cant check if CaptureKeysDuringCallNotificationL
    // was called.
    // When this test dll is transfered to use MOCK FW this case can be made fully functional.
    /*iStateSingleAndCallSetupAndWaiting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageRemoteBusy, 0 );
   
    iViewCommandHandle->VerifyViewCommand( EPhoneViewStopCapturingKey ); */
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
     ut_cphsingleandcallsetupandwait,
    "Add test suite description here.",
    "UNIT" )

    
EUNIT_TEST(
    "2- CPhoneSingleAndCallSetupAndWaiting: Test idle",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleIdleL, Teardown )
    
/*EUNIT_TEST(
    "3- CPhoneSingleAndCallSetupAndWaiting: Test remote busy",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_RemoteBusyL, Teardown )  */
    
EUNIT_END_TEST_TABLE
