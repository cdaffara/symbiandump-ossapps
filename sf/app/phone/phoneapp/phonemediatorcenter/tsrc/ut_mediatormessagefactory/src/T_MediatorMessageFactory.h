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

#ifndef __T_MEDIATORMESSAGEFACTORY_H__
#define __T_MEDIATORMESSAGEFACTORY_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>



// FORWARD DECLARATIONS
class CPhoneMediatorMessageFactory;

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( T_MediatorMessageFactory )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static T_MediatorMessageFactory* NewL();
        static T_MediatorMessageFactory* NewLC();
        ~T_MediatorMessageFactory();

    private: // Constructors

        T_MediatorMessageFactory();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void TestCreatingMessageL();
         void TestNoMessageForEngineMessageL();
        

    private: // Data

        CPhoneMediatorMessageFactory* iFactory;

        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __T_MEDIATORMESSAGEFACTORY_H__
