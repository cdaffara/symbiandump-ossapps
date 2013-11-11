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
* Description:    Window to a list of objects
*
*/




#include "glxlistwindow.h"

#include "glxpanic.h"
#include "glxlistutils.h" //for GlxListUtils
#include <glxtracer.h>                 //For logging
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CGlxDataWindow 
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
class CGlxDataWindow : public CBase 
    {
    friend class TGlxWindowIterator;
public:
    /** Destructor*/
    ~CGlxDataWindow()
        {
        iObjects.Close();
        }
        
    /** Destroy contents of the window */
    void Destroy()
        {
        iObjects.ResetAndDestroy();
        }
        
    /** Reserve max window size */
    inline void ReserveL( TInt aMaxCount )
        {
        iObjects.ReserveL( aMaxCount );
        }
        
    /** Preparet the window to accept items */
    void InitializeL( const CGlxListWindow::TRange& aRange, TInt aTotalSize )
        {
        __TEST_INVARIANT;
        
        iTotalSize = aTotalSize;    
        // use GlxListUtils::NormalizedIndex instead of Normalize to avoid 
        // execution-order dependency to the setting of iTotalSize
        iWindowStartIndex = GlxListUtils::NormalizedIndex( aRange.iStartIndex, iTotalSize );
        
        // all existing pointers in the array are NULL, as they have been 
        // set so by PopulateExistingAndUnuseOld
        
        // add sufficient number of new null pointers
        for ( TInt i = iObjects.Count(); i < aRange.iLength; i++ ) 
            {
            // Append cannot fail since reservation has been made
            ( void ) iObjects.AppendL( NULL );
            }

        // remove unnecessary pointers
        for ( TInt i = iObjects.Count() - 1; i >= aRange.iLength; i-- )
            {
            iObjects.Remove( i );
            }
            
        __TEST_INVARIANT;
        }
        
    /** @return an iterator that can iterate throught the items in window */
    inline TGlxWindowIterator Iterator() const
        {
        return TGlxWindowIterator( *this );
        }
        
    /** @return whether the list index is within range */
    inline TBool InRange( TInt aListIndex ) const
        {
        return KErrNotFound != WindowIndex( aListIndex );
        }

    inline CBase*& operator[] ( TInt aListIndex )
        {
        return iObjects[ WindowIndex( aListIndex ) ];
        }
        
    /** @return the length of the window */
    inline TInt Size() const
        {
        return iObjects.Count();
        }
        
    /** @return the total length of the list */
    inline TInt TotalSize() const
        {
        return iTotalSize;
        }
        
private:        
    /** convert a window index to list index */
    TInt ListIndex( TInt aWindowIndex ) const
        {
    	__ASSERT_DEBUG( aWindowIndex >= 0 && aWindowIndex < Size(), Panic( EGlxPanicIllegalArgument ) ); 
        return Normalize( aWindowIndex + iWindowStartIndex );
    	}

    /** convert a list index to window index */
    /// @todo Make this private
    TInt WindowIndex( TInt aListIndex ) const
        {
        if ( KErrNotFound == aListIndex )
            {
            return KErrNotFound;
            }

    	__ASSERT_DEBUG( aListIndex >= 0 && aListIndex < iTotalSize, Panic( EGlxPanicIllegalArgument ) );
    	
    	// Convert the list index to a window index
    	TInt windowIndex = Normalize( aListIndex - iWindowStartIndex );
    	if ( windowIndex >= Size() ) 
    		{
    		return KErrNotFound; // Does not fit to the window
    		}
    	return windowIndex; 
    	}
        
    /** Normalise index to list length */
    inline TInt Normalize( TInt aIndex ) const
        {
        return GlxListUtils::NormalizedIndex( aIndex, iTotalSize );
        }
    
    /** Test state */
    void __DbgTestInvariant() const
        {
        __ASSERT_DEBUG(iTotalSize >= 0 && iTotalSize < 50000, Panic(EGlxPanicIllegalState)); // Sanity check
        __ASSERT_DEBUG(iObjects.Count() <= iTotalSize, Panic(EGlxPanicIllegalState));
        }
      
private:   
    /// Full size of the list
	TInt iTotalSize;
	
	/// Index of the first object in iWindow in the list,
	/// i.e., the list index from which the window starts from
	TInt iWindowStartIndex;
	
    /// List of objects in the window, starting from iWindowStartIndex
    RPointerArray< CBase > iObjects;
    };
    
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
TGlxWindowIterator::TGlxWindowIterator( const CGlxDataWindow& aWindow )    
    {
    TRACER("TGlxWindowIterator::TGlxWindowIterator");
    
    iWindow = &aWindow;
    iWindowIndex = 0;
    }
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
EXPORT_C TGlxWindowIterator::TGlxWindowIterator( const TGlxWindowIterator& aIterator ) 
    {
    TRACER("TGlxWindowIterator::TGlxWindowIterator");
    
    iWindow = aIterator.iWindow;
    iWindowIndex = aIterator.iWindowIndex;
    }
    
