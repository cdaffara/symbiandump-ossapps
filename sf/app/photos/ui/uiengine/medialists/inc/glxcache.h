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
* Description:    Cache implementation for media items sharing the same Id space
*
*/




#ifndef C_GLXCACHE_H
#define C_GLXCACHE_H

#include <e32base.h>

#include "glxmediaid.h"

// Forward declarations
class CMPXMedia;
class CGlxMedia;
class T_CacheTestHelpers;
class TMPXAttribute;
class CGlxCacheManager;

/**
 *  CGlxCache
 *
 *  Individual cache of media items sharing the same Id space
 */
NONSHARABLE_CLASS( CGlxCache ) : public CBase
	{
public:
    /**
     * Constructor
     * @param aIdSpaceId The id of the id address space of this cache 
	 * @param aCacheManager Instance of cachemanager to cleanup the media
     */
    CGlxCache( const TGlxIdSpaceId& aIdSpaceId, CGlxCacheManager* aCacheManager );
    
    /**
     * Destructor
     */
    ~CGlxCache();

    /**
     * Returns the Id space covered by this cache
     * @return The Id space covered by this cache    
     */
    TGlxIdSpaceId IdSpaceId() const;    

    /**
     * Notify the cache that attributes have changed on media item(s)
     * @param aMedia The updated media item or array of updated media items
     */
    void MediaUpdatedL( const CMPXMedia& aMedia );

    /**
     * Update media item
     *
     * @param aMedia The updated media item
     */
    void UpdateMediaL( const CMPXMedia& aMedia );

    /**
     * Cleanup the media of the given media id: broadcast this to all observers
	 * @param aMediaId The media id of the item
     */ 
    void CleanupMedia(const TGlxMediaId& aMediaId);

    /**
     * Handles modifications of item in cache
     *
     * @param aId Media item id
     * @param aAttributes Attributes that have been modified
     */
    void HandleItemModified( const TGlxMediaId& aId, const RArray<TMPXAttribute>& aAttributes );

    /**
     * Access to the items stored in the cache: return the item matching the given media Id
     * @param aId The media Id
     * @return The media item, or NULL if not found
     */
    CGlxMedia* Media( const TGlxMediaId& aId ) const;

    /**
     * Access to the items stored in the cache: return the item matching the given media Id.
     * If not found, a new item is created
     * @param aId The media Id
     * @return The media item
     */
    CGlxMedia* FindItemForceCreateL( const TGlxMediaId& aId );
    
    /**
     * Number of items in cache
     * @return Number of items in cache
     */
    TInt Count();
    
    /**
     * Cached item by index
     * @param aIndex index of the item
     * @return Cached item
     */
    CGlxMedia& Media( TInt aIndex );
    
    /**
     * Delete an item at index from the cache.
     * @param aIndex Index of the item to delete
     */
    void Delete( TInt aIndex );
    
    /**
     * Reserve space for a number of users, for all items in the cache
     *
     * @param aCount The number of users to reserve space for
     */
    void ReserveUsersL( TInt aCount );

    /**
	 * Finds the media index present in cache
	 * @returns the index of media
	 */    
    TInt FindMediaIndexInCache(TGlxMediaId aMediaId);

private:
    /**
     * Copy the new and modified attributes from aSource to aTarget
     * @param aTarget The target media item
     * @param aSource The source media item
     * @param aNewAttributes The new and modified attributes
     */
    void CopyNewAndModifiedL( CGlxMedia& aTarget, const CMPXMedia& aSource, 
        RArray<TMPXAttribute>& aNewAttributes );

    /**
     * Function for TLinearOrder() for comparing two CGlxMedias 
     * Used from ordering the iItemPool by id
     * @param aItem1 The first item to compare
     * @param aItem2 The second item to compare
     * @return -1 if aItem1 < aItem2; 1 if aItem1 > aItem2; 0 if aItem1 == aItem2
     */
    static TInt MediaItemOrderById( const CGlxMedia& aItem1, const CGlxMedia& aItem2 );
    
    /**
     * Function for comparing a TGlxMediaId with a CGlxMedia 
     * Used from ordering iItemPool by media id
     * @param aMediaId The media id to compare against aItem2
     * @param aItem2 The second item to compare
     * @return -1 if aMediaId < aItem2.Id(); 1 if aMediaId > aItem2.Id(); 0 if aMediaId == aItem2.Id()
     */
    static TInt MediaItemOrderByKey( const TGlxMediaId* aMediaId, const CGlxMedia& aItem2 );

    /**
     * Create and add an item to the item pool
     * @param aId Id for the new item
     * @return Pointer to the created item
     */
    CGlxMedia* CreateItemL( const TGlxMediaId& aId );
    
private:
    /// Id space that this cache covers
    TGlxIdSpaceId iIdSpaceId;
    
    /// Pool of media items. Owned
    RPointerArray<CGlxMedia> iItemPool; 
    
    ///Instance of cacheManager to cleanup cache
    CGlxCacheManager* iCacheManager;

    friend class T_CacheTestHelpers;
    };

#endif // C_GLXCACHE_H
