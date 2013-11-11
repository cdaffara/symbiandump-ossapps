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
* Description:   Unit tests, entry point
*
*/



#ifndef __UT_CGLXSTATICITEMLIST_H__
#define __UT_CGLXSTATICITEMLIST_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "glxstaticitemlist.h"
#include "glxlistreconstruction.h"
#include "glxlisttestbase.h"

//  FORWARD DECLARATIONS

/**
 * Reconstruction of a static item list
 */
NONSHARABLE_CLASS( CGlxStaticItemListReconstruction ) : public CGlxListReconstruction< CGlxStaticItemList >
    {
    public:    
        static CGlxStaticItemListReconstruction* NewL( TInt aReservationCount = 400 );
        // From CGlxListReconstructionBase
        TBool ReconstructionEquals() const;
    };

/**
 * EUnit test suite
 */
NONSHARABLE_CLASS( ut_CGlxStaticItemList ) : 
        public CGlxListTestBase< CGlxStaticItemList, CGlxStaticItemListReconstruction >
    {
    public:     // Constructors and destructors
        /**
         * Two phase construction
         */
        static ut_CGlxStaticItemList* NewL();
        static ut_CGlxStaticItemList* NewLC();
        /**
         * Destructor
         */
        ~ut_CGlxStaticItemList();

    private:    // Constructors and destructors
        ut_CGlxStaticItemList();
        void ConstructL();

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
        void T_SetStaticItemsEnabledL();
        void T_AddStaticItemLL();

    private:    // Data
        EUNIT_DECLARE_TEST_TABLE; 
    };

#endif      //  __UT_CGLXSTATICITEMLIST_H__

// End of file
