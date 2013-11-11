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
* Description: Definition of Email API main interface.
*
*/

#ifndef EMAILCLIENTAPIIMPL_H
#define EMAILCLIENTAPIIMPL_H


#include <implementationinformation.h> 
#include "emailapiutils.h"
#include "CFSMailCommon.h"
#include "MFSMailEventObserver.h"    
#include <memailclientapi.h>
#include "emailclientpluginmanager.h"


using namespace EmailInterface;

class CFSMailBox;
class MEmailEventObserver;
class CVwsSessionWrapper;
class CFSMailPlugin;
class TFSMailMsgId;
class CEmailMailboxCache;
class CFSMailClient;

/**
*  "Framework" class including plugin management
* @since S60 v5.2
*/
NONSHARABLE_CLASS( CEmailClientApi ) : public CBase, public MEmailClientApi
{
public:
    /**
    * Constructor
    * @return
    */
    static CEmailClientApi* NewL();
    
    /**
     *
     */
     CEmailClientApi();       

public: // from MEmailInterface
    TEmailTypeId InterfaceId() const;
    
    void Release();
    
public: // from MEmailClientApi

    /** @see MEmailClientApi */   
    MEmailMailbox* MailboxL( const TMailboxId& aId );
   
    /** @see MEmailClientApi */   
    MEmailMailbox* MailboxL( const TPtrC& aAddress );
           
    /**
    * Gets all mailboxes in pointer array. It is recommended to free other
    * mailboxes besides needed one to optimise memory usage.
    */    
    TInt GetMailboxesL( RMailboxPtrArray& aMailboxes );
    
    /**
    */
    void LaunchEmailL( const TLaunchPolicy aPolicy );

public: // new methods

    /**
     * Gets array of mailbox ids
     * @param aMailboxes mailbox ids from all plugins
     * @return number of mailboxes returned in aMailboxes
     */
    TInt GetMailboxIdsL( REmailMailboxIdArray& aMailboxes );
        
private:

    /**
     * Constructor
     */
    void ConstructL();
    
     /**
     * Destructor
     */
    ~CEmailClientApi();

    /**
     * Push ECom implementation info to cleanup operation to cleanup stack
     */
    void CleanupImplInfoPushL( RPointerArray<CImplementationInformation>& aArray );
    
    /**
     * Cleanup operation for ECom implementation info
     */
    static void CleanupImplInfo( TAny* aAny );
   

    // Group of methods for book keeping of loaded protocol plugins by this object.
    // In certain situations it is preferable to have a plugin in memory
    // instead of loading/unloading frequently.
    
    /** Increases plugin reference count if CEmailClientApi already doesn't
     *  have active instance. When plugin is needed internally, this method
     * should be accessor to a plugin. Counterpart is ReleasePlugin() when
     * CEmailClientApi doesn't itself need plugin any more.
     */
    CFSMailPlugin* UsePlugin( CPluginData& aPluginData );
    
    /**
    * For finding plugin data
    */
    TInt IndexOfLoadedPluginData( const TPluginData& aPluginData ) const;
    
    /**
    * Used in comparing plugin data in array (part of find algorithm)
    */    
    static TBool PluginDataEquals( const TPluginData& a1, const TPluginData& a2 );
    
    /**
    * This object doesn't need related protocol plugin
    */
    void ReleasePlugin( CPluginData& aPluginData );
    
    /**
    * This object doesn't need any protocol plugin
    */
    void ReleaseAllPlugins();
    
    /**
    * Return mailbox cache, creates it when called first time
    */
    CEmailMailboxCache& MailboxInfoCacheL();
    
    /**
    * Caches mailbox ids and their mapping to related plugin data for quick
    * lookup. When a mailbox with specific id is next time asked, related
    * protocol plugin is already known.
    */
    void UpdateMailboxInfoCacheL();

    /**
    * Internal, used from UpdateMailboxInfoCacheL
    */    
    TBool CachePluginMailboxesL( 
        CPluginData& aPluginData, 
        CFSMailPlugin& aPlugin );
    
private:    
    typedef RPointerArray<CPluginData> RPluginDataArray;
    
    /**
    * Iterator for CPluginData elements in an array
    */
    class TPluginIterator
    {
    public:
        // constructor
        inline TPluginIterator( RPluginDataArray& aArray ) : 
            iArray( aArray ), iIndex( 0 ) {}
        
        /**
        * return next element
        */
        CPluginData* Next();
    private:
        // iterator array
        RPluginDataArray& iArray;
        // current iterator index
        TInt iIndex;
    };
    
private:
    // plugin data array for all protocol plugins
    RPluginDataArray    iPluginDataArray;

    // singleton instance counter    
    TInt iInstanceCounter;
    
    // book keeping of loaded plugins
    RArray<TPluginData> iLoadedPluginsArray;
       
    // Mailbox ids are cached when first time listed. Related plugin uid
    // is include in the cache.
    CEmailMailboxCache* iMailboxCache;

    CFSMailClient* iMailClient;
    
};

/**
* Email application launcher. 
*/
NONSHARABLE_CLASS( CEmailLauncher ) : public CBase
{
public:
    static CEmailLauncher* NewL();
    
    ~CEmailLauncher();
                                    
    /**
    *
    */
     void LaunchL( const TMailboxId& aMailboxId );
private:
    
    CEmailLauncher();
    
    void ConstructL();
    
    // Owned window server session.
    CVwsSessionWrapper* iViewSrvSession;
};


#endif // EMAILCLIENTAPIIMPL_H

// End of file
