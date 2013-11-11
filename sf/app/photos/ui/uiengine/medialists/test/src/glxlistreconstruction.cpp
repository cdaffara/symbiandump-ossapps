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
* Description:   Class that reconstructs a list from notifications
*
*/



//  CLASS HEADER
#include "glxlistreconstruction.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>
#include <mpxcollectionpath.h>
    
//  INTERNAL INCLUDES

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
void CGlxListReconstructionBase::ConstructL( TInt aReservationCount )
    {
    iNotifications.ReserveL( aReservationCount );
    iReconstructedList.ReserveL( aReservationCount );
    iFocusIndex = -1;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxListReconstructionBase::~CGlxListReconstructionBase()
    {
    iNotifications.Close();
    iReconstructedList.Close();
    iSelectedItemIndices.Close();
    }
    
// -----------------------------------------------------------------------------
// Clear notifications array
// -----------------------------------------------------------------------------
//
void CGlxListReconstructionBase::ClearNotifications()
    {
    iNotifications.Reset();
    }

// -----------------------------------------------------------------------------
// Handle items being added
// -----------------------------------------------------------------------------
//
void CGlxListReconstructionBase::HandleItemsAdded( TInt aAddedAtIndex, TInt aCount ) 
    {
    TNotification::TData notification[] = { TNotification::EAdd, aAddedAtIndex, aCount };
    // If fails, not big enough reservation
    EUNIT_ASSERT( KErrNone == iNotifications.Append( *notification )  ); 
    
    // reconstruct the list
    for ( TInt i = 0; i < aCount; i++ )
        {
        // If fails, not big enough reservation
        EUNIT_ASSERT( KErrNone == iReconstructedList.Insert( 
            OriginalItem( aAddedAtIndex ), aAddedAtIndex ) );
        }
    
    // Move focus
    if ( KErrNotFound == iFocusIndex )
        {
        iFocusIndex = 0;
        }
    else if ( iFocusIndex >= aAddedAtIndex )
        {
        iFocusIndex += aCount;
        }
    
    // Move selection        
    for ( TInt i = 0; i < iSelectedItemIndices.Count(); i++ )
        {
        if ( iSelectedItemIndices[ i ] >= aAddedAtIndex )
            {
            iSelectedItemIndices[ i ] += aCount;
            }
        }
    
    // Make sure the original and reconstruction match
    EUNIT_ASSERT( ReconstructionEquals() );
    }
    
// -----------------------------------------------------------------------------
// Handle items being removed
// -----------------------------------------------------------------------------
//
void CGlxListReconstructionBase::HandleItemsRemoved( TInt aRemovedFromIndex, TInt aCount ) 
    {
    TNotification::TData notification[] = { TNotification::ERemove, aRemovedFromIndex, aCount };
    // If fails, not big enough reservation
    EUNIT_ASSERT( KErrNone == iNotifications.Append( *notification ) ); 
    // reconstruct the list
    for ( TInt i = 0; i < aCount; i++ )
        {
        iReconstructedList.Remove( aRemovedFromIndex );
        }

    // Move focus
    if ( iFocusIndex >= aRemovedFromIndex + aCount )
        {
        iFocusIndex -= aCount;
        }
    else if ( iFocusIndex >= aRemovedFromIndex )
        {
        // Focused item was removed
        iFocusIndex = aRemovedFromIndex - 1;
        if ( iFocusIndex < 0 )
            {
            iFocusIndex = 0;
            }
        }
    if ( !iReconstructedList.Count() )
        {
        iFocusIndex = KErrNotFound;
        }
    
    // Move/remove selection        
    TInt i = 0;
    while ( i < iSelectedItemIndices.Count() )
        {
        if ( iSelectedItemIndices[ i ] >= aRemovedFromIndex + aCount )
            {
            iSelectedItemIndices[ i ] -= aCount;
            }
        else if ( iSelectedItemIndices[ i ] >= aRemovedFromIndex )
            {
            iSelectedItemIndices.Remove( i );
            continue;
            }
        i++;
        }

    // Make sure the original and reconstruction match
    EUNIT_ASSERT( ReconstructionEquals() );
    }
    
// -----------------------------------------------------------------------------
// Handle focus changed
// -----------------------------------------------------------------------------
//
void CGlxListReconstructionBase::HandleFocusChanged( NGlxListDefs::TFocusChangeType aType, 
            TInt aNewIndex, TInt aOldIndex )
    {
    TNotification::TData notification[] = { TNotification::EFocus, aOldIndex, static_cast< TInt >( aType ) };
    // If fails, not big enough reservation
    EUNIT_ASSERT( iNotifications.Append( *notification ) == KErrNone ); 

    iFocusIndex = aNewIndex;

    // Make sure the original and reconstruction match
    EUNIT_ASSERT( ReconstructionEquals() );
    }
        
// -----------------------------------------------------------------------------
// Handle items selected
// -----------------------------------------------------------------------------
//
void CGlxListReconstructionBase::HandleItemSelected( TInt aIndex, TBool aSelected )
    {
    TNotification::TData notification[] = { TNotification::ESelection, aIndex, aSelected };
    // If fails, not big enough reservation
    EUNIT_ASSERT( iNotifications.Append( *notification ) == KErrNone ); 

    // Should not get a notification that causes no change
    if ( aSelected )
        {
        EUNIT_ASSERT( KErrNotFound == iSelectedItemIndices.Find( aIndex ) );
        iSelectedItemIndices.Append( aIndex );
        }
    else
        {
        TInt index = iSelectedItemIndices.Find( aIndex );
        EUNIT_ASSERT( KErrNotFound != index );
        iSelectedItemIndices.Remove( index );
        }

    EUNIT_ASSERT( ReconstructionEquals() );
    }
    
// -----------------------------------------------------------------------------
// Test if notifications match
// -----------------------------------------------------------------------------
//
TBool CGlxListReconstructionBase::NotificationListEquals( const TNotification::TData* aNotificationList )
    {
    ASSERT( aNotificationList );
    
    TBool fail = EFalse;
    // Iterator through notifications list
    TBool markerFound = EFalse;
    TInt i = 0;
    while ( ETrue )
        {
        if ( i == iNotifications.Count() )
            {
            if ( KEndMarker == aNotificationList[ i ] )
                {
                markerFound = ETrue;
                }
            break;
            }
        // Notifications must match
        const TNotification& notification = iNotifications[ i ]; 
        fail |= ( notification != aNotificationList[ i ] );
        i++;
        }
    // Must have reached the end of iNotifications
    fail |= ( i != iNotifications.Count() ); 
    // Must have found a marker
    fail |= !markerFound;
    
    return !fail;
    }

