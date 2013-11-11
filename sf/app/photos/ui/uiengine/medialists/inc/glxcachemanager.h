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
* Description:    Manager of media item cache
*
*/




#ifndef C_GLXCACHEMANAGER_H
#define C_GLXCACHEMANAGER_H

#include <e32base.h>

#include "mglxcache.h"
#include "mglxcacheobserver.h"
#include "mglxmediapool.h"
#include "glximagereader.h"

#include <apmstd.h>

#ifdef USE_S60_TNM
#include <thumbnailmanager.h>
#include <thumbnailmanagerobserver.h>
#endif

// Forward declarations
class CFbsBitmap;
class CGlxCache;
class CGlxGarbageCollector;
class CGlxMedia;
class CGlxMediaList;
class CMPXCollectionPath;
class CMPXMedia;
class MGlxMediaList;
class CGlxImageViewerManager;

/**
 *  CGlxCacheManager
 *
 *  Manager of item caches
 *
 *  @lib glxmedialists.lib
 */
class CGlxCacheManager : public CBase, 
                         public MGlxCache, 
                         public MGlxMediaPool
#ifdef USE_S60_TNM
                         , public MThumbnailManagerObserver
#endif
	{
public:
    /**
     * Return a reference-counted instance of CGlxCacheManager. User MUST Close() this when finished with it.
     * @return Singleton cache manager object
     */
	static CGlxCacheManager* InstanceL();

    /**
     * Called when a media item has new attributes
     * @param aIdSpaceId Id space of media item
     * @param aMedia The media object 
     * @param aError Error
     */
	void HandleCollectionMediaL(const TGlxIdSpaceId& aIdSpaceId, const CMPXMedia& aMedia, TInt aError);

	/**
	 * Informs that focus in the window of the list has changed. The lists 
	 * calls this function so that the manager knows to start updating the cache
	 * to load the now needed items.
     *
     * @param aList List that has changed
	 */
	void HandleWindowChangedL(CGlxMediaList* aList);

	/**
	 * Handles modification of item in a cache
	 *
	 * @param aIdSpaceId Id space of the cache containing the media item
	 * @param aMediaId Media item id
	 * @param aAttributes Attributes that have been modified
	 */
	void HandleItemModified(const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aMediaId, const RArray<TMPXAttribute>& aAttributes);

    /**
     * Returns the temporary thumbnail used in a thumbnail request
     * @return The temporary thumbnail used in a thumbnail request
     * @deprecated Temporary solution - waiting for global chunk solution
     */
	CFbsBitmap* TempThumbnail();
	
	/**
	 * The Id of the temporary thumbnail used in a thumbnail request
	 * @return The Id of the temporary thumbnail used in a thumbnail request
     * @deprecated Temporary solution - waiting for global chunk solution
	 */
	TGlxMediaId TempThumbnailId();
	
	/**
	 * Sets the temporary thumbnail to NULL. Thumbnail is not deleted, use when ownership has transfered to
	 * another object
     * @deprecated Temporary solution - waiting for global chunk solution
	 */
    void SetTempThumbnailToNull();

    /**
     * New attributes are available for a media item: broadcast this to all observers
	 * @param aIdSpaceId The Id Space of the item
	 * @param aMediaId The media id of the item
	 * @param aAttributes The new attributes
	 * @param aMedia The media item
     */    
    void BroadcastAttributesAvailableL(const TGlxIdSpaceId& aIdSpaceId, 
                        const TGlxMediaId& aMediaId, 
                        const RArray<TMPXAttribute>& aAttributes, 
                        const CGlxMedia* aMedia);

    /**
     * Cleanup the media of the given media id: broadcast this to all observers
	 * @param aMediaId The media id of the item
     */    
	void CleanupMedia(const TGlxMediaId& aMediaId);

    /**
     * Returns an array of all id-space caches currently in use.
     * @return Array of all id-space caches currently in use.
     */
    const RPointerArray<CGlxCache>& Caches();
    
    /**
     * Sets the temporary error flag. When set, this indicates that there is at least one temporary
     * error on an item in the cache.
     */
    void SetTemporaryErrorFlag();
    
	/**
	 * Informs that a media list has been deleted. The lists 
	 * calls this function so that the manager can check to
     * see if the last request was from this list.  If so, it
     * should reset and continue with requests from other lists
     *
     * @param aList List that has been deleted
	 */
	void HandleListDeleted(CGlxMediaList* aList);

    /**
     * Inform cache manager to reserve space for a number of users
     * for all items in a particular cache
     *
     * @param aIdSpaceId The Id space Id to determine which cache to reserve users
     * @param aCount The number of users to reserve space for
     */
    void ReserveUsersL(const TGlxIdSpaceId& aIdSpaceId, TInt aCount);

    /**
	 * Check if the media item selected is NULL.
	 * If media item is NULL, cancel the pending requests 
	 * and place a new request
	 */    
    void CancelPreviousRequest();

public: // From MGlxCache
    CGlxMedia* Media( const TGlxIdSpaceId& aIdSpaceId,
                        const TGlxMediaId& aMediaId ) const;
    void AddObserverL( MGlxCacheObserver* aObserver );
    void RemoveObserver( MGlxCacheObserver* aObserver );
    void RefreshL();
    void Close();
    //OOM
    void ReleaseRAML(TBool aFlushOnRequest = EFalse);
    void StopRAMReleaseL();
	//OOM
    void ForceCleanupMedia(TGlxIdSpaceId aSpaceId,TGlxMediaId aId); 

private:
    /**
     * Constructor
     */
	CGlxCacheManager();
	
	/**
	 * All Garbage Clean-up,Cancel Garbage Cleanup is Centralised Here 
	 * It can be used to Cancel Garbage if there is Enough Memory Else it starts Flushing 30 items Together,
	 * It can be used to start Garbage Clean-up if there is Not Enough Memory
	 *
	 * @param aStart
	 */
	void HandleGarbageCollectionL(TBool aStart);
	    
	/**
	 * Two-phase construction
	 */
    static CGlxCacheManager* NewL();
    
	/**
	 * Second-phase constructor
	 */
	void ConstructL();
	
	/**
	 * Destructor
	 */
	virtual ~CGlxCacheManager();
	
	/**
	 * Deletes unused items in all id-space caches, requests new attributes
	 */
    void MaintainCacheL();
    static TInt MaintainCacheL(TAny* aPtr);

	/**
	 * Compares two paths to see if their levels match
	 */
	static TBool Match(const CMPXCollectionPath& aPath1, const CMPXCollectionPath& aPath2);
	
    /**
     * Returns the cache for the given id space. Create one if it doesn't exist.
     * @param aIdSpaceId The id space
     * @return The cache for the id space
     */
    CGlxCache* FindCacheForceCreateL(const TGlxIdSpaceId& aIdSpaceId);

    /**
     * Finds a media item based on id-space and media id. Creates one if none found
     * @param aIdSpaceId The id space of the media item
     * @param aMediaId The id of the media item
     * @return The media item
     */
    CGlxMedia* MediaForceCreateL(const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aMediaId);
    
    /**
     * Function for TLinearOrder() for comparing two CGlxCaches 
     * Used from ordering iCaches by id-space id
     * @param aItem1 The first item to compare
     * @param aItem2 The second item to compare
     * @return -1 if aItem1 < aItem2; 1 if aItem1 > aItem2; 0 if aItem1 == aItem2
     */
    static TInt CacheOrderById(const CGlxCache& aItem1, const CGlxCache& aItem2);
    
    /**
     * Function for comparing a TGlxIdSpaceId with a CGlxCache 
     * Used from ordering iCaches by id-space id
     * @param aIdSpaceId The id space id to compare against aItem2
     * @param aItem2 The second item to compare
     * @return -1 if aIdSpaceId < aItem2.Id(); 1 if aIdSpaceId > aItem2.Id(); 0 if aIdSpaceId == aItem2.Id()
     */
    static TInt CacheOrderByKey(const TGlxIdSpaceId* aIdSpaceId, const CGlxCache& aItem2);
    
    /**
     * Returns whether any requests are being made on errored attributes
     * @return Whether any requests are being made on errored attributes
     */
    TBool ErrorsOnRequestedItemsL();

    /**
     * Timer callback for temporary error reminder
     * @param aPtr Pointer to object that started the timer
     * @return 0 to signal that further callbacks are unnecessary, 1 otherwise
     */
    static TInt TempErrorTimerCallbackL(TAny* aPtr);
    
    /**
     * Starts the temporary error timer. When complete, the caches are checked again for any missing attributes
     */
    void StartTempErrorTimer();

    /**
     * Called when the temporary error timer completes: forces a re-examination of all caches
     */
    void TempErrorTimerCompleteL();

    /**
     * Create a path with the request items
     *
     * @param aList list to obtain the path
     */
    CMPXCollectionPath* RequestAsPathLC(const CGlxMediaList& aList);
    
    /**
     * Gets the mime type of a given file.
     * @param aFileName The file name for which a MIME type to be determined
     * @aMimeType The MIME type associated with the given file will be returned
     */
    void GetMimeTypeL(TFileName& aFileName, TDataType& aMimeType );

    
#ifdef USE_S60_TNM
    /*
     * This function doesnt add up any value, added to reduce compiler warnings
     */
    void ThumbnailReadyL(TInt aError, MThumbnailData& aThumbnail, 
                         TThumbnailRequestId aId, TBool aQuality);

private: // From MThumbnailManagerObserver

    void ThumbnailPreviewReady( MThumbnailData& aThumbnail,
        TThumbnailRequestId aId );
    void ThumbnailReady( TInt aError, MThumbnailData& aThumbnail,
        TThumbnailRequestId aId );

	TInt FindLoadingById(TThumbnailRequestId aId, TBool aRemove = EFalse);
#endif
	
private:
    /// Single instance of the cache manager
	static CGlxCacheManager* iCacheManager;
	
	/// Current reference count
	TInt iReferenceCount;

    /// List of observers, not owned
    RPointerArray<MGlxCacheObserver> iObserverList;

    /// Caches, one per Id space. Owned.
    RPointerArray<CGlxCache> iCaches;

    /// Bitmap for pending thumbnail request
    CFbsBitmap* iTempThumbnail;
    /// ID of item whose thumbnail was requested
    TGlxMediaId iThumbnailId;
    
	/// Information if a request has been made to collection which 
	/// has not complete yet. (Currently, only one request is made at a time)
    /// Stores the media list that owns the request
	CGlxMediaList* iRequestOwner;

	/// The requested Ids in the current request
	RArray<TGlxMediaId> iRequestedItemIds;
	/// The requested attributes in the current request
	RArray<TMPXAttribute> iRequestedAttrs;
	/// The Id Space of the items in the current request
	TGlxIdSpaceId iRequestedItemsIdSpace;

    /// Garbage collector	
    CGlxGarbageCollector* iGarbageCollector;	

    /// Temporary error flag    
    TBool iTempError;
    
    // Clean up is On Going- Timer Started Or it is Cleaning Up
    TBool iCleanUpOnGoing;
    
    /// Temporary error timer
	CPeriodic* iTempErrorTimer;
	
    CAsyncCallBack* iMaintainCacheCallback;

	RArray<TInt> iRequestedItemIndexes;
    CGlxImageReader* iReader;
#ifdef USE_S60_TNM
    CThumbnailManager* iTnEngine; // Own
	CMPXMedia* iMPXMedia;

    // Loading information
    class TLoadingTN
        {
    public:
        TLoadingTN(TThumbnailRequestId aId, TGlxIdSpaceId aSpaceId, 
                   TSize aSize, TGlxMediaId aThumbnailId)
        : iId(aId), iSpaceId(aSpaceId), 
		  iSize(aSize), iThumbnailId(aThumbnailId) 
            {
            }
        
        TThumbnailRequestId iId;
        TGlxIdSpaceId iSpaceId;
        TSize iSize;
        TGlxMediaId iThumbnailId;
        };

	RArray<TLoadingTN> iThumbnailRequestIds;
#endif
	
#ifdef _DEBUG
	TTime iStartTime;
    TTime iStopTime;
#endif    
    };

#endif // C_GLXCACHEMANAGER_H
