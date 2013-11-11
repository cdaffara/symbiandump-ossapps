// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __AGMSERV_H__
#define __AGMSERV_H__

#include <e32base.h>


const TUint KAgnServerMajorVersion = 0;
const TUint KAgnServerMinorVersion = 1;
const TUint KAgnServerBuildVersion = 1;

const TInt KInitialBufferSize = 1024;	// Initalize buffers to 2K

// Define server name

_LIT(KAgendaServerName,"!AgendaServer");

_LIT(KAgendaServerExe,"AGSVEXE.EXE");

const TInt KCapabilityNone = 0;
const TInt KCapabilityReadUserData = 100;
const TInt KCapabilityWriteUserData = 200;

const TInt KTransferAttachmentToSrvFsArgIndex = 2;
const TInt KTransferAttachmentToSrvFileArgIndex = 3;
const TInt KTransferAttachmentToCliFileArgIndex = 1;

enum TAgendaServerFunction
	{
	// No capability required 
	EOpenAgenda = KCapabilityNone,			
	ECloseAgenda,						
	ETransmitBuffer,					
	EGetInstanceExtractor,	
	EPreviousInstances,			
	ENextInstances,				
	ECreateEntryIterator,				
	EEntryIteratorNext,					
	EGetEntryUidsSinceDate,				
	EGetCategoryListCount,				
	ECategoryFilter,					
	EStartBuildIndex,					
	EGetListFileNames,	
	ECancelTask,				
	EAgnResourceCount,	//	test
	EAgnHeapSizeCount,	//	test
	EAgnSetHeapFailure,	//	test 
	EAgendaFileExists,
 	EDisableChangeBroadcast,
	EEnableChangeBroadcast,
	ERequestChangeNotificationParameters,
	ERequestChangeNotification,
	ECancelChangeNotification,
	ERequestProgress,
	ESetUpdateAlarm,
	ESetEnablePubSubNotification,
	ERestoreAlarmAction,
	ETzDbChangedTime,
	
	// ReadUserData required 
	EFetchEntry = KCapabilityReadUserData,
	EFetchEntryByUID, 						
	EFetchSimpleEntry,	
	EFetchSimpleEntries,	
	ERestoreText,					
	EGetCategoryListItem,
	EGetChangesSinceLastNotification,
	EFindInstances,
	EFetchEntryByGuid,
	EFetchSimpleEntriesByGuid,
	ETransferAttachmentFileToClient,
	EFetchSortedAttachments,
	EEntriesWithAttachment,
	EFetchAttachmentById,
	EInstanceIteratorCreate,
	EInstanceIteratorDestroy,
	EInstanceIteratorNext,
	EInstanceIteratorPrevious,
	EInstanceIteratorCount,
	EInstanceIteratorLocateIndex,
	EFetchSimpleEntryByUID,
	EEntryIteratorPosition,	
	EGetCalendarInfo,
	EGetPropertyValue,
	EGetFileChangesSinceLastNotification,

	// WriteUserData required 
	EUpdateEntry = KCapabilityWriteUserData,						
	EAddEntry,							
	EDeleteEntry,				
	EAddCategoryToList,					
	EDeleteAgendaFile,
	ETidyByDateReadParams,
	ETidyByDateStart,
	ECategoryStart,
	ECategoryStartAsyn,
	ECreateAgendaFile,	
	EDeleteEntriesByLocalUid,
	EDeleteEntryByGuid,
	ECommit,
	ERollback,
	ETransferAttachmentFileToServer,
	ETransferFileToClientToWrite,
	EMoveFileToServer,
	ESetCalendarInfo,
	// Do not put new server functions after here
	EAgnNotSupported 
	};

#endif
