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
* Description: This file implements utilities like CPluginData.
*
*/

#ifndef EMAILAPIUTILS_H
#define EMAILAPIUTILS_H

#include <e32base.h>
#include "CFSMailCommon.h"
#include <emailapidefs.h>

//using namespace EmailInterface;

class CFSMailPlugin;

enum TDataOwner
    {
    EClientOwns,
    EAPIOwns
    };


/**
* Helper container class for storing plugin pointer and implementation UID
* @since S60 v5.2
*/
NONSHARABLE_CLASS( TPluginData )
    {
    public:
    
        inline TPluginData( TUid aUid ) : iPlugin( NULL ), iUid( TUid::Uid( aUid.iUid ) ) {}
        
        CFSMailPlugin* iPlugin;
        
        const TUid     iUid;
    };
    
/**
* Plugin data is a utility for protocol plugin creation and book 
* keeping. Objects using the plugin should access it via this
* class for optimised reference counting.
*/
NONSHARABLE_CLASS( CPluginData ) : public CBase
{
public:    
    /**
    * returns plugin instance or null if failed to instantiate.
    * LoadResult() will return error code in instantiation
    */
    CFSMailPlugin* ClaimInstance();
    
    /**
    * returns plugin instance or leaves if failed to instantiate.
    */
    CFSMailPlugin* ClaimInstanceL();

    /**
    *  Decreases access count of the plugin and deletes if reaches zero.
    * Note! this should be called only aftre succesful ClaimInstance or 
    * ClaimInstanceL.
    */
    void ReleaseInstance();
    
    /**
    * Adds cleanup operation calling ReleaseInstance to cleanup stack
    */                                                 
    void ReleasePushL();
    /**
    * Cleanup operation for CleanupReleasePluginPushL
    */
    static void CleanupOperation( TAny* aAny );
    
    TUid Uid() const;
    
    TInt LoadResult() const;

private: 
    /* only CEmailClientApi owns instances of this, it should make sure that
     * no duplicates exist, otherwise reference counting of plugins fail
     * (this is not singleton class)
     */
    friend class CEmailClientApi;
    friend class CEmailMailboxCache;
    
    CPluginData( TUid aUid );
    ~CPluginData();

private:    
    // plugin pointer and uid
    TPluginData iData;
    
    TInt iPluginLoadError;
        
    // >0 when iPlugin is instantiated
    TUint          iRefCount;
    
    // EFalse if shared from Email Framework (CFSMailClient). If true,
    // ReleaseInstance() doesn't delete plugin
    TBool          iOwned;
};

/**
* Constructs internal message id type from plugin data and mailbox/folder/message id 
*/
TFSMailMsgId FsMsgId( const CPluginData& aPluginData, const EmailInterface::TBaseId& aId );

/**
* Cleanup support for pointer arrays
*/
template<class T>
class CleanupResetAndDestroy
{
public:
    inline static void PushL( T& aItem );

private:
    inline static void ResetAndDestroy( TAny *aPtr );
} ;

template <class T>
inline void CleanupResetAndDestroyPushL( T& aRef )
    {
    CleanupResetAndDestroy<T>::PushL( aRef );
    }

/**
* Cleanup support for email interface objects.
*/
class CleanupReleasePush
{
public:
    static void PushL( EmailInterface::MEmailInterface& aItem );

private:
    static void Release( TAny *aPtr );
} ;


#include "emailapiutils.inl"


#endif // EMAILAPIUTILS_H

// End of file
