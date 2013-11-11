// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "calentryimpl.h"

#include "calclient.h"
#include "agsalarm.h"
#include "agmattachment.h"
#include "agmdate.h"
#include "agmentry.h"
#include "agmpanic.h"
#include "agmcategory.h"
#include "agmattendee.h"
#include "agmcontent.h"
#include <calalarm.h>
#include "calcategoryimpl.h"
#include <calcontent.h>
#include <calrrule.h>
#include <calsession.h>
#include "calsessionimpl.h"
#include "caluserimpl.h"

const TInt KAttendeeGranularity = 2;
const TInt KCategoryGranularity = 1;
const TInt KAttachmentGranularity = 1;


CCalEntryImpl::~CCalEntryImpl()
	{
	if(iSessionImpl)
		{
		iSessionImpl->DecrementReferenceCount();
		}

	if (iLiteEntry)
 		{
		iLiteEntry->DecrementRefCount();
 		}
	if(iCalAttachmentList)
		{
		iCalAttachmentList->ResetAndDestroy();
		delete iCalAttachmentList; 	
		}
	if(iCalAttendeeList)
		{
		iCalAttendeeList->ResetAndDestroy();
		delete iCalAttendeeList;
		}
	if(iCalCategoryList)
		{
		iCalCategoryList->ResetAndDestroy();
		delete iCalCategoryList;
		}
	delete iCalOrganizer;
	delete iFullEntry;
	}

CCalEntryImpl::CCalEntryImpl()
	{
	}

CCalEntryImpl::CCalEntryImpl(CAgnEntry& aEntry, CCalSessionImpl& aSessionImpl)
	: iFullEntry(&aEntry), iSessionImpl(&aSessionImpl)
	{
	iSessionImpl->IncrementReferenceCount();
	}


CCalEntryImpl::CCalEntryImpl(CCalLiteEntry& aEntry, CCalSessionImpl& aSessionImpl)
	: iLiteEntry(&aEntry), iSessionImpl(&aSessionImpl)
 	{
 	iSessionImpl->IncrementReferenceCount();
	iLiteEntry->IncrementRefCount();
 	}

void CCalEntryImpl::ClearSimpleEntry()
	{
	if (iSessionImpl && iLiteEntry)
		{
		iLiteEntry->DecrementRefCount();
		iLiteEntry = NULL;
		}
	}
	
CCalEntryImpl* CCalEntryImpl::NewL(CCalEntry::TType aType, HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum)
	{
	__ASSERT_ALWAYS(aUid && aUid->Length() > 0, User::Leave(KErrArgument));

	CCalEntryImpl* self = new (ELeave) CCalEntryImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aType, aUid, aMethod, aSeqNum); // takes ownership of aUid
	CleanupStack::Pop(self);
	return self;
	}

CCalEntryImpl* CCalEntryImpl::NewL(CCalEntry::TType aType, HBufC8* aUid, CCalEntry::TMethod aMethod,
			TUint aSeqNum, const TCalTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange)
	{
	__ASSERT_ALWAYS(aUid && aUid->Length() > 0, User::Leave(KErrArgument));

	CCalEntryImpl* self = new (ELeave) CCalEntryImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aType, aUid, aMethod, aSeqNum, CalUtils::TCalTimeToTAgnCalendarTimeL(aRecurrenceId), aRange);
	   // takes ownership of aUid
	CleanupStack::Pop(self);
	return self;
	}

CCalEntryImpl* CCalEntryImpl::NewL(CAgnEntry& aEntry, CCalSessionImpl& aSessionImpl)
	{
	CCalEntryImpl* self = new (ELeave) CCalEntryImpl(aEntry, aSessionImpl);
	return self;
	}

CCalEntryImpl* CCalEntryImpl::NewL(CCalLiteEntry& aEntry, CCalSessionImpl& aSessionImpl)
	{
	CCalEntryImpl* self = new (ELeave) CCalEntryImpl(aEntry, aSessionImpl);
	return self;
	}

void CCalEntryImpl::ConstructL(CCalEntry::TType aType, HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum, const TAgnCalendarTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange)
	{
	// Create appropriate CAgnEntry type child entry.
	iFullEntry = CAgnEntry::NewL(aType, aUid, aMethod, aSeqNum, aRecurrenceId, aRange); // takes ownership of aUid
	}


void CCalEntryImpl::ConstructL(CCalEntry::TType aType, HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum)
	{
	// Create appropriate CAgnEntry type parent entry.
	iFullEntry = CAgnEntry::NewL(aType, aUid, aMethod,aSeqNum);
	}

void CCalEntryImpl::LoadFullEntryL()
	{
	if (iLiteEntry)
		{
		// iFullEntry should be NULL at this point
		__ASSERT_DEBUG(iFullEntry == NULL, User::Leave(KErrCorrupt));
		
		// extract full CAgnEntry from TAgnLiteEntry and store it in iFullEntry
		TCalLocalUid id = iLiteEntry->LiteEntry().LocalUid();
		iFullEntry = iSessionImpl->Server().FetchEntryL(id, iSessionImpl->FileId());
		__ASSERT_ALWAYS(iFullEntry, User::Leave(KErrNotFound));
		iFullEntry->SetCollectionId(iLiteEntry->LiteEntry().CollectionId());
		ClearSimpleEntry();
		}
	}

// return the CAgnEntry associated with this entry, loading it from the server if necessary
CAgnEntry* CCalEntryImpl::GetFullEntryL()
	{
	LoadFullEntryL();
	return iFullEntry;
	}

void CCalEntryImpl::PopulateCategoryListL()
	{
	if (!iCalCategoryList)
		{
		LoadFullEntryL();
		iCalCategoryList = new (ELeave) RPointerArray<CCalCategory>(KCategoryGranularity);

		const TInt KCategoryCount = iFullEntry->CategoryCount();
		for (TInt i = 0; i < KCategoryCount; ++i)
			{
			CAgnCategory& agnCategory = iFullEntry->FetchCategory(i); // ownership not taken
			CCalCategoryImpl* impl = CCalCategoryImpl::NewL(&agnCategory); // doesn't take ownership of agnCategory
			CleanupStack::PushL(impl);
			CCalCategory* category = CCalCategory::NewL(impl); // takes ownership of impl
			CleanupStack::Pop(impl);
			CleanupStack::PushL(category);
			iCalCategoryList->AppendL(category); // takes ownership of category
			CleanupStack::Pop(category);
			}
		}
	}

void CCalEntryImpl::PopulateAttendeeListL()
	{
	if(!iCalAttendeeList)
		{
		LoadFullEntryL();

		iCalAttendeeList = new(ELeave) RPointerArray<CCalAttendee>(KAttendeeGranularity);

		const TInt KAttendeeCount = iFullEntry->AttendeeCount();
		for(TInt i = 0; i < KAttendeeCount; ++i)
			{
			CAgnAttendee& agnAttendee = iFullEntry->FetchAttendee(i);
			
			CCalUserImpl* impl = CCalUserImpl::NewL(&agnAttendee);	// doesn't take ownership of agnAttendee
			CleanupStack::PushL(impl);
			CCalAttendee* calAttendee = CCalAttendee::NewL(impl);	// takes ownership of impl
			CleanupStack::Pop(impl);
			
			CleanupStack::PushL(calAttendee);
			iCalAttendeeList->AppendL(calAttendee);
			CleanupStack::Pop(calAttendee);
			}
		}
	}


