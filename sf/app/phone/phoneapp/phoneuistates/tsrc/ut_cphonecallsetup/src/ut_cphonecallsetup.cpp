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

#include "ut_cphonecallsetup.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

#include "cphonecallsetup.h"

#include "CPhoneState.h"

#include "cphonestatecallsetupstub.h"
#include "CPhoneStateMachineStub.h"
#include "CPhoneViewControllerStub.h"
#include "CPhoneMainResourceResolver.h"

CallSetupWrapperClass* CallSetupWrapperClass::NewL( 
        MPhoneStateMachine* aStateMachine, 
        MPhoneViewCommandHandle* aViewCommandHandle,
        MPhoneCustomization* aPhoneCustomization )
    {
    CallSetupWrapperClass* self = new( ELeave ) CallSetupWrapperClass(
            aStateMachine,
            aViewCommandHandle,
            aPhoneCustomization );
        
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CallSetupWrapperClass::~CallSetupWrapperClass()
    {
    }

CallSetupWrapperClass::CallSetupWrapperClass(
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneCallSetup( aStateMachine, aViewCommandHandle, aPhoneCustomization )    
    {
    }

void CallSetupWrapperClass::ConstructL()
    {
    CPhoneCallSetup::ConstructL();
    }


void CallSetupWrapperClass::HandleConnectingL( TInt aCallId )
    {
    CPhoneCallSetup::HandleConnectingL(aCallId);
    }

// - Construction -----------------------------------------------------------

ut_cphonecallsetup* ut_cphonecallsetup::NewL()
    {
    ut_cphonecallsetup* self = ut_cphonecallsetup::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphonecallsetup* ut_cphonecallsetup::NewLC()
    {
    ut_cphonecallsetup* self = new( ELeave ) ut_cphonecallsetup();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonecallsetup::~ut_cphonecallsetup()
    {
    }

ut_cphonecallsetup::ut_cphonecallsetup()
    {
    }

void ut_cphonecallsetup::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------


void ut_cphonecallsetup::SetupL(  )
    {
    iViewCommandHandle = CPhoneViewController::NewL();
    iStateMachine = CPhoneStateMachineStub::NewL(); 
    iWrapper = CallSetupWrapperClass::NewL( iStateMachine, iViewCommandHandle, NULL );
    }
    

void ut_cphonecallsetup::Teardown(  )
    {
    delete iWrapper; 
    iWrapper = NULL;
    delete iStateMachine;
    iStateMachine = NULL;
    delete iViewCommandHandle;
    iViewCommandHandle = NULL;
    delete iStubResourceResolver;
    iStubResourceResolver = NULL; 
    }
    

void ut_cphonecallsetup::T_CPhoneCallSetup__CPhoneCallSetupL(  )
    {
    }
    
void ut_cphonecallsetup::T_CPhoneCallSetup_HandleHandleConnectingMessageL(  )
    {
    TInt callId(1);
    iWrapper->HandleConnectingL( callId );
    VerifyEPhoneViewRemoveGlobalNoteIsNotUsed();
    }
    
void ut_cphonecallsetup::VerifyEPhoneViewRemoveGlobalNoteIsNotUsed( )
    {
    EUNIT_PRINT( _L( "Checking that command EPhoneViewRemoveGlobalNote is not used!" ) );
    TInt command = EPhoneViewRemoveGlobalNote;
    TInt count = iViewCommandHandle->iCmds->Count();
    TBool cmdFound = EFalse;
    TInt i = 0;
    for ( ; i < count; i++ )
        {
        TCmds cmd = iViewCommandHandle->iCmds->At(i);
        if( cmd.iCmd == EPhoneViewRemoveGlobalNote )  
            {
            EUNIT_FAIL_TEST( "EPhoneViewRemoveGlobalNote Command found -> FAIL! " );
            cmdFound = ETrue;
            break;
            }
        }
    }

TInt ut_cphonecallsetup::VerifyViewCommand( const TInt& aCmd )
    {
    _LIT(KDesC, "Checking command id = %d ");
    EUNIT_PRINT( KDesC(), aCmd );
    
    TInt count = iViewCommandHandle->iCmds->Count();
    TBool cmdFound = EFalse;
    TInt i = 0;
    for ( ; i < count; i++ )
        {
        TCmds cmd = iViewCommandHandle->iCmds->At(i);
        if( cmd.iCmd == aCmd )  
            {
            cmdFound = ETrue;
            break;
            }
        }
    EUNIT_ASSERT_DESC( cmdFound, "View Command not ok" );
    return i;
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphonecallsetup,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "CPhoneCallSetup const./desct.",
    "CPhoneCallSetup",
    "Setup-teardown",
    "FUNCTIONALITY",
    SetupL, T_CPhoneCallSetup__CPhoneCallSetupL, Teardown)
    
EUNIT_TEST(
    "CPhoneCallSetup - HandleConnectingL",
    "CPhoneCallSetup",
    "HandlePhoneEngineMessageL - HandleConnectingL",
    "FUNCTIONALITY",
    SetupL, T_CPhoneCallSetup_HandleHandleConnectingMessageL, Teardown)
    
EUNIT_END_TEST_TABLE
