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

#include "T_MediatorMessageFactory.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include "pevirtualengine.h"

#include "CPhoneMediatorMessageFactory.h"
#include "MPhoneMediatorMessage.h"

#include "CPhoneDataPortMessage.h"

CPhoneDataPortMessage* CDataPortMessage_NewLReturnValue;
MPEEngineInfo* CDataPortMessage_NewLEngineInfo;

// - Construction -----------------------------------------------------------

T_MediatorMessageFactory* T_MediatorMessageFactory::NewL()
    {
    T_MediatorMessageFactory* self = T_MediatorMessageFactory::NewLC();
    CleanupStack::Pop();
    return self;
    }

T_MediatorMessageFactory* T_MediatorMessageFactory::NewLC()
    {
    T_MediatorMessageFactory* self = new( ELeave ) T_MediatorMessageFactory();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

T_MediatorMessageFactory::~T_MediatorMessageFactory()
    {
    }

T_MediatorMessageFactory::T_MediatorMessageFactory()
    {
    }

void T_MediatorMessageFactory::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------



void T_MediatorMessageFactory::SetupL(  )
    {
    MPEEngineInfo* info = reinterpret_cast<MPEEngineInfo*>(0xdeadbeef); // No access to engine info needed in tests.
    iFactory = CPhoneMediatorMessageFactory::NewL( *info );
    }
    

void T_MediatorMessageFactory::Teardown(  )
    {
    delete iFactory;
    }
    

void T_MediatorMessageFactory::TestCreatingMessageL(  )
    {
    CDataPortMessage_NewLReturnValue = reinterpret_cast<CPhoneDataPortMessage*>(0x12341234);  
    
    MPhoneMediatorMessage* msg = iFactory->CreateMessageL( MEngineMonitor::EPEMessageDataPortLoaned, 0 );
    
    // Compare pointers
    EUNIT_ASSERT( CDataPortMessage_NewLReturnValue == static_cast<CPhoneDataPortMessage*>(msg) );
    EUNIT_ASSERT( CDataPortMessage_NewLEngineInfo == reinterpret_cast<MPEEngineInfo*>(0xdeadbeef) );
    
    // Do not delete the msg, because msg is not really created
    }
    
void T_MediatorMessageFactory::TestNoMessageForEngineMessageL()
    {
    MPhoneMediatorMessage* msg = 
        iFactory->CreateMessageL( static_cast<MEngineMonitor::TPEMessagesFromPhoneEngine>( -1 ), 0 );
    EUNIT_ASSERT( msg == NULL );
    }
// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    T_MediatorMessageFactory,
    "Unittests for CPhoneMediatorMessageFactory",
    "UNIT" )

EUNIT_TEST(
    "Create message",
    "CreateMessageL",
    "CPhoneMediatorMessageFactory",
    "FUNCTIONALITY",
    SetupL, TestCreatingMessageL, Teardown)

EUNIT_TEST(
    "No message to create for pe message",
    "CreateMessageL",
    "CPhoneMediatorMessageFactory",
    "FUNCTIONALITY",
    SetupL, TestNoMessageForEngineMessageL, Teardown)
    

EUNIT_END_TEST_TABLE
