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

#ifndef __AGSEXTRACTOR_H__
#define __AGSEXTRACTOR_H__

#include "calinstanceviewimpl.h"
#include "agmdate.h"
#include "agsindexiterator.h"

class CAgnEntryModel;
class CAgnIndex;
class CAgnSimpleEntryTable;
class TAgnIterator;
class TAgnSortInstance;

/** The instance extractor.

The extractor is given an iterator that it will use to iterate through entries and create instances.

@internalComponent
*/
class CAgnInstanceExtractor : public CBase
	{
public:	
	CAgnInstanceExtractor(TAgnIterator* aIterator, CAgnEntryModel& aEntryModel);
	~CAgnInstanceExtractor();
	
	inline void SetIndex(CAgnIndex& aSimpleEntryIndex);
	inline void SetFilter(const TAgnFilter& aFilter);
	inline void SetUndatedTodoTime(const TTime& aUndatedTodoTime);

	void FindInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams);
	void NextInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams);			
	void PreviousInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams);

private:
	inline const TAgnFilter* Filter() const;
	inline const TTime& UndatedTodoTime() const;
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
	TBool ContinueSearchL(const TBool aAlarmInstanceSearch, const TTime& aSearchRangeEndLocal) const;

	void InsertInstanceL(const TBool aAlarmInstanceSearch, 
						 CArrayFix<TAgnSortInstance>& aInstances, 
						 TAgnSortInstance& aInstance,
						 const TFindInstanceParams& aFindInstanceParams) const;
#endif	
	
private:
	CAgnEntryModel& iEntryModel;
	TTime iUndatedTodoTime;
	
	TAgnIterator* iIterator;
	};

/** An interface that groups together instance extraction operations

@internalComponent
*/
class TAgnInstanceExtractor
	{
public:
	TAgnInstanceExtractor(CAgnSimpleEntryTable& aEntryInfoTable);
	void FindInstancesL(CArrayFix<TAgnSortInstance>& aList, const TFindInstanceParams& aSearchParams);
	void NextPossibleInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams);
	void PreviousPossibleInstancesL(CArrayFix<TAgnSortInstance>& aInstances, const TFindInstanceParams& aSearchParams);

private:
	CAgnSimpleEntryTable* iSimpleEntryTable;
	};

#include "agsextractor.inl"

#endif
