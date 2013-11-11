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

#ifndef __UT_CPHONECALLSETUPWAITING_H__
#define __UT_CPHONECALLSETUPWAITING_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS
class CPhoneCallSetupAndWaiting;
class CPhoneStateMachineStub;
class CPhoneViewController;
class CSmcMockContext;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphonecallsetupandwaiting )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonecallsetupandwaiting* NewL();
        static ut_cphonecallsetupandwaiting* NewLC();
        ~ut_cphonecallsetupandwaiting();

    private: // Constructors

        ut_cphonecallsetupandwaiting();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
         
         void T_HandleIdleL();        

    private: // Data
   
        CPhoneViewController* iViewCommandHandle;
                        
        CPhoneStateMachineStub* iStateMachine; 
                    
        CPhoneCallSetupAndWaiting * iStateCallSetupAndWaiting;

        CSmcMockContext* iMockContext;
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONECALLSETUPWAITING_H__
