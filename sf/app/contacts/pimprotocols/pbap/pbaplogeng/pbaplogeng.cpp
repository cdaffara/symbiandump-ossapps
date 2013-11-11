// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "pbaplogeng.h"

#include "btaccesshostlog.h"

//
// CPbapLogWrapper
//

/*static*/ EXPORT_C CPbapLogWrapper* CPbapLogWrapper::NewL(RFs& aFs)
	{
	LOG_STATIC_FUNC
	CPbapLogWrapper* self = new(ELeave) CPbapLogWrapper;
	CleanupStack::PushL(self);
	self->ConstructL(aFs);
	CleanupStack::Pop(self);
	return self;
	}

CPbapLogWrapper::CPbapLogWrapper()
	{
	LOG_FUNC
	}

void CPbapLogWrapper::ConstructL(RFs& aFs)
	{
	LOG_FUNC
	
	// create the CLogWrapper to forward requests to.
	iLogWrapper = CLogWrapper::NewL(aFs);

	if (iLogWrapper->ClientAvailable())
		{
		iLogClient = static_cast<CLogClient*>(&iLogWrapper->Log());
		}
	else
		{
		// if no client is available then we have no access to the call history
		// information. The PBAP server will continue anyway but only giving
		// access to the contact information.
		User::Leave(KErrNotSupported);
		}
	}

EXPORT_C  CPbapLogWrapper::~CPbapLogWrapper()
	{
	LOG_FUNC
	
	iPbapObservers.Close();
	
	delete iLogWrapper;	
	}

EXPORT_C  TBool CPbapLogWrapper::ClientAvailable() const
	{
	LOG_FUNC	
	return iLogWrapper->ClientAvailable();
	}
	
EXPORT_C  TInt CPbapLogWrapper::GetString(TDes &aString, TInt aId) const
	{
	LOG_FUNC
	return iLogClient->GetString(aString, aId);
	}

CLogClient& CPbapLogWrapper::LogClient()
	{
	LOG_FUNC
	return  *iLogClient;
	}

TInt CPbapLogWrapper::AddPbapObserver(MPbapLogEngViewObserver& aObserver)
	{
	LOG_FUNC
	TInt rerr = iPbapObservers.Append(&aObserver);
	
	if ((iPbapObservers.Count() == 1) && (rerr == KErrNone))
		{
		// first observer was just successfully added, register for global events
		TRAP(rerr, iLogClient->SetGlobalChangeObserverL(this));
		}
		
	return rerr;
	}
	
void CPbapLogWrapper::RemovePbapObserver(MPbapLogEngViewObserver& aObserver)
	{
	LOG_FUNC
	
	for (TInt i = iPbapObservers.Count() - 1; i >= 0; i--)
		{
		if (iPbapObservers[i] == &aObserver)
			{
			iPbapObservers.Remove(i); // don't delete - it's not ours!
			}
		}

	if (iPbapObservers.Count() == 0)
		{
		// no more observers, deregister for global events
		TRAP_IGNORE(iLogClient->SetGlobalChangeObserverL(NULL));
		}
	}

// from MLogClientChangeObserver
void CPbapLogWrapper::HandleLogClientChangeEventL(TUid aChangeType, TInt aParam1, TInt aParam2, TInt aParam3)
	{
	LOG_FUNC
	
	// Notify all of the PBAP Observers and turn the LogEng Global events into PBAP view events
	for (TInt i = iPbapObservers.Count() - 1; i >= 0; i--)
		{
		iPbapObservers[i]->PbapLogEngViewChangeEventL(aChangeType, aParam1, aParam2, aParam3);
		}
	}

//
// CPbapLogViewEvent
//

