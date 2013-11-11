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



#ifndef __UT_CGLXNAVIGABLELIST_H__
#define __UT_CGLXNAVIGABLELIST_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include <EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "glxnavigablelist.h"
#include "glxlistreconstruction.h"
#include "glxlisttestbase.h"

//  FORWARD DECLARATIONS

/**
 * Reconstruction of a navigable item list
 */
NONSHARABLE_CLASS( CGlxNavigableListReconstruction ) : public CGlxListReconstruction< CGlxNavigableList >
    {
    public:
        static CGlxNavigableListReconstruction* NewL( TInt aReservationCount = 400 );
        // From CGlxListReconstructionBase
        TBool ReconstructionEquals() const;
        
    private:
        TBool SelectionEquals() const;
        TInt FindInOriginalSelection( TInt aSelectionIndex ) const;
    };
    
/**
 * EUnit test suite
 */
NONSHARABLE_CLASS( ut_CGlxNavigableList ) :
    	public CGlxListTestBase< CGlxNavigableList, CGlxNavigableListReconstruction >
    {
    public:     // Constructors and destructors
        /**
         * Two phase construction
         */
        static ut_CGlxNavigableList* NewL();
        static ut_CGlxNavigableList* NewLC();
        /**
         * Destructor
         */
        ~ut_CGlxNavigableList();

    private:    // Constructors and destructors
        ut_CGlxNavigableList();
        void ConstructL();

    private:    // New methods
         void SetupL();
         void Teardown();
        
         void T_SetContentsLL();
         void T_ReorderContentsLL();
         void T_RemoveL();
         void T_RemoveReferenceL();
         void T_AddStaticItemLL();
         void T_SetStaticItemsEnabledL();
         void T_CountL();
         void T_FocusIndexL();
         void T_SetFocusL();
         void T_ItemL();
         void T_IndexL();
         void T_IsSelectedL();
         void T_SetSelectedLL();
         void T_SelectedItemIndicesL();
         void T_IdSpaceIdL();

        TBool TrySetFocusL( const TDesC8& aContents, TInt aInitialFocus, 
            NGlxListDefs::TFocusSetType aFocusSetType, TInt aFocusValue, TInt aExpectedFocus, 
            const TNotification::TData* aNotificationList );
            
        TBool TrySetContentsL( const TDesC8& aStringBefore, 
            const TDesC8& aStringAfter, TBool aFocusBefore, TBool aFocusAfter, 
            const TDesC8& aSelectionBefore, const TDesC8& aSelectionAfter,
            const TNotification::TData* aNotificationList );
            
        TBool TryReorderContentsL( const TDesC8& aStringBefore, 
            const TDesC8& aStringAfter, TBool aFocusBefore, TBool aFocusAfter, 
            const TDesC8& aSelectionBefore, const TDesC8& aSelectionAfter,
            const TNotification::TData* aNotificationList );

        TBool TrySetSelectedL( const TDesC8& aContents, 
            const TDesC8& aInitialSelection, const TDesC8& aFinalSelection,
            TBool aSelect, TInt aIndex, const TNotification::TData* aNotificationList );
            
        void SetSelectedL( const TDesC8& aSelection );
        TBool SelectionEquals( const TDesC8& aSelection ) const;
                    
    private:    // Data
		
        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __UT_CGLXNAVIGABLELIST_H__
