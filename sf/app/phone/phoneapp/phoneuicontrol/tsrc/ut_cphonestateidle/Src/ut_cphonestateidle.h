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

#ifndef __UT_CPHONESTATEIDLE_H__
#define __UT_CPHONESTATEIDLE_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

// FORWARD DECLARATIONS

class CPhoneStateIdle_Inherited;
class CPhoneViewCommandHandleMock;
class CPhoneStateMachine;
class CPhoneMainResourceResolverMock;
class CPEEngineInfo;
class CPhoneStateMachineGSM;
class CSmcMockContext;
class CCoeEnv;
class CPhoneCustomizationVoip;
class TPhoneCommandParam;

TBool operator==(const TPhoneCommandParam& param1, const TPhoneCommandParam& param2 );

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphonestateidle )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonestateidle* NewL();
        static ut_cphonestateidle* NewLC();
        ~ut_cphonestateidle();

    private: // Constructors

        ut_cphonestateidle();
        void ConstructL();

    private: // New methods

         void SetupL();
         void Teardown();

         void T_IdlePhoneFocusLost_DiallerNotUsedL();
         void T_IdlePhoneFocusLost_DiallerUsedL();
         void T_IdleEndKeyPressGeneratesCancelNotificationsL();
         void T_HandleKeyMessageL();
         void T_HandleKeyMessageL_AppKeyL();
         void T_HandleKeyMessageL_AppKeyL_SecurityModeOff();
         void T_HandleKeyMessageL_AppKeyL_NEOpen();
         void T_HandleCommand_NumberAcqCmdCallL();
         void T_HandleCommand_NumberAcqCmdSendCommandL();
         void T_DialL();
         void T_DialVideoCallL();
         void T_HandleEndKeyPressSecurityNoteShownSimlessOfflineUndefL();
         void T_HandleEndKeyPressSecurityNoteShownSimlessOfflineDefinedL();
         void T_HandleNumberEntryClearedL();

    private: // Data

         CPhoneViewCommandHandleMock* iViewCmdHandleMock;
        CPhoneStateMachineGSM* iStateMachineGsmMock;
        CPhoneStateIdle_Inherited* iStateIdle;
        CSmcMockContext* iMockContext;
        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;
        CPEEngineInfo* iEngineInfoMock;
        CCoeEnv* iCoeEnvMock;
        CPhoneCustomizationVoip* iCustomizationMock;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONESTATEIDLE_H__
