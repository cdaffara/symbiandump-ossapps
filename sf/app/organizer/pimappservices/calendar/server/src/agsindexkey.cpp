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

#include "agsindexkey.h"
#include "agmsimpleentry.h"
#include "agmdate.h"

// ---------------------------------TAgnKeyDate------------------------------

TAgnKeyDate::TAgnKeyDate(const MAgnCalendarTimeMode::TTimeMode aTimeMode)
		:	TAgnKeyBase<CAgnSimpleEntry,TTime>(),
			iTimeMode(aTimeMode)
	{
	__ASSERT_DEBUG(iTimeMode == MAgnCalendarTimeMode::EFixedUtc || iTimeMode == MAgnCalendarTimeMode::EFloating, User::Invariant());
	}

void TAgnKeyDate::SetValueL(const TTime& aDate)
//
// set the key value that will be used by search
//
	{ 
	if(iTimeMode == MAgnCalendarTimeMode::EFixedUtc)
		{
		iSearchKeyValue.SetLocalL(aDate); 
		}
	else // we can assume iTimeMode == MAgnCalendarTimeMode::EFloating because of the
		 //	assert in the TAgnKeyDate constructor.
		{
		iSearchKeyValue.SetFloatingL(aDate); 
		}
	}

TInt TAgnKeyDate::Compare(const CAgnSimpleEntry* aEntry) const
//
// compare entry with search value
//
	{
	// Note that undated entries will return a key value of
	// AgnDateTime::MaxDate() since we want them to have
	// a key value larger than all dated entries.
	//
	TTime entryTime;
	TTime searchKeyTime;
	if (iTimeMode == MAgnCalendarTimeMode::EFixedUtc)
		{
		entryTime =  GetKeyValueUtcL(aEntry);
		searchKeyTime = iSearchKeyValue.UtcL(); 
		}
	else // we can assume iTimeMode == MAgnCalendarTimeMode::EFloating because of the
		 //	assert in the TAgnKeyDate constructor.
		{
		entryTime =  GetKeyValueL(aEntry);
		searchKeyTime = iSearchKeyValue.LocalL(); 
		}

	if (searchKeyTime == Time::NullTTime())
		{
		searchKeyTime = Time::MaxTTime();
		}
 
	if (entryTime == searchKeyTime )
		{
		return (0);
		}

	return entryTime < searchKeyTime ? -1 : 1;
	}

TInt TAgnKeyDate::Compare(const CAgnSimpleEntry* aLeft, const CAgnSimpleEntry* aRight) const
//
// Compare two entries
//
	{
	// Note that undated entries will return a key value of
	// AgnDateTime::MaxDate() since we want them to have
	// a key value larger than all dated entries.
	//
   	TTime leftStart;
   	TTime rightStart;

	if (iTimeMode == MAgnCalendarTimeMode::EFixedUtc)
		{
		leftStart = GetKeyValueUtcL(aLeft);
   		rightStart = GetKeyValueUtcL(aRight);
		}
	else // we can assume iTimeMode == MAgnCalendarTimeMode::EFloating because of the
		 //	assert in the TAgnKeyDate constructor.
		{
		leftStart = GetKeyValueL(aLeft);
   		rightStart = GetKeyValueL(aRight);
		}

	if (leftStart == rightStart)
		{
		return (0);
		}
		
	return (leftStart < rightStart? -1 : 1);
	}


TInt TAgnKeyDate::Compare(TInt aLeft,TInt aRight) const
//
// compare two entries that are at position aLeft and aRight in array
//
	{

	CAgnSimpleEntry* LeftEntry = *(CAgnSimpleEntry**)At(aLeft);
	CAgnSimpleEntry* RightEntry = *(CAgnSimpleEntry**)At(aRight);

	if (aRight == -1 && RightEntry == NULL) 
		{
		return Compare(LeftEntry);
		}

	return Compare(LeftEntry,RightEntry);
	}

// ----------------------------TAgnKeyStartDate---------------------------------

TAgnKeyStartDate::TAgnKeyStartDate(const MAgnCalendarTimeMode::TTimeMode aTimeMode) 
	: TAgnKeyDate(aTimeMode) 
	{
	}

TTime TAgnKeyStartDate::GetKeyValueL(const CAgnSimpleEntry* aEntry) const
//
// Return value of key of entry
//
	{
	return aEntry->ValidFromTimeLocalL(); 
	}

TTime TAgnKeyStartDate::GetKeyValueUtcL(const CAgnSimpleEntry* aEntry) const
//
// Return value of key of entry (utc version of this function)
//
	{
	return AgnDateTime::ConvertToUtcTimeL(GetKeyValueL(aEntry));
	}


// ----------------------------TAgnKeyRptDate-----------------------------------

TAgnKeyRptDate::TAgnKeyRptDate(const MAgnCalendarTimeMode::TTimeMode aTimeMode)
	 : TAgnKeyDate(aTimeMode)
	 {
	 }

TTime TAgnKeyRptDate::GetKeyValueL(const CAgnSimpleEntry* aEntry) const
//
// Return value of key of entry
//
	{
	return aEntry->ValidFromTimeLocalL();  
	}
	
TTime TAgnKeyRptDate::GetKeyValueUtcL(const CAgnSimpleEntry* aEntry) const
//
// Return value of key of entry (utc version of this function)
//
	{
	return AgnDateTime::ConvertToUtcTimeL(GetKeyValueL(aEntry));
	}


// ----------------------------TAgnKeyTodoNonRptDate----------------------------

TAgnKeyTodoNonRptDate::TAgnKeyTodoNonRptDate(const MAgnCalendarTimeMode::TTimeMode aTimeMode)
	 : TAgnKeyDate(aTimeMode)
	 {
	 }

/** Return value of key of entry
*/
TTime TAgnKeyTodoNonRptDate::GetKeyValueL(const CAgnSimpleEntry* aEntry) const
	{
	const TTime KCompletedDateUtc = aEntry->CompletedDateUtc();
	if (KCompletedDateUtc != Time::NullTTime())
		{
		return AgnDateTime::ConvertToLocalTimeL(KCompletedDateUtc);
		}	
	
	if ( ! aEntry->EndTime().IsSet()) // if its undated
		{
		return iUndatedTodoTime;
		}
		
	return aEntry->EndTime().LocalL();
	}

/** Return value of key of entry (utc version of this function)
*/
TTime TAgnKeyTodoNonRptDate::GetKeyValueUtcL(const CAgnSimpleEntry* aEntry) const
	{

	TTime completedDate = aEntry->CompletedDateUtc();
	if (completedDate != Time::NullTTime())
		{
		return completedDate;
		}
	
	if ( ! aEntry->EndTime().IsSet()) // if its undated
		{
		return AgnDateTime::ConvertToUtcTimeL(iUndatedTodoTime);
		}
	
	return aEntry->EndTime().UtcL();
	}
