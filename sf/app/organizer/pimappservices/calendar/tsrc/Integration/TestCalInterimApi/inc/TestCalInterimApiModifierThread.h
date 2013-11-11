/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/



#ifndef __TEST_CALINTERIMAPI_MODIFIER_THREAD_H__
#define __TEST_CALINTERIMAPI_MODIFIER_THREAD_H__

#include "TestCalInterimApiReadData.h"
#include <e32std.h>


#include <calsession.h>
#include <calcategory.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calalarm.h>
#include <caluser.h>
#include <calrrule.h>
#include <calinstance.h>
#include <calchangecallback.h>


_LIT(KThreadName,               					"TestCalInterimApiObserverThread");
_LIT(KTestCalInterimApiModifierThread,				"TestCalInterimApiModifierThread");
_LIT(KChunkName, 									"TestCalInterimModifierObserverChunk");

_LIT(KUndatedTodos,             					"undatedtodos");
_LIT(KFilterStartTime,          					"filterstarttime");
_LIT(KFilterEndTime,            					"filterendtime");
_LIT(KFilterEntryType,           				 	"filterentrytype");
_LIT(KSemaphoreName,            					"semaphorename");
_LIT(KCalendarFileName,         					"calenderfile");
_LIT(KDataFile,										"datafile");
_LIT(KInRangeFlag,									"inrange");
_LIT(KDeleteType, 									"deletetype");
_LIT(KDeleteByUid, 									"byuid");
_LIT(KDeleteByReference, 							"byreference");
_LIT(KDeleteByLocalUids,							"bylocaluids");
_LIT(KDeleteByTimeRange,							"bytimerange");
_LIT(KDeleteByInstances,							"byinstances");
_LIT(KStartNotifier,								"startnotifier");
_LIT(KStopNotifier,									"stopnotifier");
_LIT(KReEnableNotifier,								"reenablenotifier");
_LIT(KDisableAndEnableBroadCast,					"disableandenablebroadcast");
_LIT(KAddEntriesFlag,								"add");
_LIT(KUpdateEntriesFlag,							"update");
_LIT(KDeleteEntriesFlag,							"delete");
_LIT(KDisableBroadCast,								"disablebroadcast");
_LIT(KEnableBroadCast,								"enablebroadcast");
_LIT(KTypeToDo,										"todo");
_LIT(KTypeEvent,									"event");
_LIT(KTypeALL,										"all");
_LIT(KFilterTimeMode,								"timemode");
_LIT(KFilterTimeModeLocal,							"local");
_LIT(KFilterTimeModeLocalFloating,					"localfloating");
_LIT(KDeleteFilterStartTime,						"delfilterstarttime");
_LIT(KDeleteFilterEndTime,							"delfilterendtime");
_LIT(KBulkEntries,									"bulkentries");



_LIT(KInfoDatafile,									"\n datafile=%S \n");
_LIT(KAddOperation, 								"\n Addition of Entries in Progress..... \n");
_LIT(KModifyOperation, 								"\n Updation of Entries in Progress..... \n");
_LIT(KDeleteOperation, 								"\n Deletion of Entries in Progress..... \n");
_LIT(KInfoObserverThread,							"\n Observer Thread Successfully Launched \n");
_LIT(KErrAddOperation,								"\n Add operation failed with err code %d \n");
_LIT(KErrUpdateOperation,							"\n Update operation failed with err code %d \n");
_LIT(KErrDeleteOperation,							"\n Delete operation failed with err code %d \n");
_LIT(KTotalNumOfEntries,							"\n The total number of entries in the calendar file is %d \n");
_LIT(KTotalNotificationCount,						"\n The total notification count is %d \n");
_LIT(KErrDatafileDataMissing,						"\n datafile data missing \n");
_LIT(KErrObserverThread,							"\n Unable to launch Observer Thread, Failed with error %d \n");



LOCAL_D const TInt KMaxHeapSize = 0x20000;
GLDEF_C TInt RunObserverThread(TAny* aTestStep); // Observer thread entry point

/**
 * Teststep that launches an observer thread & performs add, modify & delete of Calendar entries
 * It also populates an array of expected notifications which are validated by the observer
 */
class CTestCalInterimApiModifierThread : public CTestCalInterimApiReadData
	{
public:
	CTestCalInterimApiModifierThread();
	~CTestCalInterimApiModifierThread();

	virtual TVerdict	doTestStepL();
	virtual void		ProcessEntryL(CCalEntry* aEntry);

	void 	AddModifyDeleteEntriesL(const TBool& aInRange, const TBool& aNotificationExpected, const TBool& aDisableChangeBroadCast, const TPtrC& aDeleteFlag);
	void    AddEntriesL(const TBool& aInRange, const TBool& aDisableChangeBroadCast);
    void 	UpdateEntriesL(const TBool& aInRange);
	void 	StartNotifierInObserverThread(TBool aStart);
	void 	AlterCalChangeBroadcast(TBool aEnableBroadCast);
	void    ResetExpectedNotificationCount(const TBool& aNotificationExpected);
	void 	ResumeObserverThread();
	void 	LaunchObserverThreadL();
	void	CreateGlobalChunkL();
	void 	CreateNotificationFilterL();
	void    RetreiveIniDataL();
	void 	StartNotifierAndTestL(const TBool& aInRangeFlag, const TPtrC& aDeleteType);
	void  	StopNotifierAndTestL(const TBool& aInRangeFlag, const TPtrC& aDeleteType);
	void  	ReEnableNotifierAndTestL(const TBool& aInRangeFlag, const TPtrC& aDeleteType);
	void 	DisableBroadCastAndTestL(const TBool& aInRangeFlag, const TPtrC& aDeleteType);
	void 	EnableBroadCastAndTestL(const TBool& aInRangeFlag, const TPtrC& aDeleteType);
	void 	StopExecution();
	void 	ResetExpectedNotificationArray();
	void 	DeleteEntriesByUidListL(const TBool& aInRange);
	void 	DeleteEntriesByReferenceL(const TBool& aInRange);
	void 	DeleteEntriesByTimeRangeL(const TBool& aInRange);
	void 	DeleteInstancesL(const TBool& aInRange);
	void 	DeleteEntriesByArrayOfLocalUidsL(const TBool& aInRange);
	void 	PopulateNotificationOfEntriesBeingDeletedL(const TBool& aInRange, TBool parentUpdate);
	MCalChangeCallBack2::TChangeEntryType  GetFilterEntryType(const TPtrC& aFilter);

public:
	RPointerArray<CCalEntry>	        iEntryArray;
	RChunk								iChunk;
	RSemaphore							iSemaphore;
	TInt								iNumExpectedChanges;
	TCalChangeEntry*					iExpectedChanges;
	TFileName							iFileName;
	TPtrC								iSemName;
	TBool								iStopChangeNotification;
	TBool                               iExecutionCompleted;
	TBool							 	iNotifierOff;
	CCalChangeNotificationFilter*		iFilter;
	RThread								iThread;
	};

#endif
