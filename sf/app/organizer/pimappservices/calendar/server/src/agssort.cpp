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

#include "agssort.h"
#include "agssortinstance.h"
#include "agmsimpleentry.h"

// -------------------------- TAgnDaySortKey --------------------------------------
TAgnDaySortKey::TAgnDaySortKey(const TTime& aTargetDate, const TTime& aUndatedTodoTime) 
	: TKeyArrayFix(0,ECmpNormal,0), iUndatedTodoTime(aUndatedTodoTime), iTargetDate(aTargetDate)
	{
	} 

TInt TAgnDaySortKey::OrderByType(TUint aLeft,TUint aRight) const
//
// Entries with equal times are ordered according by: todo, anniv, notes, appts
//
	{

	TInt r=0;
	switch(static_cast<TAgnSortInstance*>(At(aRight))->SimpleEntry().Type())
		{
		case CCalEntry::EAppt: 
		case CCalEntry::EReminder:++r;
		case CCalEntry::ETodo: ++r;
		case CCalEntry::EAnniv:	
		case CCalEntry::EEvent:	break;				
			
		}

	TInt l=0;
	switch(static_cast<TAgnSortInstance*>(At(aLeft))->SimpleEntry().Type())
		{
		case CCalEntry::EAppt:
		case CCalEntry::EReminder: ++l;
		case CCalEntry::ETodo: ++l;
		case CCalEntry::EAnniv:	
		case CCalEntry::EEvent:	 break;
		}
	return(l-r);
	}


TInt TAgnDaySortKey::OrderByAppt(TUint aLeft,TUint aRight) const
//
// Entries which are both appointments are ordered by start time then duration time
//
	{
	const CAgnSimpleEntry& leftAppt = static_cast<TAgnSortInstance*>(At(aLeft))->SimpleEntry();
	const CAgnSimpleEntry& rightAppt = static_cast<TAgnSortInstance*>(At(aRight))->SimpleEntry();

	const TAgnCalendarTime& KLeftDur = leftAppt.EndTime(); 
	const TAgnCalendarTime& KRightDur = rightAppt.EndTime(); 
	if (KLeftDur == KRightDur)
		{
		return(0);
		}
	return (KLeftDur < KRightDur ? -1 : 1);   
	}


TInt TAgnDaySortKey::OrderByToDo(TUint aLeft,TUint aRight) const
//
// Ordering the same as order by date then priority
//
	{
	return OrderByDatePriority(aLeft,aRight);
	}


TInt TAgnDaySortKey::OrderByCompletedDate(TUint aLeft,TUint aRight) const
//
//	Crossed out entries appear after un-crossed out ones (all else being equal)
//
	{

	TBool leftX=(static_cast<TAgnSortInstance*>(At(aLeft))->SimpleEntry().CompletedDateUtc() != Time::NullTTime());
	TBool rightX=(static_cast<TAgnSortInstance*>(At(aRight))->SimpleEntry().CompletedDateUtc() != Time::NullTTime());
	if (leftX==rightX)
		{
		return 0;
		}
	return(leftX<rightX ? -1 : 1);  
	}



TInt TAgnDaySortKey::OrderByRepeating(TUint aLeft,TUint aRight) const
//
// Order so that non-repeating entries have priority over repeating ones
//
	{

	const CAgnRptDef* leftRpt=static_cast<TAgnSortInstance*>(At(aLeft))->SimpleEntry().RptDef(); 
	const CAgnRptDef* rightRpt=static_cast<TAgnSortInstance*>(At(aRight))->SimpleEntry().RptDef(); 
	if (TBool(leftRpt)==TBool(rightRpt))
		return(0);
	return(leftRpt ? -1 : 1);  
	}


