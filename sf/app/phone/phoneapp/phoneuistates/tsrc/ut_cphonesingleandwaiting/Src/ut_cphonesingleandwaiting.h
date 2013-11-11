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

#ifndef __UT_CPHONESINGLEANDWAITING_H__
#define __UT_CPHONESINGLEANDWAITING_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>
#include "CPhoneSingleAndWaiting.h"

// FORWARD DECLARATIONS
class CSmcMockContext;
class CPhoneStateMachineGSM;
class CPhoneViewCommandHandleMock;
class CPhoneMainResourceResolverMock;
class CPEEngineInfo;
class MPhoneViewCommandHandle;
class TPhoneCommandParam;
class CPhoneSingleAndWaiting;
// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphonesingleandwaiting )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonesingleandwaiting* NewL();
        static ut_cphonesingleandwaiting* NewLC();
        ~ut_cphonesingleandwaiting();

    private: // Constructors

        ut_cphonesingleandwaiting();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
         
         void T_HandleIdle();
         
         void TestBubbleAndUICommandsAreUpdatedWhenCallIsPutToHoldL();
         void TestBubbleAndUICommandsAreUpdatedWhenCallIsResumedL();

    private: // Data
   
        MPhoneViewCommandHandle* iViewCmdHandleMock;

        CPhoneStateMachineGSM* iStateMachineMock;

        CSmcMockContext* iMockContext;

        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;

        CPEEngineInfo* iEngineInfoMock;
              
        CPhoneSingleAndWaiting* iState;            

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONESINGLEANDWAITING_H__
