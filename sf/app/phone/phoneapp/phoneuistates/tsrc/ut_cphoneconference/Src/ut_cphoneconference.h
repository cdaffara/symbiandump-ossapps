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

#ifndef __UT_CPHONECONFERENCE_H__
#define __UT_CPHONECONFERENCE_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>
#include "TPhoneCommandParam.h"


// FORWARD DECLARATIONS
class CPhoneConference;
class CPhoneStateMachineStub;
class CPhoneViewController;
class CSmcMockContext;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphoneconference )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphoneconference* NewL();
        static ut_cphoneconference* NewLC();
        ~ut_cphoneconference();

    private: // Constructors

        ut_cphoneconference();
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
                    
        CPhoneConference * iStateConference;
        
        CSmcMockContext* iMockContext;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONECONFERENCE_H__