// -----------------------------------------------------------------------------
// ++
// -----------------------------------------------------------------------------
EXPORT_C TInt TGlxWindowIterator::operator++( TInt ) 
    {
    TRACER("TGlxWindowIterator::operator++");
    
    if ( iWindowIndex == iWindow->iObjects.Count() ) 
        {
        return KErrNotFound;
        }
    return iWindow->ListIndex( iWindowIndex++ );
    }
    
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// TChange
// -----------------------------------------------------------------------------
//    
CGlxListWindow::TChange::TChange( TInt aNewFocusIndex, TInt aNewTotalSize, TChangeType aChangeType, 
        TInt aFirstChangedIndex, TInt aLastChangedIndex )   
    {
    TRACER("TChange::Default Constructor");
    
    iNewFocusIndex = aNewFocusIndex;
    iNewTotalSize = aNewTotalSize;
    iChangeType = aChangeType;
    iFirstChangedIndex = aFirstChangedIndex;
    iLastChangedIndex = aLastChangedIndex;
    }
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxListWindow::CGlxListWindow( MGlxWindowObjectFactory& aObjectFactory ) 
        : iObjectFactory( aObjectFactory )
	{
	TRACER("CGlxListWindow::Default Constructor");
	
    iFocusIndex = KErrNotFound; // No items, hence no focus
	}
    
// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxListWindow::ConstructL() 
	{
	TRACER("CGlxListWindow::ConstructL");
	
    iWorkingWindow = new (ELeave) CGlxDataWindow;
    iWindow = new (ELeave) CGlxDataWindow;
    // default range offsets are 0, 0, which means that only the focused item
    // in in the window by default. Create an object for the focus. This
    // makes it optional to call SetRangeOffsetsL if range remains 0,0
    iUnusedObjects.AppendL( iObjectFactory.CreateObjectL() );
	}
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CGlxListWindow::~CGlxListWindow() 
	{
	TRACER("CGlxListWindow::Destructor");
	
    // objects may exist in either iWindow or object pool (iUnusedObjects)
    if ( iWindow )
        {
        // this call could be removed, if could call Cleanup here
        // currently deriving classes implement CleanupObject, so calling it
        // here is not possible => requires a different object as factory
        iWindow->Destroy(); 
        delete iWindow;
        }
    iUnusedObjects.ResetAndDestroy();
    // the working window does not own anything
    delete iWorkingWindow;
	}

// -----------------------------------------------------------------------------
// Cleans up remaining objects in the window
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxListWindow::CleanupL() 
	{
	TRACER("CGlxListWindow::CleanupL");
	
    // make the window empty. this will clean up all remaining objects.
    UpdateL( TChange( KErrNotFound, 0, EChangeObjectsRemoved, 0, iWindow->TotalSize() - 1 ) );
	}
	
// -----------------------------------------------------------------------------
// SetRangeOffsetsL
// -----------------------------------------------------------------------------
// DEPRECATED
EXPORT_C void CGlxListWindow::SetRangeOffsetsL( TInt aFrontOffset, TInt aRearOffset ) 
	{
	TRACER("CGlxListWindow::SetRangeOffsetsL");
	
    SetRangeOffsetsL( iFocusIndex, iWindow->TotalSize(), aFrontOffset, aRearOffset );
	}
    
