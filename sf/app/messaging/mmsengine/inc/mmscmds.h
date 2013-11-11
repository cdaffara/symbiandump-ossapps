/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     Mtm commands from client mtm to server mtm
*
*/



#ifndef MMSCMDS_H
#define MMSCMDS_H

#include <msvstd.hrh>

//
// Mtm commands for requests from client side to server side
//
enum TMmsCommands
    {
    EMmsSend = KMtmFirstFreeMtmFunctionId, // from outbox to MMSC
    EMmsReceive,             // from MMSC to inbox
    EMmsScheduledSend,       // schedule to send
    EMmsScheduledReceive,    // schedule to fetch
    EMmsDeleteSchedule,      // delete selection from schedule (cancel)
    EMmsDecodePushedMessage, // Decode pushed message content, and invoke
                             // appropriate action.
    EMmsLogDeliveryReport,   // scheduled delivery report handling
    EMmsDeleteEntries,       // should only be used to clean old notifications
    EMmsReceiveForced,       // fetch message even in deferred or off state
    EMmsScheduledReceiveForced, // schedule message fetching even in deferred or off state
    EMmsGarbageCollection,   // if battery has been removed in the middle
                             // of an operation, entries may not be completely
                             // handled. Attempt to reschedule incomplete operations.
    EMmsDeleteExpiredNotifications, // in manual mode expired notifications must
                                    // be deleted every now and then
    EMmsRetryServiceLoading, // If service loading fails with KErrInUse, retry
                             // otherwise schsendexe panics.
    EMmsMessageGeneration,   // create "ready-made" mms messages from binaries
                             // stored in mmstest folder.
                             // For operator message variation feature
    EMmsForward,             // For forward without retrieval operation
    EMmsScheduledForward,    // For forward without retrieval operation
    EMmsNotificationDelete,  // For Notification Delete operation
    EMmsScheduledNotificationDelete, // For Notification Delete operation
    EMmsUpdateMmboxList,     // update mmbox list view (unscheduled)
    EMmsSendReadReport,      // Send MMS read report
    EMmsScheduledReadReport  // Schedule sending of MMS read report
    };

//
// General param pack
//
class TCommandParameters
    {
    public:
        inline TCommandParameters();
        // The delay to wait before operation is started.
        // if 0, delay is determined by hard-coded default value
        // (a small delay, for example 10 seconds, exact value must
        // be determined when hardware timing is better known)
        // This parameter is intended to be used to initiate
        // retry operation after a delay (for example 10 minutes)
        TTimeIntervalSeconds iInitialDelay;
        // only schedule entries that have failed because of following error
        // if iError == KErrNone, everything is rescheduled
        TInt iError; 
    };

typedef TPckgBuf<TCommandParameters> TCommandParametersBuf;

TCommandParameters::TCommandParameters():
iInitialDelay( 0 ),
iError( 0 )
    {
    }

//
// parameter pack for EMmsDecodePushedMessage command
//
class TWatcherParameters
    {
    public:
        inline TWatcherParameters();
        TThreadId iWatcherId;
        TAny* iDataPointer;
    };

typedef TPckgBuf<TWatcherParameters> TWatcherParametersBuf;

TWatcherParameters::TWatcherParameters():
iWatcherId( 0 ),
iDataPointer( 0 )
    {
    }

//
// parameter pack for EMmsGarbageCollection command
//
class TMMSGarbageCollectionParameters
    {
    public:
        inline TMMSGarbageCollectionParameters();
        TUint32 iReasonFlags;
        TTime iMediaUnavailableTime; // the moment when media has become unavailable.
    };

typedef TPckgBuf<TMMSGarbageCollectionParameters> TMMSGarbageCollectionParametersBuf;

TMMSGarbageCollectionParameters::TMMSGarbageCollectionParameters():
iReasonFlags( 0 ),
iMediaUnavailableTime( 0 )
    {
    }

#endif      // MMSCMDS_H  
            
// End of File
