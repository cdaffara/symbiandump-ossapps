/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Test for singleton store
*
*/



#ifndef __UT_CGlxSingletonStore_H__
#define __UT_CGlxSingletonStore_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CGlxSingletonStore;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( ut_CGlxSingletonStore )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static ut_CGlxSingletonStore* NewL();
        static ut_CGlxSingletonStore* NewLC();
        /**
         * Destructor
         */
        ~ut_CGlxSingletonStore();

    private:    // Constructors and destructors

        ut_CGlxSingletonStore();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void ut_CGlxSingletonStore_TestL();
        
     

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_CGlxSingletonStore_H__

// End of file
