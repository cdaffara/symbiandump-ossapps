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

#include "agmentry.h"

#include "agmattendee.h"
#include "agmcategory.h"
#include <calinterimapipanic.h>
#include "agmcontent.h"
#include "agmattachment.h"
#include "agmdate.h"
#include "agmrptdef.h"
#include "agmutil.h"

#include <calentry.h>
//#include "calentryimpl.h"
#include <e32math.h>

#include <asshddefs.h>


//---------------------------------- CAgnEntry ------------------------------------------

EXPORT_C CAgnEntry* CAgnEntry::NewL(CCalEntry::TType aType)
	{
	__ASSERT_ALWAYS(aType>=CCalEntry::EAppt && aType<=CCalEntry::ENote, Panic(EAgmErrBadTypeEntry));
	// allocate a CAgnEntry object, which invokes the CAgnSimpleEntry new operator, but passing no allocator
	// this makes the CAgnSimpleEntry new operator use the default new operator
	
	return new (ELeave, NULL) CAgnEntry(aType);;
	}

EXPORT_C CAgnEntry* CAgnEntry::NewL(RReadStream& aStream)
//Creates an entry and restores it from aStream
	{
	// type must be read first because NewL requires it
	TUint8 typeData = aStream.ReadUint8L();
		
	CCalEntry::TType type=CCalEntry::TType(typeData);
	
	__ASSERT_ALWAYS(type==CCalEntry::EAppt || type==CCalEntry::EEvent ||
					type==CCalEntry::EAnniv || type==CCalEntry::ETodo ||
					type==CCalEntry::EReminder || type == CCalEntry::ENote,
					User::Leave(KErrCorrupt));

	CAgnEntry* entry = CAgnEntry::NewL(type);
	CleanupStack::PushL(entry);
	entry->InternalizeL(aStream);
	CleanupStack::Pop();
	return entry;
	}
		

CAgnEntry::CAgnEntry(CCalEntry::TType aType) : CAgnSimpleEntry(aType)
	{
	iSummary = NULL;
	iSummaryStreamId = KNullStreamId;
	iDescription = NULL;
	iDescriptionStreamId = KNullStreamId;
	SetFlagOff(EAgnAlarmActionDeleted);
	iDTStampUtc = Time::NullTTime();
	iLatitude=KGEODefaultValue;
	iLongitude=KGEODefaultValue;
	}

void CAgnEntry::ConstructL(HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum)
	{
	iGsData = CGsParentData::NewL(aUid, aSeqNum, aMethod); // takes ownership of aUid
	}

void CAgnEntry::ConstructL(HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum, const TAgnCalendarTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange)
	{
	iGsData = CGsChildData::NewL(aUid, aSeqNum, aMethod, aRecurrenceId, aRange); // takes ownership of aUid
	}

EXPORT_C CAgnEntry* CAgnEntry::NewL(CCalEntry::TType aType, HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum, const TAgnCalendarTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange)
	{
	CAgnEntry* entry = new (ELeave, NULL) CAgnEntry(aType);
	
	CleanupStack::PushL(entry);
	entry->ConstructL(aUid, aMethod, aSeqNum, aRecurrenceId, aRange);
	CleanupStack::Pop(entry);
	
	return entry;
	}

EXPORT_C CAgnEntry* CAgnEntry::NewL(CCalEntry::TType aType, HBufC8* aUid, CCalEntry::TMethod aMethod, TUint aSeqNum)
	{
	CAgnEntry* entry = new (ELeave, NULL) CAgnEntry(aType);
	
	CleanupStack::PushL(entry);
	entry->ConstructL(aUid, aMethod, aSeqNum);
	CleanupStack::Pop(entry);
	
	return entry;
	}
	
EXPORT_C CAgnEntry::~CAgnEntry()
/** The destructor frees all resources owned by the entry, prior to its destruction. 
@internalComponent
*/
	{
	if ( iAttendeeList )
		{
		iAttendeeList->ResetAndDestroy();
		delete iAttendeeList;
		}
	delete iMeetingOrganizer;
	
	delete iLocation;
	
	if ( iCategoryList )
		{
		iCategoryList->ResetAndDestroy();
		delete iCategoryList;
		}		
	
	if (iAttachments)
		{
		iAttachments->ResetAndDestroy();
		delete iAttachments;		
		}

	delete iDescription;
	delete iSummary;
	delete iAlarmSoundName;
	delete iAlarmAction;	
	
    delete iGsData;
	}


EXPORT_C const TDesC& CAgnEntry::Summary() const
/** Gets a pointer to the rich text object owned by the entry.

@internalComponent
@return Pointer to the rich text object. The caller does not take ownership. */
	{
	if ( iSummary )
		{
		return *iSummary;
		}
		
	return KNullDesC;
	}


