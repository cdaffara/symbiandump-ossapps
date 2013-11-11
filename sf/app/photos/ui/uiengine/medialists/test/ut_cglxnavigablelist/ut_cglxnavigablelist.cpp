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



//  CLASS HEADER
#include "ut_CGlxNavigableList.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "glxnavigablelist.h"

using namespace NGlxListDefs;

const TNotification::TData KNotificationsList_Empty[] = { { END_MARKER } };

// -----------------------------------------------------------------------------
// Constructors 
// -----------------------------------------------------------------------------
//
CGlxNavigableListReconstruction* CGlxNavigableListReconstruction::NewL( TInt aReservationCount )
    {
    CGlxNavigableListReconstruction* self = new (ELeave) CGlxNavigableListReconstruction();
    CleanupStack::PushL( self );
    self->ConstructL( aReservationCount );
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// Test if reconstruction equals original
// -----------------------------------------------------------------------------
//
TBool CGlxNavigableListReconstruction::ReconstructionEquals() const
    {
    // test items
    TBool fail = !ItemsEqual();
    // test focus
    fail |= ( iList->FocusIndex() != iFocusIndex );
    // test selection
    fail |= !SelectionEquals();

    return !fail;
    }

// -----------------------------------------------------------------------------
// Test if reconstruction of selection equals original
// -----------------------------------------------------------------------------
//
TBool CGlxNavigableListReconstruction::SelectionEquals() const
    {
    TBool fail = ( iList->SelectedItemIndices().Count() != iSelectedItemIndices.Count() );
    
    // Don't assume order in arrays is the same: make sure all items in replica
    // exist in original, and all items in original are in replica
    for ( TInt i = 0; i < iSelectedItemIndices.Count(); i++ )
        {
        // Try to find replica index from original
        fail |= ( KErrNotFound == FindInOriginalSelection( iSelectedItemIndices[ i ] ) );
        }
    
    for ( TInt i = 0; i < iList->SelectedItemIndices().Count(); i++ )
        {
        fail |= ( KErrNotFound == iSelectedItemIndices.Find( iList->SelectedItemIndices()[ i ] ) );
        }
        
    return !fail;
    }

// -----------------------------------------------------------------------------
// Return index of selected item index in original list
// -----------------------------------------------------------------------------
//
TInt CGlxNavigableListReconstruction::FindInOriginalSelection( TInt aSelectionIndex ) const
    {
    TInt i = 0;
    while ( i < iList->SelectedItemIndices().Count() )
        {
        TInt originalIndex = iList->SelectedItemIndices()[ i ];
        if ( originalIndex == aSelectionIndex )
            {
            return i; // Found it
            }
        i++;
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// Constructors & destructors
// -----------------------------------------------------------------------------
//
ut_CGlxNavigableList* ut_CGlxNavigableList::NewL()
    {
    ut_CGlxNavigableList* self = ut_CGlxNavigableList::NewLC();
    CleanupStack::Pop();

    return self;
    }

ut_CGlxNavigableList* ut_CGlxNavigableList::NewLC()
    {
    ut_CGlxNavigableList* self = new( ELeave ) ut_CGlxNavigableList();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
ut_CGlxNavigableList::~ut_CGlxNavigableList()
    {
    }

// Default constructor
ut_CGlxNavigableList::ut_CGlxNavigableList()
    {
    }

// Second phase construct
void ut_CGlxNavigableList::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// -----------------------------------------------------------------------------
// Setup & Teardown
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::SetupL(  )
    {
    }

void ut_CGlxNavigableList::Teardown(  )
    {
    DeleteMembers();
    }    

// -----------------------------------------------------------------------------
// Test FocusIndex(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_FocusIndexL(  )
    {
    CreateListL( _L8("") );
    EUNIT_ASSERT ( KErrNotFound == iList->FocusIndex() );
    CreateListL( _L8("ab") );
    EUNIT_ASSERT ( 0 == iList->FocusIndex() );
    iList->SetFocus( EAbsolute, 1 );
    EUNIT_ASSERT ( 1 == iList->FocusIndex() );
    }

// -----------------------------------------------------------------------------
// TrySetFocusL
// -----------------------------------------------------------------------------
//
TBool ut_CGlxNavigableList::TrySetFocusL( const TDesC8& aContents, 
    TInt aInitialFocus, TFocusSetType aFocusSetType, 
        TInt aFocusValue, TInt aExpectedFocus, 
            const TNotification::TData* aNotificationList )
    {
    CreateListL( aContents );
    // We need to set the focus to aInitialFocus only if it's valid
    if ( KErrNotFound != aInitialFocus )
        {
        iList->SetFocus( EAbsolute, aInitialFocus );
        }
    iReconstruction->ClearNotifications();
    // Set focus
    iList->SetFocus( aFocusSetType, aFocusValue );
    TBool fail = ( iList->FocusIndex() != aExpectedFocus );
    fail |= !iReconstruction->ReconstructionEquals();
    fail |= !iReconstruction->NotificationListEquals( aNotificationList );
    return !fail;

    }
    
// -----------------------------------------------------------------------------
// Test SetFocus(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_SetFocusL(  )
    {
        { // Test setting focus to same index
        // absolute
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 1, EAbsolute, 1,
            1, KNotificationsList_Empty ) );
        // relative
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 1, ERelative, 0,
            1, KNotificationsList_Empty ) );
        }
        
        { // Test setting focus to another index
        TNotification::TData notifications[] = 
                                { { END_MARKER } };
        // absolute
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 1, EAbsolute, 1,
            1, notifications ) );
        // relative
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 1, ERelative, 0,
            1, notifications ) );
        }
        
        { // Test setting focus to directly another index
        TNotification::TData notifications[] = 
                                { { TNotification::EFocus, 0, EUnknown }, // type, old focus, focus change type
                                  { END_MARKER } };
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 0, EAbsolute, 1,
            1, notifications ) );
        }
        
        // Test moving focus 
        { // Non-looping, 0 > 2
        TNotification::TData notifications[] = 
                                { { TNotification::EFocus, 0, EForward }, // type, old focus, focus change type
                                  { END_MARKER } };
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 0, ERelative, 2,
            2, notifications ) );
        }
        
        { // Non-looping, 1 > 4
        TNotification::TData notifications[] = 
                                { { TNotification::EFocus, 1, EForward }, // type, old focus, focus change type
                                  { END_MARKER } };
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 1, ERelative, 3,
            4, notifications ) );
        }
        
        { // Non-looping, 4 > 0
        TNotification::TData notifications[] = 
                                { { TNotification::EFocus, 4, EBackward }, // type, old focus, focus change type
                                  { END_MARKER } };
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 4, ERelative, -4,
            0, notifications ) );
        }
        
        { // Looping, 4 > 0
        TNotification::TData notifications[] = 
                                { { TNotification::EFocus, 4, EForward }, // type, old focus, focus change type
                                  { END_MARKER } };
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 4, ERelative, 1,
            0, notifications ) );
        }
        
        { // Looping, 2 > 3
        TNotification::TData notifications[] = 
                                { { TNotification::EFocus, 2, EForward }, // type, old focus, focus change type
                                  { END_MARKER } };
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 2, ERelative, 6,
            3, notifications ) );
        }
        
        { // Looping, 3 > 2
        TNotification::TData notifications[] = 
                                { { TNotification::EFocus, 3, EBackward }, // type, old focus, focus change type
                                  { END_MARKER } };
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 3, ERelative, -1,
            2, notifications ) );
        }
        
        { // Looping, 2 > 2
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 2, ERelative, 5,
            2, KNotificationsList_Empty ) );
        }
        
        { // Looping, 2 > 2
        EUNIT_ASSERT ( TrySetFocusL( _L8("abcde"), 2, ERelative, -5,
            2, KNotificationsList_Empty ) );
        }

        { // empty list 
        EUNIT_ASSERT ( TrySetFocusL( _L8(""), -1, ERelative, -5,
            -1, KNotificationsList_Empty ) );
        }

    }
    
// -----------------------------------------------------------------------------
// Test IsSelected(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_IsSelectedL(  )
    {
    CreateListL( _L8("abcde") );
    EUNIT_ASSERT( !iList->IsSelected( 0 ) );
    EUNIT_ASSERT( !iList->IsSelected( 1 ) );
    EUNIT_ASSERT( !iList->IsSelected( 2 ) );
    EUNIT_ASSERT( !iList->IsSelected( 3 ) );
    EUNIT_ASSERT( !iList->IsSelected( 4 ) );
    iList->SetSelectedL( 3, ETrue );
    iList->SetSelectedL( 0, ETrue );
    iList->SetSelectedL( 4, ETrue );
    EUNIT_ASSERT( iList->IsSelected( 0 ) );
    EUNIT_ASSERT( !iList->IsSelected( 1 ) );
    EUNIT_ASSERT( !iList->IsSelected( 2 ) );
    EUNIT_ASSERT( iList->IsSelected( 3 ) );
    EUNIT_ASSERT( iList->IsSelected( 4 ) );
    }
    
// -----------------------------------------------------------------------------
// SetSelectedL(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::SetSelectedL( const TDesC8& aSelection )
    {
    for ( TInt i = 0; i < aSelection.Length(); i++ )
        {   
        iList->SetSelectedL( i, 't' == aSelection[ i ] );
        }
    }
    
// -----------------------------------------------------------------------------
// SelectionEquals(...)
// -----------------------------------------------------------------------------
//
TBool ut_CGlxNavigableList::SelectionEquals( const TDesC8& aSelection ) const
    {
    // aSelection should have a flag for each item in the list
    TBool fail = ( iList->Count() != aSelection.Length() );
    TInt selectedItemCount = 0;
    for ( TInt i = 0; i < aSelection.Length(); i++ )
        {   
        TBool shouldBeSelected = 't' == aSelection[ i ];
        if ( shouldBeSelected )
            {
            selectedItemCount++;
            }
        // Compare via not, to make sure true is always the same value
        fail |= ( !iList->IsSelected( i ) != !shouldBeSelected );
        }
    // Make sure there are no additional items selected
    fail |= ( iList->SelectedItemIndices().Count() != selectedItemCount );
    return !fail;
    }
    
// -----------------------------------------------------------------------------
// Test SetSelectedL(...)
// -----------------------------------------------------------------------------
//
TBool ut_CGlxNavigableList::TrySetSelectedL( const TDesC8& aContents, 
    const TDesC8& aInitialSelection, const TDesC8& aFinalSelection,
        TBool aSelect, TInt aIndex, 
        const TNotification::TData* aNotificationList )
    {
    CreateListL( aContents );
    // set initial selection
    SetSelectedL( aInitialSelection );
    iReconstruction->ClearNotifications();
    // run test
    iList->SetSelectedL( aIndex, aSelect );
    
    // Check item is selected/unselected as expected
    TBool fail = EFalse;
    if ( aSelect )
        {
        fail |= !iList->IsSelected( aIndex );
        }
    else
        {
        fail |= iList->IsSelected( aIndex );
        }
    // Test selection matches
    fail |= SelectionEquals( aFinalSelection );
    // Test reconstruction matched
    fail |= !iReconstruction->ReconstructionEquals();
    // Test notification equal
    fail |= !iReconstruction->NotificationListEquals( aNotificationList );
    
    return !fail;
    }
    
// -----------------------------------------------------------------------------
// Test SetSelectedL(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_SetSelectedLL(  )
    {
        { // Select first item
        TNotification::TData notifications[] = 
                                { { TNotification::ESelection, 0, ETrue },
                                  { END_MARKER } };
        TrySetSelectedL( _L8("abcde"), _L8("fffff"), _L8("tffff"), ETrue, 0, 
            notifications );
        }
    
        { // Select another item
        TNotification::TData notifications[] = 
                                { { TNotification::ESelection, 3, ETrue }, 
                                  { END_MARKER } };
        TrySetSelectedL( _L8("abcde"), _L8("tfftf"), _L8("tfttf"), ETrue, 3,
            notifications );
        }
    
        { // Select a selected item
        TrySetSelectedL( _L8("abcde"), _L8("tfftf"), _L8("tfftf"), ETrue, 0, 
            KNotificationsList_Empty );
        }

        { // Unselect item
        TNotification::TData notifications[] = 
                                { { TNotification::ESelection, 4, EFalse },
                                  { END_MARKER } };
        TrySetSelectedL( _L8("abcde"), _L8("tfftf"), _L8("tffff"), EFalse, 4, 
            notifications );
        }
        
        { // Unselect last item
        TNotification::TData notifications[] = 
                                { { TNotification::ESelection, 0, EFalse },
                                  { END_MARKER } };
        TrySetSelectedL( _L8("abcde"), _L8("tffff"), _L8("fffff"), EFalse, 0, 
            notifications );
        }
    
        { // Unselect an unselected item
        TrySetSelectedL( _L8("abcde"), _L8("tftff"), _L8("tftff"), EFalse, 4, 
            KNotificationsList_Empty );
        }
        
        { // Try to select static items
        // Add a static item at front, and two at rear
        NGlxListTestHelpers::TStaticItemInfo::TData staticItemInfos[] = 
            { { '0', EInsertFirst },
              { '1', EInsertLast },
              { '2', EInsertLast },
              { K_STATIC_ITEMS_END } };

        CreateListL( _L8("abcde"), ETrue, staticItemInfos );
        iList->SetSelectedL( 0, ETrue ); // static item
        iList->SetSelectedL( 1, ETrue ); // normal item
        iList->SetSelectedL( 6, ETrue ); // static item
        iList->SetSelectedL( 7, ETrue ); // static item
        TNotification::TData notifications[] = 
                                { { TNotification::ESelection, 1, ETrue },
                                  { END_MARKER } };
        // Test selection matches
        TBool fail = !SelectionEquals( _L8("ftffffff") );
        // Test reconstruction matched
        fail |= !iReconstruction->ReconstructionEquals();
        // Test notification equal
        fail |= !iReconstruction->NotificationListEquals( notifications);
        EUNIT_ASSERT( !fail );
        }
    }
      
// -----------------------------------------------------------------------------
// Test SelectedItemIndices(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_SelectedItemIndicesL(  )
    {
    CreateListL( _L8("abcde") );
    EUNIT_ASSERT( iList->SelectedItemIndices().Count() == 0 );
    iList->SetSelectedL( 3, ETrue );
    iList->SetSelectedL( 0, ETrue );
    iList->SetSelectedL( 4, ETrue );
    EUNIT_ASSERT( iList->SelectedItemIndices().Count() == 3 );
    // This test code assumes indexes are ordered, but this is not a requirement 
    // for navigable list. So if ordering navigable list is modified, this may 
    // need changing also
    EUNIT_ASSERT( iList->SelectedItemIndices()[ 0 ] == 0 );
    EUNIT_ASSERT( iList->SelectedItemIndices()[ 1 ] == 3 );
    EUNIT_ASSERT( iList->SelectedItemIndices()[ 2 ] == 4 );
    }
    
// -----------------------------------------------------------------------------
// TrySetContentsL(...)
// -----------------------------------------------------------------------------
//
TBool ut_CGlxNavigableList::TrySetContentsL( const TDesC8& aStringBefore, 
    const TDesC8& aStringAfter, TBool aFocusBefore, TBool aFocusAfter, 
        const TDesC8& aSelectionBefore, const TDesC8& aSelectionAfter,
            const TNotification::TData* aNotificationList )
    {
    CreateListL( aStringBefore );
    SetSelectedL( aSelectionBefore );
    if ( KErrNotFound != aFocusBefore )
        {
        iList->SetFocus( EAbsolute, aFocusBefore );
        }
    iReconstruction->ClearNotifications();
   
    // Run function under test
    SetContentsL( aStringAfter );
    
    // Test reconstruction matched
    TBool fail = !Equals( aStringAfter );
    // Test focus matches
    fail |= ( iList->FocusIndex() != aFocusAfter );
    // Test selection matches
    fail |= !SelectionEquals( aSelectionAfter );
    // Test notification equal
    fail |= !iReconstruction->NotificationListEquals( aNotificationList );
    
    return !fail;
    }

// -----------------------------------------------------------------------------
// Test SetContentsL(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_SetContentsLL(  )
    {
    // Set no contents to empty list
        {
        EUNIT_ASSERT( TrySetContentsL( _L8(""), _L8(""), KErrNotFound, KErrNotFound,
            _L8(""), _L8(""), KNotificationsList_Empty ) );
        }
        
    // Set contents to empty list
        {
        TNotification::TData notifications[] = 
                                { { TNotification::EAdd, 0, 5 },
                                  { TNotification::EFocus, KErrNotFound, EUnknown },
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8(""), _L8("abcde"), KErrNotFound, 0,
            _L8(""), _L8("fffff"), notifications ) );
        }

    // Set contents so that focus is before insertion point
        {
        TNotification::TData notifications[] = 
                                { { TNotification::EAdd, 3, 4 },
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8("abc0123de"), 2, 2,
            _L8("tfttf"), _L8("tftfffftf"), notifications ) );
        }
    // Set contents so that focus is after insertion point
        {
        TNotification::TData notifications[] = 
                                { { TNotification::EAdd, 2, 4 },
                                  { TNotification::EFocus, 2, EUnknown }, 
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8("ab0123cde"), 2, 6,
            _L8("ftftf"), _L8("ftffffftf"), notifications ) );
        }
    // Set contents so that focus is before removed items
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 2, 2 },
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8("abe"), 1, 1,
            _L8("ftttf"), _L8("ftf"), notifications ) );

        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8("abe"), 0, 0,
            _L8("ttttt"), _L8("ttt"), notifications ) );
        }
    // Set contents so that focus is after removed items
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 2, 2 },
                                  { TNotification::EFocus, 4, EUnknown }, 
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8("abe"), 4, 2, // focus on e
            _L8("ffttf"), _L8("fff"), notifications ) );
        }
    // Set contents so that focus is on removed items, item before exists
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 2, 2 },
                                  { TNotification::EFocus, 3, EUnknown }, 
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8("abe"), 3, 1, // Focus from d to b
            _L8("fffff"), _L8("fff"), notifications ) );
        }
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 2, 2 },
                                  { TNotification::EFocus, 2, EUnknown }, 
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8("abe"), 2, 1, // Focus from c to b
            _L8("fffff"), _L8("fff"), notifications ) );
        }
    // Set contents so that focus is on removed items, item before does not exist
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 2 },
                                  { TNotification::EFocus, 0, EUnknown }, 
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8("cde"), 0, 0, // Focus from a to c
            _L8("fffff"), _L8("fff"), notifications ) );
        }
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 2 },
                                  { TNotification::EFocus, 1, EUnknown }, 
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8("cde"), 1, 0, // Focus from b to c
            _L8("fffff"), _L8("fff"), notifications ) );
        }
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 2 },
                                  { TNotification::EFocus, 2, EUnknown }, 
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8("cde"), 2, 0, // Focus on c
            _L8("fffff"), _L8("fff"), notifications ) );
        }
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 2 },
                                  { TNotification::EFocus, 3, EUnknown }, 
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8("cde"), 3, 1, // Focus on d
            _L8("fffff"), _L8("fff"), notifications ) );
        }
    // Set contents to empty
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 5 },
                                  { TNotification::EFocus, 1, EUnknown }, 
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8(""), 1, KErrNotFound, // focus initially on b
            _L8("fffff"), _L8(""), notifications ) );
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8(""), 1, KErrNotFound, // focus initially on b
            _L8("ttttt"), _L8(""), notifications ) );
        }
    // Set contents so items get replaced
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 2, 2 },
                                  { TNotification::EFocus, 3, EUnknown }, 
                                  { TNotification::EAdd, 2, 4 },
                                  { END_MARKER } };
        EUNIT_ASSERT( TrySetContentsL( _L8("abcde"), _L8("ab0123e"), 3, 1, // focus d to b
            _L8("fttft"), _L8("ftfffft"), notifications ) );
        }
    // Set contents so that some selected items stay, some move and some are removed
    }
    
