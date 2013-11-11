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

#include "iralarmobserver.h"
#include "irsystemeventobserverinterface.h"

CIRAlarmObserver* CIRAlarmObserver::NewL(MIRAlarmObserverInterface* aObserver)
{

    CIRAlarmObserver* self = CIRAlarmObserver::NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
}


CIRAlarmObserver* CIRAlarmObserver::NewLC(MIRAlarmObserverInterface* aObserver)
{     
    CIRAlarmObserver* self = new (ELeave) CIRAlarmObserver;
    CleanupStack::PushL(self);
    self->ConstructL(aObserver);     
    return self;
}


CIRAlarmObserver::~CIRAlarmObserver()
{    
    Cancel();
    iAlarmSession.Close();    
}


void CIRAlarmObserver::Start()
{         
    if (!IsActive())
    {
        iAlarmSession.NotifyChange(iStatus, iAlarmId);
        SetActive();
    }    
}


void CIRAlarmObserver::RunL()
{          
    if (EAlarmChangeEventSoundPlaying == iStatus.Int())
    {
        if( iObserver )
        {
            iObserver->alarmStarted();
        }                
    }    
    else if (EAlarmChangeEventSoundStopped == iStatus.Int())
    {
        if( iObserver )
        {
            iObserver->alarmStopped();
        }        
    }
     
    Start();    
}


void CIRAlarmObserver::DoCancel()
{     
    if (IsActive())
    {
        iAlarmSession.NotifyChangeCancel();
    }    
}


TInt CIRAlarmObserver::RunError(TInt aError)
{    
    return aError;
}


void CIRAlarmObserver::ConstructL(MIRAlarmObserverInterface* aObserver)
{     
    iObserver = aObserver;  
    User::LeaveIfError(iAlarmSession.Connect());
    CActiveScheduler::Add(this);   
}


CIRAlarmObserver::CIRAlarmObserver() :
    CActive(CActive::EPriorityStandard)
{
     
}

