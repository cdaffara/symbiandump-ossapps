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

#ifndef __UT_CPHONESINGLEANDCALLSETUPANDWAITING_H__
#define __UT_CPHONESINGLEANDCALLSETUPANDWAITING_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS
class CPhoneSingleAndCallSetupAndWaiting;
class CPhoneStateMachineStub;
class CPhoneViewController;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphsingleandcallsetupandwait )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphsingleandcallsetupandwait* NewL();
        static ut_cphsingleandcallsetupandwait* NewLC();
        ~ut_cphsingleandcallsetupandwait();

    private: // Constructors

        ut_cphsingleandcallsetupandwait();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
         
         void T_HandleIdleL();
             
         void T_RemoteBusyL();

    private: // Data
   
        CPhoneViewController* iViewCommandHandle;
                        
        CPhoneStateMachineStub* iStateMachine; 
                    
        CPhoneSingleAndCallSetupAndWaiting * iStateSingleAndCallSetupAndWaiting;


        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONESINGLEANDCALLSETUPANDWAITING_H__