// -----------------------------------------------------------------------------
// SetRangeOffsetsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxListWindow::SetRangeOffsetsL( TInt aFocusIndex, 
        TInt aTotalSize, TInt aFrontOffset, TInt aRearOffset ) 
	{
	TRACER("CGlxListWindow::SetRangeOffsetsL");
	
	__ASSERT_DEBUG( aFrontOffset <= 0 && aRearOffset >= 0, Panic( EGlxPanicIllegalArgument ) );
	__ASSERT_DEBUG( ( 0 <= aFocusIndex && aFocusIndex < aTotalSize ) ||
                    ( KErrNotFound == aFocusIndex && 0 == aTotalSize ), 
                    Panic( EGlxPanicIllegalArgument ) );
	
	iFrontOffset = aFrontOffset;
	iRearOffset = aRearOffset;
	
    // only ever expand the range. there is no use case to do the otherwise, 
    // so don't bother optimising that case by freeing unnecessary objects.
	
    // all or no objects may be in use at any time - reserve max count in all arrays
    TInt maxObjectCount = aRearOffset - aFrontOffset + 1;
    iWorkingWindow->ReserveL( maxObjectCount );
    iWindow->ReserveL( maxObjectCount );
    iUnusedObjects.ReserveL( maxObjectCount );
    
    // create enough new objects into the unused objects pool
    // objects are either in the main window or in the pool
    for ( TInt i = iWindow->Size() + iUnusedObjects.Count(); i < maxObjectCount; i++ )
        {
        // cannot fail since reservation made above
        ( void )iUnusedObjects.AppendL( iObjectFactory.CreateObjectL() );
        }
	
	UpdateL( TChange( aFocusIndex, aTotalSize, EChangeNone, 0, 0 ) );
	}
    
// -----------------------------------------------------------------------------
// Iterator
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxWindowIterator CGlxListWindow::Iterator() const
    {
    TRACER("CGlxListWindow::Iterator");
    
    return iWindow->Iterator();
    }
	
// -----------------------------------------------------------------------------
// SetFocusIndexL
// -----------------------------------------------------------------------------
// DEPRICATED
EXPORT_C void CGlxListWindow::SetFocusIndexL( TInt aFocusIndex ) 
	{
	TRACER("CGlxListWindow::SetFocusIndexL");
	
	SetFocusIndexL( aFocusIndex, iWindow->TotalSize() );
	}
	
// -----------------------------------------------------------------------------
// SetFocusIndexL
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxListWindow::SetFocusIndexL( TInt aFocusIndex, TInt aTotalSize ) 
	{
	TRACER("CGlxListWindow::SetFocusIndexL");
	
	__ASSERT_DEBUG( ( 0 <= aFocusIndex && aFocusIndex < aTotalSize ) ||
                    ( KErrNotFound == aFocusIndex && 0 == aTotalSize ), 
                    Panic( EGlxPanicIllegalArgument ) );
    __TEST_INVARIANT;
    
	UpdateL( TChange( aFocusIndex, aTotalSize, EChangeNone, 0, 0 ) );
    
    __TEST_INVARIANT;
	}
		
// -----------------------------------------------------------------------------
// AddObjects
// -----------------------------------------------------------------------------
// DEPRICATED
EXPORT_C void CGlxListWindow::AddObjectsL( TInt aFirstNewIndex, TInt aLastNewIndex ) 
	{
	TRACER("CGlxListWindow::AddObjectsL");
	
	TInt newItemCount = aLastNewIndex - aFirstNewIndex + 1;
	TInt newTotalSize = iWindow->TotalSize() + newItemCount;
	
	// Check if it is necessary to move focus
	TInt newFocusIndex = iFocusIndex;
	if ( KErrNotFound == newFocusIndex )
		{
		// List is empty, so set focus to the first item
		__ASSERT_DEBUG(iWindow->TotalSize() == 0, Panic(EGlxPanicIllegalState)); // Must have had an empty list to have no focus
		newFocusIndex = 0; 
		}
	else if (newFocusIndex >= aFirstNewIndex)
		{
		// Move focus, so that the focus stays on the same item that was 
		// focused before items were added
		newFocusIndex += newItemCount;
		}
		
    AddObjectsL( newFocusIndex, newTotalSize, aFirstNewIndex, aLastNewIndex );
	}
		