void CCalEntryImpl::AddAttendeeL(CCalAttendee* aAttendee)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
						User::Leave(KErrNotSupported));

	if (!aAttendee)
		{
		User::Leave( KErrArgument );
		}

	CleanupStack::PushL(aAttendee);
	__ASSERT_ALWAYS(aAttendee != NULL, User::Leave(KErrArgument));
	PopulateAttendeeListL();

	CCalUserImpl* userImpl = aAttendee->Impl();
	CAgnAttendee* attendee = userImpl->Attendee();
	
	// add attendee to CAgnEntry (no longer owned by CCalAttendee)
	userImpl->SetOwnsCAgnAttendee(EFalse);		
	iFullEntry->AddAttendeeL(attendee);		
	
	// store attendee in attendee array too
	iCalAttendeeList->AppendL(aAttendee);
	CleanupStack::Pop(aAttendee);
	}

RPointerArray<CCalAttendee>& CCalEntryImpl::AttendeesL()
	{
	PopulateAttendeeListL();
	return *iCalAttendeeList;
	}

void CCalEntryImpl::DeleteAttendeeL(TInt aIndex)
	{
	LoadFullEntryL();

	if(iCalAttendeeList)
		{
		if (aIndex < 0 || aIndex >= iCalAttendeeList->Count())
			{
			User::Leave(KErrArgument);
			}
		delete (*iCalAttendeeList)[aIndex];
		iCalAttendeeList->Remove(aIndex);
		}

	iFullEntry->DeleteAttendee(aIndex);
	}

void CCalEntryImpl::SetOrganizerL(CCalUser* aUser)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
							User::Leave(KErrNotSupported));

	CleanupStack::PushL(aUser);
	__ASSERT_ALWAYS(aUser != NULL, User::Leave(KErrArgument));
	LoadFullEntryL();
	
	aUser->Impl()->SetOwnsCAgnAttendee(EFalse);
	iFullEntry->SetOrganizerL(aUser->Impl()->Attendee());	//	takes ownership of user immediately

	delete iCalOrganizer;
	iCalOrganizer = aUser;
	CleanupStack::Pop(aUser);
	}

void CCalEntryImpl::SetPhoneOwnerL(const CCalUser* aOwner)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
							User::Leave(KErrNotSupported));
	__ASSERT_ALWAYS(aOwner != NULL, User::Leave(KErrArgument));
	LoadFullEntryL();
	CCalUserImpl* userImpl = aOwner->Impl();
	iFullEntry->SetPhoneOwnerL(userImpl->Attendee());
	}

CCalUser* CCalEntryImpl::PhoneOwnerL()
	{
	LoadFullEntryL();

	CAgnAttendee* agnPhoneOwner = iFullEntry->PhoneOwner();

	if(agnPhoneOwner)
		{	
		//check to see if phone owner is the organizer
		if(OrganizerL())
			{
			if(agnPhoneOwner == iCalOrganizer->Impl()->Attendee())
				{
				return iCalOrganizer;
				}
			}

		PopulateAttendeeListL();
		
		// see if the phone owner is one of the other attendees
		const TInt KAttendeeCount = iCalAttendeeList->Count();
		for(TInt i = 0; i < KAttendeeCount; ++i)
			{
			CCalAttendee* calAttendee = (*iCalAttendeeList)[i];
			if(agnPhoneOwner == calAttendee->Impl()->Attendee())
				{
				return calAttendee;
				}
			}
		}
	return NULL;
	}


CCalUser* CCalEntryImpl::OrganizerL()
	{
	LoadFullEntryL();
	if( ! iCalOrganizer)
		{
		CAgnAttendee* organizer = iFullEntry->Organizer();
		
		if(organizer)
			{
			CCalUserImpl* impl = CCalUserImpl::NewL(organizer);
			CleanupStack::PushL(impl);
			impl->SetOwnsCAgnAttendee(EFalse);
			iCalOrganizer = CCalUser::NewL(impl);
			CleanupStack::Pop(impl);
			}
		}

	return iCalOrganizer;
	}

CTzRules* CCalEntryImpl::GetTzRulesL()
	{
	CAgnRptDef* rptDef = SimpleEntry()->RptDef();
	
	if (rptDef == NULL)
		{
		return NULL;
		}
	return rptDef->CloneTzRulesL();
	}

void CCalEntryImpl::SetTzRulesL(const CTzRules& aTzRules)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
								User::Leave(KErrNotSupported));
	LoadFullEntryL();
	
	if (TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		User::Leave(KErrArgument);
		}

	CAgnRptDef* rptDef = SimpleEntry()->RptDef(); // doesn't take ownership
	
	if (!rptDef)
		{
		User::Leave(KErrNotFound);
		}
   
	rptDef->SetTimeZoneL(aTzRules); // rptDef doesn't take ownership of aTzRules
	}

void CCalEntryImpl::SetTzRulesL()
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
							   User::Leave(KErrNotSupported));
	LoadFullEntryL();
	
	if (TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		User::Leave(KErrArgument);
		}

	CAgnRptDef* rptDef = SimpleEntry()->RptDef();
	if (rptDef == NULL)
		{
		User::Leave(KErrNotFound);
		}
	rptDef->SetTimeZoneL();
	}

void CCalEntryImpl::SetRDatesL(const RArray<TCalTime>& aRDates)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
								User::Leave(KErrNotSupported));
	LoadFullEntryL();
	SetDatesL(ETrue, aRDates);
	}

void CCalEntryImpl::SetExceptionDatesL(const RArray<TCalTime>& aExDates)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
								User::Leave(KErrNotSupported));
	LoadFullEntryL();
	
	if (iFullEntry->RptDef() == NULL)
		{
		User::Leave(KErrNotFound);
		}

	SetDatesL(EFalse, aExDates);
	}
	
//if aDateType is ETrue, add dates to sporadic date list
//if aDateType is EFalse, add dates to exception list
void CCalEntryImpl::SetDatesL(TBool aDateType, const RArray<TCalTime>& aDates)
	{
	const TCalTime::TTimeMode KEntryTimeMode = static_cast<TCalTime::TTimeMode>(TimeMode());
	const TInt KDateCount = aDates.Count();
	TInt i = 0;
	for (i = 0; i < KDateCount; i++)
		{
		if (aDates[i].TimeMode() != KEntryTimeMode)
			{
			User::Leave(KErrArgument);
			}
		}

	CAgnRptDef* rptDef = SimpleEntry()->RptDef();

	TBool entryHasNoRptDef(rptDef == NULL);
	if (entryHasNoRptDef)
		{
		rptDef = CAgnRptDef::NewL(*SimpleEntry());
		CleanupStack::PushL(rptDef);
		}
	else
		{
		if (aDateType)
			{
			rptDef->RemoveAllSporadicDates();
			}
		else
			{
			rptDef->RemoveAllExceptions();
			}
		}

	// store the dates
	for (i = 0; i < KDateCount; ++i)
		{
		TAgnCalendarTime agnTime = CalUtils::TCalTimeToTAgnCalendarTimeL(aDates[i]);
		if (aDateType)
			{
			rptDef->AddSporadicDateL(agnTime);
			}
		else
			{
			rptDef->AddExceptionL(agnTime);
			}
		}

	// if the rptDef didn't exist in the entry already, add it
	if (entryHasNoRptDef)
		{
		iFullEntry->SetRptDefL(*rptDef); // makes a copy of rptDef
		CleanupStack::PopAndDestroy(rptDef);
		}
	}

void CCalEntryImpl::GetRDatesL(RArray<TCalTime>& aRDates)
	{
	aRDates.Reset();
	const CAgnRptDef* KRptDef = SimpleEntry()->RptDef();
	if (KRptDef)
		{
		FetchDatesL(KRptDef->SporadicDateList(), aRDates);
		}
	}

void CCalEntryImpl::GetExceptionDatesL(RArray<TCalTime>& aExDates)
	{
	aExDates.Reset();
	const CAgnRptDef* KRptDef = SimpleEntry()->RptDef();
	if (KRptDef)
		{
		FetchDatesL(KRptDef->Exceptions(), aExDates);
		}
	}
	
