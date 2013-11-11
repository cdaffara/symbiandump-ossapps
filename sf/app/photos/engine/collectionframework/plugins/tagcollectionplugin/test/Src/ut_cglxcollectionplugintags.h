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



#ifndef __ut_cglxcollectionplugintags_H__
#define __ut_cglxcollectionplugintags_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>
#include <glxcollectionplugintags.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS


//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( ut_cglxcollectionplugintags )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static ut_cglxcollectionplugintags* NewL();
        static ut_cglxcollectionplugintags* NewLC();
        /**
         * Destructor
         */
        ~ut_cglxcollectionplugintags();

    private:    // Constructors and destructors

        ut_cglxcollectionplugintags();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void ut_cglxcollectionplugintagsL();
        

    private:    // Data
		
        
        EUNIT_DECLARE_TEST_TABLE; 
        
        CGlxCollectionPluginTags* iCollectionPluginTags;

    };

#endif      //  __ut_cglxcollectionplugintags_H__

// End of file
