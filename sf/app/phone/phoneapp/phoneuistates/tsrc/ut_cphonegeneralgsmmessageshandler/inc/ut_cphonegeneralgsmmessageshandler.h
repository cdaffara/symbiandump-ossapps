/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
/*
-----------------------------------------------------------------------------
This file has been generated with EUnit Pro
http://www.digia.com/eunit
-----------------------------------------------------------------------------
*/
#ifndef __UT_CPHONEGENERALGSMMESSAGESHANDLER_H__
#define __UT_CPHONEGENERALGSMMESSAGESHANDLER_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


// FORWARD DECLARATIONS
class CSmcMockContext;
class CPhoneGeneralGsmMessagesHandler;
class MPhoneStateMachine;
class MPhoneViewCommandHandle;
class CCoeEnv;
class CPhoneMainResourceResolver;
class CPhoneState;
class TPhoneCommandParam;

// CLASS DEFINITION

TBool operator==(const TPhoneCommandParam& param1, const TPhoneCommandParam& param2 );

/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphonegeneralgsmmessageshandler )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonegeneralgsmmessageshandler* NewL();
        static ut_cphonegeneralgsmmessageshandler* NewLC();
        ~ut_cphonegeneralgsmmessageshandler();

    private: // Constructors

        ut_cphonegeneralgsmmessageshandler();
        void ConstructL();

    private: // New methods

        void SetupL();
        
        void Teardown();
        
        void T_ConstructionL();
        
        void T_HandlePhoneEngineMessageL_EPEMessageIssuingSSRequest_L();
         
        void T_HandlePhoneEngineMessageL_EPEMessageIssuedSSRequest_L();

    private: // Data

        CPhoneGeneralGsmMessagesHandler* iPhoneGeneralGsmMessagesHandler;
        CSmcMockContext* iMockContext;
        MPhoneStateMachine* iStateMachineMock;
        CPhoneState* iMockState;
        MPhoneViewCommandHandle* iViewCommandHandleMock;
        CCoeEnv* iCoeEnvMock;
        CPhoneMainResourceResolver* iCPhoneMainResourceResolverMock;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONEGENERALGSMMESSAGESHANDLER_H__