// -----------------------------------------------------------------------------
// AddObjects
// -----------------------------------------------------------------------------
//
EXPORT_C void CGlxListWindow::AddObjectsL( TInt aFocusIndex, 
        TInt aTotalSize, TInt aFirstNewIndex, TInt aLastNewIndex ) 
	{
	TRACER("CGlxListWindow::AddObjectsL");
	
    __ASSERT_DEBUG( 0 <= aFirstNewIndex && aFirstNewIndex <= aLastNewIndex && 
                    aLastNewIndex < aTotalSize &&
                    0 <= aFocusIndex && aFocusIndex < aTotalSize,
                    Panic( EGlxPanicIllegalArgument ) ); 
    __TEST_INVARIANT;
	
	UpdateL( TChange( aFocusIndex, aTotalSize, EChangeObjectsAdded, 
        aFirstNewIndex, aLastNewIndex ) );
		
    __TEST_INVARIANT;
	}
	
// -----------------------------------------------------------------------------
// RemoveObjects
// -----------------------------------------------------------------------------
// DEPRICATED
EXPORT_C void CGlxListWindow::RemoveObjectsL( TInt aFirstRemovedIndex, TInt aLastRemovedIndex ) 
	{
	TRACER("CGlxListWindow::RemoveObjectsL");
	
	TInt itemsRemovedCount = aLastRemovedIndex - aFirstRemovedIndex + 1;
	TInt newTotalSize = iWindow->TotalSize() - itemsRemovedCount;
	TInt newFocusIndex = iFocusIndex;
	    
	// check number of items left
	if (newTotalSize == 0)
	    {
	    // all items have been removed
	    newFocusIndex = KErrNotFound;
	    }
	else  // still some items in the list
	    {
	    // Focus should stay on the same item if possible. If not, it should
	    // go to the first previous item still existing. If none, it should go to 
	    // first (=first next) item.
        
        // @todo: Do not maintain focus index in this class!!! This is broken
	    
	    // Update focus if the removal point was before	the focus
	    if (aFirstRemovedIndex <= newFocusIndex) 
	    	{
	    	TInt firstToFocusCount = newFocusIndex - aFirstRemovedIndex + 1;
	    	newFocusIndex -= Min(itemsRemovedCount, firstToFocusCount);
	    	
	    	if (newFocusIndex < 0)
	    		{
	    		newFocusIndex = 0;
	    		}
	    	}
	    }

	RemoveObjectsL( newFocusIndex, newTotalSize, aFirstRemovedIndex, aLastRemovedIndex );
	}	

// -----------------------------------------------------------------------------
// RemoveObjects
// -----------------------------------------------------------------------------
//	
EXPORT_C void CGlxListWindow::RemoveObjectsL( TInt aFocusIndex, 
        TInt aTotalSize, TInt aFirstRemovedIndex, TInt aLastRemovedIndex ) 
	{
	TRACER("CGlxListWindow::RemoveObjectsL");
	
    __ASSERT_DEBUG( 0 <= aFirstRemovedIndex && aFirstRemovedIndex <= aLastRemovedIndex &&
                    ( ( 0 <= aFocusIndex && aFocusIndex < aTotalSize ) || 
                      ( KErrNotFound == aFocusIndex && aTotalSize == 0 ) ),
                    Panic( EGlxPanicIllegalArgument ) ); 
    __TEST_INVARIANT;
    
	UpdateL( TChange( aFocusIndex, aTotalSize, EChangeObjectsRemoved, 
        aFirstRemovedIndex, aLastRemovedIndex ) );

    __TEST_INVARIANT;
	}	
    
