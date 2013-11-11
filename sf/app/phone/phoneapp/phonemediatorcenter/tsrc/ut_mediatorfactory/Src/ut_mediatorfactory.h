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

#ifndef __UT_MEDIATORFACTORY_H__
#define __UT_MEDIATORFACTORY_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

class CPhoneMediatorFactory;

// FORWARD DECLARATIONS


// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( ut_mediatorfactory )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static ut_mediatorfactory* NewL();
        static ut_mediatorfactory* NewLC();
        ~ut_mediatorfactory();

    private: // Constructors

        ut_mediatorfactory();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void TestInitialisingCommandListenerL();
         void TestCommandListenerNotInitL();
         void TestMPhoneMenuAndCbaEventsPreCondCheckL();
         void TestMPhoneEngineMessageSenderPreCondCheckL();
         void TestEngineInfoPreCondCheckL();
         void TestMediatorMessageFactoryCheckL();
         void CreateMessageFailCheckL();
         void SuccesfulMediatorMessageL();
         void TestFetchingSenderL();

    private: // Data

        CPhoneMediatorFactory* iFactory;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __UT_MEDIATORFACTORY_H__
