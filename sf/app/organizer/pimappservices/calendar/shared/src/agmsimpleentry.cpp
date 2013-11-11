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

#include "agmsimpleentry.h"
#include "agmallocator.h"
#include "agmrptdef.h"
#include "agmdate.h"
#include "agmpanic.h"

// ------------------------------------ CAgnSimpleEntry ----------------------------------
EXPORT_C void CAgnSimpleEntry::SetCollectionId(TCalCollectionId aCallectionId)
	{
	iCollectionId = aCallectionId;
	}

EXPORT_C TCalCollectionId CAgnSimpleEntry::CollectionId() const
	{
	return iCollectionId;
	}

TAny* CAgnSimpleEntry::operator new(TUint aSize, TLeave aType, CAgnSimpleEntryAllocator* aAllocator)
/** Overloaded new operator, to allocate CAgnSimpleEntry objects using the CAgnSimpleEntryAllocator.
@internalComponent
*/
	{
	// check if the allocator is passed...
	
	if ( aAllocator )
		{
		return ( aAllocator->NewSimpleEntryL(aSize) );
		}

	// ...if not allocate the object with the base class new operator
		
	return ( CBase::operator new(aSize, aType) );
	}
	
CAgnSimpleEntry::CAgnSimpleEntry(CCalEntry::TType aType)
	: iCompletedDateUtc(Time::NullTTime()), 
	  iAlarmOffset(KAlarmNotSet), 
	  iType(aType), 
	  iStatus(CCalEntry::ENullStatus)
/**
@internalComponent
*/	
	{
	iLastModifiedDateUtc.UniversalTime();
	iStartTime.SetNull();
	iEndTime.SetNull();
	}

CAgnSimpleEntry::~CAgnSimpleEntry()
/** The destructor frees all resources owned by the object, prior to its destruction.

@internalComponent
*/
	{
    if (iRptDef)
        {
        delete iRptDef;
        iRptDef = NULL;
        }
	}


EXPORT_C TBool CAgnSimpleEntry::HasAlarm() const
/** Tests whether the entry is alarmed.

@internalComponent
@return ETrue if the entry has an alarm, EFalse if it does not have an alarm 
or if the entry is crossed out. 
*/
	{ 
	if (iCompletedDateUtc != Time::NullTTime())
		{
		return (EFalse);
		}
		
	return (iAlarmOffset != KAlarmNotSet); 
	}

EXPORT_C void CAgnSimpleEntry::SetAlarmOffset(const TTimeIntervalMinutes& aStartTimeOffset)
/**
@internalComponent
*/
	{
	iAlarmOffset = aStartTimeOffset.Int();
	}

EXPORT_C TTimeIntervalMinutes CAgnSimpleEntry::AlarmOffset() const
/** Gets the time of the alarm as a time interval before the alarm origin, i.e. the instance time.

@internalComponent
@return Time interval in minutes. 
*/
	{
	return TTimeIntervalMinutes(iAlarmOffset);
	}

EXPORT_C CCalEntry::TType CAgnSimpleEntry::Type() const
/**
@internalComponent
*/
	{
	return static_cast<CCalEntry::TType>(iType);
	}

EXPORT_C void CAgnSimpleEntry::SetRptDefL(const CAgnRptDef& aRptDef)
/** Sets the entry's repeat definition. 

The repeat definition includes daily, weekly, monthly and yearly repeat 
details, until time, sporadic dates and exceptions. 

@internalAll
@param aRptDef Reference to the new repeat definition. 
*/
	{
	__ASSERT_DEBUG(aRptDef.InvariantL()==KErrNone, User::Leave(KErrArgument));
	
	delete iRptDef;
	iRptDef = NULL;

	iRptDef = CAgnRptDef::NewL(*this);
	iRptDef->CopyL(aRptDef);
	}

EXPORT_C CAgnRptDef* CAgnSimpleEntry::RptDef()
	{
	return iRptDef;
	}

