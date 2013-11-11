/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MDE harvester utility class
 *
*/


#include "mdeharvester.h"

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::NewL()
// -----------------------------------------------------------------------------
//
CMdeHarvester* CMdeHarvester::NewL(CMdESession* aSession,
                                   MdeMEdiaObserver* aObserver,
                                   CMdeObjectQueueManager* aObjJobQueue)
    {
    CMdeHarvester* self = CMdeHarvester::NewLC(aSession,aObserver,aObjJobQueue);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::NewLC()
// -----------------------------------------------------------------------------
//
CMdeHarvester* CMdeHarvester::NewLC(CMdESession* aSession,
                                    MdeMEdiaObserver* aObserver,
                                    CMdeObjectQueueManager* aObjJobQueue)
    {
    CMdeHarvester* self = new (ELeave) CMdeHarvester();
    CleanupStack::PushL(self);
    self->ConstructL(aSession,aObserver,aObjJobQueue);
    return self;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::~CMdeHarvester()
// -----------------------------------------------------------------------------
//
CMdeHarvester::~CMdeHarvester()
   {
   Cancel();
   delete iWait;
   delete iQuery;
   }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::ConstructL()
// -----------------------------------------------------------------------------
//
void CMdeHarvester::ConstructL(CMdESession* aSession,
                               MdeMEdiaObserver* aObserver,
                               CMdeObjectQueueManager* aObjJobQueue)
    {
    iSession = aSession;
    iObserver = aObserver;
    iJobQueue = aObjJobQueue;
    iWait = new (ELeave) CActiveSchedulerWait;
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::CMdeHarvester()
// -----------------------------------------------------------------------------
//
CMdeHarvester::CMdeHarvester():CActive(CActive::EPriorityStandard)
    {
    iCurrentIndex = 0;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::RunL()
// -----------------------------------------------------------------------------
//
//from CActive
void CMdeHarvester::RunL()
    {
    if(KErrNone == iStatus.Int())
      {
      HandleNextRequestL();
      }
    else
      {
      iObserver->HarvestingCompletedL(/*iStatus.Int()*/);//Introduce Error
      }
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::DoCancel()
// -----------------------------------------------------------------------------
//
void CMdeHarvester::DoCancel()
    {
    
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::RunError()
// -----------------------------------------------------------------------------
//
TInt CMdeHarvester::RunError()
    {
    //@TODO: handle next items here incrementing count index 
    return KErrNone;
    }
   

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::handleQueryNewResults()
// -----------------------------------------------------------------------------
//
void CMdeHarvester::HandleQueryNewResults(CMdEQuery& aQuery,
                                          TInt aFirstNewItemIndex,
                                          TInt aNewItemCount)
    {
    
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::HandleQueryCOmpleted()
// -----------------------------------------------------------------------------
//
void CMdeHarvester::HandleQueryCompleted(CMdEQuery& aQuery, TInt aError)
    {
    if(aError !=KErrNone)
        {
        TInt count = aQuery.Count();
        }
    if(iWait->IsStarted()) //Complete in Handle
        iWait->AsyncStop();
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::DoHarvestL()
// -----------------------------------------------------------------------------
//
void CMdeHarvester::DoHarvesetL(const TDesC& aNameSpaceDef)
    {
    iCurrentIndex = 0;
    QueryAllMediaObjectsL(aNameSpaceDef);
    }
   

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::QueryAllMediaObjectsL()
// -----------------------------------------------------------------------------
//
void CMdeHarvester::QueryAllMediaObjectsL(const TDesC& aNameSpaceDef)
    {
    //delete iQuery;
	iQuery->Delete(iQuery);
	iQuery = NULL;
    CMdENamespaceDef& defaultNamespaceDef = iSession->GetDefaultNamespaceDefL();
    CMdEObjectDef& audioObjDef = defaultNamespaceDef.GetObjectDefL(aNameSpaceDef);    
    // query objects with object definition "Audio" "video" "images" etc..
    iQuery = iSession->NewObjectQueryL( defaultNamespaceDef, audioObjDef, this );
    iQuery->FindL();
    iWait->Start();  //Complete in Handle
    TInt count = iQuery->Count();
    CompleteSelf();    
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::HandleNextRequestL()
// -----------------------------------------------------------------------------
//
void CMdeHarvester::HandleNextRequestL()
    {
    if(iCurrentIndex < iQuery->Count())
        {
        //Get the Object ID and pass to ObjectQueue manager
        CMdEObject& object = static_cast<CMdEObject&>( iQuery->ResultItem( iCurrentIndex ) );
        //@TODO: ResultId does'nt work need to check with MDS 
        //iJobQueue->AddMdeItemToQueueL(iQuery->ResultId(iCurrentIndex),ECPixAddAction);
        iJobQueue->AddMdeItemToQueueL(object.Id(),ECPixAddAction);
        iCurrentIndex++;
        CompleteSelf();
        }
    else
        {
        iObserver->HarvestingCompletedL();
        }
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::CompleteSelf()
// -----------------------------------------------------------------------------
//
void CMdeHarvester::CompleteSelf()
    {
    // trigger our RunL
    TRequestStatus*  caller;
    caller = &iStatus;
    iStatus = KRequestPending;
    User::RequestComplete(caller, KErrNone);
    SetActive();    
    }

//End of life
