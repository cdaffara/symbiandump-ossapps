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
* Description:    Garbage collector
*
*/




#include "glxgarbagecollector.h"

#include <glxassert.h>
#include <glxtracer.h>
#include <glxlog.h>

#include "glxcache.h"
#include "glxerrormanager.h"
#include "glxmedia.h"
#include "glxmedialist.h"
#include "mglxmediauser.h"
#include "glxthumbnailutility.h"

/**
 * Interval for the periodic timer, in microseconds
 */
const TInt KPeriodicInterval = 500000;
/**
 * Start Delay for the periodic timer, in microseconds
 */
const TInt KPeriodicStartDelay = 300000;   // It is Changed to 300Micro Second ; To be able to give Others a Chance to Run before it starts
/** 
 * Max number of items to scan per periodic callback
 * @todo Find optimal value for this
 */
const TInt KMaxScannedMediaCountPerPeriodicCallback = 15;  // Changed from 30; Still there is Scope for Nominal Value Or Dynamic Value

/**
 * Max number of attributes that an item may have in use
 */
const TInt KMaxAttributesInUse = 50;

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CGlxGarbageCollector* CGlxGarbageCollector::NewL( 
        const RPointerArray<CGlxCache>& aCaches )
    {
    TRACER( "CGlxGarbageCollector::NewL" );

    CGlxGarbageCollector* self = new( ELeave ) CGlxGarbageCollector( aCaches );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxGarbageCollector::CGlxGarbageCollector( 
    const RPointerArray<CGlxCache>& aCaches )
        : iCaches( aCaches )
    {
    TRACER("CGlxGarbageCollector::Default Constructor");
    }
    
// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxGarbageCollector::ConstructL()
    {
    TRACER("CGlxGarbageCollector::ConstructL");
    
    // Create callback loop in the lowest priority, so that the garbage
    // collection does not slow down the application
    iPeriodic = CPeriodic::NewL( CActive::EPriorityIdle );
    iAttributesInUse.ReserveL(KMaxAttributesInUse);
    }
                
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxGarbageCollector::~CGlxGarbageCollector()
    {
    TRACER("CGlxGarbageCollector::Destructor");
    delete iPeriodic;
    iAttributesInUse.Close();
    }

// -----------------------------------------------------------------------------
// CleanupL
// -----------------------------------------------------------------------------
//
void CGlxGarbageCollector::CleanupL()
    {
    TRACER("CGlxGarbageCollector::CleanupL");
    
    if ( !iPeriodic->IsActive() )
        {
        iPeriodic->Start( KPeriodicStartDelay, KPeriodicInterval, TCallBack( &PeriodicCallbackL, static_cast<TAny*>( this ) ) );
        }
    else
        {
        // a full cleanup round is required to make the cache fully clean,
        // if cleanup is currently ongoing. we don't know whether the cache
        // was modified before or after the current (cache or media) index being
        // cleaned up.
        iRequiresFullCleanupRound = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CancelCleanup
// -----------------------------------------------------------------------------
//
void CGlxGarbageCollector::CancelCleanup()
    {
    TRACER("CGlxGarbageCollector::CancelCleanup");
    
    if (iPeriodic->IsActive())
        {
        iPeriodic->Cancel();
        // Consideration of Restarting the timer is there; 
        }
    }

// -----------------------------------------------------------------------------
// Callback from periodic timer
// -----------------------------------------------------------------------------
//
TInt CGlxGarbageCollector::PeriodicCallbackL( TAny* aPtr )
    {
    TRACER("CGlxGarbageCollector::PeriodicCallback");
    
    GLX_ASSERT_DEBUG( aPtr != NULL, Panic( EGlxPanicLogicError ), 
        "Received null pointer for garbage collector" );
        
    // get "this" pointer
    static_cast< CGlxGarbageCollector* >( aPtr )->PeriodicCallbackL();

    // return value ignored for periodic timers
    return 0;
    }

// -----------------------------------------------------------------------------
// Flush Count Pages
// -----------------------------------------------------------------------------
//
void CGlxGarbageCollector::FlushPagesL(TInt aCount)
    {
    TRACER("CGlxGarbageCollector::FlushPagesL");
    TBool reachedEnd = CleanupCaches(aCount);
    
    if ( reachedEnd )
        {
        // reset to the start of the caches
        iScanningPosition.iCurrentCacheIndex = 0;
        iScanningPosition.iNextMediaIndexToCleanup = 0;
        
        // next time the full round reaches end, the caches will be fully clean
        // (unless client calls Cleanup() )
        iRequiresFullCleanupRound = EFalse;
        }
    }
// -----------------------------------------------------------------------------
// Callback from periodic timer
// inlined in cpp only => inlines for arm
// -----------------------------------------------------------------------------
//
inline void CGlxGarbageCollector::PeriodicCallbackL()
    {
    TRACER("CGlxGarbageCollector::PeriodicCallbackL");
    
    TBool reachedEnd = CleanupCaches();
    
    // determine whether idle callback should be cancelled (note that check 
    // needs to be done before resetting iRequiresFullCleanupRound below)
    // do not cancel if reached the end, but cache is dirty
    if ( reachedEnd && !iRequiresFullCleanupRound )
        {
        iPeriodic->Cancel();
        }

    // if scanned to the end of caches, set up for the next cleanup round
    if ( reachedEnd )
        {
        // reset to the start of the caches
        iScanningPosition.iCurrentCacheIndex = 0;
        iScanningPosition.iNextMediaIndexToCleanup = 0;
        
        // next time the full round reaches end, the caches will be fully clean
        // (unless client calls Cleanup() )
        iRequiresFullCleanupRound = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// Clean up all caches
// -----------------------------------------------------------------------------
//
TBool CGlxGarbageCollector::CleanupCaches(TInt aCount)
    {
    TRACER( "CGlxGarbageCollector::CleanupCaches" );

    TInt remainingScanCount = 0;

    // set the maximum number of items to scan during this call
    // Count is Needed for Flushing Direct
    remainingScanCount = aCount*KMaxScannedMediaCountPerPeriodicCallback;
    
    // Iterate through all cache until scanned enough items (remainingScanCount)
    // (unlikely to have many caches, so ok to call RPointerArray::Count() on 
    // each iteration)
    while ( iScanningPosition.iCurrentCacheIndex < iCaches.Count() )
        {
        // clean up current cache
        remainingScanCount = CleanupCache( 
            *iCaches[iScanningPosition.iCurrentCacheIndex], remainingScanCount );
            
        // exit the loop if reached full scan count, since the above loop might
        // have reached the end of the cache. (so don't increment the current
        // cache index)
        if ( 0 == remainingScanCount )
            {
            break;
            }
        
        // set indexes to the beginning of the next cache
        iScanningPosition.iCurrentCacheIndex++;
        iScanningPosition.iNextMediaIndexToCleanup = 0; 
        }
    
    // determine if there is anything more to clean up
    TBool reachedEndOfAllCaches = 
        ( iScanningPosition.iCurrentCacheIndex == iCaches.Count() );
    return reachedEndOfAllCaches;
    }

// -----------------------------------------------------------------------------
// Clean up cache that is currently under cleanup
// return number of items yet to scan during this idle callback
// -----------------------------------------------------------------------------
//
TInt CGlxGarbageCollector::CleanupCache( CGlxCache& aCache,     
        TInt aRemainingScanCount )
    {
    TRACER( "CGlxGarbageCollector::CleanupCache" );

    // set the maximum number of items to scan during this call
    TInt remainingScanCount = aRemainingScanCount;
    
    // don't store cache.Count() locally, as the count changes during the loop
    while ( iScanningPosition.iNextMediaIndexToCleanup < aCache.Count() &&
            remainingScanCount > 0 && ( aCache.IdSpaceId() != KGlxIdSpaceIdRoot ) )
        {
        CGlxMedia& media = aCache.Media( iScanningPosition.iNextMediaIndexToCleanup );
        
        if ( media.UserCount() > 0 )
            {
            // the media object still has users. Check if any attributes
            // it has can be deleted.
            TRAPD( err, CleanupMediaL( aCache, media ) );
            
            // skip the media object if cleanup failed. This allows cleanup to 
            // continue also when has no free memory. Skip also if there
            // are still attributes in use.
            if ( ( media.Count() == 0 && !err ) || ( err > 0 ) )
                {
                // the media object has no more attributes. It can be deleted.
                // don't increment iNextMediaIndexToCleanup since the index
                // of the next item will be decreased by one due to removal
                aCache.Delete( iScanningPosition.iNextMediaIndexToCleanup );
                GLX_LOG_INFO1( "CGlxGarbageCollector - Deleted Media Id=%d since no attributes left", media.Id().Value());
                }
            else if( err < 0 )
                {
                // scan next item
                iScanningPosition.iNextMediaIndexToCleanup++;                
                }
            else 
                {
                // scan next item
                iScanningPosition.iNextMediaIndexToCleanup++;                
                }
            }
        else
            {
            // the media object has no more users in present cache. It may be deleted
            if( !MediaInUse( media ) )
	            {
	            // Bug Fix @ EDDG-7V3CJA:: Recheck other caches for UserCount 
	            aCache.Delete( iScanningPosition.iNextMediaIndexToCleanup );
	            GLX_LOG_INFO1( "CGlxGarbageCollector - Deleted Media Id=%d since no users left", media.Id().Value());
	            }            
            }            
        remainingScanCount--;
        }

    return remainingScanCount;
    }
    
// -----------------------------------------------------------------------------
// Perform cleanup on provided media object
// -----------------------------------------------------------------------------
//
void CGlxGarbageCollector::CleanupMediaL( CGlxCache& aCache, CGlxMedia& aMedia )
    {
    TRACER("CGlxGarbageCollector::CleanupMediaL");
    
    // check which attributes can be deleted.
    iAttributesInUse.Reset();

    GetAttributesInUseL( aMedia, iAttributesInUse );
    if ( iAttributesInUse.Count() == 0 )
        {
        User::Leave(1);
        }
    GlxErrorManager::ClearExpiredAndUnusedErrorsL( aMedia, iAttributesInUse );
    
    // add the error attribute to the list of attributes that are in use, so
    // that it won't be deleted by the loop below.
    // (if there are no errors left, GlxErrorManager::ClearExpiredAndNotInUseErrorsL 
    // would have deleted the error attribute from aMedia, so checking for it
    // does no harm)
    iAttributesInUse.AppendL( GlxErrorManager::ErrorAttribute() );
    
    // delete all attributes that are not being used
    DeleteOtherAttributes( aCache, aMedia, iAttributesInUse );
    }

// -----------------------------------------------------------------------------
// GetAttributesInUseL
// -----------------------------------------------------------------------------
//
void CGlxGarbageCollector::GetAttributesInUseL( const CGlxMedia& aMedia, 
        RArray<TMPXAttribute>& aAttributes ) const
    {
    TRACER("CGlxGarbageCollector::GetAttributesInUseL");
    
    // get needed attributes from each user of the media object
    // ( unlikely to have many users, so calling CGlxMedia::UserCount and 
    // CGlxMedia::Id within the loop )
    for ( TInt userIndex = 0; userIndex < aMedia.UserCount(); userIndex++ )
        {
        aMedia.User( userIndex ).GetRequiredAttributesL( aMedia.IndexInUser( userIndex ), 
            aAttributes );
        }
    }

// -----------------------------------------------------------------------------
// Delete all attributes from a media object except those specified
// -----------------------------------------------------------------------------
//
void CGlxGarbageCollector::DeleteOtherAttributes( CGlxCache& /*aCache*/, CGlxMedia& aMedia, 
        const RArray<TMPXAttribute>& aAttributesToKeep ) const
    {
    TRACER("CGlxGarbageCollector::DeleteOtherAttributes");
    
    // loop backwards so can delete attributes during the loop
    for ( TInt attrIndex = aMedia.Count() - 1; attrIndex >= 0; attrIndex-- )
        {
        // delete the attribute if it is not in use
        if ( KErrNotFound == aAttributesToKeep.Find( aMedia.Attribute( attrIndex ), TMPXAttribute::Match ) )
            {
            GLX_LOG_INFO( "CGlxGarbageCollector::CleanupMediaL - Deleted attribute" );
            aMedia.DeleteAttribute( attrIndex );
            }
        }
    }
    
// -----------------------------------------------------------------------------
// Check for User Count for media to be deleted in the remaining caches
// -----------------------------------------------------------------------------
//    
TBool CGlxGarbageCollector::MediaInUse(const CGlxMedia&  aMedia) const
	{
	// Bug Fix @ EDDG-7V3CJA :: If media has non zero user count in any of the caches
	// then deletion of texture is to be avoided.	
	for(TInt cacheCount = 0; cacheCount < iCaches.Count(); cacheCount++ )
		{		
		if ( ( iCaches[cacheCount]->Media( aMedia.Id() ) ) 
				&& ( iCaches[cacheCount]->IdSpaceId() != KGlxIdSpaceIdRoot ) )
			{
			if ( ( iCaches[cacheCount]->Media(aMedia.Id() ) )->UserCount() > 0 )
				{
				return ETrue;		
				}
			}
		}
	return EFalse;	
    }
