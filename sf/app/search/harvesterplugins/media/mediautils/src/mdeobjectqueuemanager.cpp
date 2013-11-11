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
* Description:  MDE object queuemanager for indexing scheduling
 *
*/

#include "mdeobjectqueuemanager.h"
#include "harvesterserverlogger.h"

// DEFINES
#define INDEXING_QUEUE_MAX 300 // Maximum number of objects in the queue
#define INDEXING_DELAY 3000000 // Nano seconds to delay the monitored object
#define MAX_RETRY_COUNT    3   //maximum number of retry on not found object


// -----------------------------------------------------------------------------
// CCPixIndexerUtils::NewL()
// -----------------------------------------------------------------------------
//
CMdeObjectQueueManager* CMdeObjectQueueManager::NewL(MMediaObjectHandler* aHandler)
    {
    CMdeObjectQueueManager* self = CMdeObjectQueueManager::NewLC(aHandler);
    CleanupStack::Pop();
    return self;
    }
// -----------------------------------------------------------------------------
// CCPixIndexerUtils::NewLC()
// -----------------------------------------------------------------------------
//
CMdeObjectQueueManager* CMdeObjectQueueManager::NewLC(MMediaObjectHandler* aHandler)
    {
    CMdeObjectQueueManager* self = new (ELeave) CMdeObjectQueueManager(aHandler);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::~CMdeObjectQueueManager()
// -----------------------------------------------------------------------------
//
CMdeObjectQueueManager::~CMdeObjectQueueManager()
    {
    Cancel();
    iTimer.Close();
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::CMdeObjectQueueManager()
// -----------------------------------------------------------------------------
//
CMdeObjectQueueManager::CMdeObjectQueueManager(MMediaObjectHandler* aHandler):
                        CActive(CActive::EPriorityStandard),
                        iState(EStateNone),
                        iMdeObjectHandler(aHandler)
    {
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::ConstructL()
// -----------------------------------------------------------------------------
//
void CMdeObjectQueueManager::ConstructL()
    {
    CActiveScheduler::Add(this);
    User::LeaveIfError(iTimer.CreateLocal());
    iHState = EStateResume;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::AddMdeItemToQueueL()
// -----------------------------------------------------------------------------
//
void CMdeObjectQueueManager::AddMdeItemToQueueL( TItemId aMsgId, 
                                                 TCPixActionType aActionType)
    {
    OverWriteOrAddToQueueL(aMsgId,aActionType);    
    ActivateAO();
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::OverWriteOrAddToQueueL()
// -----------------------------------------------------------------------------
//
void CMdeObjectQueueManager::OverWriteOrAddToQueueL( TItemId aObjId, 
                                                     TCPixActionType aActionType)
    {
    // Overwrite or add the index to the queue
    TMdeActionRecord object;
    object.iObjectId = aObjId;
    object.iAction = aActionType;
    for (TInt i=0; i<iJobQueue.Count(); i++)
    {
        if (iJobQueue[i].iObjectId ==aObjId)
        {
            // Older version found
            iJobQueue[i] = object;
            return;
        }
    }
    
    // older not found, append
    iJobQueue.AppendL(object);    
    }

void CMdeObjectQueueManager::ActivateAO()
    {
    // Proceed only if the plugin is not in pause state 
    if ( iHState == EStateResume )
        {
        // Check the size against maximum queue size
        if (iJobQueue.Count() > INDEXING_QUEUE_MAX)
            {
            // Maximum is exceeded, force the write immediately
            if (iState == EStateWaiting)
                {
                iTimer.Cancel(); // RunL will be called with iStatus of KErrCancelled
                }
            else if (iState == EStateNone)
                {
                SetActive();
                TRequestStatus* status = &iStatus;
                User::RequestComplete(status, KErrNone); // RunL will be called with iStatus of KErrNone
                }
            }
        else
            {
            // Maximum is not exceeded, keep waiting
            if (iState == EStateNone)
                {
                iState = EStateWaiting;
                iTimer.After(iStatus, INDEXING_DELAY); // Wait 5 seconds before putting this to index
                SetActive();
                }
            }
        }
    }
// -----------------------------------------------------------------------------
// CCPixIndexerUtils::RunL()
// -----------------------------------------------------------------------------
//
void CMdeObjectQueueManager::RunL()
    {
    CPIXLOGSTRING("START CMdeObjectQueueManager::RunL");
    
    if ( iHState == EStatePause )
        {
        iState = EStateNone;
        CPIXLOGSTRING("END CMdeObjectQueueManager::RunL as Harvester in Pause state");  
        return;          
        }
    while (iJobQueue.Count()>0 )
        {
        TMdeActionRecord object = iJobQueue[0];
        //iJobQueue.Remove(0);
        //Let the indexer handle this object TRAP it as it can leave
        TRAPD(err,iMdeObjectHandler->HandleMdeItemL(object.iObjectId, object.iAction));
        /*Process the Item for three time maximum if in case not able to be found from
          MDS*/
        if(KErrNone == err)
            {
            iJobQueue.Remove(0);
            }
        else if(KErrNotFound == err && object.iRetryCount < MAX_RETRY_COUNT)
                {
                iJobQueue.Remove(0); //remove existing and add new one 
                object.iRetryCount++;
                iJobQueue.AppendL(object);
                CPIXLOGSTRING3("CMdeObjectQueueManager::RunL HandleMdeItemL Item Not found err = %d, iRetry = %d",err,object.iRetryCount); 
                }
        else
            {
            iJobQueue.Remove(0);  //retry count exceeded remove it
            }
        }
#ifdef __PERFORMANCE_DATA
        iMdeObjectHandler->UpdateLogL();
#endif
        // Everything is indexed no need to be waiting anymore
        iState = EStateNone;    
        CPIXLOGSTRING("END CMdeObjectQueueManager::RunL");    
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::DoCancel()
// -----------------------------------------------------------------------------
//
void CMdeObjectQueueManager::DoCancel()
    {
    iTimer.Cancel();
    iState = EStateNone; 
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::RunError()
// -----------------------------------------------------------------------------
//
TInt CMdeObjectQueueManager::RunError()
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::PauseL()
// -----------------------------------------------------------------------------
//
void CMdeObjectQueueManager::PauseL()
    {
    iHState = EStatePause;
    }
// -----------------------------------------------------------------------------
// CCPixIndexerUtils::ResumeL()
// -----------------------------------------------------------------------------
//
void CMdeObjectQueueManager::ResumeL()
    {
    iHState = EStateResume;
    ActivateAO();       
    }

//End of file
