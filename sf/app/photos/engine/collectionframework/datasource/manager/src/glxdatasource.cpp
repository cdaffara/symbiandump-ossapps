/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Implementation of the Data Source Class.
*
*/





#include "glxdatasource.h"

#include <glxsingletonstore.h>
#include <glxpanic.h>
#include <glxtracer.h>

#include "glxdatasourcetask.h"
#include "mglxdatasourceupdateobserver.h"
#include "mglxdatasourcerequestobserver.h"

// ---------------------------------------------------------------------------
// CDataSourceArrayOwner::InstanceL
// ---------------------------------------------------------------------------
//
CDataSourceArrayOwner* CDataSourceArrayOwner::InstanceL() 
	{
    TRACER("CDataSourceArrayOwner::InstanceL()");
    return CGlxSingletonStore::InstanceL(&NewL);
    }
    
// ---------------------------------------------------------------------------
// CDataSourceArrayOwner::Close
// ---------------------------------------------------------------------------
//
void CDataSourceArrayOwner::Close() 
   {
    TRACER("void CDataSourceArrayOwner::Close()");
   CGlxSingletonStore::Close(this);
   }
  
// ---------------------------------------------------------------------------
// CDataSourceArrayOwner::Array
// ---------------------------------------------------------------------------
//
RPointerArray<CGlxDataSource>& CDataSourceArrayOwner::Array()
	{
    TRACER("CDataSourceArrayOwner::Array()");
    return iDataSources;
	}

// ---------------------------------------------------------------------------
// CDataSourceArrayOwner::NewL
// ---------------------------------------------------------------------------
//
CDataSourceArrayOwner* CDataSourceArrayOwner::NewL() 
   {
    TRACER("CDataSourceArrayOwner* CDataSourceArrayOwner::NewL()");
   return new (ELeave) CDataSourceArrayOwner();
   }

