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


#ifndef __UT_CPHONESTATESTARTUP_H
#define __UT_CPHONESTATESTARTUP_H

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


// FORWARD DECLARATIONS
class CSmcMockContext;
class CPhoneStateStartup;
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

NONSHARABLE_CLASS( ut_cphonestatestartup )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonestatestartup* NewL();
        static ut_cphonestatestartup* NewLC();
        ~ut_cphonestatestartup();

    private: // Constructors

        ut_cphonestatestartup();
        void ConstructL();

    private: // New methods

         void SetupL();
         void Teardown();
         void T_HandleDtmfKeyToneL ();
         
    private: // Data
    
         CPhoneViewCommandHandleMock* iViewCmdHandleMock;
        CPhoneStateMachineGSM* iStateMachineMock;
        CPhoneStateStartup* iStateStartup;
        CSmcMockContext* iMockContext;
        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;
        CPEEngineInfo* iEngineInfoMock;
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONESTATESTARTUP_H
