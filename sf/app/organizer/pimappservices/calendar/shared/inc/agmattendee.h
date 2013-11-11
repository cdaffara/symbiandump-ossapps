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


#ifndef __AGMATTENDEE_H__
#define __AGMATTENDEE_H__

#include "caluser.h"

//
// CAgnAttendee

class RWriteStream;
class RReadStream;

NONSHARABLE_CLASS(CAgnAttendee) : public CBase
/** Attendee to a group event, for instance a meeting.

The event may have multiple attendees. Each attendee has a role (e.g. delegate, 
organiser), a status (e.g. accepted, declined), an indicator of whether a reply 
is requested from the attendee, an address.

These properties are identical to those accessible through the CCalAttendee interface.

@internalAll
@released
*/
	{
public:

	/** Attendee's role 
	@internalAll
	@released
	*/
	enum TAgnRole 
			{
			/** Attendee. */
			EAttendee,
			/** Organizer, but not owner. */
			EOrganizer, 
			/** Owner. */
			EOwner, 
			/** A delegate of another attendee. */
			EDelegate 
			};

public:
	// construct / destruct functions
	IMPORT_C static CAgnAttendee* NewL(const TDesC& aAddress, const TDesC& aSentBy);
	IMPORT_C CAgnAttendee* CloneL() const;
	static CAgnAttendee* NewL(RReadStream& aStream);
	~CAgnAttendee();

	// Get functions
	IMPORT_C TBool ResponseRequested() const;
	IMPORT_C const TDesC& Address() const;
	IMPORT_C const TDesC& CommonName() const;
	IMPORT_C const TDesC& SentBy() const;
	IMPORT_C CCalAttendee::TCalRole CalRole() const;
	IMPORT_C CCalAttendee::TCalStatus CalStatus() const;
	IMPORT_C CCalAttendee::TVCalExpect VCalExpect() const;

	// Set functions
	IMPORT_C void SetRole(TAgnRole aRole);
	IMPORT_C void SetResponseRequested(TBool aRsvp);	// default is no
	IMPORT_C void SetCommonNameL(const TDesC& aCommonName);	
	IMPORT_C void SetCalRoleL(CCalAttendee::TCalRole aRole);
	IMPORT_C void SetCalStatus(CCalAttendee::TCalStatus aStatus);
	IMPORT_C void SetVCalExpect(CCalAttendee::TVCalExpect aExpect);
	
	// Persistence
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
	void CopyL(const CAgnAttendee& aSource);
	
private:
	CAgnAttendee();
	void ConstructL(const TDesC& aAddress, const TDesC& aSentBy);
	
private:
	TBool						iRsvp;
	CCalAttendee::TVCalExpect	iExpect;
	CCalAttendee::TCalRole		iCalRole;
	CCalAttendee::TCalStatus	iCalStatus;
	HBufC*						iAddress;
	HBufC*						iCommonName;
	HBufC*						iSentBy;
	};

#endif
