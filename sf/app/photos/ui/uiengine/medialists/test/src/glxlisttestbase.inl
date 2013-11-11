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



#include "glxlistreconstruction.h"

#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <mpxcollectionpath.h>
#include <glxlistdefs.h>
#include <glxpanic.h>

#include "glxmediastaticitemdefs.h"

const TGlxIdSpaceId KListIdSpace( 1 );
      
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
CGlxListTestBase< _List, _Reconstruction >::~CGlxListTestBase() 
    {
    DeleteMembers();
    }
    
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
void CGlxListTestBase< _List, _Reconstruction >::DeleteMembers()
    {
    delete iReconstruction;
    iReconstruction = NULL;
    delete iList;
    iList = NULL;
    iMediaPool.ResetAndDestroy();
    }
    
// -----------------------------------------------------------------------------
// Ignore
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >::GetRequiredAttributesL( 
        TInt /*aIndex*/, RArray< TMPXAttribute >& /*aAttributes*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// Ignore
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >
        ::HandleAttributesAvailableL(TInt /*aIndex*/, 
            const RArray< TMPXAttribute >& /*aAttributes*/ )
    {
    }    
    
// -----------------------------------------------------------------------------
// Ignore
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >
        ::RemoveReference( TInt /*aIndex*/ )
    {
    }

// -----------------------------------------------------------------------------
// Ignore
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >
        ::HandleError( TInt /*aError*/ )
    {
    }
    
// -----------------------------------------------------------------------------
// MGlxMediaPool::Media implementation
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
CGlxMedia* CGlxListTestBase< _List, _Reconstruction >
        ::Media( const TGlxIdSpaceId& /*aIdSpaceId*/, const TGlxMediaId& /*aId*/) const  
    {
    return NULL;
    }
    
// -----------------------------------------------------------------------------
// Create a list from string
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >
        ::CreateListL( const TDesC8& aString )
    {
    CMPXCollectionPath* path = CreatePathLC( aString );

    // Create reconstruction
    CreateReconstructionL();
    
    // Delete media items
    iMediaPool.ResetAndDestroy();
    
    // Create list
    delete iList;
    iList = NULL;
    iList = _List::NewL( KListIdSpace, *iReconstruction, *this );
    iReconstruction->SetList( *iList );
    // Set the contents of the old list
    iList->SetContentsL( *path, *this );
    // Reset for test
    iReconstruction->ClearNotifications();

    CleanupStack::PopAndDestroy(path );
    }

// -----------------------------------------------------------------------------
// Create a list from string
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >
        ::CreateListL( const TDesC8& aString, const TDesC8& aLinkedToMedia )
    {
    // Create list
    CreateListL( aString );

    // Link to media
    for ( TInt i = 0; i < aLinkedToMedia.Length(); i++)
        {
        if ( aLinkedToMedia[ i ] == 't' )
            {
            CGlxMedia* media = new (ELeave) CGlxMedia( iList->Item( i ).Id() );
            CleanupStack::PushL( media );
            iMediaPool.AppendL( media );
            CleanupStack::Pop( media );

            media->ReserveUsersL( 1 );
            iList->Item(i).SetMedia( media, *this);
            }
        }
    }

// -----------------------------------------------------------------------------
// Create a list from string
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >
        ::CreateListL( const TDesC8& aString, TBool aStaticItemsEnabled,
            NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos )
    {
    // Create list
    CreateListL( aString );
    // Add static items, if any
    AddStaticItemsL( aStaticItemInfos );
    // Enable/disable static items
    iList->SetStaticItemsEnabled( aStaticItemsEnabled );
    // Reset for test
    iReconstruction->ClearNotifications();
    }

// -----------------------------------------------------------------------------
// Create reconstruction
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >::CreateReconstructionL()
    {
    delete iReconstruction;
    iReconstruction = NULL;
    iReconstruction = _Reconstruction::NewL();
    }
    
