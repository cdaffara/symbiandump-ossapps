// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Header file for Phonebook Synchroniser common code. Contains definitions
// of classes, methods and types used by both the client side and
// server/engine side code.
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __COMMON_H__
#define __COMMON_H__


/**
 *  Phonebook Sync Server and Background Sync Engine IPC requests.
 *
 *  @note When adding IPC Requests please update
 *        KPhonebookSyncRequestNames as well in the session files.
 */
enum TPhonebookSyncRequest
	{
	ESyncDoSynchronisation,
	ESyncGetContactFormat,
	ESyncGetSyncMode,
	ESyncSetSyncMode,
	ESyncDeleteCntFromICC,
	ESyncWriteCntToICC,
	ESyncValidateContact,
	ESyncUpdateLookupTable,
	ESyncGetPhoneBookId,
	ESyncGetCacheState,
	ESyncDbgMarkHeap,
	ESyncDbgCheckHeap,
	ESyncDbgMarkEnd,
	ESyncDbgFailNext,
	ESyncDoSynchronisationCancel,
	ESyncDeleteCntFromICCCancel,
	ESyncWriteCntToICCCancel,
	ESyncNotifyCacheStateChange,
	ESyncNotifyCacheStateChangeCancel,
	ESyncGetLastSyncError,
	ESyncGetNumSlots,
	ESyncGetNumFreeSlots,
	ESyncGetFreeSlots,
	ESyncGetSlotId,
	ESyncFlushInterfaceQueue,
	ESyncShutdownServer
	};


/**
 *  Panic codes.
 */
enum TPhBkSyncPanic
	{
	EPhBkSyncPanicUnexpectedLeave = 0,
	EPhBkSyncPanicCreateTrapCleanup = 1,
	EPhBkSyncPanicSvrCreateServer = 2,
	EPhBkSyncPanicSvrStartServer = 3,
	EPhBkSyncPanicDoMultipleReadError = 4,
	EPhBkSyncPanicDoIccWriteError = 5,
	EPhBkSyncPanicDoIccDeleteError = 6,
	EPhBkSyncPanicUpdateEntryInTableError = 7,
	EPhBkSyncPanicGetPhoneStoreInfoError = 8,
	EPhBkSyncPanicNotifySecurityEventError = 9,
	EPhBkSyncPanicNotifySATUpdatesError = 10,
	EPhBkSyncPanicNotifyAppInfoChangeError = 11,
	EPhBkSyncPanicEnumerateUSimAppsError = 12,
	EPhBkSyncPanicCancelSyncReqError = 13,
	EPhBkSyncPanicConnecttoFileServerError = 14,
	EPhBkSyncPanicIccUidError = 15,
	EPhBkSyncPanicInvalidIniValue = 16,
 	EPhBkSyncPanicGroupIdAlreadySet = 17,
 	EPhBkSyncPanicGroupIdNotSet = 18,
 	EPhBkSyncPanicTemplateIdAlreadySet = 19,
 	EPhBkSyncPanicTemplateIdNotSet = 20,
 	EPhBkSyncPanicInvalidContactId = 21,
 	EPhBkSyncPanicContactIdNotNull = 22,
	EPhBkSyncPanicNotifyLockInfoChangeError = 23,
	EPhBkSyncPanicTooManyPhonebooks = 24,
	EPhBkSyncPanicInvalidEngineRequest = 25,
	EPhBkSyncPanicIllegalFunction = 26,
	EPhBkSyncPanicBadDescriptor = 27,
	EPhBkSyncPanicNullHandle = 28,
	EPhBkSyncPanicHandleNotClosed = 29,
	EPhBkSyncPanicNotificationAlreadyPending
	};


/**
 *  Panic function. Can be used to panic the server or client.
 */
void PhBkSyncPanic(TPhBkSyncPanic aPanicCode);


#endif // __COMMON_H__
