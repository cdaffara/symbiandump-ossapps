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
* Description:  MDE media monitor utility
*
*/


#include "mdsmediamonitor.h"
#include <mdesession.h>
#include <mdelogiccondition.h>
#include <mdeconstants.h>
#include "mdeobjectqueuemanager.h"
#include "common.h"
#include "harvesterserverlogger.h"


namespace
    {
// -----------------------------------------------------------------------------
// CCPixIndexerUtils::GetCpixAction()
// -----------------------------------------------------------------------------
//
    TCPixActionType GetCpixAction(TObserverNotificationType aType)
        {
        TCPixActionType action( ECPixAddAction );//Initializing to silence compiler warning. 
        switch(aType)
            {
            case ENotifyAdd:
                action = ECPixAddAction;
                break;
            case ENotifyModify:
                action = ECPixUpdateAction;
                break;
            case ENotifyRemove:
                action = ECPixRemoveAction;
                break;
            default:
                break;
            }
        return action;
        }
    };

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::NewL()
// -----------------------------------------------------------------------------
//
CMdsMediaMonitor* CMdsMediaMonitor::NewL(CMdESession* aSession,CMdeObjectQueueManager* aJobQueue)
    {
    CMdsMediaMonitor* self = CMdsMediaMonitor::NewLC(aSession,aJobQueue);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::NewLC()
// -----------------------------------------------------------------------------
//
CMdsMediaMonitor* CMdsMediaMonitor::NewLC(CMdESession* aSession,CMdeObjectQueueManager* aJobQueue)
    {
    CMdsMediaMonitor* self = new (ELeave) CMdsMediaMonitor(aSession,aJobQueue);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::~CMdsMediaMonitor()
// -----------------------------------------------------------------------------
//
CMdsMediaMonitor::~CMdsMediaMonitor()
    {
    StopMonitoringL();
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::CMdsMediaMonitor()
// -----------------------------------------------------------------------------
//
CMdsMediaMonitor::CMdsMediaMonitor(CMdESession* aSession,
                                   CMdeObjectQueueManager* aJobQueue):
                                   iMdsSession(aSession),
                                   iMdeObjectQueueManager(aJobQueue)
    {
    
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::ConstructL()
// -----------------------------------------------------------------------------
//
void CMdsMediaMonitor::ConstructL()
    {
    
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::StartMonitoringL()
// -----------------------------------------------------------------------------
//
void CMdsMediaMonitor::StartMonitoringL(const TDesC& aNameSpaceDef)
    {
    CMdELogicCondition* condition = CMdELogicCondition::NewL( ELogicConditionOperatorAnd );
    CleanupStack::PushL( condition );
    CMdENamespaceDef& defaultNamespace = iMdsSession->GetDefaultNamespaceDefL();
    CMdEObjectDef& audioObjDef = defaultNamespace.GetObjectDefL(aNameSpaceDef );
    condition->AddObjectConditionL( audioObjDef );
    //Start receiveing events for Audio objects
    iMdsSession->AddObjectObserverL( *this, condition);
    iMdsSession->AddObjectPresentObserverL( *this );
    CleanupStack::Pop(condition);
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::StopMonitoringL()
// -----------------------------------------------------------------------------
//
void CMdsMediaMonitor::StopMonitoringL()
    {
    if(iMdsSession)
        {
        iMdsSession->RemoveObjectObserverL(*this,&iMdsSession->GetDefaultNamespaceDefL());
        iMdsSession->RemoveObjectPresentObserverL(*this);
        }
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::HandleObjectNotification()
// -----------------------------------------------------------------------------
//
void CMdsMediaMonitor::HandleObjectNotification(CMdESession& aSession, 
                            TObserverNotificationType aType,
                            const RArray<TItemId>& aObjectIdArray)
    {
    /*Bhuvi: Observed when MMC card removed and inserted back the Add event
    Comes we have to reharvest if in case media is MMC card this way the
    search results are acqurate and also index is synchronized*/
    CPIXLOGSTRING2("CMdsMediaMonitor::HandleObjectNotification ActionType = %d",aType);
    TCPixActionType action = GetCpixAction(aType);
    //handle items here push it to Queue manager
    for (TInt i=0;i<aObjectIdArray.Count();i++)
        {
        TItemId objectId = aObjectIdArray[i];
        TRAP_IGNORE( iMdeObjectQueueManager->AddMdeItemToQueueL(objectId,action) );
        }
    }

// -----------------------------------------------------------------------------
// CCPixIndexerUtils::HandleObjectPresentNotification()
// -----------------------------------------------------------------------------
//
void CMdsMediaMonitor::HandleObjectPresentNotification(CMdESession& aSession, 
                TBool aPresent, const RArray<TItemId>& aObjectIdArray)
    {
    CPIXLOGSTRING2("CMdsMediaMonitor::HandleObjectPresentNotification aPresent = %d",aPresent);
    //Handle only Presence true events 
    if(aPresent)
        {
        CPIXLOGSTRING("CMdsMediaMonitor::HandleObjectPresentNotification Add Actions");
        // Add in the queue for Addition
        for (TInt i=0;i<aObjectIdArray.Count();i++)
            {
            TItemId objectId = aObjectIdArray[i];
            TRAP_IGNORE( iMdeObjectQueueManager->AddMdeItemToQueueL(objectId,ECPixAddAction) );
            }
        }
    //Dont handle presence false notification for performance as it is only for MMC items
    /*
    else
        {
        CPIXLOGSTRING("CMdsMediaMonitor::HandleObjectPresentNotification remove Actions");
        // Add in the queue for removal
        for (TInt i=0;i<aObjectIdArray.Count();i++)
            {
            TItemId objectId = aObjectIdArray[i];
            iMdeObjectQueueManager->AddMdeItemToQueueL(objectId,ECPixRemoveAction);
            }
        }
    */
    }

//End of File