// -----------------------------------------------------------------------------
// Test if list equals the list in the string
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >::Equals( const TDesC8& aNewString )
    {
    ASSERT( iList && iReconstruction );
    // length of list and string must match
    TBool fail = ( iList->Count() != aNewString.Length() );

    for ( TInt i = 0; i < iList->Count(); i++ )
        {
        fail |= ( iList->Item( i ).Id() != TGlxMediaId( aNewString[ i ] ) );
        }
        
    // Test reconstruction 
    fail |= !iReconstruction->ReconstructionEquals();
    
    return !fail;
    }
    
// -----------------------------------------------------------------------------
// Test if list matches the string and reconstruction matches original
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
TBool CGlxListTestBase< _List, _Reconstruction >::Equals( const TDesC8& aNewString, 
        const TNotification::TData* aNotificationList )
    {
    ASSERT( iList && iReconstruction );
    // Test if list contents match the new intended contents
    TBool fail = !Equals( aNewString );

    // Test notifications and reconstruction match
    fail |= !iReconstruction->NotificationListEquals( aNotificationList );
    
    return !fail;
    }
 
// -----------------------------------------------------------------------------
// Test match
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
TBool CGlxListTestBase< _List, _Reconstruction >
    ::Equals( const TDesC8& aStringWithoutStaticItems, 
        const TDesC8& aStringWithStaticItems, 
            TBool aStaticItemsEnabled, 
                const TNotification::TData* aNotificationList)
    {
    TBool fail = EFalse;
    if ( aStaticItemsEnabled )
        {
        fail |= !Equals( aStringWithStaticItems, aNotificationList );
        }
    else
        {
        fail |= !Equals( aStringWithoutStaticItems, aNotificationList );
        }
    
    return !fail;
    }

// -----------------------------------------------------------------------------
// Test if items are linked to media objects correctly
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::IsLinkedToMedia( const TDesC8& aAnswers ) const
    {
    TBool fail = EFalse;
    for ( TInt i = 0; i < aAnswers.Length(); i++)
        {
        TBool shouldBeLinked = ( aAnswers[ i ] == 't' ); 
        TBool isLinked = ( iList->Item( i ).Properties() != NULL );
        // Test via not, just in case true is not always exactly 1
        fail |= ( !shouldBeLinked != !isLinked );       
        }
    return !fail;
    }   
    
// -----------------------------------------------------------------------------
// Try linking to media item
// -----------------------------------------------------------------------------
//
/*template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::TryOfferMedia( TChar aId, const TDesC8& aAnswers, TBool aShouldAccept )
    {
    ASSERT( iList );

    CGlxMedia* media = new (ELeave) CGlxMedia( TGlxMediaId( aId ) );
    CleanupStack::PushL( media );
    iMediaPool.AppendL( media );
    CleanupStack::Pop( media );
    media->ReserveUsersL( 1 );
    // Offer media and test acceptance. Compare via not, to since True might be not be 1
    TBool fail = ( !iList->OfferMedia(KListIdSpace,  *media ) != !aShouldAccept );    
    fail |= !IsLinkedToMedia( aAnswers );
    return !fail;
    }*/

// -----------------------------------------------------------------------------
// Try removing link from media item
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::TryRemoveReference( TInt aIndex, const TDesC8& aAnswers )
    {
    ASSERT( iList );

    // Remove reference
    iList->RemoveReference( aIndex );
    TBool fail = !IsLinkedToMedia( aAnswers );
    return !fail;
    }

// -----------------------------------------------------------------------------
// Test OfferMedia(...)
// -----------------------------------------------------------------------------
//
/*template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >::TestOfferMediaL()
    {
    // Create the item list
    CreateListL( _L8("abcdefg") );

    EUNIT_ASSERT( IsLinkedToMedia( _L8("fffffff") ) );
    EUNIT_ASSERT( TryOfferMedia( '0', _L8("fffffff"), EFalse ) ); // reject
    EUNIT_ASSERT( TryOfferMedia( 'a', _L8("tffffff"), ETrue ) );  
    EUNIT_ASSERT( TryOfferMedia( 'c', _L8("tftffff"), ETrue ) );  
    EUNIT_ASSERT( TryOfferMedia( 'd', _L8("tfttfff"), ETrue ) );  
    EUNIT_ASSERT( TryOfferMedia( 'g', _L8("tfttfft"), ETrue ) );  
    EUNIT_ASSERT( TryOfferMedia( '1', _L8("tfttfft"), EFalse ) ); // reject
    }*/

