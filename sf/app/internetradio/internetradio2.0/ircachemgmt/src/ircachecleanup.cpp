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


#include "ircachecleanup.h"
#include "ircachemgmt.h"
#include "irdebug.h"

//Constant decided on the rate of data collection in cache 
/*in a high speed connection*/
/* const TUint KConstantForCleanupFrequency = KCacheLimitSize*90; */
//maximum time interval = 2147483648/KCacheLimitSize   

const TUint KIntervalPerByte =2048 ;
//
const TInt KMinCleanupInterval = 2*60*1000000;//2 mins
const TInt KMaxCleanupInterval = 2147483647;//~36 mins for 1MB data

// ---------------------------------------------------------------------------
//Two-phased constructor.
// ---------------------------------------------------------------------------
//
CIRCacheCleanup* CIRCacheCleanup::NewL(CIRCacheMgmt& aCacheDb)
    {
    IRLOG_DEBUG( "CIRCacheCleanup::NewL - Entering" );
    CIRCacheCleanup* self = new(ELeave)CIRCacheCleanup(aCacheDb);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRCacheCleanup::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CIRCacheCleanup::~CIRCacheCleanup()
    {
    IRLOG_DEBUG( "CIRCacheCleanup::~CIRCacheCleanup - Entering" );
    Cancel();
    IRLOG_DEBUG( "CIRCacheCleanup::~CIRCacheCleanup - Exiting" );
    }

// ---------------------------------------------------------------------------
//Default Constructor
// ---------------------------------------------------------------------------
//
CIRCacheCleanup::CIRCacheCleanup(CIRCacheMgmt& aCacheDb):
    CTimer( CActive::EPriorityLow ), iDatabase( aCacheDb )
    {
    IRLOG_DEBUG( "CIRCacheCleanup::CIRCacheCleanup - Entering" );
    CActiveScheduler::Add( this );
    IRLOG_DEBUG( "CIRCacheCleanup::CIRCacheCleanup - Exiting" );
    }

// ---------------------------------------------------------------------------
//Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CIRCacheCleanup::ConstructL()
    {
    IRLOG_DEBUG( "CIRCacheCleanup::ConstructL - Entering" );
    CTimer::ConstructL();   
    CheckStatusL();
    IRLOG_DEBUG( "CIRCacheCleanup::ConstructL - Exiting" );
    }

// ---------------------------------------------------------------------------
//Cleanup CacheDb if necessary
// ---------------------------------------------------------------------------
//
void CIRCacheCleanup::CleanupCacheDbL()
    {
    IRLOG_DEBUG( "CIRCacheCleanup::CleanupCacheDbL - Entering" );
    iDatabase.CheckSizeL();
    IRLOG_DEBUG( "CIRCacheCleanup::CleanupCacheDbL - Exiting" );
    }
    
// ---------------------------------------------------------------------------
//CheckStatus of CacheDb and do cleanup if necessary
// ---------------------------------------------------------------------------
//
void CIRCacheCleanup::CheckStatusL()
    {
    IRLOG_DEBUG( "CIRCacheCleanup::CheckStatusL - Entering" );
    TTimeIntervalMicroSeconds32  interval(GetCleanupInterval());
    After(interval);
    CleanupCacheDbL();
    IRLOG_DEBUG( "CIRCacheCleanup::CheckStatusL - Exiting" );
    }
    
// ---------------------------------------------------------------------------
//  CIRCacheCleanup::RunL().
// ---------------------------------------------------------------------------
//
void CIRCacheCleanup::RunL()
     {
     IRLOG_DEBUG( "CIRCacheCleanup::RunL - Entering" );
     CheckStatusL();
     IRLOG_DEBUG( "CIRCacheCleanup::RunL - Exiting" );
     }
      
// ---------------------------------------------------------------------------
//  CIRCacheCleanup::GetCleanupInterval().
// ---------------------------------------------------------------------------
//
TUint32 CIRCacheCleanup::GetCleanupInterval()
    {
    IRLOG_DEBUG( "CIRCacheCleanup::GetCleanupInterval - Entering" );
    //formula derivation:
    //Timeinterval of cleanup (T) is inversely proportional to the cache size(S)
    //Minimum time interval for cleanup routine 2 min.
    //Maximum time interval for cleanup routine ~35mins.
    //timeintervalperbyte = (35-2)*60*1000000/1024*1024
    //TimeInterval = timeintervalperbyte*(MaxCacheLimit-CurrentCachesize)
    
    TInt interval =KIntervalPerByte * (KCacheLimitSize - iDatabase.CacheSize());
    //keep the value range bound
    interval = Max(interval,KMinCleanupInterval);
    interval = Min(interval,KMaxCleanupInterval);
    IRLOG_DEBUG( "CIRCacheCleanup::GetCleanupInterval - Exiting" );
    return interval;
    }
