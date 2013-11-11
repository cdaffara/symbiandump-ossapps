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
* Description:  Interface for presence cache reader.
*
*/

#ifndef MPRESENCECACHEREADER2_H
#define MPRESENCECACHEREADER2_H

#include <e32std.h>
#include <e32base.h>
#include <badesca.h>
#include <presencecacheids.hrh>

class MPresenceBuddyInfo2;
class MPresenceCacheReadHandler2;

/**
 * Interface for presence cache reader API. This interface is implemented 
 * by the Presence Cache client. Notice that an xsp id is
 * in the form ovi:xyz@example.com
 *
 * @since S60 v5.1
 */
class MPresenceCacheReader2
    {
public:


    /**
     * Factory method to instantiate MPresenceCacheReader.
     *
     * @return The new MPresenceCacheReader object. Object
     *         ownership is returned to caller.
     */
    IMPORT_C static MPresenceCacheReader2* CreateReaderL( );    

    /**
     * Public destructor.
     * Objects can be deleted through this interface.
     */
    virtual ~MPresenceCacheReader2() {};        
    

public: //Sub interfaces for presence cache reader, Synchonous methods

    
    /**
     * Reads presence info from cache for a given identity, returns immediately.
     *
     * @param aIdentity buddy id, identity must be in xsp id format
     * @return MPresenceInfo for the given buddy. Ownership to caller.
     *         Null if service or buddy not found, not on Cleanup in that case.    
     */
    virtual MPresenceBuddyInfo2* PresenceInfoLC(const TDesC& aIdentity) = 0;
    
    /**
     * Returns total number of buddies stored in presence cache.
     * It includes all services, probably useful while deciding performance 
     * overheads or informing to the user.
     *
     * @return TInt Total number of presence infos in presence cache.
     */                           
    virtual TInt BuddyCountInAllServices() = 0;
    
    /**
     * Returns total number of buddies (presence infos) stored in a specific
     * service in presence cache. It includes only the given service. 
     *
     * @param aServiceName as in device's service table
     * @return TInt buddy count or error code. e.g. in case of service not
     *         found return value is KErrNotFound.
     */                           
    virtual TInt BuddyCountInService(const TDesC& aServiceName) = 0;
    
    /**
     * Returns total number services in presence cache.
     *
     * @return TInt total number of services in presence cache.
     */                           
    virtual TInt ServicesCount() = 0;
        
    /**
     * Cancels any async. read operation going on. After calling this
     * method no callback will be received from asycn read method.
     *
     * @return error code, KErrNone if successfully cancelled.
     */
    virtual TInt CancelRead() = 0;
                                                

public: //Sub interfaces for presence cache reader, Asynchronous methods

    /**
     * Reads all presence info from cache for a given service. Returns in 
     * MPresenceCacheReadHandler2's HandlePresenceReadL.
     *
     * @param aServiceName Service name to search from
     * @param aHandler to handle async. callback
     * @return error code, in case of error callback wont be called.
     */    
    virtual TInt AllBuddiesPresenceInService(const TDesC& aServiceName, 
                                MPresenceCacheReadHandler2* aHandler) = 0;

public: //Sub interfaces for presence cache reader, Asynchronous methods
    
    /**
     * Sets the observer to receive Presence Buddy Change notifications.
     * Observer must be set before using SubscribePresenceBuddyChangeL.
     * If client changes the observer using this method, all the notifications
     * related to existing and new subscriptions will be delivered to 
     * the new observer.
     *
     * @return Error code, if KErrNone observer is successfully set.
     */
    virtual TInt SetObserverForSubscribedNotifications(
        MPresenceCacheReadHandler2* aHandler) = 0;
    
    /**
     * Subscribe for given presence buddy's change in presence. An observer
     * must be set before using this method by SetObserverForSubscribedNotifications.
     * Notifcations are delivered through MPresenceCacheReadHandler2's HandlePresenceNotificationL
     *
     * @param aIdentity buddy id, identity must be in xsp id format
     * @return KErrNone if successful, KErrArgument if observer not set
     * @leave Only in system error cases e.g. if failed to allocate memory.
     */
    virtual TInt SubscribePresenceBuddyChangeL(const TDesC& aIdentity) = 0;
                                                
    /**
     * Unsubscribe for given presence buddy's change in presence. The request
     * is ignored if invalid.
     *
     * @param aIdentity buddy id, identity must be in xsp id format
     * @leave Only in system error cases e.g. if failed to allocate memory.
     */
    virtual void UnSubscribePresenceBuddyChangeL(const TDesC& aIdentity) = 0;

    };

#endif // MPRESENCECACHEREADER2_H



