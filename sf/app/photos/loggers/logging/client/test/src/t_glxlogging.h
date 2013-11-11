/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __T_GLXLOGGING_H__
#define __T_GLXLOGGING_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS


//  CLASS DEFINITION
/**
 * EUnit test suite for Logging
 *
 */
NONSHARABLE_CLASS( T_GlxLogging )
	: public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_GlxLogging* NewL();
        static T_GlxLogging* NewLC();
        /**
         * Destructor
         */
        ~T_GlxLogging();

    private:    // Constructors and destructors

        T_GlxLogging();
        void ConstructL();

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void T_TestWithoutChunkL();
         void T_TestWithChunk1L();
         void T_TestWithBothChunksL();
        

    private:    // Data
		

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_GLXLOGGING_H__

// End of file
