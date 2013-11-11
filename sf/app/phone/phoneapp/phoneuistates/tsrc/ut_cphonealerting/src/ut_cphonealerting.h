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

#ifndef __UT_CPHONEALERTING_H__
#define __UT_CPHONEALERTING_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS
class CSmcMockContext;
class CPhoneAlerting;
class CPhoneStateMachineGSM;
class CPhoneViewCommandHandleMock;
class CPhoneMainResourceResolverMock;
class CPEEngineInfo;
class MPhoneViewCommandHandle;
class TPhoneCommandParam;
class MPhoneState;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */


TBool operator==(const TPhoneCommandParam& param1, const TPhoneCommandParam& param2 ); 

NONSHARABLE_CLASS( ut_cphonealerting )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonealerting* NewL();
        static ut_cphonealerting* NewLC();
        ~ut_cphonealerting();

    private: // Constructors

        ut_cphonealerting();
        void ConstructL();

    private: // New methods

         void SetupL();
         void Teardown();
                  
         
         void T_HandlePhoneEngineMessageIncomingL();
         void T_HandlePhoneEngineMessageIdleL();
         void T_HandlePhoneEngineMessageIdleAfterIncomingL();
         
    private: // Data
    
        MPhoneViewCommandHandle* iViewCmdHandleMock;
        CPhoneStateMachineGSM* iStateMachineMock;
        CPhoneAlerting * iStateAlerting;
        CSmcMockContext* iMockContext;
        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;
        CPEEngineInfo* iEngineInfoMock;
        MPhoneState* iPhoneIdleMock;
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONEALERTING_H__