void CCalEntryImpl::FetchDatesL(const RArray<TAgnCalendarTime>* aDateList, RArray<TCalTime>& aRDates)
	{
	// aDateList may be NULL
	if (aDateList)
		{
		const TInt KDateCount = aDateList->Count();
		for (TInt i = 0; i < KDateCount; ++i)
			{
			TCalTime calTime = CalUtils::TAgnCalendarTimeToTCalTimeL((*aDateList)[i]);
			aRDates.AppendL(calTime);
			}
		}
	}

CCalEntry::TMethod CCalEntryImpl::MethodL()
	{
	LoadFullEntryL();
	return iFullEntry->Method();
	}
	
TCalTime CCalEntryImpl::RecurrenceIdL()
	{
	// If this entry has been committed to store, the Rec-Id can be retrieved from the
	// CAgnEntryModel by passing the appropraite CAgnEntry.
	// Else try to retrieve it from CAgnEntry's temporary GS data.

	LoadFullEntryL();
	return CalUtils::TAgnCalendarTimeToTCalTimeL(iFullEntry->RecurrenceId());
	}
	
CalCommon::TRecurrenceRange CCalEntryImpl::RecurrenceRangeL()
   	{
   	LoadFullEntryL();
	return iFullEntry->RecurrenceRange();
   	}
	
const TDesC8& CCalEntryImpl::UidL()
	{
	LoadFullEntryL();
	// Does not transfer ownership
	if(iFullEntry->Guid() == KNullDesC8)
		{
		// this entry must be a child entry whose GUID has not been loaded
		__ASSERT_ALWAYS(iFullEntry->GsDataType() == CGsData::EChild, User::Leave(KErrCorrupt)); 
		
		// get parent entry
		TCalLocalUid parentId = iFullEntry->ParentId();
		CAgnEntry* parentEntry = iSessionImpl->Server().FetchEntryL(parentId, iSessionImpl->FileId());
		__ASSERT_ALWAYS(parentEntry, User::Leave(KErrNotFound));
		CleanupStack::PushL(parentEntry);
		HBufC8* guidHbuf = parentEntry->Guid().AllocL();
		CleanupStack::PopAndDestroy(parentEntry);	

		iFullEntry->SetGuid(guidHbuf);
		}
		
	return iFullEntry->Guid();
	}


TInt CCalEntryImpl::SequenceNumberL()
	{
	LoadFullEntryL();
	return iFullEntry->SequenceNumber();
	}


void CCalEntryImpl::SetMethodL(CCalEntry::TMethod aMethod)
	{
	LoadFullEntryL();
	iFullEntry->SetMethod(aMethod);
	}

const RPointerArray<CCalCategory>& CCalEntryImpl::CategoryListL()
	{
	PopulateCategoryListL();

	return *iCalCategoryList;
	}

void CCalEntryImpl::AddCategoryL(CCalCategory* aCategory)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
						User::Leave(KErrNotSupported));

	CleanupStack::PushL(aCategory);
	__ASSERT_ALWAYS(aCategory != NULL, User::Leave(KErrArgument));
	
 	PopulateCategoryListL();

	// add the category to the list
 	iCalCategoryList->AppendL(aCategory);
	CleanupStack::Pop(aCategory);

	TInt err(KErrNone);

	// add category to CAgnEntry
	TRAP(err , iFullEntry->AddCategoryL( aCategory->Impl()->AgnCategory() ) ); // this can leave before taking ownership

 	if (err != KErrNone)
 		{
 		delete (*iCalCategoryList)[iCalCategoryList->Count() - 1];
 		iCalCategoryList->Remove(iCalCategoryList->Count() - 1);
 		User::Leave(err);
 		}

 	aCategory->Impl()->SetOwnAgnCategory(EFalse);
	}

void CCalEntryImpl::DeleteCategoryL(TInt aIndex)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
						User::Leave(KErrNotSupported));
	LoadFullEntryL();
	
	if (aIndex < 0 || aIndex >= iFullEntry->CategoryCount())
		{
		User::Leave(KErrArgument);
		}
			
	iFullEntry->DeleteCategory(aIndex);
	if(iCalCategoryList)
		{
		delete (*iCalCategoryList)[aIndex];
		iCalCategoryList->Remove(aIndex);
		
		// check count of CAgnCategory and CCalCategory is the same
		__ASSERT_DEBUG(iCalCategoryList->Count() == iFullEntry->CategoryCount(), Panic(EAgmErrCategoryCountCorrupt)); 
		}
	}

TBool CCalEntryImpl::GetRRuleL(TCalRRule& aRule)
	{
	const CAgnRptDef* KRptDef = SimpleEntry()->RptDef();
	if (KRptDef == NULL || KRptDef->RRule() == NULL)
		{
		return EFalse;
		}

	const TAgnRpt* KRpt = KRptDef->RRule();

	// get the DTSTART property
//	if (SimpleEntry()->Type() == CCalEntry::ETodo)
//		{
//		aRule.SetDtStart(EndTimeL());
//		}
//	else
		{
		aRule.SetDtStart(StartTimeL());
		}

	TInt repCount = 0;
	TCalTime untilCaltime = CalUtils::TAgnCalendarTimeToTCalTimeL(KRpt->UntilTimeL());
		
	if (untilCaltime.TimeLocalL() == TCalTime::MaxTime())
		{
		//Idealy, only UTC maximum time should be used to indicate the repeating forever. However, for a behaviour compatible reason,
		//when the client sets local or UTC repeating until time to be maximum time, it is treated as repeating forever. 
		untilCaltime.SetTimeUtcL(TCalTime::MaxTime());
		}
	else
		{
		repCount = KRpt->InstanceCountL();
		}

	aRule.SetUntilAndCount(untilCaltime, repCount);	
	
	// get the INTERVAL property
	aRule.SetInterval(KRpt->Interval());

	// get the TCalRRule's type, and type-specific data
	switch (KRpt->Type())
		{
		case TAgnRpt::EDaily:
			aRule.SetType(TCalRRule::EDaily);
			break;
		case TAgnRpt::EYearlyByDate:
			aRule.SetType(TCalRRule::EYearly);
			break;
		case TAgnRpt::EYearlyByDay:
			{
			aRule.SetType(TCalRRule::EYearly);

			TDay day;
			TAgnRpt::TWeekInMonth weekInMonth;
			TMonth month;
			TInt year;

			const TAgnYearlyByDayRpt* KYearlyRpt = static_cast<const TAgnYearlyByDayRpt*>(KRpt);
			KYearlyRpt->GetStartDayL(day, weekInMonth, month, year);
			TInt8 week = CalUtils::AgnWeekInMonthToWeekNumberL(weekInMonth);
			TCalRRule::TDayOfMonth dayOfMonth(day, week);

			RArray<TCalRRule::TDayOfMonth> dayOfMonthArray;
			CleanupClosePushL(dayOfMonthArray);
			dayOfMonthArray.AppendL(dayOfMonth);
			aRule.SetByDay(dayOfMonthArray);
			CleanupStack::PopAndDestroy(&dayOfMonthArray);

			RArray<TMonth> monthArray;
			CleanupClosePushL(monthArray);
			monthArray.AppendL(month);
			aRule.SetByMonth(monthArray);
			CleanupStack::PopAndDestroy(&monthArray);
			}
			break;
		case TAgnRpt::EWeekly:
			{
			aRule.SetType(TCalRRule::EWeekly);
			const TAgnWeeklyRpt* KWeeklyRpt = static_cast<const TAgnWeeklyRpt*>(KRpt);
			aRule.SetWkSt(KWeeklyRpt->FirstDayOfWeek());

			RArray<TDay> array;
			CleanupClosePushL(array);
			TDay day = EMonday;
			while (day <= ESunday)
				{
				if (KWeeklyRpt->IsDaySet(day))
					{
					array.AppendL(day);
					}
				day = static_cast<TDay>(static_cast<TInt>(day) + 1);
				}
			aRule.SetByDay(array);
			CleanupStack::PopAndDestroy(&array);
			}
			break;
		case TAgnRpt::EMonthlyByDays:
			{
			aRule.SetType(TCalRRule::EMonthly);
			const TAgnMonthlyByDaysRpt* KMonthlyByDayRpt = static_cast<const TAgnMonthlyByDaysRpt*>(KRpt);

			RArray<TCalRRule::TDayOfMonth> dayArray;
			CleanupClosePushL(dayArray);
			
			TAgnRpt::TWeekInMonth week = TAgnRpt::EFirst;
			while (week <= TAgnRpt::ELast)
				{
				TDay day = EMonday;
				while (day <= ESunday)
					{
					if (KMonthlyByDayRpt->IsDaySet(day, week))
						{
						TInt8 weekInt = CalUtils::AgnWeekInMonthToWeekNumberL(week);
						TCalRRule::TDayOfMonth dayOfMonth(day, weekInt);
						dayArray.AppendL(dayOfMonth);
						}
					day = static_cast<TDay>(static_cast<TInt>(day) + 1);
					}
			
				week = static_cast<TAgnRpt::TWeekInMonth>(static_cast<TInt>(week) + 1);
				}
			//coverity [dead_error_begin]
			aRule.SetByDay(dayArray);

			CleanupStack::PopAndDestroy(&dayArray); // dayArray.Close()
			}
			break;
		case TAgnRpt::EMonthlyByDates:
			{
			aRule.SetType(TCalRRule::EMonthly);
			const TAgnMonthlyByDatesRpt* KMonthlyByDateRpt = static_cast<const TAgnMonthlyByDatesRpt*>(KRpt);

			RArray<TInt> array;
			CleanupClosePushL(array);
			for (TInt i = 0; i < 31; ++i) // dates run from 0 to 30
				{
				if (KMonthlyByDateRpt->IsDateSet(i))
					{
					array.AppendL(i);
					}
				}
			aRule.SetByMonthDay(array);
			CleanupStack::PopAndDestroy(&array);
			}
			break;
		default:
			CalUtils::Panic(EInvalidRepeatRuleType);
			break;
		}

	return ETrue;
	}

