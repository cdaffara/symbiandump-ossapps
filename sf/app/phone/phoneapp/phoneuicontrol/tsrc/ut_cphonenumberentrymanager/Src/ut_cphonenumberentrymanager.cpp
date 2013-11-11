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

#include "ut_cphonenumberentrymanager.h"
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <coemain.h>

#include "CPhoneStateMachineGSM.h"
#include "mock_cphoneviewcommandhandle.h"
#include "CPhoneCenRepProxy.h"
#include "TPhoneCmdParamInteger.h"
#include "TPhoneCmdParamString.h"
#include "phonerssbase.h"
#include "phoneappcommands.hrh"
#include "cpeengineinfo.h"
#include "cpeengineinfoimpl.h"

CPhoneCenRepProxy* GlobalCPhoneCenRepProxy;
HBufC* GlobalHBufC;

#include <csmcmockcontext.h>
#include <msmcmockspecbuilder.h>

// needed for mocking CPhoneMainResourceResolver::Instance()
class CPhoneMainResourceResolverMock : public CPhoneMainResourceResolver
    {
    
    };

//CONSTANS
_LIT( KTestString, "050123456" );

// this is set true if there is no need check param id value.
TBool doNotCheckParamId;

// -----------------------------------------------------------------------------
// operator for comparing parameter expected and result value.
// -
// -----------------------------------------------------------------------------
//
TBool operator==(const TPhoneCommandParam& param1, const TPhoneCommandParam& param2 )
    {
    TBool ret( ETrue );
    
    if ( param1.ParamId() == param2.ParamId() )
        {
    
        if ( TPhoneCommandParam::EPhoneParamIdInteger == param1.ParamId() )
            {
            const TPhoneCmdParamInteger  &value1 = static_cast<const TPhoneCmdParamInteger&> (param1);
            const TPhoneCmdParamInteger  &value2 = static_cast<const TPhoneCmdParamInteger&> (param2);
            if ( value1.Integer() != value2.Integer() )
                {
                ret = EFalse;
                }
            }
        else if ( TPhoneCommandParam::EPhoneParamIdBoolean == param1.ParamId() && !doNotCheckParamId )
            {
            const TPhoneCmdParamBoolean  &value1 = static_cast<const TPhoneCmdParamBoolean&> (param1);
            const TPhoneCmdParamBoolean  &value2 = static_cast<const TPhoneCmdParamBoolean&> (param2);
            if ( value1.Boolean() != value2.Boolean() )
                {
                ret = EFalse;
                }
            } 
        else
            {
            ret = ETrue;  // just paramiId compared
            }
        }
    return ret;
    }

// - Construction -----------------------------------------------------------

ut_cphonenumberentrymanager* ut_cphonenumberentrymanager::NewL()
    {
    ut_cphonenumberentrymanager* self = ut_cphonenumberentrymanager::NewLC();
    CleanupStack::Pop();
    return self;
    }

ut_cphonenumberentrymanager* ut_cphonenumberentrymanager::NewLC()
    {
    ut_cphonenumberentrymanager* self = new( ELeave ) ut_cphonenumberentrymanager();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

ut_cphonenumberentrymanager::~ut_cphonenumberentrymanager()
    {
    CSmcMockContext::Release();
    }

ut_cphonenumberentrymanager::ut_cphonenumberentrymanager()
    {
    }

void ut_cphonenumberentrymanager::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    iMockContext = CSmcMockContext::InstanceL();
    }

// - Test methods -----------------------------------------------------------