// -----------------------------------------------------------------------------
// Test RemoveReference(...)
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >::TestRemoveReferenceL()
    {
    // Create the item list
    CreateListL( _L8("abcdefg"), _L8("tfttfft") );
    EUNIT_ASSERT( IsLinkedToMedia( _L8("tfttfft") ) );

    // Add static items
    // Static items are linked to media
    AddStaticItemL( '0', NGlxListDefs::EInsertFirst );
    AddStaticItemL( '1', NGlxListDefs::EInsertLast );
    EUNIT_ASSERT( IsLinkedToMedia( _L8("ttfttfftt") ) );

    EUNIT_ASSERT( TryRemoveReference( 1, _L8("tffttfftt") ) );
    EUNIT_ASSERT( TryRemoveReference( 3, _L8("tffftfftt") ) );
    EUNIT_ASSERT( TryRemoveReference( 4, _L8("tfffffftt") ) );
    EUNIT_ASSERT( TryRemoveReference( 7, _L8("tffffffft") ) );
    EUNIT_ASSERT_PANIC_DESC( TryRemoveReference( 8, _L8("tffffffft") ), 
                             "Images & videos", EGlxPanicIllegalArgument, 
                             "Incorrect panic" );
    }

// -----------------------------------------------------------------------------
// Test setting new contents
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::SetContentsL( const TDesC8& aString )
    {
    ASSERT( iList );

    // Create source
    CMPXCollectionPath* path = CreatePathLC( aString );
    // Copy source for comparison
    TInt originalIndex = path->Index();
    // Set the source 
    iList->SetContentsL( *path, *this );
    
    // Test path was not modified
    TBool fail = !( !path->Levels() || originalIndex == path->Index() );
    
    CleanupStack::PopAndDestroy( path );
    return !fail;
    }

// -----------------------------------------------------------------------------
// Test reordering contents
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::ReorderContentsL( const TDesC8& aString )
    {
    ASSERT( iList );

    // Create source
    CMPXCollectionPath* path = CreatePathLC( aString );
    // Copy source for comparison
    TInt originalIndex = path->Index();
    // Set the source 
    iList->ReorderContentsL( *path, *this );
    
    // Test path was not modified
    TBool fail = !( !path->Levels() || originalIndex == path->Index() );
    
    CleanupStack::PopAndDestroy( path );
    return !fail;
    }

// -----------------------------------------------------------------------------
// Test setting new contents
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::TrySetContentsL(  const TDesC8& aOldString, const TDesC8& aNewString )
    {
    // Create the item list
    CreateListL( aOldString );
    // Set the contents
    TBool fail = !SetContentsL( aNewString );
    // Test if list contents match the new intended contents
    fail |= !Equals( aNewString );
    
    return !fail;
    }
    
// -----------------------------------------------------------------------------
// Test setting contents and test notifications also
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::TrySetContentsL(  const TDesC8& aOldString, const TDesC8& aNewString, 
            const TNotification::TData* aNotificationList )
    {
    // Test list creation
    TBool fail = !TrySetContentsL( aOldString, aNewString );
    // Test if notification list matches expected
    fail |= !Equals( aNewString, aNotificationList );
    
    return !fail;
    }
    
// -----------------------------------------------------------------------------
// Test setting contents and test notifications also
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::TrySetContentsL(  const TDesC8& aOldString, const TDesC8& aNewString, 
            const TDesC8& aNewStringWithStaticItems, 
                const TNotification::TData* aNotificationList, 
                    TBool aStaticItemsEnabled, 
                        NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos )
    {
    // Create list
    CreateListL( aOldString, aStaticItemsEnabled, aStaticItemInfos );

    TBool fail = !SetContentsL( aNewString );
    fail |= !Equals( aNewString, aNewStringWithStaticItems, aStaticItemsEnabled,
        aNotificationList );
    
    return !fail;
    }
    
