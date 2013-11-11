/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#include <e32base.h>

//Max size in Bytes
const TInt KCacheLimitSize = 8*1024*1024;

class CIRCacheMgmt;

NONSHARABLE_CLASS ( CIRCacheCleanup):public CTimer
    {
public:

    /**
    * Two-phased constructor.
    * @return  The newly created object.
    */
    static CIRCacheCleanup* NewL( CIRCacheMgmt& aCacheDb);

    /**
    * Destructor.
    */
    ~CIRCacheCleanup();

private:

    /**
     * Default Constructor.
     * @param CIRCacheMgmt refrerence
     */
    CIRCacheCleanup( CIRCacheMgmt& aCacheDb);

    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     *CleanupCacheDbL()
     * Cleansup routine
     */
    void CleanupCacheDbL();
    
    /**
     *CheckStatusL()
     * CheckStatus of CacheDb and do cleanup if necessary
     */
    void CheckStatusL();
    

    /**
     * @see CTimer::RunL().
     */
    void RunL();
    
    /**
    * GetCleanupInterval().
    *gets the cleanup routine call interval based on the current cache size.
    *formula derivation:
    *Timeinterval of cleanup ( T) is inversely proportional to the cache size( S)
    *Minimum time interval for cleanup routine 2 min.
    *Maximum time interval for cleanup routine ~35mins.
    *timeintervalperbyte = ( 35-2)*60*1000000/1024*1024
    *TimeInterval = timeintervalperbyte*( MaxCacheLimit-CurrentCachesize)
    */
    TUint32 GetCleanupInterval();
     
private:

    /** The cache database. */
    CIRCacheMgmt& iDatabase;
    
  };
  