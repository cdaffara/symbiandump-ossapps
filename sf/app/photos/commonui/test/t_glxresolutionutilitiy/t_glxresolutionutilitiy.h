/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __T_GLXRESOLUTIONUTILITIY_H__
#define __T_GLXRESOLUTIONUTILITIY_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>


//  INTERNAL INCLUDES
#include "mglxresolutionchangeobserver.h"

#include "glxresolutionmanager.h"

//  FORWARD DECLARATIONS
class CGlxHuiUtility;
class CGlxResolutionUtility;


//  CLASS DEFINITION
/**
 * TODO Auto-generated EUnit test suite
 *
 */
NONSHARABLE_CLASS( T_CGlxResolutionUtility ) 
	: public CEUnitTestSuiteClass,
	  public MGlxResolutionChangeObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static T_CGlxResolutionUtility* NewL();
        static T_CGlxResolutionUtility* NewLC();
        /**
         * Destructor
         */
        ~T_CGlxResolutionUtility();


        void HandleResolutionChanged ();
        
    private:    // Constructors and destructors

        T_CGlxResolutionUtility();
        void ConstructL();

    private:    // New methods

         void SetupL();
         void Teardown();
         void T_TestTvConnectedL();

    private:    // Data

		CGlxResolutionUtility*  iResolutionUtility;
		CGlxResolutionManager*  iResolutionMgr;
        TSize iSize;
        TReal32 iExpectedHeightPoss;
        TReal32 iExpectedWidthPoss;
                
        EUNIT_DECLARE_TEST_TABLE; 
    };

#endif      //  __T_GLXRESOLUTIONUTILITIY_H__

// End of file