// -----------------------------------------------------------------------------
// Test setting contents and test notifications also with and without static items
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::TrySetContentsL( const TDesC8& aOldString, const TDesC8& aNewString, 
            const TDesC8& aNewStringWithStaticItems, 
                const TNotification::TData* aNotificationListStaticItemsEnabled,
                    const TNotification::TData* aNotificationListStaticItemsDisabled, 
                        NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos )
    {
    // without adding static items, while static items are enabled
    TBool fail = !TrySetContentsL( aOldString, aNewString, aNewString,
        aNotificationListStaticItemsDisabled, ETrue, NULL );

    // without adding static items, while static items are disabled
    fail |= !TrySetContentsL( aOldString, aNewString, aNewStringWithStaticItems, 
        aNotificationListStaticItemsDisabled, EFalse, NULL );
        
    // with adding static items, while static items are enabled
     fail |= !TrySetContentsL( aOldString, aNewString, aNewStringWithStaticItems,
        aNotificationListStaticItemsEnabled, ETrue, aStaticItemInfos );

    // with adding static items, while static items are disabled    
    fail |= !TrySetContentsL( aOldString, aNewString, aNewStringWithStaticItems, 
        aNotificationListStaticItemsDisabled, EFalse, aStaticItemInfos );
    
    return !fail;
    }

// -----------------------------------------------------------------------------
// Test Adding a static item
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::TryAddStaticItemL( const TDesC8& aInitString, TChar aStaticItemId, 
            NGlxListDefs::TInsertionPosition aInsertPos,
                const TDesC8& aStringAfter,
                    const TNotification::TData* aNotificationList,
                        TBool aStaticItemsEnabled,
                            NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos )
    {
    CreateListL( aInitString, aStaticItemsEnabled, aStaticItemInfos );
    CGlxMedia* media = new (ELeave) CGlxMedia( TGlxMediaId ( aStaticItemId ) );
    CleanupStack::PushL( media );
    iList->AddStaticItemL( media, aInsertPos );
    CleanupStack::Pop( media );

    TBool fail = !Equals( aInitString, aStringAfter, aStaticItemsEnabled,
        aNotificationList );
    
    return !fail;
    }

// -----------------------------------------------------------------------------
// Test Adding a static item
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::TryAddStaticItemL( const TDesC8& aInitString, 
            TChar aStaticItemId, NGlxListDefs::TInsertionPosition aInsertPos,
                const TDesC8& aStringAfter, const TNotification::TData* aNotificationListForAdd,
                    const TNotification::TData* aNotificationListForAllStatic,
                        NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos )
    {
    TNotification::TData noNotifications[] = 
                           { { END_MARKER } };
    
    // with adding static items, while static items are enabled
    TBool fail = !TryAddStaticItemL( aInitString, aStaticItemId, aInsertPos, 
        aStringAfter, aNotificationListForAdd, ETrue, aStaticItemInfos );

    // with adding static items, while static items are disabled    
    fail |= !TryAddStaticItemL( aInitString, aStaticItemId, aInsertPos,
        aInitString, noNotifications, EFalse, aStaticItemInfos );
        
    // turn on static items, to make sure items were added in disabled state
    iReconstruction->ClearNotifications();
    iList->SetStaticItemsEnabled( ETrue );
    fail |= !Equals( aStringAfter, aNotificationListForAllStatic );
    
    return !fail;
    }
            

// -----------------------------------------------------------------------------
// Test removing an item
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::TryRemoveL( const TDesC8& aOldString, const TChar aItemToRemove, 
            const TDesC8& aNewString, const TNotification::TData* aNotificationList )
    {
    CreateListL( aOldString );
    iList->Remove( KListIdSpace, TGlxMediaId( aItemToRemove ) );
    // Test if notification list matches expected
    return Equals( aNewString, aNotificationList );
    }

