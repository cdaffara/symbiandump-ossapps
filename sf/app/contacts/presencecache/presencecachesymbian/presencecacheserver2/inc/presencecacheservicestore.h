/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The cache stores a single array of this type. 
*
*/

#ifndef CPRESENCECACHESERVICESTORE_H
#define CPRESENCECACHESERVICESTORE_H

#include <e32base.h>

class CPresenceCacheBuddyStore;
class MPresenceBuddyInfo2;

/**
 * This class is used in presence cache store. The presence server has an array
 * of this object, each object represents the buddies in a specific service name.
 * This class contains an array of CPresenceCacheBuddyStore. The objects are
 * identified by their Identities.
 *
 * @lib presencecacheserver.exe
 * @since S60 v3.2
 */
class CPresenceCacheServiceStore: public CBase                                           
    {

public:

    /**
     * Symbian constructors and c++ destructor
     */
    IMPORT_C static CPresenceCacheServiceStore* NewLC();
    IMPORT_C static CPresenceCacheServiceStore* NewL();
    virtual ~CPresenceCacheServiceStore();

private:

    /**
     * c++ constructor
     */
    CPresenceCacheServiceStore();
    
    /**
     * Symbian second phase constructor
     */
    void ConstructL();

public:

    /**
    * Get the buddy stores pointer array contained by this object.
    * Those stores are owned by this object.
    *
    * @return reference to RPointerArray<CPresenceCacheSession>
    */
    RPointerArray<CPresenceCacheBuddyStore>& GetObjectCollection();
    

    /**
    * Service name of this object
    *
    * @return TPtrC service name of this object
    */
    TPtrC ServiceName() const;
    

    /**
    * Sets service name of this object
    *
    * @param aServiceName service name for this object
    */
    void SetServiceNameL(TDesC& aServiceName);
    

    /**
    * Count of buddy stores in this object.
    *
    * @return TInt number of buddy stores in this object
    */
    TInt Count() const;
    

    /**
    * Count of those buddy stores who has presence info.
    *
    * @return TInt number of buddy stores who has presence info
    */
    TInt PresenceCount() const;


    /**
    * Adds a buddy store object, without checking whether it exists or not.
    * Identity of the given store must be set. 
    * It takes ownership of given buddy store.
    *
    * @param aPresenceBuddyInfo buddy store to add or replace.
    * @return TInt an error code
    */
    TInt AddBlind(CPresenceCacheBuddyStore* aPresenceBuddyInfo);
    
    /**
    * Removes a buddy store object blindly. Unlike the other addition and removal
    * functions this will not modify the iPresenceCount. This will also not delete
    * the provided object. It's caller's responsiblity to delete it.
    *
    * @param aPresenceBuddyInfo buddy store to be removed
    */
    void RemoveBlind(CPresenceCacheBuddyStore* aPresenceBuddyInfo);

    /**
    * Adds of replaces a buddies presence info. If the buddy is found, then only
    * presence info is updated, leaving subscribed sessions untouched. It is
    * recommended to use when an update to an already existing buddy receives. 
    * It takes ownership of given presence info.
    *
    * @param aBuddyPresInfo buddy presence info to add or replace.
    * @return pointer to newly created or existing CPresenceCacheBuddyStore
    */
    CPresenceCacheBuddyStore* AddOrReplacePresenceL(MPresenceBuddyInfo2* aBuddyPresInfo);
    
    
    /**
    * Finds removed the buddy store with given identity. If the buddy does not
    * contain any subscribed sessions, this function removes the whole buddy store.
    * But if there are subscribed sessions to this buddy, then it only removes
    * presence from that buddy.
    *
    * @param aIdentity identity of the given buddy to be searched.
    * @param aErr the error code is returned here.
    * @return pointer to removed CPresenceCacheBuddyStore, returns NULL if the
    *         buddy store is completely removed, or not found.
    */
    CPresenceCacheBuddyStore* FindAndRemove(const TDesC& aIdentity, TInt& aErr);
    
    /**
    * Finds and get the buddy store with given identity.
    *
    * @param aIdentity identity of the given buddy to be searched.
    * @return pointer to found CPresenceCacheBuddyStore, returns NULL if the
    *         buddy store is not found.
    */
    CPresenceCacheBuddyStore* FindAndGet(const TDesC& aIdentity) const;
    
    /**
    * Removes presence info from all buddy stores contained in this object.
    * Notice that those buddy stores which doesnt have any subscribed sessions,
    * would be completely removed.
    */
    void RemoveAllPresences();
    
    /**
     * Removes buddy info from store if buddy info is expired
     * @param aBuddyStores buddy store is added to this 
     *        array if buddy info is removed and there is subscribed session
     */
    void RemoveExpiredBuddyInfos( 
            RPointerArray<CPresenceCacheBuddyStore>& aBuddyStores );
    
private: // data


    /**
     * Array of owned CPresenceCacheBuddyStore pointers
     */
    RPointerArray< CPresenceCacheBuddyStore> iBuddyStoreCollection;

    /**
     * OWN: Service name for this object
     */
    HBufC* iServiceName;
    
    /**
    * OWN: Count of buddy stores which contain valid presence info, because there
    * can be objects whose presences are NULL
    */
    TInt iPresenceCount;
    
    };


#endif // CPRESENCECACHESERVICESTORE_H