void ut_cphonenumberentrymanager::SetupL(  )
    {
    iCPhoneMainResourceResolverMock = new ( ELeave ) CPhoneMainResourceResolverMock();
    SmcDefaultValue<CPhoneMainResourceResolver*>::SetL( iCPhoneMainResourceResolverMock );
    
    iCoeEnvMock = new (ELeave ) CCoeEnv();
    SmcDefaultValue<CCoeEnv*>::SetL( iCoeEnvMock );
    
    iViewCommandHandle = new CPhoneViewCommandHandleMock();
    iStateMachine = CPhoneStateMachineGSM::NewL( iViewCommandHandle );
    
    iCustomization = NULL;
    iCPhoneNumberEntryManager = NULL;
    
    iPhoneStateMock = new (ELeave) CPhoneState(iStateMachine, iViewCommandHandle, iCustomization );
    
    iEngineInfoMock = CPEEngineInfo::NewL();
    SmcDefaultValue<MPEEngineInfo*>::SetL( iEngineInfoMock );
    
    iCmdManagerMock = CPhoneUiCommandManager::NewL(
            iPhoneStateMock,
            *iViewCommandHandle,
            *iStateMachine );
    
    iCPhoneNumberEntryManager = CPhoneNumberEntryManager::NewL(
                    iPhoneStateMock,
                    *iViewCommandHandle,
                    *iStateMachine,
                    iCustomization,
                    *iCmdManagerMock );
    
    GlobalCPhoneCenRepProxy = NULL;
    
    doNotCheckParamId = EFalse;
    }
    

void ut_cphonenumberentrymanager::Teardown(  )
    {
    delete iCPhoneNumberEntryManager;
    iCPhoneNumberEntryManager = NULL;
    
    delete iViewCommandHandle;
    iViewCommandHandle = NULL;
    
    delete iStateMachine;
    iStateMachine = NULL;
    
    delete iPhoneStateMock;
    iPhoneStateMock = NULL;
    
    delete iCmdManagerMock;
    iCmdManagerMock = NULL;
    
    delete iCustomization;
    iCustomization = NULL;
    
    delete iCoeEnvMock;
    iCoeEnvMock = NULL;
    SmcDefaultValue<CCoeEnv*>::Reset();
    
    delete iEngineInfoMock;
    iEngineInfoMock = NULL;
    SmcDefaultValue<MPEEngineInfo*>::Reset();
    
    delete GlobalHBufC;
    GlobalHBufC = NULL;
    
    delete GlobalCPhoneCenRepProxy;
    GlobalCPhoneCenRepProxy = NULL;
    
    delete iCPhoneMainResourceResolverMock;
    iCPhoneMainResourceResolverMock = NULL;
    SmcDefaultValue<CPhoneMainResourceResolver*>::Reset();
    
    iMockContext->Reset();
    }
    
void ut_cphonenumberentrymanager::T_EmptyL(  )
    {
    //EMPTY CASE
    }


void ut_cphonenumberentrymanager::GetNumberEntryCount(
        TPhoneViewCommandId /*aCmdId*/,
        TPhoneCommandParam* aCommandParam )
      {
      if ( aCommandParam->ParamId() ==
              TPhoneCommandParam::EPhoneParamIdInteger )
          {
          TPhoneCmdParamInteger* intParam =
              static_cast<TPhoneCmdParamInteger*>( aCommandParam );
          intParam->SetInteger(9);
          }
      }

void ut_cphonenumberentrymanager::GetNumberFromEntry(
        TPhoneViewCommandId /*aCmdId*/,
        TPhoneCommandParam* aCommandParam )
      {
      if ( aCommandParam->ParamId() ==
              TPhoneCommandParam::EPhoneParamIdString )
          {
          TPhoneCmdParamString* stringParam =
              static_cast<TPhoneCmdParamString*>( aCommandParam );
          GlobalHBufC->Des().Copy(KTestString);
          stringParam->SetString(&GlobalHBufC->Des());
          }
      }

void ut_cphonenumberentrymanager::SaveNumberToStoreL()
    {
    GlobalHBufC = HBufC::NewL( KPhoneNumberEntryBufferSize );
    GlobalHBufC->Des().Copy(KTestString);
    
    TPhoneCmdParamInteger intParam;
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL(int, TPhoneCommandParam *)" ).
        WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewGetNumberEntryCount, intParam ).
        WillOnceL( InvokeL( this, &GetNumberEntryCount));
    
    TPhoneCmdParamString stringParam;
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL(int, TPhoneCommandParam *)" ).
            WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewGetNumberFromEntry, stringParam ).
            WillOnceL( InvokeL( this, &GetNumberFromEntry));
    iCPhoneNumberEntryManager->StoreNumberEntryContentL();
    }

