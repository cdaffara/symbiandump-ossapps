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

#ifndef __T_MEDIATORSENDER_H__
#define __T_MEDIATORSENDER_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

// FORWARD DECLARATIONS
class CPhoneMediatorSender;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( T_MediatorSender )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static T_MediatorSender* NewL();
        static T_MediatorSender* NewLC();
        ~T_MediatorSender();

    private: // Constructors

        T_MediatorSender();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
         void FreeIssueCommandGlobals();
         void VerifyIssueCommandBoilerplateL();
         void VerifyRaiseEventBoilerplateL();
        
         void TestInstanceGotFromCoeEnvL();
         void TestInstanceCreatedL();
         void TestGenericEventsAreRegisteredL();
         void TestGenericEventRegisterationFailsL();         
         void TestSendingCallDataWhenCallHeaderIsRemovedL();
         void TestSendingAudioMuteCommandL();
         void TestSendingAudioUnMuteCommandL();
         void TestAudioMuteCommandBufferedIfMediatorBusyAndResendWhenResponseArrivesL();

         
    private: // Data

        CPhoneMediatorSender* iSender;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __T_MEDIATORSENDER_H__
