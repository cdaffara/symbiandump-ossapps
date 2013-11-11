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

#ifndef __UT_CPHONEACCESSORYBTHANDLER_H__
#define __UT_CPHONEACCESSORYBTHANDLER_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include "MPhoneNEClearedHandler.h"

// FORWARD DECLARATIONS
class CPhoneMainResourceResolverMock;
class CSmcMockContext;
class CCoeEnv;
class CPEEngineInfo;
class CPhoneAccessoryBTHandler;
class CPhoneViewCommandHandleMock;
class CPhoneStateMachineGSM;
class MPhoneNEClearedHandler;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_CPhoneAccessoryBTHandler )
    : public CEUnitTestSuiteClass,
    public MPhoneNEClearedHandler
    {
    public:  // Constructors and destructor

        static UT_CPhoneAccessoryBTHandler* NewL();
        static UT_CPhoneAccessoryBTHandler* NewLC();
        ~UT_CPhoneAccessoryBTHandler();
   
    private: // MPhoneNEClearedHandler
        
        void HandleNumberEntryClearedL();  

    private: // Constructors

        UT_CPhoneAccessoryBTHandler();
        void ConstructL();
        
    private: // New methods

         void SetupL();
         void Teardown();
        
         void UT_SetBTHandsfreeModeL_ONL();
         void UT_SetBTHandsfreeModeL_OFFL();
         void UT_SetBTHandsfreeModeL_ON_AudioRoutedToWiredAudioAccessoryL();
         
         void UT_SetHandsfreeModeL_ONL();
         void UT_SetHandsfreeModeL_OFFL();
         void UT_SetHandsfreeModeL_ON_AudioRoutedToWiredAudioAccessoryL();
        
         void UT_ShowBTAddressLL();
         void UT_ShowBTLoopbackL();
         void UT_ShowBTActivatedL();

    private: // Data
         
         CPhoneMainResourceResolverMock* iCPhoneMainResourceResolverMock;
         CPhoneViewCommandHandleMock* iViewCmdHandleMock;
         CPhoneStateMachineGSM* iStateMachineMock;
         CCoeEnv* iCoeEnvMock;
         CSmcMockContext* iMockContext;
         CPhoneAccessoryBTHandler* iCPhoneAccessoryBTHandler;
         TBool iEntryCleared;
         CPEEngineInfo* iEngineInfoMock;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_CPHONEACCESSORYBTHANDLER_H__
