// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "agmtlsproxy.h"
#include "agmcalendartime.h"
 
#include <vtzrules.h>
#include <tzconverter.h>

const TInt KMinYear = 0;
const TInt KMaxYear = 9999;

CAgnTlsProxy* CAgnTlsProxy::NewL(TAgnTlsTzRulesType aTzRulesType)
	{
	CAgnTlsProxy* self = new (ELeave) CAgnTlsProxy();
	CleanupStack::PushL(self);
	self->ConstructL(aTzRulesType);
	CleanupStack::Pop(self);
	return self;
	}
	
CAgnTlsProxy::~CAgnTlsProxy()
	{
	delete iCurrentTzChangeObserver;
	delete iTzRulesChangeObserver;
	delete iTimeConverter;
	delete iFixedTimeMode;
	delete iFloatingTimeMode;
	delete iTlsTzRules;
	iTzServer.Close();
	iObservers.Reset();
	}

EXPORT_C CAgnTlsProxy* CAgnTlsProxy::CreateL(TAgnTlsTzRulesType aTzRulesType)
	{
	CAgnTlsProxy* self = static_cast<CAgnTlsProxy*>(Dll::Tls());
	
	if (!self)
		{
		self = CAgnTlsProxy::NewL(aTzRulesType);
		Dll::SetTls(self);
		}
		
	++self->iRefCount;
	
	return self;
	}
	
EXPORT_C void CAgnTlsProxy::Release(TAny* /*aRef*/)
	{
	CAgnTlsProxy* self = static_cast<CAgnTlsProxy*>(Dll::Tls());
	
	if(self)
	    {
	    if (--self->iRefCount <= 0)
	        {
	        __ASSERT_DEBUG(self->iRefCount == 0, User::Invariant());
	            
	        delete self;
	        Dll::FreeTls();
	        }
	    }	
	}
	
CAgnTlsProxy::CAgnTlsProxy()
	:iRefCount(0)
	{
	}

void CAgnTlsProxy::ConstructL(TAgnTlsTzRulesType aTzRulesType)
	{
	User::LeaveIfError(iTzServer.Connect());
	iTimeConverter = CTzConverter::NewL(iTzServer);
	iFixedTimeMode = new (ELeave) TAgnCalendarFixedTimeMode(*iTimeConverter);
	iFloatingTimeMode = new (ELeave) TAgnCalendarFloatingTimeMode(*iTimeConverter);
	switch(aTzRulesType)
		{
		case TAgnTlsTzRulesType_Client:
			iTlsTzRules = new (ELeave) CAgnClientTlsTzRules();
			break;
		case TAgnTlsTzRulesType_Server:
			iTlsTzRules = new (ELeave) CAgnServerTlsTzRules();
			break;
		default:
			iTlsTzRules = NULL;
			break;
		}
	iCurrentTzChangeObserver = CAgnTzChangeObserver::NewL(*this, NTzUpdate::ECurrentTimeZoneId);
	iTzRulesChangeObserver = CAgnTzChangeObserver::NewL(*this, NTzUpdate::ETzRulesChange);;
	}

const TAgnCalendarFixedTimeMode& CAgnTlsProxy::FixedTimeMode() const
	{
	return *iFixedTimeMode;
	}

const TAgnCalendarFloatingTimeMode& CAgnTlsProxy::FloatingTimeMode() const
	{
	return *iFloatingTimeMode;
	}

EXPORT_C RTz& CAgnTlsProxy::TzServer()
	{
	return iTzServer;
	}

TUint16 CAgnTlsProxy::CurrentTzId() const
	{
	return iTimeConverter->CurrentTzId();
	}

CTzRules* CAgnTlsProxy::GetZoneForRulesL(const CTzRules& aRules)
	{
	CTzRules* foundTzRules = NULL;
	if(iTlsTzRules)
		{
		foundTzRules = iTlsTzRules->GetSharedTzRulesL(aRules);
		}
	return foundTzRules;
	}

void CAgnTlsProxy::ReferenceRemoved(const CTzRules* aReference)
	{
	if(iTlsTzRules)
		{
		iTlsTzRules->ReferenceRemoved(aReference);
		}
	}

