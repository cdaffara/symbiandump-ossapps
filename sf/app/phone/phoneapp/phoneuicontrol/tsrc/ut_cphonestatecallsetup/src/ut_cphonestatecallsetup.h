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

#ifndef __UT_CPHONESTATECALLSETUP_H
#define __UT_CPHONESTATECALLSETUP_H

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS
class CSmcMockContext;
class CPhoneStateCallSetup;
class CPhoneStateMachineGSM;
class CPhoneViewCommandHandleMock;
class CPhoneMainResourceResolverMock;
class CPEEngineInfo;
class MPhoneViewCommandHandle;
class TPhoneCommandParam;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */


TBool operator==(const TPhoneCommandParam& param1, const TPhoneCommandParam& param2 ); 

NONSHARABLE_CLASS( ut_cphonestatecallsetup )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonestatecallsetup* NewL();
        static ut_cphonestatecallsetup* NewLC();
        ~ut_cphonestatecallsetup();

    private: // Constructors

        ut_cphonestatecallsetup();
        void ConstructL();

    private: // New methods

         void SetupL();
         void Teardown();
         void T_EmptyL();

    private: // Data
    
        CPhoneViewCommandHandleMock* iViewCmdHandleMock;
        CPhoneStateMachineGSM* iStateMachineMock; 
        CPhoneStateCallSetup* iStateCallSetup;
        CSmcMockContext* iMockContext;
        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;
        CPEEngineInfo* iEngineInfoMock;
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONESTATECALLSETUP_H