// -----------------------------------------------------------------------------
// Test removing an item
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::TryRemoveL( const TDesC8& aOldString, const TChar aItemToRemove, 
            TBool aIsStaticId, const TDesC8& aNewString, 
                    const TNotification::TData* aNotificationList,
                        TBool aStaticItemsEnabled,
                            NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos )
    {
    CreateListL( aOldString, aStaticItemsEnabled, aStaticItemInfos );

    iList->Remove( aIsStaticId ? KGlxStaticItemIdSpaceId : KListIdSpace, 
        TGlxMediaId( aItemToRemove ) );
    TBool fail = !Equals( aNewString, aNotificationList );
    
    return !fail;
    }

// -----------------------------------------------------------------------------
// Test setting contents and test notifications also
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
TBool CGlxListTestBase< _List, _Reconstruction >
    ::TryRemoveL( const TDesC8& aOldString, const TChar aItemToRemove, TBool aIsStaticId, 
        const TDesC8& aNewStringWithoutStaticItems, 
            const TDesC8& aNewStringWithStaticItems, 
                const TNotification::TData* aNotificationListStaticItemsDisabled,
                    const TNotification::TData* aNotificationListStaticItemsEnabled, 
                        NGlxListTestHelpers::TStaticItemInfo::TData* aStaticItemInfos)
    {
    // without adding static items, while static items are enabled
    TBool fail = !TryRemoveL( aOldString, aItemToRemove, aIsStaticId, 
        aNewStringWithoutStaticItems, aNotificationListStaticItemsDisabled, 
            ETrue, NULL );

    // without adding static items, while static items are disabled
    fail |= !TryRemoveL( aOldString, aItemToRemove, aIsStaticId, 
        aNewStringWithoutStaticItems, aNotificationListStaticItemsDisabled, 
            EFalse, NULL );
        
    // with adding static items, while static items are enabled
     fail |= !TryRemoveL( aOldString, aItemToRemove, aIsStaticId, 
         aNewStringWithStaticItems, aNotificationListStaticItemsEnabled, 
            ETrue, aStaticItemInfos );

    // with adding static items, while static items are disabled    
    fail |= !TryRemoveL( aOldString, aItemToRemove, aIsStaticId, 
        aNewStringWithoutStaticItems, aNotificationListStaticItemsDisabled, 
            EFalse, aStaticItemInfos );
    
    return !fail;
    }

// -----------------------------------------------------------------------------
// MGlxMediaPool::Media implementation
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    CMPXCollectionPath* CGlxListTestBase< _List, _Reconstruction >
        ::CreatePathLC( const TDesC8& aString )
    {
    // Populate array from the string
    RArray<TMPXItemId> ids;
    CleanupClosePushL( ids );
    TInt count = aString.Length();
    for ( TInt i = 0; i < count; i++ )
        {
        ids.AppendL( aString[ i ] );
        }
    // Populate path from the array
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( ids.Array() );
    CleanupStack::Pop( path ); // ids
    CleanupStack::PopAndDestroy(); // ids
    CleanupStack::PushL( path );
    return path;
    }
    
// -----------------------------------------------------------------------------
// Add a static item
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >::AddStaticItemL( 
        TChar aId, NGlxListDefs::TInsertionPosition aPosition )
    {
    ASSERT( iList );
    CGlxMedia* item = new (ELeave) CGlxMedia( TGlxMediaId( aId ) );
    CleanupStack::PushL( item );
    iList->AddStaticItemL( item, aPosition );
    CleanupStack::Pop( item );
    }

// -----------------------------------------------------------------------------
// Add a static items
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >
        ::AddStaticItemsL( NGlxListTestHelpers::TStaticItemInfo::TData* aInfos )
    {
    if ( aInfos )
        {
        TInt i = 0;
        while ( NGlxListTestHelpers::KEndMarker != aInfos[ i ] )
            {
            AddStaticItemL( aInfos[ i ].iId, aInfos[ i ].iPosition );
            i++; 
            }
        }
    }
    