EXPORT_C TBool CAgnEntry::CompareL(const CAgnEntry& aEntry) const
/** Compares another entry for equality with this one.

The entries' instance IDs and unique IDs are also compared.

@internalComponent
@param aEntry Pointer to an entry.
@param aCompareId This argument is used internally, for test purposes only. 
As it has a default value, it can be ignored by developers.
@return ETrue if aEntry is the same as this entry, EFalse if not. Note. Replication 
data is not included in the comparison, except for the replication status. 
*/
	{
	if ( ! (*this == aEntry) )
		{
		return EFalse;
		}

	if ( ReplicationStatus() != aEntry.ReplicationStatus() )
		{
		return EFalse;
		}

	if ( HasAlarm() )
		{
		if ( AlarmSoundName() != aEntry.AlarmSoundName() )
			{
			return EFalse;
			}
			
		CAgnContent* content1 = AlarmAction();
		CAgnContent* content2 = aEntry.AlarmAction();
		if ( content1 == NULL )
			{
			if ( content2 != NULL )
				{
				return EFalse;
				}
			}
		else
			{
			if ( content2 == NULL )
				{
				return EFalse;
				}
				
			if ( content1->Type() != content2->Type() )
				{
				return EFalse;
				}
				
			if ( content1->Content() != content2->Content() )
				{
				return EFalse;
				}
				
			if ( content1->MimeType() != content2->MimeType() )
				{
				return EFalse;
				}
			}
		}
	
	if ( BusyStatus() != aEntry.BusyStatus() )
		{
		return EFalse;
		}
	
	// Compare GEO values
	TReal entryLatitude;
	TReal entryLongitude;
	
	aEntry.GeoValue(entryLatitude,entryLongitude);
	
	if ( iLatitude != entryLatitude || entryLongitude != iLongitude )
		{
		return EFalse;
		}	
		
	// Compare the user integer.
	if (UserInt() != aEntry.UserInt())
		{
		return EFalse;
		}	
	
	if ( DTStampUtcL() != aEntry.DTStampUtcL() )
		{
		return EFalse;
		}

	if ( Method() != aEntry.Method() )
		{
		return EFalse;
		}

	if ( SequenceNumber() != aEntry.SequenceNumber() )
		{
		return EFalse;
		}
	
	if ( RecurrenceId() != aEntry.RecurrenceId() )
		{
		return EFalse;
		}
	
	const TInt KNumOfCategories = CategoryCount();
	
	if ( aEntry.CategoryCount() != KNumOfCategories )
		{
		return EFalse;
		}
	
	const RPointerArray<CAgnCategory>* categoryList1 = iCategoryList;
	const RPointerArray<CAgnCategory>* categoryList2 = aEntry.iCategoryList;

	if (categoryList1 && categoryList2)
		{
		TBool categoryFound = EFalse;

		for ( TInt i = 0 ; i < KNumOfCategories; ++i )
			{
			categoryFound = EFalse;

			for ( TInt j = 0; ! categoryFound && j < KNumOfCategories; ++j )
				{
				if ( (*categoryList1)[i]->Category() == (*categoryList2)[j]->Category() )
					{
					if ( (*categoryList1)[i]->Category() == CCalCategory::ECalExtended )
						{
						if ( (*categoryList1)[i]->ExtendedCategoryName() == (*categoryList2)[j]->ExtendedCategoryName() )
							{
							categoryFound = ETrue;
							}
						}
					else
						{
						categoryFound = ETrue;
						}
					}
				}

			if ( !categoryFound )
				{
				return EFalse;
				}
			}
		}

	const TInt KNumOfAttendees = AttendeeCount();

	if ( KNumOfAttendees != aEntry.AttendeeCount() )
		{
		return EFalse;
		}

	if ( KNumOfAttendees > 0 && KNumOfAttendees == aEntry.AttendeeCount() )
		{
		const RPointerArray<CAgnAttendee>& KAttendeeList1 = *iAttendeeList;
		const RPointerArray<CAgnAttendee>& KAttendeeList2 = *aEntry.iAttendeeList;

		for ( TInt i = 0; i < KNumOfAttendees; ++i )
			{
			const TPtrC& KAttendee1 = KAttendeeList1[i]->Address();

			TBool attendeeFound = EFalse;
			
			for ( TInt j = 0; !attendeeFound && j < KNumOfAttendees; ++j )
				{
				const TPtrC& KAttendee2 = KAttendeeList2[j]->Address();
				
				if ( KAttendee1.CompareF(KAttendee2) == 0 )
					{
					attendeeFound = ETrue;
					}
				}
				
			if ( !attendeeFound )
				{
				return EFalse;
				}
			}
		}

	const TInt KNumAttachments = AttachmentCount();
	if (KNumAttachments != aEntry.AttachmentCount())
		{
		return EFalse;
		}
	if (KNumAttachments > 0)
		{
		RArray<TBool> foundAttachmentAlready;
		CleanupClosePushL(foundAttachmentAlready);
		
		for (TInt i = 0; i < KNumAttachments; ++i)
			{
			foundAttachmentAlready.AppendL(EFalse);
			}
		
		for (TInt i = 0; i < KNumAttachments; ++i)
			{
			TBool attachmentFound = EFalse;
			for (TInt j = 0; j < KNumAttachments; ++j)
				{
				if (!foundAttachmentAlready[j] && Attachment(i).CompareL(aEntry.Attachment(j)))
					{
					foundAttachmentAlready[j] = ETrue;
					attachmentFound = ETrue;
					break;
					}
				}
			if (!attachmentFound)
				{
				CleanupStack::PopAndDestroy(); // foundAttachmentAlready.Close()
				return EFalse;
				}
			}
		
		CleanupStack::PopAndDestroy(); // foundAttachmentAlready.Close()
		}
	
	if ( PhoneOwner() == NULL )
		{
		if ( aEntry.PhoneOwner() != NULL )
			{
			return EFalse;
			}
		}
	else
		{
		// PhoneOwner() != NULL
		if ( aEntry.PhoneOwner() == NULL )
			{
			return EFalse;
			}
		else if ( PhoneOwner()->Address() != aEntry.PhoneOwner()->Address() )
			{
			return EFalse;
			}
		}

	if ( Summary() != aEntry.Summary() )
		{
		return EFalse;
		}

	if ( Description() != aEntry.Description() )
		{
		return EFalse;
		}

	if ( Location() != aEntry.Location() )
		{
		return EFalse;
		}
	
	return ETrue;
	}


EXPORT_C const TDesC& CAgnEntry::AlarmSoundName() const
/** Gets the name of the alarm sound as set by SetAlarmSoundName(). 

Returns NULL if the entry isn't alarmed.

@internalComponent
@return The name of the alarm sound. 
*/
	{

	if ( HasAlarm() && iAlarmSoundName )
		{
		return *iAlarmSoundName;
		}		
	
	return KNullDesC();
	}


EXPORT_C void CAgnEntry::SetAlarmSoundNameL(const TDesC& aAlarmSoundName)
/** Sets the name of the alarm sound.

Note: if the length of the new alarm sound name is greater than KMaxAlarmSoundName 
characters, it will be ignored.

@internalComponent
@param aAlarmSoundName The name of the alarm sound. 
*/
	{
	const TInt KLen = aAlarmSoundName.Length();
	
	if ( KLen > 0 && KLen <= KMaxAlarmSoundNameLength )
		{
		// delete and set always to NULL before AllocL because
		// that call could leave and the pointer be not NULL
				
		delete iAlarmSoundName;
		iAlarmSoundName = NULL;
			
		iAlarmSoundName = aAlarmSoundName.AllocL();
		}
	}


EXPORT_C void CAgnEntry::ClearAlarm()
/** Clear associated alarm

@internalComponent
*/
	{
	delete iAlarmSoundName;
	iAlarmSoundName = NULL;
		
	if ( iAlarmActionStreamId != KNullStreamId)
		{
		SetFlagOn(EAgnAlarmActionDeleted);
		}
	
    delete iAlarmAction;
    iAlarmAction = NULL;
		
	SetAlarmOffset(KAlarmNotSet);
	}

 void CAgnEntry::ExternalizeGsDataL(RWriteStream& aStream, TBool aToBuffer) const
	{
	aStream.WriteInt8L(GsDataType());
	
	if(aToBuffer)
		{
		iGsData->ExternalizeToBufferL(aStream);		
		}
	else
		{
		iGsData->ExternalizeL(aStream);	
		}
	}
 
EXPORT_C void CAgnEntry::PruneRDatesL(const TAgnCalendarTime& aRecurrenceId, CalCommon::TRecurrenceRange aRange)
/**
Used when adding a repeating child to a parent entry.
Removes all sporadic dates either after or before the specified recurrence ID. 
@internalComponent
*/
	{
	__ASSERT_DEBUG(iGsData->GsDataType() == CGsData::EParent, Panic(EAgmErrNotParentEntry));
	__ASSERT_DEBUG(RptDef(), Panic(EAgmErrNotParentEntry));
	__ASSERT_DEBUG(aRange == CalCommon::EThisAndFuture || aRange == CalCommon::EThisAndPrior, Panic(EAgmErrInvalidRecurrenceRange));
	
	if (RptDef()->HasSporadicDates())
		{
		const RArray<TAgnCalendarTime>* KSporadicDates = RptDef()->SporadicDateList();
		
		for (TInt i = KSporadicDates->Count() - 1; i >= 0; --i)
			{
			const TAgnCalendarTime& KSporadicDate = (*KSporadicDates)[i];
			if ( (aRecurrenceId > KSporadicDate && aRange == CalCommon::EThisAndPrior ) ||
				 (aRecurrenceId < KSporadicDate && aRange == CalCommon::EThisAndFuture ) )
				{
				RptDef()->RemoveSporadicDate(KSporadicDate);
				}
			}
		}
	}

void CAgnEntry::InternalizeGsDataL(RReadStream& aStream, TBool aFromBuffer)
 	{
 	CGsData::TGsDataType dataType =  static_cast<CGsData::TGsDataType>(aStream.ReadInt8L());
	
	if(dataType == CGsData::EParent)
		{
		iGsData = CGsParentData::NewL();
		}
	else 
		{
		iGsData = CGsChildData::NewL();
		}
		
	if(aFromBuffer)
		{
		iGsData->InternalizeFromBufferL(aStream);
		}
	else
		{
		iGsData->InternalizeL(aStream);
		}
  	}
	
