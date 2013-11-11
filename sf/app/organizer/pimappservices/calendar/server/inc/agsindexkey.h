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

#ifndef __AGSINDEXKEY_H__
#define __AGSINDEXKEY_H__

#include "agmids.h"

// -------------------------------- Referenced classes--------------------------

class CAgnSimpleEntry;

template <class T,class KeyValue>
class TAgnKeyBase : public TKeyArrayFix 		
	{
public:
	inline TAgnKeyBase() : TKeyArrayFix(0, ECmpNormal,sizeof(T*)), iUndatedTodoTime(Time::MaxTTime()) {}
	virtual KeyValue GetKeyValueL(const T* aEntry) const = 0;
	virtual TInt Compare(const T* aLeft, const T* aRight) const = 0;	
	virtual TInt Compare(const T* aEntry) const = 0;
	virtual void SetValueL(const KeyValue& aDate) = 0;
	inline void SetUndatedTodoTime(TTime& aUndatedTodoTime) {iUndatedTodoTime = aUndatedTodoTime;}
protected:
	TTime iUndatedTodoTime;
	};


// -----------------------------------------------------------------------------

class TAgnKeyDate : public TAgnKeyBase<CAgnSimpleEntry,TTime> 		
	{
public:
	TAgnKeyDate(const MAgnCalendarTimeMode::TTimeMode aTimeMode);
	void SetValueL(const TTime& aDate);
	virtual TInt Compare(TInt aLeft,TInt aRight) const;
	TInt Compare(const CAgnSimpleEntry* aLeft, const CAgnSimpleEntry* aRight) const;	
	TInt Compare(const CAgnSimpleEntry* aEntry) const;
	virtual TTime GetKeyValueL(const CAgnSimpleEntry* aEntry) const = 0;
	virtual TTime GetKeyValueUtcL(const CAgnSimpleEntry* aEntry) const = 0;
protected:
	MAgnCalendarTimeMode::TTimeMode iTimeMode;
private:
	TAgnCalendarTime iSearchKeyValue;
	};


// ----------------------------TAgnKeyStartDate---------------------------------

class TAgnKeyStartDate : public TAgnKeyDate
	{
public:
	TAgnKeyStartDate(const MAgnCalendarTimeMode::TTimeMode aTimeMode = MAgnCalendarTimeMode::EFixedUtc);
	virtual TTime GetKeyValueL(const CAgnSimpleEntry* aEntry) const;
	virtual TTime GetKeyValueUtcL(const CAgnSimpleEntry* aEntry) const;
	};


// ----------------------------TAgnKeyRptDate-----------------------------------

class TAgnKeyRptDate : public TAgnKeyDate
	{
public:
	TAgnKeyRptDate(const MAgnCalendarTimeMode::TTimeMode aTimeMode = MAgnCalendarTimeMode::EFixedUtc);
	virtual TTime GetKeyValueL(const CAgnSimpleEntry* aEntry) const;
	virtual TTime GetKeyValueUtcL(const CAgnSimpleEntry* aEntry) const;
	};


// ----------------------------TAgnKeyTodoNonRptDate----------------------------

class TAgnKeyTodoNonRptDate : public TAgnKeyDate
	{
public:
	TAgnKeyTodoNonRptDate(const MAgnCalendarTimeMode::TTimeMode aTimeMode = MAgnCalendarTimeMode::EFixedUtc);
	virtual TTime GetKeyValueL(const CAgnSimpleEntry* aEntry) const;
	virtual TTime GetKeyValueUtcL(const CAgnSimpleEntry* aEntry) const;
	};

#endif

