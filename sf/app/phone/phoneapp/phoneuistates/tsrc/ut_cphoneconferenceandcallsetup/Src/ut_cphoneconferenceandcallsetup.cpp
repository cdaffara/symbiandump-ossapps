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

#include "ut_cphoneconferenceandcallsetup.h"

#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <featmgr.h>
#include "CPhoneConferenceAndCallSetup.h"
#include "PEVirtualEngine.h" 
#include "CPhoneViewControllerStub.h"
#include "CPhoneStateMachineStub.h"
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "CPhoneMainResourceResolver.h"
#include "phonestatedefinitionsgsm.h"
#include "tphonecmdparamcallheaderdata.h"

// - Construction -----------------------------------------------------------

ut_cphoneconferenceandcallsetup* ut_cphoneconferenceandcallsetup::NewL()
    {
    ut_cphoneconferenceandcallsetup* self = ut_cphoneconferenceandcallsetup::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphoneconferenceandcallsetup* ut_cphoneconferenceandcallsetup::NewLC()
    {
    ut_cphoneconferenceandcallsetup* self = new( ELeave ) ut_cphoneconferenceandcallsetup();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphoneconferenceandcallsetup::~ut_cphoneconferenceandcallsetup()
    {
    CSmcMockContext::Release();
    }

ut_cphoneconferenceandcallsetup::ut_cphoneconferenceandcallsetup()
    {
    }

void ut_cphoneconferenceandcallsetup::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphoneconferenceandcallsetup::SetupL(  )
    {
    iMockContext->InitializeL();
    iViewCommandHandle = CPhoneViewController::NewL();
    iStateMachine = CPhoneStateMachineStub::NewL(); 
    iViewCommandHandle->iCallId = 2; 
    iStateConferenceAndCallSetup  = CPhoneConferenceAndCallSetup::NewL(
           iStateMachine, iViewCommandHandle, NULL );
    
    }
    

void ut_cphoneconferenceandcallsetup::Teardown(  )
    {
    delete CPhoneMainResourceResolver::Instance();
    delete iStateConferenceAndCallSetup;
    delete iViewCommandHandle;
    delete iStateMachine;
    iMockContext->Reset();
    }
    

// -----------------------------------------------------------------------------
// Test  conference and waiting state in connecting 
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandcallsetup::T_HandleConnectedL(  )
    {   
    iMockContext->Reset();
    iMockContext->ExpectCallL( "CPhoneState::UpdateCallHeaderAndUiCommandsL" ).
             WithL<TInt> ( 2 );
    iStateConferenceAndCallSetup->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnected, 2 );

    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// Test  idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandcallsetup::T_HandleIdleL()
    {
    
    iViewCommandHandle->SetViewGetCountOfActiveCalls(1);
    
    iStateConferenceAndCallSetup->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );
    
    iViewCommandHandle->VerifyViewCommand( EPhoneViewRemoveCallHeader );
        
    
    }

// -----------------------------------------------------------------------------
// Test  conference idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandcallsetup::T_HandleConferenceIdleL( )
    {
          
    iViewCommandHandle->SetViewGetCountOfActiveCalls(1);
       
    iStateConferenceAndCallSetup->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConferenceIdle, 0 );
    
    iViewCommandHandle->VerifyViewCommand( EPhoneViewRemoveConferenceBubble );
    
    
    }

// -----------------------------------------------------------------------------
// Test  conference call setup and zero send
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandcallsetup::T_HandleConferenceAndAlertingL( )
    {
          
    iViewCommandHandle->SetViewGetCountOfActiveCalls(2);
    iViewCommandHandle->SetViewGetCallExistsInConference(ETrue);
 
    iStateConferenceAndCallSetup->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnecting, 0 );   
    iStateConferenceAndCallSetup->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );
    
    if( iStateMachine->iState != EPhoneStateAlertingInSingle )
        {
        EUNIT_FAIL_TEST("Wrong State!");
        }
    }

// -----------------------------------------------------------------------------
// Test  conference call setup and zero send
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneconferenceandcallsetup::T_HandleConferenceAndCallSetupL( )
    {
          
    iViewCommandHandle->SetViewGetCountOfActiveCalls(2);
    iViewCommandHandle->SetViewGetCallExistsInConference(ETrue);
       
    iStateConferenceAndCallSetup->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIdle, 0 );
    
    if( iStateMachine->iState != EPhoneStateCallSetupInSingle )
        {
        EUNIT_FAIL_TEST("Wrong State!");
        }
    
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphoneconferenceandcallsetup,
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
    
EUNIT_TEST(
    "5- Test Conference And Alerting",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleConferenceAndAlertingL, Teardown)    
    
EUNIT_TEST(
    "6- Test Conference And Call Setup",
    "",
    "Test0",
    "FUNCTIONALITY",
    SetupL, T_HandleConferenceAndCallSetupL, Teardown)      
    

EUNIT_END_TEST_TABLE