TBool CCalEntryImpl::CompareL(const CCalEntry& aEntry)
	{
	// load full CAgnEntries before comparison
	LoadFullEntryL();
	aEntry.Impl()->LoadFullEntryL();
	
	// Load both entry's summary, description and alarm action data before comparing - these properties
	// are stored in separate streams so may not be loaded at this point
	SummaryL();
	aEntry.SummaryL();
	DescriptionL();
	aEntry.DescriptionL();
	delete AlarmL();
	delete aEntry.AlarmL();
	
	return (iFullEntry->CompareL(*aEntry.Impl()->GetFullEntryL()));
	}

void CCalEntryImpl::CopyFromL(CCalEntryImpl& aOther, CCalEntry::TCopyType aCopyType)
	{
	TCalCollectionId collectionId = 0;
	if (iLiteEntry)
		{
		TUint8 collectionId = aOther.SimpleEntry()->CollectionId();
		}
	aOther.LoadFullEntryL();
	// Delete old state.
	ClearSimpleEntry();

	if(iCalAttendeeList)
		{
		iCalAttendeeList->ResetAndDestroy();
		delete iCalAttendeeList; 	
		iCalAttendeeList = NULL;
		}

	if(iCalCategoryList)
		{
		iCalCategoryList->ResetAndDestroy();
		delete iCalCategoryList; 
		iCalCategoryList = NULL;
		}

	delete iCalOrganizer;
	iCalOrganizer = NULL;

	if(iSessionImpl)
		{
        // Use this entry's existing uid.
		iSessionImpl->DecrementReferenceCount();
		}
	iSessionImpl = aOther.iSessionImpl;
	if (iSessionImpl)
		{
		iSessionImpl->IncrementReferenceCount();
		}

	// ensure other entry has a full CAgnEntry
	aOther.LoadFullEntryL();
	
	// ensure other entry has summary, description and alarm action properties loaded - these
	// are stored in separate streams
	aOther.SummaryL();
	aOther.DescriptionL();
	delete aOther.AlarmL();
	
	iFullEntry->CopyFromL(*aOther.iFullEntry, aCopyType);
	iFullEntry->SetLastModifiedDate();
	iFullEntry->SetCollectionId(collectionId);
	}

void CCalEntryImpl::SetAlarmL(CCalAlarm* aAlarm)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
						User::Leave(KErrNotSupported));

	LoadFullEntryL();
	
	if ( aAlarm == NULL ) 
		{
		// if aAlarm is NULL, clear the alarm data
		iFullEntry->ClearAlarm();
		}
	else
		{
		// This method does not take ownership of aAlarm, so need to copy the alarm details
		// before adding to the entry.
		
		// the alarm must be on the same day in local time as the event
		TDateTime dt = iFullEntry->EntryTime().LocalL().DateTime();
		TInt minutes = dt.Hour()*60 + dt.Minute();
		__ASSERT_ALWAYS(minutes - aAlarm->TimeOffset().Int() <= KAgnMinutesInADay, User::Leave(KErrNotSupported));

		iFullEntry->SetAlarmOffset(aAlarm->TimeOffset());
	 	iFullEntry->SetAlarmSoundNameL(aAlarm->AlarmSoundNameL());

		CCalContent* alarmAction = aAlarm->AlarmAction(); // doesn't take ownership
		
		if ( alarmAction )
			{
			CAgnContent* agnAlarmAction = static_cast<CAgnContent*>(alarmAction->Impl());
			
			if ( agnAlarmAction )
				{
				CAgnContent* alarmActionCopy = agnAlarmAction->CloneL();
				iFullEntry->SetAlarmAction(alarmActionCopy); // entry takes ownership of alarmActionCopy
				}					
			}
		else
		    {
		    iFullEntry->SetAlarmAction(NULL); 
		    }
		}
	}

CCalAlarm* CCalEntryImpl::AlarmL()
	{
	LoadFullEntryL();
	
	if (!iFullEntry->HasAlarm())
		{
		return NULL;
		}
        
	TTime time = iFullEntry->EntryTime().LocalL();
	
	// Invalid or non-existant entries are set on import to NULL time
	// That being the case, we return NULL for the Alarm.
	TDateTime timeOfEvent;
	if(time == Time::NullTTime())
		{
		return NULL;
		}
	else
		{
		timeOfEvent = time.DateTime();
		}

	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	
	alarm->SetTimeOffset(iFullEntry->AlarmOffset().Int());
	alarm->SetAlarmSoundNameL(iFullEntry->AlarmSoundName());
	
	if(!iFullEntry->AlarmActionIsLoaded())
		{
		CAgnContent* agnAlarmaction = iSessionImpl->Server().RestoreAlarmActionL(iFullEntry->AlarmActionStreamId(), iSessionImpl->FileId());
		// Restore should always return something, even if it's only a Null Descriptor
		iFullEntry->SetAlarmAction(agnAlarmaction);
		}

	if (iFullEntry->AlarmAction())
		{
		CCalContent* alarmAction = CCalContent::NewL();
		CleanupStack::PushL(alarmAction);
		CAgnContent* agnAlarmActionCopy = iFullEntry->AlarmAction()->CloneL();
		alarmAction->SetImpl(agnAlarmActionCopy); // Takes ownership of agnAlarmAction.
		alarm->SetAlarmAction(alarmAction); // Takes ownership of alarmAction.
		CleanupStack::Pop(alarmAction);
		}

	CleanupStack::Pop(alarm);
	return alarm;
	}

