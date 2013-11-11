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
#include "CPhoneMainResourceResolver.h"
#include "CPhoneStateMachineGSM.h"
#include "mock_cphoneviewcommandhandle.h"
#include "PEVirtualEngine.h"
#include "MPhoneKeyEvents.h"
#include "TPhoneCommandParam.h"
#include "TPhoneCmdParamCallHeaderData.h"
#include "TPhoneCmdParamAudioOutput.h"
#include "cpeengineinfo.h"
#include "phonerssbase.h"
#include "phonestatedefinitionsgsm.h"
#include "TPhoneCmdParamInteger.h"
#include "TPhoneCmdParamCallStateData.h"
#include "TelephonyVariant.hrh"

#include "CPhoneIncoming.h"
#include "ut_cphoneincoming.h"

// needed for mocking CPhoneMainResourceResolver::Instance()
class CPhoneMainResourceResolverMock : public CPhoneMainResourceResolver
    {
    };

// -----------------------------------------------------------------------------
// operator for comparing parameter expected and result value.
// -
// -----------------------------------------------------------------------------
//
TBool operator==(const TPhoneCommandParam& param1, const TPhoneCommandParam& param2 )
    {
    TBool ret( ETrue );

    if ( param1.ParamId() == param2.ParamId() )
        {
    
        if ( TPhoneCommandParam::EPhoneParamIdInteger == param1.ParamId() )
            {
            const TPhoneCmdParamInteger  &value1 = static_cast<const TPhoneCmdParamInteger&> (param1);
            const TPhoneCmdParamInteger  &value2 = static_cast<const TPhoneCmdParamInteger&> (param2);
            if ( value1.Integer() != value2.Integer() )
                {
                ret = EFalse;
                }
            }
        else
            {
            ret = ETrue;  // just paramiId compared
            }
        }
    return ret;
    }


// - Construction -----------------------------------------------------------

ut_cphoneincoming* ut_cphoneincoming::NewL()
    {
    ut_cphoneincoming* self = ut_cphoneincoming::NewLC();
    CleanupStack::Pop();
    return self;
    }
    
ut_cphoneincoming* ut_cphoneincoming::NewLC()
    {
    ut_cphoneincoming* self = new( ELeave ) ut_cphoneincoming();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphoneincoming::~ut_cphoneincoming()
    {
    CSmcMockContext::Release();
    }

ut_cphoneincoming::ut_cphoneincoming()
    {
    }

void ut_cphoneincoming::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphoneincoming::SetupL(  )
    {
    iMockContext->InitializeL();

    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
          SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );

    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();

    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock );

    iStateIncoming  = CPhoneIncoming::NewL(
                iStateMachineMock, iViewCmdHandleMock, NULL );

    iEngineInfoMock = CPEEngineInfo::NewL();
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );

    SmcDefaultValue<TPESimState>::SetL( EPESimUsable );
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPENotActive );

    }


void ut_cphoneincoming::Teardown(  )
    {
    delete iViewCmdHandleMock;
        iViewCmdHandleMock = NULL;
    delete iStateIncoming;
        iStateIncoming = NULL;
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
// Test ss request
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneincoming::T_HandleWaiting()
    {     
    iMockContext->ExpectCallL( "TPhoneCmdParamCallStateData::CallId" ).
        ReturnsL( 1 );
    iMockContext->ExpectCallL( "CPhoneStateMachine::ChangeState" ).
        WithL( EPhoneStateWaitingInSingle );

    iStateIncoming->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIncoming, 2 ); 
    iStateIncoming->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnected,1 ); 
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
        
    }

// -----------------------------------------------------------------------------
// Test error handling
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneincoming::T_HandleError_IsNotForwardedToBaseClassL()
    {
    TPEErrorInfo aErrorInfo;
    aErrorInfo.iErrorCode = ECCPErrorCCUserAlertingNoAnswer;
    iMockContext->ExpectCallL( "CPhoneState::HandleErrorL" ).
        TimesL( 0 );
     
    iStateIncoming->HandleErrorL( aErrorInfo );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// -----------------------------------------------------------------------------
// Test error handling
// -check commands
// -----------------------------------------------------------------------------
//  
void ut_cphoneincoming::T_HandleError_ForwardedToBaseClassL()
    {
    TPEErrorInfo aErrorInfo;
    aErrorInfo.iErrorCode = ECCPErrorCCCallRejected;
    iMockContext->ExpectCallL( "CPhoneState::HandleErrorL" ).
        TimesL( 1 );
     
    iStateIncoming->HandleErrorL( aErrorInfo );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }


// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphoneincoming,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "1- Test waiting",
    "",
    "Test1",
    "FUNCTIONALITY",
    SetupL, T_HandleWaiting, Teardown)
    
EUNIT_TEST(
    "2- Test error",
    "",
    "Test2",
    "FUNCTIONALITY",
    SetupL, T_HandleError_IsNotForwardedToBaseClassL, Teardown)

EUNIT_TEST(
    "3- Test error",
    "",
    "Test3",
    "FUNCTIONALITY",
    SetupL, T_HandleError_ForwardedToBaseClassL, Teardown)
    
    
EUNIT_END_TEST_TABLE