/*static*/ EXPORT_C CPbapLogViewEvent* CPbapLogViewEvent::NewL(CPbapLogWrapper& aLogWrap)
	{
	LOG_STATIC_FUNC
	CPbapLogViewEvent* self = new(ELeave) CPbapLogViewEvent(aLogWrap);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*static*/ EXPORT_C CPbapLogViewEvent* CPbapLogViewEvent::NewL(CPbapLogWrapper& aLogWrap, MPbapLogEngViewObserver& aObserver)
	{
	LOG_STATIC_FUNC
	CPbapLogViewEvent* self = new(ELeave) CPbapLogViewEvent(aLogWrap);
	CleanupStack::PushL(self);
	self->ConstructL(&aObserver);
	CleanupStack::Pop(self);
	return self;
	}

CPbapLogViewEvent::CPbapLogViewEvent(CPbapLogWrapper& aLogWrap) : iWrapper(aLogWrap)
	{
	LOG_FUNC
	}
		
void CPbapLogViewEvent::ConstructL(MPbapLogEngViewObserver* aObserver)
	{
	LOG_FUNC

	// create the CLogViewEvent to forward requests to.
	if (aObserver != NULL)
		{
		// store the observer and add it receive global logeng events
		User::LeaveIfError(iWrapper.AddPbapObserver(*aObserver));
		iPbapObserver = aObserver;

		iViewEvent = CLogViewEvent::NewL(iWrapper.LogClient(), *this);
		}
	else
		{
		iViewEvent = CLogViewEvent::NewL(iWrapper.LogClient());
		}
	}

EXPORT_C  CPbapLogViewEvent::~CPbapLogViewEvent()
	{
	LOG_FUNC
	
	if (iPbapObserver)
		{
		// remove observer from receiving global events
		iWrapper.RemovePbapObserver(*iPbapObserver);
		}

	delete iViewEvent;
	}

EXPORT_C TBool CPbapLogViewEvent::SetFilterL(const CLogFilterList& aFilterList, TRequestStatus& aStatus)
	{
	LOG_FUNC
	return iViewEvent->SetFilterL(aFilterList, aStatus);
	}
	
EXPORT_C TBool CPbapLogViewEvent::SetFilterL(const CLogFilter& aFilter, TRequestStatus& aStatus)
	{
	LOG_FUNC
	return iViewEvent->SetFilterL(aFilter, aStatus);
	}
	
EXPORT_C const CLogEvent& CPbapLogViewEvent::Event() const
	{
	LOG_FUNC
	return iViewEvent->Event();
	}
	
EXPORT_C TBool CPbapLogViewEvent::FirstL(TRequestStatus& aStatus)
	{
	LOG_FUNC
	return iViewEvent->FirstL(aStatus);	
	}
	
EXPORT_C TBool CPbapLogViewEvent::NextL(TRequestStatus& aStatus)
	{
	LOG_FUNC
	return iViewEvent->NextL(aStatus);	
	}

EXPORT_C TInt CPbapLogViewEvent::CountL()
	{
	LOG_FUNC
	return iViewEvent->CountL();	
	}

EXPORT_C void CPbapLogViewEvent::Cancel()
	{
	LOG_FUNC
	iViewEvent->Cancel();
	}

// from MLogViewChangeObserver, pass all events to the PBAP view event observer
void CPbapLogViewEvent::HandleLogViewChangeEventAddedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
	LOG_FUNC

	iPbapObserver->PbapLogEngViewChangeEventAddedL(aId, aViewIndex, aChangeIndex, aTotalChangeCount);
	}
	
void CPbapLogViewEvent::HandleLogViewChangeEventChangedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
	LOG_FUNC

	iPbapObserver->PbapLogEngViewChangeEventChangedL(aId, aViewIndex, aChangeIndex, aTotalChangeCount);
	}
	
void CPbapLogViewEvent::HandleLogViewChangeEventDeletedL(TLogId aId, TInt aViewIndex, TInt aChangeIndex, TInt aTotalChangeCount)
	{
	LOG_FUNC

	iPbapObserver->PbapLogEngViewChangeEventDeletedL(aId, aViewIndex, aChangeIndex, aTotalChangeCount);
	}
