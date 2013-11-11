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

#ifndef __UT_CPHONESTATEINCALL_H__
#define __UT_CPHONESTATEINCALL_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS
class CPhoneViewCommandHandleMock;
class CCoeEnv;
class CPhoneStateMachineGSM;
class CPhoneStateInCall;
class CPEEngineInfo;
class CPhoneMainResourceResolverMock;
class CSmcMockContext;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphonestateincall )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonestateincall* NewL();
        static ut_cphonestateincall* NewLC();
        ~ut_cphonestateincall();

    private: // Constructors

        ut_cphonestateincall();
        void ConstructL();

    private: // New methods

         void SetupL();
         void Teardown();
        
         void T_ConstructionL();
         void T_NumberAcqCmdVideoCall();
         void T_HandleStoppedDTMFL();
        

    private: // Data

        CPhoneViewCommandHandleMock* iViewCommandHandle;
        CCoeEnv* iCoeEnvMock;
        CPhoneStateMachineGSM* iStateMachine;
        CPhoneStateInCall* iStateInCall;
        CPEEngineInfo* iEngineInfoMock;
        CSmcMockContext* iMockContext;
        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONESTATEINCALL_H__
