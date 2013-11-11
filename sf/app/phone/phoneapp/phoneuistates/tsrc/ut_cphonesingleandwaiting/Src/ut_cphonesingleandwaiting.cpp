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
#include "ut_cphonesingleandwaiting.h"
#include "CPhoneEmergency.h"
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

#include "TPhoneCmdParamCallHeaderData.h"
#include "TPhoneCmdParamCallStateData.h"

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
        if ( TPhoneCommandParam::EPhoneParamIdBoolean == param1.ParamId() )
            {
            const TPhoneCmdParamBoolean  &value1 = static_cast<const TPhoneCmdParamBoolean&> (param1);
            const TPhoneCmdParamBoolean  &value2 = static_cast<const TPhoneCmdParamBoolean&> (param2);
            if (  value1.Boolean() != value2.Boolean() )
                {
                ret = EFalse;
                }
            }
        else if ( TPhoneCommandParam::EPhoneParamIdInteger == param1.ParamId() )
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

ut_cphonesingleandwaiting* ut_cphonesingleandwaiting::NewL()
    {
    ut_cphonesingleandwaiting* self = ut_cphonesingleandwaiting::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphonesingleandwaiting* ut_cphonesingleandwaiting::NewLC()
    {
    ut_cphonesingleandwaiting* self = new( ELeave ) ut_cphonesingleandwaiting();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonesingleandwaiting::~ut_cphonesingleandwaiting()
    {
    CSmcMockContext::Release();
    }

ut_cphonesingleandwaiting::ut_cphonesingleandwaiting()
    {
    }

void ut_cphonesingleandwaiting::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphonesingleandwaiting::SetupL(  )
    {
    iMockContext->InitializeL();

    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
          SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );

    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();

    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock );

     
    iState  = CPhoneSingleAndWaiting::NewL(
            iStateMachineMock, iViewCmdHandleMock, NULL );

    iEngineInfoMock = CPEEngineInfo::NewL();
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );

    SmcDefaultValue<TPESimState>::SetL( EPESimUsable );
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPENotActive );

    }


void ut_cphonesingleandwaiting::Teardown(  )
    {
    delete iState;
        iState = NULL;
    delete iViewCmdHandleMock;
        iViewCmdHandleMock = NULL;
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


// - Test methods -----------------------------------------------------------

void ut_cphonesingleandwaiting::TestBubbleAndUICommandsAreUpdatedWhenCallIsPutToHoldL()
    {
    TInt callId( 1 );
    iMockContext->ExpectCallL("CPhoneState::UpdateCallHeaderAndUiCommandsL");
    
    iState->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageHeld, callId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonesingleandwaiting::TestBubbleAndUICommandsAreUpdatedWhenCallIsResumedL()
    {
    TInt callId( 0 );
    iMockContext->ExpectCallL("CPhoneState::UpdateCallHeaderAndUiCommandsL");
    
    iState->HandlePhoneEngineMessageL( MEngineMonitor::EPEMessageConnected, callId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphonesingleandwaiting,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "Test bubble and ui commands are updated when call is put to hold",
    "",
    "",
    "FUNCTIONALITY",
    SetupL, TestBubbleAndUICommandsAreUpdatedWhenCallIsPutToHoldL, Teardown)
    
EUNIT_TEST(
    "Test bubble and ui commands are updated when call is resumed",
    "",
    "",
    "FUNCTIONALITY",
    SetupL, TestBubbleAndUICommandsAreUpdatedWhenCallIsResumedL, Teardown)

    
 EUNIT_END_TEST_TABLE
