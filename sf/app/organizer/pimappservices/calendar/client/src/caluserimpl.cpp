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

#include "agmattendee.h"	

CCalUserImpl::CCalUserImpl()
	:iOwnsCAgnAttendee(ETrue)
	{
	}

CCalUserImpl::CCalUserImpl(CAgnAttendee* aAttendee)
	{
	iAttendee = aAttendee;
	iOwnsCAgnAttendee = EFalse;
	}

CCalUserImpl::~CCalUserImpl()
	{
	if(iOwnsCAgnAttendee)
		{
		delete iAttendee;
		}
	}

CCalUserImpl* CCalUserImpl::NewL(const TDesC& aAddress)
	{
	CCalUserImpl* self = new(ELeave) CCalUserImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aAddress, KNullDesC);
	CleanupStack::Pop(self);
	return self;
	}

CCalUserImpl* CCalUserImpl::NewL(const TDesC& aAddress, const TDesC& aSentBy) 
	{
	CCalUserImpl* self = new(ELeave) CCalUserImpl();
	CleanupStack::PushL(self);
	self->ConstructL(aAddress, aSentBy);
	CleanupStack::Pop(self);
	return self;
	}

CCalUserImpl* CCalUserImpl::NewL(CAgnAttendee* aAttendee) 
	{
	// by default, doesn't take ownership of aAttendee
	CCalUserImpl* self = new(ELeave) CCalUserImpl(aAttendee);
	self->SetOwnsCAgnAttendee(EFalse);
	return self;
	}

void CCalUserImpl::ConstructL(const TDesC& aAddress, const TDesC& aSentBy)
	{
	iAttendee = CAgnAttendee::NewL(aAddress, aSentBy);
	}
	
CCalAttendee::TCalRole CCalUserImpl::RoleL() const
	{
	return iAttendee->CalRole();
	}
	
CCalAttendee::TCalStatus CCalUserImpl::StatusL() const
	{
	return iAttendee->CalStatus();
	}

TBool CCalUserImpl::ResponseRequested() const
	{
	return iAttendee->ResponseRequested();
	}

CCalAttendee::TVCalExpect CCalUserImpl::VCalExpect() const
	{
	return iAttendee->VCalExpect();
	}
	
const TDesC& CCalUserImpl::Address() const
	{
	return iAttendee->Address();
	}
	
const TDesC& CCalUserImpl::CommonName() const
	{
	return iAttendee->CommonName();
	}
	
const TDesC& CCalUserImpl::SentBy() const
	{
	return iAttendee->SentBy();
	}


CAgnAttendee* CCalUserImpl::Attendee() const
	{
	return iAttendee;
	}

TBool CCalUserImpl::SelfOwnership() const
	{
	return iOwnsCAgnAttendee;
	}

void CCalUserImpl::SetRoleL(CCalAttendee::TCalRole aRole)
	{
	iAttendee->SetCalRoleL(aRole);
	}
	
void CCalUserImpl::SetStatusL(CCalAttendee::TCalStatus aStatus)
	{
	if(aStatus < CCalAttendee::ENeedsAction || aStatus > CCalAttendee::EInProcess)
		{
		User::Leave(KErrNotFound);
		}
	iAttendee->SetCalStatus(aStatus);
	}
	
void CCalUserImpl::SetResponseRequested(TBool aRsvp)
	{
	iAttendee->SetResponseRequested(aRsvp);
	}
	
void CCalUserImpl::SetCommonNameL(const TDesC& aCommonName)
	{
	iAttendee->SetCommonNameL(aCommonName);
	}
	
void CCalUserImpl::SetOwnsCAgnAttendee(TBool aOwnership)
	{
	iOwnsCAgnAttendee = aOwnership;
	}
	
void CCalUserImpl::SetVCalExpect(CCalAttendee::TVCalExpect aExpected)
	{
	iAttendee->SetVCalExpect(aExpected);
	}


