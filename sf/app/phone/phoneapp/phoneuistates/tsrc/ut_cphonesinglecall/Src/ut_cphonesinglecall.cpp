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
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <featmgr.h>
#include "ut_cphonesinglecall.h"
#include "CPhoneMainResourceResolver.h"
#include "CPhoneStateMachineGSM.h"
#include "mock_cphoneviewcommandhandle.h"
#include "PEVirtualEngine.h"
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "TPhoneCmdParamCallHeaderData.h"
#include "TPhoneCmdParamAudioOutput.h"
#include "cpeengineinfoimpl.h"
#include "phonerssbase.h"
#include "phonestatedefinitionsgsm.h"
#include "TPhoneCmdParamInteger.h"
#include "CPhoneSingleCall.h"

// needed for mocking CPhoneMainResourceResolver::Instance()
class CPhoneMainResourceResolverMock : public CPhoneMainResourceResolver
    {
    };

// - Construction -----------------------------------------------------------

ut_cphonesinglecall* ut_cphonesinglecall::NewL()
    {
    ut_cphonesinglecall* self = ut_cphonesinglecall::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphonesinglecall* ut_cphonesinglecall::NewLC()
    {
    ut_cphonesinglecall* self = new( ELeave ) ut_cphonesinglecall();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonesinglecall::~ut_cphonesinglecall()
    {
    CSmcMockContext::Release();
    }

ut_cphonesinglecall::ut_cphonesinglecall()
    {
    }

void ut_cphonesinglecall::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphonesinglecall::SetupL(  )
    {
    iMockContext->InitializeL();

    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
          SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );

    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();

    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock );

    iStateSingleCall  = CPhoneSingleCall::NewL(
            iStateMachineMock, iViewCmdHandleMock, NULL );

    iEngineInfoMock = CPEEngineInfoImpl::NewL();
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );

    SmcDefaultValue<TPESimState>::SetL( EPESimUsable );
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPENotActive );

    }

void ut_cphonesinglecall::Teardown(  )
    {
    delete iViewCmdHandleMock;
        iViewCmdHandleMock = NULL;
    delete iStateSingleCall;
        iStateSingleCall = NULL;
    delete iStateMachineMock;
        iStateMachineMock = NULL;
    delete iEngineInfoMock;
        iEngineInfoMock = NULL;
    delete iCPhoneMainResourceResolverMock;
        iCPhoneMainResourceResolverMock = NULL;

    SmcDefaultValue<TPESimState>::Reset();
    SmcDefaultValue<const TPEAudioOutput&>::Reset();

    iMockContext->Reset();
    }

// -----------------------------------------------------------------------------
// Test  conference idle
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphonesinglecall::T_HandleConnectedL( )
    {
    const TInt callId( 0 );
    iMockContext->ExpectCallL("CPhoneState::UpdateCallHeaderAndUiCommandsL").
        WithL<TInt>(callId);
        
    iStateSingleCall->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnected, callId );   
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
     
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphonesinglecall,
    "ut_cphonesinglecall",
    "UNIT" )

EUNIT_TEST(
    "2. HandleConnectedL",
    "",
    "HandleConnectedL",
    "FUNCTIONALITY",
    SetupL, T_HandleConnectedL, Teardown)    
    
EUNIT_END_TEST_TABLE

 
