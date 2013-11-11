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

#include "cphonestatemachinegsm.h"
#include "ut_cphonestatestartup.h"
#include "cphonestatestartup.h"
#include "cphonemainresourceresolver.h"
#include "pevirtualengine.h" 
#include "mphonekeyevents.h"
#include "tphonecommandparam.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamaudiooutput.h"
#include "cpeengineinfo.h"
#include "mock_cphoneviewcommandhandle.h"

//test
#include "TPhoneCmdParamInteger.h"
#include "tphonecmdparamboolean.h"

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
ut_cphonestatestartup* ut_cphonestatestartup::NewL()
    {
    ut_cphonestatestartup* self = ut_cphonestatestartup::NewLC();
    CleanupStack::Pop();
    return self;
    }
	
ut_cphonestatestartup* ut_cphonestatestartup::NewLC()
    {
    ut_cphonestatestartup* self = new( ELeave ) ut_cphonestatestartup();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonestatestartup::~ut_cphonestatestartup()
    {
    CSmcMockContext::Release();
    }

ut_cphonestatestartup::ut_cphonestatestartup()
    {
    }

void ut_cphonestatestartup::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphonestatestartup::SetupL(  )
    {
    iMockContext->InitializeL();

    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
          SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );
    
    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();
  
    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock ); 
  
  	// Have to be here otherwise execution will stop to ASSERT_DEBUG when 
	// construct the CPhoneStateStartup-object.
    iEngineInfoMock = CPEEngineInfo::NewL();
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );
    
    iStateStartup  = CPhoneStateStartup::NewL(
            iStateMachineMock, iViewCmdHandleMock, NULL );
      
    SmcDefaultValue<TPESimState>::SetL( EPESimStatusUninitialized );
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPENotActive );
        
    }
    

void ut_cphonestatestartup::Teardown(  )
    {
    delete iViewCmdHandleMock;
    iViewCmdHandleMock = NULL;
    delete iStateStartup;
    iStateStartup = NULL;
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
// ut_cphonestatestartup::T_HandleDtmfKeyToneL
// -check commands
// -----------------------------------------------------------------------------
// 
void ut_cphonestatestartup::T_HandleDtmfKeyToneL()
    {     
    MPhoneState* stateStateStartup= iStateStartup; 
    TKeyEvent keyEvent;
    TEventCode eventCode;
    iMockContext->ExpectCallL( "CPhoneState::HandleDtmfKeyToneL" ).
        TimesL (0);
     
    stateStateStartup->HandleDtmfKeyToneL( keyEvent, eventCode );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphonestatestartup,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "1- Handle Dtmf key tone method override",
    "CPhoneStateStartup",
    "Test1",
    "FUNCTIONALITY",
    SetupL, T_HandleDtmfKeyToneL, Teardown)
    
    
EUNIT_END_TEST_TABLE

