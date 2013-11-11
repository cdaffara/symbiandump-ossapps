// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/
#ifndef __CALASYNCTASK_H__
#define __CALASYNCTASK_H__

#include "calcommon.h"

class CCalCategory;
class CCalEntry;
class CCalSessionImpl;
class MCalProgressCallBack;
class TAgnEntryId;

NONSHARABLE_CLASS(CCalAsyncTaskManager) : public CActive
	{
public:
	enum TAsyncAction 
		{//server should use this as well
		ENoAction,
		EBuildIndex,
		EDeleteEntry,
		EFilterCategory,
		EDeleteCategory
		};

	~CCalAsyncTaskManager();
	static CCalAsyncTaskManager* NewL(CCalSessionImpl& aSessionImpl);
	void StartBuildIndexL(MCalProgressCallBack& aProgressCallBack);
	void StartDeleteEntryL(MCalProgressCallBack& aProgressCallBack, const CalCommon::TCalTimeRange& aCalTimeRange, CalCommon::TCalViewFilter aFilter);
	void StartCategoryTaskL(MCalProgressCallBack& aProgressCallBack, TAsyncAction aAction, const CCalCategory& aCategory, RPointerArray<CCalEntry>* aFilteredEntry = NULL);
	void CancelAsynTask();
	
	TBool BuildingIndex() const;
	TBool IsBusy() const;

private:
	CCalAsyncTaskManager(CCalSessionImpl& aSessionImpl);
	void DoCancel();
	void RunL();
	void GetFilteredEntryL();
	void CompleteRequest(TInt aCompleteCode);

private:
	CCalSessionImpl& 					iSessionImpl;		
	RArray<MCalProgressCallBack*>		iCallBacks;//not owned
	TAsyncAction				iAction;
	TBool						iNeedProgressReport;//may not needed because server knows
	RPointerArray<CCalEntry>* 	iFilteredEntry;//Owned by the caller and will be filled in with array filtered entries
	CArrayFixSeg<TAgnEntryId>* 	iEntryIdList;//Holds an array of entry id temporarily
	};

#endif