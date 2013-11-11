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
* Description:  Interface for presence cache writer.
*
*/

#ifndef MPRESENCECACHEWRITER2_H
#define MPRESENCECACHEWRITER2_H

#include <e32std.h>
#include <badesca.h>
#include <presencecacheids.hrh>

class MPresenceBuddyInfo2;
class MPresenceCacheWriteHandler2;

/**
 * Interface for presence cache writer API. This interface is implemented 
 * by the Presence Cache client. Notice that an xsp id is
 * in the form ovi:xyz@example.com
 *
 * @since S60 v3.2
 */
class MPresenceCacheWriter2
    {
public:

    /**
     * Factory method to instantiate MPresenceCacheWriter2.
     *
     * @return The new MPresenceCacheWriter object. Object
     *         ownership is returned to caller.
     */
    
    IMPORT_C static MPresenceCacheWriter2* CreateWriterL( );  
    
    /**
     * Public destructor.
     * Objects can be deleted through this interface.
     */    
    virtual ~MPresenceCacheWriter2() {};    
    

public: //Sub interfaces for presence cache writer, Synchronous methods

    /**
     * Writes presence info to cache for a given buddy. Leaves only in system
     * errors.
     *
     * @param aPresenceBuddyInfo contains xsp identity and presence
     * @return Error code
     */
    virtual TInt WritePresenceL(const MPresenceBuddyInfo2* aPresenceBuddyInfo) = 0;
            
    /**
     * Instantiates new buddy presence info object.
     *
     * @return New buddy presence info instance. Object ownership
     *         is returned to caller.
     * @leave KErrNoMemory if failed to allocate memory.
     */
    virtual MPresenceBuddyInfo2* NewBuddyPresenceInfoLC() = 0;

        
    /**
     * Delete all buddies presence associated with given service.
     *
     * @param aServiceName service name as in sp table
     * @return error code
     */
    virtual TInt DeleteService(const TDesC& aServiceName) = 0;
    
    /**
     * Deletes all presence related to given xsp identity.
     * Leaves only in system errors.
     *
     * @param aIdentity identity for buddy to be removed, in xsp format
     * @return error code
     */
    virtual TInt DeletePresenceL(const TDesC& aIdentity) = 0;
    
    /**
     * Cancels any async. write operation going on. After calling this
     * method no callback will be received from asycn write method.
     *
     * @return error code, KErrNone if successfully cancelled.
     */
    virtual TInt CancelWrite() = 0;
    

public: //Sub interfaces for presence cache writer, Asynchronous methods

    /**
     * Writes buddy presence info objects to cache. Returns in 
     * MPresenceCacheWriteHandler2's HandlePresenceWriteL. Provide a null pointer
     * to receive no call back. Leaves only in system errors.
     *
     * @param aBuddyPresenceInfos presence infos for all buddies. 
     *         Ownership remains to caller.
     * @param MPresenceCacheWriterHandler2 to handle async calls. A 'NULL' pointer
     *         can be provided to receive no call back.
     * @return TInt error code.
     */    
    virtual TInt WriteMultiplePresenceL(
        const RPointerArray<MPresenceBuddyInfo2>& aPresenceBuddyInfos,
        MPresenceCacheWriteHandler2* aHandler) = 0;  
    
    };

#endif // MPRESENCECACHEWRITER_H
