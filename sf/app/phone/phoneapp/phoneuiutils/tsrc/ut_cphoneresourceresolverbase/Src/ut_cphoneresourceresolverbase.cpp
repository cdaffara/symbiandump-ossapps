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

#include <csmcmockcontext.h>
#include <smcdefaultvalue.h>
#include <msmcmockspecbuilder.h>
#include <smcobjecttotypemapper.h>
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <featmgr.h>
#include "CPhoneMainResourceResolver.h"
//#include "MPhoneKeyEvents.h"
#include "phonerssbase.h"

#include "ut_cphoneresourceresolverbase.h"
#include "cphoneresourceresolverbase.h"
#include "phoneresourceids.h"

#include <PhoneUI.rsg>
#include <HWRMDomainPSKeys.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>     

 
class CPhoneMainResourceResolverBaseTester :  public CPhoneResourceResolverBase
    {
    public:
    static CPhoneMainResourceResolverBaseTester* CPhoneMainResourceResolverBaseTester::NewL()
        {
        CPhoneMainResourceResolverBaseTester* self = 
            new( ELeave ) CPhoneMainResourceResolverBaseTester();
        return self;
        }    
    };
 
// - Construction -----------------------------------------------------------

ut_cphoneresourceresolverbase* ut_cphoneresourceresolverbase::NewL()
    {
    ut_cphoneresourceresolverbase* self = ut_cphoneresourceresolverbase::NewLC();
    CleanupStack::Pop();
    return self;
    }
	
ut_cphoneresourceresolverbase* ut_cphoneresourceresolverbase::NewLC()
    {
    ut_cphoneresourceresolverbase* self = new( ELeave ) ut_cphoneresourceresolverbase();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphoneresourceresolverbase::~ut_cphoneresourceresolverbase()
    {
    CSmcMockContext::Release();
    }

ut_cphoneresourceresolverbase::ut_cphoneresourceresolverbase()
    {
    }

void ut_cphoneresourceresolverbase::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphoneresourceresolverbase::SetupL(  )
    {
    iMockContext->InitializeL();
  
    }


void ut_cphoneresourceresolverbase::Teardown(  )
    {
    if ( iResourceResolverBase )
        {
        delete iResourceResolverBase;
        iResourceResolverBase = NULL;
        }
      
    iMockContext->Reset();
    }
   

// -----------------------------------------------------------------------------
// Test menu 
// -example
// -----------------------------------------------------------------------------
//  
void ut_cphoneresourceresolverbase::T_NumberAcqMenubar()
    {
    iMockContext->ExpectCallL( "FeatureManager::FeatureSupported" ).
            WithL( KFeatureIdOnScreenDialer ).
            ReturnsL(ETrue);  
    
    iResourceResolverBase  = CPhoneMainResourceResolverBaseTester::NewL();  // variation done in construction
         
    EUNIT_ASSERT_EQUALS ( R_PHONEUIDIALER_NUMBERACQ_MENUBAR, 
                         iResourceResolverBase->ResolveResourceID( EPhoneNumberAcqMenubar ) );         
    }

/**
 *  Test name: T_IncomingCallSilenceButtonsL
 *  Use case: MT call, touch screen supported, cenrep 2001B2E6.txt
 *  modified "1" to "2" -> call buttons are "Answer" and "Silence"
 *  Motivation: 417-33878
 *  stif ref: None.
 * 
 * */
/*
void ut_cphoneresourceresolverbase::T_IncomingCallSilenceButtonsL()
    {
    iResourceResolverBase  = CPhoneMainResourceResolverBaseTester::NewL();
    
    iResourceResolverBase->BaseConstructL();
    
    EUNIT_ASSERT_EQUALS ( R_PHONEUI_INCOMING_CALL_SILENCE_BUTTONS, 
                         iResourceResolverBase->ResolveResourceID( EPhoneIncomingCallButtons ) );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations());    
    }*/

/**
 *  Test name: T_IncomingCallButtonsL
 *  Use case: MT call, touch screen supported -> call buttons
 *  are "Answer" and "Reject"
 *  Motivation: 417-33878
 *  stif ref: None.
 * 
 * */
void ut_cphoneresourceresolverbase::T_IncomingCallButtonsL()
    {
    iResourceResolverBase  = CPhoneMainResourceResolverBaseTester::NewL(); 
    iResourceResolverBase->BaseConstructL();
    
    EUNIT_ASSERT_EQUALS ( R_PHONEUI_INCOMING_CALL_BUTTONS, 
                         iResourceResolverBase->ResolveResourceID( EPhoneIncomingCallButtons ) );
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations());    
    }

 
// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphoneresourceresolverbase,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "1- Test Number Acq Menu ",
    "",
    "Test1",
    "FUNCTIONALITY",
    SetupL, T_NumberAcqMenubar, Teardown)

EUNIT_TEST(
    "2- Test Incoming call buttons ",
    "",
    "Test2",
    "FUNCTIONALITY",
    SetupL, T_IncomingCallButtonsL, Teardown)

/*    
// Change \epoc32\release\winscw\udeb\z\private\10202be9\2001B2E6.txt 1 -> 2
EUNIT_TEST(
    "2- Test Incoming silence call buttons ",
    "",
    "Test2",
    "FUNCTIONALITY",
    SetupL, T_IncomingCallSilenceButtonsL, Teardown)  
*/

EUNIT_END_TEST_TABLE
