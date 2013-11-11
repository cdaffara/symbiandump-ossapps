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

#include <e32cmn.h>
#include <f32file.h>
#include <LOGCLI.H>
#include <LogsApiConsts.h>
#include <cccecallparameters.h>
#include <cpeengineinfo.h>
#include <mpedatastore.h>

#include "UT_CPELogInfo.h"
#include "cpeloginfo.h"

const TInt KCallId = 0;


TBool AllocTestEnabled()
    {
    CEUnitTestCaseDecorator* decorator 
        = CEUnitTestCaseDecorator::ActiveTestCaseDecorator( 
        KEUnitAllocTestCaseDecoratorName );
    return ( decorator ? ETrue : EFalse );
    }

// - Construction -----------------------------------------------------------
UT_CPELogInfo* UT_CPELogInfo::NewL()
    {
    UT_CPELogInfo* self = UT_CPELogInfo::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

UT_CPELogInfo* UT_CPELogInfo::NewLC()
    {
    UT_CPELogInfo* self = new( ELeave ) UT_CPELogInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

UT_CPELogInfo::~UT_CPELogInfo()
    {
    }

TInt UT_CPELogInfo::SaveCallEntry( const TInt aCallId )
    {
    TInt id( aCallId );
    return KErrNone;
    // derived
    }


MPEDataStore* UT_CPELogInfo::DataStore()
    {
    return iDataStore;
    }

void UT_CPELogInfo::HandleInternalMessage( const TInt aMessage )
    {
    TInt mes( aMessage );
    }
void UT_CPELogInfo::SendMessage( 
       const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage )
    {
    TInt mes(aMessage);
    }
void UT_CPELogInfo::SendMessage( 
       const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
       const TInt aCallId )
    {
    TInt mes(aMessage);
    TInt call(aCallId);
    }
MPEExternalDataHandler* UT_CPELogInfo::DataStoreExt()
    {
    return NULL;
    }
    
CPERemotePartyInfoMediator* UT_CPELogInfo::MediatorCommunicationHandler()
    {
    return NULL;
    }        

UT_CPELogInfo::UT_CPELogInfo()
    //iFsSession( *( RFs* )0x1 )
    {
    }

void UT_CPELogInfo::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------
void UT_CPELogInfo::SetupL(  )
    {
    }

void UT_CPELogInfo::Teardown(  )
    {   
    }

void UT_CPELogInfo::T_CPELogInfo_VoIPAddressL()
    {
    CPELogInfo* info = CPELogInfo::NewL();
    
    TPEPhoneNumber voipAdderess;
    voipAdderess.Copy( TPtrC8( ( TText8* )( "remotecontact@domain.com" ) ) );
 
    info->SetVoipAddress(  voipAdderess );
    
    if ( voipAdderess != info->VoipAddress() )
        {
        EUNIT_FAIL_TEST( "VoIPAddressL FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_EventTypeL()
    { 
    CPELogInfo* info = CPELogInfo::NewL();
  
    info->SetEventType(  CPELogInfo::EPEVoIPEvent );
    CPELogInfo::TPELogEventType eventType = info->EventType();
    
    if ( CPELogInfo::EPEVoIPEvent != eventType )
        {
        EUNIT_FAIL_TEST( "EventTypeL FAILED wrong return value!");
        }
    
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_CallDirectionL()
    { 
    CPELogInfo* info = CPELogInfo::NewL();
    info->SetCallDirection(  RMobileCall::EMobileOriginated );
    
    if ( RMobileCall::EMobileOriginated != info->CallDirection() )
        {
        EUNIT_FAIL_TEST( "CallDirectionL FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_CallIdL()
    {
    TInt callId( 2 );
    CPELogInfo* info = CPELogInfo::NewL();
    info->SetCallId( callId );
    
    if ( callId != info->CallId() )
        {
        EUNIT_FAIL_TEST( "CallId FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_PhoneNumberL()
    {
    CPELogInfo* info = CPELogInfo::NewL(); 
    
    TPEPhoneNumber phoneNumber;
    phoneNumber.Copy( TPtrC8( ( TText8* )( "0123456789pw*#" ) ) );
    
    info->SetPhoneNumber(  phoneNumber );
    
    if ( phoneNumber != info->PhoneNumber() )
        {
        EUNIT_FAIL_TEST( "PhoneNumber FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_NameL()
    {
    CPELogInfo* info = CPELogInfo::NewL();

    TPEContactName name;
    name.Copy( TPtrC8( ( TText8* )( "Tester 007" ) ) );
    
    info->SetName(  name );
    
    if ( name != info->Name() )
        {
        EUNIT_FAIL_TEST( "Name FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_CallstateL()
    {
    CPELogInfo* info = CPELogInfo::NewL();
  
    info->SetCallState(  EPEStateRinging );
    
    if ( EPEStateRinging != info->CallState() )
        {
        EUNIT_FAIL_TEST( "CallState FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_CallDurationL()
    {
    TTimeIntervalSeconds duration(10000);
    CPELogInfo* info = CPELogInfo::NewL();
  
    info->SetDuration(  duration );
    
    if ( duration != info->Duration() )
        {
        EUNIT_FAIL_TEST( "CallDuration FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_CurrentLineL()
    {
    CPELogInfo* info = CPELogInfo::NewL();
  
    CCCECallParameters::TCCELineType currentLine( CCCECallParameters::ECCELineTypePrimary );
    
    info->SetCurrentLine(  currentLine );
    
    if ( currentLine != info->CurrentLine() )
        {
        EUNIT_FAIL_TEST( "CallDuration FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_PhoneNumberIdL()
    {
    CPELogInfo* info = CPELogInfo::NewL();
  
    TPEPhoneNumberIdType phoneNumberId( EPEVoipNumber );
    
    info->SetPhoneNumberId( phoneNumberId );
    
    if ( phoneNumberId != info->PhoneNumberId() )
        {
        EUNIT_FAIL_TEST( "PhoneNumberId FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_LoggingEnabledL()
    {
    CPELogInfo* info = CPELogInfo::NewL();
    
    info->SetLoggingEnabled( ETrue );
    
    if ( ETrue != info->LoggingEnabled() )
        {
        EUNIT_FAIL_TEST( "LoggingEnabled FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_CallStartTimeL()
    {
    TTime startTime(1000);
    CPELogInfo* info = CPELogInfo::NewL();
  
    info->SetCallStartTime(  startTime );
    
    if ( startTime != info->CallStartTime() )
        {
        EUNIT_FAIL_TEST( "CallStartTime FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_MissedCallL()
    {
    CPELogInfo* info = CPELogInfo::NewL();
    
    info->SetMissedCall( ETrue );
    
    if ( ETrue != info->MissedCall() )
        {
        EUNIT_FAIL_TEST( "MissedCall FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_ForcedCompletionL()
    {
    CPELogInfo* info = CPELogInfo::NewL();
    
    info->SetForcedCompletion( ETrue );
    
    if ( ETrue != info->ForcedCompletion() )
        {
        EUNIT_FAIL_TEST( "ForcedCompletion FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_ServiceIdL()
    {
    CPELogInfo* info = CPELogInfo::NewL();
  
    TUint32 serviceId( 12 );
    
    info->SetServiceId( serviceId );
    
    if ( serviceId != info->ServiceId() )
        {
        EUNIT_FAIL_TEST( "serviceId FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_MyAddressL()
    {
    CPELogInfo* info = CPELogInfo::NewL();

    TPEPhoneNumber myAddress;
    myAddress.Copy( TPtrC8( ( TText8* )( "myaddress@domain.com" ) ) );
 
    info->SetMyAddress(  myAddress );
 
    if ( myAddress != info->MyAddress() )
        {
        EUNIT_FAIL_TEST( "MyAddress FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_ContactLinkL()
    {
    CPELogInfo* info = CPELogInfo::NewL();

    HBufC8* linkToContact = NULL;
 
    info->SetContactLink(  linkToContact );

    if ( &info->ContactLink() != linkToContact )
        {
        EUNIT_FAIL_TEST( "ContactLink FAILED wrong return value!");
        }
    delete info;
    }

void UT_CPELogInfo::T_CPELogInfo_CopyLogInfoL()
    {
    CPELogInfo* infoSource = CPELogInfo::NewL();
    CPELogInfo* infoDestination = CPELogInfo::NewL();
    
    TPEPhoneNumber myAddress;
    myAddress.Copy( TPtrC8( ( TText8* )( "myaddress@domain.com" ) ) );
 
    infoSource->SetMyAddress(  myAddress );
  
    infoDestination->CopyL(  *infoSource );

    if ( infoSource->MyAddress() != infoDestination->MyAddress() )
        {
        EUNIT_FAIL_TEST( "CopyLogInfoL case 1 FAILED wrong return value!");
        }
    
    infoDestination->Reset();
    
    if ( infoSource->MyAddress() == infoDestination->MyAddress() )
        {
        EUNIT_FAIL_TEST( "CopyLogInfoL case 2 FAILED wrong return value!");
        }
    
    delete infoSource;
    delete infoDestination;
    }


void UT_CPELogInfo::T_CPELogInfo_SetEventDataL()
    {       
    TPEContactName name;
    name.Copy( TPtrC8( ( TText8* )( "remotecontact" ) ) );
  
    CPELogInfo* info = CPELogInfo::NewL();
    iDataStore = static_cast<MPEDataStore*>( CPEEngineInfo::NewL() );
    
    iDataStore->SetRemoteName( name, KCallId );
    
    info->SetEventData( KCallId, *DataStore() );
    
    if ( name != info->Name() )
        {
        EUNIT_FAIL_TEST( "SetEventDataL FAILED wrong return value!");
        }

    delete static_cast<CPEEngineInfo*>( iDataStore );
    delete info;
    }

/**
 * Copy event data.
 * 
 * @since S60 v5.0
 * @param aLogInfo Log info to be copied
 */
void CopyL( const CPELogInfo& aLogInfo );

/**
 * Restore default values.
 * 
 * @since S60 v5.0
 */
void Reset();

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    UT_CPELogInfo,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "VoIP Address - test1",
    "CPELogInfo",
    "VoIP Address - test1",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_VoIPAddressL, Teardown)
   
EUNIT_TEST(
    "EventType - test2",
    "CPELogInfo",
    "EventType - test2",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_EventTypeL, Teardown)
    
EUNIT_TEST(
    "CallDirection - test3",
    "CPELogInfo",
    "CallDirection - test3",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_CallDirectionL, Teardown)
        
EUNIT_TEST(
    "CallId - test4",
    "CPELogInfo",
    "CallId - test4",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_CallIdL, Teardown)
    
EUNIT_TEST(
    "PhoneNumber - test5",
    "CPELogInfo",
    "PhoneNumber - test5",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_PhoneNumberL, Teardown)
    
EUNIT_TEST(
    "NameL - test6",
    "CPELogInfo",
    "NameL - test6",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_NameL, Teardown)
    
EUNIT_TEST(
    "CallStateL - test7",
    "CPELogInfo",
    "CallStateL - test7",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_CallstateL, Teardown)
    
EUNIT_TEST(
    "Call duration - test8",
    "CPELogInfo",
    "Call duration - test8",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_CallDurationL, Teardown)
    
EUNIT_TEST(
    "Current line - test9",
    "CPELogInfot",
    "Current line - test9",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_CurrentLineL, Teardown)
    
EUNIT_TEST(
    "PhoneNumberId - test10",
    "CPELogInfo",
    "PhoneNumberId - test10",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_PhoneNumberIdL, Teardown)
    
EUNIT_TEST(
    "LoggingEnabledL - test11",
    "CPELogInfo",
    "LoggingEnabledL - test11",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_LoggingEnabledL, Teardown)  
    

EUNIT_TEST(
    "CallStartTimeL - test12",
    "CPELogInfo",
    "CallStartTimeL - test12",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_CallStartTimeL, Teardown) 
    
EUNIT_TEST(
    "MissedCall - test13",
    "CPELogInfo",
    "MissedCall - test13",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_MissedCallL, Teardown) 
    
EUNIT_TEST(
    "ForcedCompletion - test14",
    "CPELogInfo",
    "ForcedCompletion - test14",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_ForcedCompletionL, Teardown) 

EUNIT_TEST(
    "ServiceId - test145",
    "CPELogInfo",
    "ServiceId - test15",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_ServiceIdL, Teardown)

EUNIT_TEST(
    "MyAddress - test16",
    "CPELogInfo",
    "MyAddress - test16",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_MyAddressL, Teardown)
    

EUNIT_TEST(
    "ContactLink - test17",
    "CPELogInfo",
    "ContactLink - test17",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_ContactLinkL, Teardown)
    

EUNIT_TEST(
    "CopyLogInfo - test18",
    "CPELogInfo",
    "CopyLogInfo - test18",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_CopyLogInfoL, Teardown)
    

EUNIT_TEST(
    "SetEventDataL - test19",
    "CPELogInfo",
    "SetEventDataL - test19",
    "FUNCTIONALITY",
    SetupL, T_CPELogInfo_SetEventDataL, Teardown)
    
EUNIT_END_TEST_TABLE
