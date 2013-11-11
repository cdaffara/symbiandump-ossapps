
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

#include <phoneui.rsg>

#include "mock_cphoneviewcommandhandle.h"
#include "cphonestatemachine.h"
#include "ut_cphoneuicommandmanager.h"
#include "CPhoneCenRepProxy.h"
#include "cpeengineinfo.h"
#include "phonerssbase.h"
#include "CPhoneMainResourceResolver.h"

#include <csmcmockcontext.h>
#include <msmcmockspecbuilder.h>

#include "telephonyvariant.hrh"
#include "TPhoneCmdParamInteger.h"
#include "TPhoneCmdParamBoolean.h"
#include <startupdomainpskeys.h>
#include <hwrmdomainpskeys.h>
#include "tphonecmdparamcallstatedata.h"
#include "phoneresourceids.h"

CPhoneCenRepProxy* GlobalCPhoneCenRepProxy;
TPEPhoneAudioRouteParameters routeParameters;

// needed for mocking CPhoneMainResourceResolver::Instance()
class CPhoneMainResourceResolverMock : public CPhoneMainResourceResolver
    {
    
    };

// Class to access protected constructor
class CPhoneStateMachineMock : public CPhoneStateMachine
    {
public:
    CPhoneStateMachineMock( MPhoneViewCommandHandle* aViewCommandHandle )
        : CPhoneStateMachine( aViewCommandHandle )
        {
            
        }
    };

//---------------------------------------------------------------------------

//CONSTANS
_LIT( KTestString, "test_string" );

// - Construction -----------------------------------------------------------

ut_cphoneuicommandmanager* ut_cphoneuicommandmanager::NewL()
    {
    ut_cphoneuicommandmanager* self = ut_cphoneuicommandmanager::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphoneuicommandmanager* ut_cphoneuicommandmanager::NewLC()
    {
    ut_cphoneuicommandmanager* self = new( ELeave ) ut_cphoneuicommandmanager();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphoneuicommandmanager::~ut_cphoneuicommandmanager()
    {
    CSmcMockContext::Release();
    }

ut_cphoneuicommandmanager::ut_cphoneuicommandmanager()
    {
    }

void ut_cphoneuicommandmanager::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------



void ut_cphoneuicommandmanager::SetupL(  )
    {    
    iViewCmdHandleMock = new CPhoneViewCommandHandleMock();
    iStateMachineMock = new (ELeave) CPhoneStateMachineMock( iViewCmdHandleMock );
    iCustomization = NULL;
    iPhoneStateMock = new (ELeave) CPhoneState(iStateMachineMock, iViewCmdHandleMock, iCustomization );
    iCmdManager = CPhoneUiCommandManager::NewL(
                iPhoneStateMock,
                *iViewCmdHandleMock,
                *iStateMachineMock );
    
    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
    SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );
        
    GlobalCPhoneCenRepProxy = NULL;      
    
    iEngineInfoMock = CPEEngineInfo::NewL();    
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );
    
    SmcDefaultValue<const TPESimState&>::SetL( EPESimUsable );
    SmcDefaultValue<const TPEAudioOutput&>::SetL( EPENotActive );
    SmcDefaultValue<const TPEPhoneAudioRouteParameters&>::SetL( routeParameters );
    SmcDefaultValue<const TBool&>::SetL( EFalse );
    }

void ut_cphoneuicommandmanager::Teardown(  )
    {
    delete iCmdManager;
    iCmdManager = NULL;
    delete iPhoneStateMock;
    iPhoneStateMock = NULL;
    delete iViewCmdHandleMock;
    iViewCmdHandleMock = NULL;
    delete iStateMachineMock;
    iStateMachineMock = NULL;
    delete iCustomization;
    iCustomization = NULL;
    delete iCPhoneMainResourceResolverMock;
    iCPhoneMainResourceResolverMock = NULL;
    SmcDefaultValue<CPhoneMainResourceResolver*>::Reset();
    iMockContext->Reset();
    delete iEngineInfoMock;
    iEngineInfoMock = NULL;
    
    SmcDefaultValue<MPEEngineInfo*>::Reset();
    SmcDefaultValue<const TPESimState&>::Reset();
    SmcDefaultValue<const TPEAudioOutput&>::Reset();
    SmcDefaultValue<const TPEPhoneAudioRouteParameters&>::Reset();
    SmcDefaultValue<const TBool&>::Reset();
    }
    
void ut_cphoneuicommandmanager::T_CPhoneUiCommandManager__EmptyL(  )
    {
    //EMPTY CASE
    }

/**
 *  Test name: T_UpdateUiCommands
 *      
 *  Use case: Client calls UpdateUiCommands check that SetButtonFlags/SetTouchButtons/SetToolBarButtons gets called. 
 *  Motivation: 
 */
void ut_cphoneuicommandmanager::T_UpdateUiCommands(  )
    {
    /*expect call from SetButtonFlags*/
    TPhoneCmdParamBoolean muteParam;
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommand" ).
            WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewSetMuteFlag, muteParam ); 
    
    /*expect call from SetTouchButtons*/
    TPhoneCmdParamBoolean booleanParam;
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommand" ).
            WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewSetTouchPaneButtons, booleanParam ); 
        
    /*expect call from SetToolBarButtons*/
    TPhoneCmdParamInteger integerParam;
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommand" ).
           WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewUpdateCba, integerParam ); 
    
    iCmdManager->UpdateUiCommands();
    
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

/**
 *  Test name: T_UpdateUiCommands
 *      
 *  Use case: Client calls UpdateUiCommands check that SetButtonFlags/SetTouchButtons/SetToolBarButtons gets called. 
 *  Motivation: 
 */
void ut_cphoneuicommandmanager::T_Set_Get_SoftRejectStatus(  )
    {
    iCmdManager->SetSoftRejectStatus(ETrue);
    EUNIT_ASSERT_EQUALS( ETrue, iCmdManager->SoftRejectStatus() );
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphoneuicommandmanager,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "1- ut_cphoneuicommandmanager",
    "ut_cphoneuicommandmanager",
    "1- Setup-teardown",
    "FUNCTIONALITY",
    SetupL, T_CPhoneUiCommandManager__EmptyL, Teardown)
    
EUNIT_TEST(
    "2- ut_cphoneuicommandmanager",
    "ut_cphoneuicommandmanager",
    "2- T_UpdateUiCommands",
    "FUNCTIONALITY",
    SetupL, T_UpdateUiCommands, Teardown)
    
EUNIT_TEST(
    "3- ut_cphoneuicommandmanager",
    "ut_cphoneuicommandmanager",
    "3- T_Set_Get_SoftRejectStatus",
    "FUNCTIONALITY",
    SetupL, T_Set_Get_SoftRejectStatus, Teardown)

EUNIT_END_TEST_TABLE
