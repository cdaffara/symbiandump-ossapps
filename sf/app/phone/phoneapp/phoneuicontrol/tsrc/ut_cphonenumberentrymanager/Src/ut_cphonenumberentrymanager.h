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

#ifndef __UT_CPHONENUMBERENTRYMANAGER_H__
#define __UT_CPHONENUMBERENTRYMANAGER_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include "cphonenumberentrymanager.h"
#include "mphonecustomization.h"
#include "cphonemainresourceresolver.h"
#include "TPhoneCmdParamCallHeaderData.h"
#include "TPhoneCommandParam.h"
#include "CPhoneState.h"
#include "CPhoneUiCommandManager.h"

// FORWARD DECLARATIONS
class CPhoneViewCommandHandleMock;
class CPhoneStateMachineGSM;
class CBubbleManager;
class CCoeEnv;
class CPhoneMainResourceResolverMock;
class CSmcMockContext;
class CPhoneState;
class CPEEngineInfo;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphonenumberentrymanager )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonenumberentrymanager* NewL();
        static ut_cphonenumberentrymanager* NewLC();
        ~ut_cphonenumberentrymanager();

    private: // Constructors

        ut_cphonenumberentrymanager();
        void ConstructL();

    private: // New methods

         void SetupL();
         void Teardown();
         void T_EmptyL();
         
         void T_StoreNumberEntryContentAndCheckStoreL();
         void T_ClearNumberEntryContentCacheL();
         void T_SetNumberEntryVisibilityL();
         void T_SetVisibilityIfNumberEntryUsedL();
         void T_RemoveNumberEntryIfVisibilityIsFalseL();
         void T_ClearNumberEntryContentCacheIfContentStoredL();
         void T_CallFromNumberEntryL();
         void T_PhoneNumberFromEntryLC();
         void T_NumberEntryClearL();
         
    private:
         
         void GetNumberEntryCount(
             TPhoneViewCommandId aCmdId,
             TPhoneCommandParam* aCommandParam );
         
         void GetNumberFromEntry(
                 TPhoneViewCommandId aCmdId,
                 TPhoneCommandParam* aCommandParam );
         
         void SaveNumberToStoreL();
        
    private: // Data

        CPhoneNumberEntryManager* iCPhoneNumberEntryManager;
        CPhoneViewCommandHandleMock* iViewCommandHandle;
        CPhoneStateMachineGSM* iStateMachine;
        CPhoneState* iPhoneStateMock;
        CPEEngineInfo* iEngineInfoMock;
        CPhoneUiCommandManager* iCmdManagerMock;
        CCoeEnv* iCoeEnvMock;
        MPhoneCustomization* iCustomization;
        CSmcMockContext* iMockContext;
        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONENUMBERENTRYMANAGER_H__
