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
* Description:    List of media items, which has focus
*
*/




// my include
#include "glxnavigablelist.h"

// system includes
#include <mpxcollectionpath.h>
#include <glxtracer.h>
#include <glxlog.h>

// user includes
#include "glxlistutils.h"
#include "glxmedia.h"
#include "glxstaticitemlist.h"
#include "mglxnavigablelistobserver.h"

using namespace NGlxNavigableList;

namespace NGlxNavigableList
    {
    
    /** 
     * Strategy to process selection indexes
     */ 
    class MSelectionIndexStrategy 
        {
    public:
        /** 
         * Process a single selection index
         * @param aSelectedItemIndex index in selected item index array to process
         */ 
        virtual void Process( TInt aSelectedItemIndex ) = 0;
        };
        
    /** 
     * Strategy to remove selection indexes
     */ 
    NONSHARABLE_CLASS( TRemoveSelectedItemIndexStrategy ) : 
            public MSelectionIndexStrategy
        {
    public:
        /**
         * Constructor
         * @param aSelectedItemIndices Selected item index array
         */
        TRemoveSelectedItemIndexStrategy( RArray< TInt>& aSelectedItemIndices )
                : iSelectedItemIndices( aSelectedItemIndices )
            {
            }

        /** See @ref MSelectionIndexStrategy */
        void Process( TInt aSelectedItemIndex )
            {
            iSelectedItemIndices.Remove( aSelectedItemIndex );
            }

    private:
        /// Selected item indexes
        RArray< TInt>& iSelectedItemIndices; 
        };

    /** 
     * Strategy to move selection indexes
     */ 
    NONSHARABLE_CLASS( TMoveSelectedItemIndexStrategy ) : 
            public MSelectionIndexStrategy
        {
    public:
        /** 
         * Constructor 
         * @param aMoveCount a number of items to move index by
         */
        TMoveSelectedItemIndexStrategy( RArray< TInt>& aSelectedItemIndices, 
            TInt aMoveCount )
                : iSelectedItemIndices( aSelectedItemIndices )
            {
            iMoveCount = aMoveCount;
            }
            
        /** See @ref MSelectionIndexStrategy */
        void Process( TInt aSelectedItemIndex )
            {
            iSelectedItemIndices[ aSelectedItemIndex ] += iMoveCount;
            }
        
    private: 
        /// The count of items to move by
        TInt iMoveCount;
        /// Selected item indexes
        RArray< TInt>& iSelectedItemIndices; 
        };
    
    } // namespace NGlxNavigableList
    
// -----------------------------------------------------------------------------
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CGlxNavigableList* CGlxNavigableList::NewL( const TGlxIdSpaceId& aIdSpaceId,
        MGlxNavigableListObserver& aObserver, MGlxMediaUser& aMediaUser )
    {
    TRACER("CGlxNavigableList::NewL");
            
    CGlxNavigableList* self = new (ELeave) CGlxNavigableList( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aIdSpaceId, aMediaUser );
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxNavigableList::CGlxNavigableList( MGlxNavigableListObserver& aObserver ) :
        iFocusInitialPosition( NGlxListDefs::EFocusFirst ),
        iObserver( aObserver )
    {
    TRACER("CGlxNavigableList::CGlxNavigableList");
    
    iFocusIndex = KErrNotFound;
    }

// -----------------------------------------------------------------------------
// Second-phase constructor
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::ConstructL( const TGlxIdSpaceId& aIdSpaceId,
        MGlxMediaUser& aMediaUser )
    {
    TRACER("CGlxNavigableList::ConstructL");
    
    iItemList = CGlxStaticItemList::NewL( aIdSpaceId, *this, aMediaUser );
    

    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxNavigableList::~CGlxNavigableList()
    {
    TRACER( "CGlxNavigableList::~CGlxNavigableList" );
    iSelectedItemIndices.Close();
    delete iItemList;

    
    }

// -----------------------------------------------------------------------------
// return id space id
// -----------------------------------------------------------------------------
// 
const TGlxIdSpaceId& CGlxNavigableList::IdSpaceId() const
    {
    TRACER( "CGlxNavigableList::IdSpaceId");
    
    return iItemList->IdSpaceId();
    }

// -----------------------------------------------------------------------------
// Synchronise contents of the list with the collection path
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::SetContentsL( const CMPXCollectionPath& aPath,
        const MGlxMediaPool& aMediaPool )
    {
    TRACER( "CGlxNavigableList::SetContentsL" );
    iItemList->SetContentsL( aPath, aMediaPool );
    }

// -----------------------------------------------------------------------------
// Re-orders contents of the list with the collection path
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::ReorderContentsL( const CMPXCollectionPath& aPath, 
        const MGlxMediaPool& aMediaPool )
    {
    TRACER( "CGlxNavigableList::ReorderContentsL" );
    __TEST_INVARIANT;

    // List contents have been reordered. If we called SetContentsL directly,
    // the client would get semi-random (but correct) sequence of added/removed
    // notifications. To provide a cleaner notification, send only "all removed"
    // and "all added" notifications. 
    
    // store the focus and selection temporarily for restoring later-
    // (store ids; cannot store index, since indexes may change upon reorder)
    TGlxMediaId focusedItemId = FocusId();

    RArray< TGlxMediaId > selectedItemIds;
    CleanupClosePushL( selectedItemIds );
    SelectionL( selectedItemIds );

    // send notification "all items removed"
    ClearContentsL( aMediaPool );

    // set contents to reordered list, this removes focus and selection.
    // send notification of "all items added" (in new order)
    SetContentsL( aPath, aMediaPool );

    // restore focus and selection
    SetFocus( focusedItemId ); 
    SelectL( selectedItemIds );

    CleanupStack::PopAndDestroy( &selectedItemIds );

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// Remove an item form the list
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::Remove( const TGlxIdSpaceId& aIdSpaceId, 
        const TGlxMediaId& aItemId )
    {
    TRACER( "CGlxNavigableList::Remove" );
    iItemList->Remove( aIdSpaceId, aItemId );
    }

// -----------------------------------------------------------------------------
// Remove any pointers to the media object at the specified index
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::RemoveReference( TInt aIndex )
    {
    TRACER( "CGlxNavigableList::RemoveReference" );
    iItemList->RemoveReference( aIndex );
    }
    
// -----------------------------------------------------------------------------
// Add a static item
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::AddStaticItemL( CGlxMedia* aStaticItem, 
        NGlxListDefs::TInsertionPosition aTargetPosition )
    {
    TRACER( "CGlxNavigableList::AddStaticItemL" );
    iItemList->AddStaticItemL( aStaticItem, aTargetPosition );
    }
        
// -----------------------------------------------------------------------------
// Enable/disable static items
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::SetStaticItemsEnabled( TBool aEnabled )
    {
    TRACER( "CGlxNavigableList::SetStaticItemsEnabled" );
    iItemList->SetStaticItemsEnabled( aEnabled );
    }
    
// -----------------------------------------------------------------------------
// return ETrue if static items are enabled
// -----------------------------------------------------------------------------
//
TBool CGlxNavigableList::IsStaticItemsEnabled() const
    {
    TRACER( "CGlxNavigableList::IsStaticItemsEnabled");
    
    return iItemList->IsStaticItemsEnabled();
    }
    
// -----------------------------------------------------------------------------
// Return count
// -----------------------------------------------------------------------------
//
TInt CGlxNavigableList::Count( NGlxListDefs::TCountType aType ) const
    {
    TRACER( "CGlxNavigableList::Count");
    
    return iItemList->Count( aType );
    }

// -----------------------------------------------------------------------------
// Sets the initial focus position, first or last item
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::SetFocusInitialPosition(NGlxListDefs::TFocusInitialPosition aFocusInitialPosition)
    {
    TRACER( "CGlxNavigableList::SetFocusInitialPosition");
    
    iFocusInitialPosition = aFocusInitialPosition;
    SetInitialFocus();
    }
    
// -----------------------------------------------------------------------------
// Return focus index
// -----------------------------------------------------------------------------
//
TInt CGlxNavigableList::FocusIndex() const 
    {
    TRACER( "CGlxNavigableList::FocusIndex");
    
    return iFocusIndex;
    }

// -----------------------------------------------------------------------------
// Set focus index
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::SetFocus( NGlxListDefs::TFocusSetType aType, TInt aValue )
    {
    TRACER( "CGlxNavigableList::SetFocus");
    GLX_LOG_INFO1( "CGlxNavigableList::SetFocus: Entry: %d", aValue );
    
    __TEST_INVARIANT;

    // Observer callback needs to know the focus change type
    NGlxListDefs::TFocusChangeType type = NGlxListDefs::EUnknown;
    
    TInt oldIndex = iFocusIndex;

    switch ( aType )
        {
        case NGlxListDefs::EAbsolute:
            type = SetFocus( aValue );
            break;
            
        case NGlxListDefs::ERelative:
            type = MoveFocus( aValue );
            break;
            
        default:
            Panic( EGlxPanicIllegalArgument ); // Unsupported focus change type
            break;
        }



    // notify observer if focus changed
    NotifyFocusChange( type, oldIndex,  iFocusIndex != oldIndex );

    __TEST_INVARIANT;
    GLX_LOG_INFO( "CGlxNavigableList::SetFocusL: Exit" );
    }

// -----------------------------------------------------------------------------
// Set focus by id
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxNavigableList::SetFocus( const TGlxMediaId& aItemId )
    {
    TRACER( "CGlxNavigableList::SetFocus");
    
    TInt index = Index( IdSpaceId(), aItemId );
    if ( KErrNotFound != index )
        {
        // Focus the item if it still exists in the new list
        SetFocus( NGlxListDefs::EAbsolute, index );
        }
    }

// -----------------------------------------------------------------------------
// Set focus index
// -----------------------------------------------------------------------------
//
NGlxListDefs::TFocusChangeType CGlxNavigableList::SetFocus( TInt aIndex )
    {
    TRACER( "CGlxNavigableList::SetFocus");
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < Count(), Panic( EGlxPanicIllegalArgument ) );

    iFocusIndex = aIndex;

    // it is not possible to know which direction focus was moving, since
    // client did not specify it. Looping of the list makes it impossible to 
    // compare the old and new index. 
    return NGlxListDefs::EUnknown;
    }

// -----------------------------------------------------------------------------
// Move focus index
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline NGlxListDefs::TFocusChangeType CGlxNavigableList::MoveFocus( TInt aDelta )
    {
    TRACER( "CGlxNavigableList::MoveFocus");
    
    // Moving focus in not possible when the list is empty
    if ( KErrNotFound != iFocusIndex )
        {
        // Move focus index
        iFocusIndex += aDelta;
        
        // wrap back into the loop
        iFocusIndex = GlxListUtils::NormalizedIndex( iFocusIndex, Count() );

        // determine direction of focus change
        if ( aDelta > 0 )
            {
            return NGlxListDefs::EForward;
            }
        // client won't be notified if aDelta is 0, so it is suitable
        // to return EBackward, even if focus does not move
        return NGlxListDefs::EBackward;
        }
        
    return NGlxListDefs::EUnknown;
    }

// -----------------------------------------------------------------------------
// Notify observers of focus change
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::NotifyFocusChange( NGlxListDefs::TFocusChangeType aType, 
        TInt aOldIndex, TBool aNotify )
    {
    TRACER( "CGlxNavigableList::NotifyFocusChange");
    
    if ( aNotify )
        {
        iObserver.HandleFocusChanged( aType, iFocusIndex, aOldIndex );
        }
    }
    
// -----------------------------------------------------------------------------
// Sets the initial focus
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::SetInitialFocus()
    {
    TRACER( "CGlxNavigableList::SetInitialFocus");
    
    // Only need to set focus if there are some items
    if ( Count() )
        {
        if ( iFocusInitialPosition == NGlxListDefs::EFocusFirst )
            {
            iFocusIndex = 0;
            }
        else // iFocusInitialPosition == NGlxListDefs::EFocusLast
            {
            iFocusIndex = Count() - 1;
            }
        }
    }

// -----------------------------------------------------------------------------
// Resets the focus to the initial position
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::ResetFocus()
    {
    TRACER( "CGlxNavigableList::ResetFocus");
    
    TInt oldFocusIndex = iFocusIndex;

    SetInitialFocus();

    // notify of focus change after resetting focus
    // Only need to notify if there are some items
    if( Count() )
    	{
    NotifyFocusChange( NGlxListDefs::EUnknown, oldFocusIndex, ETrue );
    	}
    }

// -----------------------------------------------------------------------------
// Return item by index
// -----------------------------------------------------------------------------
//
TGlxMedia& CGlxNavigableList::Item( TInt aIndex )
    {
    TRACER( "CGlxNavigableList::Item");
    
    return iItemList->Item( aIndex );
    }

// -----------------------------------------------------------------------------
// Return index by id
// -----------------------------------------------------------------------------
//
TInt CGlxNavigableList::Index(const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aId ) const
    {
    TRACER( "CGlxNavigableList::Index");
    
    return iItemList->Index(aIdSpaceId, aId );
    }

// -----------------------------------------------------------------------------
// Return whether item at index is selected
// -----------------------------------------------------------------------------
//
TBool CGlxNavigableList::IsSelected(TInt aIndex) const
    {
    TRACER( "CGlxNavigableList::IsSelected");
    
    return ( KErrNotFound != iSelectedItemIndices.FindInOrder( aIndex ) );
    }

// -----------------------------------------------------------------------------
// Select/deselect item
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::SetSelectedL( TInt aIndex, TBool aSelected )
    {
    TRACER( "CGlxNavigableList::SetSelectedL");    
    GLX_LOG_INFO2( "CGlxNavigableList::SetSelectedL: index %d selected %b", 
            aIndex, aSelected );
    __TEST_INVARIANT;
    
    if ( aSelected )
        {
        SelectL( aIndex );
        }
    else
        {
        Deselect( aIndex );
        }

    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// Return id of focused item or KGlxIdNone
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline TGlxMediaId CGlxNavigableList::FocusId() const
    {
    TRACER( "CGlxNavigableList::FocusId");
    
    if ( KErrNotFound != iFocusIndex )
        {
        return iItemList->Item( iFocusIndex ).Id();
        }
    return KGlxIdNone;
    }

// -----------------------------------------------------------------------------
// Return id of focused item or KGlxIdNone
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxNavigableList::SelectionL( RArray< TGlxMediaId >& aItemIds ) const
    {
    TRACER( "CGlxNavigableList::SelectionL");
    
    CleanupClosePushL(aItemIds);
    aItemIds.Reset();

    // Reserve full required space to avoid reallocations during loop
    aItemIds.ReserveL( iSelectedItemIndices.Count() );

    TInt count = iSelectedItemIndices.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        aItemIds.AppendL( iItemList->Item( iSelectedItemIndices[ i ] ).Id() );
        }
    CleanupStack::Pop(&aItemIds);
    }

// -----------------------------------------------------------------------------
// Clear list contents
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxNavigableList::ClearContentsL( const MGlxMediaPool& aMediaPool )
    {
    TRACER( "CGlxNavigableList::ClearContentsL");
    
    // Create a path with no items in order to empty the list
    CMPXCollectionPath* emptyPath = CMPXCollectionPath::NewL();
    CleanupStack::PushL( emptyPath );

    iItemList->SetContentsL( *emptyPath, aMediaPool );

    CleanupStack::PopAndDestroy( emptyPath );
    }

// -----------------------------------------------------------------------------
// Select items by id
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxNavigableList::SelectL( const RArray< TGlxMediaId >& aItemIds ) 
    {
    TRACER( "CGlxNavigableList::SelectL");
    
    // Reserve free space for the full selection, so that the operation can
    // be atomic
    ReserveFreeSpaceInSelectionL( aItemIds.Count() );

    // pick id space id locally to avoid re-retrieving during the loop
    TGlxIdSpaceId idSpaceId = IdSpaceId();

    // Select the requested items     
    TInt count = aItemIds.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TInt index = Index( idSpaceId, aItemIds[ i ] );
        if ( index != KErrNotFound )
            {
            Select( index );
            }
        }    
    }
    
// -----------------------------------------------------------------------------
// Select item
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxNavigableList::SelectL( TInt aIndex )
    {
    TRACER( "CGlxNavigableList::SelectL");
    
    // Don't allow static items to be selected
    if ( !Item( aIndex ).IsStatic() )
        {
        ReserveFreeSpaceInSelectionL( 1 );
        Select( aIndex );
        }
    }

// -----------------------------------------------------------------------------
// Select item. Must call ReserveFreeSpaceInSelectionL before
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::Select( TInt aIndex )
    {
    TRACER( "CGlxNavigableList::Select");
    
    __ASSERT_DEBUG( _iSelectionReserveCount > 0, Panic( EGlxPanicNoReservation ) ); // No reservation made
    __DEBUG_ONLY( _iSelectionReserveCount-- );
    
    // returns KErrNone if inserted successfully, KErrAlreadyExists if already exists
    if ( KErrNone == iSelectedItemIndices.InsertInOrder( aIndex ) )
        {
        iObserver.HandleItemSelected( aIndex, ETrue );
        }
    }

// -----------------------------------------------------------------------------
// Reserve free space is selected item indexes array
// -----------------------------------------------------------------------------
//
inline void CGlxNavigableList::ReserveFreeSpaceInSelectionL( TInt aCount )
    {
    TRACER( "CGlxNavigableList::ReserveFreeSpaceInSelectionL");
    
    iSelectedItemIndices.ReserveL( iSelectedItemIndices.Count() + aCount );
    
    // store reservation count so can protect agains Select(...) being called
    // without reservation
    __DEBUG_ONLY( _iSelectionReserveCount = aCount );
    }
        
// -----------------------------------------------------------------------------
// Deselect item
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxNavigableList::Deselect( TInt aIndex )
    {
    TRACER( "CGlxNavigableList::Deselect");
    
    // item has been deselected, remove if found
    TInt selectionArrayIndex = iSelectedItemIndices.FindInOrder( aIndex );
    if ( KErrNotFound != selectionArrayIndex )
        {
        // remove from array of selected items
        iSelectedItemIndices.Remove( selectionArrayIndex );
        
        // free unused memory from selected item indexes array
        // (this call may be a performance bottleneck if executing unmark all
        //  in a list of many thousands of items when most of them are marked.
        //  if it is proved to be a performance bottleneck, it may be sensible
        //  to count how many times Deselect has been called, an only
        //  compress every x times.)
        //iSelectedItemIndices.Compress();
        
        // selection has changed, notify observer
        iObserver.HandleItemSelected( aIndex, EFalse );
        }
    // else: ignore if item was not selected
    }

// -----------------------------------------------------------------------------
// Return selected items
// -----------------------------------------------------------------------------
//
const TArray< TInt > CGlxNavigableList::SelectedItemIndices() const
    {
    TRACER( "CGlxNavigableList::SelectedItemIndices");
    
    return iSelectedItemIndices.Array();
    }

// -----------------------------------------------------------------------------
// Handle items being added
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::HandleItemsAdded( TInt aFirstInsertedIndex, 
        TInt aCount )
    {
    TRACER( "CGlxNavigableList::HandleItemsAdded" );

    // Do not test invariant in the beginning of this function. Right now,
    // the state is not valid, since iItemList's state has already changed, and
    // this function will align the state of this object with iItemList.

    // Move selection indexes from insertion point 
    TMoveSelectedItemIndexStrategy moveStrategy( iSelectedItemIndices, aCount );
    ProcessSelectionItemIndicesBetweenIndexes( aFirstInsertedIndex, 
        Count( NGlxListDefs::ECountAll ), moveStrategy );

    // move focus
    TInt oldFocusIndex = iFocusIndex;
    TInt notifyOfFocusChange = UpdateFocusAfterInsertion( aFirstInsertedIndex, aCount );

    // Adjust index by amount of static items
    iObserver.HandleItemsAdded( aFirstInsertedIndex, aCount );
    
    // notify of focus change after having notified of items being added,
    // so that focus index points to the correct real item on client side
    NotifyFocusChange( NGlxListDefs::EUnknown, oldFocusIndex, 
            notifyOfFocusChange );
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// Move focus after items have been inserted
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline TInt CGlxNavigableList::UpdateFocusAfterInsertion( TInt aInsertionIndex, 
        TInt aInsertionCount )  
    {
     TRACER( "CGlxNavigableList::UpdateFocusAfterInsertion" );
     
    // Move focus if index is changed.
    TBool notifyObserver = MoveFocusIfIndexChanged( aInsertionIndex, aInsertionCount );
        
    // Set focus to initial focus position if not previously set. Test also that count larger 
    // than zero, just to be safe for future maintenance of other classes
    if ( KErrNotFound == iFocusIndex && Count( NGlxListDefs::ECountAll ) > 0 )
        {
        GLX_LOG_INFO("iFocusIndex is KErrNotFound");

        // Set initial focus position
        SetInitialFocus();

        notifyObserver = ETrue;
        }
    
    return notifyObserver;
    }

// -----------------------------------------------------------------------------
// Move focus if index is changed.
// -----------------------------------------------------------------------------
//
TBool CGlxNavigableList::MoveFocusIfIndexChanged( TInt aChangedIndex, TInt aMoveBy )  
    {
    TRACER( "CGlxNavigableList::MoveFocusIfIndexChanged" );
    
    if ( iFocusIndex >= aChangedIndex )
       {
	       iFocusIndex += aMoveBy;
	       return ETrue; // notify observer
       }
       
       return EFalse; // don't notify observer
    }

// -----------------------------------------------------------------------------
// Handle items being removed
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::HandleItemsRemoved( TInt aRemovedFromIndex, TInt aCount )  
    {
    TRACER( "CGlxNavigableList::HandleItemsRemoved" );

    // Do not test invariant in the beginning of this function. Right now,
    // the state is not valid, since iItemList's state has already changed, and
    // this function will align the state of this object with iItemList.

    // update selection
    UpdateSelectionAfterRemove( aRemovedFromIndex, aCount );
    TInt oldFocusIndex = iFocusIndex;
    TBool notifyOfFocusChange = UpdateFocusAfterRemoval( aRemovedFromIndex, 
        aCount );
    
    // notify observer
    iObserver.HandleItemsRemoved( aRemovedFromIndex, aCount );
    
    // notify of focus change after having notified of items being removed,
    // so that focus index points to the correct real item on client side
    NotifyFocusChange( NGlxListDefs::EUnknown, oldFocusIndex, 
                notifyOfFocusChange );
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// Update selection after remove
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxNavigableList::UpdateSelectionAfterRemove( TInt aRemovedFromIndex, 
        TInt aRemovedCount )
    {
    TRACER( "CGlxNavigableList::UpdateSelectionAfterRemove");
    
    // remove items from selected list that were removed
    TRemoveSelectedItemIndexStrategy removeStrategy( iSelectedItemIndices );
    ProcessSelectionItemIndicesBetweenIndexes( aRemovedFromIndex, 
        aRemovedFromIndex + aRemovedCount, removeStrategy );
    
    // move selection indexes from removal point 
    TMoveSelectedItemIndexStrategy moveStrategy( iSelectedItemIndices, -aRemovedCount );
    ProcessSelectionItemIndicesBetweenIndexes( aRemovedFromIndex + aRemovedCount, 
        KMaxTInt, moveStrategy );
    }

// -----------------------------------------------------------------------------
// Move focus after items have been removed
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline TBool CGlxNavigableList::UpdateFocusAfterRemoval( TInt aRemovedFromIndex, 
        TInt aRemovedCount )  
    {
    TRACER( "CGlxNavigableList::UpdateFocusAfterRemoval");
    
    // Assume focus will be changed (simplifies code below)
    TBool notifyObserver = EFalse;   
    
    // move focus if it is after the last removed item (incl. list has become empty)
    if ( iFocusIndex >= aRemovedFromIndex + aRemovedCount )
        {
        iFocusIndex -= aRemovedCount;        
        // although the focus did not logically change in this case, notify
        // anyway to simplify client code
        notifyObserver = ETrue;
        }
    // move focus if it is on removed items
    else if ( iFocusIndex >= aRemovedFromIndex )
        {
        iFocusIndex = Min( aRemovedFromIndex, Count() - 1 );
        notifyObserver = ETrue;
        }
    // else focus is before removed items, so do nothing       

    return notifyObserver;
    }

// -----------------------------------------------------------------------------
// Remove selection indices between two indexes
// -----------------------------------------------------------------------------
//
void CGlxNavigableList::ProcessSelectionItemIndicesBetweenIndexes( 
        TInt aFromIndex, TInt aToIndex, MSelectionIndexStrategy& aStrategy )
    {
    TRACER("CGlxNavigableList::ProcessSelectionItemIndicesBetweenIndexes");
    
    // Find first selection item index that points to an index larger or equal
    // than aFromIndex
    TInt selectedItemIndex = FindFirstSelectedItemIndexBefore( aToIndex );
    // Process until finds a selection item index pointing to an item smaller 
    // than aFromIndex
    while ( selectedItemIndex >= 0 && 
            iSelectedItemIndices[ selectedItemIndex ] >= aFromIndex ) 
        {
        aStrategy.Process( selectedItemIndex );
        selectedItemIndex--;
        }
    }

// -----------------------------------------------------------------------------
// Return first selection item index on or after
// -----------------------------------------------------------------------------
//
inline TInt CGlxNavigableList::FindFirstSelectedItemIndexBefore( TInt aMaxIndex )
    {
    TRACER( "CGlxNavigableList::FindFirstSelectedItemIndexBefore");
    
    // Find first selection item index that points to an index larger or equal
    // than aAtLeastIndex
    TInt i = iSelectedItemIndices.Count() - 1;
    while ( i >= 0 )
        {
        if ( iSelectedItemIndices[i] < aMaxIndex )
            {
            break;
            }
        i--;
        }
    
    // KErrNotFound == -1, which will be returned if the loop did not break
    return i; 
    }

// ---------------------------------------------------------------------------
// Test invariant
// ---------------------------------------------------------------------------
EXPORT_C void CGlxNavigableList::__DbgTestInvariant() const
    {
    TRACER( "CGlxNavigableList::__DbgTestInvariant");
    
    #if 0
    //def _DEBUG

    // focus index must be KErrNotFound if list is empty
    __ASSERT_DEBUG( iFocusIndex == KErrNotFound || iItemList->Count( NGlxListDefs::ECountAll ), Panic( EGlxPanicIllegalState ) ); 
    // focus index must be between 0 and count if list is not empty
    __ASSERT_DEBUG( ( iFocusIndex >= 0 && 
                      iFocusIndex < iItemList->Count( NGlxListDefs::ECountAll ) ) || 
                      !iItemList->Count( NGlxListDefs::ECountAll ), 
                      Panic( EGlxPanicIllegalState ) ); 
    // max count amount of items can be selected
    __ASSERT_DEBUG( iSelectedItemIndices.Count() <= iItemList->Count( NGlxListDefs::ECountAll ), Panic( EGlxPanicIllegalState ) ); 
    // test selection item index array
    TInt previousSelectedIndex = -1;
    for ( TInt i = 0; i < iSelectedItemIndices.Count(); i++ )
        {
        // index must be between zero and count
        __ASSERT_DEBUG( iSelectedItemIndices[ i ] >= 0 && iSelectedItemIndices[ i ] < iItemList->Count( NGlxListDefs::ECountAll ), Panic( EGlxPanicIllegalState ) ); 
        // index must not be in order
        __ASSERT_DEBUG( iSelectedItemIndices[ i ] > previousSelectedIndex, Panic( EGlxPanicIllegalState ) ); 
        // prepare next round
        previousSelectedIndex = iSelectedItemIndices[ i ];
        }
    
    #endif // _DEBUG
    }
