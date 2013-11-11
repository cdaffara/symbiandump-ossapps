// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGSASYNCDELETE_H__
#define __AGSASYNCDELETE_H__

#include <e32base.h>

class CAgnEntryModel;
class CAgnSimpleEntry;
class CAgnSimpleEntryTable;
class TAgnChangeFilter;
class TAgnEntryDateIterator;
class TAgnEntryId;
class TAgnFilter;

class CCalAsyncDelete : public CBase
	{
public:
	static CCalAsyncDelete* NewL(CAgnEntryModel& aModel, TAgnChangeFilter& aChangeFilter, CAgnSimpleEntryTable& aEntryTable);
	~CCalAsyncDelete();

	TBool SetUpDeleteL(const TAgnFilter& aFilter, const TTime& aTodaysDate, const TTime& aStartDate, const TTime& aEndDate);
	TInt DoDeleteStepL();
	
private:
	class TStreamIdKey : public TKeyArrayFix
		{
	public:
		inline TStreamIdKey() : TKeyArrayFix(0,ECmpTUint32) {};
		virtual TInt Compare(TInt aLeft, TInt aRight) const;	
		};
		
	CCalAsyncDelete(CAgnEntryModel& aModel, TAgnChangeFilter& aChangeFilter, CAgnSimpleEntryTable& aEntryTable);
	void ConstructL();

	void TidyEntryL(const CAgnSimpleEntry& aSimpleEntry);
	TBool FallInTimeRangeL(const CAgnSimpleEntry& aSimpleEntry);
	TBool CheckDateIteratorEnd() const;
	void DeleteTidiedEntriesL();
	TBool IsEntryValidToTidyL(const CAgnSimpleEntry& aSimpleEntry);

private:
	CArrayFixFlat<TAgnEntryId>*		iTidyDeleteArray; 
	CArrayFixFlat<TAgnEntryId>*		iAlreadyTidied;
	TAgnEntryDateIterator*			iTidyIter;
	TTime							iTidyStartDate;
	TTime							iTidyEndDate;
	TTime							iTidyDate;
	TInt							iNumEntriesProcessed;
	TInt							iNumEntriesToProcess;
	CAgnEntryModel&					iModel;
	TAgnChangeFilter&				iChangeFilter;
	CAgnSimpleEntryTable& 			iEntryTable;
 	};

#endif


