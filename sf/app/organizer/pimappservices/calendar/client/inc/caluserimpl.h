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

#ifndef __CALUSERIMPL_H__
#define __CALUSERIMPL_H__

#include "caluser.h"

class CAgnAttendee;

/**
@internalComponent
*/
NONSHARABLE_CLASS(CCalUserImpl) : public CBase
	{
public:	
	static CCalUserImpl* NewL(const TDesC& aAddress);
	static CCalUserImpl* NewL(const TDesC& aAddress, const TDesC& aSentBy);
	/*	This constructor is for internal use only. This is only used to populate 
		the CalInterimAPI CalAttendeeList from the agnmodel attendeelist thus it 
		never takes ownership of the CAgnAttendees.		*/
	static CCalUserImpl* NewL(CAgnAttendee* aAttendee);
	~CCalUserImpl();

	void SetRoleL(CCalAttendee::TCalRole aRole);
	void SetStatusL(CCalAttendee::TCalStatus aStatus);
	void SetVCalExpect(CCalAttendee::TVCalExpect aExpected);
	void SetResponseRequested(TBool aRsvp);
	void SetCommonNameL(const TDesC& aCommonName);	
	void SetOwnsCAgnAttendee(TBool aOwnership);
	
	CCalAttendee::TCalRole RoleL() const;	
	CCalAttendee::TCalStatus StatusL() const;
	CCalAttendee::TVCalExpect VCalExpect() const;
	TBool ResponseRequested() const;
	const TDesC& Address() const;
	const TDesC& CommonName() const;
	const TDesC& SentBy() const;
	TBool SelfOwnership() const;

	CAgnAttendee* Attendee() const;

private:
	CCalUserImpl();
	CCalUserImpl(CAgnAttendee* aAttendee);

	void ConstructL(const TDesC& aAddress, const TDesC& aSentBy);

private:	
	CAgnAttendee* iAttendee;
	//	iOwnsCAgnAttendee is a flag to determine if this CCalAttendee class owns the 
	//	iAttendee or not when calling its destructor.
	TBool iOwnsCAgnAttendee;
	};
	
#endif