EXPORT_C CTzRules* CAgnTlsProxy::GetCurrentSystemTzRulesL()
	{
	if(iTlsTzRules)
		{
		return iTlsTzRules->GetCurrentSystemTzRulesL(iTzServer);
		}
	return NULL;
	}

void CAgnTlsProxy::HandleTzPropertyChangeL(NTzUpdate::TPropertyKeys aTzChangePropertyKey)
	{
	if(iTlsTzRules)
		{
		iTlsTzRules->SystemTzZoneChangedL(iTzServer, aTzChangePropertyKey);
		}

	// Notify observers
	
	__ASSERT_DEBUG(aTzChangePropertyKey==NTzUpdate::ECurrentTimeZoneId || aTzChangePropertyKey==NTzUpdate::ETzRulesChange, User::Invariant());
	const TInt KObserverCount(iObservers.Count());
	if(aTzChangePropertyKey == NTzUpdate::ECurrentTimeZoneId)
		{
		CTzId* sysTime = iTzServer.GetTimeZoneIdL();
		CleanupStack::PushL(sysTime);
		for (TInt i(0) ; i < KObserverCount ; ++i)
			{
			iObservers[i]->HandleCurrentTimeZoneChangeL(*sysTime);
			}
		CleanupStack::PopAndDestroy(sysTime);
		}
	else 
		{
		TPckgBuf<NTzUpdate::TTzRulesChange> pubSubBuf;

		if(KErrNone == RProperty::Get(NTzUpdate::KPropertyCategory, NTzUpdate::ETzRulesChange, pubSubBuf))
			{
			for (TInt i(0) ; i < KObserverCount ; ++i)
				{
				iObservers[i]->HandleTzRulesChangeL(pubSubBuf().iUTCTimeOfRulesChange);
				}
			}
		}
	}

