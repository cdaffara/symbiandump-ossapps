/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines for Presence Cache.
 *
*/

#ifndef PRESENCECACHEDEFS2_H
#define PRESENCECACHEDEFS2_H

#include <e32std.h>

/*
 * ============================================================================
 * Presence cache constants
 * ============================================================================
 */
namespace NCacheSrv2
    {
    /**
     * Cache server version numbers
     */
    namespace NVersion
        {
        const TInt KMajor = 1;
        const TInt KMinor = 1;
        const TInt KBuild = 1;
        }
        
    namespace DataBase
        {
        const TInt KBuddyIdMaxLen = 100;
            
        }    


    /**
     * Cache server names
     */
    namespace NName
        {
        _LIT( KExecutable,      "presencecacheserver2.exe" );
        _LIT( KMainThread,      "PrCacheSrv2" );
        _LIT( KSymbianServer,   "!PresenceCacheSrv" );
        // _LIT( KServerPattern,   "*Presence*" );
        }

    /**
     * Cache server requests
     */
    namespace NRequest
        {
        //Number asynchronous message slots for Cache server
        const TUint KMsgSlotCount = 4;

        //Presence cache op synch. codes
        // Do not change the order of enums since they are referred by TPolicy.
        enum TOpSyncCodes
            {
            // Read security capabilities    
            EPrepReadPresenceInfoSync = 1,
            EGetLastPreparedPacket,
            EGetLastAsyncPreparedPacket,
            EBuddyCountInAllServices,
            EBuddyCountInService,
            EGetServiceCount,
            //Subscription/Notification
            ESubscribeBuddyPresenceChange, // 0:aService.iUid, 1:myIdPack
            EUnSubscribeBuddyPresenceChange,// 0:aService.iUid, 1:myIdPack
            EGetLastNotifiedtPacket, // 0:&ptrBuf            
            // Write security capabilities for the rest
            EWritePresenceInfoSync,
            EDeleteService,
            EDeletePresence                                    
            }; //See the starting count in TOpAsyncCodes before adding here
               //Total number should be less than that here
            
        //Presence cache op asynch. codes
        enum TOpAsyncCodes
            {
            // Read security capabilities                    
            ENoRequestMade = 21,
            ECancelAsyncOperation,
            EPrepReadAllBuddiesPresenceInService,
            EPrepReadAllGivenBuddiesPresence,            
            //Subscription/Notification related
            EWaitingForNotification, // 0:&sizePckg
            ECancelWaitingForNotification, //             
            // Write security capabilities for the rest            
            EWritePresenceInfoAsync            
            };
        }
        
    /**
     * Other constants
     */
    namespace NConstants
        {
        //Number buddy infos write in each cycle of runl
        const TUint KMaxWriteEachCycle = 5;
        const TUint KMaxReadEachCycle  = 5;
        const TUint KMaxServicesCount  = 10;
        const TUint KGranularity	   = 10;
        }
        
    }

#endif  //PRESENCECACHEDEFS_H
