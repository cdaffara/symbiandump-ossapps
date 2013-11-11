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
#include "ut_cphonestatecallsetup.h"
#include "cphonestatecallsetup.h"
#include "cphonemainresourceresolver.h"
#include "cphonestatemachinegsm.h"
#include "pevirtualengine.h" 
#include "mphonekeyevents.h"
#include "tphonecommandparam.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamaudiooutput.h"
#include "cpeengineinfo.h"
#include "cpeengineinfoimpl.h"
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
ut_cphonestatecallsetup* ut_cphonestatecallsetup::NewL()
    {
    ut_cphonestatecallsetup* self = ut_cphonestatecallsetup::NewLC();
    CleanupStack::Pop();
    return self;
    }
	
ut_cphonestatecallsetup* ut_cphonestatecallsetup::NewLC()
    {
    ut_cphonestatecallsetup* self = new( ELeave ) ut_cphonestatecallsetup();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonestatecallsetup::~ut_cphonestatecallsetup()
    {
    CSmcMockContext::Release();
    }

ut_cphonestatecallsetup::ut_cphonestatecallsetup()
    {
    }

void ut_cphonestatecallsetup::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphonestatecallsetup::SetupL(  )
    {
    iMockContext->InitializeL();

    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
          SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );
    
    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();
  
    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock ); 
  
  	// Have to be here otherwise execution will stop to ASSERT_DEBUG when 
	// construct the CPhoneStateCallSetup-object.
    iEngineInfoMock = CPEEngineInfo::NewL();
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );
    
    iStateCallSetup  = CPhoneStateCallSetup::NewL(
            iStateMachineMock, iViewCmdHandleMock, NULL );
      
    SmcDefaultValue<TPESimState>::SetL( EPESimStatusUninitialized );
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPENotActive );
    }
    

void ut_cphonestatecallsetup::Teardown(  )
    {
    delete iViewCmdHandleMock;
    iViewCmdHandleMock = NULL;
    delete iStateCallSetup;
    iStateCallSetup = NULL;
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
// ut_cphonestatecallsetup::T_EmptyL
// -check commands
// -----------------------------------------------------------------------------
// 
void ut_cphonestatecallsetup::T_EmptyL()
    {
    }
 
// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphonestatecallsetup,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "1- Constrcution & destruction.",
    "CPhoneStateCallSetup",
    "1- Constrcution & destruction.",
    "FUNCTIONALITY",
    SetupL, T_EmptyL, Teardown)
    
EUNIT_END_TEST_TABLE

