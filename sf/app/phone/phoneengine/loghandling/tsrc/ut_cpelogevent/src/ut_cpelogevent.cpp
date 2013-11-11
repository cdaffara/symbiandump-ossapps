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

#include <eunitmacros.h>
#include <eunitdecorators.h>
#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>

#include <cpeengineinfo.h>
#include <e32cmn.h>
#include <f32file.h>
#include <LOGCLI.H>
#include <LogsApiConsts.h>
#include <mpedatastore.h>

#include "UT_CPELogEvent.h"
#include "cpelogexternaldata.h"
#include "cpeloginfo.h"
#include "cpeloghandling.h"
#include "cpeloghandlingcommand.h"
#include "cpelogevent.h"
#include "testdefs.h"

const TInt KCallId = 0;


TBool AllocTestEnabled()
    {
    CEUnitTestCaseDecorator* decorator 
        = CEUnitTestCaseDecorator::ActiveTestCaseDecorator( 
        KEUnitAllocTestCaseDecoratorName );
    return ( decorator ? ETrue : EFalse );
    }

// - Construction -----------------------------------------------------------
UT_CPELogEvent* UT_CPELogEvent::NewL()
    {
    UT_CPELogEvent* self = UT_CPELogEvent::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

UT_CPELogEvent* UT_CPELogEvent::NewLC()
    {
    UT_CPELogEvent* self = new( ELeave ) UT_CPELogEvent();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

UT_CPELogEvent::~UT_CPELogEvent()
    {
    CSmcMockContext::Release();
    }

TInt UT_CPELogEvent::SaveCallEntry( const TInt aCallId )
    {
    TInt id( aCallId );
    return KErrNone;
    // derived
    }

MPEDataStore* UT_CPELogEvent::DataStore()
    {
    return iDataStore;
    }
void UT_CPELogEvent::HandleInternalMessage( const TInt aMessage )
    {
    TInt mes( aMessage );
    }
void UT_CPELogEvent::SendMessage( 
       const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage )
    {
    TInt mes(aMessage);
    }
void UT_CPELogEvent::SendMessage( 
       const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
       const TInt aCallId )
    {
    TInt mes(aMessage);
    TInt call(aCallId);
    }
MPEExternalDataHandler* UT_CPELogEvent::DataStoreExt()
    {
    return NULL;
    }
    
CPERemotePartyInfoMediator* UT_CPELogEvent::MediatorCommunicationHandler()
    {
    return NULL;
    }    

UT_CPELogEvent::UT_CPELogEvent()
    //iFsSession( *( RFs* )0x1 )
    {
    }

void UT_CPELogEvent::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------
void UT_CPELogEvent::SetupL(  )
    {
    iMockContext->InitializeL();
    
    User::LeaveIfError(iFsSession.Connect());
    iLogClient = CLogClient::NewL( iFsSession );
    
    iLogHandlingMock = static_cast<CPELogHandling*>( 
        CPELogHandling::NewL( *this, iFsSession ) );
    
    iLogInfoMock = CPELogInfo::NewL();
  
    iLogHCMock = new (ELeave) CPELogHandlingCommand( *static_cast<CPELogHandling*>(iLogHandlingMock), *iLogClient );

    iCPELogEvent = CPELogEvent::NewL( *static_cast<CPELogHandling*>(iLogHandlingMock), *iLogHCMock );
    iDataStore = static_cast<MPEDataStore*>( CPEEngineInfo::NewL() );
    
    _LIT( KBuf, "Number" );
    TBuf<10> buf;
    buf.Append( KBuf );
    iDataStore->SetRemotePhoneNumber( buf, 0 );
    iDataStore->SetServiceId( KServiceId, 0 );
    iDataStore->SetCallType( EPECallTypeVoIP, 0 );
    }

void UT_CPELogEvent::Teardown(  )
    {   
    delete iLogInfoMock;
    iLogInfoMock = NULL;
    delete iLogHandlingMock;
    iLogHandlingMock = NULL;
    delete iLogClient;
    iLogClient = NULL;
    delete iCPELogEvent; 
    iCPELogEvent = NULL; 
    delete iLogHCMock;
    iLogHCMock = NULL;
    delete static_cast<CPEEngineInfo*>( iDataStore );
    iDataStore = NULL;
    iFsSession.Close();
    iMockContext->Reset();
    }

void UT_CPELogEvent::T_CPELogEvent_CallIdL()

    {
    iMockContext->ExpectCallL( "CPELogInfo::CallId" );
    
    iCPELogEvent->CallId( );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );  
    }

void UT_CPELogEvent::T_CPELogEvent_UpdateLogInfoL(  )
    {    
    iMockContext->ExpectCallL( "CPELogInfo::CopyL" );
        
    iCPELogEvent->UpdateLogInfoL( *iLogInfoMock );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );  
    }

