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



#ifndef TEST_CALINTERIMAPI_REMOVEMEDIA_H
#define TEST_CALINTERIMAPI_REMOVEMEDIA_H

#include <calsession.h>
#include <calcategory.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calcategorymanager.h>
#include <caliterator.h>
#include <calinstance.h>
#include <e32std.h>
#include "TestCalInterimApiReadData.h"


_LIT(KTestCalInterimApiRemoveMedia,		        	"TestCalInterimApiRemoveMedia");
_LIT(KRemoveMediaAddOperation, 						" \n Addition of Entries in Progress..... \n");
_LIT(KRemoveMediaModifyOperation, 					" \n Updation of Entries in Progress..... \n");
_LIT(KRemoveMediaDeleteOperation, 					" \n Deletion of Entries in Progress..... \n");
_LIT(KRemoveMediaDeleteType, 						"DeleteType");
_LIT(KRemoveMediaCalendarFileName,					"calendarfilename");
_LIT(KRemoveMediaErrAddOperation,					"Add operation failed with err code %d \n");
_LIT(KRemoveMediaErrUpdateOperation,				"Update operation failed with err code %d \n");
_LIT(KRemoveMediaErrDeleteOperation,				"Delete operation failed with err code %d \n");
_LIT(KRemoveMediaLoadEntriesFlag, 					"load");
_LIT(KRemoveMediaAddEntriesFlag,					"addentries");
_LIT(KRemoveMediaUpdateEntriesFlag,					"updateentries");
_LIT(KRemoveMediaDeleteEntriesFlag,					"delete");
_LIT(KRemoveMediaDeleteTypeFlag,					"deletetype");
_LIT(KRemoveMediaCategories,						"categories");
_LIT(KRemoveMediaAddCategories,						"addcategories");
_LIT(KRemoveMediaRemoveCategories,					"removecategories");
_LIT(KRemoveMediaValidateEntryFlag, 				"validateentry");
_LIT(KRemoveMediaEntryExpected, 					"entryexpected");
_LIT(KRemoveMediaEntryGuid, 						"entryguid");
_LIT(KRemoveMediaEntryRecurrenceId, 				"entryrecurrenceid");
_LIT(KRemoveMediaFetchEntriesFlag,					"fetchentries");
_LIT(KRemoveMediaDataFile,							"datafile");
_LIT(KRemoveMediaCalenderFile,						"calenderfile");
_LIT(KRemoveMediaDeleteByReference, 				"deletebyreference");
_LIT(KRemoveMediaDeleteByUidList,					"deletebyuidlist");
_LIT(KRemoveMediaDeleteByArrayOfLocalUids,			"deletebylocaluids");
_LIT(KRemoveMediaDeleteByTimeRange, 				"deletebytimerange");
_LIT(KRemoveMediaDeleteInstances,					"deleteinstances");
_LIT(KRemoveMediaFileCompValidationFlag,			"filecompressionflag");
_LIT(KRemoveMediaEntryCountValidationFlag,			"entrycountflag");
_LIT(KRemoveMediaBasicCalendarOperations,			"basicoperationflag");
_LIT(KRemoveMediaBasicCalendarOperation,			"basicoperation");
_LIT(KRemoveMediaExpectedEntryCount, 				"expectedentrycount");
_LIT(KRemoveMediaAttachmentExists,	 				"attachmentexists");
_LIT(KRemoveMediaInfoCalendarEntryCount,      	 	"Total number of entries present in the calendar file is %d \n");
_LIT(KRemoveMediaErrAddCategoryOperation,    	  	"Add Category operation failed with err code %d \n");
_LIT(KRemoveMediaErrRemoveCategoryOperation,  	 	"Delete Category operation failed with err code %d \n");
_LIT(KRemoveMediaErrStoreOperation,					"Error Occurred during Store Operation -  %d \n");
_LIT(KRemoveMediaErrUnableToOpenCalFile,			"Error Occurred during Calendar file open Operation -  %d \n");
_LIT(KRemoveMediaErrUnableToRetrieveCalFileSize, 	"Error Occurred while fetching the Calendar file size -  %d \n");
_LIT(KRemoveMediaEntryCountMismatch, 				"Entry Count Mismatch... The expected entry count is %d, while the actual entry count %d \n");
_LIT(KRemoveMediaErrFileCompression,				"File compression feature failed \n");
_LIT(KRemoveMediaErrFetchOperation, 				"Error occurred during fetch operation \n");
_LIT(KRemoveMediaErrEntriesNotdeleted, 		 		"Calendar Entires not deleted \n");
_LIT(KRemoveMediaErrFindInstanceOperation,			"Error Occurred during find instances Operation -  %d \n");
_LIT(KRemoveMediaErrFileCompressionMechanismFailed, "File compression mechanism failed. The Original file Size is %d And the current File Size is %d \n");
_LIT(KRemoveMediaErrFindInstancesOperation, 		"Error Occurred during Find Instances Operation -  %d \n");
_LIT(KRemoveMediaUnDesiredPresent, 					"Undesired Entry Present \n");
_LIT(KRemoveMediaDesiredEntryNotPresent, 			"Desired entry not present \n");

