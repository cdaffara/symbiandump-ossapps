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
* Description: 
*
*/



#ifndef __UT_GLXERRORMANAGER_H__
#define __UT_GLXERRORMANAGER_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CGlxMedia;

//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( ut_glxerrormanager )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static ut_glxerrormanager* NewL();
        static ut_glxerrormanager* NewLC();
        /**
         * Destructor
         */
        ~ut_glxerrormanager();

    private:    // Constructors and destructors

        ut_glxerrormanager();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_Test1L();
         void T_Test2L();
         void T_Test3L();
         void T_Test4L();
         void T_Test5L();
         void T_Test6L();
        

    private:    // Data
		

        EUNIT_DECLARE_TEST_TABLE; 

        CGlxMedia* iItem1;
        CGlxMedia* iItem2;
        CGlxMedia* iItem3;
        CGlxMedia* iItem4;
        CGlxMedia* iItem5;
        CGlxMedia* iItem6;
    };

#endif      //  __UT_GLXERRORMANAGER_H__

// End of file
