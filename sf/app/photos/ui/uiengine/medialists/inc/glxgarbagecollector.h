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





#ifndef C_GLXGARBAGECOLLECTOR_H
#define C_GLXGARBAGECOLLECTOR_H

#include <e32base.h>


class CGlxMedia;
class CGlxCache;
class T_CGlxGarbageCollector;
class TMPXAttribute;
/**
 * CGlxGarbageCollector
 *
 * Garbage collector for the media item cache
 *
 * @lib glxmedialists.lib
 */
class CGlxGarbageCollector : public CBase
    {
public:
    /**
     * Two-phase constructor
     * @return new garbage collector instance
     */
    static CGlxGarbageCollector* NewL( const RPointerArray<CGlxCache>& aCaches );
	
    /**
     * Destructor
     */
    ~CGlxGarbageCollector();

    /**
     * Start background garbage collection
     */
    void CleanupL();
    
    /**
     * Cancel background garbage collection
     */
    void CancelCleanup();
    
    /**
     * Flush Some Pages As System Reach Low Memory Limit
     * pointer to be used
     */
    void FlushPagesL(TInt aCount);
    
private:
    /**
     * Constructor
     */
    CGlxGarbageCollector( const RPointerArray<CGlxCache>& aCaches );
	
    /**
     * Second-phase constructor
     */
    void ConstructL();
    
    /**
     * Callback for periodic timer
     * @see TCallBack
     */
    static TInt PeriodicCallbackL( TAny* aPtr );
    
    /**
     * Callback for periodic timer, non-static, and hence allows this
     * pointer to be used
     */
    void PeriodicCallbackL();

    /**
     * Perform garbage collection on all caches
     * Only cleans up only a certain number of items each call, to avoid
     * this call taking a very long time, as this will be called
     * from the periodic timer callback
     * @param aCount 
     * @return ETrue if reached the end of caches; EFalse if not
     */
    TBool CleanupCaches(TInt aCount = 1);

    /**
     * Perform garbage collection on specified cache
     * Only cleans up only aRemainingScanCount amount of items.
     * @param aRemainingScanCount maximum number of items to scan
     * @return the remaining scan count
     */
    TInt CleanupCache( CGlxCache& aCache, TInt aRemainingScanCount );
        
    /**
     * Clean a particular media object: remove attributes that no fetch contexts want
     */
    void CleanupMediaL( CGlxCache& aCache, CGlxMedia& aMedia );
    
    /**
     * Get a list of all attributes that should not  be deleted form a given 
     * media object
     * @param aMedia Media object for which attributes are required
     * @return aNeededAttributes list of attributes that should not be 
     *                           deleted 
     */
    void GetAttributesInUseL( const CGlxMedia& aMedia, 
        RArray<TMPXAttribute>& aNeededAttributes ) const;

    /**
     * Delete attributes from a media object other than those specified
     * @param aMedia Media object from which to delete attributes
     * @return aAttributesToKeep List of attributes that will not be deleted
     */
    void DeleteOtherAttributes( CGlxCache& aCache, CGlxMedia& aMedia, 
        const RArray<TMPXAttribute>& aAttributesToKeep ) const;
        
    /**
     * Check user count in remaining caches for media item to be deleted
     * @param aMedia Media id which is to be checked
     * @return EFalse if user count is zero in all caches
     *	       ETrue if user count is non zero in any of the caches  
     */
    TBool MediaInUse(const CGlxMedia& aMedia) const;
private:
    /**
     * Abstraction of indexes that control the scanning of the cache across idle 
     * callbacks
     */
    struct TScanIndexes 
        {
        /// Index of the cache index that is currently being cleaned up
    	TInt iCurrentCacheIndex;
        /// Index of the media index in the current cache that should next be cleaned up
        TInt iNextMediaIndexToCleanup;
        };

private: // Data
    /// Single instance of the garbage collector
    static CGlxGarbageCollector* iGarbageCollector;

    /// A list of caches that should be kept clean
    /// not own (pointed cache objects)
    const RPointerArray<CGlxCache>& iCaches;

    // A list of attributes that are still in use by an item
    RArray<TMPXAttribute> iAttributesInUse;

    /// Periodic timer active object to use for callbacks
    /// own
    CPeriodic* iPeriodic; 
    
    /// Scan indexes of the current scanning position in the caches
    TScanIndexes iScanningPosition;
    
    /// Info of whether a full cleanup round should be performed to have all
    /// garbage freed from all caches.
    /// ETrue if cleanup should be run from the start before fully cleaned
    /// EFalse if the caches are clean currently or after the current cleanup
    /// round completes.
    TBool iRequiresFullCleanupRound;

    friend class T_CGlxGarbageCollector;
    };

#endif // C_GLXGARBAGECOLLECTOR_H