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

#ifndef __UT_CPHONECONFERENCEANDWAITING_H__
#define __UT_CPHONECONFERENCEANDWAITING_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS

class CPhoneConferenceAndWaiting;

class CSmcMockContext;
class CPhoneStateMachineGSM;
class MPhoneViewCommandHandle;
class TPhoneCommandParam;
class CPhoneMainResourceResolverMock;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
TBool operator==(const TPhoneCommandParam& param1, const TPhoneCommandParam& param2 );

NONSHARABLE_CLASS( ut_cphoneconferenceandwaiting )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphoneconferenceandwaiting* NewL();
        static ut_cphoneconferenceandwaiting* NewLC();
        ~ut_cphoneconferenceandwaiting();

    private: // Constructors

        ut_cphoneconferenceandwaiting();
        void ConstructL();

    private: // New methods

         void SetupL();
         void Setup_NoRingingCallIdL();
        
         void Teardown();
         
         void T_ConstructionL();
         
         void T_HandleConnected_1L();
         void T_HandleConnected_2L();
         
         void T_HandleIdleL_1();
         void T_HandleIdleL_2();
         void T_HandleIdleL_3();
         void T_HandleIdleL_4();
         
         void T_HandleWentOneToOne_1L();

        

    private: // Data

        MPhoneViewCommandHandle* iViewCmdHandleMock;

        CPhoneStateMachineGSM* iStateMachineMock;
                    
        CPhoneConferenceAndWaiting* iStateConferenceAndWaiting;
        
        CSmcMockContext* iMockContext;
        
        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;


        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONECONFERENCEANDWAITING_H__
