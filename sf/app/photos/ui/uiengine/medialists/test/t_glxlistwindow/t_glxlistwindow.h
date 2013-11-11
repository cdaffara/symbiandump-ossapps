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
* Description:   Unit tests
*
*/



#ifndef __T_GLXLISTWINDOW_H__
#define __T_GLXLISTWINDOW_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

//  INTERNAL INCLUDES

//  FORWARD DECLARATIONS
    
/**
 * EUnit test suite
 */
NONSHARABLE_CLASS( t_CGlxListWindow ) : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors
        /**
         * Two phase construction
         */
        static t_CGlxListWindow* NewL();
        static t_CGlxListWindow* NewLC();
        /**
         * Destructor
         */
        ~t_CGlxListWindow();

    private:    // Constructors and destructors
        t_CGlxListWindow();
        void ConstructL();

    private:    // New methods
         void SetupL();
         void Teardown();
        
         void T_CleanupL();
         void T_SetRangeOffsetsLL();
         void T_SetFocusIndexL();
         void T_AddObjectsL();
         void T_RemoveObjectsL();
                    
    private:    // Data
		
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __T_GLXLISTWINDOW_H__