// -----------------------------------------------------------------------------
// TryReorderContentsL(...)
// -----------------------------------------------------------------------------
//
TBool ut_CGlxNavigableList::TryReorderContentsL( const TDesC8& aStringBefore, 
    const TDesC8& aStringAfter, TBool aFocusBefore, TBool aFocusAfter, 
        const TDesC8& aSelectionBefore, const TDesC8& aSelectionAfter,
            const TNotification::TData* aNotificationList )
    {
    CreateListL( aStringBefore );
    SetSelectedL( aSelectionBefore );
    if ( KErrNotFound != aFocusBefore )
        {
        iList->SetFocus( EAbsolute, aFocusBefore );
        }
    iReconstruction->ClearNotifications();
   
    // Run function under test
    ReorderContentsL( aStringAfter );
    
    // Test reconstruction matched
    TBool fail = !Equals( aStringAfter );
    // Test focus matches
    fail |= ( iList->FocusIndex() != aFocusAfter );
    // Test selection matches
    fail |= !SelectionEquals( aSelectionAfter );
    // Test notification equal
    fail |= !iReconstruction->NotificationListEquals( aNotificationList );
    
    return !fail;
    }

// -----------------------------------------------------------------------------
// Test ReorderContentsL(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_ReorderContentsLL(  )
    {
    // Reorder no contents from empty list
        {
        EUNIT_ASSERT( TryReorderContentsL( _L8(""), _L8(""), KErrNotFound, KErrNotFound,
            _L8(""), _L8(""), KNotificationsList_Empty ) );
        }
        
    // Reorder contents from empty list
        {
        TNotification::TData notifications[] = 
                                { { TNotification::EAdd, 0, 5 },
                                  { TNotification::EFocus, KErrNotFound, EUnknown },
                                  { END_MARKER } };
        EUNIT_ASSERT( TryReorderContentsL( _L8(""), _L8("abcde"), KErrNotFound, 0,
            _L8(""), _L8("fffff"), notifications ) );
        }

    // Reorder contents so that focus is unchanged
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 5 },
                                  { TNotification::EFocus, 0, EUnknown },
                                  { TNotification::EAdd, 0, 5 },
                                  { TNotification::EFocus, KErrNotFound, EUnknown },
                                  { END_MARKER } };
        EUNIT_ASSERT( TryReorderContentsL( _L8("abcde"), _L8("abcde"), 0, 0,
            _L8("fffff"), _L8("fffff"), notifications ) );
        }

    // Reorder contents so that focus is changed
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 5 },
                                  { TNotification::EFocus, 0, EUnknown },
                                  { TNotification::EAdd, 0, 5 },
                                  { TNotification::EFocus, KErrNotFound, EUnknown },
                                  { TNotification::EFocus, 0, EUnknown },
                                  { END_MARKER } };
        EUNIT_ASSERT( TryReorderContentsL( _L8("abcde"), _L8("bcdea"), 0, 4,
            _L8("fffff"), _L8("fffff"), notifications ) );
        }

    // Reorder contents so that focus is lost
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 5 },
                                  { TNotification::EFocus, 0, EUnknown },
                                  { TNotification::EAdd, 0, 4 },
                                  { TNotification::EFocus, KErrNotFound, EUnknown },
                                  { END_MARKER } };
        EUNIT_ASSERT( TryReorderContentsL( _L8("abcde"), _L8("bcde"), 0, 0,
            _L8("fffff"), _L8("ffff"), notifications ) );
        }

    // Reorder contents so that selection is unchanged
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 5 },
                                  { TNotification::EFocus, 0, EUnknown },
                                  { TNotification::EAdd, 0, 5 },
                                  { TNotification::EFocus, KErrNotFound, EUnknown },
                                  { TNotification::ESelection, 0, ETrue },
                                  { END_MARKER } };
        EUNIT_ASSERT( TryReorderContentsL( _L8("abcde"), _L8("abcde"), 0, 0,
            _L8("tffff"), _L8("tffff"), notifications ) );
        }

    // Reorder contents so that selection is changed
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 5 },
                                  { TNotification::EFocus, 0, EUnknown },
                                  { TNotification::EAdd, 0, 5 },
                                  { TNotification::EFocus, KErrNotFound, EUnknown },
                                  { TNotification::EFocus, 0, EUnknown },
                                  { TNotification::ESelection, 4, ETrue },
                                  { TNotification::ESelection, 1, ETrue },
                                  { TNotification::ESelection, 3, ETrue },
                                  { END_MARKER } };
        EUNIT_ASSERT( TryReorderContentsL( _L8("abcde"), _L8("bcdea"), 0, 4,
            _L8("tftft"), _L8("ftftt"), notifications ) );
        }

    // Reorder contents so that selection is lost
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 5 },
                                  { TNotification::EFocus, 0, EUnknown },
                                  { TNotification::EAdd, 0, 5 },
                                  { TNotification::EFocus, KErrNotFound, EUnknown },
                                  { TNotification::ESelection, 0, ETrue },
                                  { TNotification::ESelection, 2, ETrue },
                                  { END_MARKER } };
        EUNIT_ASSERT( TryReorderContentsL( _L8("abcde"), _L8("abcd0"), 0, 0,
            _L8("tftft"), _L8("tftff"), notifications ) );
        }
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 5 },
                                  { TNotification::EFocus, 0, EUnknown },
                                  { TNotification::EAdd, 0, 5 },
                                  { TNotification::EFocus, KErrNotFound, EUnknown },
                                  { END_MARKER } };
        EUNIT_ASSERT( TryReorderContentsL( _L8("abcde"), _L8("a0123"), 0, 0,
            _L8("ftttt"), _L8("fffff"), notifications ) );
        }

    // Reorder contents to empty
        {
        TNotification::TData notifications[] = 
                                { { TNotification::ERemove, 0, 5 },
                                  { TNotification::EFocus, 0, EUnknown }, 
                                  { END_MARKER } };
        EUNIT_ASSERT( TryReorderContentsL( _L8("abcde"), _L8(""), 0, KErrNotFound,
            _L8("ttttt"), _L8(""), notifications ) );
        }
    }

