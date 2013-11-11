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

#include "T_MediatorSender.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include <PEVirtualEngine.h>
#include <mediatoreventsfromtelephonyapi.h>
#include <videotelcontrolmediatorapi.h>
#include <MediatorEventProvider.h>
//#include <bmbubblemanager.h>
#include "mphoneshutdowncommand.h"
#include "phoneui.pan"
#include "TPhoneCmdParamBoolean.h"
#include "TPhoneCmdParamCallHeaderData.h"
#include "PhoneViewCommandDefinitions.h"
#include "CPhoneMediatorSender.h"
#include "MediatorDomainUIDs.h"

CPhoneMediatorSender* coeEnvMediatorSenderInstance;

// Mediator registeration
TInt mediatorRegisterationReturnValue;
TUid domain;
TUid category;
MediatorService::REventList eventList;

// Raise event
TUid raiseEventDomain;
TUid raiseEventCategory;
TInt raiseEventEventId;
TVersion raiseEventVersion;
HBufC8* raiseEventData;

// Issue command
TInt issueCommandReturnValue;
TUid issueCommandDomain;
TUid issueCommandCategory;
TInt issueCommandCommandId;
TVersion issueCommandVersion;
HBufC8* issueCommandData;




// - Construction -----------------------------------------------------------

T_MediatorSender* T_MediatorSender::NewL()
    {
    T_MediatorSender* self = T_MediatorSender::NewLC();
    CleanupStack::Pop();
    return self;
    }

T_MediatorSender* T_MediatorSender::NewLC()
    {
    T_MediatorSender* self = new( ELeave ) T_MediatorSender();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

T_MediatorSender::~T_MediatorSender()
    {
    }

T_MediatorSender::T_MediatorSender()
    {
    }

void T_MediatorSender::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

class CShutdownCommandMock : public CBase, public MPhoneShutdownCommand
    {
public:
    
    static CShutdownCommandMock* NewL() { return new( ELeave )CShutdownCommandMock(); }
    
    void ExecuteLD() { iExecuteLDCalled = ETrue; }
    
    TBool iExecuteLDCalled;
    };

// - Test methods -----------------------------------------------------------



void T_MediatorSender::SetupL(  )
    {
    eventList = MediatorService::REventList();
    coeEnvMediatorSenderInstance = NULL;
    domain = TUid::Null();
    category = TUid::Null();
    mediatorRegisterationReturnValue = KErrNone;
    issueCommandDomain = raiseEventDomain = TUid::Null();
    issueCommandCategory = raiseEventCategory = TUid::Null();
    issueCommandCommandId = raiseEventEventId = 0;
    issueCommandVersion = raiseEventVersion = TVersion(); 
    issueCommandData = raiseEventData = NULL;
    issueCommandReturnValue = KErrNone;
    iSender = CPhoneMediatorSender::Instance();
    }    

void T_MediatorSender::Teardown(  )
    {
    eventList.Close();
    delete iSender;
    delete raiseEventData;
    delete issueCommandData;
    }

void T_MediatorSender::VerifyIssueCommandBoilerplateL()
    {
    EUNIT_ASSERT( issueCommandDomain == KMediatorVideoTelephonyDomain );
    EUNIT_ASSERT( issueCommandCategory == KCatPhoneToVideotelCommands );
    EUNIT_ASSERT( issueCommandVersion.iMajor == KPhoneToVideotelCmdVersionMajor );
    EUNIT_ASSERT( issueCommandVersion.iMinor == KPhoneToVideotelCmdVersionMinor );
    EUNIT_ASSERT( issueCommandVersion.iBuild == KPhoneToVideotelCmdVersionBuild );
    }

void T_MediatorSender::VerifyRaiseEventBoilerplateL()
    {
    EUNIT_ASSERT( raiseEventDomain == KMediatorTelephonyDomain );
    EUNIT_ASSERT( raiseEventCategory == KCatEventsFromTelephony );
    EUNIT_ASSERT( raiseEventEventId == EPhoneEventCallData );
    EUNIT_ASSERT( raiseEventVersion.iMajor == KTelephonyEventsVersionMajor );
    EUNIT_ASSERT( raiseEventVersion.iMinor == KTelephonyEventsVersionMinor );
    EUNIT_ASSERT( raiseEventVersion.iBuild == KTelephonyEventsVersionBuild );

    }
    
void T_MediatorSender::FreeIssueCommandGlobals()
    {
    issueCommandDomain = TUid::Null();
    issueCommandCategory = TUid::Null();
    issueCommandCommandId = 0;
    issueCommandVersion = TVersion(); 
    delete issueCommandData; issueCommandData = NULL; 
    }

void T_MediatorSender::TestInstanceGotFromCoeEnvL()
    {
    coeEnvMediatorSenderInstance = reinterpret_cast<CPhoneMediatorSender*>( 0xdeadbeef );
    
    CPhoneMediatorSender* instance = CPhoneMediatorSender::Instance();
    EUNIT_ASSERT( instance == coeEnvMediatorSenderInstance );
    }

void T_MediatorSender::TestInstanceCreatedL()
    {
    CPhoneMediatorSender* instance = CPhoneMediatorSender::Instance();
    EUNIT_ASSERT( instance != NULL );
    delete instance;
    }

void T_MediatorSender::TestGenericEventsAreRegisteredL()
    {    
    EUNIT_ASSERT( domain == KMediatorTelephonyDomain );
    EUNIT_ASSERT( category == KCatEventsFromTelephony );
    EUNIT_ASSERT( eventList.Count() == 1 );
    MediatorService::TEvent event = eventList[0];
    EUNIT_ASSERT( event.iEventId == EPhoneEventCallData );
    EUNIT_ASSERT( event.iVersion.iMajor == KTelephonyEventsVersionMajor );
    EUNIT_ASSERT( event.iVersion.iMinor == KTelephonyEventsVersionMinor );
    EUNIT_ASSERT( event.iVersion.iBuild == KTelephonyEventsVersionBuild );
    }

void T_MediatorSender::TestGenericEventRegisterationFailsL()
    {    
    mediatorRegisterationReturnValue = KErrGeneral;
    EUNIT_ASSERT_PANIC( CPhoneMediatorSender::Instance(), "Common phone", EPhoneMediatorCenterRegistrationFailed  );
    }

void T_MediatorSender::TestSendingCallDataWhenCallHeaderIsRemovedL()
    {       
    iSender->SendEvent( EPhoneViewRemoveCallHeader, 2 );
    
    VerifyRaiseEventBoilerplateL();
    
    TTelephonyCallDataParamPackage pckg;
    pckg.Copy( *raiseEventData );
    const TTelephonyCallDataParam params = pckg();  

    EUNIT_ASSERT( params.iCallId == 2 );
    EUNIT_ASSERT( params.iCallState == ECallStateIdle );   
    }

void T_MediatorSender::TestSendingAudioMuteCommandL()
    {
    TPhoneCmdParamBoolean param;
    param.SetBoolean( ETrue );
    
    iSender->SendEvent( EPhoneViewActivateMuteUIChanges, param );
    
    VerifyIssueCommandBoilerplateL();
    EUNIT_ASSERT( issueCommandCommandId == EVtCmdMute );
    EUNIT_ASSERT( issueCommandData->Match( KNullDesC8 ) == KErrNone );  
    }

void T_MediatorSender::TestSendingAudioUnMuteCommandL()
    {
    TPhoneCmdParamBoolean param;
    param.SetBoolean( EFalse );
    
    iSender->SendEvent( EPhoneViewActivateMuteUIChanges, param );
    
    VerifyIssueCommandBoilerplateL();
    EUNIT_ASSERT( issueCommandCommandId == EVtCmdUnmute );
    EUNIT_ASSERT( issueCommandData->Match( KNullDesC8 ) == KErrNone );  
    }

void T_MediatorSender::TestAudioMuteCommandBufferedIfMediatorBusyAndResendWhenResponseArrivesL()
    {
    issueCommandReturnValue = KErrInUse; // Make the mediator busy
    
    TPhoneCmdParamBoolean param;
    param.SetBoolean( ETrue );
    iSender->SendEvent( EPhoneViewActivateMuteUIChanges, param );
    
    // Clear the issue command sensing globals, because SendEvent tries to send the data and we need to verify
    // that when response arrives the mute command is send again.
    FreeIssueCommandGlobals();
    
    iSender->CommandResponseL( TUid::Null(), TUid::Null(), 0, 0, KNullDesC8() );
    
    
    VerifyIssueCommandBoilerplateL();
    EUNIT_ASSERT( issueCommandCommandId == EVtCmdMute );
    EUNIT_ASSERT( issueCommandData->Match( KNullDesC8 ) == KErrNone ); 
    
    // Verify that next response wont send the mute command
    FreeIssueCommandGlobals();
    iSender->CommandResponseL( TUid::Null(), TUid::Null(), 0, 0, KNullDesC8() );
    
    EUNIT_ASSERT( issueCommandDomain == TUid::Null() );
    }


// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    T_MediatorSender,
    "Tests for CPhoneMediatorSender",
    "UNIT" )

