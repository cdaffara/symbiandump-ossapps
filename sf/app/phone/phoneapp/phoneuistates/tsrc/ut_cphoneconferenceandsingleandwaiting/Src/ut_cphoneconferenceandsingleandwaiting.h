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

#ifndef __UT_CPHONECONFERENCEANDSINGLEANDWAITING_H__
#define __UT_CPHONECONFERENCEANDSINGLEANDWAITING_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS
class CPhoneConferenceAndSingleAndWaiting;
class CPhoneStateMachineStub;
class CPhoneViewController;
class CSmcMockContext;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphoneconferenceandsingleandwaiting )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphoneconferenceandsingleandwaiting* NewL();
        static ut_cphoneconferenceandsingleandwaiting* NewLC();
        ~ut_cphoneconferenceandsingleandwaiting();

    private: // Constructors

        ut_cphoneconferenceandsingleandwaiting();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void T_HandleConnectedL();
         
         void T_HandleIdleL();
         
         void T_HandleConferenceIdleL();
        

    private: // Data
   
        CPhoneViewController* iViewCommandHandle;
                        
        CPhoneStateMachineStub* iStateMachine; 
                    
        CPhoneConferenceAndSingleAndWaiting * iStateConferenceAndSingleAndWaiting;

        CSmcMockContext* iMockContext;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONECONFERENCEANDSINGLEANDWAITING_H__
