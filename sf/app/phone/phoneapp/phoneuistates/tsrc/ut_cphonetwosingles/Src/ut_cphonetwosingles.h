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

#ifndef __UT_CPHONETWOSINGLES_H__
#define __UT_CPHONETWOSINGLES_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS
class CPhoneTwoSingles;
class CPhoneStateMachineStub;
class CPhoneViewController;
class CSmcMockContext; 

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphonetwosingles )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonetwosingles* NewL();
        static ut_cphonetwosingles* NewLC();
        ~ut_cphonetwosingles();

    private: // Constructors

        ut_cphonetwosingles();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void T_OpenMenuBarL();
         
         void T_HandleIdleL();
             
        

    private: // Data
   
        CPhoneViewController* iViewCommandHandle;
                        
        CPhoneStateMachineStub* iStateMachine; 
                    
        CPhoneTwoSingles * iStateTwoSingles;

        CSmcMockContext* iMockContext; 
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONETWOSINGLES_H__
