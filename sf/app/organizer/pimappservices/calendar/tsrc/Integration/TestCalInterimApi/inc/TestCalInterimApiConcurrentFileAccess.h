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



#ifndef __TEST_CALINTERIMAPI_CONCURRENT_FILEACCESS_H__
#define __TEST_CALINTERIMAPI_CONCURRENT_FILEACCESS_H__

#include "TestCalInterimApiSuiteStepBase.h"
#include <e32std.h>

#include <calsession.h>
#include <calcategory.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calcategorymanager.h>
#include <calalarm.h>
#include <caluser.h>
#include <caliterator.h>
#include <calrrule.h>
#include <calinstance.h>
#include <e32std.h>
#include <f32file.h>
#include <bautils.h>


_LIT(KTestCalInterimApiConcurrentFileAccess,		        "TestCalInterimApiConcurrentFileAccess");
_LIT(KConcurrentFileAccessAddOperation, 					" \n Addition of Entries in Progress..... \n");
_LIT(KConcurrentFileAccessModifyOperation, 					" \n Updation of Entries in Progress..... \n");
_LIT(KConcurrentFileAccessDeleteOperation, 					" \n Deletion of Entries in Progress..... \n");
_LIT(KConcurrentFileAccessDeleteType, 						"DeleteType");
_LIT(KConcurrentFileAccessCalendarFileName,					"calendarfilename");
_LIT(KConcurrentFileAccessErrAddOperation,					"Add operation failed with err code %d \n");
_LIT(KConcurrentFileAccessErrUpdateOperation,				"Update operation failed with err code %d \n");
_LIT(KConcurrentFileAccessErrDeleteOperation,				"Delete operation failed with err code %d \n");
_LIT(KConcurrentFileAccessAddEntriesFlag,					"add");
_LIT(KConcurrentFileAccessUpdateEntriesFlag,				"update");
_LIT(KConcurrentFileAccessDeleteEntriesFlag,				"delete");
_LIT(KConcurrentFileAccessDeleteTypeFlag,					"deletetype");
_LIT(KConcurrentFileAccessCategories,						"categories");
_LIT(KConcurrentFileAccessAddCategories,					"addcategories");
_LIT(KConcurrentFileAccessRemoveCategories,					"removecategories");
_LIT(KConcurrentFetchEntriesFlag,							"fetchentries");
_LIT(KConcurrentFindInstancesFlag,							"findinstances");
_LIT(KConcurrentFileAccessDataFile,							"datafile");
_LIT(KConcurrentFileAccessCalenderFile,						"calenderfile");
_LIT(KConcurrentFileAccessDeleteByReference, 				"deletebyreference");
_LIT(KConcurrentFileAccessDeleteByUidList,					"deletebyuidlist");
_LIT(KConcurrentFileAccessDeleteByArrayOfLocalUids,			"deletebylocaluids");
_LIT(KConcurrentFileAccessDeleteByTimeRange, 				"deletebytimerange");
_LIT(KConcurrentFileAccessDeleteInstances,					"deleteinstances");
_LIT(KConcurrentFileAccessFileCompValidationFlag,			"filecompressionflag");
_LIT(KConcurrentFileAccessEntryCountValidationFlag,			"entrycountflag");
_LIT(KConcurrentExpectedEntryCount, 						"expectedentrycount");
_LIT(KConcurrentFileAccessInfoCalendarEntryCount,          	"Total number of entries present in the calendar file is %d \n");
_LIT(KConcurrentFileAccessErrAddCategoryOperation,      	"Add Category operation failed with err code %d \n");
_LIT(KConcurrentFileAccessErrRemoveCategoryOperation,   	"Delete Category operation failed with err code %d \n");
_LIT(KConcurrentFileAccessErrStoreOperation,				"Error Occurred during Store Operation -  %d \n");
_LIT(KConcurrentFileAccessErrUnableToOpenCalFile,			"Error Occurred during Calendar file open Operation -  %d \n");
_LIT(KConcurrentFileAccessErrUnableToRetrieveCalFileSize, 	"Error Occurred while fetching the Calendar file size -  %d \n");
_LIT(KConcurrentFileAccessEntryCountMismatch, 				"Entry Count Mismatch... The expected entry count is %d, while the actual entry count %d \n");
_LIT(KConcurrentFileAccessErrFileCompression,				"File compression feature failed \n");
_LIT(KConcurrentFileAccessErrFetchOperation, 				"Error occurred during fetch operation \n");
_LIT(KConcurrentFileAccessErrEntriesNotdeleted, 			"Calendar Entires not deleted \n");
_LIT(KConcurrentFileAccessErrFindInstanceOperation,			"Error Occurred during find instances Operation -  %d \n");
_LIT(KConcurrentFileAccessErrFileCompressionMechanismFailed, "File compression mechanism failed. The Original file Size is %d And the current File Size is %d \n");
_LIT(KConcurrentFileAccessErrFindInstancesOperation, 		"Error Occurred during Find Instances Operation -  %d \n");



/**
 * Teststep that supports various calendar operations like deletion of Calendar entries, find Instances
 * fetch entries, Add categories, remove categories etc...
 */
class CTestCalInterimApiConcurrentFileAccess : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiConcurrentFileAccess();
	~CTestCalInterimApiConcurrentFileAccess();

	virtual TVerdict	doTestStepL();
	void 				CalendarOperationsL();
	void 				DeleteEntriesByUidListL();
	void 				DeleteEntriesByReferenceL();
	void 				DeleteEntriesByDateTimeRange();
	void   				ResetExpectedNotificationCount();
	void 				DeleteEntriesByArrayOfLocalUidsL();
	void 				DeleteEntriesByTimeRangeL();
	void 				DeleteInstancesL();
	void  			  	AddCategoriesL();
	void  			  	RemoveCategoriesL();
	void 				ValidateEntryCountL();
	TInt 				EntryCountL();
	void 				AddModifyDeleteL();
	void 				FetchEntriesL();
	void 				FindInstancesL();
	void 				PopulateEntriesGuidListL();

public:
	CCalEntryView*						iEntryView;
	CCalInstanceView*					iInstanceView;
	CCalIter*							iEntryIterator;
	CDesC8ArrayFlat*					iGuidList;

	};

#endif
