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
#include "ut_cphonealerting.h"
#include "cphonealerting.h"
#include "cphonemainresourceresolver.h"
#include "cphonestatemachinegsm.h"
#include "mock_cphoneviewcommandhandle.h"
#include "pevirtualengine.h" 
#include "mphonekeyevents.h"
#include "tphonecommandparam.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamaudiooutput.h"
#include "cpeengineinfoimpl.h"
#include "cphoneidle.h"

//test
#include "TPhoneCmdParamInteger.h"

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
    TBool ret( EFalse );
    
    if ( param1.ParamId() == param2.ParamId() )
        {
        if ( TPhoneCommandParam::EPhoneParamIdBoolean == param1.ParamId() )
            {
            const TPhoneCmdParamBoolean  &value1 = static_cast<const TPhoneCmdParamBoolean&> (param1);
            const TPhoneCmdParamBoolean  &value2 = static_cast<const TPhoneCmdParamBoolean&> (param2);
            if (  value1.Boolean() == value2.Boolean() )
                {
                ret = ETrue;
                }
            }
        else if ( TPhoneCommandParam::EPhoneParamIdCallHeaderData == param1.ParamId() )
            {
            const TPhoneCmdParamCallHeaderData  &value1 = static_cast<const TPhoneCmdParamCallHeaderData&> (param1);
            const TPhoneCmdParamCallHeaderData  &value2 = static_cast<const TPhoneCmdParamCallHeaderData&> (param2);
            if ( value1.CallState() == value2.CallState() )
                {
                ret = ETrue;
                }
            }
        else if ( TPhoneCommandParam::EPhoneParamIdInteger == param1.ParamId() )
            {
            const TPhoneCmdParamInteger  &value1 = static_cast<const TPhoneCmdParamInteger&> (param1);
            const TPhoneCmdParamInteger  &value2 = static_cast<const TPhoneCmdParamInteger&> (param2);
            if ( value1.Integer() == value2.Integer() )
                {
                ret = ETrue;
                }
            }
        else if ( TPhoneCommandParam::EPhoneParamIdAudioOutput == param1.ParamId() )
            {
            const TPhoneCmdParamAudioOutput  &value1 = static_cast<const TPhoneCmdParamAudioOutput&> (param1);
            const TPhoneCmdParamAudioOutput  &value2 = static_cast<const TPhoneCmdParamAudioOutput&> (param2);
            if ( value1.AudioOutput() == value2.AudioOutput() )
                {
                ret = ETrue;
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
ut_cphonealerting* ut_cphonealerting::NewL()
    {
    ut_cphonealerting* self = ut_cphonealerting::NewLC();
    CleanupStack::Pop( self );
    return self;
    }
  
ut_cphonealerting* ut_cphonealerting::NewLC()
    {
    ut_cphonealerting* self = new( ELeave ) ut_cphonealerting();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

  
ut_cphonealerting::~ut_cphonealerting()
    {
    CSmcMockContext::Release();
    }


ut_cphonealerting::ut_cphonealerting()
    {
    }


void ut_cphonealerting::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }


// - Test methods -----------------------------------------------------------
void ut_cphonealerting::SetupL(  )
    {
    iMockContext->InitializeL();

    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
          SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );
    
    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();
  
    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock ); 
       
    iStateAlerting  = CPhoneAlerting::NewL(
            iStateMachineMock, iViewCmdHandleMock, NULL );
  
    iEngineInfoMock = CPEEngineInfoImpl::NewL();
    
    iPhoneIdleMock = CPhoneIdle::NewL( 
        iStateMachineMock, iViewCmdHandleMock, NULL );
    
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );
    SmcDefaultValue<TPESimState>::SetL( EPESimStatusUninitialized );
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPENotActive );
    SmcDefaultValue<MPhoneState*>::SetL( iPhoneIdleMock );
    }


void ut_cphonealerting::Teardown(  )
    {
    delete iViewCmdHandleMock;
    iViewCmdHandleMock = NULL;
    delete iStateAlerting;
    iStateAlerting = NULL;
    delete iStateMachineMock;
    iStateMachineMock = NULL;
    delete iEngineInfoMock;
    iEngineInfoMock = NULL;
    delete iCPhoneMainResourceResolverMock;
    iCPhoneMainResourceResolverMock = NULL;
    delete iPhoneIdleMock;
    iPhoneIdleMock = NULL;
    
    SmcDefaultValue<TPESimState>::Reset();
    SmcDefaultValue<const TPEAudioOutput&>::Reset();
    SmcDefaultValue<MPhoneState*>::Reset();
    
    iMockContext->Reset();
    }