EXPORT_C void CAgnTlsProxy::AddSystemTimeObserverL(MAgnTzChangeObserver* aObserver)
	{
	TRAPD(err, iObservers.InsertInAddressOrderL(aObserver));
	if(err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	//Get the time stamp of time zone database changed last time and force the observer to respond 
	TPckgBuf<NTzUpdate::TTzRulesChange> pubSubBuf;
	if(KErrNone == RProperty::Get(NTzUpdate::KPropertyCategory, NTzUpdate::ETzRulesChange, pubSubBuf))
		{
		TTime tzChange = pubSubBuf().iUTCTimeOfRulesChange;
		aObserver->HandleTzRulesChangeL(tzChange);
		}
	}

EXPORT_C void CAgnTlsProxy::RemoveSystemTimeObserver(MAgnTzChangeObserver* aObserver)
	{
	TInt index(iObservers.FindInAddressOrder(aObserver));
	
	if (index != KErrNotFound)
		{
		iObservers.Remove(index);
		}
	}
	
#if defined(_DEBUG)

EXPORT_C void CAgnTlsProxy::__dbgClearTzClientCacheL(TBool aRestartCaching)
	{
	// This will clear the actualised rules cache
	CTzRules* rules = iTlsTzRules->GetCurrentSystemTzRulesL(iTzServer);
	rules->SetInitialStdTimeOffset(rules->InitialStdTimeOffset());
	iTlsTzRules->ReferenceRemoved(rules);
	
	iTzServer.__dbgClearCacheL(aRestartCaching);
	}
	
#else

EXPORT_C void CAgnTlsProxy::__dbgClearTzClientCacheL(TBool /*aRestartCaching*/)	{}

#endif

//
// Tls Shared Time Zone Rules classes
//

CAgnTlsTzRules::CAgnTlsTzRules() : iCurrentSystemTzRules(NULL) 
	{
	}

CAgnTlsTzRules::~CAgnTlsTzRules()
	{
	}

CTzRules* CAgnTlsTzRules::GetSystemTzFromTzServerL(RTz& aTzServer)
	{
	TTime startTime(0);
	TTime endTime(0);
	return aTzServer.GetTimeZoneRulesL(startTime, endTime, ETzUtcTimeReference);
	}

CAgnServerTlsTzRules::~CAgnServerTlsTzRules()
	{
	delete iCurrentSystemTzRules;
	}

CTzRules* CAgnServerTlsTzRules::GetSharedTzRulesL(const CTzRules& aTzRules)
	{
	return aTzRules.CloneL();
	}

void CAgnServerTlsTzRules::ReferenceRemoved(const CTzRules* aReference)
	{
	__ASSERT_DEBUG(aReference != iCurrentSystemTzRules, _L("deleting iCurrentSystemTzZone in CAgnServerTlsTzRules"));
	delete aReference;
	}

CTzRules* CAgnServerTlsTzRules::GetCurrentSystemTzRulesL(RTz& aTzServer)
	{
	if(!iCurrentSystemTzRules)
		{
		iCurrentSystemTzRules = GetSystemTzFromTzServerL(aTzServer);
		}
	
	return iCurrentSystemTzRules->CloneL(); 
	}

void CAgnServerTlsTzRules::SystemTzZoneChangedL(RTz& aTzServer, NTzUpdate::TPropertyKeys /*aTzChangePropertyKey*/)
	{
	CTzRules* oldTzRules = iCurrentSystemTzRules; 
	iCurrentSystemTzRules = GetSystemTzFromTzServerL(aTzServer);
	delete oldTzRules;
	}

CAgnClientTlsTzRules::~CAgnClientTlsTzRules()
	{
	iTzRules.ResetAndDestroy();
	iTzRulesReferences.Reset();
	iTzIds.Reset();
	}

CTzRules* CAgnClientTlsTzRules::GetSharedTzRulesL(const CTzRules& aTzRules)
	{
	CTzRules* foundTzRules = NULL;
	const TInt KRuleCount(iTzRules.Count());
	for (TInt i = 0; i < KRuleCount; i++)
		{
		if (iTzRules[i]->IsEqualTo(aTzRules))
			{
			foundTzRules = iTzRules[i];
			iTzRulesReferences[i] = iTzRulesReferences[i] + 1;
			break;
			}
		}
	
	if (foundTzRules == NULL)
		{
		foundTzRules = aTzRules.CloneL();
		
		CleanupStack::PushL(foundTzRules);
		User::LeaveIfError(iTzRulesReferences.Append(1));
		iTzRules.AppendL(foundTzRules);
		iTzIds.AppendL(KInvalidTzId);//KInvalidTzId represents a rule which is not obtained from the tz server
		CleanupStack::Pop(foundTzRules);
		}
	
	return foundTzRules;
	}

void CAgnClientTlsTzRules::ReferenceRemoved(const CTzRules* aReference)
	{
	if (aReference == NULL)
		{
		return;
		}

	const TInt KRuleCount(iTzRules.Count());
	for (TInt i = 0; i < KRuleCount; i++)
		{
		if (iTzRules[i] == aReference)
			{
			if (--iTzRulesReferences[i] <= 0)
				{
				delete iTzRules[i];
				iTzRules.Remove(i);
				iTzRulesReferences.Remove(i);
				iTzIds.Remove(i);
				}
			break;
			}
		}
	}

void CAgnClientTlsTzRules::SetCurrentSystemTzRulesL(CTzRules* aTzRules, TUint aTzId)
	{
	if(iCurrentSystemTzRules)
		{
		if (iCurrentSystemTzRules->IsEqualTo(*aTzRules))
			{
			delete aTzRules;
			return; //We don't need to do anything as the system zone has been set.
			}
		else
			{
			ReferenceRemoved(iCurrentSystemTzRules);
			iCurrentSystemTzRules = NULL;
			}
		}
	
	CleanupStack::PushL(aTzRules);
	User::LeaveIfError(iTzRulesReferences.Append(1));
	iTzRules.AppendL(aTzRules);
	iTzIds.AppendL(aTzId);
	CleanupStack::Pop(aTzRules);
	
	iCurrentSystemTzRules = aTzRules;
	}

void CAgnClientTlsTzRules::UpdateCurrentSystemTzRulesL(RTz& aTzServer)
	{
	CTzId* tzid = aTzServer.GetTimeZoneIdL();
	CleanupStack::PushL(tzid);
	CTzRules* rules = GetSystemTzFromTzServerL(aTzServer);
	SetCurrentSystemTzRulesL(rules, tzid->TimeZoneNumericID());
	CleanupStack::PopAndDestroy(tzid);
	}

CTzRules* CAgnClientTlsTzRules::GetCurrentSystemTzRulesL(RTz& aTzServer)
	{
	if(iCurrentSystemTzRules == NULL)
		{
		UpdateCurrentSystemTzRulesL(aTzServer);
		}
	
	AddReference(iCurrentSystemTzRules);
	
	return iCurrentSystemTzRules;  
	}

void CAgnClientTlsTzRules::UpdateCacheL(RTz& aTzServer)
	{
	const TInt count = iTzIds.Count();
	for(TInt ii=0; ii<count; ++ii)
		{
		if(iTzIds[ii]>0)
			{
			CTzId* id = CTzId::NewL(iTzIds[ii]);
			CTzRules* rules = NULL;
			TRAPD(err, rules = aTzServer.GetTimeZoneRulesL(*id, KMinYear, KMaxYear, ETzUtcTimeReference));
			delete id;
			CleanupStack::PushL(rules);
			if(err != KErrNone && err != KErrNotFound)
				{
				User::Leave(err);
				}
			if(rules && !iTzRules[ii]->IsEqualTo(*rules))
				{//If a tz rule chached is different from the one in tz server for the same TZ ID, it needs to be updated with the new one.
				iTzRules[ii]->CopyL(*rules);
				}
			CleanupStack::PopAndDestroy(rules);
			}
		}
	}

void CAgnClientTlsTzRules::SystemTzZoneChangedL(RTz& aTzServer, NTzUpdate::TPropertyKeys aTzChangePropertyKey)
	{
	UpdateCurrentSystemTzRulesL(aTzServer);
	if(aTzChangePropertyKey == NTzUpdate::ETzRulesChange)
		{
		UpdateCacheL(aTzServer);
		}
	}

void CAgnClientTlsTzRules::AddReference(const CTzRules* aTzRules)
	{
	const TInt KRuleCount(iTzRules.Count());
	for (TInt i = 0; i < KRuleCount; i++)
		{
		if (iTzRules[i] == aTzRules)
			{
			iTzRulesReferences[i] = iTzRulesReferences[i] + 1;
			break;
			}
		}
	}
/***************************** Implementation of class CAgnTzChangeObserver ***************/
CAgnTzChangeObserver::CAgnTzChangeObserver(CAgnTlsProxy& aTlsProxy, NTzUpdate::TPropertyKeys aTzChangePropertyKey)
	:CActive(CActive::EPriorityStandard), iAgnTlsProxy(aTlsProxy), iTzChangePropertyKey(aTzChangePropertyKey)
	{
	CActiveScheduler::Add(this);
	}

void CAgnTzChangeObserver::ConstructL()
	{
	User::LeaveIfError(iTzChangeProperty.Attach(NTzUpdate::KPropertyCategory, iTzChangePropertyKey));
	iTzChangeProperty.Subscribe(iStatus);
	SetActive();
	}

CAgnTzChangeObserver* CAgnTzChangeObserver::NewL(CAgnTlsProxy& aTlsProxy, NTzUpdate::TPropertyKeys aTzChangePropertyKey)
	{
	CAgnTzChangeObserver* self = new (ELeave) CAgnTzChangeObserver(aTlsProxy, aTzChangePropertyKey);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CAgnTzChangeObserver::~CAgnTzChangeObserver()
	{
	Cancel();
	iTzChangeProperty.Close();
	}

void CAgnTzChangeObserver::RunL()
	{
	if (iStatus.Int() == KErrNone)
		{
		iAgnTlsProxy.HandleTzPropertyChangeL(iTzChangePropertyKey);
		iTzChangeProperty.Subscribe(iStatus);
		SetActive();
		}
	}

void CAgnTzChangeObserver::DoCancel()
	{
	iTzChangeProperty.Cancel();
	}



