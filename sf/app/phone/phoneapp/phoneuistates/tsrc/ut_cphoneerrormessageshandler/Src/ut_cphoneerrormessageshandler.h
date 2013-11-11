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
#ifndef __ut_cphoneerrormessageshandler_H__
#define __ut_cphoneerrormessageshandler_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include "cpeengineinfo.h"
#include <phonerssbase.h>

// FORWARD DECLARATIONS
class CSmcMockContext;
class CPhoneErrorMessagesHandler;
class MPhoneStateMachine;
class MPhoneViewCommandHandle;
class CCoeEnv;
class CPEEngineInfo;
class CPhoneMainErrorMessagesHandler;
class CPhoneMainResourceResolver;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphoneerrormessageshandler )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphoneerrormessageshandler* NewL();
        static ut_cphoneerrormessageshandler* NewLC();
        ~ut_cphoneerrormessageshandler();

    private: // Constructors

        ut_cphoneerrormessageshandler();
        void ConstructL();

    private: // New methods

         void SetupL();

         void Teardown();

         void T_Global_ShowErrorSpecificNoteForVideoCallL();

         void T_Global_ShowErrorSpecificNoteForCsCallL();

         void VerifyNote( TPECallType aCallType, TCCPError aErrorCode, TPhoneRssBase aNote );
         void VerifyNoNote( TPECallType aCallType, TCCPError aErrorCode );
         void T_Global_ShowErrorSpecificNoteL();

    private: // Data

        CPhoneErrorMessagesHandler* iCPhoneErrorMessagesHandler;
        CSmcMockContext* iMockContext;
        MPhoneStateMachine* iStateMachineMock;
        MPhoneViewCommandHandle* iViewCommandHandleMock;
        CCoeEnv* iCoeEnvMock;
        CPhoneMainErrorMessagesHandler* iCPhoneMainErrorMessagesHandlerMock;
        CPhoneMainResourceResolver* iCPhoneMainResourceResolverMock;
        CPEEngineInfo* iEngineInfoMock;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __ut_cphoneerrormessageshandler_H__
