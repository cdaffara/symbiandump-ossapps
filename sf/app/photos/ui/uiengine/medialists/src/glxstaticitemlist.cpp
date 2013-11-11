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
#include "glxstaticitemlist.h"

// system includes
#include <glxassert.h>
#include <glxtracer.h>

// user includes
#include "glxitemlist.h"
#include "glxmediastaticitemdefs.h"

using namespace NGlxListDefs;


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxStaticItemList::RStaticMediaArray::~RStaticMediaArray()
    {
    TRACER( "CGlxStaticItemList::RStaticMediaArray::~RStaticMediaArray" );
    
    // delete owned static items
    TInt count = Count();
    for ( TInt i = 0 ; i < count; i++ )
        {
        Delete( i );
        }
    Close();
    }

// -----------------------------------------------------------------------------
// Delete item at index
// -----------------------------------------------------------------------------
void CGlxStaticItemList::RStaticMediaArray::Delete( TInt aIndex )
    {
    TRACER( "CGlxStaticItemList::RStaticMediaArray::Delete" );
    
    delete ( (*this)[ aIndex ] ).Properties();
    }
    
// -----------------------------------------------------------------------------
// CGlxStaticItemList class implementation
// -----------------------------------------------------------------------------
    
// -----------------------------------------------------------------------------
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CGlxStaticItemList* CGlxStaticItemList::NewL( const TGlxIdSpaceId& aIdSpaceId,
        MGlxItemListObserver& aObserver, MGlxMediaUser& aMediaUser )
    {
    TRACER( "CGlxStaticItemList::NewL");
    CGlxStaticItemList* self = new (ELeave) CGlxStaticItemList( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aIdSpaceId, aMediaUser );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxStaticItemList::CGlxStaticItemList( MGlxItemListObserver& aObserver )
        : iObserver( aObserver )
    {
    TRACER( "CGlxStaticItemList::CGlxStaticItemList" );
    
    // Enable static items by default
    iStaticItemsEnabled = ETrue;
    }

// -----------------------------------------------------------------------------
// Second-phase constructor
// -----------------------------------------------------------------------------
//
void CGlxStaticItemList::ConstructL( const TGlxIdSpaceId& aIdSpaceId,
        MGlxMediaUser& aMediaUser )
    {
    TRACER( "CGlxStaticItemList::ConstructL" );
    
    iItemList = CGlxItemList::NewL( aIdSpaceId, *this, aMediaUser );
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxStaticItemList::~CGlxStaticItemList()
    {
    TRACER( "CGlxStaticItemList::~CGlxStaticItemList" );
    
    delete iItemList;
    // iFrontStaticItems and iRearStaticItems have a destructor that closes them
    }
    
// -----------------------------------------------------------------------------
// return id space id
// -----------------------------------------------------------------------------
//
const TGlxIdSpaceId& CGlxStaticItemList::IdSpaceId() const
    {
    TRACER( "CGlxStaticItemList::IdSpaceId");
    return iItemList->IdSpaceId();
    }

// -----------------------------------------------------------------------------
// Synchronise contents of the list with the collection path
// -----------------------------------------------------------------------------
//
void CGlxStaticItemList::SetContentsL( const CMPXCollectionPath& aPath, 
        const MGlxMediaPool& aMediaPool )
    {
    TRACER( "CGlxStaticItemList::SetContentsL" );
    
    __TEST_INVARIANT;
    iItemList->SetContentsL( aPath, aMediaPool );
    __TEST_INVARIANT;
    }
    
// -----------------------------------------------------------------------------
// Remove an item form the list
// -----------------------------------------------------------------------------
//
void CGlxStaticItemList::Remove( const TGlxIdSpaceId& aIdSpaceId, 
        const TGlxMediaId& aItemId )
    {
    TRACER( "CGlxStaticItemList::Remove" );

    __TEST_INVARIANT;

    if ( aIdSpaceId == KGlxStaticItemIdSpaceId )
        {
        DeleteStaticItem( aItemId );
        }
    else 
        {
        iItemList->Remove( aIdSpaceId, aItemId );
        }
    
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// Remove any pointers to the media object at the specified index
// -----------------------------------------------------------------------------
//
void CGlxStaticItemList::RemoveReference( TInt aIndex )
    {
    TRACER( "CGlxStaticItemList::RemoveReference" );

    __ASSERT_DEBUG( aIndex >= iCountCurrentFrontStatic && 
                    aIndex < iCountCurrentFrontStatic + CountNonStatic(), 
                    Panic( EGlxPanicIllegalArgument ) );

    return iItemList->RemoveReference( aIndex - iCountCurrentFrontStatic );
    }

// -----------------------------------------------------------------------------
// Add a static item
// -----------------------------------------------------------------------------
//
void CGlxStaticItemList::AddStaticItemL( CGlxMedia* aStaticItem, 
        TInsertionPosition aTargetPosition )
    {
    TRACER( "CGlxStaticItemList::AddStaticItemL" );
    __TEST_INVARIANT;

    if ( !aStaticItem )
        {
        GLX_LOG_WARNING( "Supplied static item is null" );
        return;
        }

   __ASSERT_DEBUG( aStaticItem->Id() != KGlxIdNone, Panic( EGlxPanicIllegalArgument ) ); // Must have an id defined,

    // Since ownership is given, it is assumed that this function is free to 
    // modify the static item
    SetupStaticItemL( *aStaticItem );

    // make reservations to arrays so that the object supplied as parameter
    // can be safely modified. Assumption is that client has pushed aStaticItem
    // to cleanup stack.
    // for simplicity, reserve in both static arrays, although only one is needed
    iRearStaticItems.ReserveL( iRearStaticItems.Count() + 1 ); // Space for one new item
    iFrontStaticItems.ReserveL( iFrontStaticItems.Count() + 1 ); // Space for one new item
    
    // try to replace existing static item with the same id, if any
    // TryReplaceStaticItem only replaces the item if the item exists
    // in the same position (list beginning/end) as the new one
    if ( !TryReplaceStaticItem( *aStaticItem, aTargetPosition ) )
        {
        // TryReplaceStaticItem will fail if the item already exists, but in the 
        // other end of the list (aTargetPosition). Therefore, need to try to
        // delete the static item, if any.
        // this call notifies the observer of item being removed
        DeleteStaticItem( aStaticItem->Id() ); 
        
        // add the item to static item list, notified observer if
        // static items currently active
        AddStaticItem( *aStaticItem, aTargetPosition );
        }

    __TEST_INVARIANT;
    }
    
// -----------------------------------------------------------------------------
// Try to replace and existing static item 
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline TBool CGlxStaticItemList::TryReplaceStaticItem( CGlxMedia& aStaticItem, 
        TInsertionPosition aTargetPosition )
    {
    TRACER( "CGlxStaticItemList::TryReplaceStaticItem" );
    
    GLX_ASSERT_DEBUG(aTargetPosition == EInsertFirst || aTargetPosition == EInsertLast, Panic(EGlxPanicLogicError), "Unknown static item target position");
    
    if (aTargetPosition == EInsertFirst)
        {
        return TryReplaceStaticItem( aStaticItem, iFrontStaticItems );
        }
    else 
        {
        return TryReplaceStaticItem( aStaticItem, iRearStaticItems );
        }
    }

// -----------------------------------------------------------------------------
// Try to replace and existing static item 
// -----------------------------------------------------------------------------
//
TBool CGlxStaticItemList::TryReplaceStaticItem( CGlxMedia& aStaticItem, 
        RStaticMediaArray& aStaticItemArray )
    {
    TRACER( "CGlxStaticItemList::TryReplaceStaticItem");
    
    // try to find a static item with the same id in the array.
    // if found, replace the original static item with the new one
    TInt index = Index( aStaticItem.Id(), aStaticItemArray );
    if ( KErrNotFound != index )
        {
        // delete old static itemm does not remove space from array
        aStaticItemArray.Delete( index );
        // take ownership of the static item
        aStaticItemArray[ index ] = TGlxMedia( aStaticItem );

        GLX_LOG_INFO( "Existing static item replaced" );
        return ETrue; // replaced
        }
    
    return EFalse; // not replaced
    }

// -----------------------------------------------------------------------------
// Remove static item
// -----------------------------------------------------------------------------
//
void CGlxStaticItemList::DeleteStaticItem( const TGlxMediaId& aId )
    {
    TRACER( "CGlxStaticItemList::DeleteStaticItem");
    
    // blindly try both arrays, does nothing if item not found
    DeleteStaticItem( aId, iFrontStaticItems, 0, 
        iCountCurrentFrontStatic ); 
    DeleteStaticItem( aId, iRearStaticItems, FirstPostStaticIndex(), 
        iCountCurrentRearStatic ); 
    }

// -----------------------------------------------------------------------------
// Remove static item
// -----------------------------------------------------------------------------
//
void CGlxStaticItemList::DeleteStaticItem( const TGlxMediaId& aId, 
        RStaticMediaArray& aStaticItemArray, TInt aArrayOffset, 
        TInt& aCountCurrentStatic )
    {
    TRACER( "CGlxStaticItemList::DeleteStaticItem");
    
    TInt index = Index( aId, aStaticItemArray );
    if ( KErrNotFound != index )
        {
        // delete old static item, does not remove space from array
        aStaticItemArray.Delete( index );
        
        // remove item from array
        aStaticItemArray.Remove( index );
        
         if ( iStaticItemsEnabled )
            {
            // Remove from static item count
            aCountCurrentStatic--;
            // notify observer
            iObserver.HandleItemsRemoved( index + aArrayOffset, 1 );
            }
        }
    }

// -----------------------------------------------------------------------------
// Add a static item
// -----------------------------------------------------------------------------
//
void CGlxStaticItemList::AddStaticItem( CGlxMedia& aStaticItem, 
        NGlxListDefs::TInsertionPosition aTargetPosition )
    {
    TRACER( "CGlxStaticItemList::AddStaticItem");
    
    // add the item to static item list, notified observer if
    // static items currently active
    if ( EInsertFirst == aTargetPosition )
        {
        AddFrontStaticItem( aStaticItem );
        }
    else 
        {
        AddRearStaticItem( aStaticItem );
        }
    }

// -----------------------------------------------------------------------------
// Add static item to the front of the list
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxStaticItemList::AddFrontStaticItem( CGlxMedia& aStaticItem )
    {
    TRACER( "CGlxStaticItemList::AddFrontStaticItem");
    
    // Add to the beginning of front static item array
    iFrontStaticItems.Insert( TGlxMedia( aStaticItem ), 0 );
    // ignore return value, since assumes reservation was made to the array 
    // prior to calling this function, i.e. cannot fail

    // if static items are currently enabled, add item to the main item list
    if ( iStaticItemsEnabled )
        {
        iCountCurrentFrontStatic++;
        
        // notify of item being added 
        iObserver.HandleItemsAdded( 0, 1 );
        }
    }

// -----------------------------------------------------------------------------
// Add static item to the front of the end of the list
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxStaticItemList::AddRearStaticItem( CGlxMedia& aStaticItem )
    {
    TRACER( "CGlxStaticItemList::AddRearStaticItem");
    
    // Add to the end of rear static item array
    iRearStaticItems.Append( TGlxMedia( aStaticItem ) );
    // ignore return value, since assumes reservation was made to the array 
    // prior to calling this function, i.e. cannot fail
    
    // if static items are currently enabled, add item to the main item list
    if ( iStaticItemsEnabled )
        {
        iCountCurrentRearStatic++;

        // notify of item being added 
        TInt addIndex = CountAll() - 1;
        iObserver.HandleItemsAdded( addIndex, 1 );
        }
    }
 
// -----------------------------------------------------------------------------
// Make static item ready to be added to the list
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxStaticItemList::SetupStaticItemL( CGlxMedia& aStaticItem )
    {
    TRACER( "CGlxStaticItemList::SetupStaticItemL");
    
    // add static attribute, so that can later check if this item is static
    aStaticItem.SetTObjectValueL<TBool>( KGlxMediaStaticItem, ETrue );
             
    // set media category attribute to "command", unless already defined by client
    if( !aStaticItem.IsSupported( KMPXMediaGeneralCategory ) )
        {
        aStaticItem.SetTObjectValueL( KMPXMediaGeneralCategory, EMPXCommand );
        }
    }

// -----------------------------------------------------------------------------
// return ETrue if static items are enabled
// -----------------------------------------------------------------------------
//
TBool CGlxStaticItemList::IsStaticItemsEnabled() const
    {
    TRACER( "CGlxStaticItemList::IsStaticItemsEnabled");
    
    return iStaticItemsEnabled;
    }
        
// -----------------------------------------------------------------------------
// Enable/disable static items
// -----------------------------------------------------------------------------
//
void CGlxStaticItemList::SetStaticItemsEnabled( TBool aEnabled )
    {
    TRACER( "CGlxStaticItemList::SetStaticItemsEnabled");
    GLX_LOG_INFO1( "CGlxStaticItemList::SetStaticItemsEnabled: %b", aEnabled );
    __TEST_INVARIANT;

    iStaticItemsEnabled = aEnabled;
    if ( aEnabled )
        {
        EnableFrontStaticItems();
        EnableRearStaticItems(); 
        }
    else
        {
        DisableFrontStaticItems();
        DisableRearStaticItems(); 
        }
    
    __TEST_INVARIANT;
    }

// -----------------------------------------------------------------------------
// Enable front static items
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxStaticItemList::EnableFrontStaticItems()
    {
    TRACER( "CGlxStaticItemList::EnableFrontStaticItems");
    
    // Notify only if current static item count is zero and there is more than
    // one fron static item
    if ( iCountCurrentFrontStatic < iFrontStaticItems.Count() )
        {
        // Add front items and notify observer
        iCountCurrentFrontStatic = iFrontStaticItems.Count();
        iObserver.HandleItemsAdded( 0, iCountCurrentFrontStatic );
        }
    }
    
// -----------------------------------------------------------------------------
// Enable rear static items
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxStaticItemList::EnableRearStaticItems()
    {
    TRACER( "CGlxStaticItemList::EnableRearStaticItems");
    
    // Notify only if current static item count is zero and there is more than
    // one fron static item
    if ( iCountCurrentRearStatic < iRearStaticItems.Count() )
        {
        // Add rear items and notify observer
        iCountCurrentRearStatic = iRearStaticItems.Count();
        iObserver.HandleItemsAdded( FirstPostStaticIndex(), 
            iCountCurrentRearStatic );
        }
    }

// -----------------------------------------------------------------------------
// Disable static items from the front
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxStaticItemList::DisableFrontStaticItems()
    {
    TRACER( "CGlxStaticItemList::DisableFrontStaticItems");
    
    // Remove static items from front
    if ( iCountCurrentFrontStatic > 0 )
        {
        TInt removedCount = iCountCurrentFrontStatic;
        iCountCurrentFrontStatic = 0;

        // Notify observer
        iObserver.HandleItemsRemoved( 0, removedCount );
        }
    }
    
// -----------------------------------------------------------------------------
// Enable static items from the front
// inline private member function only in cpp file, so will be inlined in arm
// (ok as long as called only once)
// -----------------------------------------------------------------------------
//
inline void CGlxStaticItemList::DisableRearStaticItems()
    {
    TRACER( "CGlxStaticItemList::DisableRearStaticItems");
    
    // Remove static items from rear
    if ( iCountCurrentRearStatic > 0 )
        {
        TInt removedCount = iCountCurrentRearStatic;
        iCountCurrentRearStatic = 0;

        // Notify observer
        iObserver.HandleItemsRemoved( FirstPostStaticIndex(), removedCount );
        }
    }
    
// -----------------------------------------------------------------------------
// Return count
// -----------------------------------------------------------------------------
//
TInt CGlxStaticItemList::Count( NGlxListDefs::TCountType aType ) const
    {
    TRACER( "CGlxStaticItemList::Count");
    
    switch ( aType )  
        {        
        case ECountAllStatic:
            return CountStatic();
            
        case ECountPreStatic:
            return iCountCurrentFrontStatic;
            
        case ECountPostStatic:
            return iCountCurrentRearStatic;  
         
        case ECountNonStatic:
            return CountNonStatic();
            
        default:
            GLX_LOG_WARNING( "Unsupported count type" );
            // Fall through, and return all count as default
        case ECountAll:                                     
            return CountAll();
        }
    }

// -----------------------------------------------------------------------------
// Return count of all items
// inline private member function only in cpp file, so will be inlined in arm
// -----------------------------------------------------------------------------
//
inline TInt CGlxStaticItemList::CountStatic() const
    {
    TRACER( "CGlxStaticItemList::CountStatic");
    
    return iCountCurrentFrontStatic + iCountCurrentRearStatic;
    }

// -----------------------------------------------------------------------------
// Return count of non-static items
// inline private member function only in cpp file, so will be inlined in arm
// -----------------------------------------------------------------------------
//
inline TInt CGlxStaticItemList::CountNonStatic() const
    {
    TRACER( "CGlxStaticItemList::CountNonStatic");
    
    return iItemList->Count(); 
    }

// -----------------------------------------------------------------------------
// Return count of all items
// inline private member function only in cpp file, so will be inlined in arm
// -----------------------------------------------------------------------------
//
inline TInt CGlxStaticItemList::CountAll() const
    {
    TRACER( "CGlxStaticItemList::CountAll");
    
    return iCountCurrentFrontStatic + iItemList->Count() + iCountCurrentRearStatic;
    }
    
// -----------------------------------------------------------------------------
// Index of the first post static item
// -----------------------------------------------------------------------------
//
TInt CGlxStaticItemList::FirstPostStaticIndex() const
    {
    TRACER( "CGlxStaticItemList::FirstPostStaticIndex");
    
    return iCountCurrentFrontStatic + iItemList->Count();
    }
    
// -----------------------------------------------------------------------------
// Return item by index
// -----------------------------------------------------------------------------
//
TGlxMedia& CGlxStaticItemList::Item( TInt aIndex )
    {
    TRACER( "CGlxStaticItemList::Item");
    
    // test if a front static index
    if ( aIndex < iCountCurrentFrontStatic )
        {
        return iFrontStaticItems[ aIndex ];
        }
    
    // test if normal index
    if ( aIndex < iCountCurrentFrontStatic + CountNonStatic() )
        {
        return iItemList->Item( aIndex - iCountCurrentFrontStatic );
       }
        
    // must be a rear static index
    return iRearStaticItems[ aIndex - iCountCurrentFrontStatic - CountNonStatic() ];
    }

// -----------------------------------------------------------------------------
// Return index by id
// -----------------------------------------------------------------------------
//
TInt CGlxStaticItemList::Index( const TGlxIdSpaceId& aIdSpaceId, 
        const TGlxMediaId& aId ) const
    {
    TRACER( "CGlxStaticItemList::Index");
    
    if ( KGlxStaticItemIdSpaceId == aIdSpaceId.Value() )
        {
        // try to find from front static items; may return KErrNotFound
        TInt index = Index( aId, iFrontStaticItems );
        if ( KErrNotFound != index )
            {
            return index;
            }

        // Try rear static items
        index = Index( aId, iRearStaticItems );
        if ( KErrNotFound != index )
            {
            return index + FirstPostStaticIndex();
            }
        }
    else
        {
        // Look at the main list first (most likely case)
        TInt index = iItemList->Index( aIdSpaceId, aId );
        if ( KErrNotFound != index )
            {
            return index + iCountCurrentFrontStatic;
            }
        }
        
    // No such id
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// Return index by id
// -----------------------------------------------------------------------------
//
TInt CGlxStaticItemList::Index( const TGlxMediaId& aId, 
        const RStaticMediaArray& aStaticItems ) const
    {
    TRACER( "CGlxStaticItemList::Index");
    
    // set up comparison functor
    TIdentityRelation< TGlxMedia > match ( &TGlxMedia::MatchById );
    // create dummy object to compare against
    // Create to stack directly, CMedia constructor NULLs all member variables
    TGlxMedia mediaToCompare( aId ); 

    return aStaticItems.Find( mediaToCompare, match );
    }

// -----------------------------------------------------------------------------
// Handle items being added
// -----------------------------------------------------------------------------
//
void CGlxStaticItemList::HandleItemsAdded( TInt aFirstInsertedIndex, 
        TInt aCount )
    {
    TRACER( "CGlxStaticItemList::HandleItemsAdded");
    
    // there may be static items at the front of the list, so adjust and forward
    // to observer
    iObserver.HandleItemsAdded( aFirstInsertedIndex + iCountCurrentFrontStatic,
        aCount );
    }

// -----------------------------------------------------------------------------
// Handle items being removed
// -----------------------------------------------------------------------------
//
void CGlxStaticItemList::HandleItemsRemoved( TInt aRemovedFromIndex, TInt aCount )  
    {
    TRACER( "CGlxStaticItemList::HandleItemsRemoved");
    
    // there may be static items at the front of the list, so adjust and forward
    // to observer
    iObserver.HandleItemsRemoved( aRemovedFromIndex + iCountCurrentFrontStatic,
        aCount );
    }
    
// ---------------------------------------------------------------------------
// Test invariant
// ---------------------------------------------------------------------------
EXPORT_C void CGlxStaticItemList::__DbgTestInvariant() const
    {
    TRACER( "CGlxStaticItemList::__DbgTestInvariant");
    
    #ifdef _DEBUG
    
    __ASSERT_DEBUG( &iObserver, Panic( EGlxPanicIllegalState ) ); // Null observer
    __ASSERT_DEBUG( iItemList, Panic( EGlxPanicIllegalState ) ); // No list

    // Make sure static item counts are zero when static items are disabled
    __ASSERT_DEBUG( ( iCountCurrentFrontStatic == 0 && iCountCurrentRearStatic == 0 )
         || iStaticItemsEnabled, Panic( EGlxPanicIllegalState ) ); // No list
   
    // Make sure static item counts are match array counts when static items are enabled
    __ASSERT_DEBUG( ( iCountCurrentFrontStatic == iFrontStaticItems.Count() 
         && iCountCurrentRearStatic == iRearStaticItems.Count() )
         || !iStaticItemsEnabled, Panic( EGlxPanicIllegalState ) ); // No list

    #endif // _DEBUG
    }
