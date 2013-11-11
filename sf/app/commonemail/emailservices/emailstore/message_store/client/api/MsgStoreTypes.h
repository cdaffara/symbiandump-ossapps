/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message store type definitions.
*
*/



#ifndef __MSG_STORE_TYPES_H__
#define __MSG_STORE_TYPES_H__

// ========
// INCLUDES
// ========

#include <e32std.h>

// =====
// TYPES
// =====

/** Defines the type for IDs within the message store. */ 
typedef TUint TMsgStoreId;	

/** Enumerates the supported property value types. */
enum TMsgStorePropertyValueType
	{
	EMsgStoreTypeUint32,
	EMsgStoreTypeDes8,
	EMsgStoreTypeDes,
	EMsgStoreTypeContainer,
	EMsgStoreTypeBool,
	EMsgStoreTypeTime,
	EMsgStoreTypeAddress
	}; // end enum TMsgStorePropertyValueType



/** Defines the meaning of the individual bits of the message flags 32-bit integer. */
enum TMsgStoreFlagsMask
	{
	EMsgStoreFlag_Read = 1,               // Message is read (or "seen") on the server
	EMsgStoreFlag_Read_Locally = 2,       // Message is read on the client
	EMsgStoreFlag_Low = 4,			      // Message has low priority
	EMsgStoreFlag_Important = 8,          // Message has high priority
	EMsgStoreFlag_FollowUpComplete = 16,  // The message follow-up is complete
	EMsgStoreFlag_FollowUp = 32,          // Message is flagged (a flag is showing next to msg in Outlook)
	EMsgStoreFlag_Attachments = 64,       // Message has attachments
	EMsgStoreFlag_Multiple = 128,         // Message has more than one recipient
	EMsgStoreFlag_CalendarMsg = 256,      // Message is a calendar message
	EMsgStoreFlag_Answered = 512,         // The message was replied to
	EMsgStoreFlag_Forwarded = 1024,       // The message was forwarded
	EMsgStoreFlag_OnlyToMe = 2048,        // The message was sent only to this user
	EMsgStoreFlag_RemoteDeleted = 4096,   // The message has been deleted on the server
	EMsgStoreFlag_HasMsgSender = 8192     // The message has one or more senders 
	}; // end enum TMsgStoreFlagsMask

const TUint KMsgStoreSystemEventMin = 1000;

/** Defines the system events of which observers receive notification. */
enum TMsgStoreSystemEvent
	{
    // Available and Unavailable events have been added which include the
    // status of backup/restore and Pointsec.  It is recommended that these
    // new events be used to determine Msg Store availability.
    // Backup/restore events and Pointsec events left for backwards compatibility.
    
    // A backup or restore is in progress.
    EMsgStoreBackupOrRestoreInProgress = KMsgStoreSystemEventMin,
    
    // The backup or restore completed.
    EMsgStoreBackupOrRestoreCompleted,
    
    // PointSec has locked the file system, all operations are suspendded
    EMsgStorePointSecLockStarted,
    
    // PointSec has unlocked the file system, all operations are resumed
    EMsgStorePointSecLockEnded,
    
	// The message store has been wiped, including the password.
	EMsgStoreDatabaseWiped,

	// The message store has transitioned from an authenticated state to an unauthenticated state.
	EMsgStoreNotAuthenticated,

	// The message store has transitioned from an unauthenticated state to an authenticated state.
	EMsgStoreAuthenticated,
	
	// The observer event queue has overflowed and has been reset, so events have been lost.  This
	// shouldn't happen unless the client thread has been starved for an extended period of time, during
	// which many message store operations have occurred.
	EObserverEventQueueOverflow,
	
    // The message store is unavailable.  This may be due to 
    // a backup or restore being in progress, a Pointsec lock being issued,
    // the disk drive being unavailable, etc.
    EMsgStoreUnavailable,
    
    // All lock conditions have cleared and the message store is available again.
    EMsgStoreAvailable
	
	}; // end enum TMsgStoreSystemEvent
	
const TUint KMsgStoreAccountEventMin = 2000;

