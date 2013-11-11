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
* Description:  Message store client server comunication definitions.
*
*/



#ifndef __MESSAGE_STORE_CLIENT_SERVER_H__
#define __MESSAGE_STORE_CLIENT_SERVER_H__

// ========
// INCLUDES
// ========

#include <e32std.h>
//<cmail>
#include "MsgStoreTypes.h"
#include "MsgStoreAccount.h"
//</cmail>

// =========
// CONSTANTS
// =========

_LIT( KMsgStoreServerName, "MessageStoreServer" );

const TInt KMsgStoreMajorVersion = 0;
const TInt KMsgStoreMinorVersion = 1;
const TInt KMsgStoreBuild        = 0;

// =====================
// FUNCTION DECLARATIONS
// =====================

// If aParams == NULL then this function does a thread rendezvous to signal the client, otherwise it
// does a process rendezvous.
IMPORT_C TInt MessageStoreServerThreadFunction( TAny* aParams );

// ================================================================
// Definition of acceptable commands that can be sent to the server
// ================================================================
enum TMsgStoreMessages
	{
    
    // These commands should be accessible only in debug builds
    EMsgStoreCmdShutdownServer       = 0,
    EMsgStoreCmdInjectBackupRestoreEvent, 
    EMsgStoreCmdInjectPointSecEvent,      
    EMsgStoreCmdSimulateLowDiskSpace,     
    EMsgStoreCmdGetEncryptionState,
    
    
	// The following messages do not require any platform security capabilities to use.
    EMsgStoreCmdAuthenticated        = 100,
    EMsgStoreCmdClearAuthentication,
    EMsgStoreCmdChildrenCounts,
    EMsgStoreCmdTotalCounts,
    EMsgStoreCmdChildrenIds,
    EMsgStoreCmdContentLength,
	EMsgStoreCmdStartObserving,
	EMsgStoreCmdGetEvents,
	EMsgStoreCmdStopObserving,	
	EMsgStoreCmdCancelSearch,
    EMsgStoreCmdClearSearchResultCache,
    EMsgStoreCmdGetIteratorGroupCount,
    EMsgStoreCmdGetIteratorIdsAndFlags,
    EMsgStoreCmdSortedIndexOf,
    EMsgStoreCmdGetIteratorIds,
    EMsgStoreCmdGetIteratorIdsAndGroupCount,
    EMsgStoreCmdEndSorting,

    // The following messages require ECapabilityReadUserData to use.

    EMsgStoreCmdAuthenticate,
    EMsgStoreCmdHasPassword,
	EMsgStoreCmdOpenAccount,
	EMsgStoreCmdListAccounts,
    EMsgStoreCmdPropertiesList,
    EMsgStoreCmdFetchProperties,
    EMsgStoreCmdFetchContentToBuffer,
    EMsgStoreCmdFetchContentToFile,
	EMsgStoreCmdPrepareSearch,
	EMsgStoreCmdGetMatches,
    EMsgStoreCmdGetSortedRows,
    EMsgStoreCmdOpenContentFile,
    EMsgStoreCmdGetMruAddressList,
    
    // The following messages require ECapabilityWriteUserData to use.
    
    EMsgStoreCmdEnableEncryption,
    EMsgStoreCmdDisableEncryption,
    EMsgStoreCmdSetPassword,
    EMsgStoreCmdChangePassword,
    EMsgStoreCmdCreateAccount,
    EMsgStoreCmdRenameAccount,
    EMsgStoreCmdDeleteAccount,
	EMsgStoreCmdCreateContainer,
	EMsgStoreCmdCommitContainer,
	EMsgStoreCmdAbandonContainer,
	EMsgStoreCmdMoveContainer,
	EMsgStoreCmdCopyContainer,
    EMsgStoreCmdDeleteContainer,
    EMsgStoreCmdUpdateProperties,
    EMsgStoreCmdUpdateProperty,
    EMsgStoreCmdReplaceContentWithBuffer,
    EMsgStoreCmdReplaceContentWithFile,
    EMsgStoreCmdAppendToContent,
    EMsgStoreCmdPrependToContent,
    EMsgStoreCmdRemoveContent,
    EMsgStoreCmdStartSorting,
    EMsgStoreCmdSetMaxMruAddressCount,
    EMsgStoreCmdSetMruAddressList,
    EMsgStoreCmdBeginBatchInsert,
    EMsgStoreCmdFinishBatchInsert,
    EMsgStoreCmdWipeEverything
  	};

const TMsgStoreMessages KMsgStoreCmdAlwaysPass          = EMsgStoreCmdAuthenticated;
const TMsgStoreMessages KMsgStoreCmdFirstReadUserData   = EMsgStoreCmdAuthenticate;
const TMsgStoreMessages KMsgStoreCmdFirstWriteUserData  = EMsgStoreCmdSetPassword;
const TMsgStoreMessages KMsgStoreCmdLast                = EMsgStoreCmdWipeEverything;