EXPORT_C const CAgnRptDef* CAgnSimpleEntry::RptDef() const
	{
	return iRptDef;
	}

TBool CAgnSimpleEntry::operator==(const CAgnSimpleEntry& aSimpleEntry) const
//
// Equality operator
//
	{
	if (iType != aSimpleEntry.iType)
		{
		return EFalse;
		}
	
	if ( TimeMode() == MAgnCalendarTimeMode::EFloating )
		{
		if ( StartTime() != aSimpleEntry.StartTime() )
			{
			return EFalse;
			}

		if ( EndTime() != aSimpleEntry.EndTime() )
			{
			return EFalse;
			}
		}
	else
		{
		if ( StartTime() != aSimpleEntry.StartTime() )
			{
			return EFalse;
			}

		if ( EndTime() != aSimpleEntry.EndTime() )
			{
			return EFalse;
			}
		}
	
	if ( CompletedDateUtc() != aSimpleEntry.CompletedDateUtc() )
		{
		return EFalse;
		}

	if ( TimeMode() != aSimpleEntry.TimeMode() )
		{
		return EFalse;
		}

	if (iCompletedDateUtc != aSimpleEntry.iCompletedDateUtc ||
		AlarmOffset() != aSimpleEntry.AlarmOffset() ||
		HasAlarm() != aSimpleEntry.HasAlarm() ||
		Status() != aSimpleEntry.Status())
		{
		return EFalse;
		}
	
	if (iRptDef)
		{
		if (aSimpleEntry.iRptDef)
			{
			return (*iRptDef == *aSimpleEntry.iRptDef);
			}
		else
			{
			return EFalse; // this has a rpt def but aSimpleEntry doesn't
			}
		}
	else if (aSimpleEntry.iRptDef)
		{
		return EFalse; // this doesn't have a rpt def but aSimpleEntry does
		}
	
	return ETrue;
	}

EXPORT_C void CAgnSimpleEntry::ClearRepeat()
/** Clears the repeat details including exceptions and unsets the entry's 'is repeating' 
property.
 
@internalComponent
*/
	{
	delete iRptDef;
	iRptDef=NULL;
	}

void CAgnSimpleEntry::CopySimpleEntryL(const CAgnSimpleEntry& aSimpleEntry, CCalEntry::TCopyType aCopyType)
//
// Copy the details from aSimpleEntry.
// If aCopyRptDetails is false then the repeat details including exceptions is not copied
// and the entry therefore becomes a non-repeating entry.
//
	{
	iStatus = aSimpleEntry.iStatus;
	iAlarmOffset = aSimpleEntry.iAlarmOffset;

	delete iRptDef;
	iRptDef = NULL;

	if ( aSimpleEntry.iRptDef )
		{
		iRptDef = CAgnRptDef::NewL(*this);
		iRptDef->CopyL(*aSimpleEntry.iRptDef);
		}
	
	if (aCopyType == CCalEntry::ECopyAll)
		{
		iEntryId = aSimpleEntry.iEntryId;
		iLocalUid = aSimpleEntry.iLocalUid;
		iGuidHash = aSimpleEntry.iGuidHash;
		}
	
	iPriority = aSimpleEntry.iPriority;	
	
	iStartTime = aSimpleEntry.StartTime();
	iEndTime = aSimpleEntry.EndTime();
		
	StartTimeChanged();
		
	iCompletedDateUtc = aSimpleEntry.iCompletedDateUtc;
	iLastModifiedDateUtc = aSimpleEntry.iLastModifiedDateUtc;

	iUserInt = aSimpleEntry.UserInt();
	iCollectionId = aSimpleEntry.CollectionId();
	}