// -----------------------------------------------------------------------------
// Check if counts equal
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >::CountEqualsL( TInt aAll, 
        TInt aNonStatic, TInt aAllStatic, TInt aPreStatic, TInt aPostStatic )
    {
    TBool enabled = iList->IsStaticItemsEnabled();

    // static items disabled
    iList->SetStaticItemsEnabled( EFalse );
    TBool fail = ( iList->Count( NGlxListDefs::ECountAll ) != aNonStatic );
    fail |= ( iList->Count( NGlxListDefs::ECountNonStatic ) != aNonStatic );
    fail |= ( iList->Count( NGlxListDefs::ECountAllStatic ) != 0 );
    fail |= ( iList->Count( NGlxListDefs::ECountPreStatic ) != 0 );
    fail |= ( iList->Count( NGlxListDefs::ECountPostStatic ) != 0 );
    fail |= ( iList->Count() != iList->Count( NGlxListDefs::ECountAll ) );
    
    // static items enabled
    iList->SetStaticItemsEnabled( ETrue );
    fail |= ( iList->Count( NGlxListDefs::ECountAll ) != aAll );
    fail |= ( iList->Count( NGlxListDefs::ECountNonStatic ) != aNonStatic );
    fail |= ( iList->Count( NGlxListDefs::ECountAllStatic ) != aAllStatic );
    fail |= ( iList->Count( NGlxListDefs::ECountPreStatic ) != aPreStatic );
    fail |= ( iList->Count( NGlxListDefs::ECountPostStatic ) != aPostStatic );
    fail |= ( iList->Count() != iList->Count( NGlxListDefs::ECountAll ) );

    // Reset to original state
    iList->SetStaticItemsEnabled( enabled );
    return !fail;
    }
    
// -----------------------------------------------------------------------------
// Try enabling static items
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    TBool CGlxListTestBase< _List, _Reconstruction >
        ::TrySetStaticItemsEnabledL( const TDesC8& aStringWithoutStaticItems, 
            const TDesC8& aStringWithStaticItems, 
                NGlxListTestHelpers::TStaticItemInfo::TData* aInfos,
                    const TNotification::TData* aNotificationListForEnable,
                        const TNotification::TData* aNotificationListForDisable )
    {
    CreateListL( aStringWithoutStaticItems );
    AddStaticItemsL( aInfos );
    iList->SetStaticItemsEnabled( EFalse );
    TBool fail = iList->IsStaticItemsEnabled();
    // test enabling static items
    iReconstruction->ClearNotifications();
    iList->SetStaticItemsEnabled( ETrue );
    fail |= !iList->IsStaticItemsEnabled();
    fail |= !Equals( aStringWithStaticItems, aNotificationListForEnable );
    // test disabling static items 
    iReconstruction->ClearNotifications();
    iList->SetStaticItemsEnabled( EFalse );
    fail |= iList->IsStaticItemsEnabled();
    fail |= !Equals( aStringWithoutStaticItems, aNotificationListForDisable );
    
    return !fail;
    }
    
// -----------------------------------------------------------------------------
// Test Count(...)
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >::TestCountL()
    {
    // Test list with no normal items
    CreateListL( _L8("") );
    EUNIT_ASSERT( CountEqualsL( 0, 0, 0, 0, 0 ) ); // All, non-static, static, pre-static, post-static

    AddStaticItemL( '0', NGlxListDefs::EInsertFirst );
    EUNIT_ASSERT( CountEqualsL( 1, 0, 1, 1, 0 ) ); // All, non-static, static, pre-static, post-static

    AddStaticItemL( '1', NGlxListDefs::EInsertFirst );
    EUNIT_ASSERT( CountEqualsL( 2, 0, 2, 2, 0 ) ); // All, non-static, static, pre-static, post-static

    AddStaticItemL( '2', NGlxListDefs::EInsertLast );
    EUNIT_ASSERT( CountEqualsL( 3, 0, 3, 2, 1 ) ); // All, non-static, static, pre-static, post-static

    // Test list with normal items
    CreateListL( _L8("abcdefg") );
    // test items
    EUNIT_ASSERT( CountEqualsL( 7, 7, 0, 0, 0 ) ); // All, non-static, static, pre-static, post-static
    
    AddStaticItemL( '0', NGlxListDefs::EInsertFirst );
    EUNIT_ASSERT( CountEqualsL( 8, 7, 1, 1, 0 ) ); // All, non-static, static, pre-static, post-static

    AddStaticItemL( '1', NGlxListDefs::EInsertFirst );
    EUNIT_ASSERT( CountEqualsL( 9, 7, 2, 2, 0 ) ); // All, non-static, static, pre-static, post-static

    AddStaticItemL( '2', NGlxListDefs::EInsertLast );
    EUNIT_ASSERT( CountEqualsL( 10, 7, 3, 2, 1 ) ); // All, non-static, static, pre-static, post-static
    }
    
