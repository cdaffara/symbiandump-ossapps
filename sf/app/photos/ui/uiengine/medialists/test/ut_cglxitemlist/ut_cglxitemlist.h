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



#ifndef __UT_CGLXITEMLIST_H__
#define __UT_CGLXITEMLIST_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "glxitemlist.h"
#include "glxlistreconstruction.h"
#include "glxlisttestbase.h"

//  FORWARD DECLARATIONS

/**
 * Reconstruction of an item list
 */
NONSHARABLE_CLASS( CGlxItemListReconstruction ) : public CGlxListReconstruction< CGlxItemList >
    {
    public:    
        static CGlxItemListReconstruction* NewL( TInt aReservationCount = 400 );
        // From CGlxListReconstructionBase
        TBool ReconstructionEquals() const;
    };

/**
 * EUnit test suite
 */
NONSHARABLE_CLASS( ut_CGlxItemList ) : 
        public CGlxListTestBase< CGlxItemList, CGlxItemListReconstruction >
    {
    public:     // Constructors and destructors
        /**
         * Two phase construction
         */
        static ut_CGlxItemList* NewL();
        static ut_CGlxItemList* NewLC();
        /**
         * Destructor
         */
        ~ut_CGlxItemList();
        void ConstructL();
        
    private:    // Constructors and destructors
        ut_CGlxItemList();
            
    private:    // New methods
        void SetupL();
        void Teardown();

        void T_SetContentsLL();
        void T_RemoveL();
        void T_RemoveReferenceL();
        void T_CountL();
        void T_ItemL();
        void T_IndexL();
        void T_IdSpaceIdL();
        
    private:    // Data
        EUNIT_DECLARE_TEST_TABLE; 
    };

#endif // __UT_CGLXITEMLIST_H__

