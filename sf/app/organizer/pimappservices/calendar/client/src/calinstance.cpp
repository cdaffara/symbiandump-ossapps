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

#include <calinstance.h>

#include "calcommonimpl.h"
#include <calentry.h>
#include "calinstanceimpl.h"

CCalInstance::CCalInstance() 
	{
	}
	
/** Destructor for the instance - if this is the last instance of an 
entry, the entry is deleted too. 

@publishedAll
@released
@capability None
*/
EXPORT_C CCalInstance::~CCalInstance()
	{
	delete iImpl;
	}

CCalInstance* CCalInstance::NewL(CCalEntry* aEntry, const TCalTime& aTime)
	{
	__ASSERT_ALWAYS(aEntry, CalUtils::Panic(EInstanceCreatedFromNullEntry));
	CCalInstance* self = new (ELeave) CCalInstance();
	CleanupStack::PushL(self);
	self->ConstructL(aEntry, aTime);
	CleanupStack::Pop(self);
	return self;
	}


void CCalInstance::ConstructL(CCalEntry* aEntry, const TCalTime& aTime)
	{
	iImpl = CCalInstanceImpl::NewL(aEntry, aTime);
	}

/** Gets the entry that this instance belongs to.

@return Reference to the entry. 

@publishedAll
@released
@capability None
*/
EXPORT_C CCalEntry& CCalInstance::Entry() const
	{
	return iImpl->Entry();
	}

/** Get the time of this instance.

@return The time of the instance. 

@publishedAll
@released
@capability None
@capability None
*/
EXPORT_C TCalTime CCalInstance::Time() const
	{
	return iImpl->Time();
	}
	
/** Get the start time of this instance.

@return The start time of this instance. 

@publishedAll
@released
@capability None
@capability None
*/
EXPORT_C TCalTime CCalInstance::StartTimeL() const
	{
	return iImpl->StartTimeL();
	}
	
/** Get the end time of this instance.

@return The end time of this instance. 

@publishedAll
@released
@capability None
@capability None
*/
EXPORT_C TCalTime CCalInstance::EndTimeL() const
	{
	return iImpl->EndTimeL();
	}

/** Get the TCalInstanceId of this instance.

@return The TCalInstanceId of this instance. 

@publishedAll
@released
@capability None
@capability None
*/
EXPORT_C TCalInstanceId CCalInstance::InstanceIdL() const
	{
	return iImpl->InstanceIdL(); 
	}

