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

#include "UT_CPhoneAccessoryBTHandler.h"
#include "FeatMgr.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <btengdomainpskeys.h>
#include <e32property.h>

#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>

#include <coemain.h>

#include "cpeengineinfo.h"
#include "cpeengineinfoimpl.h"
#include "cphonemainresourceresolver.h"
#include "tphonecmdparamnote.h"

#include "cphonestatemachinegsm.h"
#include "mock_cphoneviewcommandhandle.h"
#include "cphoneaccessorybthandler.h"


// needed for mocking CPhoneMainResourceResolver::Instance()
class CPhoneMainResourceResolverMock : public CPhoneMainResourceResolver
    {
    };

// - Construction -----------------------------------------------------------

UT_CPhoneAccessoryBTHandler* UT_CPhoneAccessoryBTHandler::NewL()
    {
    UT_CPhoneAccessoryBTHandler* self = UT_CPhoneAccessoryBTHandler::NewLC();
    CleanupStack::Pop();
    return self;
    }

UT_CPhoneAccessoryBTHandler* UT_CPhoneAccessoryBTHandler::NewLC()
    {
    UT_CPhoneAccessoryBTHandler* self = new( ELeave ) UT_CPhoneAccessoryBTHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

UT_CPhoneAccessoryBTHandler::~UT_CPhoneAccessoryBTHandler()
    {
    CSmcMockContext::Release();
    }

UT_CPhoneAccessoryBTHandler::UT_CPhoneAccessoryBTHandler()
    {
    }

void UT_CPhoneAccessoryBTHandler::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    } 

// - Test methods -----------------------------------------------------------
void UT_CPhoneAccessoryBTHandler::SetupL(  )
    {
    iEntryCleared = EFalse;
    iMockContext->InitializeL();
    
    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
           SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );
           
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPENotActive );
    
    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();
    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCmdHandleMock );
    
    iCoeEnvMock = new (ELeave ) CCoeEnv();
    SmcDefaultValue<CCoeEnv*>::SetL( iCoeEnvMock );

    iEngineInfoMock = CPEEngineInfo::NewL();
            SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );
    
    iCPhoneAccessoryBTHandler = 
        CPhoneAccessoryBTHandler::NewLC( 
        iViewCmdHandleMock,
        iStateMachineMock );

    }

void UT_CPhoneAccessoryBTHandler::Teardown(  )
    {
    delete iCPhoneMainResourceResolverMock;
    iCPhoneMainResourceResolverMock = NULL;
            
    CleanupStack::PopAndDestroy( iCPhoneAccessoryBTHandler ); 
    iCPhoneAccessoryBTHandler = NULL;
    
    delete iViewCmdHandleMock;
    iViewCmdHandleMock = NULL;
    
    delete iStateMachineMock;
    iStateMachineMock = NULL;
    
    delete iCoeEnvMock;
    iCoeEnvMock = NULL;
    SmcDefaultValue<CCoeEnv*>::Reset();
    
    delete iEngineInfoMock;
    iEngineInfoMock = NULL;
    
    SmcDefaultValue<const TPEAudioOutput&>::Reset();
    
    iMockContext->Reset();
    }
    
void UT_CPhoneAccessoryBTHandler::HandleNumberEntryClearedL()
    {
    iEntryCleared = ETrue;
    }
  
void UT_CPhoneAccessoryBTHandler::UT_SetBTHandsfreeModeL_ONL(  )
    {
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::AudioOutput" );
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetAudioOutputCommand" ).
            WithL<TPEAudioOutput, TInt>(EPEBTAudioAccessory, ETrue);
    iCPhoneAccessoryBTHandler->SetBTHandsfreeModeL(ETrue );
    
    EUNIT_ASSERT_EQUALS( ETrue, iCPhoneAccessoryBTHandler->SetBTHandsfreeModeL( ETrue ) );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

void UT_CPhoneAccessoryBTHandler::UT_SetBTHandsfreeModeL_OFFL(  )
    {
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::AudioOutput" );
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetAudioOutputCommand" ).
            WithL<TPEAudioOutput, TInt>(EPEHandset, EFalse);
    
    EUNIT_ASSERT_EQUALS( ETrue, iCPhoneAccessoryBTHandler->SetBTHandsfreeModeL( EFalse ) );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

void UT_CPhoneAccessoryBTHandler::UT_SetBTHandsfreeModeL_ON_AudioRoutedToWiredAudioAccessoryL(  )
    {
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPEWiredAudioAccessory );
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::AudioOutput" ).TimesL(1);
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetAudioOutputCommand" ).TimesL(0);
    
    EUNIT_ASSERT_EQUALS( EFalse, iCPhoneAccessoryBTHandler->SetBTHandsfreeModeL( ETrue ) );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }
    
void UT_CPhoneAccessoryBTHandler::UT_SetHandsfreeModeL_ONL(  )
    {
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::AudioOutput" ).
            ReturnsL( EPENotActive );
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetAudioOutputCommand" ).
            WithL<TPEAudioOutput, TInt>(EPELoudspeaker, ETrue);
    iCPhoneAccessoryBTHandler->SetBTHandsfreeModeL( ETrue );
    
    EUNIT_ASSERT_EQUALS( ETrue, iCPhoneAccessoryBTHandler->SetHandsfreeModeL( ETrue ) );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

void UT_CPhoneAccessoryBTHandler::UT_SetHandsfreeModeL_OFFL(  )
    {
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::AudioOutput" ).
            ReturnsL( EPENotActive );
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetAudioOutputCommand" ).
            WithL<TPEAudioOutput, TInt>(EPEHandset, EFalse);
    
    EUNIT_ASSERT_EQUALS( ETrue, iCPhoneAccessoryBTHandler->SetHandsfreeModeL( EFalse ) );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

void UT_CPhoneAccessoryBTHandler::UT_SetHandsfreeModeL_ON_AudioRoutedToWiredAudioAccessoryL(  )
    {
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPEWiredAudioAccessory );
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::AudioOutput" ).TimesL(1);
    iMockContext->ExpectCallL( "CPEEngineInfoImpl::SetAudioOutputCommand" ).TimesL(0);
    
    EUNIT_ASSERT_EQUALS( EFalse, iCPhoneAccessoryBTHandler->SetHandsfreeModeL( ETrue ) );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }
    
void UT_CPhoneAccessoryBTHandler::UT_ShowBTAddressLL(  )
    {
    // dummy test.
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL( EPhoneViewClearNumberEntryContent );
    
    TPhoneCmdParamNote noteParam;
    noteParam.SetType( EPhoneNoteCustom );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewShowNote, noteParam );
    
    iCPhoneAccessoryBTHandler->ShowBTAddressL();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }
    
void UT_CPhoneAccessoryBTHandler::UT_ShowBTLoopbackL(  )
    {
    // dummy test.
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL( EPhoneViewClearNumberEntryContent );
    
    TPhoneCmdParamNote noteParam;
    noteParam.SetType( EPhoneNoteCustom );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewShowNote, noteParam );
    
    iCPhoneAccessoryBTHandler->ShowBTLoopbackL();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }
    
