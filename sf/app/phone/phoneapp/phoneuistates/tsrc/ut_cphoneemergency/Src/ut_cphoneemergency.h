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

#ifndef __UT_CPHONEEMERGENCY_H__
#define __UT_CPHONEEMERGENCY_H__

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
class CPhoneEmergency;
// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */


TBool operator==(const TPhoneCommandParam& param1, const TPhoneCommandParam& param2 );

NONSHARABLE_CLASS( ut_cphoneemergency )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphoneemergency* NewL();
        static ut_cphoneemergency* NewLC();
        ~ut_cphoneemergency();

    private: // Constructors

        ut_cphoneemergency();
        void ConstructL();

    private: // New methods

         void SetupL();

         void Teardown();

         void T_HandleConnectingL();

         void T_HandleDialingL();

         void T_HandleConnectedL();

         void T_HandleIdleEmergencyL();

         void T_HandleAudioOutputChangedL();

         void T_EmergencyCallSetup_HandleEPEMessageConnectedL();

         void T_HandleIdleEmergencyWithNeOpenL();
         
         void T_HandleKeyEventSimNotUsableL();
         
         void T_HandleOtherIdleThanEmergency();
         
         void T_HandleIdleEmergencySecurityNoteShownSimlessOfflineOffL();
         
         void T_HandleIdleEmergencySecurityNoteShownSimlessOfflineOnL();
         
         void T_HandleIdleEmergencySecurityNoteNotShownSimlessOfflineOffL();
         
         void T_HandleIdleEmergencySecurityNoteNotShownSimlessOfflineOnL();
         
         void T_HandleRemConCommand_MuteL();

    private: // Data

        MPhoneViewCommandHandle* iViewCmdHandleMock;

        CPhoneStateMachineGSM* iStateMachineMock;

        CSmcMockContext* iMockContext;

        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;

        CPEEngineInfo* iEngineInfoMock;

        CPhoneEmergency * iStateEmergency;
        
        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONEEMERGENCY_H__