EXPORT_C void CAgnEntry::ExternalizeL(RWriteStream& aStream) const
/** Externalises an entry's details and attributes, but not its rich text to a 
write stream. 

The presence of this function means that the standard templated operator<<() 
(defined in s32strm.h) is available to externalise objects of this class.

@internalComponent
@param aStream Stream to which the object should be externalised. 
*/
	{
	ExternalizeEntryL(aStream);
	ExternalizeGsDataL(aStream, EFalse);
	}

void CAgnEntry::ExternalizeEntryL(RWriteStream& aStream, TBool aToBuffer) const
	{
	CAgnSimpleEntry::doExternalizeL(aStream, aToBuffer);
	aStream.WriteInt8L(iReplicationStatus);
	
	if ( HasAlarm() )
		{
		if (iAlarmSoundName)
			{
			aStream << *iAlarmSoundName;
			}			
		else
			{
			aStream << KNullDesC();
			}		
		}

	// Make sure there is no server pointer
	// - This may have been set up from a previous FetchEntry
	__ASSERT_DEBUG(iSummaryStreamId == KNullStreamId ||
			    !(iSummary && iSummary->Length() > 0 && iSummaryStreamId == KNullStreamId),
				 Panic(EAgmNoNotesStreamId));

	aStream << iSummaryStreamId;

	TInt size = 0;
	if ( iLocation )
		{
		size = iLocation->Length();
		aStream.WriteUint32L(size);
		aStream.WriteL(iLocation->Des(), size);
		}
	else
		{
		size = 0;
		aStream.WriteUint32L(size);
		}

	TInt phoneOwnerIndex = 0;
	
	if ( iAttendeeList )
		{
		size = iAttendeeList->Count();
		aStream.WriteUint32L(size);
		
		for ( TInt i = 0; i < size; ++i )
			{
			CAgnAttendee*& attendeeI = (*iAttendeeList)[i];			
			
			attendeeI->ExternalizeL(aStream);
			
			if ( iPhoneOwner == attendeeI )
				{
				phoneOwnerIndex = i + 1;
				}
			}
		}
	else
		{		
		size = 0;
		aStream.WriteUint32L(size);
		}

	//	phone owner index. 0 implies no phoneowner
	aStream.WriteUint32L(phoneOwnerIndex);

	//	meeting organizer if present
	if (iMeetingOrganizer)
		{
		aStream.WriteUint8L(ETrue);
		iMeetingOrganizer->ExternalizeL(aStream);
		}
	else
		{
		aStream.WriteUint8L(EFalse);
		}
		
	// Write the creation date field

	aStream << iDTStampUtc.Int64();

	// this does not change the file format
	if ( iCategoryList )
		{
		size = iCategoryList->Count();
		aStream.WriteUint32L(size);
		
		for ( TInt ccount = 0; ccount < size; ++ccount )
			{
			(*iCategoryList)[ccount]->ExternalizeL(aStream);
			}
		}
	else
		{		
		size = 0;
		aStream.WriteUint32L(size);
		}
	
	// Write the Notes Stream Id - Null value indicates no Notes data.
	// If the Notes exist, they should have been saved previously, so Stream id != 0 !!!
	__ASSERT_DEBUG((iDescriptionStreamId == KNullStreamId) ||
				    !(iDescription && iDescription->Length() > 0 && iDescriptionStreamId == KNullStreamId),
					 Panic(EAgmNoNotesStreamId));
	aStream << iDescriptionStreamId;
	aStream << iAlarmActionStreamId;

	aStream.WriteUint8L(iBusyStatus);
	
	// GEO properties
	aStream.WriteReal64L(iLatitude);
	aStream.WriteReal64L(iLongitude);
	
	const TInt KAttachmentCount = AttachmentCount();
	aStream.WriteUint16L(KAttachmentCount);
	for (TInt i = 0; i < KAttachmentCount; ++i)
		{
		if ( ! Attachment(i).FlagsSet(CAgnAttachment::EDataHasBeenSet))
			{
			// attachments with just content IDs should not be stored
			User::Leave(KErrArgument);
			}
		else
			{
			aStream << Attachment(i);
			}
		}

	// Set the user integer of the stream.
	aStream.WriteInt32L( UserInt() );
	
	// future DC proofing
	aStream.WriteUint32L(0); // number of bytes until end of entry
	}


EXPORT_C void CAgnEntry::InternalizeL(RReadStream& aStream)
	{
	InternalizeEntryL(aStream, EFalse);
	InternalizeGsDataL(aStream, EFalse);
	}

void CAgnEntry::InternalizeEntryL(RReadStream& aStream, TBool aFromBuffer)
/** Internalises an entry's details and attributes, but not its rich text from 
a read stream. 

The presence of this function means that the standard templated operator>>() 
(defined in s32strm.h) is available to internalise objects of this class.

@internalComponent
@param aStream Stream from which the object should be internalised. 
*/
	{
	CAgnSimpleEntry::doInternalizeL(aStream, aFromBuffer);
	iReplicationStatus = static_cast<CCalEntry::TReplicationStatus>(aStream.ReadInt8L());
	
	delete iAlarmSoundName;
	iAlarmSoundName = NULL;
	
	if ( HasAlarm() )
		{
		iAlarmSoundName = HBufC::NewL(aStream, KMaxAlarmSoundNameLength);
		if ( iAlarmSoundName->Length() == 0 )
			{
			delete iAlarmSoundName;
			iAlarmSoundName = NULL;
			}
		}

	aStream >> iSummaryStreamId;

	// Delete any location that may exist
	delete iLocation;
	iLocation = NULL;

	TInt size = aStream.ReadUint32L();
	if ( size )
		{
		iLocation = HBufC::NewL(size);
		TPtr bufPtr = iLocation->Des();
		aStream.ReadL(bufPtr, size);
		}

	size = aStream.ReadUint32L();
	if ( iAttendeeList )
		{
		iAttendeeList->ResetAndDestroy();
		}
	else
		{
		if ( size )
			{
			CreateAttendeeListL();
			}
		}
	iPhoneOwner = NULL;
	iMeetingOrganizer = NULL;
	
	for ( TInt count = 0; count < size; ++count )
		{
		CAgnAttendee* attendee = CAgnAttendee::NewL(aStream);
		CleanupStack::PushL(attendee);
		iAttendeeList->AppendL(attendee);
		CleanupStack::Pop(attendee);
		}
		
	// populate the phoneowner/organizer fields if existing
	TInt phoneOwnerIndex = aStream.ReadUint32L();
	if (phoneOwnerIndex > 0)
		{
		iPhoneOwner = (*iAttendeeList)[phoneOwnerIndex-1];
		}

	//	meeting organizer if present
	if (aStream.ReadUint8L())
		{
		iMeetingOrganizer = CAgnAttendee::NewL(aStream);
		}
	
	TInt64 dtStamp;
	aStream >> dtStamp;
	iDTStampUtc = dtStamp;

	size = aStream.ReadUint32L();
	
	if ( iCategoryList )
		{
		iCategoryList->ResetAndDestroy();
		}
	else
		{
		if ( size )
			{
			iCategoryList = new (ELeave) RPointerArray<CAgnCategory>;
			}
		}
		
	for ( TInt ccount = 0; ccount < size; ++ccount )
		{
		CAgnCategory* category = CAgnCategory::NewL(aStream);
		CleanupStack::PushL(category);
		iCategoryList->AppendL(category);
		CleanupStack::Pop(category);
		}
		
	// If it's the new file format read in the Stream Id for the notes text
	aStream >> iDescriptionStreamId;

	aStream >> iAlarmActionStreamId;
		
	iBusyStatus = static_cast<CCalEntry::TTransp>(aStream.ReadUint8L());
	
	// GEO properties	
	TReal geoLatitude = aStream.ReadReal64L();
	TReal geoLongitude = aStream.ReadReal64L();
	
	SetGeoValueL(geoLatitude, geoLongitude);
	
	TInt attachmentCount = aStream.ReadUint16L();
	for (TInt i = 0; i < attachmentCount; ++i)
		{
		
		CAgnAttachment* attachment = AttachmentFactory::NewAttachmentL(aStream);
		CleanupStack::PushL(attachment);
		AddAttachmentL(attachment);
		CleanupStack::Pop(attachment);
		}
	
	// Set the user integer of this entry from the stream.
	SetUserInt(aStream.ReadInt32L());
	
	// future DC proofing
	size = aStream.ReadUint32L(); // number of bytes until end of entry
	while (size > 0)
		{
		aStream.ReadUint8L(); // ignore data
		size--;
		}
	}