void CCalEntryImpl::SetCommonRptDataL(const TCalRRule& aRpt, CAgnRptDef& aRptRule)
	{
	// set repeat rule data which is not specific to the rule type
	// Both repeating count and until time could be set. If so, the one come earlist will be set as repeating until time.
	// Note that the repeating until time will be adjusted in TAgnRpt::UntilTimeL() when the entry is stored (CCalEntryViewImpl::DoStore and 
	// CAgnSimpleEntry::AdjustStartUntilTimeForRepEntryL)By then, the TZ rule has been set to a repeating entry.
	aRptRule.SetInterval(aRpt.Interval());
	const_cast<TAgnRpt*> (aRptRule.RRule())->SetCount(aRpt.Count());

    TTime untilTime = aRpt.Until().TimeLocalL();
    if (untilTime != Time::NullTTime())
    	{
		TAgnCalendarTime untilTimeTAgn = CalUtils::TCalTimeToTAgnCalendarTimeL(aRpt.Until());
		aRptRule.SetUntilTime(untilTimeTAgn);
    	}
	}

void CCalEntryImpl::SetRRuleL(const TCalRRule& aRpt)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
						User::Leave(KErrNotSupported));

	if (aRpt.Until().TimeLocalL() != Time::NullTTime() && aRpt.DtStart().TimeLocalL() >= aRpt.Until().TimeLocalL())
		{
		// if Until time is set to earlier than the start time and is non-NULL, leave
		User::Leave(KErrArgument);
		}
	LoadFullEntryL();

	//Leaves if this is a todo entry and its end date has not been set or this is a non-todo entry and its start date has not been set
	//since repeating pattern is based on those time attributes.
	if ( ! iFullEntry->EntryTime().IsSet())
		{
		User::Leave(KErrArgument);
		}

	TBool createdNewRptDef = EFalse;
	CAgnRptDef* rptDef = iFullEntry->RptDef();
	if(rptDef == NULL)
		{
		rptDef = CAgnRptDef::NewL(*iFullEntry);
		CleanupStack::PushL(rptDef);
		createdNewRptDef = ETrue;
		}

	switch (aRpt.Type())
		{
		case TCalRRule::EDaily:
			{
			TAgnDailyRpt daily(*rptDef);
			rptDef->SetRRuleL(daily);
			}
			break;
		case TCalRRule::EWeekly:
			{
			TAgnWeeklyRpt weekly(*rptDef);
			weekly.SetFirstDayOfWeek(aRpt.WkSt());
			RArray<TDay> array;
			CleanupClosePushL(array);
			aRpt.GetByDayL(array);
			const TInt KWeeklyDayCount = array.Count();
			for (TInt i = 0; i < KWeeklyDayCount; ++i)
				{
				weekly.SetDay(array[i]);
				}
			CleanupStack::PopAndDestroy(&array);
			
			rptDef->SetRRuleL(weekly);
			}
			break;
		case TCalRRule::EMonthly:
			{
			RArray<TInt> array;
			CleanupClosePushL(array);
			aRpt.GetByMonthDayL(array);
			if (array.Count() <= 0)
				{
				// may be monthly by week
				RArray<TCalRRule::TDayOfMonth> dayArray;
				CleanupClosePushL(dayArray);
				aRpt.GetByDayL(dayArray);
				const TInt KMonthlyDayCount = dayArray.Count();
				if (KMonthlyDayCount > 0)
					{
					TAgnMonthlyByDaysRpt monthlyByDayAndWeek(*rptDef);
					for (TInt i = 0; i < KMonthlyDayCount; ++i)
						{
						TCalRRule::TDayOfMonth dayOfMonth = dayArray[i];
						TAgnRpt::TWeekInMonth weekNum = CalUtils::WeekNumberToAgnWeekInMonthL(dayOfMonth.WeekInMonth());
						monthlyByDayAndWeek.SetDay(dayOfMonth.Day(), weekNum);
						}
					
					rptDef->SetRRuleL(monthlyByDayAndWeek);
					}
				CleanupStack::PopAndDestroy(&dayArray); // dayArray.Close()
				}
			else
				{
				TAgnMonthlyByDatesRpt monthlyByDate(*rptDef);
				const TInt KMonthlyDateCount = array.Count();
				for (TInt i = 0; i < KMonthlyDateCount; ++i)
					{
					monthlyByDate.SetDate(array[i]);
					}
				
				rptDef->SetRRuleL(monthlyByDate);
				}
			CleanupStack::PopAndDestroy(&array); // array.Reset();
			}
			break;
		case TCalRRule::EYearly:
			{
			RArray<TCalRRule::TDayOfMonth> dayArray;
			CleanupClosePushL(dayArray);
			aRpt.GetByDayL(dayArray);
			if (dayArray.Count() > 0)
				{
				RArray<TMonth> monthArray;
				CleanupClosePushL(monthArray);
				aRpt.GetByMonthL(monthArray);
				if (monthArray.Count() > 0)
					{
					TDay day = dayArray[0].Day();
					TAgnRpt::TWeekInMonth week = CalUtils::WeekNumberToAgnWeekInMonthL(dayArray[0].WeekInMonth());
					TMonth month = monthArray[0];

					TAgnYearlyByDayRpt yearlyByDay(*rptDef);

					// Check that the first month to repeat on is earlier than the repeat dtstart date...
					TDateTime rptStartTime = aRpt.DtStart().TimeUtcL().DateTime();
					TInt year = rptStartTime.Year();
					if (month < rptStartTime.Month())
						{
						// ...if not, start the following year
						year++;
						}
						
					TTime newStartDay = yearlyByDay.FindStartDayL(day, week, month, year);
					TCalTime newStartDaycal;
					newStartDaycal.SetTimeLocalL(newStartDay);
					const_cast<TCalRRule&>(aRpt).SetDtStart(newStartDaycal);
					TTime untilTimeLocal = yearlyByDay.UntilTimeL().LocalL();
					TDateTime rptUntilTime = untilTimeLocal.DateTime();
					if (aRpt.Count() != 0 || year < rptUntilTime.Year() ||
						(year == rptUntilTime.Year() && month < rptUntilTime.Month())
						)
						{
                        // The count has been set
                        // or the first month to repeat on is not later than the repeat until date
						rptDef->SetRRuleL(yearlyByDay);
						}
					}
				else
					{
					User::Leave(KErrArgument);	
					}	
				CleanupStack::PopAndDestroy(&monthArray);
				}
			else
				{
				TAgnYearlyByDateRpt yearlyByDate(*rptDef);
				rptDef->SetRRuleL(yearlyByDate);
				}
			CleanupStack::PopAndDestroy(&dayArray);
			}
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	
	if (rptDef->RRule())
		{
		SetCommonRptDataL(aRpt, *rptDef);
		}
	
	if (createdNewRptDef)
		{
		if (rptDef->RRule())
			{
			iFullEntry->SetRptDefL(*rptDef);
			}
		CleanupStack::PopAndDestroy(rptDef);
		}
	
	//Move the entry start date is same as repeating date
	
	TDateTime newEntryDateTime;
	if(EntryTypeL() == CCalEntry::ETodo)
		{
		newEntryDateTime = EndTimeL().TimeLocalL().DateTime();	
		}
	else
		{
		newEntryDateTime = StartTimeL().TimeLocalL().DateTime();		
		}

	newEntryDateTime.SetYear(aRpt.DtStart().TimeLocalL().DateTime().Year());
	newEntryDateTime.SetMonth(aRpt.DtStart().TimeLocalL().DateTime().Month());
	newEntryDateTime.SetDay(aRpt.DtStart().TimeLocalL().DateTime().Day());
	
	//adjust the entrytime (it the entry start time for non-todos and end time for todos) 
	iFullEntry->MoveStartTimeLocalL(newEntryDateTime);
	}

