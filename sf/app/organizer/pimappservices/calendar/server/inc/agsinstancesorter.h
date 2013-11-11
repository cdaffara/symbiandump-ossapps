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

#ifndef __AGSINSTANCESORTER_H_
#define __AGSINSTANCESORTER_H_

#include <e32std.h>
#include <e32base.h>
#include <calentry.h>

class TAgnSortInstance;
class CAgnSortCriteria;

typedef TInt (*InstanceCompareFptr)(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond);

class TAgsInstanceSortUtil
	{
public:	
	static InstanceCompareFptr GetCompareFptr(CalCommon::TCalSortAttribute aSortAttr);
	static TInt CompareStartTime(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond);
	static TInt CompareEndTime(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond);
	static TInt CompareLastModifiedTime(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond);
	static TInt ComparePriority(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond);

private:
	static TInt CompareByTime(const TTime& aFirst, const TTime& aSecond);
	};

class TAgsInstanceSortElement
	{
public:
	TAgsInstanceSortElement(CalCommon::TCalSortAttribute aSortAttr, CalCommon::TCalSortDirection);
	virtual TInt Compare(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond) const;

protected:
	InstanceCompareFptr iFptr;
	TInt iSortDir;
	};

class TAgsInstanceSortType : public TAgsInstanceSortElement
	{
public:
	TAgsInstanceSortType(CalCommon::TCalSortAttribute aSortAttr, CalCommon::TCalSortDirection aSortDir, const RArray<CCalEntry::TType>& aEntryTypeOrder);
	virtual TInt Compare(const TAgnSortInstance& aFirst, const TAgnSortInstance& aSecond) const;

private:
	const RArray<CCalEntry::TType>& iEntryTypeOrder;
	};

class TAgnInstanceSortKey : public TKeyArrayFix
	{
public:
	TAgnInstanceSortKey(const RPointerArray<TAgsInstanceSortElement>& aSortElements);
	virtual TInt Compare(TInt aLeft,TInt aRight) const;
	
private:
	const RPointerArray<TAgsInstanceSortElement>& iSortElements;
	};


class CAgsInstanceSorter : public CBase
	{
public:
	static CAgsInstanceSorter* NewL(CAgnSortCriteria* aSortCriteria, const RArray<CCalEntry::TType>& aDefaultTypeOrder);
	~CAgsInstanceSorter();
	
	void SortL(CArrayFix<TAgnSortInstance>& aInstances);
	TInt FindL(CArrayFix<TAgnSortInstance>& aInstances, TAgnSortInstance& aInstance);
	
private:
	CAgsInstanceSorter();
	void ConstructL(const RArray<CCalEntry::TType>& aDefaultTypeOrder, CAgnSortCriteria* aSortCriteria);
	
private:	
	CAgnSortCriteria* iSortCriteria;
	RPointerArray<TAgsInstanceSortElement> iSortElements;
	};

#endif /*__AGSINSTANCESORTER_H_*/
