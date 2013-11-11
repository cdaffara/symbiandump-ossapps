/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
/*
-----------------------------------------------------------------------------
This file has been generated with EUnit Pro
http://www.digia.com/eunit
-----------------------------------------------------------------------------
*/

#include <coemain.h>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include <csmcmockcontext.h>
#include <msmcmockspecbuilder.h>
#include <pevirtualengine.h>

#include "ut_cphonegeneralgsmmessageshandler.h"
#include "cphonegeneralgsmmessageshandler.h"
#include "CPhoneStateMachineGSM.h"
#include "mock_cphoneviewcommandhandle.h"
#include "cphonemainresourceresolver.h"
#include "cphonestate.h"
#include "TPhoneCmdParamBoolean.h"
#include "TPhoneCmdParamInteger.h"
#include "TPhoneCmdParamKeyEvent.h"
#include "TPhoneCmdParamGlobalNote.h"
#include "TPhoneCmdParamKeyEvent.h"
#include "apgtask.h"

// needed for mocking CPhoneMainResourceResolver::Instance()
class CPhoneMainResourceResolverMock : public CPhoneMainResourceResolver
    {
    
    };

// -----------------------------------------------------------------------------
// operator for comparing parameter expected and result value.
// -
// -----------------------------------------------------------------------------
//
// this is set true if there is no need check param id value.
TBool doNotCheckParamId( EFalse );

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
            }/*
        else if ( TPhoneCommandParam::EPhoneParamIdKeyEvent == param1.ParamId() )
            {
            const TPhoneCmdParamKeyEvent  &value1 = static_cast<const TPhoneCmdParamKeyEvent&> (param1);
            const TPhoneCmdParamKeyEvent  &value2 = static_cast<const TPhoneCmdParamKeyEvent&> (param2);
            if ( value1.EventCode() != value2.EventCode() )
                {
                ret = EFalse;
                }
            }*/
        else if ( TPhoneCommandParam::EPhoneParamIdGlobalNote == param1.ParamId() )
            {
            const TPhoneCmdParamGlobalNote  &value1 = static_cast<const TPhoneCmdParamGlobalNote&> (param1);
            const TPhoneCmdParamGlobalNote  &value2 = static_cast<const TPhoneCmdParamGlobalNote&> (param2);
            if ( value1.Type() != value2.Type() )
                {
                ret = EFalse;
                }
            }
        else if ( TPhoneCommandParam::EPhoneParamIdBoolean == param1.ParamId() && !doNotCheckParamId )
            {
            const TPhoneCmdParamBoolean  &value1 = static_cast<const TPhoneCmdParamBoolean&> (param1);
            const TPhoneCmdParamBoolean  &value2 = static_cast<const TPhoneCmdParamBoolean&> (param2);
            if ( value1.Boolean() != value2.Boolean() )
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

ut_cphonegeneralgsmmessageshandler* ut_cphonegeneralgsmmessageshandler::NewL()
    {
    ut_cphonegeneralgsmmessageshandler* self = ut_cphonegeneralgsmmessageshandler::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphonegeneralgsmmessageshandler* ut_cphonegeneralgsmmessageshandler::NewLC()
    {
    ut_cphonegeneralgsmmessageshandler* self = new( ELeave ) ut_cphonegeneralgsmmessageshandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonegeneralgsmmessageshandler::~ut_cphonegeneralgsmmessageshandler()
    {
    CSmcMockContext::Release();
    
    delete iCoeEnvMock;
    delete iCPhoneMainResourceResolverMock;
    delete iViewCommandHandleMock;
    delete iStateMachineMock;
    }

ut_cphonegeneralgsmmessageshandler::ut_cphonegeneralgsmmessageshandler()
    {
    }

void ut_cphonegeneralgsmmessageshandler::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphonegeneralgsmmessageshandler::SetupL(  )
    {
    iMockContext->InitializeL();
    
    iCoeEnvMock = new ( ELeave ) CCoeEnv();
    SmcDefaultValue<CCoeEnv*>::SetL( iCoeEnvMock );
    CleanupResetDefaultValuePushL<SmcDefaultValue<CCoeEnv*> >();

    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
    SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );
    
    iViewCommandHandleMock = new CPhoneViewCommandHandleMock(); 
    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCommandHandleMock );
    
    iMockState = new (ELeave) CPhoneState( iStateMachineMock, NULL, NULL );

    iPhoneGeneralGsmMessagesHandler = CPhoneGeneralGsmMessagesHandler::NewL( 
        *iStateMachineMock, *iViewCommandHandleMock, *iMockState );
     
    }
    
void ut_cphonegeneralgsmmessageshandler::Teardown(  )
    {
    delete iPhoneGeneralGsmMessagesHandler; 
    iPhoneGeneralGsmMessagesHandler = NULL;
    delete iMockState;
    iMockState = NULL;
    delete iStateMachineMock;
    iStateMachineMock = NULL;
    delete iViewCommandHandleMock;
    iViewCommandHandleMock = NULL;
    delete iCPhoneMainResourceResolverMock;
    iCPhoneMainResourceResolverMock = NULL;
    delete iCoeEnvMock;
    iCoeEnvMock = NULL;  
    CleanupStack::PopAndDestroy();
    SmcDefaultValue<CPhoneMainResourceResolver*>::Reset();
    iMockContext->Reset();
    }
    
        
void ut_cphonegeneralgsmmessageshandler::T_ConstructionL()
    {
    // SetupL and Teardown does the test. 
    }
    
void ut_cphonegeneralgsmmessageshandler::T_HandlePhoneEngineMessageL_EPEMessageIssuingSSRequest_L()
    {
    iMockContext->ExpectCallL("CPhoneStateMachine::SendPhoneEngineMessage").WithL( MPEPhoneModel::EPEMessageEndDTMF );
    
    iMockContext->ExpectCallL("CPhoneViewCommandHandleMock::ExecuteCommandL").
         WithL( EPhoneViewRemoveQuery );
         
    iPhoneGeneralGsmMessagesHandler->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIssuingSSRequest, -1 );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );    
    }
    
void ut_cphonegeneralgsmmessageshandler::T_HandlePhoneEngineMessageL_EPEMessageIssuedSSRequest_L()
    {    
    iMockContext->ExpectCallL("CPhoneViewCommandHandleMock::ExecuteCommandL").TimesL(0);
         
    iPhoneGeneralGsmMessagesHandler->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageIssuedSSRequest, -1 );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );    
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphonegeneralgsmmessageshandler,
    "Add test suite description here.",
    "UNIT" )
    
EUNIT_TEST(
    "Construction",
    "CPhoneGeneralGsmMessagesHandler",
    "test1",
    "FUNCTIONALITY",
    SetupL, T_ConstructionL, Teardown)

EUNIT_TEST(
    "HandlePhoneEngineMessageL EPEMessageIssuingSSRequest",
    "CPhoneGeneralGsmMessagesHandler",
    "test2",
    "FUNCTIONALITY",
    SetupL, T_HandlePhoneEngineMessageL_EPEMessageIssuingSSRequest_L, Teardown)

EUNIT_TEST(
    "HandlePhoneEngineMessageL EPEMessageIssuedSSRequest",
    "CPhoneGeneralGsmMessagesHandler",
    "test2",
    "FUNCTIONALITY",
    SetupL, T_HandlePhoneEngineMessageL_EPEMessageIssuedSSRequest_L, Teardown)

EUNIT_END_TEST_TABLE