// The first 4 bits of the message ID are used to encode the type of the container.
enum TMsgStoreContainerMasks
	{
	EMsgStoreMailBoxBits    = 0x00000000,
	EMsgStoreFolderBits     = 0x10000000,
	EMsgStoreMessageBits    = 0x20000000,
	EMsgStorePartBits       = 0x30000000,
	EMsgStoreContainerMask  = 0xF0000000,
	};

struct TMsgStoreCounts
	{
	TUint iMessagesCount;
	TUint iUnreadCount;
	};
	
const TUint KMsgStoreCountsLength = sizeof(TMsgStoreCounts);

// iType will be a value of TMsgStoreOperation, TMsgStoreSystemEvent, or TMsgStoreAccountEvent
// For system events, only the type field is used.
struct TMsgStoreEvent
	{
	TMsgStoreId iMailBoxId;
	TUint       iType;
	TMsgStoreId iId;
	TMsgStoreId iParentId;
	TMsgStoreId iOtherId;
	TUint32     iFlags;	
	TBuf8<KMaxAccountNameLen> iAccountName;
	TBuf8<KMaxAccountNameLen> iNewAccountName;   //for rename account only
	}; // end TMsgStoreEvent
    
struct TMsgStoreCopyContainerCmdParams
    {    
    TMsgStoreId iId;
    TMsgStoreId iSourceId;
    TMsgStoreId iSourceParentId;
    TMsgStoreId iDestinationId;
    TMsgStoreId iDestinationParentId;
	TMsgStoreId iMailBoxId;
    }; // end TMsgStoreCopyContainerCmdParams
    
struct TMsgStoreFetchPropertiesCmdParams
	{
	TMsgStoreId iId;
	TMsgStoreId iGrandparentId;
	TMsgStoreId iMailboxId;
	}; // end TMsgStoreFetchPropertiesCmdParams
	
struct TMsgStorePropertiesListCmdParams
	{
	// input parameters
	TBool iStartOver;
	TBool iUseQuery;
	TBool iQuickProperties; // only supported if iUseQuery = ETrue
	
	// output parameters
	TBool iMoreProperties;     
	TBool iClearExisting;
	TUint iNewBufferSize;
	}; // end TMsgStoreChildrenPropertiesCmdParams

struct TMsgStorePropertiesListQueryParams
	{
	TMsgStoreId             iId;        
	TMsgStoreId             iParentId;  
	TMsgStoreContainerMasks iType;      
	TBool                   iRecursive; 
	}; // end TMsgStoreChildrenPropertiesCmdParams

struct TMsgStoreCreateContainerCmdParams
	{
	TMsgStoreId             iParentId;
	TMsgStoreId             iGrandparentId;
	TMsgStoreId             iMailBoxId;
	TBool                   iCommitContainer;
	TMsgStoreContainerMasks iType;
	TBool                   iHasContent;
	TMsgStoreId             iId;  // out parameter
	}; // end TMsgStoreCreateContainerCmdParams
	
struct TMsgStoreUpdatePropertyCmdParams
	{
	TMsgStoreId iId;
	TMsgStoreId iParentId;
	TMsgStoreId iMailBoxId;
	TUint8      iType;
	}; // end TMsgStoreUpdatePropertyCmdParams
	
struct TMsgStoreOpenAccountCmdParams
	{
	TInt32       iOwnerId;
	TMsgStoreId  iMailBoxId;
	}; // end TMsgStoreOpenAccountCmdParams
    
/** Defines the search criteria (not including the leading string to be ignored ) */
struct TMsgStoreSortCriteria 
    {
    /** Specifies the mailbox id
     */
    TMsgStoreId          iMailBoxId;
    
    /** Specifies the folder in which the sorting is to be perform
     */
    TMsgStoreId          iFolderId;
    
    /** Specifies the sort field
     */
    TMsgStoreSortByField iSortBy;
    
    /** Specifies the sort order
     */
    TMsgStoreSortOrder   iSortOrder;
    
    /** defines the order for the secondary field (received date) when primary field is NOT EMsgStoreSortByReceivedDate
     */
    TMsgStoreSortOrder   iSecondarySortOrder;  
    };
    
struct TMsgStoreGetSortedRowsCmdParams
    {
    TMsgStoreId                 iSortSessionId;
    TMsgStoreId                 iStartingMessageId;
    TMsgStoreIteratorDirection  iDirection;
    TUint                       iRowsToRetrieve;
    TBool                       iHasStartWith;
    TBool                       iSkipCurrentGroup;
    TBool                       iHasMoreRows;
    };
    
struct TMsgStoreSearchCmdParams
    {
    TUint                 iSearchFields;
    TMsgStoreSortByField  iSortBy;
    TMsgStoreSortOrder    iSortOrder;
    TMsgStoreSortOrder    iSecondarySortOrder;  
    };

#endif // __MESSAGE_STORE_CLIENT_SERVER_H__



