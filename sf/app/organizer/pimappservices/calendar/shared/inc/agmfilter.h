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

#ifndef __AGMFILTS_H__
#define __AGMFILTS_H__

#include "calcommon.h"
#include "agmcalendartime.h"
#include "agmids.h"

class RReadStream;
class RWriteStream;

class CAgnSimpleEntry;

const TUint KAgnMaxSearchStringLength = 32;

NONSHARABLE_CLASS(TAgnFilter)
/** Identifies which model entry types should be involved when tidying, searching 
or populating an instance or symbol list.

It is usually used in combination with a date or date range. 
@internalAll
@released
*/
	{
public:
	IMPORT_C TAgnFilter();
	IMPORT_C TAgnFilter(const CalCommon::TCalViewFilter& aFilter, const CalCommon::TCalSearchBehaviour& aBehaviour);
	IMPORT_C TBool IsValid(const CAgnSimpleEntry& aElement) const;
	inline TBool AreTimedApptsIncluded() const;
	inline TBool AreRemindersIncluded() const;
	inline TBool AreEventsIncluded() const;
	inline TBool AreAnnivsIncluded() const;
	inline TBool AreNonTodosIncluded() const;
	inline TBool RptNextInstanceOnly() const;
	inline TBool AreCompletedTodosIncluded() const;
	inline TBool AreIncompletedTodosIncluded() const;
	inline TBool AreAlarmedOnlyIncluded() const;
	inline TBool IsExactTextOnlySearch() const;
	inline TBool IsFoldedOnlySearch() const;
	inline TBool IsEntryLocationSearched() const;
	inline TBool IsEntryDescriptionSearched() const;
	inline TBool IsAttendeeAddressSearched() const;
	inline TBool IsAttendeeSentByAddressSearched() const;
	inline TBool IsAttendeeCommonNameSearched() const;
	inline TBool IsOrganizerAddressSearched() const;
	inline TBool IsOrganizerSentByAddressSearched() const;
	inline TBool IsOrganizerCommonNameSearched() const;
	inline TBool StartTimeOverlapOnly() const;
	inline TBool EndTimeOverlapOnly() const;
	inline TBool ExcludeInstanceAdjoiningRange() const;

	
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS	
	inline TBool IsAlarmedInstanceSearch() const;
	inline void SetAlarmedInstanceSearch(const TBool aAlarmedInstanceSearch);
	inline TBool FloatingInstanceOnly() const;
	inline void SetFloatingInstanceOnly(const TBool aFloatingInstanceOnly);
#endif	
	
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	
	/**
	 * @brief check for Notes included in the filter
	 * 
	 * @return ETrue if notes are included otherwise return EFalse 
	 */
	inline TBool AreNotesIncluded() const;
	
private:
	CalCommon::TCalViewFilter iFilter;
	CalCommon::TCalSearchBehaviour iSearchBehaviour;
	
public:
	/** The search range of priority. */
	TUint iLowPriority;
	TUint iHighPriority;
	
	/** The search criteria for UserInt(32 bits integer). */
	TUint32	iUserIntFilterValue;
	TUint32	iUserIntFilterMask;

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS	
	TBool iAlarmedInstanceSearch;
	TBool iFloatingInstanceOnly;
#endif	
	};

#include "agmfilter.inl"

/** The find instance settings that are sent to the server 
*/
class TFindInstanceParams
	{
public:
	IMPORT_C void InternalizeL(RReadStream& aReadStream);
	IMPORT_C void ExternalizeL(RWriteStream& aWriteStream) const;
	
public:
	/** The entry filter to use when filtering entries. */
	TAgnFilter iFilter;
	/** The start of the find instance search range. */
	TAgnCalendarTime iRangeStart;
	/** The end of the find instance search range. */
	TAgnCalendarTime iRangeEnd;
	/** The time that undated todos should appear when  */
	TTime iUndatedTodoTimeLocal;
	/** The instance to search from when doing a next or previous instance call */
	mutable TAgnInstance iInstance;
	/** The number of instances a next or previous call should return */
	mutable TInt iNumInstances;
	/** The search string to use when filtering entries by text fields */
	TBuf< KAgnMaxSearchStringLength > iSearchString;
	};
	
#endif