void UT_CPELogEvent::T_CPELogEvent_ResetLogInfoL(  )
    {
    iMockContext->ExpectCallL( "CPELogInfo::Reset" );
        
    iCPELogEvent->ResetLogInfo( );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );  
    }

void UT_CPELogEvent::T_CPELogEvent_IsCompletedL(  )
    {
    if ( iCPELogEvent->IsCompleted( ) )
        {
        EUNIT_FAIL_TEST( "IsCompleted FAILED return value!");
        }
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void UT_CPELogEvent::T_CPELogEvent_SaveLeaveL(  )
    {
    if ( iCPELogEvent->SaveLeave( ) )
        {
        EUNIT_FAIL_TEST( "SaveLeave FAILED wrong return value!");
        }
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }
    
void UT_CPELogEvent::T_CPELogEvent_SaveLL(  )
    {
    TLogString test;
    _LIT( KMyAddress, "myaddress@domain.com" );
    _LIT( KVoipAddress, "remotecontact@domain.com" );
    
    iMockContext->ExpectCallL( "CPELogHandling::LogStringDelivery" ).
        ReturnsL( test );
    
    iMockContext->ExpectCallL( "CPELogHandling::LogStringOut" ).
        ReturnsL( test );
    
    iMockContext->ExpectCallL( "CPELogInfo::PhoneNumberId" ).
        ReturnsL( EPEUnknownNumber );
    
    iMockContext->ExpectCallL( "CPELogInfo::EventType" ).
        ReturnsL( CPELogInfo::EPEVoiceEvent );
   
    iMockContext->ExpectCallL( "CPELogInfo::CurrentLine" ).
        ReturnsL( CCCECallParameters::ECCELineTypePrimary ).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::CallDirection" ).
        ReturnsL( RMobileCall::EMobileOriginated );

    iMockContext->ExpectCallL( "CPELogInfo::Name" ).
        ReturnsL( KNullDesC() );
    
    iMockContext->ExpectCallL( "CPELogInfo::PhoneNumber" ).
        ReturnsL( KNullDesC() );
    
    iMockContext->ExpectCallL( "CPELogInfo::VoipAddress" ).
        ReturnsL( KVoipAddress()).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::MyAddress" ).
        ReturnsL( KMyAddress()).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::Duration" ).
        ReturnsL( 0 );

    iMockContext->ExpectCallL( "CPELogInfo::ContactLink").
        ReturnsL( KNullDesC() ).
        TimesL( 4 );
    
    iMockContext->ExpectCallL( "CPELogHandlingCommand::AddEvent");
    
    iCPELogEvent->SaveL();
       
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

void UT_CPELogEvent::T_CPELogEvent_SaveL2L(  )
    {
    TLogString test;
    _LIT( KMyAddress, "myaddress@domain.com" );
    _LIT( KVoipAddress, "remotecontact@domain.com" );

    iMockContext->ExpectCallL( "CPELogInfo::CallState" ).
        ReturnsL<TPEState>( EPEStateIdle ).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogHandling::LogStringDelivery" ).
        ReturnsL( test );
    
    iMockContext->ExpectCallL( "CPELogHandling::LogStringOutAlt" ).
        ReturnsL( test );
    
    iMockContext->ExpectCallL( "CPELogInfo::PhoneNumberId" ).
        ReturnsL( EPEUnknownNumber );
    
    iMockContext->ExpectCallL( "CPELogInfo::EventType" ).
        ReturnsL( CPELogInfo::EPEVoIPEvent );
  
    iMockContext->ExpectCallL( "CPELogInfo::CurrentLine" ).
        ReturnsL( CCCECallParameters::ECCELineTypeAux ).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::CallDirection" ).
        ReturnsL( RMobileCall::EMobileOriginated );

    iMockContext->ExpectCallL( "CPELogInfo::Name" ).
        ReturnsL( KNullDesC() );
    
    iMockContext->ExpectCallL( "CPELogInfo::PhoneNumber" ).
        ReturnsL( KNullDesC() );
    
    iMockContext->ExpectCallL( "CPELogInfo::VoipAddress" ).
        ReturnsL( KVoipAddress()).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::MyAddress" ).
        ReturnsL( KMyAddress()).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::Duration" ).
        ReturnsL( 0 );

    iMockContext->ExpectCallL( "CPELogInfo::ContactLink").
        ReturnsL( KNullDesC() ).
        TimesL( 4 );
    
    iMockContext->ExpectCallL( "CPELogHandlingCommand::AddEvent");
     
    iCPELogEvent->SaveL();
       
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

void UT_CPELogEvent::T_CPELogEvent_SaveL3L(  )
    {
    TLogString test;
    _LIT( KMyAddress, "myaddress@domain.com" );
    _LIT( KVoipAddress, "remotecontact@domain.com" );

    iMockContext->ExpectCallL( "CPELogInfo::ForcedCompletion" ).
        ReturnsL( ETrue );
   
    iMockContext->ExpectCallL( "CPELogInfo::CurrentLine" ).
        ReturnsL( CCCECallParameters::ECCELineTypePrimary ).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::CallDirection" ).
        ReturnsL( RMobileCall::EMobileTerminated ).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogHandling::LogStringIn" ).
        ReturnsL( test );
    
    iMockContext->ExpectCallL( "CPELogHandling::LogStringDelivery" ).
        ReturnsL( test );

    iMockContext->ExpectCallL( "CPELogInfo::Name" ).
        ReturnsL( KNullDesC() );
    
    iMockContext->ExpectCallL( "CPELogInfo::PhoneNumber" ).
        ReturnsL( KNullDesC() );
    
    iMockContext->ExpectCallL( "CPELogInfo::VoipAddress" ).
        ReturnsL( KVoipAddress()).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::MyAddress" ).
        ReturnsL( KMyAddress()).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::Duration" ).
        ReturnsL( 0 );
    
    iMockContext->ExpectCallL( "CPELogHandlingCommand::ChangeEvent");
    
    iCPELogEvent->SetAdded();
 
    iCPELogEvent->SaveL();
       
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

void UT_CPELogEvent::T_CPELogEvent_SaveL4L(  )
    {
    TLogString test;
    _LIT( KVoipAddress, "remotecontact@domain.com" );

    iMockContext->ExpectCallL( "CPELogInfo::CallState" ).
        ReturnsL<TPEState>( EPEStateIdle );
    
    iMockContext->ExpectCallL( "CPELogHandling::LogStringDelivery" ).
        ReturnsL( test );
    
    iMockContext->ExpectCallL( "CPELogHandling::LogStringMissed" ).
        ReturnsL( test );
    
    iMockContext->ExpectCallL( "CPELogInfo::PhoneNumberId" ).
        ReturnsL( EPEUnknownNumber );
    
    iMockContext->ExpectCallL( "CPELogInfo::EventType" ).
        ReturnsL( CPELogInfo::EPEVideoCallEvent );
  
    iMockContext->ExpectCallL( "CPELogInfo::CurrentLine" ).
        ReturnsL( CCCECallParameters::ECCELineTypeAux );
    
    iMockContext->ExpectCallL( "CPELogInfo::CallDirection" ).
        ReturnsL( RMobileCall::EMobileTerminated ).
        TimesL( 2 );

    iMockContext->ExpectCallL( "CPELogInfo::Name" ).
        ReturnsL( KNullDesC() );
    
    iMockContext->ExpectCallL( "CPELogInfo::PhoneNumber" ).
        ReturnsL( KNullDesC() );
    
    iMockContext->ExpectCallL( "CPELogInfo::VoipAddress" ).
        ReturnsL( KVoipAddress()).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::MyAddress" ).
        ReturnsL( KNullDesC());
    
    iMockContext->ExpectCallL( "CPELogInfo::Duration" ).
        ReturnsL( 0 );

    iMockContext->ExpectCallL( "CPELogInfo::ContactLink").
        ReturnsL( KNullDesC() ).
        TimesL( 4 );
    
    iMockContext->ExpectCallL( "CPELogInfo::CallState" ).
        ReturnsL<TPEState>( EPEStateIdle ).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::MissedCall" ).
        ReturnsL( ETrue );
    
    iMockContext->ExpectCallL( "CPELogHandlingCommand::AddEvent");
     
    iCPELogEvent->SaveL();
       
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

void UT_CPELogEvent::T_CPELogEvent_SaveL5L(  )
    {
    TLogString test;

    _LIT( KMyNumber, "040665544" );
    _LIT( KMyName, "Tester" );

    iMockContext->ExpectCallL( "CPELogInfo::CallState" ).
        ReturnsL<TPEState>( EPEStateIdle );
    
    iMockContext->ExpectCallL( "CPELogHandling::LogStringDelivery" ).
        ReturnsL( test );
    
    iMockContext->ExpectCallL( "CPELogHandling::LogStringInAlt" ).
        ReturnsL( test );
    
    iMockContext->ExpectCallL( "CPELogInfo::PhoneNumberId" ).
        ReturnsL( EPEUnknownNumber );
    
    iMockContext->ExpectCallL( "CPELogInfo::EventType" ).
        ReturnsL( CPELogInfo::EPEUnknownEvent );
  
    iMockContext->ExpectCallL( "CPELogInfo::CurrentLine" ).
        ReturnsL( CCCECallParameters::ECCELineTypeAux ).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::CallDirection" ).
        ReturnsL( RMobileCall::EMobileTerminated ).
        TimesL( 2 );

    iMockContext->ExpectCallL( "CPELogInfo::Name" ).
        ReturnsL( KMyName() ).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::PhoneNumber" ).
        ReturnsL( KMyNumber() ).
        TimesL( 2 );
    
    iMockContext->ExpectCallL( "CPELogInfo::VoipAddress" ).
        ReturnsL(  KNullDesC());
    
    iMockContext->ExpectCallL( "CPELogInfo::MyAddress" ).
        ReturnsL( KNullDesC());
    
    iMockContext->ExpectCallL( "CPELogInfo::Duration" ).
        ReturnsL( 0 );

    iMockContext->ExpectCallL( "CPELogInfo::ContactLink").
        ReturnsL( KNullDesC() ).
        TimesL( 4 );
    
    iMockContext->ExpectCallL( "CPELogHandlingCommand::AddEvent");
     
    iCPELogEvent->SaveL();
       
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() ); 
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    UT_CPELogEvent,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "CallId - test1",
    "CPELogEvent",
    "CallId - test1",
    "FUNCTIONALITY",
    SetupL, T_CPELogEvent_CallIdL, Teardown)
   
EUNIT_TEST(
    "UpdateLogInfo - test2",
    "CPELogEvent",
    "UpdateLogInfo - test2",
    "FUNCTIONALITY",
    SetupL, T_CPELogEvent_UpdateLogInfoL, Teardown)
    
EUNIT_TEST(
    "ResetLogInfo - test3",
    "CPELogEvent",
    "ResetLogInfo - test3",
    "FUNCTIONALITY",
    SetupL, T_CPELogEvent_ResetLogInfoL, Teardown)
    
EUNIT_TEST(
    "IsCompleted - test4",
    "CPELogEvent",
    "IsCompleted - test4",
    "FUNCTIONALITY",
    SetupL, T_CPELogEvent_IsCompletedL, Teardown)
    
EUNIT_TEST(
    "SaveLeave - test5",
    "CPELogEvent",
    "SaveLeave - test5",
    "FUNCTIONALITY",
    SetupL, T_CPELogEvent_SaveLeaveL, Teardown)
    
EUNIT_TEST(
    "SaveL - test6",
    "CPELogEvent",
    "SaveL - test6",
    "FUNCTIONALITY",
    SetupL, T_CPELogEvent_SaveLL, Teardown)
    
EUNIT_TEST(
    "SaveL 2 - test7",
    "CPELogEvent",
    "SaveL 2 - test7",
    "FUNCTIONALITY",
    SetupL, T_CPELogEvent_SaveL2L, Teardown)
    
EUNIT_TEST(
    "SaveL 3 - test8",
    "CPELogEvent",
    "SaveL 3 - test8",
    "FUNCTIONALITY",
    SetupL, T_CPELogEvent_SaveL3L, Teardown)
    
EUNIT_TEST(
    "SaveL 4 - test9",
    "CPELogEvent",
    "SaveL 4 - test9",
    "FUNCTIONALITY",
    SetupL, T_CPELogEvent_SaveL4L, Teardown)
    
EUNIT_TEST(
    "SaveL 5 - test10",
    "CPELogEvent",
    "SaveL 5 - test10",
    "FUNCTIONALITY",
    SetupL, T_CPELogEvent_SaveL5L, Teardown)  

EUNIT_END_TEST_TABLE
