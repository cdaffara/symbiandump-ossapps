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

#ifndef __UT_CPHONECONFERENCEANDWAITINGANDCALLSETUP_H__
#define __UT_CPHONECONFERENCEANDWAITINGANDCALLSETUP_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS
class CPhoneConferenceAndWaitingAndCallSetup;
class CPhoneMainResourceResolverMock;
class CPhoneStateMachineGSM;
class CPhoneViewCommandHandleMock;
class CSmcMockContext;
class CCoeEnv;
class CPEEngineInfo;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphconfandwaitingandcallsetup )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphconfandwaitingandcallsetup* NewL();
        static ut_cphconfandwaitingandcallsetup* NewLC();
        ~ut_cphconfandwaitingandcallsetup();

    private: // Constructors

        ut_cphconfandwaitingandcallsetup();
        void ConstructL();

    private: // New methods

         void SetupL();
         void Teardown();
        
         void T_HandleConnectedL();
         void T_HandleIdleL();
         void T_HandleConferenceIdleL();
        

    private: // Data
   
        CPhoneViewCommandHandleMock* iViewCommandHandleMock;
        CPhoneStateMachineGSM* iStateMachineMock; 
        CPhoneConferenceAndWaitingAndCallSetup * iStateConferenceAndCallSetupAndWaiting;
        CCoeEnv* iCoeEnvMock;
        CSmcMockContext* iMockContext;
        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;
        CPEEngineInfo* iEngineInfoMock;
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONECONFERENCEANDWAITINGANDCALLSETUP_H__