void UT_CPhoneAccessoryBTHandler::UT_ShowBTActivatedL(  )
    {
    // dummy test.
    TPhoneCmdParamNote noteParam;
    noteParam.SetType( EPhoneNoteConfirmation );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewShowNote, noteParam );
    
    iCPhoneAccessoryBTHandler->ShowBTActivatedL();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    UT_CPhoneAccessoryBTHandler,
    "Add test suite description here.",
    "UNIT" )
    
EUNIT_TEST(
    "SetBTHandsfreeModeL_ONL - test1",
    "CPhoneAccessoryBTHandler",
    "SetBTHandsfreeModeL_ONL - test1",
    "FUNCTIONALITY",
    SetupL, UT_SetBTHandsfreeModeL_ONL, Teardown)
    
EUNIT_TEST(
    "SetBTHandsfreeModeL_OFFL - test2",
    "CPhoneAccessoryBTHandler",
    "SetBTHandsfreeModeL_OFFL - test2",
    "FUNCTIONALITY",
    SetupL, UT_SetBTHandsfreeModeL_OFFL, Teardown)
    
EUNIT_TEST(
    "SetBTHandsfreeModeL_ON_AudioRoutedToWiredAudioAccessoryL - test3",
    "CPhoneAccessoryBTHandler",
    "SetBTHandsfreeModeL_ON_AudioRoutedToWiredAudioAccessoryL - test3",
    "FUNCTIONALITY",
    SetupL, UT_SetBTHandsfreeModeL_ON_AudioRoutedToWiredAudioAccessoryL, Teardown)
    
EUNIT_TEST(
    "SetHandsfreeModeL_ONL - test4",
    "CPhoneAccessoryBTHandler",
    "SetHandsfreeModeL_ONL - test4",
    "FUNCTIONALITY",
    SetupL, UT_SetHandsfreeModeL_ONL, Teardown)
    
EUNIT_TEST(
    "SetHandsfreeModeL_OFFL - test5",
    "CPhoneAccessoryBTHandler",
    "SetHandsfreeModeL_OFFL - test5",
    "FUNCTIONALITY",
    SetupL, UT_SetHandsfreeModeL_OFFL, Teardown)
    
EUNIT_TEST(
    "SetHandsfreeModeL_ON_AudioRoutedToWiredAudioAccessoryL - test6",
    "CPhoneAccessoryBTHandler",
    "SetHandsfreeModeL_ON_AudioRoutedToWiredAudioAccessoryL - test6",
    "FUNCTIONALITY",
    SetupL, UT_SetHandsfreeModeL_ON_AudioRoutedToWiredAudioAccessoryL, Teardown)
    
EUNIT_TEST(
    "ShowBTAddressLL - test7",
    "CPhoneAccessoryBTHandler",
    "ShowBTAddressLL - test7",
    "FUNCTIONALITY",
    SetupL, UT_ShowBTAddressLL, Teardown)
    
EUNIT_TEST(
    "ShowBTLoopbackL - test8",
    "CPhoneAccessoryBTHandler",
    "ShowBTLoopbackL - test8",
    "FUNCTIONALITY",
    SetupL, UT_ShowBTLoopbackL, Teardown)
    
EUNIT_TEST(
    "ShowBTActivatedL - test9",
    "CPhoneAccessoryBTHandler",
    "ShowBTActivatedL - test9",
    "FUNCTIONALITY",
    SetupL, UT_ShowBTActivatedL, Teardown)

EUNIT_END_TEST_TABLE