// -----------------------------------------------------------------------------
// Test RemoveReference(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_RemoveReferenceL(  )
    {
    TestRemoveReferenceL();
    }

// -----------------------------------------------------------------------------
// Test Remove(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_RemoveL(  )
    {
    }
    
// -----------------------------------------------------------------------------
// Test AddStaticItemL(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_AddStaticItemLL(  )
    {
    }
    
// -----------------------------------------------------------------------------
// Test SetStaticItemsEnabled(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_SetStaticItemsEnabledL(  )
    {
    }
    
// -----------------------------------------------------------------------------
// Test Count(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_CountL(  )
    {
    TestCountL();
    }
    
// -----------------------------------------------------------------------------
// Test Item(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_ItemL(  )
    {
    TestItemL();
    }
    
// -----------------------------------------------------------------------------
// Test Index(...)
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_IndexL(  )
    {
    TestIndexL();
    }
    
// -----------------------------------------------------------------------------
// Test IdSpaceId
// -----------------------------------------------------------------------------
//
void ut_CGlxNavigableList::T_IdSpaceIdL()
    {
    TestIdSpaceIdL();
    }    

// -----------------------------------------------------------------------------
// Test table
// -----------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    ut_CGlxNavigableList,
    "Add test suite description here.",
    "UNIT" )