void CCalEntryImpl::ClearRepeatingPropertiesL()
	{
	SimpleEntry()->ClearRepeat();
	}

TCalTime CCalEntryImpl::NextInstanceForLocalUIDL(const TCalTime& aTime)
	{
	const TTime KTimeUtc = aTime.TimeUtcL() + TTimeIntervalMicroSeconds(1);
	TTime nextTimeUtc = Time::NullTTime();
	
	const CAgnRptDef* KRptDef = SimpleEntry()->RptDef();
	if (KRptDef)
		{
		if (KRptDef->NudgeNextInstanceUtcL(KTimeUtc, nextTimeUtc) == EFalse)
			{
			nextTimeUtc = Time::NullTTime();
			}
		}
	else
		{
		TTime entryTimeUtc = SimpleEntry()->EntryTime().UtcL();
		if (KTimeUtc < entryTimeUtc)
			{
			nextTimeUtc = entryTimeUtc;
			}
		}
	
	TCalTime calTime;
	calTime.SetTimeUtcL(nextTimeUtc);
	return calTime;
	}

TCalTime CCalEntryImpl::PreviousInstanceForLocalUIDL(const TCalTime& aTime)
	{
	const TTime KTimeUtc = aTime.TimeUtcL() - TTimeIntervalMicroSeconds(1);
	TTime prevTimeUtc = Time::NullTTime();
	
	const CAgnRptDef* KRptDef = SimpleEntry()->RptDef();
	if (KRptDef)
		{
		if (KRptDef->NudgePreviousInstanceUtcL(KTimeUtc, prevTimeUtc) == EFalse)
			{
			prevTimeUtc = Time::NullTTime();
			}
		}
	else
		{
		TTime entryTimeUtc = SimpleEntry()->EntryTime().UtcL();
		if (KTimeUtc > entryTimeUtc)
			{
			prevTimeUtc = entryTimeUtc;
			}
		}
	
	TCalTime calTime;
	calTime.SetTimeUtcL(prevTimeUtc);
	return calTime;
	}
	
TCalTime CCalEntryImpl::LastModifiedDateL()
	{
	TTime lastModifiedEntry = SimpleEntry()->LastModifiedDateUtc();
	if(iSessionImpl && SimpleEntry()->RptDef() && SimpleEntry()->TimeMode()!= MAgnCalendarTimeMode::EFloating)
		{
		TTime lastModifiedTzChange = iSessionImpl->TzRulesLastModifiedDateL();
		if(lastModifiedTzChange>lastModifiedEntry)
			{
			lastModifiedEntry = lastModifiedTzChange;
			}
		}
	TCalTime lastModifiedTime;
	lastModifiedTime.SetTimeUtcL(lastModifiedEntry);
	return lastModifiedTime;
	}

TCalTime CCalEntryImpl::DTStampL()
	{
	LoadFullEntryL();
	TCalTime dTStampTime;
	dTStampTime.SetTimeUtcL(iFullEntry->DTStampUtcL());
	return dTStampTime;
	}

void CCalEntryImpl::SetLocationL(const TDesC& aLocation)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
						User::Leave(KErrNotSupported));
	LoadFullEntryL();
	iFullEntry->SetLocationL(aLocation);
	}

const TDesC& CCalEntryImpl::LocationL()
	{
	LoadFullEntryL();
	return iFullEntry->Location();
	}

void CCalEntryImpl::SetPriorityL(TUint aPriority)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
						User::Leave(KErrNotSupported));
	LoadFullEntryL();
	iFullEntry->SetPriority(aPriority);
	}

TUint CCalEntryImpl::PriorityL()
	{
	return SimpleEntry()->Priority();
	}

TCalTime CCalEntryImpl::StartTimeL()
	{
	return CalUtils::TAgnCalendarTimeToTCalTimeL(SimpleEntry()->StartTime());
	}

TCalTime CCalEntryImpl::EndTimeL()
	{
	TCalTime calEndTime; // initialised to Null time on construction
	if (SimpleEntry()->Type() != CCalEntry::EReminder)
		{
		calEndTime = CalUtils::TAgnCalendarTimeToTCalTimeL(SimpleEntry()->EndTime());
		}
	return calEndTime;
	}

void CCalEntryImpl::SetStartAndEndTimeL(const TCalTime& aStartTime, const TCalTime& aEndTime)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
						User::Leave(KErrNotSupported));
	LoadFullEntryL();
	TCalTime::TTimeMode mode = aStartTime.TimeMode();
	if (aEndTime.TimeUtcL() != Time::NullTTime())
		{
		if (aStartTime.TimeUtcL() != Time::NullTTime())
			{
			// both times are non-null so check the modes
			__ASSERT_ALWAYS(aStartTime.TimeMode() == aEndTime.TimeMode(), User::Leave(KErrArgument));

			// both times are non-null so check that end time is not earlier than start time
			__ASSERT_ALWAYS(aStartTime.TimeUtcL() <= aEndTime.TimeUtcL(), User::Leave(KErrArgument));
			}
		else
			{
			// start time non-null so fetch mode from end time
			mode = aEndTime.TimeMode();
			}
		}
	__ASSERT_ALWAYS(mode == TCalTime::EFixedUtc || mode == TCalTime::EFloating, User::Leave(KErrArgument));

	// either this is a parent entry, or time mode should match recurrence ID
	__ASSERT_ALWAYS(RecurrenceIdL().TimeUtcL() == Time::NullTTime() ||
					mode == RecurrenceIdL().TimeMode(),
					User::Leave(KErrArgument));		

	TAgnCalendarTime agnStartTime = CalUtils::TCalTimeToTAgnCalendarTimeL(aStartTime);
	TAgnCalendarTime agnEndTime = CalUtils::TCalTimeToTAgnCalendarTimeL(aEndTime);

	const CCalEntry::TType KType = SimpleEntry()->Type();
		
	switch (KType)
		{
		case CCalEntry::ETodo:
			// for todos, if end time is not set, set start time to null as well - this is an undated entry
			if ( ! agnEndTime.IsSet())
				{
				agnStartTime = agnEndTime;
				}
			break;
		case CCalEntry::EReminder:
			// for reminders, end time is the same as start time
			agnEndTime = agnStartTime;
			break;
		case CCalEntry::EAppt:
		case CCalEntry::EAnniv:
		case CCalEntry::EEvent:
			// for other entries, if end time is not set, make it the same as the start time
			if ( ! agnEndTime.IsSet())
				{
				agnEndTime = agnStartTime;
				}
			break;
		default:
			// bad entry type
			User::Leave(KErrArgument);
			break;
		}
		
	iFullEntry->SetStartAndEndTimeL(agnStartTime, agnEndTime);
	}

MAgnCalendarTimeMode::TTimeMode CCalEntryImpl::TimeMode() const
	{
	return SimpleEntry()->TimeMode();
	}

TCalTime CCalEntryImpl::CompletedTimeL()
	{
	TTime completedDateUtc(Time::NullTTime());
	if (SimpleEntry()->Type() == CCalEntry::ETodo)
		{
		completedDateUtc = SimpleEntry()->CompletedDateUtc();
		}
	TCalTime calCompletedDate;
	calCompletedDate.SetTimeUtcL(completedDateUtc);
	return calCompletedDate;
	}