TInt TAgnDaySortKey::OrderByDate(TUint aLeft,TUint aRight) const
//	
//	First come the entries which are live and about to expire soon. These have 
//  a start date before or equal to today. These are sorted by their due date (end date)
//
//  Next come undated todos
//
//  Last come those to start in the future, these are sorted by start date and then
//  due date
//
//  Crossed out entries come at the end
	{

	TAgnSortInstance* instLeft = static_cast<TAgnSortInstance*>(At(aLeft));
	TAgnSortInstance* instRight = static_cast<TAgnSortInstance*>(At(aRight));

	TBool completedLeft(instLeft->SimpleEntry().CompletedDateUtc() != Time::NullTTime());
	TBool completedRight(instRight->SimpleEntry().CompletedDateUtc() != Time::NullTTime());

	if (completedLeft && completedRight)
		{
		TTime completedDateLeft = instLeft->SimpleEntry().CompletedDateUtc();
		TTime completedDateRight = instRight->SimpleEntry().CompletedDateUtc();
		if (completedDateLeft == completedDateRight)
			{
			return 0;
			}
		return completedDateLeft < completedDateRight? -1 : 0;
		}
	if (completedLeft)
		{
		return 1;
		}
	if (completedRight)
		{
		return -1;
		}

	TBool undatedLeft = ! instLeft->SimpleEntry().EndTime().IsSet();
	TBool undatedRight = ! instRight->SimpleEntry().EndTime().IsSet();

	if (undatedLeft && undatedRight)
		{
		return 0;						// both undated
		}
	if (undatedLeft)
		{
		return instRight->iStartTimeLocal <= UndatedTodoTime()? 1 : -1; 
		}
	if (undatedRight)
		{
		return instLeft->iStartTimeLocal <= UndatedTodoTime()? -1 : 1; 
		}

	if (instLeft->iStartTimeLocal <= UndatedTodoTime() && instRight->iStartTimeLocal <= UndatedTodoTime()) // both live
		{
		if (instLeft->iEndTimeLocal == instRight->iEndTimeLocal)
			{
			return 0;
			}
		return instLeft->iEndTimeLocal < instRight->iEndTimeLocal? -1 : 1;
		}
	if (instLeft->iStartTimeLocal <= UndatedTodoTime())
		{
		return -1;
		}
	if (instRight->iStartTimeLocal <= UndatedTodoTime())
		{
		return 1;
		}
	if (instLeft->iStartTimeLocal == instRight->iStartTimeLocal)
		{
		if (instLeft->iEndTimeLocal == instRight->iEndTimeLocal)
			{
			return 0;
			}
		return instLeft->iEndTimeLocal < instRight->iEndTimeLocal ? -1 : 1;
		}
	return instLeft->iStartTimeLocal < instRight->iStartTimeLocal ? -1 : 1;
	}


TInt TAgnDaySortKey::OrderByPriority(TUint aLeft,TUint aRight) const
//
//	Order todo entries by priority
//
	{
	const CAgnSimpleEntry& leftTodo = static_cast<TAgnSortInstance*>(At(aLeft))->SimpleEntry();
	const CAgnSimpleEntry& rightTodo = static_cast<TAgnSortInstance*>(At(aRight))->SimpleEntry();

	TUint leftPri = leftTodo.Priority(); 
	TUint rightPri = rightTodo.Priority(); 
	if (leftPri == rightPri)
		{
		return(0);
		}
	return(leftPri<rightPri ? -1 : 1);  
	}



TInt TAgnDaySortKey::OrderByDatePriority(TUint aLeft,TUint aRight) const
//
//	Order todo entries by date then priority  (which date?)
//
	{	
	
	TInt ret=OrderByDate(aLeft,aRight);
	if (ret==0)
		if ((ret=OrderByPriority(aLeft,aRight)) == 0)
			ret = OrderById(aLeft,aRight);
	return(ret);
	}

TInt TAgnDaySortKey::OrderById(TUint aLeft,TUint aRight) const
//
//	Order todo entries according to their id
//
	{
	TAgnEntryId leftId = static_cast<TAgnSortInstance*>(At(aLeft))->SimpleEntry().EntryId();
	TAgnEntryId rightId = static_cast<TAgnSortInstance*>(At(aRight))->SimpleEntry().EntryId();
	if (leftId == rightId)
		return 0;
	return  (leftId.StreamId().Value() << 4) + leftId.PartialId() < (rightId.StreamId().Value() << 4) + rightId.PartialId()? -1 : 1;
	}

