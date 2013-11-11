/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CEmailMailboxCache.
*
*/

#include "emailmailboxcache.h"
#include "emailapiutils.h"

static const TInt KCacheGranularity = 2;

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CEmailMailboxCache* CEmailMailboxCache::NewL()
    {
    CEmailMailboxCache* cache = new ( ELeave ) CEmailMailboxCache();
    return cache;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CEmailMailboxCache::~CEmailMailboxCache()
    {
    iEntries.Close();
    }

// ---------------------------------------------------------------------------
// Start of cache transaction with clenanup support
// ---------------------------------------------------------------------------
void CEmailMailboxCache::StartCachingPushL()
    {
    TCleanupItem item( &CEmailMailboxCache::CleanupOp, this );
    CleanupStack::PushL( item );
    iState = ECaching;
    iEntries.Reset();
    }

// ---------------------------------------------------------------------------
//  End of cache transaction
// ---------------------------------------------------------------------------
void CEmailMailboxCache::EndCachingPop()
    {
    iState = EComplete;
    CleanupStack::Pop();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TBool CEmailMailboxCache::IsCached() const
    {
    return iState == EComplete;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMailboxCache::AddMailboxL( 
    CPluginData& aPluginData, 
    const TMailboxId& aMailboxId )
    {
    if ( iState != ECaching )
        {
        User::Leave( KErrNotReady );
        }
    if ( FindById( aMailboxId ) == KErrNotFound )
        {
        TCacheEntry entry( &aPluginData, aMailboxId );
        iEntries.AppendL( entry );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CPluginData* CEmailMailboxCache::PluginDataL( const TMailboxId& aMailboxId ) const
    {
    CPluginData* pdata = NULL;
    const TInt index( FindById( aMailboxId ) );
    if ( index != KErrNotFound )
        {
        const TCacheEntry& entry = iEntries[ index ];
        pdata = entry.iPluginData;
        }
    return pdata;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CPluginData* CEmailMailboxCache::PluginDataL( const TUid& aPluginId ) const
    {
    CPluginData* pdata = NULL;
    const TInt index( FindByPluginIdL( aPluginId ) );
    if ( index != KErrNotFound )
        {
        const TCacheEntry& entry = iEntries[ index ];
        pdata = entry.iPluginData;
        }
    return pdata;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void CEmailMailboxCache::GetIdsL( REmailMailboxIdArray& aIdArray ) const
    {
    if ( iState != EComplete )
        {
        // cache not up to date
        User::Leave( KErrNotReady );
        }
    for ( TInt i=0; i < iEntries.Count(); i++ )
        {
        TMailboxId id = iEntries[i].iMailboxId;
        aIdArray.AppendL( id );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TInt CEmailMailboxCache::FindById( const TMailboxId& aMailboxId ) const
    {
    TIdentityRelation<TCacheEntry> rel( CEmailMailboxCache::Equals );

    // don't care about plugin data because this is seach key only and
    // mailbox id is used for search
    TCacheEntry entry( NULL, aMailboxId );
    return iEntries.Find( entry, rel );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TInt CEmailMailboxCache::FindByPluginIdL( const TUid& aPluginId ) const
    {
    TIdentityRelation<TCacheEntry> rel( CEmailMailboxCache::PluginEquals );
    CPluginData* key = new ( ELeave ) CPluginData( aPluginId );
    TCacheEntry entry( key, TMailboxId() );
    TInt index = iEntries.Find( entry, rel ); 
    delete key;
    return index;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TBool CEmailMailboxCache::Equals( const TCacheEntry& a1, const TCacheEntry& a2 )
    {
    return ( a1.iMailboxId == a2.iMailboxId );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
TBool CEmailMailboxCache::PluginEquals( const TCacheEntry& a1, const TCacheEntry& a2 )
    {
    return ( a1.iPluginData->Uid() == a2.iPluginData->Uid() );
    }

// ---------------------------------------------------------------------------
// Cleanup 
// ---------------------------------------------------------------------------
void CEmailMailboxCache::CleanupOp( TAny* aAny )
    {
    CEmailMailboxCache* cache = reinterpret_cast<CEmailMailboxCache*>( aAny );
    cache->iEntries.Reset();
    cache->iState = EEmpty;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
CEmailMailboxCache::CEmailMailboxCache() : 
    iState( EEmpty ), 
    iEntries( KCacheGranularity )
    {
    }

// End of file
