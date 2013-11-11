/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/

#include <AccPolGenericID.h>

#include "irsystemeventobserverinterface.h"
#include "iraccessoryobserver.h" 
#include "irqlogger.h"

CIRAccessoryObserver::CIRAccessoryObserver(MIRHeadsetObserverInterface *aObserver) :
    CActive(CActive::EPriorityStandard),iObserver(aObserver)
{

}

void CIRAccessoryObserver::ConstructL()
{
    User::LeaveIfError(iAccessoryServer.Connect());

    User::LeaveIfError(iAccessoryModeSession.CreateSubSession(iAccessoryServer));
    User::LeaveIfError(iAccessoryModeSession.GetAccessoryMode(
            iAccPolAccessoryMode));

    CActiveScheduler::Add(this);

    iAccessoryModeSession.NotifyAccessoryModeChanged(iStatus, iAccPolAccessoryMode);
    SetActive();
}

CIRAccessoryObserver* CIRAccessoryObserver::NewL(MIRHeadsetObserverInterface *aObserver)
{
    CIRAccessoryObserver* self = new (ELeave) CIRAccessoryObserver(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CIRAccessoryObserver::~CIRAccessoryObserver()
{
    Cancel();
    iAccessoryModeSession.CloseSubSession();
    iAccessoryServer.Close();
}

TBool CIRAccessoryObserver::IsHeadsetConnected() const
{     
    TBool accessoryConnected;

    switch (iAccPolAccessoryMode.iAccessoryMode)
    {
    case EAccModeWiredHeadset:  
    case EAccModeLoopset:  
    case EAccModeHeadphones:
    {
        accessoryConnected = ETrue;
        break;
    }
    default:
    {
        accessoryConnected = EFalse;
        break;
    }
    }

    return accessoryConnected;
} 

void CIRAccessoryObserver::RunL()
{

    TRequestStatus status = iStatus;
    iAccessoryModeSession.NotifyAccessoryModeChanged(iStatus,
            iAccPolAccessoryMode);
    SetActive();

    if (status == KErrNone)
    {
        // Accessory mode may change when combined connection status changes 
        // or when audio routing status changes. 
        if (IsHeadsetConnected())
        {
            iObserver->headsetIsConnected();
        }
        else
        {
            iObserver->headsetIsDisconnected();
        }
    }
}

void CIRAccessoryObserver::DoCancel()
{
    iAccessoryModeSession.CancelNotifyAccessoryModeChanged();
}