// -----------------------------------------------------------------------------
// Update
// -----------------------------------------------------------------------------
//
void CGlxListWindow::UpdateL( const TChange& aChange )
	{
	TRACER("CGlxListWindow::UpdateL");
	
    // (in a list of:  |abcdefghijklm|
    // iWindow:        |----efghi----|
    // iWorkingWindow: undefined state
    // iUnusedObjects: XXX (3 objects, in reality only contains objects if list 
    //                      is shorter than max window length)
    
    // Prepare the working window to accept objects
    iWorkingWindow->InitializeL( Range( aChange ), aChange.iNewTotalSize );

    // iWindow:        |----efghi----|
    // iWorkingWindow: |------00000--|
    // iUnusedObjects: XXX 
    
    // move unused objects to pool, and reusable objects to working window
    PopulateExistingAndUnuseOldL( aChange );

    // iWindow:        |----efghi----|
    // iWorkingWindow: |------ghi00--|
    // iUnusedObjects: XXXXX (2 objects added, used to be e and f)
    
    // set the current window to working window and vice versa.
    // do this before populating new items, since when the deriving class
    // is asked to set up the items, it is better to allow it to access the
    // objects from the window (only objects from main window can be accessed
    // externally)
    SwapWindows();
    iFocusIndex = aChange.iNewFocusIndex;
    
    // iWindow:        |------ghi00--|
    // iWorkingWindow: |----xxxxx----| = undefined
    // iUnusedObjects: XXXXX 

    // populate objects in working window that did not exist in current window
    PopulateNew();
    
    // iWindow:        |------ghijk--|
    // iWorkingWindow: |----xxxxx----| = undefined
    // iUnusedObjects: XXX (2 objects moved to iWindow)
	}
    
// -----------------------------------------------------------------------------
// PopulateExistingAndUnuseOld
// -----------------------------------------------------------------------------
//	
void CGlxListWindow::PopulateExistingAndUnuseOldL( const TChange& aChange ) 
    {
    TRACER("CGlxListWindow::PopulateExistingAndUnuseOldL");
    
    // move objects that are needed after the change into the working window, 
    // and objects that are not needed into the object pool
    TGlxWindowIterator currentWindowIterator = iWindow->Iterator();
    TInt index = 0;
    while ( KErrNotFound != ( index = currentWindowIterator++ ) )
        {
        // translate to what the list index would be after the change.
        TInt indexAfterChange = IndexAfterChange( index,  aChange );
        // item is needed in the new window if it is in the range of the new window
        if ( iWorkingWindow->InRange( indexAfterChange ) )
            {
            // move the object from the current window to the working window
            ( *iWorkingWindow )[ indexAfterChange ] = ( *iWindow )[ index ];
            }
        else 
            {
            // ask deriving class to clean up the object (free resources, etc).
            // do this first, so that the deriving class can access the window 
            // by index, if needed
            iObjectFactory.CleanupObject( index, *( *iWindow )[ index ] );
            // add the object to the unused objects pool
            // cannot fail since reservation made
            ( void )iUnusedObjects.AppendL( ( *iWindow )[ index ] );
            }
            
        // clear the pointer in the existing window. it is not strictly necessary
        // to do here, but saves a loop from doing it in CGlxDataWindow::Initialize
        ( *iWindow )[ index ] = NULL;
        }
    }

// -----------------------------------------------------------------------------
// Populate objects that were not reused
// -----------------------------------------------------------------------------
//	
void CGlxListWindow::PopulateNew() 
    {
    TRACER("CGlxListWindow::PopulateNew");
    
    // iWindow is now partially constructed 
    // populate all pointers that are null
    TGlxWindowIterator newWindowIterator = iWindow->Iterator();
    TInt index = 0;
    while ( KErrNotFound != ( index = newWindowIterator++ ) )
        {
        if ( NULL == ( *iWindow )[ index ] )
            {
            CBase* object = UnusedObject();
            ( *iWindow )[ index ] = object;
            // configure the object
        	iObjectFactory.SetupObject( index, *object );
            }
        }
    }
    
// -----------------------------------------------------------------------------
// Reuse
// -----------------------------------------------------------------------------
//	
void CGlxListWindow::SwapWindows() 
    {
    TRACER("CGlxListWindow::SwapWindows");
    
    CGlxDataWindow* temp = iWorkingWindow;
    iWorkingWindow = iWindow;
    iWindow = temp;
    }
    
// -----------------------------------------------------------------------------
// Return an object from the pool, and remove it from the pool
// -----------------------------------------------------------------------------
//
CBase* CGlxListWindow::UnusedObject() 
    {
    TRACER("CGlxListWindow::UnusedObject");
    
    TInt lastObjectIndex = iUnusedObjects.Count() - 1;
    CBase* object = iUnusedObjects[ lastObjectIndex ];
    iUnusedObjects.Remove( lastObjectIndex );
    return object;
    }