TInt TAgnDaySortKey::OrderByFileId(TUint aLeft,TUint aRight) const
//
//	Order todo entries according to their id
//
	{
	TUint8 leftfileId  = static_cast<TAgnSortInstance*>(At(aLeft))->SimpleEntry().CollectionId();
	TUint8 rightfileId = static_cast<TAgnSortInstance*>(At(aRight))->SimpleEntry().CollectionId();
	return 	leftfileId - rightfileId;
	}

void TAgnDaySortKey::SetTargetDate(const TTime& aTargetDate)
//
// 
//
	{
	iTargetDate = aTargetDate;
	}

TInt TAgnDaySortKey::OrderByTime(TUint aLeft,TUint aRight) const
//
// Order by time of entry (may be default time)
//
	{
	TAgnSortInstance* leftInst = static_cast<TAgnSortInstance*>(At(aLeft));
	TAgnSortInstance* rightInst = static_cast<TAgnSortInstance*>(At(aRight));

	TTime leftDate = leftInst->InstanceDate(); 
	TTime rightDate = rightInst->InstanceDate(); 

	if (leftInst->SimpleEntry().Type() == CCalEntry::ETodo && leftDate == Time::NullTTime() && leftInst->SimpleEntry().CompletedDateUtc() == Time::NullTTime())
		{
		leftDate = UndatedTodoTime();
		}
	if (rightInst->SimpleEntry().Type() == CCalEntry::ETodo && rightDate == Time::NullTTime() && rightInst->SimpleEntry().CompletedDateUtc() == Time::NullTTime())
		{
		rightDate = UndatedTodoTime();
		}

	if (leftDate != rightDate)
		{
		return(leftDate < rightDate ? -1 : 1);  
		}

	return 0;  
	}


TInt TAgnDaySortKey::Compare(TInt aLeft,TInt aRight) const
//
// Entries within a day are first ordered by time, then for entries with equal times:
// 1) Todos. Two todo entries are ordered by due date, priority then start date
// 2) Anniversaries
// 3) Day notes
// 4) Appointments. Two appointments are ordered by start time then end time
// Finally if two entries are still the same then crossed out ones appear after un-crossed out ones
//
	{
	TInt ret=OrderByTime(aLeft,aRight);
	if (ret==0)
	// entries have identical times so sort them further:
		{
		// Starting with the types: todo, anniv, note, appt
		ret=OrderByType(aLeft,aRight);
		if (ret==0)
		// types are identical
			{
			switch (static_cast<TAgnSortInstance*>(At(aLeft))->SimpleEntry().Type())
				{
				case CCalEntry::EAppt:
				case CCalEntry::EReminder: 	ret=OrderByAppt(aLeft,aRight); break;
				case CCalEntry::ETodo: ret=OrderByToDo(aLeft,aRight); break;								
				case CCalEntry::EAnniv:	
				case CCalEntry::EEvent:	break;
				}
			if (ret==0)
				{
				// order by un-crossed out, crossed out
				if ((ret=OrderByCompletedDate(aLeft,aRight)) == 0)
					// finally order by Id
					{
					ret = OrderById(aLeft,aRight);
					if (ret==0)
						{
						ret = OrderByFileId(aLeft,aRight);
						}
					}
				}									
			}		
		}
	return(ret);
	};


// ---------------------------TAgnAlarmSortKey -----------------------------------
TAgnAlarmSortKey::TAgnAlarmSortKey()
: TKeyArrayFix(0,ECmpNormal,0)
	{
	
	}
	
TInt TAgnAlarmSortKey::Compare(TInt aLeft, TInt aRight) const
	{
	TAgnSortInstance* leftInstance =  static_cast<TAgnSortInstance*>(At(aLeft));
	TAgnSortInstance* rightinstance = static_cast<TAgnSortInstance*>(At(aRight));
	TTime leftInstanceAlarmTime = leftInstance->InstanceAlarmDateTime();
	TTime rightInstanceAlarmTime = rightinstance->InstanceAlarmDateTime();
	
	if (leftInstanceAlarmTime != rightInstanceAlarmTime)
		return(leftInstanceAlarmTime < rightInstanceAlarmTime ? -1 : 1);  

	return(0);
	}
