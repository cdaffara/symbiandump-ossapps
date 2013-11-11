/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   CGlxFetchErrorArray unit tests
*
*/



#ifndef __T_CGLXFETCHERRORARRAY_H__
#define __T_CGLXFETCHERRORARRAY_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

//  INTERNAL INCLUDES

//  FORWARD DECLARATIONS
class CGlxFetchErrorArray;

//  CLASS DEFINITION
/**
 * T_CGlxFetchErrorArray Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( T_CGlxFetchErrorArray )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CGlxFetchErrorArray* NewL();
        static T_CGlxFetchErrorArray* NewLC();
        /**
         * Destructor
         */
        ~T_CGlxFetchErrorArray();

    private:    // Constructors and destructors

        T_CGlxFetchErrorArray();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_CGlxFetchErrorArray_ErrorCountL();
        
         void T_CGlxFetchErrorArray_ErrorL();
        
         void T_CGlxFetchErrorArray_AddErrorLL();
        
         void T_CGlxFetchErrorArray_RemoveL();
        
         void T_CGlxFetchErrorArray_FindErrorL();
        
         void T_CGlxFetchErrorArray_FindError2L();
        

    private:    // Data
		
        CGlxFetchErrorArray* iCGlxFetchErrorArray1;
        CGlxFetchErrorArray* iCGlxFetchErrorArray2;
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_CGLXFETCHERRORARRAY_H__

// End of file
