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
* Description:  Interface for presence cache reader handler.
*
*/

#ifndef MPRESENCECACHEREADHANDLER2_H
#define MPRESENCECACHEREADHANDLER2_H

#include <badesca.h>

/**
 * Interface for presence cache reader API handler for asynchronous functions used in API.
 * This interface is implemented by the clients as per their needs. 
 */
 
// class MPresenceBuddyInfoList;
class MPresenceBuddyInfo2;

class MPresenceCacheReadHandler2
{
public:
    /**
     * Handler for AllBuddiesPresenceInService method of MPresenceCacheReader.
     *
     * @param aErrorCode 
     * @param aPresenceBuddyInfoList presence infos for all buddies requested. 
     *         Ownership of elements to callee. Can be empty if not found.
     */   
    virtual void HandlePresenceReadL(TInt aErrorCode,
         RPointerArray<MPresenceBuddyInfo2>& aPresenceBuddyInfoList) = 0;
                        
    /**
     * Handler for Buddy presence change notifications.
     *
     * @param aErrorCode 
     * @param aPresenceBuddyInfo presence info for buddy. 
     *         Ownership to callee. Can be NULL if aErrorCode exist.
     */    
    virtual void HandlePresenceNotificationL(TInt aErrorCode,
        MPresenceBuddyInfo2* aPresenceBuddyInfo) = 0;
};

#endif // MPRESENCECACHEREADHANDLER2_H