EXPORT_C void CAgnSimpleEntry::SetStatus(CCalEntry::TStatus aStatus)
/** Sets the status of the entry to one of the values defined in the TAgnStatus enumeration.

@internalComponent
@param aStatus The status of the entry. 
*/
	{
	__ASSERT_DEBUG( (aStatus == CCalEntry::ETentative)
					|(aStatus == CCalEntry::EConfirmed)
					|(aStatus == CCalEntry::ECancelled)
					|(aStatus == CCalEntry::ETodoNeedsAction)
					|(aStatus == CCalEntry::ETodoCompleted)
					|(aStatus == CCalEntry::ETodoInProcess)
					|(aStatus == CCalEntry::ENullStatus)
					|(aStatus == CCalEntry::EVCalAccepted)
					|(aStatus == CCalEntry::EVCalNeedsAction)
					|(aStatus == CCalEntry::EVCalSent)
					|(aStatus == CCalEntry::EVCalDeclined)
					|(aStatus == CCalEntry::EVCalDelegated),
					Panic(EAgmErrInvalidStatus));
	
	iStatus = aStatus;
	}


EXPORT_C CCalEntry::TStatus CAgnSimpleEntry::Status() const
/** Returns the status of the entry.

@internalComponent
@return The status of the entry, as defined in the TAgnStatus enumeration.
*/
	{
	return static_cast<CCalEntry::TStatus>(iStatus);  
	}

EXPORT_C void CAgnSimpleEntry::SetLastModifiedDate()
/**
@internalComponent
*/
	{
	iLastModifiedDateUtc.UniversalTime();
	}

EXPORT_C void CAgnSimpleEntry::SetLastModifiedDateUtc(const TTime& aLastModifiedDateUtc)
/**
@internalComponent
*/	
	{
	iLastModifiedDateUtc = aLastModifiedDateUtc;
	}

EXPORT_C const TTime& CAgnSimpleEntry::LastModifiedDateUtc() const
/**
@internalComponent
*/
	{
	return iLastModifiedDateUtc;
	}

EXPORT_C void CAgnSimpleEntry::SetPriority(TUint8 aPriority)
/**
@internalComponent
*/
	{
	iPriority = aPriority;
	}

EXPORT_C TUint8 CAgnSimpleEntry::Priority() const
/**
@internalComponent
*/
	{
	return iPriority;
	}

EXPORT_C const TTime& CAgnSimpleEntry::CompletedDateUtc() const
/**
@internalComponent
*/
	{
	return iCompletedDateUtc;
	}

EXPORT_C void CAgnSimpleEntry::SetCompletedDateUtcL(const TTime& aDateUtc)
/**
@internalComponent
*/
	{
	__ASSERT_ALWAYS(iType == CCalEntry::ETodo, User::Leave(KErrNotSupported));
	iCompletedDateUtc = aDateUtc;
	}

EXPORT_C void CAgnSimpleEntry::SetIncomplete()
/**
@internalComponent
*/
	{
	iCompletedDateUtc = Time::NullTTime();
	}

EXPORT_C void CAgnSimpleEntry::SetStartAndEndTimeL(const TAgnCalendarTime& aStartTime, const TAgnCalendarTime& aEndTime)
/**
@internalComponent
*/
	{
	__ASSERT_ALWAYS(aStartTime.TimeMode() == aEndTime.TimeMode() || ! aStartTime.IsSet() || ! aEndTime.IsSet(), User::Leave(KErrNotSupported));
	if (iType != CCalEntry::ETodo)
		{
		__ASSERT_ALWAYS(aStartTime.IsSet(), User::Leave(KErrArgument));
		}

	if (iStartTime != aStartTime)
		{
		StartTimeChanged();
		}
	iStartTime = aStartTime;
	iEndTime = aEndTime;
	}