EUNIT_TEST(
    "SetContentsL",
    "CGlxNavigableList",
    "SetContentsL",
    "FUNCTIONALITY",
    SetupL, T_SetContentsLL, Teardown)
    
EUNIT_TEST(
    "ReorderContentsL",
    "CGlxNavigableList",
    "ReorderContentsL",
    "FUNCTIONALITY",
    SetupL, T_ReorderContentsLL, Teardown)

EUNIT_TEST(
    "Remove",
    "CGlxNavigableList",
    "Remove",
    "FUNCTIONALITY",
    SetupL, T_RemoveL, Teardown)
    
EUNIT_TEST(
    "RemoveReference",
    "CGlxNavigableList",
    "RemoveReference",
    "FUNCTIONALITY",
    SetupL, T_RemoveReferenceL, Teardown)

EUNIT_TEST(
    "AddStaticItemL",
    "CGlxNavigableList",
    "AddStaticItemL",
    "FUNCTIONALITY",
    SetupL, T_AddStaticItemLL, Teardown)
    
EUNIT_TEST(
    "SetStaticItemsEnabled",
    "CGlxNavigableList",
    "SetStaticItemsEnabled",
    "FUNCTIONALITY",
    SetupL, T_SetStaticItemsEnabledL, Teardown)
    
EUNIT_TEST(
    "Count",
    "CGlxNavigableList",
    "Count",
    "FUNCTIONALITY",
    SetupL, T_CountL, Teardown)
    
EUNIT_TEST(
    "FocusIndex",
    "CGlxNavigableList",
    "FocusIndex",
    "FUNCTIONALITY",
    SetupL, T_FocusIndexL, Teardown)
    
EUNIT_TEST(
    "SetFocus",
    "CGlxNavigableList",
    "SetFocus",
    "FUNCTIONALITY",
    SetupL, T_SetFocusL, Teardown)
    
EUNIT_TEST(
    "Item",
    "CGlxNavigableList",
    "Item",
    "FUNCTIONALITY",
    SetupL, T_ItemL, Teardown)
    
EUNIT_TEST(
    "Index",
    "CGlxNavigableList",
    "Index",
    "FUNCTIONALITY",
    SetupL, T_IndexL, Teardown)
    
EUNIT_TEST(
    "IsSelected",
    "CGlxNavigableList",
    "IsSelected",
    "FUNCTIONALITY",
    SetupL, T_IsSelectedL, Teardown)
    
EUNIT_TEST(
    "SetSelectedL",
    "CGlxNavigableList",
    "SetSelectedL",
    "FUNCTIONALITY",
    SetupL, T_SetSelectedLL, Teardown)
    
EUNIT_TEST(
    "SelectedItemIndices",
    "CGlxNavigableList",
    "SelectedItemIndices",
    "FUNCTIONALITY",
    SetupL, T_SelectedItemIndicesL, Teardown)    
    
EUNIT_TEST(
    "IdSpaceId",
    "CGlxNavigableList",
    "IdSpaceId",
    "FUNCTIONALITY",
    SetupL, T_IdSpaceIdL, Teardown)    

EUNIT_END_TEST_TABLE

//  END OF FILE
