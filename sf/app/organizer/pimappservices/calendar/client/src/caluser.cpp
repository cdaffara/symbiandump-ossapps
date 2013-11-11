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

#include "caluserimpl.h"

/** Constructor.
@internalComponent
*/
CCalUser::CCalUser(CCalUserImpl* aImpl)
	: iImpl(aImpl)
	{
	}

/** Constructor.
All variables are implicitly set to default values due to CBase derivation.
iRole set to TCalRole::EReqParticipant.
iStatus set to TCalStatus::ENeedsAction.
The default value must be at index position 0 of the enum.
@internalComponent
*/
CCalUser::CCalUser()
	{
	}

/** Destructor.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalUser::~CCalUser()
	{
	delete iImpl;
	}

/** Allocates and constructs a new user with the specified email address.

@param aAddress The address of the user.
@return The newly constructed object.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalUser* CCalUser::NewL(const TDesC& aAddress)
	{
	CCalUser* self = new (ELeave) CCalUser();
	CleanupStack::PushL(self);
	self->ConstructL(aAddress);
	CleanupStack::Pop(self);
	return self;
	}

/** Allocates and constructs a new user with the specified email address and sender.

@param aAddress The address of the user.
@param aSentBy The sender.
@return The newly constructed object.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalUser* CCalUser::NewL(const TDesC& aAddress, const TDesC& aSentBy)
	{
	CCalUser* self = new (ELeave) CCalUser();
	CleanupStack::PushL(self);
	self->ConstructL(aAddress, aSentBy);
	CleanupStack::Pop(self);
	return self;
	}
// Takes ownership of the implementation class
CCalUser* CCalUser::NewL(CCalUserImpl* aImpl)
	{
	CCalUser* self = new (ELeave) CCalUser(aImpl);
	return self;
	}

/** Second phase constructor.
@internalComponent
*/
void CCalUser::ConstructL(const TDesC& aAddress)
	{
	iImpl = CCalUserImpl::NewL(aAddress);
	}

/** Second phase constructor.
@internalComponent
*/
void CCalUser::ConstructL(const TDesC& aAddress, const TDesC& aSentBy)
	{
	iImpl = CCalUserImpl::NewL(aAddress, aSentBy);
	}

/** Gets the email address of the user.

@return The address.

@publishedAll
@released
@capability None
*/
EXPORT_C const TDesC& CCalUser::Address() const
	{
	return iImpl->Address();
	}

/** Gets the iCalendar-defined common name (CN) of the user.

@return The common name.

@publishedAll
@released
@capability None
*/
EXPORT_C const TDesC& CCalUser::CommonName() const
	{
	return iImpl->CommonName();
	}

/** Gets the sender of the user.

@return The sender.

@publishedAll
@released
@capability None
*/
EXPORT_C const TDesC& CCalUser::SentBy() const
	{
	return iImpl->SentBy();
	}

CCalUserImpl* CCalUser::Impl() const
	{
	return iImpl;
	}

/** Sets the iCalendar-defined common name (CN) of the user.

@param aCommonName The common name.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalUser::SetCommonNameL(const TDesC& aCommonName)
	{
	iImpl->SetCommonNameL(aCommonName);
	}

CCalAttendee::CCalAttendee()
	{
	}

CCalAttendee::CCalAttendee(CCalUserImpl* aImpl)
	: CCalUser(aImpl)
	{
	}

/** Allocates and constructs a new attendee with the specified email address.

@param aAddress The address of the attendee.
@return Pointer to newly constructed object
@publishedAll
@released
@capability None
*/
EXPORT_C CCalAttendee* CCalAttendee::NewL(const TDesC& aAddress)

	{
	CCalAttendee* self = new (ELeave) CCalAttendee();
	CleanupStack::PushL(self);
	self->ConstructL(aAddress);
	CleanupStack::Pop(self);
	return self;
	}

/** Allocates and constructs a new attendee with the specified email address and sender.

@param aAddress The address of the attendee.
@param aSentby The sender.
@return Pointer to newly constructed object
@publishedAll
@released
@capability None
*/
EXPORT_C CCalAttendee* CCalAttendee::NewL(const TDesC& aAddress, const TDesC& aSentBy)
	{
	CCalAttendee* self = new (ELeave) CCalAttendee();
	CleanupStack::PushL(self);
	self->ConstructL(aAddress, aSentBy);
	CleanupStack::Pop(self);
	return self;
	}


CCalAttendee* CCalAttendee::NewL(CCalUserImpl* aImpl)
	{
	CCalAttendee* self = new (ELeave) CCalAttendee(aImpl);
	return self;
	}

/** Get the role of this attendee.

@return The role.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalAttendee::TCalRole CCalAttendee::RoleL() const
	{
	return iImpl->RoleL();
	}

/** Get the status of this attendee.

@return The status.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalAttendee::TCalStatus CCalAttendee::StatusL() const
	{
	return iImpl->StatusL();
	}

/** Gets whether or not a response was requested for this attendee.
@publishedAll
@released
@capability None
@return ETrue if a response was requested, EFalse if not. */
EXPORT_C TBool CCalAttendee::ResponseRequested() const
	{
	return iImpl->ResponseRequested();
	}

/** Set the role of this attendee.

@param aRole The role.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalAttendee::SetRoleL(TCalRole aRole)
	{
	iImpl->SetRoleL(aRole);
	}

/** Set the status of this attendee.

@param aStatus The status.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalAttendee::SetStatusL(TCalStatus aStatus)
	{
	iImpl->SetStatusL(aStatus);
	}

/** Sets whether or not a response was requested for this attendee.

@param aRsvp ETrue if a response was requested, EFalse if not.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalAttendee::SetResponseRequested(TBool aRsvp)
	{
	iImpl->SetResponseRequested(aRsvp);
	}

/** Sets the expected response of this attendee.
This property is supported in vCalendar only.
@param aExpected The expected response.

@publishedAll
@released
@capability None
*/
EXPORT_C void CCalAttendee::SetVCalExpect(TVCalExpect aExpected)
	{
	iImpl->SetVCalExpect(aExpected);
	}
	
/** Gets the expected response of this attendee.
This property is supported in vCalendar only.
@return The expected response.

@publishedAll
@released
@capability None
*/
EXPORT_C CCalAttendee::TVCalExpect CCalAttendee::VCalExpect() const
	{
	return iImpl->VCalExpect();
	}