EXPORT_C CAgnEntry* CAgnEntry::CloneL() const
	{
	CAgnEntry* entry = NULL;
	HBufC8* guid = (this->Guid()).AllocLC();
	if(this->GsDataType() == CGsData::EParent)
		{
		entry = CAgnEntry::NewL(this->Type(), guid, this->Method(), this->SequenceNumber());
		}
	else
		{
		entry = CAgnEntry::NewL(this->Type(), guid, this->Method(), this->SequenceNumber(), this->RecurrenceId(), this->RecurrenceRange());
		}
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entry->CopyFromL(*this, CCalEntry::ECopyAll);
	CleanupStack::Pop(entry);
	return entry;
	}
	
EXPORT_C void CAgnEntry::CopyFromL(const CAgnEntry& aSource, CCalEntry::TCopyType aCopyType)
/** Copy the details common to all entry types from aSource.
If aCopyType is EShallowCopy it indicates that if aSources rich text is stored out of line and its
not loaded then it isn't loaded and copied
*/
	{
	CopySimpleEntryL(aSource, aCopyType);

	// NOTE: don't use the internal set methods with pointers because doing a CopyFrom
	// we are supposed to carbon copy the original. If the original has a NULL
	// pointer in a member, we have to do the same and not keep any old stuff
	iReplicationStatus = aSource.iReplicationStatus;	

	if( iAttendeeList )
		{
		iAttendeeList->ResetAndDestroy();
		}
	iPhoneOwner = NULL;
	delete iMeetingOrganizer;
	iMeetingOrganizer = NULL;

	const TInt KAttendees = aSource.AttendeeCount();
	for ( TInt count = 0; count < KAttendees; ++count )
		{
		CAgnAttendee& attendee = aSource.FetchAttendee(count);

		CAgnAttendee* newAttendee = attendee.CloneL();
		AddAttendeeL(newAttendee);
		
		if ( aSource.PhoneOwner() == &attendee )
			{
			SetPhoneOwnerL(newAttendee);
			}
		}
	
	if (aSource.Organizer())
		{
		iMeetingOrganizer = aSource.Organizer()->CloneL();
		}

	// Copy the Location
	delete iLocation;
	iLocation = NULL;

	if ( aSource.iLocation )
		{
		iLocation = aSource.iLocation->AllocL();
		}

	iDTStampUtc = aSource.iDTStampUtc;
	
	if( iCategoryList )
		{
		iCategoryList->ResetAndDestroy();
		}
		
	const TInt KCategories = aSource.CategoryCount();
	for ( TInt ccount = 0; ccount < KCategories; ++ccount )
		{
		CAgnCategory& category = aSource.FetchCategory(ccount);

		CAgnCategory* newCategory = CAgnCategory::NewL(category);
		CleanupStack::PushL(newCategory);
		AddCategoryL(newCategory);
		CleanupStack::Pop(newCategory);
		}
		
	iBusyStatus = aSource.BusyStatus();
	
	// GEO properties
	TReal sourceLatitude;
	TReal sourceLongitude;
	aSource.GeoValue(sourceLatitude,sourceLongitude);
	
	iLatitude = sourceLatitude;
	iLongitude = sourceLongitude;

	// Copy the Notes buffer
	
	delete iDescription;
	iDescription = NULL;
	
	if ( aSource.iDescription )
		{
		iDescription = aSource.iDescription->AllocL();
		}
		
	iDescriptionStreamId = aSource.iDescriptionStreamId;


	// Copy the Summary buffer

	delete iSummary;
	iSummary = NULL;

	if ( aSource.iSummary )
		{
		iSummary = aSource.iSummary->AllocL();
		}
		
	iSummaryStreamId = aSource.iSummaryStreamId;


	// Copy the Alarm sound name

	delete iAlarmSoundName;
	iAlarmSoundName = NULL;

	if ( aSource.iAlarmSoundName )
		{
		iAlarmSoundName = aSource.iAlarmSoundName->AllocL();
		}


	// Copy the Alarm action

	delete iAlarmAction;
	iAlarmAction = NULL;
		
	if ( aSource.iAlarmAction )
		{
		iAlarmAction = aSource.iAlarmAction->CloneL();			
		}

	iAlarmActionStreamId = aSource.iAlarmActionStreamId;
	iFlags = aSource.iFlags;
	
	if ( iAttachments )
		{
		iAttachments->ResetAndDestroy();
		delete iAttachments;
		iAttachments = NULL;
		}
		
	const TInt KAttachments = aSource.AttachmentCount();
	for ( TInt i = 0; i < KAttachments; ++i )
		{
		CAgnAttachment& attachment = aSource.Attachment(i);
		CAgnAttachment* attachmentCopy = AttachmentFactory::CloneL(attachment);
		CleanupStack::PushL(attachmentCopy);
		AddAttachmentL(attachmentCopy);
		CleanupStack::Pop(attachmentCopy);
		}

	iGsData->CopyDataL(*aSource.iGsData);
	
	if(aCopyType == CCalEntry::ECopyAll)
		{
		HBufC8* guid = aSource.Guid().AllocL();
		SetGuid(guid);
		}		
	}
	
EXPORT_C CCalEntry::TReplicationStatus CAgnEntry::ReplicationStatus() const
/** Get ReplicationStatus

@internalComponent
*/
	{
	return static_cast<CCalEntry::TReplicationStatus>(iReplicationStatus);
	}


EXPORT_C void CAgnEntry::SetReplicationStatusL(CCalEntry::TReplicationStatus aReplicationStatus)
/** Set ReplicationStatus

@internalComponent
@leave KErrArgument if passed parameter exceeds boundaries
*/
	{
	if( aReplicationStatus < CCalEntry::EOpen || aReplicationStatus > CCalEntry::ERestricted )
		{
		User::Leave(KErrArgument);
		}
	
	iReplicationStatus = aReplicationStatus;
	}


EXPORT_C TBool CAgnEntry::SummaryIsLoaded() const
/** Check if summary text is loaded from stream

@internalComponent
*/
	{
	if ( ! iSummary && iSummaryStreamId != KNullStreamId )
		{
		return EFalse;
		}		

	return ETrue;
	}


