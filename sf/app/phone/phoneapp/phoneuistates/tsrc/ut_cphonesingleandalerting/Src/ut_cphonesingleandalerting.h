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

#ifndef __UT_CPHONESINGLEANDALERTING_H__
#define __UT_CPHONESINGLEANDALERTING_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS
class CPhoneSingleAndAlerting;
class CPhoneStateMachineStub;
class CPhoneViewController;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphonesingleandalerting )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonesingleandalerting* NewL();
        static ut_cphonesingleandalerting* NewLC();
        ~ut_cphonesingleandalerting();

    private: // Constructors

        ut_cphonesingleandalerting();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
          
         void TestCallHeaderIsRemovedWhenCallGoesIdleL();

    private: // Data
   
        CPhoneViewController* iViewCommandHandle;
                        
        CPhoneStateMachineStub* iStateMachine; 
                    
        CPhoneSingleAndAlerting * iStateSingleAndAlerting;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONESINGLEANDALERTING_H__
