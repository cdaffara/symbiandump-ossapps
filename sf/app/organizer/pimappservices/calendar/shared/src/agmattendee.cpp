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

#include "agmattendee.h"

#include <s32stor.h>

//
// CAgnAttendee

CAgnAttendee::CAgnAttendee()
 	{
	}

CAgnAttendee* CAgnAttendee::NewL(RReadStream& aStream)
	{
	CAgnAttendee* self = new(ELeave)CAgnAttendee();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CAgnAttendee* CAgnAttendee::NewL(const TDesC& aAddress, const TDesC& aSentBy) 
/**
@internalAll
*/
	{
	CAgnAttendee* self = new(ELeave)CAgnAttendee();
	CleanupStack::PushL(self);
	self->ConstructL(aAddress, aSentBy);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CAgnAttendee* CAgnAttendee::CloneL() const
/**
@internalAll
*/
	{
	CAgnAttendee* attendee = new (ELeave) CAgnAttendee();
	CleanupStack::PushL(attendee);
	attendee->CopyL(*this);
	CleanupStack::Pop(attendee);
	return attendee;
	}

void CAgnAttendee::ConstructL(const TDesC& aAddress, const TDesC& aSentBy)
	{
	iAddress = aAddress.AllocL();
	iSentBy = aSentBy.AllocL();
	iCommonName = KNullDesC().AllocL();
	}

CAgnAttendee::~CAgnAttendee()
	{
	delete iAddress;
	delete iCommonName;
	delete iSentBy;
	}


EXPORT_C void CAgnAttendee::SetRole(TAgnRole aRole)
/** Sets the role of the attendee.

@internalAll
@param aRole The attendee's role. */
	{
	switch(aRole)
		{
		case EAttendee:
			iCalRole = CCalAttendee::EReqParticipant; 
			break;
		case EOrganizer:
            // 	Do nothing. New Organizer person will be created and handled by CCalEntry
			break;
		case EOwner:
			iCalRole = CCalAttendee::EChair; 
			break;
		case EDelegate:
			iCalRole = CCalAttendee::EReqParticipant;
			break;
		default:
			break;	
		}
	}

EXPORT_C void CAgnAttendee::SetResponseRequested(TBool aRsvp)
/** Sets whether a response is requested from the attendee.

@internalAll
@param aRsvp ETrue if a response is requested, EFalse if a response is not 
requested. */
	{
	iRsvp = aRsvp;
	}

EXPORT_C void CAgnAttendee::SetCommonNameL(const TDesC& aCommonName)
/**
@internalAll
*/
	{
	delete iCommonName;
	iCommonName = NULL;
	iCommonName = aCommonName.AllocL();
	}

EXPORT_C void CAgnAttendee::SetCalRoleL(const CCalAttendee::TCalRole aRole)
/**
@internalAll
*/
	{
	iCalRole = aRole;
	switch(aRole)
		{
		case CCalAttendee::EChair:
		case CCalAttendee::EReqParticipant:
		case CCalAttendee::EOptParticipant:
		case CCalAttendee::ENonParticipant:
		case CCalAttendee::EVCalAttendee:
		case CCalAttendee::EVCalDelegate:
		case CCalAttendee::EVCalOwner:
			break;	
		default:
			User::Leave(KErrNotFound);
			break;		
		}
	}

EXPORT_C void CAgnAttendee::SetCalStatus(const CCalAttendee::TCalStatus aStatus)
/**
@internalAll
*/
	{
	iCalStatus = aStatus;
	}


// This code should use the chevron operator to externalise
// iAddress to save space - but cannot be changed since
// it would break file compatability with AppEngines5.1
void CAgnAttendee::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint8L(iRsvp);
	aStream.WriteUint8L(iCalRole);
	aStream.WriteUint8L(iCalStatus);
	aStream.WriteUint8L(iExpect);
	
	TInt size = 0;		
	if (iAddress)
		{
		size = iAddress->Length();
		aStream.WriteUint32L(size);
		aStream.WriteL(iAddress->Des(), size);
		}
	else
		{
		aStream.WriteUint32L(size);		
		}
		
	size = 0;
			
	if (iCommonName)
		{
		size = iCommonName->Length();
		aStream.WriteUint32L(size);
		aStream.WriteL(iCommonName->Des(), size);
		}
	else
		{
		aStream.WriteUint32L(size);		
		}

	size = 0;
			
	if (iSentBy)
		{
		size = iSentBy->Length();
		aStream.WriteUint32L(size);
		aStream.WriteL(iSentBy->Des(), size);
		}
	else
		{
		aStream.WriteUint32L(size);		
		}			
	}

EXPORT_C void CAgnAttendee::SetVCalExpect(CCalAttendee::TVCalExpect aExpect)
	{
	iExpect = aExpect;
	}

EXPORT_C CCalAttendee::TVCalExpect CAgnAttendee::VCalExpect() const
	{
	return iExpect;
	}

void CAgnAttendee::InternalizeL(RReadStream& aStream)
	{
	iRsvp = aStream.ReadUint8L();
	iCalRole = static_cast<CCalAttendee::TCalRole>(aStream.ReadUint8L());
	iCalStatus = static_cast<CCalAttendee::TCalStatus>(aStream.ReadUint8L());
	iExpect = static_cast<CCalAttendee::TVCalExpect>(aStream.ReadUint8L());
	
	// Delete any possible existing data
	delete iAddress;
	iAddress = NULL;
	
	delete iCommonName;
	iCommonName = NULL;
	
	delete iSentBy;
	iSentBy = NULL;
	
	//	read in Address
	TInt size = aStream.ReadUint32L();
	iAddress = HBufC::NewL(size);
	TPtr bufPtr = iAddress->Des();
	aStream.ReadL(bufPtr,size);

	size = aStream.ReadUint32L();
	iCommonName = HBufC::NewL(size);
	TPtr bufPtr2 = iCommonName->Des();
	aStream.ReadL(bufPtr2,size);

	size = aStream.ReadUint32L();
	iSentBy = HBufC::NewL(size);
	TPtr bufPtr3 = iSentBy->Des();
	aStream.ReadL(bufPtr3,size);
	}

void CAgnAttendee::CopyL(const CAgnAttendee& aSource)
/** Copies all the details from the source into this attendee.

@param aSource Pointer to the source attendee. */
	{
	iRsvp = aSource.ResponseRequested();
	iCalRole = aSource.CalRole();
	iCalStatus = aSource.CalStatus();
	iExpect = aSource.VCalExpect();

	// Delete any address that already exists
	delete iAddress;
	iAddress = NULL;
	iAddress = aSource.Address().AllocL();
	
	delete iCommonName;
	iCommonName = NULL;
	iCommonName = aSource.CommonName().AllocL();
	
	delete iSentBy;
	iSentBy = NULL;
	iSentBy = aSource.SentBy().AllocL();
	}

EXPORT_C const TDesC& CAgnAttendee::Address() const
/**
@internalAll
*/
	{
	__ASSERT_DEBUG(iAddress,User::Invariant());
	return *iAddress;
	}

EXPORT_C const TDesC& CAgnAttendee::CommonName() const
/**
@internalAll
*/
	{
	__ASSERT_DEBUG(iCommonName,User::Invariant());
	return *iCommonName;
	}

EXPORT_C const TDesC& CAgnAttendee::SentBy() const
/**
@internalAll
*/
	{
	__ASSERT_DEBUG(iSentBy,User::Invariant());
	return *iSentBy;
	}


EXPORT_C TBool CAgnAttendee::ResponseRequested() const
/** Tests whether a response has been requested from the attendee.

@internalAll
@return ETrue if a response is requested, EFalse if a response is not requested. */
	{
	return iRsvp;
	}

EXPORT_C CCalAttendee::TCalRole CAgnAttendee::CalRole() const
/**
The role of the attendee to a meeting (e.g. chair, required participant, optional participant).
@internalAll
*/
	{
	return iCalRole;
	}

EXPORT_C CCalAttendee::TCalStatus CAgnAttendee::CalStatus() const
/**
The status of the attendee to a meeting (e.g. tentative, declined, accepted).
@internalAll
*/
	{
	return iCalStatus;
	}