// -----------------------------------------------------------------------------
// Test Item(...)
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >::TestItemL(  )
    {
    // Test list with no normal items
    CreateListL( _L8("") );
    AddStaticItemL( '0', NGlxListDefs::EInsertFirst );
    EUNIT_ASSERT( iList->Item(0).Id() == TGlxMediaId( '0' ) );

    AddStaticItemL( '1', NGlxListDefs::EInsertFirst );
    EUNIT_ASSERT( iList->Item(0).Id() == TGlxMediaId( '1' ) );
    EUNIT_ASSERT( iList->Item(1).Id() == TGlxMediaId( '0' ) );

    AddStaticItemL( '2', NGlxListDefs::EInsertLast );
    EUNIT_ASSERT( iList->Item(0).Id() == TGlxMediaId( '1' ) );
    EUNIT_ASSERT( iList->Item(1).Id() == TGlxMediaId( '0' ) );
    EUNIT_ASSERT( iList->Item(2).Id() == TGlxMediaId( '2' ) );

    // Test list with normal items
    CreateListL( _L8("abcdefg") );
    // test items
    EUNIT_ASSERT( iList->Item(0).Id() == TGlxMediaId( 'a' ) );
    EUNIT_ASSERT( iList->Item(2).Id() == TGlxMediaId( 'c' ) );
    EUNIT_ASSERT( iList->Item(6).Id() == TGlxMediaId( 'g' ) );
    
    AddStaticItemL( '0', NGlxListDefs::EInsertFirst );
    EUNIT_ASSERT( iList->Item(0).Id() == TGlxMediaId( '0' ) );
    EUNIT_ASSERT( iList->Item(1).Id() == TGlxMediaId( 'a' ) );
    EUNIT_ASSERT( iList->Item(3).Id() == TGlxMediaId( 'c' ) );
    EUNIT_ASSERT( iList->Item(7).Id() == TGlxMediaId( 'g' ) );

    AddStaticItemL( '1', NGlxListDefs::EInsertFirst );
    EUNIT_ASSERT( iList->Item(0).Id() == TGlxMediaId( '1' ) );
    EUNIT_ASSERT( iList->Item(1).Id() == TGlxMediaId( '0' ) );
    EUNIT_ASSERT( iList->Item(2).Id() == TGlxMediaId( 'a' ) );
    EUNIT_ASSERT( iList->Item(4).Id() == TGlxMediaId( 'c' ) );
    EUNIT_ASSERT( iList->Item(8).Id() == TGlxMediaId( 'g' ) );

    AddStaticItemL( '2', NGlxListDefs::EInsertLast );
    EUNIT_ASSERT( iList->Item(0).Id() == TGlxMediaId( '1' ) );
    EUNIT_ASSERT( iList->Item(1).Id() == TGlxMediaId( '0' ) );
    EUNIT_ASSERT( iList->Item(2).Id() == TGlxMediaId( 'a' ) );
    EUNIT_ASSERT( iList->Item(4).Id() == TGlxMediaId( 'c' ) );
    EUNIT_ASSERT( iList->Item(8).Id() == TGlxMediaId( 'g' ) );
    EUNIT_ASSERT( iList->Item(9).Id() == TGlxMediaId( '2' ) );
    }
    
