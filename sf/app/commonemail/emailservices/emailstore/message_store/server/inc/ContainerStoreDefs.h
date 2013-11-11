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
* Description:  Container store definitions.
*
*/



#ifndef __CONTAINER_STORE_DEFS_H__
#define __CONTAINER_STORE_DEFS_H__

// =========
// CONSTANTS
// =========

const TContainerId KLowestUserCreatedContainerId = 100;
const TContainerId KToBeDeletedContainers        = KLowestUserCreatedContainerId-1;
const TContainerId KUncommittedContainers        = KLowestUserCreatedContainerId-2;
const TContainerId KToRetryDeleteContainers      = KLowestUserCreatedContainerId-3;

// This constant is updated when a backwards incompatible change is made to the DB structure.
const TUint KDbSchemaVersion = 6;

const TUint KNumberOfContentSubdirectories = 25;

// General table
_LIT( KGeneralTableName,                      "G001" );
_LIT( KGeneralTableDbSchemaVersionCol,        "G002" );
_LIT( KGeneralTableNextIdCol,                 "G003" );
_LIT( KGeneralTableAuthenticationRequiredCol, "G004" );  //is authentication required
_LIT( KGeneralTableEncryptionFlagCol,         "G005" );  //bit(encryption on/off)
_LIT( KGeneralTableEncryptionStateCol,        "G006" );  //TEncryptionState, persisted in case power lost during encryption/descryption the entire db
_LIT( KGeneralTableAuthenticationDataCol,     "G007" );
_LIT( KGeneralTableMaxMruCountCol,            "G008" );
_LIT( KGeneralTableCurMruNumberCol,           "G009" );

// Account table
_LIT( KAccountTableName,                  "A001" );
_LIT( KAccountTableOwnerIdCol,            "A002" );
_LIT( KAccountTableAccountNameCol,        "A003" );
_LIT( KAccountTableMailBoxIdCol,          "A004" );
_LIT( KAccountTableTotalCountsCol,        "A005" );
_LIT( KAccountTableIsEncryptedCol,        "A006" );
_LIT( KAccountTableMailBoxIdIndex,        "A007" );

// Property Containers table
_LIT( KContainersTableName,               "C001" );
_LIT( KContainersTableIdCol,              "C002" );
_LIT( KContainersTableParentIdCol,        "C003" );
_LIT( KContainersTableChildrenCountCol,   "C004" );  // total number of children
_LIT( KContainersTableChildrenCountsCol,  "C005" );  // counts based on custom behavior provider
_LIT( KContainersTablePropertiesCol,      "C006" );
_LIT( KContainersTableQuickPropertiesCol, "C007" );
_LIT( KContainersTableCountsCol,		  "C008" );
_LIT( KContainersTableIsRowEncryptedCol,  "C009" );
_LIT( KContainersTableIdIndex,            "C00A" );
_LIT( KContainersTableParentIdIndex,      "C00B" );

// Sorting table
_LIT( KSortingTableName,                  "D001" );
//    column names
_LIT( KSortingTableMessageIdCol,          "D002" );
_LIT( KSortingTableFolderIdCol   ,        "D003" );
_LIT( KSortingTableMailBoxIdCol,          "D004" );
_LIT( KSortingTableReceivedDateCol,       "D005" );
_LIT( KSortingTableReadUnreadCol,         "D006" );
_LIT( KSortingTablePriorityCol,           "D007" );
_LIT( KSortingTableFlagStatusCol,         "D008" );
_LIT( KSortingTableSizeCol,               "D009" );
_LIT( KSortingTableAttachmentFlagCol,     "D00A" );
_LIT( KSortingTableFromCol,               "D00B" );
_LIT( KSortingTableToCol,                 "D00C" );
_LIT( KSortingTableSubjectCol,            "D00D" );
_LIT( KSortingTableIsEncryptedCol,        "D00E" );
//    indexes
_LIT( KSortingTableMessageIdIndex,        "D00F" );
_LIT( KSortingTableFolderIdIndex,         "D00G" );
_LIT( KSortingTableMailBoxIdIndex,        "D00H" );
_LIT( KSortingTableReceivedDateIndex,     "D00I" );
_LIT( KSortingTableReadUnreadIndex,       "D00J" );
_LIT( KSortingTablePriorityIndex,         "D00K" );
_LIT( KSortingTableFlagStatusIndex,       "D00L" );
_LIT( KSortingTableSizeIndex,             "D00M" );
_LIT( KSortingTableAttachmentFlagIndex,   "D00N" );

// Search Result table
_LIT( KSearchResultTableName,             "E001" );
//    column names
_LIT( KSearchResultTableMessageIdCol,     "E002" );
_LIT( KSearchResultTableMatchFlagCol,     "E003" );
//    indexes
_LIT( KSearchResultTableMessageIdIndex,   "E004" );

// MRU Address table
_LIT( KMruAddressTableName,               "F001" );
// Column names
_LIT( KMruAddressTableMruNumCol,          "F002" );
_LIT( KMruAddressTableMailBoxIdCol,       "F003" );
_LIT( KMruAddressTableDisplayNameCol,     "F004" );
_LIT( KMruAddressTableEmailAddressCol,    "F005" );
_LIT( KMruAddressTableIsEncryptedCol,     "F006" );
// indexes
_LIT( KMruAddressTableMruNumIndex,        "F007" );
_LIT( KMruAddressTableMailboxIdIndex,     "F008" );

_LIT(KMarkedForDeleteTableName,                 "H001");
_LIT(KMarkedForDeleteTableIDCol,                "H002");



/** Defines the states of encryption/decryption operation 
 *  This state needs to be persisted in the db, so that if
 *  power lost while encrypting/decrypting, it can continue
 *  after message store is started again.
 */
enum TEncryptionState
{
	EMsgStoreESIdle,
	EMsgStoreESEncrypting,
	EMsgStoreESDecrypting
}; // end TEncryptionState

#endif