EXPORT_C void CAgnEntry::SetSummary(HBufC* aSummary)
/** Set summary

@internalComponent
*/
	{
	delete iSummary;
	iSummary = aSummary;
	}


EXPORT_C void CAgnEntry::SetLocationL(const TDesC& aLocation)
/** Sets the contents of the entry's location field. 

Sets the entry's 'has extended' property.

@internalComponent
@param aLocation Descriptor containing a location, e.g. for a meeting. 
*/
	{
	delete iLocation;
	iLocation = NULL;
	iLocation = aLocation.AllocL();
	}


EXPORT_C const TDesC& CAgnEntry::Location() const
/** Gets the contents of the entry's location field.

@internalComponent
@return The location field. NULL if the entry has no location field. 
*/
	{
	if ( iLocation )
		{
		return *iLocation;
		}
		
	return KNullDesC();
	}


EXPORT_C TInt CAgnEntry::AttendeeCount() const
/** Gets the number of attendees that have been added to the entry.

@internalComponent
@return Number of attendees added. 
*/
	{
	if ( ! iAttendeeList )
		{
		return 0;
		}
	
	return iAttendeeList->Count();
	}

EXPORT_C CAgnAttendee& CAgnEntry::FetchAttendee(TInt aIndex) const
/** Gets the attendee at the specified index within the entry's list of 
attendees. 

The index specified must be valid (between zero and AttendeeCount() 
- 1), otherwise a panic occurs.

@internalComponent
@param aIndex Index to identify the specified attendee.
@return The attendee details. */
	{
	return *(*iAttendeeList)[aIndex];
	}


EXPORT_C void CAgnEntry::AddAttendeeL(CAgnAttendee* aAttendee)
/** Appends an attendee to the entry's list of attendees.

@internalComponent
@param aAttendee Pointer to the attendee details. 
*/
	{
	CleanupStack::PushL(aAttendee);
	CreateAttendeeListL();
	
	// Add an attendee for this entry
	iAttendeeList->AppendL(aAttendee);
	CleanupStack::Pop(aAttendee);
	}

void CAgnEntry::CreateAttendeeListL()
	{
	if ( !iAttendeeList )
		{
		iAttendeeList = new (ELeave) RPointerArray<CAgnAttendee>;
		}
	}
	
EXPORT_C void CAgnEntry::SetOrganizerL(CAgnAttendee* aAttendee)
/** Set attendee as organiser

@internalComponent
*/
	{
	if (iMeetingOrganizer == iPhoneOwner)
		{
		iPhoneOwner = NULL;
		}
	delete iMeetingOrganizer;
	iMeetingOrganizer = aAttendee;
	}

EXPORT_C CAgnAttendee* CAgnEntry::Organizer() const
/** Get Organizer as attendee object

@internalComponent
*/
	{
	return iMeetingOrganizer;
	}

EXPORT_C void CAgnEntry::DeleteAttendee(TInt aIndex)
/** Deletes the attendee at the index specified from the entry's list of attendees. 

If there is no attendee at the index specified, a panic occurs.

@internalComponent
@param aIndex Index of the attendee to delete. 
*/
	{
	CAgnAttendee* attendee = (*iAttendeeList)[aIndex];
	
	if (iPhoneOwner == attendee)
		{
		iPhoneOwner = NULL;
		}
		
	if (iMeetingOrganizer == attendee)
		{
		iMeetingOrganizer = NULL;
		}
		
	iAttendeeList->Remove(aIndex);
	delete attendee;
	}


EXPORT_C CAgnAttendee* CAgnEntry::PhoneOwner() const
/** Get Phone owner as attendee object

@internalComponent
*/
	{
	return iPhoneOwner;
	}


EXPORT_C void CAgnEntry::SetPhoneOwnerL(CAgnAttendee* aAttendee)
/** Set Phone owner as attendee, this must be an existing attendee

@internalComponent
@leave KErrNotFound if the passed attendee is not matching anyone in the current list
*/
	{
	TInt ownerExists = KErrNotFound;
	
	if (iAttendeeList)
		{
		const TInt KCount = iAttendeeList->Count();
		
		for (TInt i = 0; i < KCount; ++i)
			{		
			if (aAttendee == (*iAttendeeList)[i])
				{
				ownerExists = i;
				}
			}
		}
		
	if (ownerExists == KErrNotFound && aAttendee != iMeetingOrganizer)	
		{
		User::Leave(KErrNotFound);
		}
		
	iPhoneOwner = aAttendee;
	}


EXPORT_C void CAgnEntry::SetDTStampUtcL(const TTime& aDTStampUtc)
/**
@internalComponent
*/
	{
	iDTStampUtc = aDTStampUtc;
	}


EXPORT_C const TTime& CAgnEntry::DTStampUtcL() const
/**
@internalComponent
@return TTime
*/
	{
	return iDTStampUtc;
	}

EXPORT_C TInt CAgnEntry::CategoryCount() const
/** Gets the number of categories owned by the entry.

@internalComponent
@return The number of categories. 
*/
	{
	if ( !iCategoryList )
		{
		return 0;
		}
	
	return iCategoryList->Count();
	}


EXPORT_C void CAgnEntry::AddCategoryL(CAgnCategory* aCategory)
/** Appends a category to the entry's list of categories. 

The entry takes ownership of the category specified.

@internalComponent
@param aCategory The category to be added
@leave KErrAlreadyExists if category already present
*/
	{
	// Add an category for this entry
	if ( !iCategoryList )
		{
		iCategoryList = new (ELeave) RPointerArray<CAgnCategory>;
		}		

	// Need to check that the category isn't already in the list
	if ( ! CategoryExist(*aCategory) )
		{
		iCategoryList->AppendL(aCategory);
		}
	else
		{
		User::Leave(KErrAlreadyExists);
		}
	}

TBool CAgnEntry::CategoryExist(CAgnCategory& aSearchCategory) const
/** Check if category is already present

@internalComponent
*/
	{
	if (!iCategoryList)
		{
		return EFalse;
		}
	
	//return ETrue if aSearchCategory belongs to this object, otherwise return EFalse
	const TInt KNumCatList = iCategoryList->Count();
	
	for ( TInt i = 0; i < KNumCatList; ++i )
		{
		CAgnCategory& category = FetchCategory(i);
		
		CCalCategory::TCalCategoryType categoryType = category.Category();
		
		if ( categoryType == aSearchCategory.Category()) 
			{
			if ( categoryType == CCalCategory::ECalExtended ) // Need to compare names
				{
				if ( category.ExtendedCategoryName().CompareF(aSearchCategory.ExtendedCategoryName()) == 0 )
					{
					return ETrue;
					}
				}
			else
				{
				return ETrue;
				}
			}
		}

	return EFalse;
	}


EXPORT_C CAgnCategory& CAgnEntry::FetchCategory(TInt aIndex) const
/** Gets the indexed category.

@internalComponent
@param aIndex Index into the entry's list of categories. Must be valid 
or a panic occurs.
@return Pointer to the category located at the index specified. 
*/
	{
	__ASSERT_DEBUG(iCategoryList != NULL, Panic(EAgmErrCategoryListNotInitialised));
	__ASSERT_DEBUG(aIndex < iCategoryList->Count(), Panic(EAgmErrCategoryOutOfRange));
	
	return *(*iCategoryList)[aIndex];
	}


