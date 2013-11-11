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

#ifndef __UT_CPhoneStateIncoming_H__
#define __UT_CPhoneStateIncoming_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>
#include "TPhoneCommandParam.h"

// FORWARD DECLARATIONS
class CPhoneViewCommandHandleMock;
class CPhoneStateIncoming;
class CPhoneMainResourceResolverMock;
class CSmcMockContext;
class MPhoneViewCommandHandle;
class CPhoneCustomizationVoip;
class MPhoneStateMachine;
class CPEEngineInfo;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_CPhoneStateIncoming )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static UT_CPhoneStateIncoming* NewL();
        static UT_CPhoneStateIncoming* NewLC();
        ~UT_CPhoneStateIncoming();

    private: // Constructors

        UT_CPhoneStateIncoming();
        void ConstructL();
             
    private: // New methods

         void SetupL();
         void Teardown();
        
         void T_HandleKeyMessageL();
         void T_HandleCommandL(); 
         void T_HandleConnectedL();
         void T_HandleAudioPlayStoppedL();
         void T_HandleSendKeyAnswerL();
         void T_HandleSendKeyRejectL();
         
    private: // Data

        CPhoneStateIncoming* iCPhoneStateIncoming; 
        MPhoneStateMachine* iStateMachineMock;
        CPhoneViewCommandHandleMock* iViewCmdHandleMock;
        CPhoneCustomizationVoip* iCustomizationMock;
        CSmcMockContext* iMockContext;
        CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;   
        CPEEngineInfo* iEngineInfoMock;
        
        EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_CPhoneStateIncoming_H__