// -----------------------------------------------------------------------------
// Access object by index
// -----------------------------------------------------------------------------
//
EXPORT_C CBase* CGlxListWindow::At( TInt aIndex )
	{ 
	TRACER("CGlxListWindow::At");
	
    if ( iWindow->InRange( aIndex ) )
        {
        return ( *iWindow )[ aIndex ];
        }
    return NULL;
    }
	
// -----------------------------------------------------------------------------
// Access object by index
// -----------------------------------------------------------------------------
//
EXPORT_C const CBase* CGlxListWindow::At( TInt aIndex ) const 			
	{ 
	TRACER("CGlxListWindow::At");
	
	return const_cast< CGlxListWindow* >( this )->At( aIndex ); 
	}	

// -----------------------------------------------------------------------------
// Range
// -----------------------------------------------------------------------------
//
CGlxListWindow::TRange CGlxListWindow::Range( const TChange& aChange ) const
    {
    TRACER("CGlxListWindow::Range");
    
    TRange range;
    range.iLength = iRearOffset - iFrontOffset + 1; // Add 1 for focus;

	// Start index is always zero if the whole list fits into the window
	if ( range.iLength >= aChange.iNewTotalSize ) 
		{
		// All items are within range
		range.iStartIndex = 0;
		range.iLength = aChange.iNewTotalSize;
		}
	else 
		{
		// Not all items in the list fit within the range
		range.iStartIndex = GlxListUtils::NormalizedIndex( 
            aChange.iNewFocusIndex + iFrontOffset, aChange.iNewTotalSize );
		
		// check that ranges don't ever meet at the other side of the loop
		__ASSERT_DEBUG( range.iStartIndex != GlxListUtils::NormalizedIndex( range.iStartIndex + range.iLength, aChange.iNewTotalSize ) 
            || ( iFrontOffset == 0 && iRearOffset == 0 ), Panic( EGlxPanicLogicError ) ); 
		}

    return range;
    }

// -----------------------------------------------------------------------------
// Convert a list index to a list index in a changed list
// -----------------------------------------------------------------------------
//
TInt CGlxListWindow::IndexAfterChange( TInt aIndex, 
        const TChange& aChange ) const
	{
	TRACER("CGlxListWindow::IndexAfterChange");
	
	// Convert the new list index into an old list index
	TInt changeCount = aChange.iLastChangedIndex - aChange.iFirstChangedIndex + 1;
    
	TInt indexAfterChange = aIndex;
	if ( aChange.iFirstChangedIndex <= aIndex )
		{
		if ( aChange.iChangeType == EChangeObjectsRemoved )
			{
            //          f l                 (first, last)
            // old: |---NNN---||||||------|
            // new: |------||||||------|
            
			// If the index is between the indexes, the change will remove it
			if ( aIndex <= aChange.iLastChangedIndex )
				{
				// The index is one of the new items. It does not exist in the old window
				indexAfterChange = KErrNotFound;
				}
			else 
				{
				// The index is after the items that will be removed. Adjust the index by "removing" the items.
				indexAfterChange -= changeCount;
				}
			}
		else if ( aChange.iChangeType == EChangeObjectsAdded )
			{
			// Adjust the index by "adding" the items before the index
            // old: |------||||||------|
            // new: |---NNN---||||||------|
            //          f l                 (first, last)
			indexAfterChange += changeCount;
			}
		// if EChangeNone, no adjustment needed
		}
		// else no adjustment need, since the change happened after the index
	
	return indexAfterChange;
	}

// ---------------------------------------------------------------------------
// Test invariant
// ---------------------------------------------------------------------------
EXPORT_C void CGlxListWindow::__DbgTestInvariant() const
    {
    TRACER("CGlxListWindow::__DbgTestInvariant");
    
    __ASSERT_DEBUG( ( iFocusIndex >= 0 && iFocusIndex < iWindow->TotalSize() ) || 
                    ( KErrNotFound == iFocusIndex && 0 == iWindow->TotalSize() ), 
                    Panic( EGlxPanicIllegalState ) );
    __ASSERT_DEBUG( iFrontOffset <= 0 && iRearOffset >= 0, Panic( EGlxPanicIllegalState ) );
    }