EXPORT_C void CAgnSimpleEntry::AdjustStartUntilTimeForRepEntryL()
	{
	if (iRptDef)
		{
		TAgnRpt* repeat = const_cast<TAgnRpt*> (iRptDef->RRule());
		if (repeat)
			{
			// Ensure that the start date actually falls on a repeating instance. For example,
			// if the repeat rule is every Wednesday, and the DtStart is a Monday, it should be 
			// nudged forwards to the Wednesday of that week.
			TTime startTimeUtc;
			if (Type() == CCalEntry::ETodo)
				{
				startTimeUtc = StartTime().UtcL();
				}
			else
				{
			    startTimeUtc = EntryTime().UtcL();
				}

			startTimeUtc -= TTimeIntervalMicroSeconds(1);
			TTime firstInstanceUtc;
			
			TBool isStillRpt = ETrue;
		  	if (iRptDef->NudgeNextInstanceUtcL(startTimeUtc, firstInstanceUtc))
		  		{
			  	isStillRpt = MoveStartTimeLocalL(AgnDateTime::ConvertToLocalTimeL(firstInstanceUtc));
		  		}
		  		
		  	if (isStillRpt)
			  	{
			  	//Need to call UntilTimeL because iUntilTime might be set according to the iCount.
			  	repeat->UntilTimeL();
			  	}
			}
		}
	}
  
    
EXPORT_C TBool CAgnSimpleEntry::MoveStartTimeLocalL(const TTime& aNewEntryTimeLocal)
/** This function shifts the time of the entry to the specified time.
This is used when a repeat rule is set which cannot have an instance on the DTSTART time (or DUE time for todos).
E.g. if an event has a DTSTART on a Monday but repeats every Wednesday, the DTSTART will be shifted to the Wednesday.

@internalComponent
*/
	{
	TBool isStillRpt = ETrue;
	TTime startTime = aNewEntryTimeLocal;
	if (Type() == CCalEntry::ETodo)
		{
		//startTime = DurationMinusL(startTime);
		}
	TAgnCalendarTime startAgnTime;
	TAgnCalendarTime endAgnTime;
	if (TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		startAgnTime.SetFloatingL(startTime);
		endAgnTime.SetFloatingL(DurationPlusL(startTime));
		}
	else
		{
		startAgnTime.SetLocalL(startTime);
		endAgnTime.SetLocalL(DurationPlusL(startTime));
		}
					
	SetStartAndEndTimeL(startAgnTime, endAgnTime);
	
	if (iRptDef && iRptDef->RRule())
		{
		if (iRptDef->RRule()->InvariantL())
			{
			iRptDef->ClearRRule();
			isStillRpt = EFalse;
			}
		}
	return isStillRpt;
	}

void CAgnSimpleEntry::StartTimeChanged()
/**
@internalComponent
*/
	{
	if (iRptDef)
		{
		iRptDef->StartTimeChanged();
		}
	}

EXPORT_C TTime CAgnSimpleEntry::DurationPlusL(const TTime& aTime) const
/*
@internalComponent
Return the Time of aTime plus the duration of the entry.
*/
	{
	if ( ! iEndTime.IsSet() || ! StartTime().IsSet())
		{
		return aTime;
		}
	
	TTime returnTime(aTime);

	// add the duration in minutes (there anough minutes to cover the whole of the agenda time range)	
	TTimeIntervalMinutes deltaMinutes;
	iEndTime.LocalL().MinutesFrom(iStartTime.LocalL(), deltaMinutes);
	returnTime += deltaMinutes;
	
	// add the microseconds
	TTimeIntervalMicroSeconds deltaMicroSeconds = iEndTime.LocalL().MicroSecondsFrom(iStartTime.LocalL() + deltaMinutes);
	returnTime += deltaMicroSeconds;
		
	return returnTime;
	}

