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

#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <csmcmockcontext.h>
#include <msmcmockspecbuilder.h>
#include <centralrepository.h>
#include <SettingsInternalCRKeys.h>
#include <LogsDomainCRKeys.h>
#include "UT_CPELogExternalData.h"
#include "cpelogexternaldata.h"
#include "cpeloginfo.h"
#include "cpeengineinfo.h"
#include "cpeloghandling.h"

// - Construction -----------------------------------------------------------
UT_CPELogExternalData* UT_CPELogExternalData::NewL()
    {
    UT_CPELogExternalData* self = UT_CPELogExternalData::NewLC();
    CleanupStack::Pop();
    return self;
    }


UT_CPELogExternalData* UT_CPELogExternalData::NewLC()
    {
    UT_CPELogExternalData* self = new( ELeave ) UT_CPELogExternalData();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


UT_CPELogExternalData::~UT_CPELogExternalData()
    {
    delete iRepositoryMock;
    CSmcMockContext::Release();
    }


// from MPEPhoneModelInternal
MPEDataStore* UT_CPELogExternalData::DataStore()
    {
    return iEngineInfo;
    }


void UT_CPELogExternalData::HandleInternalMessage( const TInt /*aMessage*/ )
    {
    }


void UT_CPELogExternalData::SendMessage( 
    const MEngineMonitor::TPEMessagesFromPhoneEngine /*aMessage*/ )
    {
    }


void UT_CPELogExternalData::SendMessage( 
    const MEngineMonitor::TPEMessagesFromPhoneEngine /*aMessage*/, 
    const TInt /*aCallId*/ )
    {
    }


MPEExternalDataHandler* UT_CPELogExternalData::DataStoreExt()
    {
    return NULL;
    }

CPERemotePartyInfoMediator* UT_CPELogExternalData::MediatorCommunicationHandler()
    {
    return NULL;
    } 

UT_CPELogExternalData::UT_CPELogExternalData()
    {
    }


void UT_CPELogExternalData::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }


// - Test methods -----------------------------------------------------------
void UT_CPELogExternalData::SetupL(  )
    {
    iMockContext->InitializeL();
    
    User::LeaveIfError( iFsSession.Connect() );
    iEngineInfo = CPEEngineInfo::NewL();
    iEngineInfo->SetCallType( EPECallTypeVoIP, 0 );
    
    iLogHandlingMock = static_cast<CPELogHandling*>( 
        CPELogHandling::NewL( *this, iFsSession ) );
    iCPELogExternalData = CPELogExternalData::NewL( *iLogHandlingMock );
    
    iRepositoryMock = CRepository::NewL( KCRUidLogs );
    }


void UT_CPELogExternalData::Teardown(  )
    {
    delete iCPELogExternalData; 
    iCPELogExternalData = NULL;
    delete iEngineInfo;
    iEngineInfo = NULL;
    iFsSession.Close();
    
    delete iLogHandlingMock; 
    iLogHandlingMock = NULL;
    delete iRepositoryMock;
    iRepositoryMock = NULL;
    
    iMockContext->Reset();
    }


void UT_CPELogExternalData::T_UpdateCallTimersL(  )
    {
    CPELogInfo* info = CPELogInfo::NewL();
    const TInt KDuration = 1000000;
    info->SetDuration( KDuration );
    
    // 1) case
    info->SetCurrentLine( CCCECallParameters::ECCELineTypePrimary );
    info->SetCallDirection( RMobileCall::EMobileOriginated );
    info->SetEventType( CPELogInfo::EPEVoIPEvent );
    iMockContext->ExpectCallL( "CRepository::Set" ).
        WithL( KLogsDialledCallsTimerVoIP, KDuration ).
        ReturnsL( KErrNone );
    iCPELogExternalData->UpdateCallTimers( *info );
    
    // 2) case
    info->SetEventType( CPELogInfo::EPEVoiceEvent );
    iMockContext->ExpectCallL( "CRepository::Set" ).
        WithL( KLogsDialledCallsTimerLine1, KDuration ).
        ReturnsL( KErrNone );
    iCPELogExternalData->UpdateCallTimers( *info );
    
    // 3) case
    info->SetCallDirection( RMobileCall::EMobileTerminated );
    info->SetEventType( CPELogInfo::EPEVoIPEvent );
    iMockContext->ExpectCallL( "CRepository::Set" ).
        WithL( KLogsReceivedCallsTimerVoIP, KDuration ).
        ReturnsL( KErrNone );
    iCPELogExternalData->UpdateCallTimers( *info );
    
    // 4) case
    info->SetEventType( CPELogInfo::EPEVoiceEvent );
    iMockContext->ExpectCallL( "CRepository::Set" ).
        WithL( KLogsReceivedCallsTimerLine1, KDuration ).
        ReturnsL( KErrNone );
    iCPELogExternalData->UpdateCallTimers( *info );
    
    // 5) case
    info->SetCurrentLine( CCCECallParameters::ECCELineTypeAux );
    info->SetCallDirection( RMobileCall::EMobileOriginated );
    iMockContext->ExpectCallL( "CRepository::Set" ).
        WithL( KLogsDialledCallsTimerLine2, KDuration ).
        ReturnsL( KErrNone );
    iCPELogExternalData->UpdateCallTimers( *info );
    
    // 6) case
    info->SetCallDirection( RMobileCall::EMobileTerminated );
    iMockContext->ExpectCallL( "CRepository::Set" ).
        WithL( KLogsReceivedCallsTimerLine2, KDuration ).
        ReturnsL( KErrNone );
    iCPELogExternalData->UpdateCallTimers( *info );
    
    // 7) case, error while reading from repository
    iMockContext->ExpectCallL( "CRepository::Get" ).
        ReturnsL( KErrGeneral );
    iMockContext->ExpectCallL( "CRepository::Set" ).TimesL( 0 );
    iCPELogExternalData->UpdateCallTimers( *info );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    
    delete info;
    }


