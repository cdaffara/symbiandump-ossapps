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

#ifndef __UT_CPHONECALLSETUP_H__
#define __UT_CPHONECALLSETUP_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

#include "cphonecallsetup.h"
#include "CPhoneState.h"
#include "cphonestatecallsetupstub.h"

// FORWARD DECLARATIONS
class CPhoneViewController;
class CPhoneState;
class CPhoneStateCallSetup;
class TPhoneCmdParamBoolean;
class TPhoneCmdParamCallHeaderData;
class CPhoneMainResourceResolver;
class CPhoneStateMachineStub;

// CLASS DEFINITION
/**
 * this class was done to prevent ambigous access to CBase(CPhoneState inherits CBase also).
 */
class CallSetupWrapperClass : public CPhoneCallSetup
    {
    public:  // Constructors and destructor

        static CallSetupWrapperClass* NewL(
                MPhoneStateMachine* aStateMachine, 
                MPhoneViewCommandHandle* aViewCommandHandle,
                MPhoneCustomization* aPhoneCustomization );
        
        virtual ~CallSetupWrapperClass();
        
        void HandleConnectingL( TInt aCallId );
        
        
    protected:
       CallSetupWrapperClass( MPhoneStateMachine* aStateMachine, 
                MPhoneViewCommandHandle* aViewCommandHandle,
                MPhoneCustomization* aPhoneCustomization  );
        
        virtual void ConstructL();
    };

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_cphonecallsetup )
    : public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_cphonecallsetup* NewL();
        static ut_cphonecallsetup* NewLC();
        ~ut_cphonecallsetup();

    private: // Constructors

        ut_cphonecallsetup();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void T_CPhoneCallSetup__CPhoneCallSetupL();
        
         void T_CPhoneCallSetup_HandleHandleConnectingMessageL();
         
         TInt VerifyViewCommand( const TInt& aCmd );

         void VerifyEPhoneViewRemoveGlobalNoteIsNotUsed();
         
    private: // Data

        CPhoneViewController* iViewCommandHandle;
        CPhoneStateMachineStub* iStateMachine;
        CPhoneMainResourceResolver* iStubResourceResolver;
        CallSetupWrapperClass* iWrapper;

        EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_CPHONECALLSETUP_H__
