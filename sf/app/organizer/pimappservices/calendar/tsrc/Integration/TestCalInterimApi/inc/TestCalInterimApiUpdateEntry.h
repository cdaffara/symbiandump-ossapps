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



#ifndef TESTCALINTERIMAPIUPDATEENTRY_H
#define TESTCALINTERIMAPIUPDATEENTRY_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calcommon.h>
#include <calinstance.h>
#include <caliterator.h>

/**
This test step fetches the entry from the calendar file and then updates the non- recurrence
properties like summary, description, category of the parent or child entry using StoreL() or
UpdateL() api.
*/
class CCalEntryView;
class CTestCalInterimApiUpdateEntry : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiUpdateEntry();
	~CTestCalInterimApiUpdateEntry();

	virtual TVerdict	doTestStepL();

private:
	void	FetchAndUpdateL(const TPtrC& aGuid, const TEntryProperties& aEntryProperties, const TPtrC& aTimeMode);
	void 	UpdateEntryL(CCalEntry* aCalEntry);
	void	StoreEntryL(CCalEntry* aCalEntry);
	void	CheckRecurrenceIdAndSetDataL(RPointerArray<CCalEntry> aEntriesFetched, const TEntryProperties& aEntryProperties, const TPtrC& aTimeMode);
	void	SetDataL(CCalEntry* aCalEntry, const TEntryProperties& aEntryProperties);
	void 	AddCategoriesL(CCalEntry* aCalEntry, const TDesC& aCategories);
	void	AddAttendeesL(CCalEntry* aCalEntry, const TEntryProperties& aEntryProperties);
	void 	ModifyAlarmL(CCalEntry* aCalEntry, const TInt& aAlarmOffset);
	void 	ModifyGEOPropertyL(CCalEntry* aCalEntry, const TDesC& aGeoLat, const TDesC& aGeoLong);
	void 	ModifyTransL(CCalEntry* aCalEntry, const TInt& aTrans);
	void 	PerformanceTestEntryViewL(TReal32& aMaxTimeLimit);
	void	GetAttachmentAndSetDataL(CCalEntry* aCalEntry);
	void	SetAttachmentDataL(CCalAttachment* aCalAttachment, CCalEntry& aEntry);
	void	OOMTestForUpdateEntryL(RPointerArray<CCalEntry> aEntriesToStore, TInt& aEntriesStored);

private:
	TCalLocalUid	iLocalUid;
	TBool			iStoreEntry;
	TBool			iUpdateEntry;
	CCalEntryView*	iEntryView;
	TBool			iAllocTestForUpdate;
	TAttachment		iAttachProperties;
	};

_LIT(KTestCalInterimApiUpdateEntry, "TestCalInterimApiUpdateEntry");

_LIT(KErrDuringUpdateInstancesOperation, 			"Update operation failed with err code %d \n");
_LIT(KUpdateInstancesUsingStoreApi,					"updateusingstoreapi");
/**
 *Test step that updates all the instances present in the calendar file
 */
class CTestCalInterimApiUpdateAllInstances : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiUpdateAllInstances(void);
	virtual ~CTestCalInterimApiUpdateAllInstances(void);
	virtual TVerdict	doTestStepL(void);
	virtual TVerdict 	doTestStepPreambleL(void);

private:
	void PopulateEntriesGuidListL(void);
	void UpdateAllInstancesL(void);

private:
	CCalEntryView*		iEntryView;
	CCalIter*			iEntryIterator;
	CDesC8ArrayFlat*	iGuidList;
	};

_LIT(KTestCalInterimApiUpdateAllInstances,"TestCalInterimApiUpdateAllInstances");

#endif