EXPORT_C void CAgnEntry::DeleteCategory(TInt aIndex)
/** Deletes the indexed category.

@internalComponent
@param aIndex Index into the entry's list of categories. Must be valid 
or a panic occurs. 
*/
	{
	__ASSERT_DEBUG(iCategoryList != NULL, Panic(EAgmErrCategoryListNotInitialised));
	CAgnCategory* category = (*iCategoryList)[aIndex];
	delete category;
	iCategoryList->Remove(aIndex);
	
	if ( iCategoryList->Count() == 0 )
		{
		iCategoryList->ResetAndDestroy();
		delete iCategoryList;
		iCategoryList=NULL;
		}
	}


EXPORT_C const TDesC& CAgnEntry::Description() const
/** Gets the notes text. 

If no notes text has been set, a empty descriptor (KNullDesC) is returned.

@internalComponent
@capability ReadUserData
@return The notes text. 
*/
	{
	if ( iDescription )
		{
		return *iDescription;
		}
	return KNullDesC;
	}


EXPORT_C TBool CAgnEntry::DescriptionIsLoaded() const
/** Check notes is loaded

@internalComponent
@capability ReadUserData
@return Boolean to indicate if loaded or not
*/
	{
	if ( !iDescription && iDescriptionStreamId != KNullStreamId )
		{
		return EFalse;
		}		

	return ETrue;
	}


EXPORT_C void CAgnEntry::SetDescription(HBufC* aDescription)
/** Sets the notes text. 

This object takes ownership of aNotes.

@internalComponent
@param aNotes Notes text. 
*/
	{
	delete iDescription;
	iDescription = aDescription;
	}


EXPORT_C CAgnEntry::TAgnDataChange CAgnEntry::DescriptionChange() const
/** Check if the notes changed

	1. If iDescription is NULL, The description has never been added  or never been loaded - No Change
	2. If iDescriptionStreamId is not KNullStreamId and iDecription is KNullDesC, The descrioption has been deleted (CAgnEntry::SetDescrioprion() can be used to delete description)
	3. If iDescriptionStreamId is not KNullStreamId and iDecription is not KNullDesC, The descrioption could be updated
	4. If iDescriptionStreamId is KNullStreamId and iDescription is not KNullDesC, The description has been added

@internalComponent
@return TAgnDescriptionChange value indicating the type of change
*/
	{
	TAgnDataChange change = EAgnDataNoChange;
	if ( iDescription )
		{
		if ( iDescriptionStreamId != KNullStreamId )
			{
			if ( *iDescription == KNullDesC )
				{
				change = EAgnDataDeleted;
				}
			else
				{
				change = EAgnDataUpdated;
				}
			}
		else if ( *iDescription != KNullDesC )
			{
			change = EAgnDataAdded;
			}
		}

	return change;
	}


EXPORT_C CAgnEntry::TAgnDataChange CAgnEntry::SummaryChange() const
/** Check if the summary changed

	1. If iSummary is NULL, The Summary has never been added or laoded- No Change
	2. If iSummaryStreamId is not KNullStreamId and iSummary is KNullDesC, The Summary has been deleted (CAgnEntry::SetSummary() can be used to delete description)
	3. If iSummaryStreamId is not KNullStreamId and iSummary is not KNullDesC, The Summary could be updated
	4. If iSummaryStreamId is KNullStreamId and iSummary is not KNullDesC, The Summary has been added

@internalComponent
@return TAgnSummaryChange value indicating the type of change
*/
	{
	TAgnDataChange change = EAgnDataNoChange;
	
	if ( iSummary )
		{
		if ( iSummaryStreamId != KNullStreamId )
			{
			if ( *iSummary == KNullDesC )
				{
				change = EAgnDataDeleted;
				}
			else
				{
				change = EAgnDataUpdated;
				}
			}
		else if ( *iSummary != KNullDesC )
			{
			change = EAgnDataAdded;
			}
		}

	return change;
	}


EXPORT_C CAgnEntry::TAgnDataChange CAgnEntry::AlarmActionChange() const
/** Check if the alarm action changed

	1. If iAlarmActionDeleted is true, alarm has been deleted
	2. If iAlarmActionStreamId is KNullStreamId and iAlarmAction is not NULL, alarm has been added
	3. If iAlarmActionStreamId is not KNullStreamId and iAlarmAction is not KNullDesC, The AlarmAction could be updated

@internalComponent
@return TAgnAlarmActionChange value indicating the type of change
*/
	{
	TAgnDataChange change = EAgnDataNoChange;
	
	if ( IsFlagSet(EAgnAlarmActionDeleted) )
		{
		change = EAgnDataDeleted;
		}
	else if ( iAlarmAction )
		{
		if ( iAlarmActionStreamId == KNullStreamId )
			{
			change = EAgnDataAdded;
			}
		else
			{
			change = EAgnDataUpdated;
			}
		}

	return change;
	}


EXPORT_C const TStreamId& CAgnEntry::DescriptionStreamId() const
/** Gets the ID of the embedded stream store in which the description has been stored.

@internalComponent
@return The ID for the stream store in which the description has been stored.
*/
	{
	return iDescriptionStreamId;
	}


EXPORT_C void CAgnEntry::SetDescriptionStreamId(const TStreamId& aStreamId)
/** Sets the ID of the embedded stream store in which the description has been stored.

@internalComponent
@param The ID for the stream store in which the description has been stored.
*/
	{
	iDescriptionStreamId = aStreamId;
	}


EXPORT_C void CAgnEntry::SetSummaryStreamId(const TStreamId& aStreamId)
/** Sets the ID of the embedded stream store in which the summary has been stored.

@internalComponent
@param The ID for the stream store in which the summary has been stored.
*/
	{
	iSummaryStreamId = aStreamId;
	}

EXPORT_C const TStreamId& CAgnEntry::SummaryStreamId() const
/** Gets the ID of the embedded stream store in which the summary has been stored.

@internalComponent
@return The ID for the stream store in which the summary has been stored.
*/
	{
	return iSummaryStreamId;
	}

/**
@internalComponent
@capability None
@return   TPtrC8
*/	
EXPORT_C const TDesC8& CAgnEntry::Guid() const
	{
	return iGsData->Guid();
	}

EXPORT_C void CAgnEntry::SetMethod(CCalEntry::TMethod aMethod)
	{
	iGsData->SetMethod(aMethod);
	}

EXPORT_C void CAgnEntry::SetGuid(HBufC8* aGuid)
/**
@internalComponent
*/
	{
	iGsData->SetGuid(aGuid);
	}

EXPORT_C CGsData::TGsDataType CAgnEntry::GsDataType() const
/**
@internalComponent
*/
	{
	return iGsData->GsDataType();
	}

EXPORT_C CCalEntry::TMethod CAgnEntry::Method() const
/**
@internalComponent
*/
	{
	return iGsData->Method();
	}

EXPORT_C TInt CAgnEntry::SequenceNumber() const
/**
@internalComponent
*/
	{
	return iGsData->SequenceNumber();
	}


EXPORT_C void CAgnEntry::SetSequenceNumber(TInt aSeqNum)
	{
	iGsData->SetSequenceNumber(aSeqNum);
	}	
/**
@internalComponent
@capability None
@return   TTime
*/
EXPORT_C TAgnCalendarTime CAgnEntry::RecurrenceId() const
	{
	return iGsData->RecurrenceId();
	}

/** 
@internalComponent
*/
EXPORT_C CalCommon::TRecurrenceRange CAgnEntry::RecurrenceRange() const
	{
	return iGsData->RecurrenceRange();
	}
		
