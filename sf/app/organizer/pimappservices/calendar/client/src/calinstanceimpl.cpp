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

#include "calinstanceimpl.h"
#include <calentry.h>
#include "calentryimpl.h"
#include "agmentry.h"
#include "calinstance.h"

CCalInstanceImpl::CCalInstanceImpl(CCalEntry* aEntry, const TCalTime& aTime) :
	iCalEntry(*aEntry), iCalTime(aTime)
	{
	}
	
CCalInstanceImpl::~CCalInstanceImpl()
	{
	delete &iCalEntry;
	}

CCalInstanceImpl* CCalInstanceImpl::NewL(CCalEntry* aEntry, const TCalTime& aTime)
	{
	CCalInstanceImpl* self = new (ELeave) CCalInstanceImpl(aEntry, aTime);
	return self;
	}

CCalEntry& CCalInstanceImpl::Entry() const
	{
	return iCalEntry;
	}

TCalTime CCalInstanceImpl::Time() const
	{
	return iCalTime;
	}
	
TCalTime CCalInstanceImpl::StartTimeL() const
	{
	TCalTime instanceStartTime;
	TTimeIntervalMicroSeconds duration(0);
	if (iCalEntry.EntryTypeL() == CCalEntry::ETodo)
		{
		TTime entryStartTime(iCalEntry.StartTimeL().TimeLocalL());
		TTime entryEndTime(iCalEntry.EndTimeL().TimeLocalL());
		TTime nullTime = Time::NullTTime();
		if (!(nullTime == entryStartTime || nullTime == entryEndTime))
			{
			duration = entryEndTime.MicroSecondsFrom(entryStartTime);
			}
		}
	if (iCalTime.TimeMode() == TCalTime::EFloating)
		{
		instanceStartTime.SetTimeLocalFloatingL(iCalTime.TimeLocalL() - duration);
		}
	else
		{
		instanceStartTime.SetTimeLocalL(iCalTime.TimeLocalL() - duration);
		}
	return instanceStartTime;
	}

TCalTime CCalInstanceImpl::EndTimeL() const
	{
	TCalTime instanceEndTime;
	TTimeIntervalMicroSeconds duration(0);
	if (iCalEntry.EntryTypeL() != CCalEntry::ETodo)
		{
		TTime entryStartTime(iCalEntry.StartTimeL().TimeLocalL());
		TTime entryEndTime(iCalEntry.EndTimeL().TimeLocalL());
		TTime nullTime = Time::NullTTime();
		if (!(nullTime == entryStartTime || nullTime == entryEndTime))
			{
			duration = entryEndTime.MicroSecondsFrom(entryStartTime);
			}
		}

	if (iCalTime.TimeMode() == TCalTime::EFloating)
		{
		instanceEndTime.SetTimeLocalFloatingL(iCalTime.TimeLocalL() + duration);
		}
	else
		{
		instanceEndTime.SetTimeLocalL(iCalTime.TimeLocalL() + duration);
		}
	return instanceEndTime;
	}

TCalInstanceId CCalInstanceImpl::InstanceIdL() const
	{
	TCalInstanceId instanceId;
	instanceId.iEntryLocalId = iCalEntry.LocalUidL();
	instanceId.iInstanceTime = Time();
	instanceId.iCollectionId = iCalEntry.ShortFileIdL();
	return instanceId;
	}

