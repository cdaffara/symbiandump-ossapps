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
* Description: Definition for mailbox cache
*
*/

#ifndef EMAILMAILBOXCACHE_H
#define EMAILMAILBOXCACHE_H

#include <e32base.h> 
#include <emailapidefs.h>

using namespace EmailInterface;

class CPluginData;


/**
* Cache associating mailbox with a protocol plugin. Improves performance by
* removing need to load protocol plugins every time a mailbox needs to be
* accessed (and keeping plugin loaded when it is not needed)
* @since s60 v5.2
*/
NONSHARABLE_CLASS( CEmailMailboxCache ) : public CBase
{
public:
    /**
    * Cache entry to access plugin by mailbox id
    */
    class TCacheEntry
    {
    public:
        inline TCacheEntry( 
            CPluginData* aPluginData, 
            TMailboxId aId ) : 
            iPluginData( aPluginData ), 
            iMailboxId( aId ) {}
        CPluginData* iPluginData;
        TMailboxId iMailboxId;
    };
    
    /**
    * Constructor
    */
    static CEmailMailboxCache* NewL();
    
    /** destructor */
    ~CEmailMailboxCache();
    
    /**
    * Clears cache and indicates that one or more AddMailboxL calls will 
    * follow.
    * Cleanup support invalidates cache if leave occurs before EndCachingPop()
    * is called
    */
    void StartCachingPushL();
    
    /**
    * Marks caching fully complete for all mailboxes (call when no more
    * AddMailboxL will follow) and pops cleanup item.
    */
    void EndCachingPop();      
    
    /**
    * Returns if mailboxes are cached
    */
    TBool IsCached() const;

    /**
    * Adds mailbox to cache.
    * @param aPluginData mailbox associated with plugin(data)
    * @param aMailboxId mailbox id
    * @precondition: StartCaching must have been called. 
    */
    void AddMailboxL( CPluginData& aPluginData, const TMailboxId& aMailboxId );
    
    /**
    * Returns plugin data by mailbox id
    * @param aMailboxId 
    * @return plugin data associated with specified mailbox
    */
    CPluginData* PluginDataL( const TMailboxId& aMailboxId ) const;

    /**
    * Returns plugin data by plugin id
    * @param aPluginId 
    * @return plugin data associated with specified plugin
    */
    CPluginData* PluginDataL( const TUid& aPluginId ) const;
    
    /**
    * Gets all mailbox ids in all protocol plugins
    */
    void GetIdsL( REmailMailboxIdArray& aIdArray ) const;
    
private:    

    TInt FindById( const TMailboxId& aMailboxId ) const;
    TInt FindByPluginIdL( const TUid& aPluginId ) const;    
    static TBool Equals( const TCacheEntry& a1, const  TCacheEntry& a2 );
    static TBool PluginEquals( const TCacheEntry& a1, const  TCacheEntry& a2 );
    
    static void CleanupOp( TAny* aAny );
    
private:
    
    CEmailMailboxCache();
    
    void ConstructL();

    enum TCacheState {
        EEmpty,
        ECaching,
        EComplete
    };
private:
    // empty, caching or caching complete
    TCacheState iState;

    // cached mailbox id with associated plugin data
    RArray<TCacheEntry> iEntries;
};


#endif // EMAILMAILBOXCACHE_H

// End of file
