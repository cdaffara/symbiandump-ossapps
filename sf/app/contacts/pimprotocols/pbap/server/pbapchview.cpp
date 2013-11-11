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

#include "pbapserver.h"
#include "pbapchview.h"

#include "btaccesshostlog.h"

//
// CPbapChView
//
/*static*/ CPbapChView* CPbapChView::NewL(CPbapLogWrapper& aLogClient,
										  CFolderNodeCallHistory::THistoryType aHistoryType,
										  MPbapChViewObserver& aObserver,
										  TLogFlags aFlags)
	{
	LOG_STATIC_FUNC
	CPbapChView* self = new(ELeave) CPbapChView(aLogClient, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL(aHistoryType, aFlags);
	CleanupStack::Pop(self);
	return self;
	}

CPbapChView::CPbapChView(CPbapLogWrapper& aLogClient, MPbapChViewObserver& aObserver)
	: CActive(EPriorityStandard), iLogClient(aLogClient), iObserver(aObserver)
	{
	LOG_FUNC
	CActiveScheduler::Add(this);
	}

void CPbapChView::ConstructL(CFolderNodeCallHistory::THistoryType aHistoryType,
							 TLogFlags aFlags)
	{
	LOG_FUNC
	
	// create log view
	iLogView = CPbapLogViewEvent::NewL(iLogClient, *this);
	
	// create filter
	iLogFilterList = new(ELeave) CLogFilterList;	
	CLogFilter* filter;

	if (aHistoryType == CFolderNodeCallHistory::ECombined)
		{
		// add all directions to filter list
		filter = CreateDirectionFilterLC(CFolderNodeCallHistory::EIncoming, aFlags);
		iLogFilterList->AppendL(filter);
		CleanupStack::Pop(filter); // ownership passed
	
		filter = CreateDirectionFilterLC(CFolderNodeCallHistory::EOutgoing, aFlags);
		iLogFilterList->AppendL(filter);
		CleanupStack::Pop(filter); // ownership passed

		filter = CreateDirectionFilterLC(CFolderNodeCallHistory::EMissed, aFlags);
		iLogFilterList->AppendL(filter);
		CleanupStack::Pop(filter); // ownership passed				
		}
	else
		{
		// add direction to filter list
		filter = CreateDirectionFilterLC(aHistoryType, aFlags);
		iLogFilterList->AppendL(filter);
		CleanupStack::Pop(filter); // ownership passed		
		}

	// filter the view asynchronously
	if (!iLogView->SetFilterL(*iLogFilterList, iStatus))
		{
		// no events in view so async request was not issued, just complete
		// ourselves instead
		TRequestStatus* status=&iStatus;
		User::RequestComplete(status, KErrNone);
		
		// if no events then the view is not completely initialised, e.g. if 
		// a new event is added to the view it is not included in the view 
		// until SetFilterL is called again and completes asynchronously
		iRefreshView = ETrue;
		}
		
	SetActive();
	}

CPbapChView::~CPbapChView()
	{
	LOG_FUNC
	
	Cancel();
	
	if (iLogFilterList)
		{
		iLogFilterList->ResetAndDestroy();
		}
	delete iLogFilterList;
	delete iLogView;	
	}
	
void CPbapChView::RunL()
	{
	LOG_FUNC

	// Call History view is now ready
	iObserver.CallHistoryViewReady(iStatus.Int());
	}

void CPbapChView::DoCancel()
	{
	LOG_FUNC

	iLogView->Cancel();		
	}
	
CLogFilter* CPbapChView::CreateDirectionFilterLC(CFolderNodeCallHistory::THistoryType aHistoryType, TLogFlags aFlags)
	{
	LOG_FUNC
	
	TLogString direction;
	switch (aHistoryType)
		{
		case CFolderNodeCallHistory::EIncoming:
			User::LeaveIfError(iLogClient.GetString(direction, R_LOG_DIR_IN));
			break;
		case CFolderNodeCallHistory::EOutgoing:
			User::LeaveIfError(iLogClient.GetString(direction, R_LOG_DIR_OUT));		
			break;
		case CFolderNodeCallHistory::EMissed:
			User::LeaveIfError(iLogClient.GetString(direction, R_LOG_DIR_MISSED));
			break;
		default:
			break;
		}
		
	CLogFilter* filter = CLogFilter::NewL();
	CleanupStack::PushL(filter);
	filter->SetEventType(KLogCallEventTypeUid);
	filter->SetDirection(direction);
	
	if (aFlags != KLogNullFlags)
		{
		filter->SetFlags(aFlags);		
		}
	return filter; // ownership transferred
	}

CPbapLogViewEvent* CPbapChView::LogView()
	{
	LOG_FUNC
	return iLogView;
	}

// from MPbapLogEngViewObserver
void CPbapChView::PbapLogEngViewChangeEventAddedL(TLogId, TInt, TInt, TInt)
	{
	LOG_FUNC

	// inform our observer that a change has been made to Call History db.
	// if we need to refresh the view then the view is not ready to use.
	iObserver.CallHistoryChangeNotification(!iRefreshView);
	
	// refresh view if required
	if (iRefreshView)
		{
		// filter the view asynchronously
		if (!iLogView->SetFilterL(*iLogFilterList, iStatus))
			{
			// this should not happen, indicates no events in view but we have just been
			// given notification that a new event has been added. Complete view with
			// error and then observer should delete this object and try to create it
			// again later
			__ASSERT_DEBUG(EFalse, Panic(EPbapChViewInconsistency));
			
			TRequestStatus* status=&iStatus;
			User::RequestComplete(status, KErrCorrupt);
			}

		iRefreshView = EFalse;
		SetActive();		
		}
	}
	
void CPbapChView::PbapLogEngViewChangeEventChangedL(TLogId, TInt, TInt, TInt)
	{
	LOG_FUNC
	
	// if a change is made to an event then this will not change the corresponding
	// handle and therefore we do not need to notify the observer of a change
	}
	
void CPbapChView::PbapLogEngViewChangeEventDeletedL(TLogId, TInt, TInt, TInt)
	{
	LOG_FUNC
	
	// inform our observer that a change has been made to Call History db
	iObserver.CallHistoryChangeNotification();
	}

void CPbapChView::PbapLogEngViewChangeEventL(TUid, TInt, TInt, TInt)
	{
	LOG_FUNC
	
	// inform our observer that a change has been made to Call History db
	iObserver.CallHistoryChangeNotification();	
	}
