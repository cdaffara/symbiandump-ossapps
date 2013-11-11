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

#include "UT_CPhoneStateIncoming.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <EIKMENUP.H>
#include <featmgr.h>
#include "cphonemainresourceresolver.h"
#include "mock_cphoneviewcommandhandle.h"
#include "cphonestatemachinegsm.h"
#include "cphonecustomizationvoip.h"
#include "cpeengineinfo.h"

#include "phoneappcommands.hrh"

#include <csmcmockcontext.h>
#include <msmcmockspecbuilder.h>

// Allow testing of protected methods
#define protected public
#include "cphonestateincoming.h"
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
        /*if ( TPhoneCommandParam::EPhoneParamIdCallHeaderData == param1.ParamId() )
            {
            const TPhoneCmdParamCallHeaderData &value1 = static_cast<const TPhoneCmdParamCallHeaderData&> (param1);
            const TPhoneCmdParamCallHeaderData &value2 = static_cast<const TPhoneCmdParamCallHeaderData&> (param2);
            if ( value1.CallState() == value2.CallState() )
                {
                ret = ETrue;
                }
            }
        else  
            {*/
            ret = ETrue;  // just paramiId compared
            //}
        }
    return ret;
    }

// - Construction -----------------------------------------------------------

UT_CPhoneStateIncoming* UT_CPhoneStateIncoming::NewL()
    {
    UT_CPhoneStateIncoming* self = UT_CPhoneStateIncoming::NewLC();
    CleanupStack::Pop();
    return self;
    }

UT_CPhoneStateIncoming* UT_CPhoneStateIncoming::NewLC()
    {
    UT_CPhoneStateIncoming* self = new( ELeave ) UT_CPhoneStateIncoming();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

UT_CPhoneStateIncoming::~UT_CPhoneStateIncoming()
    {
    CSmcMockContext::Release();
    }

UT_CPhoneStateIncoming::UT_CPhoneStateIncoming()
    {
    }

void UT_CPhoneStateIncoming::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------

void UT_CPhoneStateIncoming::SetupL(  )
    {
    iMockContext->InitializeL();
    
    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
    SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );
    
    iEngineInfoMock = CPEEngineInfo::NewL();
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );
    
    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();
    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock );
    iCustomizationMock = 
        CPhoneCustomizationVoip::NewL( *iStateMachineMock, *iViewCmdHandleMock );
       
    iCPhoneStateIncoming = CPhoneStateIncoming::NewL(
        iStateMachineMock, iViewCmdHandleMock, iCustomizationMock );
    }
    

void UT_CPhoneStateIncoming::Teardown(  )
    {
    delete iCPhoneMainResourceResolverMock;
    iCPhoneMainResourceResolverMock = NULL;
    SmcDefaultValue<CPhoneMainResourceResolver*>::Reset();

    delete iEngineInfoMock;
    iEngineInfoMock = NULL;
    
    delete iCPhoneStateIncoming;
    iCPhoneStateIncoming = NULL;
    
    delete iViewCmdHandleMock;
    iViewCmdHandleMock = NULL;
        
    delete iStateMachineMock;
    iStateMachineMock = NULL;
    
    delete iCustomizationMock;
    iCustomizationMock = NULL;
       
    iMockContext->Reset();
    }
    
void UT_CPhoneStateIncoming::T_HandleKeyMessageL(  )
    {
    _LIT( KPhoneNumber, "12" );
    _LIT( KPhoneNumber2, "123" );
    HBufC* phoneNumber = KPhoneNumber().AllocLC();
   
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryVisibleL" ).
       ReturnsL( ETrue );
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
       ReturnsL( phoneNumber );

    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetPhoneNumber" );

    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage").
        WithL( MPEPhoneModel::EPEMessagePhoneNumberEdited );
    
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::PhoneNumberIsServiceCode" ).
		ReturnsL( EFalse );
    
    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage").
        WithL( MPEPhoneModel::EPEMessageCheckEmergencyNumber );
       
    iCPhoneStateIncoming->HandleKeyMessageL(
       MPhoneKeyEvents::EPhoneKeyShortPress, EKeyYes );

    HBufC* phoneNumber2 = KPhoneNumber2().AllocLC();
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::IsNumberEntryVisibleL" ).
        ReturnsL( ETrue );
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
        ReturnsL( phoneNumber2 );
    
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetPhoneNumber" );

    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage").
        WithL( MPEPhoneModel::EPEMessagePhoneNumberEdited );
    
    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage").
        WithL( MPEPhoneModel::EPEMessageCheckEmergencyNumber );
    
    iCPhoneStateIncoming->HandleKeyMessageL(
        MPhoneKeyEvents::EPhoneKeyShortPress, EKeyYes );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );

    CleanupStack::Pop( phoneNumber2 );
    CleanupStack::Pop( phoneNumber );
    }

