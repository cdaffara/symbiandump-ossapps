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

#ifndef __UT_CPHONESINGLECALL_H__
#define __UT_CPHONESINGLECALL_H__


// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS
class CSmcMockContext;
class CPhoneStateMachineGSM;
class CPhoneViewCommandHandleMock;
class CPhoneMainResourceResolverMock;
class CPEEngineInfo;
class MPhoneViewCommandHandle;
class TPhoneCommandParam;
class CPhoneSingleCall;
// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */


//TBool operator==(const TPhoneCommandParam& param1, const TPhoneCommandParam& param2 );

NONSHARABLE_CLASS( ut_cphonesinglecall )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonesinglecall* NewL();
        static ut_cphonesinglecall* NewLC();
        ~ut_cphonesinglecall();

    private: // Constructors

        ut_cphonesinglecall();
        void ConstructL();

    private: // New methods

         void SetupL();

         void Teardown();

         void T_HandleConnectedL();
         
         void T_ConstructL();
 
    private: // Data

        MPhoneViewCommandHandle* iViewCmdHandleMock;

        CPhoneStateMachineGSM* iStateMachineMock;

        CSmcMockContext* iMockContext;

        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;

        CPEEngineInfo* iEngineInfoMock;

        CPhoneSingleCall * iStateSingleCall;
        
        EUNIT_DECLARE_TEST_TABLE;

    };
 
#endif      //  __UT_CPHONESINGLECALL_H__