// -----------------------------------------------------------------------------
// Test Index(...)
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >::TestIndexL()
    {
    // Test list with no normal items
    CreateListL( _L8("") );
    AddStaticItemL( '0', NGlxListDefs::EInsertFirst );
    EUNIT_ASSERT( iList->Index(KGlxStaticItemIdSpaceId, TGlxMediaId( '0' ) ) == 0 );

    AddStaticItemL( '1', NGlxListDefs::EInsertFirst );
    EUNIT_ASSERT( iList->Index(KGlxStaticItemIdSpaceId, TGlxMediaId( '1' ) ) == 0 );
    EUNIT_ASSERT( iList->Index(KGlxStaticItemIdSpaceId, TGlxMediaId( '0' ) ) == 1 );

    AddStaticItemL( '2', NGlxListDefs::EInsertLast );
    EUNIT_ASSERT( iList->Index(KGlxStaticItemIdSpaceId, TGlxMediaId( '1' ) ) == 0 );
    EUNIT_ASSERT( iList->Index(KGlxStaticItemIdSpaceId, TGlxMediaId( '0' ) ) == 1 );
    EUNIT_ASSERT( iList->Index(KGlxStaticItemIdSpaceId, TGlxMediaId( '2' ) ) == 2 );

    // Test list with normal items
    CreateListL( _L8("abcdefg") );
    EUNIT_ASSERT( iList->Index(KListIdSpace, TGlxMediaId( 'a' ) ) == 0 );
    EUNIT_ASSERT( iList->Index(KListIdSpace, TGlxMediaId( 'c' ) ) == 2 );
    EUNIT_ASSERT( iList->Index(KListIdSpace, TGlxMediaId( 'g' ) ) == 6 );
    
    AddStaticItemL( '0', NGlxListDefs::EInsertFirst );
    EUNIT_ASSERT( iList->Index(KGlxStaticItemIdSpaceId, TGlxMediaId( '0' ) ) == 0 );
    EUNIT_ASSERT( iList->Index(KListIdSpace, TGlxMediaId( 'a' ) ) == 1 );
    EUNIT_ASSERT( iList->Index(KListIdSpace, TGlxMediaId( 'c' ) ) == 3 );
    EUNIT_ASSERT( iList->Index(KListIdSpace, TGlxMediaId( 'g' ) ) == 7 );

    AddStaticItemL( '1', NGlxListDefs::EInsertFirst );
    EUNIT_ASSERT( iList->Index(KGlxStaticItemIdSpaceId, TGlxMediaId( '1' ) ) == 0 );
    EUNIT_ASSERT( iList->Index(KGlxStaticItemIdSpaceId, TGlxMediaId( '0' ) ) == 1 );
    EUNIT_ASSERT( iList->Index(KListIdSpace, TGlxMediaId( 'a' ) ) == 2 );
    EUNIT_ASSERT( iList->Index(KListIdSpace, TGlxMediaId( 'c' ) ) == 4 );
    EUNIT_ASSERT( iList->Index(KListIdSpace, TGlxMediaId( 'g' ) ) == 8 );

    AddStaticItemL( '2', NGlxListDefs::EInsertLast );
    EUNIT_ASSERT( iList->Index(KGlxStaticItemIdSpaceId, TGlxMediaId( '1' ) ) == 0 );
    EUNIT_ASSERT( iList->Index(KGlxStaticItemIdSpaceId, TGlxMediaId( '0' ) ) == 1 );
    EUNIT_ASSERT( iList->Index(KListIdSpace, TGlxMediaId( 'a' ) ) == 2 );
    EUNIT_ASSERT( iList->Index(KListIdSpace, TGlxMediaId( 'c' ) ) == 4 );
    EUNIT_ASSERT( iList->Index(KListIdSpace, TGlxMediaId( 'g' ) ) == 8 );
    EUNIT_ASSERT( iList->Index(KGlxStaticItemIdSpaceId, TGlxMediaId( '2' ) ) == 9 );
    }
    
// -----------------------------------------------------------------------------
// Test IdSpaceId
// -----------------------------------------------------------------------------
//
template < class _List, class _Reconstruction >
    void CGlxListTestBase< _List, _Reconstruction >::TestIdSpaceIdL()
    {
    CreateReconstructionL();
    iList = _List::NewL( KListIdSpace, *iReconstruction, *this );
    EUNIT_ASSERT( iList->IdSpaceId() == TGlxIdSpaceId( 1 ) );
    }    
