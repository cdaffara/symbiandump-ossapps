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
* Description:    List of media items
*
*/




// my include
#include "glxitemlist.h"

// system includes
#include <glxassert.h>
#include <glxtracer.h>
#include <mpxcollectionpath.h>

// user includes
#include "mglxitemlistobserver.h"
#include "mglxmediapool.h"

using namespace NGlxItemList;

namespace NGlxItemList
    {
    // -----------------------------------------------------------------------------
    // Remove item from list and remove linking
    // -----------------------------------------------------------------------------
    //
    void RemoveItem( TInt aIndex, RArray< TGlxMedia >& aList, 
            MGlxMediaUser& aMediaUser )
        {
        // Remove link between TGlxMedia and CGlxMedia
        aList[ aIndex ].SetMedia( NULL, aMediaUser );
        // Remove from list
        aList.Remove( aIndex ); 
        }        
    
    /** 
     * Class to contain change processing current state and difference length
     *
     * @author Aki Vanhatalo
     */
    NONSHARABLE_CLASS ( TDifferenceInfo )
        {
    public:
        /** Constructor */
        TDifferenceInfo()
            {
            iSourceIndex = 0;
            iTargetIndex = 0;
            iCount = 0;
            }
        
        /// current index on source list. Modification strategy can change this
        /// to skip items in the source list.
        TInt iSourceIndex;
        
        /// current index on target list. Modification strategy can change this
        /// to skip items in the target list.
        TInt iTargetIndex;
        
        /// lenght of change: number of items that would need to be added or removed
        /// to eliminate difference between source and target list. It is up to 
        /// the modification strategy to decide if it actually eliminates the 
        /// difference or simply observes it. Any change of this variable by 
        /// a strategy is ignored.
        TInt iCount;
        };

    /** 
     * Modification strategy interface
     *
     * This interface will be called when differences are found in source and 
     * target list, and instructs how the differences should be solved,
     *
     * Use of modification strategies allows the same difference evaluation 
     * algorithm to be used for both calculating required target array space, and 
     * for doing the actual changes to into the target array.
     * 
     * @author Aki Vanhatalo
     */
    class MListModificationStrategy
        {
    public:
        /**
         * There is a difference in lists that requires removing items for 
         * the difference to be eliminated
         * @param aInfo iTargetIndex index from which to remove items
         *              iCount number of items that would need to be removed
         *              iSourceIndex index in source list (to allow adjustement)
         */
        virtual void Remove( TDifferenceInfo& aInfo ) = 0;
        
        /**
         * There is a difference in lists that requires adding items from source 
         * list to target lits for the difference to be eliminated
         * @param aInfo iTargetIndex index in target list at which to insert
         *              iCount number of items that would need to be copied
         *              iSourceIndex index in source list from which to copy 
         */
        virtual void Insert( TDifferenceInfo& aInfo ) = 0;
        };
        
    /** 
     * Modification strategy that calculates how much slack space is required for 
     * differences in lists to be fixed (i.e., TChangeListStategy to be executed)
     * 
     * @author Aki Vanhatalo
     */
    NONSHARABLE_CLASS( TCalculateRequiredSpaceStrategy ) : 
            public MListModificationStrategy
        {
    public:
        /** Constructor */
        TCalculateRequiredSpaceStrategy( const RArray< TGlxMedia >& aTargetList )
            {
            // Pick up initial required space
            iRequiredSpace = aTargetList.Count();
            // Set initial value
            iCurrentLengthDifference = 0;
            }
     
        // from MListModificationStrategy
        void Remove( TDifferenceInfo& aInfo )
            {
            // Currently required space is reduced
            iCurrentLengthDifference -= aInfo.iCount;
            
            // Skip over "removed" items (items were not removed, simply counted)
            aInfo.iTargetIndex += aInfo.iCount;
            }
            
        // from MListModificationStrategy
        void Insert( TDifferenceInfo& aInfo )
            {
            // Currently required space is increased
            iCurrentLengthDifference += aInfo.iCount;
            // Check if the currently required space is the maximum space
            iRequiredSpace = Max( iCurrentLengthDifference, iRequiredSpace );
            
            // Skip over source items, but not target items, since nothing was 
            // actually inserted
            aInfo.iSourceIndex += aInfo.iCount;
            }
        
    public: // allow public since internal to CGlxItemList implementation
        /// space required in item list for TChangeListStategy to be 
        /// executable without fail
        TInt iRequiredSpace; 
        /// current difference in space needed; iRequiredSpace is a max of this
        TInt iCurrentLengthDifference;
        };

    /** 
     * Modification strategy that fixes the differences between source and target 
     * list. 
     *
     * Note: Implementation assumes that there is enough slack space in target 
     * list for insertion operations to always succeed. (I.e., 
     * TCalculateRequiredSpaceStrategy needs to be used first to find out how much 
     * slack is required)
     * 
     * @author Aki Vanhatalo
     */
    NONSHARABLE_CLASS( TChangeListStategy ) : public MListModificationStrategy
        {
    public:
        /** Constructor */
        TChangeListStategy( const CMPXCollectionPath& aSourceList, 
            const TGlxIdSpaceId& aIdSpaceId, RArray< TGlxMedia >& aTargetList, 
                const MGlxMediaPool& aMediaPool, MGlxMediaUser& aMediaUser, 
                    MGlxItemListObserver& aObserver )
                        : iSource( aSourceList ), iTarget( aTargetList ),
                            iMediaPool( aMediaPool ), iIdSpaceId( aIdSpaceId ), 
                                iMediaUser( aMediaUser ), iObserver( aObserver )
            {
            // do nothing (more)
            }
        
        // from MListModificationStrategy
        void Remove( TDifferenceInfo& aInfo )
            {
            // Remove block from target list, start from end and work backwards (quicker)
            for ( TInt i = aInfo.iTargetIndex + aInfo.iCount - 1; 
                  i >= aInfo.iTargetIndex; 
                  i-- )
                {
                RemoveItem( i, iTarget, iMediaUser );
                }
            
            // Notify observer    
            iObserver.HandleItemsRemoved( aInfo.iTargetIndex, aInfo.iCount );
            }
            
        // from MListModificationStrategy
        void Insert( TDifferenceInfo& aInfo )
            {
            // Insert all items to target list
            InsertBlock( aInfo );
            
            // Notify observer    
            iObserver.HandleItemsAdded( aInfo.iTargetIndex, aInfo.iCount );
            
            // Skip over inserted and source items
            aInfo.iTargetIndex += aInfo.iCount;
            aInfo.iSourceIndex += aInfo.iCount;
            }
        
    private:
        /**
         * Copy a block of items from source to target
         * @param aInfo See @ref MListModificationStrategy::Insert
         */
        inline void InsertBlock( const TDifferenceInfo& aInfo )
            {
            TInt sourceIndex = aInfo.iSourceIndex;

            // Copy items from source list to target list
            TInt untilTargetIndex = aInfo.iTargetIndex + aInfo.iCount;
            for ( TInt targetIndex = aInfo.iTargetIndex; 
                    targetIndex < untilTargetIndex; targetIndex++ )
                {
                // IdOfIndex will not return "invalid id", since the code is only 
                // asking for ids within the range of the list
                TGlxMedia item( TGlxMediaId( iSource.IdOfIndex( sourceIndex ) ) );
                // set media, assumes that media is either NULL, or has an 
                // allocation made for a new user. Builds a link to CGlxMedia 
                // object, and from CGlxMedia object to iMediaUser
                item.SetMedia( iMediaPool.Media( iIdSpaceId, item.Id() ), iMediaUser );
                // add to item list
                // ignore error, cannot fail since reservation made
                iTarget.Insert( item, targetIndex ); 
                
                sourceIndex++;
                }
            }    
        
    private:
        /// Source list to copy ids from. 
        const CMPXCollectionPath& iSource;
        /// Target list to which to copy / from which to remove
        RArray< TGlxMedia >& iTarget;
        /// Provider of media objects
        const MGlxMediaPool& iMediaPool;
        /// Id space id, for looking up items from media provider
        TGlxIdSpaceId iIdSpaceId;
        /// User of the new media objects, if any TGlxMedia-CGlxMedia links are added
        MGlxMediaUser& iMediaUser;
        /// Observer for changes in list
        MGlxItemListObserver& iObserver;
        };
        
    } // namespace NGlxItemList
    