EUNIT_TEST(
    "Instance got from coeenv",
    "CPhoneMediatorSender",
    "Instance",
    "FUNCTIONALITY",
    SetupL, TestInstanceGotFromCoeEnvL, Teardown)
    
EUNIT_TEST(
    "Instance created",
    "CPhoneMediatorSender",
    "Instance",
    "FUNCTIONALITY",
    SetupL, TestInstanceCreatedL, Teardown)
    
EUNIT_TEST(
    "Generic event registeration",
    "CPhoneMediatorSender",
    "SendEvent",
    "FUNCTIONALITY",
    SetupL, TestGenericEventsAreRegisteredL, Teardown)
    
EUNIT_TEST(
    "Generic event registeration fails",
    "CPhoneMediatorSender",
    "SendEvent",
    "FUNCTIONALITY",
    SetupL, TestGenericEventRegisterationFailsL, Teardown)

    
EUNIT_TEST(
    "Sending call data when call header is removed",
    "CPhoneMediatorSender",
    "SendEvent",
    "FUNCTIONALITY",
    SetupL, TestSendingCallDataWhenCallHeaderIsRemovedL, Teardown)

EUNIT_TEST(
    "Sending audio mute command",
    "CPhoneMediatorSender",
    "SendEvent",
    "FUNCTIONALITY",
    SetupL, TestSendingAudioMuteCommandL, Teardown)

EUNIT_TEST(
    "Sending audio unmute command",
    "CPhoneMediatorSender",
    "SendEvent",
    "FUNCTIONALITY",
    SetupL, TestSendingAudioUnMuteCommandL, Teardown)
    
EUNIT_TEST(
    "Mute command sending buffered",
    "CPhoneMediatorSender",
    "CommandResponseL",
    "FUNCTIONALITY",
    SetupL, TestAudioMuteCommandBufferedIfMediatorBusyAndResendWhenResponseArrivesL, Teardown)

EUNIT_END_TEST_TABLE
