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

/*
-----------------------------------------------------------------------------
This file has been generated with EUnit Pro
http://www.sysopendigia.com/qualitykit
-----------------------------------------------------------------------------
*/

/**
 * @internal reviewed 5/02/2007 by Alex Birkett
 */

#ifndef __T_GLXZOOMSTATEPUBLISHER_H__
#define __T_GLXZOOMSTATEPUBLISHER_H__

// INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

// FORWARD DECLARATIONS

// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( T_GlxZoomStatePublisher )
	: public CEUnitTestSuiteClass
    {
    public:  // Constructors and destructor

        static T_GlxZoomStatePublisher* NewL();
        static T_GlxZoomStatePublisher* NewLC();
        ~T_GlxZoomStatePublisher();

    private: // Constructors

        T_GlxZoomStatePublisher();
        void ConstructL();

    private: // New methods

         void SetupL();
        
         void Teardown();
        
         void T_TestL();
        

    private: // Data



        EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __T_GLXZOOMSTATEPUBLISHER_H__
