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

#ifndef __CALUSER_H__
#define __CALUSER_H__

#include <e32base.h>

class CCalUserImpl;

/** Class representing a calendar user.

A calendar user is the base class representation of a meeting participant.
It contains attributes common to all calendar users.

Contains methods to store and retrieve a user's ADDRESS, SENT-BY and CN
fields.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalUser) : public CBase
	{
public:
	IMPORT_C static CCalUser* NewL(const TDesC& aAddress);
	IMPORT_C static CCalUser* NewL(const TDesC& aAddress, const TDesC& aSentBy);
	IMPORT_C ~CCalUser();

	IMPORT_C void SetCommonNameL(const TDesC& aCommonName);
	IMPORT_C const TDesC& Address() const;
	IMPORT_C const TDesC& CommonName() const;
	IMPORT_C const TDesC& SentBy() const;
public:
	static CCalUser* NewL(CCalUserImpl* aImpl);

	CCalUserImpl* Impl() const;
protected:
	CCalUser();
	CCalUser(CCalUserImpl* aImpl);
	void ConstructL(const TDesC& aAddress);
	void ConstructL(const TDesC& aAddress, const TDesC& aSentBy);
protected:
/** Handle to CCalUser implementation */
	CCalUserImpl*	iImpl;
	};


/** Class representing an attendee of an event.

CCalAttendee is a specialization of the CCalUser class.

This class contains extra methods to store and retrieve an Attendee's
ROLE, PART-STAT, and RSVP fields.

@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalAttendee) : public CCalUser
	{
public:
	/** Attendee's role. 
	@publishedAll
	@released
	*/
	enum TCalRole
		{
		/** A required participant of the event. */
		EReqParticipant=0,
		/** An optional participant of the event. */
		EOptParticipant,
		/** A non-participant of the event. */
		ENonParticipant,
		/** This participant will chair the event. */
		EChair,
		
		/** Indicates an attendee at the event or todo. This value is supported in vCalendar only. */
		EVCalAttendee,
		/** Indicates a delegate of another attendee. This value is supported in vCalendar only. */
		EVCalDelegate,
		/** Indicates owner of the event or todo (not the same as phone owner). This value is supported in vCalendar only. */
		EVCalOwner,
		};

	/** Attendee's status
	@publishedAll
	@released
	*/
	enum TCalStatus
		{
		/** Action is required by attendee. */
		ENeedsAction=0,
		/** Attendee has accepted request. */
		EAccepted,
		/** Attendee has tentatively accepted the request. */
		ETentative,
		/** Attendee's presence is confirmed. */
		EConfirmed,
		/** Attendee has declined request. */
		EDeclined,
		/** The required action has been completed by attendee. */
		ECompleted,
		/** Attendee has delegated the request to another person. */
		EDelegated,
		/**	A to-do action in the process of being completed. */
		EInProcess,
		
		/** An entry has been sent. This value is supported in vCalendar only. */
		EVCalSent,
		/** An entry has been received. This value is supported in vCalendar only. */
		EVCalXReceived,
		};
	
	/** Attendee's expected participation response.
	This property is supported in vCalendar only. It is not a property of iCalendar.
	@publishedAll
	@released
	*/
	enum TVCalExpect
		{
		/** Indicates request is for your information. */
		EVCalFyi,
		/** Indicates presence is definitely required. */
		EVCalRequire,
		/** Indicates presence is being requested. */
		EVCalRequest, 
		/** Indicates an immediate response needed. */
		EVCalImmediate 
		};

public:
	IMPORT_C static CCalAttendee* NewL(const TDesC& aAddress);
	IMPORT_C static CCalAttendee* NewL(const TDesC& aAddress, const TDesC& aSentBy);

	IMPORT_C void SetRoleL(TCalRole aRole);
	IMPORT_C void SetStatusL(TCalStatus aStatus);
	IMPORT_C void SetResponseRequested(TBool aRsvp);
	IMPORT_C void SetVCalExpect(TVCalExpect aExpected);

	IMPORT_C TCalRole RoleL() const;
	IMPORT_C TCalStatus StatusL() const;
	IMPORT_C TBool ResponseRequested() const;
	IMPORT_C TVCalExpect VCalExpect() const;
public:
	static CCalAttendee* NewL(CCalUserImpl* aImpl);
private:
	CCalAttendee();
	CCalAttendee(CCalUserImpl* aImpl);
	};


#endif // __CALUSER_H__