// -----------------------------------------------------------------------------
// ut_cphonealerting::T_HandlePhoneEngineMessageIncomingL
// -----------------------------------------------------------------------------
// 
void ut_cphonealerting::T_HandlePhoneEngineMessageIncomingL()
    {
    TInt callIdentifier = 0;
    iMockContext->ExpectCallL( "CPhoneGsmInCall::HandlePhoneEngineMessageL" ).
        WithL( MEngineMonitor::EPEMessageIncoming, callIdentifier );
    
    iStateAlerting->HandlePhoneEngineMessageL( 
        MEngineMonitor::EPEMessageIncoming,
        callIdentifier );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }


// -----------------------------------------------------------------------------
// ut_cphonealerting::T_HandlePhoneEngineMessageIdleL
// -----------------------------------------------------------------------------
//
void ut_cphonealerting::T_HandlePhoneEngineMessageIdleL()
    {
    TInt callIdentifier = 0;
    iMockContext->ExpectCallL( "CPhoneGsmInCall::HandlePhoneEngineMessageL" ).
        WithL( MEngineMonitor::EPEMessageIdle, callIdentifier );
    
    iStateAlerting->HandlePhoneEngineMessageL( 
        MEngineMonitor::EPEMessageIdle,
        callIdentifier );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }


// -----------------------------------------------------------------------------
// ut_cphonealerting::T_HandlePhoneEngineMessageIdleAfterIncomingL
// Motivation for the test case: TSW error TKUA-7VJGL5.
// Incoming message for a new call may be received in alerting state if 
// teardown of the alerting call is ongoing.
// -----------------------------------------------------------------------------
//
void ut_cphonealerting::T_HandlePhoneEngineMessageIdleAfterIncomingL()
    {
    TInt identifierOfAlertingCall = 0;
    TInt identifierOfIncomingCall = 1;
    
    iMockContext->ExpectCallL( "CPhoneGsmInCall::HandlePhoneEngineMessageL" ).
        WithL( MEngineMonitor::EPEMessageIncoming, identifierOfIncomingCall );
    iMockContext->ExpectCallL( "CPhoneGsmInCall::HandlePhoneEngineMessageL" ).
        WithL( MEngineMonitor::EPEMessageIdle, identifierOfAlertingCall );
    iMockContext->ExpectCallL( "CPhoneIdle::HandlePhoneEngineMessageL" ).
        WithL( MEngineMonitor::EPEMessageIncoming, identifierOfIncomingCall );
    
    iStateAlerting->HandlePhoneEngineMessageL( 
        MEngineMonitor::EPEMessageIncoming,
        identifierOfIncomingCall );
    iStateAlerting->HandlePhoneEngineMessageL( 
        MEngineMonitor::EPEMessageIdle,
        identifierOfAlertingCall );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }


// - EUnit test table -------------------------------------------------------
EUNIT_BEGIN_TEST_TABLE(
    ut_cphonealerting,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "T_HandlePhoneEngineMessageIncomingL",
    "CPhoneAlerting",
    "T_HandlePhoneEngineMessageIncomingL",
    "FUNCTIONALITY",
    SetupL, T_HandlePhoneEngineMessageIncomingL, Teardown)

EUNIT_TEST(
    "T_HandlePhoneEngineMessageIdleL",
    "CPhoneAlerting",
    "T_HandlePhoneEngineMessageIdleL",
    "FUNCTIONALITY",
    SetupL, T_HandlePhoneEngineMessageIdleL, Teardown)

EUNIT_TEST(
    "T_HandlePhoneEngineMessageIdleAfterIncomingL",
    "CPhoneAlerting",
    "T_HandlePhoneEngineMessageIdleAfterIncomingL",
    "FUNCTIONALITY",
    SetupL, T_HandlePhoneEngineMessageIdleAfterIncomingL, Teardown)

EUNIT_END_TEST_TABLE