void CCalEntryImpl::SetCompletedL(TBool aCompleted, const TCalTime& aDate)
	{
	LoadFullEntryL();
	if (SimpleEntry()->Type() == CCalEntry::ETodo)
		{
		if (aCompleted)
			{
			SimpleEntry()->SetCompletedDateUtcL(aDate.TimeUtcL());
			SetStatusL(CCalEntry::ETodoCompleted);
			}
		else
			{
			SimpleEntry()->SetIncomplete(); // resets completed date to null time
			SetStatusL(CCalEntry::ETodoNeedsAction);
			}
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	}

void CCalEntryImpl::SetSummaryL(const TDesC& aSummary)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
			User::Leave(KErrNotSupported));
	LoadFullEntryL();
	HBufC* summ = aSummary.AllocL();
	iFullEntry->SetSummary(summ); // takes ownership of aSummary
	}

const TDesC& CCalEntryImpl::SummaryL()
	{
	LoadFullEntryL();

	if(!iFullEntry->SummaryIsLoaded())
		{
		__ASSERT_DEBUG(iFullEntry->SummaryStreamId() != KNullStreamId, Panic(EAgmNoNotesStreamId));
		HBufC* summary = iSessionImpl->Server().RestoreTextL(iFullEntry->SummaryStreamId(), iSessionImpl->FileId());
		// Restore should always return something, even if it's only a Null Descriptor
		iFullEntry->SetSummary(summary);
		}

	return iFullEntry->Summary();
	}

void CCalEntryImpl::SetDescriptionL(const TDesC& aDescription)
	{
	LoadFullEntryL();
	HBufC* desc = aDescription.AllocL();
	iFullEntry->SetDescription(desc); // takes ownership of aDescription
	}

const TDesC& CCalEntryImpl::DescriptionL()
	{
	LoadFullEntryL();
	
	if(!iFullEntry->DescriptionIsLoaded())
		{
		__ASSERT_DEBUG(iFullEntry->DescriptionStreamId() != KNullStreamId, Panic(EAgmNoNotesStreamId));
		HBufC* description = iSessionImpl->Server().RestoreTextL(iFullEntry->DescriptionStreamId(), iSessionImpl->FileId());
		// Restore should always return something, even if it's only a Null Descriptor
		iFullEntry->SetDescription(description);
		}

	return iFullEntry->Description();
	}

CCalEntry::TType CCalEntryImpl::EntryTypeL()
	{
	return SimpleEntry()->Type();
	}

void CCalEntryImpl::SetStatusL(CCalEntry::TStatus aStatus)
	{
	LoadFullEntryL();
	switch (aStatus)
		{
		case CCalEntry::ETentative:
		case CCalEntry::EConfirmed:
			__ASSERT_ALWAYS(EntryTypeL() != CCalEntry::ETodo, User::Leave(KErrNotSupported));
			break;
		case CCalEntry::ECancelled:
			SimpleEntry()->SetIncomplete();
			break;
		
		case CCalEntry::ETodoCompleted:
		case CCalEntry::ETodoNeedsAction:
		case CCalEntry::ETodoInProcess:
			__ASSERT_ALWAYS(EntryTypeL() == CCalEntry::ETodo, User::Leave(KErrNotSupported));
			break;
		default:
			// do nothing
			break;
		}
	if (aStatus == CCalEntry::ETodoCompleted)
		{
		if (iFullEntry->CompletedDateUtc() == Time::NullTTime())
			{
			TTime time;
			time.UniversalTime();
			iFullEntry->SetCompletedDateUtcL(time);
			}
		}
	
	iFullEntry->SetStatus(aStatus);
	}

void CCalEntryImpl::SetReplicationStatusL(CCalEntry::TReplicationStatus aReplicationStatus)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
				User::Leave(KErrNotSupported));
	LoadFullEntryL();
	iFullEntry->SetReplicationStatusL(aReplicationStatus);
	}

CCalEntry::TReplicationStatus CCalEntryImpl::ReplicationStatusL()
	{
	LoadFullEntryL();
	return iFullEntry->ReplicationStatus();
	}

CCalEntry::TStatus CCalEntryImpl::StatusL()
	{
	return SimpleEntry()->Status();
	}

void CCalEntryImpl::SetSequenceNumberL(TInt aSeqNum)
	{
	LoadFullEntryL();
	iFullEntry->SetSequenceNumber(aSeqNum);
	}

void CCalEntryImpl::SetLastModifiedDateL()
	{
	LoadFullEntryL();
	iFullEntry->SetLastModifiedDate();
	}
	
void CCalEntryImpl::SetLastModifiedDateL(const TCalTime& aModifiedTime)
	{
	LoadFullEntryL();
	iFullEntry->SetLastModifiedDateUtc(aModifiedTime.TimeUtcL());

	// for entry type ENote,set the modified time as the start/end time
	if(EntryTypeL() == CCalEntry::ENote)
		{
		TAgnCalendarTime agnModifiedTime =
				CalUtils::TCalTimeToTAgnCalendarTimeL(aModifiedTime);
		iFullEntry->SetStartAndEndTimeL(agnModifiedTime, agnModifiedTime);
		}
	}

void CCalEntryImpl::SetDTStampL(const TCalTime& aDTStampTime)
	{
	LoadFullEntryL();
	iFullEntry->SetDTStampUtcL(aDTStampTime.TimeUtcL());
	}

TAgnEntryId CCalEntryImpl::AgnEntryIdL()
	{
	return SimpleEntry()->EntryId();
	}

void CCalEntryImpl::SetLocalUidL(TCalLocalUid aLocalId)
	{
	LoadFullEntryL();
	iFullEntry->SetLocalUid(aLocalId);
	}

TCalLocalUid CCalEntryImpl::LocalUidL()
	{
	if(iLiteEntry)
		{
		return iLiteEntry->LiteEntry().LocalUid();
		}
	else
		{
		return iFullEntry->LocalUid();
		}
	}

TUint8 CCalEntryImpl::ShortFileIdL()
	{
	if(iLiteEntry)
		{
		return iLiteEntry->LiteEntry().CollectionId();
		}
	else
		{
		return iFullEntry->CollectionId();
		}
	}

CCalEntry::TTransp CCalEntryImpl::BusyStatusL()
	{
	LoadFullEntryL();
	return iFullEntry->BusyStatus();
	}

void CCalEntryImpl::SetBusyStatusL(CCalEntry::TTransp aBusyStatus)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
					User::Leave(KErrNotSupported));
	LoadFullEntryL();
	iFullEntry->SetBusyStatusL(aBusyStatus);
	}

CCalGeoValue* CCalEntryImpl::GeoValueL()
	{
	LoadFullEntryL();
	TReal geoLatitude;
	TReal geoLongitude;
	iFullEntry->GeoValue(geoLatitude,geoLongitude);
	if(geoLatitude != KGEODefaultValue && geoLongitude != KGEODefaultValue)
		{
		CCalGeoValue* geoValue = CCalGeoValue::NewL();
		CleanupStack::PushL(geoValue);
		geoValue->SetLatLongL(geoLatitude,geoLongitude);
		CleanupStack::Pop(geoValue);
		return geoValue;
		}
	return NULL;
	}

void CCalEntryImpl::SetGeoValueL(const CCalGeoValue& aGeoValue)
	{
	__ASSERT_ALWAYS( EntryTypeL() != CCalEntry::ENote,
					User::Leave(KErrNotSupported));

	LoadFullEntryL();
	TReal geoLatitude;
	TReal geoLongitude;
	aGeoValue.GetLatLong(geoLatitude,geoLongitude);
	iFullEntry->SetGeoValueL(geoLatitude,geoLongitude);
	}

void CCalEntryImpl::ClearGeoValueL()
	{
	LoadFullEntryL();
	iFullEntry->SetGeoValueL(KGEODefaultValue,KGEODefaultValue);
	}
	
