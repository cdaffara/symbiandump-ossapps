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
* Description:    Interface to media item cache
*
*/




#ifndef M_GLXCACHE_H
#define M_GLXCACHE_H

class CGlxMedia;
class MGlxCacheObserver;

#include "glxmediaid.h"

/**
 *  MGlxCache
 *
 *  External interface to the media item cache
 *
 *  @lib glxmedialists.lib
 */
class MGlxCache
	{
public:	
    /**
     * Return a reference-counted instance of MGlxCache. User MUST Close() this when finished with it.
     * @return Singleton cache object
     */
    IMPORT_C static MGlxCache* InstanceL();

    /**
     * Finds a media item based on id-space and media id
     * @param aIdSpaceId The id space of the media item
     * @param aMediaId The id of the media item
     * @return The media item, if found; NULL if not
     */
	virtual CGlxMedia* Media(const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aMediaId) const = 0;

    /**
     * Add a cache observer
     * @param aObserver The observer to add
     */
	virtual void AddObserverL(MGlxCacheObserver* aObserver) = 0;

    /**
     * Remove a cache observer
     * @param aObserver The observer to remove
     */
	virtual void RemoveObserver(MGlxCacheObserver* aObserver) = 0;

    /**
     * Begin refreshing fetch contexts, if currently idle.
     */
    virtual void RefreshL() = 0;

    /**
     * Close the MGlxCache object. This MUST be called when the client has finished using it
     */	
	virtual void Close() = 0;
	
	//OOM	
	/**
	 * ReleaseRAM on request
	 */ 
	 virtual void ReleaseRAML(TBool aFlushOnRequest = EFalse) = 0;
	/**
	 * Stop RAM realease.
	 */ 
	virtual void StopRAMReleaseL() = 0;
    //OOM

	/**
     * Cleanup Container Cache
     */
	virtual void ForceCleanupMedia(TGlxIdSpaceId aSpaceId,TGlxMediaId aId)=0;
	};

#endif // M_GLXCACHE_H

