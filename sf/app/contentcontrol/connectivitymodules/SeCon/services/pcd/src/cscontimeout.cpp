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
* Description:  CSconTimeOut implementation
*
*/


#include "cscontimeout.h"

CSconTimeOut::CSconTimeOut( MTimeOutObserver& aTimeOutObserver ) :
    CActive(EPriorityStandard), // Standard priority
    iTimeOutObserver(aTimeOutObserver)
    {
    }


CSconTimeOut* CSconTimeOut::NewL( MTimeOutObserver& aTimeOutObserver )
    {
    CSconTimeOut* self = new (ELeave) CSconTimeOut( aTimeOutObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CSconTimeOut::ConstructL()
    {
    User::LeaveIfError(iTimer.CreateLocal()); // Initialize timer
    CActiveScheduler::Add(this); // Add to scheduler
    }

CSconTimeOut::~CSconTimeOut()
    {
    Cancel(); // Cancel any request, if outstanding
    iTimer.Close(); // Destroy the RTimer object
    }

void CSconTimeOut::DoCancel()
    {
    iTimer.Cancel();
    }

void CSconTimeOut::Start(TTimeIntervalMicroSeconds32 aDelay)
    {
    Cancel(); // Cancel any request, just to be sure
    iTimer.After(iStatus, aDelay); // Set for later
    SetActive(); // Tell scheduler a request is active
    }

void CSconTimeOut::RunL()
    {
    iTimeOutObserver.TimeOut();
    }

TInt CSconTimeOut::RunError(TInt aError)
    {
    return aError;
    }