/**
 * Teststep that performs various calendar operations like deletion of Calendar entries, find Instances
 * fetch entries, Add categories, remove categories etc... During these operations the simulation of Removing of
 * Media is done set the File System to throw KErrNotReady. Check is done as part of validation of expected behaviour
 * Further the replacement of media is simulated by setting the file system to throw KErrNone. A new Calendar session
 * is opened and the Calendar operations are performed again.
 */
class CTestCalInterimApiRemoveMedia : public CTestCalInterimApiReadData
	{
public:
	CTestCalInterimApiRemoveMedia(void);
	~CTestCalInterimApiRemoveMedia(void);
	virtual TVerdict	doTestStepL(void);
	virtual void		ProcessEntryL(CCalEntry* aEntry);

	enum TCalendarOperations
		{
	 	EAddEntries,
	 	EUpdateEntries,
	 	EAddCategories,
	 	ERemoveCategories,
	 	EFetchEntries
		};

	enum TDeleteType
		{
		EDeleteByUidList,
		EDeleteByReference,
		EDeleteByLocalUids,
		EDeleteByTimeRange,
		EDeleteByInstances
		};


private:
	void			BuildCalendarEntitiesL(void);
	void 			CalendarOperationsL(void);
	void 			AddEntriesInRemovableMediaL(void);
	void 			UpdateEntriesInRemovableMediaL(void);
	void 			DeleteEntriesInRemovableMediaL(void);
	void 			AddCategoriesInRemovableMediaL(void);
	void 			RemoveCategoriesInRemovableMediaL(void);
	void 			FetchEntriesInRemovableMediaL()	;
	void			AddEntriesL( TInt& aNumEntriesStored);
	void 			UpdateEntriesL(TInt& aNumEntriesModified);
	void			DeleteEntriesByUidListL(void);
	void			DeleteEntriesByReferenceL(void);
	void			DeleteEntriesByDateTimeRangeL(void);
	void			ResetExpectedNotificationCountL(void);
	void 			DeleteEntriesByArrayOfLocalUidsL(TInt& aNumEntriesDeleted);
	void			DeleteEntriesByTimeRangeL(void);
	void			DeleteInstancesL(void);
	void			AddCategoriesL(TInt& aEntriesStoredOrUpdated);
	void			RemoveCategoriesL(TInt& aEntriesStoredOrUpdated);
	void			FetchEntriesL(TInt& aEntriesStoredOrUpdated);
	void 			RetreiveParentEntriesL(RPointerArray<CCalEntry>& aParentEntryArray);
	void 			PopulateEntriesGuidListL(void);
	void 			ResetConditionsL(void);
	void 			SetFsErrorCondition(TInt aErrCode, TInt aCount = 0);
	void 			Validate(TInt aActualResult, TInt aDesiredResult);
	TInt 			EntryCountL(void);
	void 			ValidateEntryCount(TInt aExpectedEntryCount);
	void 			Validate(TBool aFlag);
	void 			SetMinMaxTimeL(void);
	void 			ReleaseResourcesL(void);
	void 			ReadEntriesL(void);
	void 			LoadEntriesL(void);
	void 			OpenCalendarL(void);
	void 			BasicCalendarOperationsInRemovableMediaL(void);
	void			BasicCalendarOperationsWithMediaRemovedL(void);
	void			BasicCalendarOperationsWithMediaReInsertedL(void);
	void 			BasicCalendarOperationsL(TInt& aEntriesStoredOrUpdated);
	void			DeleteCalendarOperationsWithMediaRemovedL(void);
	void			DeleteCalendarOperationsWithMediaReInsertedL(void);
	void 			DeleteCalendarEntriesL(void);
	void 			ValidateEntryExistsL(void);
	void 			ValidateEntry(const CCalEntry* aEntry, TBool aEntryExpected);
	void			FetchEntriesForUpdateL();
	CTestCalInterimApiRemoveMedia::TCalendarOperations GetCalendarOperation(TPtrC aSpecificOperationFlag);
	CTestCalInterimApiRemoveMedia::TDeleteType GetDeleteType(TPtrC aDeleteType);

	// override from CTestCalInterimApiSuiteStepBase
	void 			Completed(TInt aError);

private:
	CCalEntryView*						iEntryView;
	CCalInstanceView*					iInstanceView;
	CCalIter*							iEntryIterator;
	CDesC8ArrayFlat*					iGuidList;
	RPointerArray<CCalEntry>            iEntryArray;
	RPointerArray<CCalEntry>            iUpdateEntryArray;
	TCalTime							iCalMinTime	;
	TCalTime							iCalMaxTime	;
	};

#endif
