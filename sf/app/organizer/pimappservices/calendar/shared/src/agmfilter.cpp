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

#include "agmfilter.h"

#include "agmsimpleentry.h"
#include <s32stor.h>

// --------------------------TAgnFilter-----------------------------------------
EXPORT_C TAgnFilter::TAgnFilter() :
	iFilter(CalCommon::EIncludeAll),
	iSearchBehaviour(CalCommon::EFoldedTextSearch)
	,iLowPriority(KMaxTUint), iHighPriority(0), 
	iUserIntFilterValue(0), iUserIntFilterMask(0)
	
	{
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
	iAlarmedInstanceSearch = EFalse;
	iFloatingInstanceOnly = EFalse;
#endif	
	}

EXPORT_C TAgnFilter::TAgnFilter(const CalCommon::TCalViewFilter& aFilter, 
								const CalCommon::TCalSearchBehaviour& aSearchBehaviour) :
	iFilter(aFilter),
	iSearchBehaviour(aSearchBehaviour)
	,iLowPriority(KMaxTUint), iHighPriority(0), 
	iUserIntFilterValue(0), iUserIntFilterMask(0)
	{
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
	iAlarmedInstanceSearch = EFalse;
	iFloatingInstanceOnly = EFalse;
#endif		
	}


EXPORT_C TBool TAgnFilter::IsValid(const CAgnSimpleEntry& aElement) const
/** Tests whether the specified entry is included in or excluded from the filter.

@internalAll
@param aElement The entry.
@return ETrue if the entry is valid with respect to the filter, EFalse if invalid. 
*/
	{
	if(aElement.Type() == CCalEntry::ETodo)
		{
		// this can't leave:
		if (aElement.CompletedDateUtc() != Time::NullTTime() && !AreCompletedTodosIncluded())
			return EFalse;
		if (aElement.CompletedDateUtc() == Time::NullTTime() && !AreIncompletedTodosIncluded())
			return EFalse;
		}
	if (!aElement.HasAlarm() && AreAlarmedOnlyIncluded())
		return EFalse;
	if (aElement.Type() == CCalEntry::EReminder && !AreRemindersIncluded())
		return EFalse;
	if (aElement.Type() == CCalEntry::EAppt && !AreTimedApptsIncluded())
		return EFalse;
	if (aElement.Type() == CCalEntry::EEvent && !AreEventsIncluded())
		return EFalse;
	if (aElement.Type() == CCalEntry::EAnniv && !AreAnnivsIncluded())
		return EFalse;
	if (aElement.Type() == CCalEntry::ENote && !AreNotesIncluded())
		return EFalse;
	if(aElement.Priority() > iLowPriority ||
			aElement.Priority() <  iHighPriority)
		{
		//the entry does not match the prority and UserInt filter
		return EFalse;
		}
	
	if(iUserIntFilterMask != 0)
		{
		if((aElement.UserInt() & iUserIntFilterMask) != (iUserIntFilterValue & iUserIntFilterMask))
			{
			return EFalse;
			}
		}	
	
	return (ETrue);
	}

EXPORT_C void TAgnFilter::InternalizeL(RReadStream& aStream)
/**
@internalAll
*/
	{
	iFilter = aStream.ReadUint32L();
	iSearchBehaviour = static_cast<CalCommon::TCalSearchBehaviour>(aStream.ReadUint32L());

	iLowPriority = aStream.ReadInt32L();
	iHighPriority = aStream.ReadInt32L();
	
	iUserIntFilterValue = aStream.ReadInt32L();
	iUserIntFilterMask = aStream.ReadInt32L();

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
	iAlarmedInstanceSearch = aStream.ReadInt8L();
	iFloatingInstanceOnly = aStream.ReadInt8L();
#endif
	}

EXPORT_C void TAgnFilter::ExternalizeL(RWriteStream& aStream) const
/**
@internalAll
*/
	{
	aStream.WriteUint32L(iFilter);
	aStream.WriteUint32L(iSearchBehaviour);
	
	aStream.WriteInt32L(iLowPriority);
	aStream.WriteInt32L(iHighPriority);
	
	aStream.WriteInt32L(iUserIntFilterValue);
	aStream.WriteInt32L(iUserIntFilterMask);

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS	
	aStream.WriteInt8L(iAlarmedInstanceSearch);
	aStream.WriteInt8L(iFloatingInstanceOnly);
#endif
	}

EXPORT_C void TFindInstanceParams::InternalizeL(RReadStream& aReadStream)
	{
	iFilter.InternalizeL(aReadStream);
	iRangeStart.InternalizeL(aReadStream);
	iRangeEnd.InternalizeL(aReadStream);

	TUint32 low = aReadStream.ReadUint32L();
	TUint32 high = aReadStream.ReadUint32L();
	iUndatedTodoTimeLocal = TTime(MAKE_TINT64(high, low));
	
	iInstance.InternalizeL(aReadStream);
	iNumInstances = aReadStream.ReadInt32L();
	
	aReadStream >> iSearchString;
	}

EXPORT_C void TFindInstanceParams::ExternalizeL(RWriteStream& aWriteStream) const
	{
	iFilter.ExternalizeL(aWriteStream);
	iRangeStart.ExternalizeL(aWriteStream);
	iRangeEnd.ExternalizeL(aWriteStream);
	
	aWriteStream << I64LOW(iUndatedTodoTimeLocal.Int64());
	aWriteStream << I64HIGH(iUndatedTodoTimeLocal.Int64());
	
	aWriteStream << iInstance;
	aWriteStream.WriteInt32L(iNumInstances);
	
	aWriteStream << iSearchString;
	}