EXPORT_C TTime CAgnSimpleEntry::DurationMinusL(const TTime& aTime) const
/*
@internalComponent
Return the Time of aTime minus the duration of the entry.
*/
	{
	if ( ! iEndTime.IsSet() || ! StartTime().IsSet())
		{
		return aTime;
		}
	
	TTime returnTime(aTime);

	// add the duration in minutes (there anough minutes to cover the whole of the agenda time range)	
	TTimeIntervalMinutes deltaMinutes;
	iEndTime.LocalL().MinutesFrom(iStartTime.LocalL(), deltaMinutes);
	returnTime -= deltaMinutes;
	
	// add the microseconds
	TTimeIntervalMicroSeconds deltaMicroSeconds = iEndTime.LocalL().MicroSecondsFrom(iStartTime.LocalL() + deltaMinutes);
	returnTime -= deltaMicroSeconds;
		
	return returnTime;
	}

EXPORT_C const TAgnCalendarTime& CAgnSimpleEntry::StartTime() const
/**
@internalComponent
*/
	{
	return iStartTime;
	}

EXPORT_C const TAgnCalendarTime& CAgnSimpleEntry::EndTime() const
/**
@internalComponent
*/
	{
	return iEndTime;
	}
	
EXPORT_C MAgnCalendarTimeMode::TTimeMode CAgnSimpleEntry::TimeMode() const
/**
@internalComponent
*/
	{
	if (!iStartTime.IsSet() && iEndTime.IsSet())
		{
		return iEndTime.TimeMode();
		}
	return iStartTime.TimeMode();
	}

EXPORT_C void CAgnSimpleEntry::ExternalizeL(RWriteStream& aStream, TBool aToBuffer) const
/**
@internalComponent
*/
	{
	doExternalizeL(aStream, aToBuffer);
	aStream.WriteInt32L(iUserInt);
	}

void CAgnSimpleEntry::doExternalizeL(RWriteStream& aStream, TBool aToBuffer) const
	{
	iEntryId.ExternalizeL(aStream);
	if (iRptDef)
		{
		aStream.WriteUint8L(ETrue);
		iRptDef->ExternalizeL(aStream, aToBuffer);
		}
	else
		{
		aStream.WriteUint8L(EFalse);
		}
	
	if (HasAlarm())
		{
		aStream.WriteInt32L(iAlarmOffset);
		}
	else
		{
		aStream.WriteInt32L(KAlarmNotSet);
		}

	aStream.WriteUint8L(iStatus);

	aStream << iLastModifiedDateUtc.Int64();
	aStream.WriteUint8L(iPriority);
	aStream << iStartTime;
	aStream << iEndTime;
	aStream << iCompletedDateUtc.Int64();
	aStream << iLocalUid;
	}

EXPORT_C void CAgnSimpleEntry::ExternalizeGuidHashL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L(iGuidHash);
	}

EXPORT_C void CAgnSimpleEntry::InternalizeL(RReadStream& aStream, TBool aFromBuffer)
/**
@internalComponent
*/
	{
	doInternalizeL(aStream, aFromBuffer);
	iUserInt = aStream.ReadInt32L();
	}

void CAgnSimpleEntry::doInternalizeL(RReadStream& aStream, TBool aFromBuffer)
	{
	iEntryId.InternalizeL(aStream);

	delete iRptDef;
	iRptDef = NULL;
	if (aStream.ReadUint8L())
		{
		iRptDef = CAgnRptDef::NewL(*this);
		iRptDef->InternalizeL(aStream, aFromBuffer);
		}
	iAlarmOffset = aStream.ReadInt32L();
	
	iStatus = static_cast<CCalEntry::TStatus>(aStream.ReadUint8L());

	TInt64 lastChanged;
	aStream >> lastChanged;
	iLastModifiedDateUtc = lastChanged;
	iPriority = aStream.ReadUint8L();
	
	aStream >> iStartTime;
	aStream >> iEndTime;
	StartTimeChanged();

	TInt64 completedDate;
	aStream >> completedDate;
	iCompletedDateUtc = completedDate;
	aStream >> iLocalUid;
	}

EXPORT_C void CAgnSimpleEntry::InternalizeGuidHashL(RReadStream& aStream)
	{
	iGuidHash = aStream.ReadUint32L();
	}