/** Defines the account events of which observers receive notification. */
enum TMsgStoreAccountEvent
	{
	// An account has been created
	EMsgStoreAccountCreated = KMsgStoreAccountEventMin,
	
	// An account has been deleted
	EMsgStoreAccountDeleted,
	
	// An account has been renamed.  Note that this is the actual message store account name, which is distinct from any
	// display names that might be shown to the user.
	EMsgStoreAccountRenamed
	
	}; // end enum TMsgStoreAccountEvent


/** Enumerates the mail box operation types. */
enum TMsgStoreOperation
    {
	EMsgStoreAdd = 1,
	EMsgStoreDelete,
	EMsgStoreMove,
	EMsgStoreUpdateProperties,
	EMsgStoreUpdateContent,
	EMsgStoreRemoveContent
	}; // end enum TMsgStoreOperation


/** Enumerates the message store container types */
enum TMsgStoreContainerType
	{
    EMsgStoreUnknownContainer = -1,
    EMsgStoreMailboxContainer,
	EMsgStoreFolderContainer,
	EMsgStoreMessageContainer,
	EMsgStorePartContainer    
	}; // end TMsgStoreContainerType

/** Enumerates the message store sortable fields */	
enum TMsgStoreSortByField
	{
	EMsgStoreSortByReceivedDate,
	EMsgStoreSortBySender,        
    EMsgStoreSortByRecipient,        
	EMsgStoreSortBySubject,       
	EMsgStoreSortByPriority,
	EMsgStoreSortByFlagStatus,    //follow up and completed
	EMsgStoreSortByUnRead,
	EMsgStoreSortBySize,
	EMsgStoreSortByAttachment,
	};  // end of TMsgStoreSortByField
	
/** Enumerates the sorting order */	
enum TMsgStoreSortOrder
	{
	EMsgStoreSortDescending,
	EMsgStoreSortAscending
	};  // end of TMsgStoreSortOrder
    
/** Enumerates the directions for moving the cursor in an iterator */ 
enum TMsgStoreIteratorDirection
    {
    EMsgStoreIteratorForward,
    EMsgStoreIteratorBackward
    };
    
    /**
     * Special id to indicate retriving from the top of the result set.
     * Can be used when calling CMsgStoreSortResultIterator::NextL()
     */
    const TMsgStoreId KMsgStoreSortResultTop = 0;
    
    /**
     * Special id to indicate retriving from the bottom of the result set.
     * Can be used when calling CMsgStoreSortResultIterator::PreviousL()
     */
    const TMsgStoreId KMsgStoreSortResultBottom = static_cast<TMsgStoreId>(-1);
    

/** Enumerates the search fields supported by the message store. */
enum TMsgStoreSearchFieldMasks {
    EMsgStoreSearchTo      =  1,
    EMsgStoreSearchCc      =  2,
    EMsgStoreSearchBcc     =  4,
    EMsgStoreSearchSubject =  8,
    EMsgStoreSearchBody    = 16
};

/** Folder types to identify the all important root folders. */
enum TMsgStoreFolderType
    {
    EMsgStoreInbox         =  1,
    EMsgStoreOutbox        =  2,
    EMsgStoreDrafts        =  3,
    EMsgStoreSent          =  4,
    EMsgStoreDeleted       =  5,
    EMsgStoreOther         =  6
    };

// =========
// CONSTANTS
// =========
	
/** Defines an ID value that represents an invalid message. */
const TMsgStoreId KMsgStoreInvalidId = 0;

/** Defines root of all mailboxes */
const TMsgStoreId KMsgStoreRootMailBoxId = 1;

/** Defines an ID value that represents the root folder. */
//const TMsgStoreId KMsgStoreRootFolderId = 1;

/** Defines the ID of the predefined inbox folder. */
//const TMsgStoreId KMsgStoreInboxFolderId = 2;
	
/** Defines the ID of the predefined outbox folder. */
//const TMsgStoreId KMsgStoreOutboxFolderId = 3;
	
/** Defines the ID of the predefined sent folder. */
//const TMsgStoreId KMsgStoreSentFolderId = 4;
	
/** Defines the ID of the predefined drafts folder. */
//const TMsgStoreId KMsgStoreDraftsFolderId = 5;

/** Defines the ID of the predefined deleted local folder. */
//const TMsgStoreId KMsgStoreDeletedFolderId = 6;

/** Defines the value used to indicate that the "flags" column was not found. */
const TUint32 KMsgStoreFlagsNotFound = 0xFFFFFFFF;
	
#endif
