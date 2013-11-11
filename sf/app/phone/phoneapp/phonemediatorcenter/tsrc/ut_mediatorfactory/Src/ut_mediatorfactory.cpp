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

#include "ut_mediatorfactory.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include "mpeengineinfo.h"
#include "phoneui.pan"
#include "globals.h"
#include "CPhoneMediatorFactory.h"

// Globals 
TInt commandListenerInitialised;
Singleton* singleton = NULL;

// - Construction -----------------------------------------------------------

ut_mediatorfactory* ut_mediatorfactory::NewL()
    {
    ut_mediatorfactory* self = ut_mediatorfactory::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_mediatorfactory* ut_mediatorfactory::NewLC()
    {
    ut_mediatorfactory* self = new( ELeave ) ut_mediatorfactory();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_mediatorfactory::~ut_mediatorfactory()
    {
    }

ut_mediatorfactory::ut_mediatorfactory()
    {
    }

void ut_mediatorfactory::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------



void ut_mediatorfactory::SetupL(  )
    {
    commandListenerInitialised = EFalse;
    Singleton::pinstance = 0;
    singleton = Singleton::Instance();
    iFactory = CPhoneMediatorFactory::Instance();
    }
    

void ut_mediatorfactory::Teardown(  )
    {
    delete iFactory;
    if( singleton != NULL )
        {
        delete singleton;
        singleton = NULL;
        }
    }
    

void ut_mediatorfactory::TestInitialisingCommandListenerL(  )
    {
    MPhoneMenuAndCbaEvents* mce = reinterpret_cast<MPhoneMenuAndCbaEvents*>(0xdeadbeef);
    MPhoneEngineMessageSender* ems = reinterpret_cast<MPhoneEngineMessageSender*>(0xdeadbeef);
    
    iFactory->CommandListener( mce, ems, NULL );
    
    EUNIT_ASSERT( commandListenerInitialised );
    }

void ut_mediatorfactory::TestCommandListenerNotInitL(  )
    {
    MPhoneMenuAndCbaEvents* mce = reinterpret_cast<MPhoneMenuAndCbaEvents*>(0xdeadbeef);
    MPhoneEngineMessageSender* ems = reinterpret_cast<MPhoneEngineMessageSender*>(0xdeadbeef);
    MPEEngineInfo* ei = reinterpret_cast<MPEEngineInfo*>( 0xdeadbeef );
    
    iFactory->CommandListener( mce, ems, ei );
    commandListenerInitialised = EFalse;

    // Should not cause initialisation anymore
    iFactory->CommandListener( NULL, NULL, NULL );
    EUNIT_ASSERT( !commandListenerInitialised );
    }

void ut_mediatorfactory::TestMPhoneMenuAndCbaEventsPreCondCheckL(  )
    {
    MPhoneEngineMessageSender* ems = reinterpret_cast<MPhoneEngineMessageSender*>(0xdeadbeef);
    MPEEngineInfo* ei = reinterpret_cast<MPEEngineInfo*>( 0xdeadbeef );
    
    EUNIT_ASSERT_PANIC( iFactory->CommandListener( NULL, ems, ei ),
            "Common phone", EPhoneMediatorCenterParameterNotInitialized );
    
    }

void ut_mediatorfactory::TestMPhoneEngineMessageSenderPreCondCheckL(  )
    {
    MPhoneMenuAndCbaEvents* mce = reinterpret_cast<MPhoneMenuAndCbaEvents*>(0xdeadbeef);
    MPEEngineInfo* ei = reinterpret_cast<MPEEngineInfo*>( 0xdeadbeef );
    
    EUNIT_ASSERT_PANIC( iFactory->CommandListener( mce, NULL, ei ),
            "Common phone", EPhoneMediatorCenterParameterNotInitialized );
    
    }

void ut_mediatorfactory::TestEngineInfoPreCondCheckL()
    {
    EUNIT_ASSERT_PANIC( iFactory->MediatorMessage( 0, 0 ),
                       "Common phone",
                       EPhoneMediatorCenterParameterNotInitialized );    
    }

void ut_mediatorfactory::TestMediatorMessageFactoryCheckL()
    {
    MPhoneMenuAndCbaEvents* mce = reinterpret_cast<MPhoneMenuAndCbaEvents*>(0xdeadbeef); 
    MPhoneEngineMessageSender* ems = reinterpret_cast<MPhoneEngineMessageSender*>(0xdeadbeef);
    MPEEngineInfo* ei = reinterpret_cast<MPEEngineInfo*>(0xdeadbeef);
    iFactory->CommandListener( mce, ems, ei );
    singleton->iLeave = ETrue;
    EUNIT_ASSERT_PANIC( iFactory->MediatorMessage( 0, 0 ),
                        "Common phone",
                        EPhoneMediatorCenterInitializationPanic );
    }

void ut_mediatorfactory::CreateMessageFailCheckL()
    {
    MPhoneMenuAndCbaEvents* mce = reinterpret_cast<MPhoneMenuAndCbaEvents*>(0xdeadbeef); 
    MPhoneEngineMessageSender* ems = reinterpret_cast<MPhoneEngineMessageSender*>(0xdeadbeef);
    MPEEngineInfo* ei = reinterpret_cast<MPEEngineInfo*>(0xdeadbeef);
    iFactory->CommandListener( mce, ems, ei );
    singleton->iCreateMessageLeave = ETrue;
    EUNIT_ASSERT_PANIC( iFactory->MediatorMessage( 0, 0 ),
                        "Common phone",
                        EPhoneMediatorCenterInitializationPanic );
    }

void ut_mediatorfactory::SuccesfulMediatorMessageL()
    {
    MPhoneMenuAndCbaEvents* mce = reinterpret_cast<MPhoneMenuAndCbaEvents*>(0xdeadbeef); 
    MPhoneEngineMessageSender* ems = reinterpret_cast<MPhoneEngineMessageSender*>(0xdeadbeef);
    MPEEngineInfo* ei = reinterpret_cast<MPEEngineInfo*>(0xdeadbeef);
    iFactory->CommandListener( mce, ems, ei );
    singleton->iMessageObject = 0xbeefdead;
    MPhoneMediatorMessage* object = iFactory->MediatorMessage( 0, 0 );    
    EUNIT_ASSERT( object == reinterpret_cast<MPhoneMediatorMessage*>(0xbeefdead) );
    }

void ut_mediatorfactory::TestFetchingSenderL()
    {
    singleton->iSenderObject = 0xbeefdead;
    EUNIT_ASSERT( iFactory->Sender() == reinterpret_cast<CPhoneMediatorSender*>(0xbeefdead) );
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_mediatorfactory,
    "Tests for CPhoneMediatorFactory",
    "UNIT" )

EUNIT_TEST(
    "Initialise command listener",
    "CPhoneMediatorFactory",
    "CommandListener",
    "FUNCTIONALITY",
    SetupL, TestInitialisingCommandListenerL, Teardown)
    
EUNIT_TEST(
    "Not Initialise command listener",
    "CPhoneMediatorFactory",
    "CommandListener",
    "FUNCTIONALITY",
    SetupL, TestCommandListenerNotInitL, Teardown)
/*
EUNIT_TEST(
    "Parameter MPhoneMenuAndCbaEvents precond check",
    "CPhoneMediatorFactory",
    "CommandListener",
    "FUNCTIONALITY",
    SetupL, TestMPhoneMenuAndCbaEventsPreCondCheckL, Teardown)

EUNIT_TEST(
    "Parameter MPhoneEngineMessageSender precond check",
    "CPhoneMediatorFactory",
    "CommandListener",
    "FUNCTIONALITY",
    SetupL, TestMPhoneEngineMessageSenderPreCondCheckL, Teardown)

EUNIT_TEST(
    "Parameter MPhoneEngineMessageSender precond check",
    "CPhoneMediatorFactory",
    "MediatorMessage",
    "FUNCTIONALITY",
    SetupL, TestEngineInfoPreCondCheckL, Teardown)

EUNIT_TEST(
    "Parameter MPhoneEngineMessageSender precond check",
    "CPhoneMediatorFactory",
    "MediatorMessage",
    "FUNCTIONALITY",
    SetupL, TestMediatorMessageFactoryCheckL, Teardown)

EUNIT_TEST(
    "Parameter MPhoneEngineMessageSender precond check",
    "CPhoneMediatorFactory",
    "MediatorMessage",
    "FUNCTIONALITY",
    SetupL, CreateMessageFailCheckL, Teardown)
*/
EUNIT_TEST(
    "Parameter MPhoneEngineMessageSender precond check",
    "CPhoneMediatorFactory",
    "MediatorMessage",
    "FUNCTIONALITY",
    SetupL, SuccesfulMediatorMessageL, Teardown)

EUNIT_TEST(
    "Parameter MPhoneEngineMessageSender precond check",
    "CPhoneMediatorFactory",
    "Sender",
    "FUNCTIONALITY",
    SetupL, TestFetchingSenderL, Teardown)
    
EUNIT_END_TEST_TABLE
