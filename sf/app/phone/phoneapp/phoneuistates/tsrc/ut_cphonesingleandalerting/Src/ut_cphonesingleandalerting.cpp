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

#include "ut_cphonesingleandalerting.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <featmgr.h>
#include "CPhoneSingleAndAlerting.h"
#include "PEVirtualEngine.h" 
#include "CPhoneViewControllerStub.h"
#include "CPhoneStateMachineStub.h"
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "CPhoneMainResourceResolver.h"


// - Construction -----------------------------------------------------------

ut_cphonesingleandalerting* ut_cphonesingleandalerting::NewL()
    {
    ut_cphonesingleandalerting* self = ut_cphonesingleandalerting::NewLC();
    CleanupStack::Pop();
    return self;
    }
    
    ut_cphonesingleandalerting* ut_cphonesingleandalerting::NewLC()
    {
    ut_cphonesingleandalerting* self = new( ELeave ) ut_cphonesingleandalerting();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonesingleandalerting::~ut_cphonesingleandalerting()
    {
    }

ut_cphonesingleandalerting::ut_cphonesingleandalerting()
    {
    }

void ut_cphonesingleandalerting::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphonesingleandalerting::SetupL(  )
    {
    iViewCommandHandle = CPhoneViewController::NewL();
    iStateMachine = CPhoneStateMachineStub::NewL(); 
    iViewCommandHandle->iCallId = 2; 
    
    iStateSingleAndAlerting  = CPhoneSingleAndAlerting::NewL(
           iStateMachine, iViewCommandHandle, NULL );
    
    }
    

void ut_cphonesingleandalerting::Teardown(  )
    {
    delete CPhoneMainResourceResolver::Instance();
    delete iStateSingleAndAlerting;
    delete iViewCommandHandle;
    delete iStateMachine;
    }
 
// -----------------------------------------------------------------------------
// Test  conference idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphonesingleandalerting::TestCallHeaderIsRemovedWhenCallGoesIdleL( )
    {
    
    iViewCommandHandle->SetViewGetCountOfActiveCalls(1);
    
    iStateSingleAndAlerting->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );
   
    iViewCommandHandle->VerifyViewCommand(EPhoneViewRemoveCallHeader);
          
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
        ut_cphonesingleandalerting,
    "Add test suite description here.",
    "UNIT" )
     
EUNIT_TEST(
    "1- CPhoneSingleAndAlerting: Test idle",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, TestCallHeaderIsRemovedWhenCallGoesIdleL, Teardown)    
    
EUNIT_END_TEST_TABLE
