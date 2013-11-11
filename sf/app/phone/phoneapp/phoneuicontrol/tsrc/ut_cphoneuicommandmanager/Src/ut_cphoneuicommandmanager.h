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

#ifndef __UT_CPHONEUIMANAGER_H__
#define __UT_CPHONEUIMANAGER_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include "cphoneuicommandmanager.h"
#include "mphonecustomization.h"
#include "cphonemainresourceresolver.h"
#include "TPhoneCommandParam.h"
#include "CPhoneState.h"

// FORWARD DECLARATIONS
class CPhoneViewCommandHandleMock;
class CPhoneStateMachine;
class CPhoneMainResourceResolverMock;
class CSmcMockContext;
class CPEEngineInfo;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphoneuicommandmanager )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphoneuicommandmanager* NewL();
        static ut_cphoneuicommandmanager* NewLC();
        ~ut_cphoneuicommandmanager();

    private: // Constructors

        ut_cphoneuicommandmanager();
        void ConstructL();

    private: // New methods

         void SetupL();
         void Teardown();
         
    private: // Test Cases.
        
         void T_CPhoneUiCommandManager__EmptyL();
         void T_UpdateUiCommands();
         void T_Set_Get_SoftRejectStatus();
        
    private: // Data

        CPhoneUiCommandManager* iCmdManager;
        CPhoneViewCommandHandleMock* iViewCmdHandleMock;
        CPhoneStateMachine* iStateMachineMock;
        MPhoneCustomization* iCustomization;
        TInt iCallId;
        CSmcMockContext* iMockContext;
        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;
        CPEEngineInfo* iEngineInfoMock;
        CPhoneState* iPhoneStateMock;

        EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_CPHONEUIMANAGER_H__
