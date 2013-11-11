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
#include "ut_cphoneerrormessageshandler.h"
#include <coemain.h>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>



#include <csmcmockcontext.h>
#include <msmcmockspecbuilder.h>
#include "cphoneerrormessageshandler.h"
#include "CPhoneStateMachineGSM.h"
#include "mock_cphoneviewcommandhandle.h"
#include "CPhoneMainErrorMessagesHandler.h"
#include "cphonemainresourceresolver.h"
#include "tphonecmdparamglobalnote.h"

// needed for mocking CPhoneMainErrorMessagesHandler::Instance()
class CPhoneMainErrorMessagesHandlerMock : public CPhoneMainErrorMessagesHandler
    {

    };

// needed for mocking CPhoneMainResourceResolver::Instance()
class CPhoneMainResourceResolverMock : public CPhoneMainResourceResolver
    {

    };


// - Construction -----------------------------------------------------------

ut_cphoneerrormessageshandler* ut_cphoneerrormessageshandler::NewL()
    {
    ut_cphoneerrormessageshandler* self = ut_cphoneerrormessageshandler::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphoneerrormessageshandler* ut_cphoneerrormessageshandler::NewLC()
    {
    ut_cphoneerrormessageshandler* self = new( ELeave ) ut_cphoneerrormessageshandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphoneerrormessageshandler::~ut_cphoneerrormessageshandler()
    {
    CSmcMockContext::Release();

    delete iCPhoneErrorMessagesHandler;
    delete iCoeEnvMock;
    delete iCPhoneMainErrorMessagesHandlerMock;
    delete iCPhoneMainResourceResolverMock;
    delete iEngineInfoMock;
    delete iViewCommandHandleMock;
    delete iStateMachineMock;
    }

ut_cphoneerrormessageshandler::ut_cphoneerrormessageshandler()
    {
    }

void ut_cphoneerrormessageshandler::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphoneerrormessageshandler::SetupL(  )
    {
    iMockContext->InitializeL();

    iCoeEnvMock = new ( ELeave ) CCoeEnv();
    SmcDefaultValue<CCoeEnv*>::SetL( iCoeEnvMock );
    CleanupResetDefaultValuePushL<SmcDefaultValue<CCoeEnv*> >();

    iCPhoneMainErrorMessagesHandlerMock = new ( ELeave ) CPhoneMainErrorMessagesHandlerMock();
    SmcDefaultValue<CPhoneMainErrorMessagesHandler*>::SetL( iCPhoneMainErrorMessagesHandlerMock );

    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
    SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );

    iEngineInfoMock = CPEEngineInfo::NewL();
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );

    iViewCommandHandleMock = new CPhoneViewCommandHandleMock();
    iStateMachineMock = CPhoneStateMachineGSM::NewL( iViewCommandHandleMock );

    iCPhoneErrorMessagesHandler = CPhoneErrorMessagesHandler::NewL( iViewCommandHandleMock, iStateMachineMock);

    }

void ut_cphoneerrormessageshandler::Teardown(  )
    {
    delete iCPhoneErrorMessagesHandler;
    iCPhoneErrorMessagesHandler = NULL;
    delete iStateMachineMock;
    iStateMachineMock = NULL;
    delete iViewCommandHandleMock;
    iViewCommandHandleMock = NULL;
    delete iEngineInfoMock;
    iEngineInfoMock = NULL;
    delete iCPhoneMainResourceResolverMock;
    iCPhoneMainResourceResolverMock = NULL;
    delete iCPhoneMainErrorMessagesHandlerMock;
    iCPhoneMainErrorMessagesHandlerMock = NULL;
    delete iCoeEnvMock;
    iCoeEnvMock = NULL;

    CleanupStack::PopAndDestroy();

    SmcDefaultValue<CPhoneMainResourceResolver*>::Reset();
    SmcDefaultValue<CPhoneMainErrorMessagesHandler*>::Reset();

    iMockContext->Reset();
    }

void ut_cphoneerrormessageshandler::T_Global_ShowErrorSpecificNoteForVideoCallL(  )
    {
     TPEErrorInfo aErrorInfo;
     aErrorInfo.iErrorCode = ECCPErrorCCPreemption;
     aErrorInfo.iCallId = 1;
     const TInt KEnabled = 1;
     const TInt KCallId = 1;

     iEngineInfoMock->SetCallTypeCommand( EPECallTypeVideo );
     iEngineInfoMock->SetCallType( EPECallTypeVideo, KCallId );

     TPECallType type(EPECallTypeVideo);
     iMockContext->ExpectCallL("CPEEngineInfoImpl::CallType").
         WithL(KCallId).
         ReturnsL(type);

     iMockContext->
         ExpectCallL( "TPhoneCmdParamBoolean::SetBoolean" ).
         WithL( EFalse );
     iMockContext->
         ExpectCallL( "TPhoneCmdParamGlobalNote::SetType" ).
         WithL( EPhoneMessageBoxInformation );
     iMockContext->
         ExpectCallL( "CPhoneMainResourceResolver::ResolveResourceID" ).
         WithL<const TInt&>( EPhoneNoteCallInfoNotSupported  );
     iMockContext->
         ExpectCallL( "TPhoneCmdParamGlobalNote::SetTextResourceId" );

     iCPhoneErrorMessagesHandler->ShowErrorSpecificNoteL( aErrorInfo );
     EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphoneerrormessageshandler::T_Global_ShowErrorSpecificNoteForCsCallL(  )
    {
     TPEErrorInfo aErrorInfo;
     aErrorInfo.iErrorCode = ECCPErrorCCPreemption;
     aErrorInfo.iCallId = 1;
     const TInt KEnabled = 1;
     const TInt KCallId = 1;
     
     EUNIT_PRINT( _L("CPEEngineInfoImpl::CallType"));
     
     TPECallType type(EPECallTypeUninitialized);
      iMockContext->ExpectCallL("CPEEngineInfoImpl::CallType").
          WithL(KCallId).
          ReturnsL(type);
      
      iEngineInfoMock->SetSimState(EPESimReadable);
      
      EUNIT_PRINT( _L("TPhoneCmdParamGlobalNote::SetType"));
     iMockContext->
         ExpectCallL( "TPhoneCmdParamGlobalNote::SetType" ).
         WithL( EPhoneNotificationDialog );

     iMockContext->
         ExpectCallL( "CPhoneMainResourceResolver::ResolveResourceID" ).
         WithL<const TInt&>( EPhoneErrorInConnection  );

     iMockContext->
         ExpectCallL( "TPhoneCmdParamGlobalNote::SetTextResourceId" );
     EUNIT_PRINT( _L("iCPhoneErrorMessagesHandler->ShowErrorSpecificNoteL ->"));
     iCPhoneErrorMessagesHandler->ShowErrorSpecificNoteL( aErrorInfo );
     EUNIT_PRINT( _L("iCPhoneErrorMessagesHandler->ShowErrorSpecificNoteL <-"));
     EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphoneerrormessageshandler::VerifyNote( TPECallType aCallType, TCCPError aErrorCode, TPhoneRssBase aNote )
    {
     TPEErrorInfo errorInfo;
     errorInfo.iErrorCode = aErrorCode;
     errorInfo.iCallId = 1;
     const TInt KEnabled = 1;
     const TInt KCallId = 1;
     
     iEngineInfoMock->SetSimState(EPESimReadable);
     iEngineInfoMock->SetCallTypeCommand( aCallType );
     iEngineInfoMock->SetCallType( aCallType, KCallId );

     iMockContext->ExpectCallL("CPEEngineInfoImpl::CallType").
         WithL(KCallId).
         ReturnsL(aCallType).
         TimesL( ( aCallType == EPECallTypeCSVoice 
                 && aErrorCode == ECCPErrorCCCallRejected ) ? 2 : 1 );

     iMockContext->
         ExpectCallL( "CPhoneMainResourceResolver::ResolveResourceID" ).
         WithL<const TInt&>( aNote ).
         TimesL( aNote == EPhoneRssBaseLast ? 0 : 1 );

     iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
         TimesL( aNote == EPhoneRssBaseLast ? 0 : 2 );

     iCPhoneErrorMessagesHandler->ShowErrorSpecificNoteL( errorInfo );
     EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphoneerrormessageshandler::VerifyNoNote( TPECallType aCallType, TCCPError aErrorCode )
    {
     TPEErrorInfo errorInfo;
     errorInfo.iErrorCode = aErrorCode;
     errorInfo.iCallId = 1;
     const TInt KEnabled = 1;
     const TInt KCallId = 1;

     iEngineInfoMock->SetCallTypeCommand( aCallType );
     iEngineInfoMock->SetCallType( aCallType, KCallId );

     iMockContext->ExpectCallL("CPEEngineInfoImpl::CallType").
         WithL(KCallId).
         ReturnsL(aCallType);

     iMockContext->ExpectCallL( "CPhonePubSubProxy::Value" ).
         TimesL( 0 ).
         ReturnsL( KEnabled );

     iMockContext->
         ExpectCallL( "CPhoneMainResourceResolver::ResolveResourceID" ).
         TimesL( 0 );

     iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
         TimesL( 0 );

     iCPhoneErrorMessagesHandler->ShowErrorSpecificNoteL( errorInfo );

     EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphoneerrormessageshandler::T_Global_ShowErrorSpecificNoteL(  )
    {
    iEngineInfoMock->SetSimState(EPESimReadable);
    //          eCall type           Error                                     Note Resource
    EUNIT_PRINT( _L( "Video call and ECCPErrorServiceSettingError" ) );
    VerifyNote( EPECallTypeVideo,   ECCPErrorServiceSettingError,             EPhoneNotePhoneOutOf3GCoverage );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "CS Voice call and ECCPErrorServiceSettingError" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeCSVoice, ECCPErrorServiceSettingError,             EPhoneNoteTextCheckNetworkservices );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "Video call and ECCPErrorCCNoRouteToDestination" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeVideo,   ECCPErrorCCNoRouteToDestination,          EPhoneErrorInConnection );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "CS Voice call and ECCPErrorCCNoRouteToDestination" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeCSVoice, ECCPErrorCCNoRouteToDestination, EPhoneErrorInConnection );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "Video call and ECCPErrorCCCallRejected" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeVideo,   ECCPErrorCCCallRejected,                  EPhoneNoteCalledNumberHasBarredIncomingCalls );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "CS Voice call and ECCPErrorCCCallRejected" ) );
     iMockContext->InitializeL();
     VerifyNote( EPECallTypeCSVoice, ECCPErrorCCCallRejected, EPhoneNoteCallInfoCauseValue21);
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "Video call and ECCPErrorNoAnswerForVideo" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeVideo,   ECCPErrorNoAnswerForVideo,                EPhoneNotePhoneOutOf3GCoverage );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "CS Voice call and ECCPErrorNoAnswerForVideo" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeCSVoice, ECCPErrorNoAnswerForVideo,                EPhoneNoAnswer );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "Video call and ECCPErrorCCRequestedFacilityNotSubscribed" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeVideo,   ECCPErrorCCRequestedFacilityNotSubscribed,EPhoneNoteVideoCallOnlyPossibleUnder3GCoverage );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "CS Voice call and ECCPErrorCCRequestedFacilityNotSubscribed" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeCSVoice, ECCPErrorCCRequestedFacilityNotSubscribed,EPhoneNoteTextNotAllowed );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "Video call and ECCPErrorCCIncompatibleDestination" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeVideo,   ECCPErrorCCIncompatibleDestination,       EPhoneNoteUnableToMakeVideoCallNotSupportedByOtherPhone );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "CS Voice call and ECCPErrorCCIncompatibleDestination" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeCSVoice, ECCPErrorCCIncompatibleDestination, EPhoneErrorInConnection );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "Video call and ECCPErrorCCBearerCapabilityNotAuthorised" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeVideo,   ECCPErrorCCBearerCapabilityNotAuthorised, EPhoneNoteUnableToMakeVideoCallNotSupportedByOtherPhone );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "CS Voice call and ECCPErrorCCBearerCapabilityNotAuthorised" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeCSVoice, ECCPErrorCCBearerCapabilityNotAuthorised, EPhoneErrorInConnection );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "Video call and ECCPErrorBadRequest" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeVideo,   ECCPErrorBadRequest,                      EPhoneNoteUnableToMakeVideoCallNotSupportedByOtherPhone );
    iMockContext->Reset();
    
    EUNIT_PRINT( _L( "CS Voice call and ECCPErrorBadRequest" ) );
     iMockContext->InitializeL();
    VerifyNote( EPECallTypeCSVoice, ECCPErrorBadRequest,                      EPhoneErrorInConnection );
    }


// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphoneerrormessageshandler,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "ShowErrorSpecificNoteForVideoCallL - test1",
    "CPhoneErrorMessagesHandler",
    "ShowErrorSpecificNoteL - test1",
    "FUNCTIONALITY",
    SetupL, T_Global_ShowErrorSpecificNoteForVideoCallL, Teardown)

EUNIT_TEST(
    "ShowErrorSpecificNoteForCsCallL - test2",
    "CPhoneErrorMessagesHandler",
    "ShowErrorSpecificNoteL - test2",
    "FUNCTIONALITY",
    SetupL, T_Global_ShowErrorSpecificNoteForCsCallL, Teardown)

EUNIT_TEST(
    "ShowErrorSpecificNoteL - test3",
    "CPhoneErrorMessagesHandler",
    "ShowErrorSpecificNoteL - test3",
    "FUNCTIONALITY",
    SetupL, T_Global_ShowErrorSpecificNoteL, Teardown)

EUNIT_END_TEST_TABLE