EXPORT_C TCalLocalUid CAgnEntry::ParentId() const
	{
	__ASSERT_DEBUG(GsDataType() == CGsData::EChild, Panic(EAgmErrNotChildEntry));

	return (static_cast<CGsChildData*>(iGsData)->ParentId());
	}

EXPORT_C void CAgnEntry::SetParentId(TCalLocalUid aParentId)
	{
	__ASSERT_DEBUG(GsDataType() == CGsData::EChild, Panic(EAgmErrNotChildEntry));

	static_cast<CGsChildData*>(iGsData)->SetParentId(aParentId);
	}

EXPORT_C void CAgnEntry::UpdateRecurrenceIdL(const TAgnCalendarTime& aRecId)
	{
	__ASSERT_DEBUG(GsDataType() == CGsData::EChild, Panic(EAgmErrNotChildEntry));
	
	CGsChildData* childData = static_cast<CGsChildData*>(iGsData);
	TTimeIntervalHours hoursChange;
	aRecId.UtcL().HoursFrom(RecurrenceId().UtcL(), hoursChange);
	childData->SetRecurrenceId(aRecId);
	}

EXPORT_C void CAgnEntry::SetRecurrenceRangeL(CalCommon::TRecurrenceRange aRange)
	{
	if (RptDef())
		{
		__ASSERT_DEBUG(aRange == CalCommon::EThisAndFuture || aRange == CalCommon::EThisAndPrior, User::Leave(KErrCorrupt));
		}
	else
		{
		__ASSERT_DEBUG(aRange == CalCommon::EThisOnly, User::Leave(KErrCorrupt));
		}

	CGsChildData* childData = static_cast<CGsChildData*>(iGsData);
	childData->SetRecurrenceRange(aRange);
	}

EXPORT_C TBool CAgnEntry::HasChildren() const
	{
	if (GsDataType() == CGsData::EParent && ChildIds().Count() > 0)
		{
		return ETrue;
		}
	return EFalse;
	}

EXPORT_C const RArray<TGsChildRefData>& CAgnEntry::ChildIds() const
	{
	__ASSERT_DEBUG(GsDataType() == CGsData::EParent, Panic(EAgmErrNotParentEntry));

	return (static_cast<CGsParentData*>(iGsData)->ChildIds());
	}
	
EXPORT_C void CAgnEntry::AddChildIdL(const TGsChildRefData& aChildRefData)
	{
	__ASSERT_DEBUG(GsDataType() == CGsData::EParent, Panic(EAgmErrNotParentEntry));

	static_cast<CGsParentData*>(iGsData)->AddChildIdL(aChildRefData);
	}
	
EXPORT_C void CAgnEntry::RemoveChildId(TCalLocalUid aId)
	{
	__ASSERT_DEBUG(GsDataType() == CGsData::EParent, Panic(EAgmErrNotParentEntry));

	static_cast<CGsParentData*>(iGsData)->RemoveChildId(aId);
	}

EXPORT_C void CAgnEntry::SetRecurrenceIdFromParentL(const CAgnEntry& aParentEntry)
	{//Set the recurrence id and range by its parent data.
	__ASSERT_DEBUG(GsDataType() == CGsData::EChild, Panic(EAgmErrNotChildEntry));
	TAgnCalendarTime recurid = RecurrenceId();
	
	CGsChildData* childGsData = static_cast<CGsChildData*>(iGsData);
	if ( ! recurid.IsSet())
		{
		recurid = aParentEntry.GetChildRecurrenceId(LocalUid());
		childGsData->SetRecurrenceId(recurid);
		}
	}

 TAgnCalendarTime CAgnEntry::GetChildRecurrenceId(TCalLocalUid aChildId) const
	{//Get the recurrence id for one of its children
	const RArray<TGsChildRefData>& KChildIds = ChildIds();

	const TInt KCount((KChildIds.Count()));
	TAgnCalendarTime recurId;
	
	for (TInt i = 0; i < KCount; ++i)
		{
		if (KChildIds[i].ChildId() == aChildId)
			{
			recurId = KChildIds[i].RecurrenceId();
			break;
			}
		}
	return recurId;
	}


EXPORT_C void CAgnEntry::SetAlarmAction(CAgnContent* aAlarmAction)
/** Set an alarm action object to the Agenda entry.
The Agenda Entry takes ownership of the passed object.
@internalComponent
*/
	{
	if(iAlarmAction && !aAlarmAction && iAlarmActionStreamId != KNullStreamId)
		{
		SetFlagOn(EAgnAlarmActionDeleted);
		}

	delete iAlarmAction;

	// Take ownership of the content.
	iAlarmAction = aAlarmAction;
	}


EXPORT_C TBool CAgnEntry::AlarmActionIsLoaded() const
/** Check if Alarm action object is loaded from the stream
@internalComponent
*/
	{
	if ( !iAlarmAction && iAlarmActionStreamId != KNullStreamId && ! IsFlagSet(EAgnAlarmActionDeleted) )
		{
		return EFalse;
		}		

	return ETrue;
	}

	
EXPORT_C CAgnContent* CAgnEntry::AlarmAction() const
/** Get the Alarm action object
@internalComponent
*/
	{
	return iAlarmAction;
	}


EXPORT_C void CAgnEntry::SetAlarmActionStreamId(const TStreamId& aStreamId)
/** Set the Alarm action stream id
@internalComponent
*/
	{
	iAlarmActionStreamId = aStreamId;
	}


EXPORT_C const TStreamId& CAgnEntry::AlarmActionStreamId() const
/** Get the Alarm action stream id
@internalComponent
*/
	{
	return iAlarmActionStreamId;
	}

EXPORT_C void CAgnEntry::AddAttachmentL(CAgnAttachment* aAttachment)
	{
	if (!iAttachments)
		{
		iAttachments = new (ELeave) RPointerArray<CAgnAttachment>;
		}
	if (aAttachment)
		{
		iAttachments->AppendL(aAttachment);	
		}
	}

EXPORT_C TInt CAgnEntry::DeleteAttachmentL(const CAgnAttachment& aAttachment)
	{
	TInt ret = -1;
	if (iAttachments)
		{
		const TInt KAttachmentCount = iAttachments->Count();
		for (TInt i = 0; i < KAttachmentCount; ++i)
			{
			CAgnAttachment* attachToCompare = (*iAttachments)[i];
			if (attachToCompare->CompareL(aAttachment))
				{
				ret = i;
				delete attachToCompare;
				iAttachments->Remove(i);
				break;
				}
			}
		}
	return ret;
	}

EXPORT_C CAgnAttachment& CAgnEntry::Attachment(TInt aIndex) const
	{
	return *(*iAttachments)[aIndex];
	}

EXPORT_C TInt CAgnEntry::AttachmentCount() const
	{
	if (iAttachments)
		{
		return iAttachments->Count();
		}
	return 0;
	}

/** 
Sets the user integer for this entry.

@param aUserInt The new value of the user integer. 
@internalComponent
*/
EXPORT_C void CAgnEntry::SetUserInt( TUint32 aUserInt )
	{
	CAgnSimpleEntry::SetUserInt(aUserInt);
	}

/**
Gets the user integer of this entry.

@return The user integer.
@internalComponent
*/
EXPORT_C TUint32 CAgnEntry::UserInt() const
	{
	return CAgnSimpleEntry::UserInt();
	}