void UT_CPELogExternalData::T_UpdateLastCallTimerByLineL(  )
    {
    CPELogInfo* info = CPELogInfo::NewL();
    const TInt KDuration = 1000000;
    info->SetDuration( KDuration );
    
    // 1) case, primary line and VoIP call
    info->SetCurrentLine( CCCECallParameters::ECCELineTypePrimary );
    info->SetEventType( CPELogInfo::EPEVoIPEvent );
    iMockContext->ExpectCallL( "CRepository::Set" ).
        WithL( KLogsLastCallTimerVoIP, KDuration ).
        ReturnsL( KErrNone );
    iCPELogExternalData->UpdateLastCallTimerByLine( *info );
    
    // 2) case, primary line and Voice call
    info->SetEventType( CPELogInfo::EPEVoiceEvent );
    iMockContext->ExpectCallL( "CRepository::Set" ).
        WithL( KLogsLastCallTimerLine1, KDuration ).
        ReturnsL( KErrNone );
    iCPELogExternalData->UpdateLastCallTimerByLine( *info );
    
    // 3) case, aux line
    info->SetCurrentLine( CCCECallParameters::ECCELineTypeAux );
    iMockContext->ExpectCallL( "CRepository::Set" ).
        WithL( KLogsLastCallTimerLine2, KDuration ).
        ReturnsL( KErrNone );
    iCPELogExternalData->UpdateLastCallTimerByLine( *info );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    
    delete info;
    }


void UT_CPELogExternalData::T_UpdateLastCallTimerL(  )
    {
    CPELogInfo* info = CPELogInfo::NewL();
    const TInt KDuration = 1000000;
    info->SetDuration( KDuration );
    
    // 1) case, primary line and VoIP call
    info->SetCurrentLine( CCCECallParameters::ECCELineTypePrimary );
    info->SetEventType( CPELogInfo::EPEVoIPEvent );
    iMockContext->ExpectCallL( "CRepository::Set" ).
        WithL( KLogsActualLastCallTimer, KDuration ).
        ReturnsL( KErrNone );
    
    iCPELogExternalData->UpdateLastCallTimer( *info );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    
    delete info;
    }

void UT_CPELogExternalData::T_IncreaseMissedCallValueL(  )
    {
    TInt callId(0);
 
    iMockContext->ExpectCallL( "CRepository::Get" ).
        ReturnsL( KErrNone );
    
    iMockContext->ExpectCallL( "CRepository::Set" ).
        ReturnsL( KErrNone );
    
    iCPELogExternalData->IncreaseMissedCallValue( callId );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }


void UT_CPELogExternalData::T_SetFirstCallStartTimeL(  )
    {
    TTime time( 0 );
    iCPELogExternalData->SetFirstCallStartTime( time );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }


// - EUnit test table -------------------------------------------------------
EUNIT_BEGIN_TEST_TABLE(
    UT_CPELogExternalData,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "UpdateCallTimers",
    "CPELogExternalData",
    "UpdateCallTimers",
    "FUNCTIONALITY",
    SetupL, T_UpdateCallTimersL, Teardown)
    
EUNIT_TEST(
    "UpdateLastCallTimerByLine",
    "CPELogExternalData",
    "UpdateLastCallTimerByLine",
    "FUNCTIONALITY",
    SetupL, T_UpdateLastCallTimerByLineL, Teardown)

    
EUNIT_TEST(
    "UpdateLastCallTimer",
    "CPELogExternalData",
    "UpdateLastCallTimer",
    "FUNCTIONALITY",
    SetupL, T_UpdateLastCallTimerL, Teardown)
    
EUNIT_TEST(
    "IncreaseMissedCallValue",
    "CPELogExternalData",
    "IncreaseMissedCallValue",
    "FUNCTIONALITY",
    SetupL, T_IncreaseMissedCallValueL, Teardown)
    
EUNIT_TEST(
    "SetFirstCallStartTime",
    "CPELogExternalData",
    "SetFirstCallStartTime",
    "FUNCTIONALITY",
    SetupL, T_SetFirstCallStartTimeL, Teardown)
    

EUNIT_END_TEST_TABLE