void UT_CPhoneStateIncoming::T_HandleCommandL(  )
    {
    _LIT( KPhoneNumber, "123" );
    HBufC* phoneNumber = KPhoneNumber().AllocLC();
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
        ReturnsL( phoneNumber );
    
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetPhoneNumber" );

    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage").
        WithL( MPEPhoneModel::EPEMessagePhoneNumberEdited );
    
    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage").
        WithL( MPEPhoneModel::EPEMessageCheckEmergencyNumber );
    
    iCPhoneStateIncoming->HandleCommandL(EPhoneNumberAcqCmdSendCommand );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );

    CleanupStack::Pop( phoneNumber );
    }
    
void UT_CPhoneStateIncoming::T_HandleAudioPlayStoppedL()
    {
    iCPhoneStateIncoming->HandleAudioPlayStoppedL();
    }
    
void UT_CPhoneStateIncoming::T_HandleConnectedL()
    {    
    const TInt callId = 1;
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
        WithL( EPhoneViewStopRingTone );
     
    // HandleConnectedL() is tested through HandlePhoneEngineMessageL()
    iCPhoneStateIncoming->HandlePhoneEngineMessageL(
            MEngineMonitor::EPEMessageConnected, callId );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void UT_CPhoneStateIncoming::T_HandleSendKeyAnswerL()
	{
    _LIT( KPhoneNumber, "p1" );
    HBufC* phoneNumber = KPhoneNumber().AllocLC();    
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
        ReturnsL( phoneNumber );

    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetPhoneNumber" );

    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage").
        WithL( MPEPhoneModel::EPEMessagePhoneNumberEdited );
    
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::PhoneNumberIsServiceCode" ).
		ReturnsL( EFalse );
    
    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage").
        WithL( MPEPhoneModel::EPEMessageCheckEmergencyNumber );

    
    iCPhoneStateIncoming->HandleCommandL( EPhoneNumberAcqCmdSendCommand );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    CleanupStack::Pop( phoneNumber );
	}

void UT_CPhoneStateIncoming::T_HandleSendKeyRejectL()
	{
    _LIT( KPhoneNumber, "0" );
    HBufC* phoneNumber = KPhoneNumber().AllocLC();    
    
    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::PhoneNumberFromEntryLC" ).
        ReturnsL( phoneNumber );

    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetPhoneNumber" );

    iMockContext->ExpectCallL( "CPhoneStateMachine::SendPhoneEngineMessage").
        WithL( MPEPhoneModel::EPEMessagePhoneNumberEdited );
    
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::PhoneNumberIsServiceCode" ).
		ReturnsL( ETrue );

    iMockContext->ExpectCallL( "CPhoneNumberEntryManager::CallFromNumberEntryL" );
    
    iCPhoneStateIncoming->HandleCommandL( EPhoneNumberAcqCmdSendCommand );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    CleanupStack::Pop( phoneNumber );
	}

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    UT_CPhoneStateIncoming,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "HandleKeyMessageL",
    "CPhoneStateIncoming",
    "HandleKeyMessageL",
    "FUNCTIONALITY",
    SetupL, T_HandleKeyMessageL, Teardown)
  
EUNIT_TEST(
    "HandleCommandL",
    "CPhoneStateIncoming",
    "HanddleCommandL",
    "FUNCTIONALITY",
    SetupL, T_HandleCommandL, Teardown)

EUNIT_TEST(
    "HandleAudioPlayStoppedL",
    "CPhoneStateIncoming",
    "HandleAudioPlayStoppedL",
    "FUNCTIONALITY",
    SetupL, T_HandleAudioPlayStoppedL, Teardown)
 
EUNIT_TEST(
    "HandleConnectedL",
    "CPhoneStateIncoming",
    "HandleConnectedL",
    "FUNCTIONALITY",
    SetupL, T_HandleConnectedL, Teardown)

EUNIT_TEST(
    "HandleSendKeyAnswerL",
    "CPhoneStateIncoming",
    "HandleSendKeyAnswerL",
    "FUNCTIONALITY",
    SetupL, T_HandleSendKeyAnswerL, Teardown)

EUNIT_TEST(
    "HandleSendKeyRejectL",
    "CPhoneStateIncoming",
    "HandleSendKeyRejectL",
    "FUNCTIONALITY",
    SetupL, T_HandleSendKeyRejectL, Teardown)

EUNIT_END_TEST_TABLE