void ut_cphonenumberentrymanager::T_StoreNumberEntryContentAndCheckStoreL()
    {
    /*Check that there is no number stored.*/
    EUNIT_ASSERT_EQUALS( EFalse, iCPhoneNumberEntryManager->IsNumberEntryContentStored() );
    SaveNumberToStoreL();
    /*Check that number was stored.*/
    EUNIT_ASSERT_EQUALS( ETrue, iCPhoneNumberEntryManager->IsNumberEntryContentStored() );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonenumberentrymanager::T_ClearNumberEntryContentCacheL()
    {
    /*Save number to store.*/
    SaveNumberToStoreL();
    /*Check that number was stored.*/
    EUNIT_ASSERT_EQUALS( ETrue, iCPhoneNumberEntryManager->IsNumberEntryContentStored() );
    iCPhoneNumberEntryManager->ClearNumberEntryContentCache();
    /*Check that number was cleared from store.*/
    EUNIT_ASSERT_EQUALS( EFalse, iCPhoneNumberEntryManager->IsNumberEntryContentStored() );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonenumberentrymanager::T_SetNumberEntryVisibilityL()
    {
    TPhoneCmdParamBoolean booleanParam;
    booleanParam.SetBoolean( ETrue );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewSetNumberEntryVisible, booleanParam );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::HandleCommandL" ).
            WithL<TPhoneViewCommandId> ( EPhoneViewGetNumberEntryIsVisibleStatus ).
            ReturnsL(EPhoneViewResponseSuccess);
    iCPhoneNumberEntryManager->SetNumberEntryVisibilityL(ETrue);
    EUNIT_ASSERT_EQUALS( ETrue, iCPhoneNumberEntryManager->IsNumberEntryVisibleL());
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    
    booleanParam.SetBoolean( EFalse );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewSetNumberEntryVisible, booleanParam );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::HandleCommandL" ).
            WithL<TPhoneViewCommandId> ( EPhoneViewGetNumberEntryIsVisibleStatus ).
            ReturnsL(EPhoneViewResponseFailed);
    iCPhoneNumberEntryManager->SetNumberEntryVisibilityL(EFalse);
    EUNIT_ASSERT_EQUALS( EFalse, iCPhoneNumberEntryManager->IsNumberEntryVisibleL());
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonenumberentrymanager::T_SetVisibilityIfNumberEntryUsedL()
    {
    /*1- NumberEntry is used and client sets number entry visibility to true*/
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::HandleCommandL" ).
           WithL<TPhoneViewCommandId> ( EPhoneViewGetNumberEntryIsUsedStatus ).
           ReturnsL(EPhoneViewResponseSuccess);
    TPhoneCmdParamBoolean booleanParam;
    booleanParam.SetBoolean( ETrue );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
           WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewSetNumberEntryVisible, booleanParam );
    /*SetVisibilityIfNumberEntryUsedL should return true*/
    EUNIT_ASSERT_EQUALS( ETrue, iCPhoneNumberEntryManager->SetVisibilityIfNumberEntryUsedL(ETrue));
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    
    /*2- NumberEntry is used and client sets number entry visibility to false*/
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::HandleCommandL" ).
           WithL<TPhoneViewCommandId> ( EPhoneViewGetNumberEntryIsUsedStatus ).
           ReturnsL(EPhoneViewResponseSuccess);
    booleanParam.SetBoolean( EFalse );
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
           WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewSetNumberEntryVisible, booleanParam );
    /*SetVisibilityIfNumberEntryUsedL should return true*/
    EUNIT_ASSERT_EQUALS( ETrue, iCPhoneNumberEntryManager->SetVisibilityIfNumberEntryUsedL(EFalse));
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    
    /*3- NumberEntry is not used and client sets number entry visibility to true*/
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::HandleCommandL" ).
           WithL<TPhoneViewCommandId> ( EPhoneViewGetNumberEntryIsUsedStatus ).
           ReturnsL(EPhoneViewResponseFailed);
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
           WithL<TPhoneViewCommandId, TPhoneCommandParam&> ( EPhoneViewSetNumberEntryVisible, booleanParam ).
           TimesL(0); // EPhoneViewSetNumberEntryVisible should not be called at all.
    /*SetVisibilityIfNumberEntryUsedL should return false*/
    EUNIT_ASSERT_EQUALS( EFalse, iCPhoneNumberEntryManager->SetVisibilityIfNumberEntryUsedL(ETrue));
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonenumberentrymanager::T_RemoveNumberEntryIfVisibilityIsFalseL()
    {
    /*1- Number entry is not visible so EPhoneViewRemoveNumberEntry command should occure*/
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::HandleCommandL" ).
           WithL<TPhoneViewCommandId> ( EPhoneViewGetNumberEntryIsVisibleStatus ).
           ReturnsL(EPhoneViewResponseFailed);
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
           WithL<TPhoneViewCommandId> ( EPhoneViewRemoveNumberEntry ).
           TimesL(1);
    iCPhoneNumberEntryManager->RemoveNumberEntryIfVisibilityIsFalseL();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    
    /*2- Number entry is visible so EPhoneViewRemoveNumberEntry command shouldnt occure*/
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::HandleCommandL" ).
            WithL<TPhoneViewCommandId> ( EPhoneViewGetNumberEntryIsVisibleStatus ).
            ReturnsL(EPhoneViewResponseSuccess);
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
           WithL<TPhoneViewCommandId> ( EPhoneViewRemoveNumberEntry ).
           TimesL(0);
    iCPhoneNumberEntryManager->RemoveNumberEntryIfVisibilityIsFalseL();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonenumberentrymanager::T_ClearNumberEntryContentCacheIfContentStoredL()
    {
    /*Check that there is no number stored.*/
    EUNIT_ASSERT_EQUALS( EFalse, iCPhoneNumberEntryManager->IsNumberEntryContentStored() );
    SaveNumberToStoreL();
    /*Check that number was stored.*/
    EUNIT_ASSERT_EQUALS( ETrue, iCPhoneNumberEntryManager->IsNumberEntryContentStored() );
    
    iCPhoneNumberEntryManager->ClearNumberEntryContentCacheIfContentStored();
    /*Check that store is empty.*/
    EUNIT_ASSERT_EQUALS( EFalse, iCPhoneNumberEntryManager->IsNumberEntryContentStored() );
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonenumberentrymanager::T_CallFromNumberEntryL()
    {
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::HandleCommandL" ).
           WithL<TPhoneViewCommandId> ( EPhoneIsCustomizedDialerVisible ).
           ReturnsL(EPhoneViewResponseFailed);
    
    GlobalHBufC = HBufC::NewL( KPhoneNumberEntryBufferSize );
    TPhoneCmdParamString stringParam;
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommand" ).
            WithL<TPhoneViewCommandId, TPhoneCommandParam&>( EPhoneViewGetNumberFromEntry, stringParam ).
            WillOnceL( InvokeL( this, &GetNumberFromEntry));
    
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL(EPhoneViewRemoveNumberEntry);
    iMockContext->ExpectCallL( "CPhoneState::HandleNumberEntryClearedL" );
    iMockContext->ExpectCallL( "CPhoneState::DialVoiceCallL" );
    
    iCPhoneNumberEntryManager->CallFromNumberEntryL();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

void ut_cphonenumberentrymanager::T_PhoneNumberFromEntryLC()
    {
    GlobalHBufC = HBufC::NewL( KPhoneNumberEntryBufferSize );
    TPhoneCmdParamString stringParam;
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommand" ).
            WithL<TPhoneViewCommandId, TPhoneCommandParam&>( EPhoneViewGetNumberFromEntry, stringParam ).
            WillOnceL( InvokeL( this, &GetNumberFromEntry));
    HBufC* number = iCPhoneNumberEntryManager->PhoneNumberFromEntryLC();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    CleanupStack::PopAndDestroy( number );
    }

void ut_cphonenumberentrymanager::T_NumberEntryClearL()
    {
    iMockContext->ExpectCallL( "CPhoneViewCommandHandleMock::ExecuteCommandL" ).
            WithL(EPhoneViewClearNumberEntryContent);
    iCPhoneNumberEntryManager->NumberEntryClearL();
    EUNIT_ASSERT_EQUALS( KErrNone, iMockContext->VerifyExpectations() );
    }

// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    ut_cphonenumberentrymanager,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "test0 - CPhoneNumberEntryManager - const./desct",
    "CPhoneNumberEntryManager",
    "test0 - Setup-teardown",
    "FUNCTIONALITY",
    SetupL, T_EmptyL, Teardown)
 
EUNIT_TEST(
    "test1 - StoreNumberEntryContentAndCheckStoreL",
    "CPhoneNumberEntryManager",
    "test1 - StoreNumberEntryContentAndCheckStoreL",
    "FUNCTIONALITY",
    SetupL, T_StoreNumberEntryContentAndCheckStoreL, Teardown)
    
EUNIT_TEST(
    "test2 - ClearNumberEntryContentCacheL",
    "CPhoneNumberEntryManager",
    "test2 - ClearNumberEntryContentCacheL",
    "FUNCTIONALITY",
    SetupL, T_ClearNumberEntryContentCacheL, Teardown)
 
EUNIT_TEST(
    "test3 - SetNumberEntryVisibilityL",
    "CPhoneNumberEntryManager",
    "test3 - SetNumberEntryVisibilityL",
    "FUNCTIONALITY",
    SetupL, T_SetNumberEntryVisibilityL, Teardown)
    
EUNIT_TEST(
    "test4 - SetVisibilityIfNumberEntryUsedL",
    "CPhoneNumberEntryManager",
    "test4 - SetVisibilityIfNumberEntryUsedL",
    "FUNCTIONALITY",
    SetupL, T_SetVisibilityIfNumberEntryUsedL, Teardown)
    
EUNIT_TEST(
    "test5 - RemoveNumberEntryIfVisibilityIsFalseL",
    "CPhoneNumberEntryManager",
    "test5 - RemoveNumberEntryIfVisibilityIsFalseL",
    "FUNCTIONALITY",
    SetupL, T_RemoveNumberEntryIfVisibilityIsFalseL, Teardown)
    
EUNIT_TEST(
    "test6 - ClearNumberEntryContentCacheIfContentStoredL",
    "CPhoneNumberEntryManager",
    "test6 - ClearNumberEntryContentCacheIfContentStoredL",
    "FUNCTIONALITY",
    SetupL, T_ClearNumberEntryContentCacheIfContentStoredL, Teardown)
    
EUNIT_TEST(
    "test7 - CallFromNumberEntryL",
    "CPhoneNumberEntryManager",
    "test7 - CallFromNumberEntryL",
    "FUNCTIONALITY",
    SetupL, T_CallFromNumberEntryL, Teardown)
    
EUNIT_TEST(
    "test8 - PhoneNumberFromEntryLC",
    "CPhoneNumberEntryManager",
    "test8 - PhoneNumberFromEntryLC",
    "FUNCTIONALITY",
    SetupL, T_PhoneNumberFromEntryLC, Teardown)

EUNIT_TEST(
    "test9 - NumberEntryClearL",
    "CPhoneNumberEntryManager",
    "test9 - NumberEntryClearL",
    "FUNCTIONALITY",
    SetupL, T_NumberEntryClearL, Teardown)

EUNIT_END_TEST_TABLE
