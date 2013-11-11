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

#include "agssortinstance.h"
#include "agmsimpleentry.h"
#include "agmdate.h"
#include "agmfilter.h"

// ----------------------------TAgnSortInstance---------------------------------
TAgnSortInstance::TAgnSortInstance(const CAgnSimpleEntry& aSimpleEntry)
	:iSimpleEntry(aSimpleEntry)
	{
	}

void TAgnSortInstance::SetL(const TTime& aInstanceTimeLocal, const TTime& aUndatedTodoTime)
	{
	// find the simple entry for this instance to create a sort instance to find this one
	if (iSimpleEntry.Type() == CCalEntry::ETodo)
		{
		if (iSimpleEntry.RptDef())
			{
			// it is a repeating todo
			iStartTimeLocal = iSimpleEntry.DurationMinusL(aInstanceTimeLocal);
			iEndTimeLocal = aInstanceTimeLocal;
			}
		else
			{
			// it is a non-repeating todo
			SetTodoTimesL(aUndatedTodoTime);
			}
		}
	else
		{
		iStartTimeLocal = aInstanceTimeLocal;
		iEndTimeLocal = iSimpleEntry.DurationPlusL(aInstanceTimeLocal);
		}
	}


void TAgnSortInstance::SetTodoTimesL(const TTime& aUndatedTodoTime)
	{
	const TTime KCompletedDateUtc = iSimpleEntry.CompletedDateUtc();

	if (KCompletedDateUtc != Time::NullTTime())
		{
		const TTime KCompletedDateLocal = AgnDateTime::ConvertToLocalTimeL(KCompletedDateUtc);
		// the todo is crossed out
		// so the instance time is the crossed out time
		iStartTimeLocal = KCompletedDateLocal;
		iEndTimeLocal = KCompletedDateLocal;
		}
	else
		{
		// The todo is incomplete
		if ( !iSimpleEntry.EndTime().IsSet() )
			{
			// This is an undated incomplete todo
			// so the instance start and end time is set to 'undated todo time'
			iStartTimeLocal = aUndatedTodoTime;
			iEndTimeLocal = aUndatedTodoTime;
			}
		else
			{
			// This is a dated incomplete todo
			// so the instance start and end is the same
			iEndTimeLocal = iSimpleEntry.EntryTime().LocalL();
			
			if ( !iSimpleEntry.StartTime().IsSet() )
				{
				iStartTimeLocal = iEndTimeLocal;
				}
			else
				{
				iStartTimeLocal = iSimpleEntry.StartTime().LocalL();
				}
			}
		}
	}

/** Checks if the start and end time overlaps match the filter settings 
*/
TBool TAgnSortInstance::CheckStartAndEndDateOverlap(const TFindInstanceParams& aSearchParams)
	{
	if ( iEndTimeLocal < aSearchParams.iRangeStart.LocalL() || iStartTimeLocal > aSearchParams.iRangeEnd.LocalL() )
		{
		return EFalse;
		}
	
	if ( aSearchParams.iFilter.StartTimeOverlapOnly() )
		{
		// start time flag set
		if ( iStartTimeLocal < aSearchParams.iRangeStart.LocalL() )
			{
			return EFalse;
			}
		}

	if ( aSearchParams.iFilter.EndTimeOverlapOnly() )
		{
		// end time flag set
		if ( iEndTimeLocal > aSearchParams.iRangeEnd.LocalL() )
			{
			return EFalse;
			}
		}
	
	if (aSearchParams.iFilter.ExcludeInstanceAdjoiningRange())
		{
		// Instances that adjoin the search start or end time are excluded
		// Instances that start and end on the search start time are included
		// Instances that start and end on the search end time are excluded
		
		if ((iEndTimeLocal == aSearchParams.iRangeStart.LocalL() && 
				iStartTimeLocal != aSearchParams.iRangeStart.LocalL()) ||
				iStartTimeLocal == aSearchParams.iRangeEnd.LocalL())
			{
			return EFalse;
			}
		
		}
	
	return ETrue;
	}

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS		   						 
TBool TAgnSortInstance::CheckAlarmTimeWithinL(const TFindInstanceParams& aSearchParams) const
	{
	return (iStartTimeLocal >= aSearchParams.iRangeStart.LocalL() && iStartTimeLocal <= aSearchParams.iRangeEnd.LocalL());
	}
#endif	


/** Returns the instance time
*/
TTime TAgnSortInstance::InstanceDate() const
	{	
	
	if (iSimpleEntry.Type() == CCalEntry::ETodo)
		{
		return iEndTimeLocal;
		}
	return iStartTimeLocal;
	}


/** Returns a reference to the simple entry
*/
const CAgnSimpleEntry& TAgnSortInstance::SimpleEntry() const
	{
	return iSimpleEntry;
	}

/** Creates and returns an instance id
*/
TAgnInstanceId TAgnSortInstance::InstanceIdL() const
	{
	TAgnInstanceId instId;
	instId.SetEntryId(iSimpleEntry.EntryId());
	TAgnCalendarTime instanceTime;
	if (iSimpleEntry.TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		instanceTime.SetFloatingL(InstanceDate());
		}
	else
		{
		instanceTime.SetLocalL(InstanceDate());
		}
	instId.SetDate(instanceTime);
	return instId;
	}

/** return instance alarm date time 
*/
TTime TAgnSortInstance::InstanceAlarmDateTime() const
	{
	if (!iSimpleEntry.HasAlarm())
		{
		return (Time::NullTTime());
		}
	
	return (InstanceDate() - iSimpleEntry.AlarmOffset());
	}

