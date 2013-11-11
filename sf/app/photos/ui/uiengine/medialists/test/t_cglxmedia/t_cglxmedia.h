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
* Description:   CGlxMedia unit tests
*
*/



#ifndef __T_CGLXMEDIA_H__
#define __T_CGLXMEDIA_H__

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
NONSHARABLE_CLASS( T_CGlxMedia )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CGlxMedia* NewL();
        static T_CGlxMedia* NewLC();
        /**
         * Destructor
         */
        ~T_CGlxMedia();

    private:    // Constructors and destructors

        T_CGlxMedia();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_CGlxMedia_DeleteAttributeL();
        
         void T_CGlxMedia_DeleteAttribute2_SubTest1L();
        
         void T_CGlxMedia_DeleteAttribute2_SubTest2L();
        
         void T_CGlxMedia_DeleteAttribute2_SubTest3L();
        

    private:    // Data
		
        CGlxMedia* iCGlxMedia;
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_CGLXMEDIA_H__

// End of file