TCalTime CCalEntryImpl::FindRptUntilTimeL(TInt aCount)
	{
	const CAgnRptDef* KRptDef = SimpleEntry()->RptDef();
	
	TTime untilTime = Time::NullTTime();
	
	if (KRptDef)
		{
		untilTime = KRptDef->RRule()->FindRptUntilTimeLocalL(aCount);
		}
	
	TCalTime calUntilTime;
	calUntilTime.SetTimeLocalL(untilTime);
	
	return calUntilTime;
	}


  void CCalEntryImpl::AddAttachmentL(CCalAttachment* aAttachment)
   	{
   	__ASSERT_ALWAYS(aAttachment, User::Leave(KErrArgument));
   	
	PopulateAttachmentListL();
	iCalAttachmentList->AppendL(aAttachment);
	
	TRAPD(err , iFullEntry->AddAttachmentL(&aAttachment->Impl()));
 
	if (err != KErrNone)
 		{
		TInt count = iCalAttachmentList->Count();
 		iCalAttachmentList->Remove(count - 1);
 		User::Leave(err);
 		}
 		
   	aAttachment->Impl().ClearFlag(CAgnAttachment::EOwnedByCalAttachment);

   	}

void CCalEntryImpl::PopulateAttachmentListL()
	{
   	if (!iCalAttachmentList)
   		{

		LoadFullEntryL();
		iCalAttachmentList = new (ELeave) RPointerArray<CCalAttachment>(KAttachmentGranularity);

		const TInt KAttachmentCount = iFullEntry->AttachmentCount();
		for (TInt i = 0; i < KAttachmentCount; ++i)
			{
			CAgnAttachment& agnAttachment = iFullEntry->Attachment(i);
			CCalAttachment* calAttachment = CCalAttachment::NewL(agnAttachment, *iSessionImpl);
			CleanupStack::PushL(calAttachment);
			agnAttachment.ClearFlag(CAgnAttachment::EOwnedByCalAttachment);
			iCalAttachmentList->AppendL(calAttachment);
			CleanupStack::Pop(calAttachment);
			}
   		}

   	}	
	
void CCalEntryImpl::DeleteAttachmentL(const CCalAttachment& aAttachment)
	{
	LoadFullEntryL();
	TInt index = iFullEntry->DeleteAttachmentL(aAttachment.Impl());
	if(iCalAttachmentList && index >= 0 && index < iCalAttachmentList->Count())
		{
		delete (*iCalAttachmentList)[index];
		iCalAttachmentList->Remove(index);
		__ASSERT_DEBUG(iCalAttachmentList->Count() == iFullEntry->AttachmentCount(), Panic(EAgmErrAttachmentCountCorrupt)); 
		}
	}
	
CCalAttachment* CCalEntryImpl::AttachmentL(TInt aIndex)
	{
	PopulateAttachmentListL();

	if (aIndex < 0 || aIndex >= iCalAttachmentList->Count() )
		{
		return NULL;
		}
		
	return (*iCalAttachmentList)[aIndex];
	}

TInt CCalEntryImpl::AttachmentCountL()
	{
	LoadFullEntryL();
	return iFullEntry->AttachmentCount();
	}
	
CAgnSimpleEntry* CCalEntryImpl::SimpleEntry()
	{
	if (!iFullEntry)
		{
		return &iLiteEntry->LiteEntry();
		}
	return iFullEntry;
	}

const CAgnSimpleEntry* CCalEntryImpl::SimpleEntry() const
	{
	if (!iFullEntry)
		{
		return &iLiteEntry->LiteEntry();
		}
	return iFullEntry;
	}

void CCalEntryImpl::SetUserIntL( TUint32 aUserInt )
	{
	SimpleEntry()->SetUserInt(aUserInt);
	}

TUint32 CCalEntryImpl::UserIntL()
	{
	return SimpleEntry()->UserInt();
	}


//  CCalEntryIdImpl //

CCalEntryIdImpl* CCalEntryIdImpl::NewL(TDesC8* aFlatData)
	{
	CCalEntryIdImpl* self = new(ELeave) CCalEntryIdImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aFlatData);
	CleanupStack::Pop(self);
	return self;
	}

CCalEntryIdImpl::CCalEntryIdImpl()
	{
	}

CCalEntryIdImpl::~CCalEntryIdImpl()
	{
	delete iFlatData;
	}

void CCalEntryIdImpl::ConstructL(TDesC8* aFlatData)
	{
	__ASSERT_ALWAYS(aFlatData != NULL, User::Leave(KErrArgument));
	
	iFlatData = aFlatData;
	iUniqueId = reinterpret_cast<const TAgnAlarmInfo*>(aFlatData->Ptr());
	}

TPtrC8 CCalEntryIdImpl::IdL()
	{
	return iUniqueId->iGlobalIdentifier;
	}

TPtrC CCalEntryIdImpl::StoreFileNameL()
	{
	return iUniqueId->iFileName;
	}

TCalTime CCalEntryIdImpl::RecurrenceIdL()
	{
	TCalTime time;
	
	if(iUniqueId->iTimeMode ==MAgnCalendarTimeMode::EFloating)
 		{
 		time.SetTimeLocalFloatingL(iUniqueId->iRecurrenceId);
 		}
 	else
 		{
 		time.SetTimeUtcL(iUniqueId->iRecurrenceId);
 		}
 	
 	return time;
	}

TCalTime CCalEntryIdImpl::InstanceTimeL()
	{
	TCalTime time;

	if(iUniqueId->iTimeMode == MAgnCalendarTimeMode::EFloating)
		{
		time.SetTimeLocalL(iUniqueId->iInstanceTime);
		}
	else
		{
		time.SetTimeUtcL(iUniqueId->iInstanceTime);		
		}
		
	return time;
	}

TCalLocalUid CCalEntryIdImpl::LocalUidL()
	{
	return iUniqueId->iAgnUniqueId;
	}
	

/** Construct a new CCalLiteEntry 

@param aLiteEntry The TAgnLiteEntry to be shared between CCalEntry objects.
@param aSession A reference to the CCalSession
@internalComponent
*/
CCalLiteEntry* CCalLiteEntry::NewL(CAgnSimpleEntry& aSimpleEntry, RAgendaServ& aAgendaServ)
	{
	CCalLiteEntry* self = new(ELeave) CCalLiteEntry(aSimpleEntry, aAgendaServ);
	return self;
	}

/** Constructor

@param aLiteEntry The TAgnLiteEntry to be shared between CCalEntry objects.
@param aSession A reference to the CCalSession
@internalComponent
*/	
CCalLiteEntry::CCalLiteEntry(CAgnSimpleEntry& aLiteEntry, RAgendaServ& aAgendaServ)
	: iAgendaServ(aAgendaServ), iLiteEntry(aLiteEntry)
	{
	iAgendaServ.IncreaseRef();
	}

/** Deletes the TAgnLiteEntry that is contained 

@internalComponent
*/	
CCalLiteEntry::~CCalLiteEntry()
	{
	iAgendaServ.DeleteSimpleEntry(&iLiteEntry);
	iAgendaServ.DecreaseRef();
	}
	
/** Get a reference to the TAgnLiteEntry

@return A reference to the TAgnLiteEntry
@internalComponent
*/
CAgnSimpleEntry& CCalLiteEntry::LiteEntry() const
	{
	return iLiteEntry;
	}

/** Increments the reference count

This should only be called by a CCalEntry object that is being created from a CCalLiteEntry 
@internalComponent
*/	
void CCalLiteEntry::IncrementRefCount()
	{
	++iRefCount;
	}

/** Decrements the reference count and deletes this CCalLiteEntry if there are no more entries refering to it

This should only be called by a CCalEntry object
@internalComponent
*/	
void CCalLiteEntry::DecrementRefCount()
	{
	__ASSERT_ALWAYS(iRefCount > 0, User::Invariant());
	if (--iRefCount == 0)
		{
		delete this;
		}
	}