EXPORT_C void CAgnEntry::ExternalizeToBufferL(RWriteStream& aWriteStream) const
/** Used for passing the data to the between client and server
@internalComponent
*/
	{
	ExternalizeEntryL(aWriteStream, ETrue);
	ExternalizeGsDataL(aWriteStream, ETrue);
		
	if ( iDescription )
		{
		aWriteStream.WriteUint8L(ETrue);
		aWriteStream << *iDescription;	
		}
	else
		{
		aWriteStream.WriteUint8L(EFalse);	
		}

	if ( iSummary )
		{
		aWriteStream.WriteUint8L(ETrue);
		aWriteStream << *iSummary;	
		}
	else
		{
		aWriteStream.WriteUint8L(EFalse);	
		}
	
	TBool hasAlarmContent = iAlarmAction?ETrue:EFalse;
	aWriteStream.WriteUint8L(hasAlarmContent);
	if ( hasAlarmContent )
		{
		aWriteStream << *iAlarmAction;
		}

	aWriteStream.WriteUint8L(IsFlagSet(EAgnAlarmActionDeleted));
	}


EXPORT_C void CAgnEntry::InternalizeFromBufferL(RReadStream& aReadStream)
/** Used for passing the data to the between client and server
@internalComponent
*/
	{
	InternalizeEntryL(aReadStream, ETrue);
	InternalizeGsDataL(aReadStream, ETrue);

	delete iDescription;
	iDescription = NULL;
	if(aReadStream.ReadUint8L())//iDescription is not NULL
		{//The following does not allocate KMaxTInt cells on heap
		iDescription = HBufC::NewL(aReadStream, KMaxTInt); 	
		}
	
	delete iSummary;
	iSummary = NULL;
	
	if(aReadStream.ReadUint8L())//iSummary is not NULL
		{//The following does not allocate KMaxTInt cells on heap
		iSummary = HBufC::NewL(aReadStream, KMaxTInt); 	
		}

	delete iAlarmAction;
	iAlarmAction = NULL;
	TBool hasAlarmContent = aReadStream.ReadUint8L();
	if ( hasAlarmContent )
		{
		iAlarmAction = new (ELeave) CAgnContent;
		aReadStream >> *iAlarmAction; 
		}

	if (aReadStream.ReadUint8L())
		{
		SetFlagOn(EAgnAlarmActionDeleted);
		}
	else
		{
		SetFlagOff(EAgnAlarmActionDeleted);
		}
	// Externalize the entry details to the buffer
	// Any rich text components will be stored in the textStream buffer
	}


EXPORT_C CCalEntry::TTransp CAgnEntry::BusyStatus() const
/** Get transparency value
@internalComponent
*/
	{
	return static_cast<CCalEntry::TTransp>(iBusyStatus);
	}


EXPORT_C void CAgnEntry::SetBusyStatusL(CCalEntry::TTransp aBusyStatus)
/** Set transparency value
@internalComponent
@leave KErrArgument if passed parameter is negative
*/
	{
	if ( aBusyStatus < 0 )
		{
		User::Leave( KErrArgument );		
		}
		
	iBusyStatus = aBusyStatus;
	}

EXPORT_C void CAgnEntry::GeoValue(TReal& aLatitude, TReal& aLongitude) const
	{
	aLatitude=iLatitude;
	aLongitude=iLongitude;
	}

/** Set GEO - Latitude and Longitude values for the entry
Latitude is bound between +-0-90 degrees
Longitude is bound between +-0-180 degrees
@internalComponent
@leave KErrArgument if passed parameter is out of bounds.
*/
EXPORT_C void CAgnEntry::SetGeoValueL(const TReal& aLatitude, const TReal& aLongitude)
	{
	// Bounds checking
	__ASSERT_ALWAYS(
	((aLatitude >= KCalGEOMinLatitude && aLatitude <= KCalGEOMaxLatitude) || aLatitude == KGEODefaultValue) 
	&& ((aLongitude >= KCalGEOMinLongitude && aLongitude <= KCalGEOMaxLongitude) || aLongitude == KGEODefaultValue), 
	User::Leave(KErrArgument));
	
	// Truncate to KGEOMaxDecimals decimal places
	TReal truncatedLatitude;
	TReal truncatedLongitude;
	Math::Round(truncatedLatitude,aLatitude,KCalGEOMaxDecimalPlaces);
	Math::Round(truncatedLongitude,aLongitude,KCalGEOMaxDecimalPlaces);
		
	iLatitude = truncatedLatitude;
	iLongitude = truncatedLongitude;
	}

// verify entry is valid before storing it
EXPORT_C void CAgnEntry::VerifyBeforeStoreL()
	{
	// check for entry type note
	if( Type() == CCalEntry::ENote  && !StartTime().IsSet())
		{
		// read the modfication time from simple entry
		TAgnCalendarTime agnStartTime;
		agnStartTime.SetUtcL(CAgnSimpleEntry::LastModifiedDateUtc());

		TAgnCalendarTime agnEndTime = agnStartTime;
		// set the modification time as start and end time
		CAgnSimpleEntry::SetStartAndEndTimeL(agnStartTime, agnEndTime);
		}

	//Check entry time
	if ( Type() != CCalEntry::ETodo )
		{
		__ASSERT_ALWAYS(StartTime().IsSet(), Panic(EAgmErrTimeNotSet));
		if ( Type() != CCalEntry::EReminder )
			{
			__ASSERT_ALWAYS(EndTime().IsSet(), Panic(EAgmErrTimeNotSet));
			}
		}
	//Check repeating rule
	if ( RptDef() )
		{
		// check entry's repeat rule valid
		if ( RptDef()->InvariantL() != KErrNone )
			{
			const TAgnRpt* rptRule = RptDef()->RRule();
			__ASSERT_ALWAYS(rptRule, User::Leave(KErrCorrupt)); // this can never be NULL if Invariant returns an error
			
			// if entry has only one instance, clear the repeat rule
			if ( rptRule->InstanceCountL() == 1 )
				{
				RptDef()->ClearRRule();
				}
			else
				{
				User::Leave(KErrArgument);
				}
			}
			
		else if (RptDef()->Exceptions())
			{
			RptDef()->PruneExceptionsL();
			}

		//check the RDates here
		TInt i = 0;
		while (RptDef()->SporadicDateList() && i < RptDef()->SporadicDateList()->Count())
			{
			const TAgnCalendarTime& KSporadicDate = (*RptDef()->SporadicDateList())[i];
			if (RptDef()->IsARepeatRuleDateInstanceL(KSporadicDate.LocalL()))
				{
				RptDef()->RemoveSporadicDate(KSporadicDate);
				}
			else if (EntryTime() == KSporadicDate)
				{
				// the DTSTART may not be checked in IsARepeatRuleDateInstance, so check it explicitly
				RptDef()->RemoveSporadicDate(KSporadicDate);
				}
			else
				{
				// this sporadic date is ok
				++i;
				}
			}

		const TAgnRpt* rptRule = RptDef()->RRule();
		if(!rptRule && !(RptDef()->HasSporadicDates()))
			{
			ClearRepeat();
			}
		}
	}

void CAgnEntry::SetFlagOn(TFlags aFlag)
	{
	iFlags |= aFlag;
	}

void CAgnEntry::SetFlagOff(TFlags aFlag)
	{
	iFlags &= ~aFlag;
	}

TBool CAgnEntry::IsFlagSet(TFlags aFlag) const
	{
	return (iFlags & aFlag);
	}
