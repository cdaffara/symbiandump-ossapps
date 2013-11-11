/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __T_CGLXGETREQUEST_H__
#define __T_CGLXGETREQUEST_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "glxgetrequest.h"

//  FORWARD DECLARATIONS


//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( T_CGlxGetRequest )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CGlxGetRequest* NewL();
        static T_CGlxGetRequest* NewLC();
        /**
         * Destructor
         */
        ~T_CGlxGetRequest();

    private:    // Constructors and destructors

        T_CGlxGetRequest();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_CGlxGetRequest_MediaIdsL();
        
         void T_CGlxGetRequest_AttributesL();
        

    private:    // Data
		
        CGlxGetRequest* iCGlxGetRequest;
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_CGLXGETREQUEST_H__

// End of file
