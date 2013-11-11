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

#ifndef __UT_CPHONEIDLE_H__
#define __UT_CPHONEIDLE_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS
class CSmcMockContext;
class CPhoneIdle;
class MPhoneStateMachine;
class CPhoneViewController;
class CPhoneIdle_Inherited;
 

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphoneidle )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphoneidle* NewL();
        static ut_cphoneidle* NewLC();
        ~ut_cphoneidle();

    private: // Constructors

        ut_cphoneidle();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void T_ConstructionL();
         
         void T_HandlePhoneEngineMessageL();
         
         void T_HandleErrorL();

    private: // Data
   
      
        CPhoneIdle * iStateIdle;
        
        CSmcMockContext* iMockContext;
             
        MPhoneStateMachine* iStateMachineMock;
 
        CPhoneViewController* iViewCommandHandle;
        
        CPhoneIdle_Inherited* iIdleInherited;
 
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONEIDLE_H__