EXPORT_C const TAgnEntryId& CAgnSimpleEntry::EntryId() const
/**
@internalComponent
*/
	{
	return iEntryId;
	}

EXPORT_C void CAgnSimpleEntry::SetEntryId(const TAgnEntryId& aId)
/**
@internalComponent
*/
	{
	iEntryId = aId;
	}
	
EXPORT_C TCalLocalUid CAgnSimpleEntry::LocalUid() const
/** Gets the entry's unique ID. 

The unique ID is assigned when the entry is created, and is preserved during updates. 
Because of this, it can be used to identify the entry during synchronisation.

@return The entry's unique ID.
@internalComponent
*/
	{
	return iLocalUid;
	}
	
EXPORT_C void CAgnSimpleEntry::SetLocalUid(TCalLocalUid aUId)
/** Sets the entry's unique ID.

@param aUId The entry's unique ID.

@internalComponent
*/
	{
	iLocalUid = aUId;
	}

EXPORT_C TUint32 CAgnSimpleEntry::GuidHash() const
/**
@internalComponent
*/
	{
	return iGuidHash;
	}

EXPORT_C void CAgnSimpleEntry::SetGuidHash(TUint32 aGuidHash)
/**
@internalComponent
*/
	{
	iGuidHash = aGuidHash;
	}

EXPORT_C const TAgnCalendarTime& CAgnSimpleEntry::EntryTime() const
/**
@internalComponent
*/
	{
	if (Type() == CCalEntry::ETodo)
		{
		return iEndTime;
		}
	return iStartTime;
	}

EXPORT_C TTime CAgnSimpleEntry::ValidFromTimeLocalL() const
/**
@internalComponent
*/
	{
	TTime startTimeLocal(iStartTime.LocalL());
	TTime firstInstanceLocal(AgnDateTime::MaxDate());
	
	if (iRptDef)
		{
		firstInstanceLocal = iRptDef->FirstInstanceL().LocalL();
		
		if (Type() == CCalEntry::ETodo)
			{
			// for a todo, the instance time minus the duration is the start of the first instance
			firstInstanceLocal = DurationMinusL(firstInstanceLocal);
			}
		}
	
	// If iEndTime is not set then this is an undated todo so we should return that (Time::NullTTime())
	// otherwise we should return the earliest from either the repeat rule or the entry itself
	return ( iEndTime.IsSet() && (firstInstanceLocal < startTimeLocal )) ? firstInstanceLocal : startTimeLocal;
	}

EXPORT_C TTime CAgnSimpleEntry::ValidToTimeLocalL() const
/**
@internalComponent
*/
	{
	TTime endTimeLocal(iEndTime.LocalL());
	TTime lastInstanceLocal(AgnDateTime::MinDate());
	
	if (iRptDef)
		{
		lastInstanceLocal = iRptDef->LastInstanceL().LocalL();
		
		if (Type() != CCalEntry::ETodo)
			{
			// for a non-todo, the until time plus duration is the end of the last instance
			lastInstanceLocal = DurationPlusL(lastInstanceLocal);
			}
		}
	
	// If iEndTime is not set then this is an undated todo so we should return that (Time::NullTTime())
	// otherwise we should return the latest from either the repeat rule or the entry itself
	return ( iEndTime.IsSet() && (lastInstanceLocal > endTimeLocal)) ? lastInstanceLocal : endTimeLocal;
	}

/** 
Sets the user integer for this entry.

@param aUserInt The new value of the user integer. 
@internalComponent
*/
EXPORT_C void CAgnSimpleEntry::SetUserInt( TUint32 aUserInt )
	{
	iUserInt = aUserInt;
	}

/**
Gets the user integer of this entry.

@return The user integer.
@internalComponent
*/
EXPORT_C TUint32 CAgnSimpleEntry::UserInt() const
	{
	return iUserInt;
	}