// ---------------------------------------------------------------------------
// CDataSourceArrayOwner::~CDataSourceArrayOwner
// ---------------------------------------------------------------------------
//
CDataSourceArrayOwner::~CDataSourceArrayOwner()
   {
    TRACER("CDataSourceArrayOwner::~CDataSourceArrayOwner()");
   iDataSources.Close();
   }    

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxDataSource::CGlxDataSource()
    : CActive(EPriorityStandard)
    {
    TRACER("EXPORT_C CGlxDataSource::CGlxDataSource()");
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// MGlxDataSource::OpenDataSourceL
// ---------------------------------------------------------------------------
//
EXPORT_C MGlxDataSource* MGlxDataSource::OpenDataSourceL(const TUid& aDataSource, MGlxDataSourceUpdateObserver& aObserver)
	{
    TRACER("MGlxDataSource* MGlxDataSource::OpenDataSourceL()");
	CDataSourceArrayOwner* dsArrayOwner = CDataSourceArrayOwner::InstanceL();
	CleanupClosePushL(*dsArrayOwner);
	RPointerArray<CGlxDataSource>& dataSoruceArray = dsArrayOwner->Array();
	CGlxDataSource* dataSource = NULL;
	if (dataSoruceArray.Count())
		{
		for (TInt i = 0;i < dataSoruceArray.Count(); i++)
			{
			if (dataSoruceArray[i]->Uid() == aDataSource)
				{
				dataSource = dataSoruceArray[i];
				dataSource->AddObserverL(aObserver);
				break;
				}
			}
		}
	if (!dataSource)
		{
		REComSession ecomSession = REComSession::OpenL();
		CleanupClosePushL(ecomSession);
		TUid dtorKey;
		dataSource = reinterpret_cast<CGlxDataSource*>(
           REComSession::CreateImplementationL(aDataSource, dtorKey));
        CleanupStack::PushL(dataSource);
        dataSoruceArray.AppendL(dataSource);
        dataSource->SetUid(aDataSource);
        dataSource->SetDtorKey(dtorKey);
        dataSource->AddObserverL(aObserver);
        CleanupStack::Pop(dataSource);
        dataSource->SetEComSession(ecomSession);
        CleanupStack::Pop(&ecomSession);
        dataSource->SetDataSourceArrayOwner(dsArrayOwner);
        CleanupStack::Pop(dsArrayOwner);
		}
	else
		{
		CleanupStack::PopAndDestroy(dsArrayOwner);
		}
	
    return dataSource;
	}

// ---------------------------------------------------------------------------
// CGlxDataSource::Close
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxDataSource::Close(MGlxDataSourceUpdateObserver& aObserver)
	{
    TRACER("void CGlxDataSource::Close(MGlxDataSourceUpdateObserver& aObserver)");
	TInt observer = iDataSourceObservers.Find(&aObserver);
	__ASSERT_DEBUG(observer !=KErrNotFound, Panic(EGlxPanicLogicError)); // Closing non-existant observer
	iDataSourceObservers.Remove(observer);
	if (iDataSourceObservers.Count() == 0)
		{
		RPointerArray<CGlxDataSource>& dataSourceArray = iDataSourceArrayOwner->Array();
		TInt dataSource = dataSourceArray.Find(this);
		__ASSERT_DEBUG(dataSource !=KErrNotFound, Panic(EGlxPanicLogicError)); // Closing non-existant observer
		dataSourceArray.Remove(dataSource);
		REComSession ecomSession = iEComSession;
		TInt count = dataSourceArray.Count();
		delete this;
		if (count == 0)
			{
			ecomSession.Close();
			}
		}	
	}

// ---------------------------------------------------------------------------
// CGlxDataSource::~CGlxDataSource
// ---------------------------------------------------------------------------
//
EXPORT_C CGlxDataSource::~CGlxDataSource()
	{
    TRACER("EXPORT_C CGlxDataSource::~CGlxDataSource()");
    Cancel();
	iDataSourceObservers.Close();
    REComSession::DestroyedImplementation(iDtorKey);
    iTasks.ResetAndDestroy();
	iDataSourceArrayOwner->Close();
	}

// ---------------------------------------------------------------------------
// CGlxDataSource::AddObserverL
// ---------------------------------------------------------------------------
//
void CGlxDataSource::AddObserverL(MGlxDataSourceUpdateObserver& aObserver)
	{
    TRACER("void CGlxDataSource::AddObserverL(MGlxDataSourceUpdateObserver& aObserver)");
	iDataSourceObservers.AppendL(&aObserver);
	}


// ---------------------------------------------------------------------------
// CGlxDataSource::RunL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxDataSource::RunL()
    {
    TRACER("void CGlxDataSource::RunL()");
    TAction action = static_cast<TAction>(iStatus.Int());
    
    switch (action)
        {
        case ECleanupAndExecute:
            {
            delete iTasks[0];
            iTasks.Remove(0);
            TaskCompletedL();
            }
        // Don't break - fall through 
        case EExecuteOnly:
            {
            if ( (iTasks.Count() > 0) && ( iDataSourceReady || !iTasks[0]->IsDataSourceNeeded() ) )
                {
                TaskStartedL();
                iTasks[0]->ExecuteRequestL(); // Execute the oldest request
                }
            }
        break;
        }
    }

// ---------------------------------------------------------------------------
// CGlxDataSource::DoCancel
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxDataSource::DoCancel()
    {
    TRACER("void CGlxDataSource::DoCancel()");
    // CActive::Cancel() will wait for the request to complete.
    // It is not necessary to take any action here.
    }

// ---------------------------------------------------------------------------
// CGlxDataSource::RunError
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CGlxDataSource::RunError(TInt aError)
    {
    TRACER("TInt CGlxDataSource::RunError(TInt aError)");
    __ASSERT_DEBUG(iTasks.Count() > 0, Panic(EGlxPanicLogicError));
    iTasks[0]->HandleRequestComplete(aError);
    
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CGlxDataSource::CompleteSelf
// ---------------------------------------------------------------------------
//
void CGlxDataSource::CompleteSelf(TAction aAction)
    {
    TRACER("void CGlxDataSource::CompleteSelf(TAction aAction)");
    TRequestStatus* status=&iStatus;
    User::RequestComplete(status, aAction);
    SetActive();    
    }

// ---------------------------------------------------------------------------
// CGlxDataSource::DataSourceRequestL
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxDataSource::DataSourceRequestL(CGlxRequest* aRequest, MGlxDataSourceRequestObserver& aObserver)
	{
    TRACER("void CGlxDataSource::DataSourceRequestL()");
	CGlxDataSourceTask* newRequest = CreateTaskL(aRequest, aObserver);
	iTasks.AppendL(newRequest);
	TryStartTask();
	}

// ---------------------------------------------------------------------------
// CGlxDataSource::TryStartTask
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxDataSource::TryStartTask(TBool aFirstStart)
	{
    TRACER("void CGlxDataSource::TryStartTask(TBool aFirstStart)");
	if ( ( aFirstStart ? iTasks.Count() >= 1 : iTasks.Count() == 1 ) && !IsActive() )
    // There is one request i.e. it was just added by the call to AppendL() above and we are not waiting 
    // for a an existing request to complete. If we are waiting for a request to complete then the request 
    // we have just added will run later.
		{
        CompleteSelf(EExecuteOnly);
		}
	}
	
// ---------------------------------------------------------------------------
// CGlxDataSource::HandleTaskComplete
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxDataSource::HandleTaskComplete(CGlxDataSourceTask* aRequest)
	{
    TRACER("void CGlxDataSource::HandleTaskComplete(CGlxDataSourceTask* aRequest)");
	__ASSERT_DEBUG(iTasks.Find(aRequest) == 0, Panic( EGlxPanicLogicError )); // The only request that should be executing is the 0th request in the array.
    CompleteSelf(ECleanupAndExecute);
	}

// ---------------------------------------------------------------------------
// CGlxDataSource::CancelRequest
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxDataSource::CancelRequest(MGlxDataSourceRequestObserver& aObserver)
	{
    TRACER("void CGlxDataSource::CancelRequest(MGlxDataSourceRequestObserver& aObserver)");    
    for (TInt i = 0; i < iTasks.Count(); i++)
		{
		if (iTasks[i]->ObserverMatch(aObserver))
			{            
            if (i == 0) // Cancel the active request
                {
                if (!IsActive())
                    {
                    // We can only cancel the request if it is currently executing. The active object being
                    // active indicates that the request has not yet started or has already completed.
                    iTasks[0]->CancelRequest();
                    }
                Cancel();
                CompleteSelf(ECleanupAndExecute);
                }
            else // The request is waiting to be executed - just remove it from the queue.
                {
                delete iTasks[i];
                iTasks.Remove(i);
                }
            break;
			}
		}
	}

// ---------------------------------------------------------------------------
// CGlxDataSource::BroadcastMessage
// ---------------------------------------------------------------------------
//
EXPORT_C void CGlxDataSource::BroadcastMessage(CMPXMessage& aMessage)
	{
    TRACER("void CGlxDataSource::BroadcastMessage(CMPXMessage& aMessage)");
	__ASSERT_DEBUG(iDataSourceObservers.Count(), Panic(EGlxPanicLogicError));
	// There should always be at least 1 observer because they are passed in
	// when the object is instatiated.
	iDataSourceObservers[0]->HandleMessage(aMessage);
	}