// -----------------------------------------------------------------------------
// CGlxItemList implementation
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CGlxItemList* CGlxItemList::NewL( const TGlxIdSpaceId& aIdSpaceId, 
        MGlxItemListObserver& aObserver, MGlxMediaUser& aMediaUser )
    {
    TRACER("CGlxItemList::NewL");    
    // No ConstructL function currently, so simply return an instance
    return new (ELeave) CGlxItemList( aIdSpaceId, aObserver, aMediaUser );
    }
            
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxItemList::CGlxItemList( const TGlxIdSpaceId& aIdSpaceId, 
    MGlxItemListObserver& aObserver, MGlxMediaUser& aMediaUser )
        : iIdSpaceId( aIdSpaceId ), iMediaUser( aMediaUser ), 
            iObserver( aObserver )
    {
    TRACER("CGlxItemList::Default Constructor");
    
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// Destructor
// ------------------------------------------------------------------------- ----
//
CGlxItemList::~CGlxItemList()
    {
    TRACER( "CGlxItemList::~CGlxItemList" );

    // Remove user of media objects
    TInt count = iItems.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        iItems[ i ].SetMedia( NULL, iMediaUser );
        }    
    
    iItems.Close();
    }

// -----------------------------------------------------------------------------
// Synchronise contents of the list with the collection path
// -----------------------------------------------------------------------------
//
void CGlxItemList::SetContentsL( const CMPXCollectionPath& aSource, 
        const MGlxMediaPool& aMediaPool )
    {
    TRACER( "CGlxItemList::SetContentsL" );
    __TEST_INVARIANT;
    
    // calculate how much space needs to be reserved in the items array
    // before doing any modifications. This allows the update to
    // be completed successfully or not at all if leave occurs
    TCalculateRequiredSpaceStrategy calculateDeltaLengthStrategy ( iItems );
    ProcessDifferences ( aSource, calculateDeltaLengthStrategy ); 
    
    // make reservation so that paths can be syncronised without fail
    iItems.ReserveL( iItems.Count() + 
        calculateDeltaLengthStrategy.iRequiredSpace );
    
    // now implement the changes (notifies observer)
    TChangeListStategy changeListStrategy( aSource, iIdSpaceId, iItems, 
        aMediaPool, iMediaUser, iObserver );
    ProcessDifferences ( aSource, changeListStrategy ); 
    
    // Remove slack space from list
    iItems.Compress();
  
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// Find differences in new and old path, and ask strategy to act on them
// -----------------------------------------------------------------------------
//
void CGlxItemList::ProcessDifferences( const CMPXCollectionPath& aSource, 
        MListModificationStrategy& aModificationStrategy )
    {
    TRACER("CGlxItemList::ProcessDifferences");
    
    TDifferenceInfo info;
    
    // Find the indexes of the first items that are the same on both
    // source and target list, until there are no more items in either list.
    // (If either one of the lists reaches its end, return the next index 
    // from the last one, i.e., count.)
    // Ask the strategy object to handle cases in which items 
    // have been inserted to the list or removed from the list.
    // 
    // Note: not using a temporary variable for "target count", since count can change
    // during the loop, as items may get added/removed.
    TInt sourceCount = aSource.Count();
    while ( info.iSourceIndex < sourceCount || info.iTargetIndex < Count() )
        {
        // find next matching source and target indexes
        TInt sourceMatchIndex = KErrNotFound;
        TInt targetMatchIndex = KErrNotFound;
        FindMatchingItems( info, aSource, sourceMatchIndex, targetMatchIndex );
        
        // Process differences
        ProcessRemove( targetMatchIndex, info, aModificationStrategy );
        ProcessInsert( sourceMatchIndex, info, aModificationStrategy );
        
        // go to next items
        info.iSourceIndex++;
        info.iTargetIndex++;
        }
    }

// -----------------------------------------------------------------------------
// Find the next matching item in source and target path
// inline in cpp file only, so will be inlined in arm compiler
// -----------------------------------------------------------------------------
//
inline void CGlxItemList::FindMatchingItems( const TDifferenceInfo& aInfo, 
        const CMPXCollectionPath& aSource, TInt& aSourceMatchIndex, 
        TInt& aTargetMatchIndex )
    {
    TRACER("CGlxItemList::FindMatchingItems");
    
    // For each remaining source item, test each remaining target item, until 
    // match found
    TInt sourceCount = aSource.Count();
    TInt targetCount = Count();
    TInt sourceIndex = aInfo.iSourceIndex;

    // The most common case is that the next items match (no items changed
    // at the indexes being examined). The loop is optimised for that case.
    // The loop (including the inner loop) terminates on the first round, 
    // since matching items are immediately found.
    // The case optimised next is the "items were removed from middle" case,
    // as this is likely to happen in an active view. In that case, the upper
    // loop is run only once, and the inner loop finds the match.
    // The slowest cases are when items were inserted or removed from the end.
    // In that case both loops may run almost to end, so that is slow.
    // However, that case should be a rare case (while view is active),
    // so from performance perspective this loop should be ok, even 
    // though it has N*M complexity.
    while ( sourceIndex < sourceCount )
        {
        // get next source item id
        // IdOfIndex will not return "invalid id", since the code is only 
        // asking for ids within the range of the list, but even if it did,
        // the logic would still work
        TGlxMediaId sourceId ( aSource.IdOfIndex( sourceIndex ) ); 
        
        // try to find a target item that matches the source item
        // (cannot use RArray::Find, since it does not allow specifying
        //  start index)
        TInt targetIndex = aInfo.iTargetIndex;
        while ( targetIndex < targetCount)
            {
            if ( sourceId == iItems[ targetIndex ].Id() )
                {
                // Found match, store return values
                aTargetMatchIndex = targetIndex;
                aSourceMatchIndex = sourceIndex;
                return;
                }
            targetIndex++;
            }
        sourceIndex++;
        }
    
    // No match found
    aTargetMatchIndex = Count();
    aSourceMatchIndex = aSource.Count();
    }
    
// -----------------------------------------------------------------------------
// Process the need to remove items to eliminate differences
// inline in cpp file only, so will be inlined in arm compiler
// -----------------------------------------------------------------------------
//
inline void CGlxItemList::ProcessRemove( TInt aTargetMatchIndex, 
        TDifferenceInfo& aInfo, MListModificationStrategy& aStrategy )
    {
    TRACER("CGlxItemList::ProcessRemove");
    
    aInfo.iCount = aTargetMatchIndex - aInfo.iTargetIndex;
    if ( aInfo.iCount > 0 )
        {
        aStrategy.Remove( aInfo );
        }
    }

// -----------------------------------------------------------------------------
// Process the need to insert items to eliminate differences
// inline in cpp file only, so will be inlined in arm compiler
// -----------------------------------------------------------------------------
//
inline void CGlxItemList::ProcessInsert( TInt aSourceMatchIndex, 
        TDifferenceInfo& aInfo, MListModificationStrategy& aStrategy )
    {
    TRACER("CGlxItemList::ProcessInsert");
    
    aInfo.iCount = aSourceMatchIndex - aInfo.iSourceIndex;
    if ( aInfo.iCount > 0 )
        {
        aStrategy.Insert( aInfo );
        }
    }
    
// -----------------------------------------------------------------------------
// Remove an item from the list
// -----------------------------------------------------------------------------
//
void CGlxItemList::Remove( const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aItemId )
    {
    TRACER("CGlxItemList::Remove");
    
    // Assume id space id does not have to be checked for performance reasons, i.e., id 
    // space id is usually, if not always, correct
    TInt index = Index(aIdSpaceId, aItemId ); 
    if ( KErrNotFound != index && iIdSpaceId == aIdSpaceId )    
        {
        // Remove item from the list
        RemoveItem( index, iItems, iMediaUser );
        // Notify observer    
        iObserver.HandleItemsRemoved( index, 1 );
        }
    __TEST_INVARIANT;
    }
    
// -----------------------------------------------------------------------------
// Remove any pointers to the media object at the specified index
// -----------------------------------------------------------------------------
//
void CGlxItemList::RemoveReference( TInt aIndex )
    {
    TRACER( "CGlxItemList::RemoveReference" );
    __TEST_INVARIANT;

    GLX_ASSERT_DEBUG( 0 <= aIndex && aIndex < iItems.Count(), 
        Panic(EGlxPanicIllegalArgument), "removing reference for item out of bounds");

    // remove the reference to the media
    (*this)[ aIndex ].SetMedia( NULL, iMediaUser );
    
    __TEST_INVARIANT;
    }
    
// -----------------------------------------------------------------------------
// Return index by id
// -----------------------------------------------------------------------------
//
TInt CGlxItemList::Index( const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aId ) const
    {
    TRACER("CGlxItemList::Index");
    
    if ( iIdSpaceId == aIdSpaceId )
        {
        // set up comparison functor
        TIdentityRelation<TGlxMedia> match ( &TGlxMedia::MatchById );
        // create dummy object to compare against
        TGlxMedia mediaToCompare( aId );
        // try to find; may return KErrNotFound
        return iItems.Find( mediaToCompare, match );
        }
        
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// Test invariant
// ---------------------------------------------------------------------------
EXPORT_C void CGlxItemList::__DbgTestInvariant() const
    {
    #ifdef _DEBUG
    
    __ASSERT_DEBUG( &iObserver , Panic( EGlxPanicIllegalState ) ); // Null observer
    __ASSERT_DEBUG( iIdSpaceId != KGlxIdNone , Panic( EGlxPanicIllegalState ) ); // No id space

    // Make sure the list contains no duplication
    TInt count = iItems.Count();
    for ( TInt indexA = 0; indexA < count; ++indexA )
        {
        for ( TInt indexB = indexA + 1; indexB < count; ++indexB )
            {
            __ASSERT_DEBUG( !TGlxMedia::MatchById( iItems[indexA], iItems[indexB] ), 
                Panic( EGlxPanicIllegalState ) ); // Duplicate item
            }
        }

    #endif // _DEBUG
    }
