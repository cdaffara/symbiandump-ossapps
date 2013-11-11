/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares CUPTimer class
*
*/


#include <e32svr.h>
#include <upnpitem.h>

#include "cuptimer.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CUPTimer* CUPTimer::NewL()
	{
    CUPTimer* self = NewLC();
    CleanupStack::Pop(self);
    return self;
	}

// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CUPTimer* CUPTimer::NewLC()
	{   
    CUPTimer* self = new (ELeave) CUPTimer();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
	}

// ---------------------------------------------------------------------------
// ~CUPTimer
// ---------------------------------------------------------------------------
//
CUPTimer::~CUPTimer()
	{
	
    Cancel();  

	if (iTimeOutTimer)
		{
			delete iTimeOutTimer;
			iTimeOutTimer = NULL;		
		}
	}

// ---------------------------------------------------------------------------
// SetActiveNow
// ---------------------------------------------------------------------------
//
void CUPTimer::SetActiveNow()
	{
    TRequestStatus* statusPtr = &iStatus;
    User::RequestComplete (statusPtr, KErrNone);
	}

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
void CUPTimer::RunL()
	{    
	iTimeOutTimer->Cancel();
    iObserver->TimerExpiredL(iItemToShare);
	}

// ---------------------------------------------------------------------------
// RunL
// ---------------------------------------------------------------------------
//
TInt CUPTimer::RunError(TInt aError)
	{
	Cancel();
	
	return aError;
	}

// ---------------------------------------------------------------------------
// DoCancel
// ---------------------------------------------------------------------------
//
void CUPTimer::DoCancel()
	{   		
	if (iTimeOutTimer->IsActive())
		{
		iTimeOutTimer->Cancel();	
		}
	return;
	}

// ---------------------------------------------------------------------------
// WaitUntilTimeExpired
// ---------------------------------------------------------------------------
//
void CUPTimer::WaitUntilTimeExpired(MUPTimerObserver* aObserver,
										TTimeIntervalMicroSeconds32 aTimeOut,
										CUpnpItem* aItemToShare)//,
	//									TRequestStatus &aStatus)
	{
     
    /* This might be a little bit dangerous but server knows when to start timer */
    Cancel();
    
    iItemToShare = aItemToShare;
	
	if(!IsActive())
		{
		SetActive();
		}
	
		
	iObserver = aObserver;
	
	// Start the time out timer
	TRAPD(trapError, iTimeOutTimer = CPeriodic::NewL(EPriorityBackground));
	if (trapError != KErrNone)
		{
	
		}

	TCallBack callback(CUPTimer::TimedOut,(TAny*)(this));
	iTimeOutTimer->Start(aTimeOut, aTimeOut, callback);
                     
	iStatus = KRequestPending;
	}


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//	
void CUPTimer::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

// ---------------------------------------------------------------------------
// CUPTimer
// ---------------------------------------------------------------------------
//	
CUPTimer::CUPTimer() : CActive(EPriorityNormal)
	{

	}	

// ---------------------------------------------------------------------------
// CancelTimer
// ---------------------------------------------------------------------------
//	

void CUPTimer::CancelTimer()
	{
   	Cancel();
	}

// ---------------------------------------------------------------------------
// TimedOut
// ---------------------------------------------------------------------------
//	
TInt CUPTimer::TimedOut(TAny* aPtr)
	{    
	CUPTimer* obj = reinterpret_cast<CUPTimer*>(aPtr);
	if (obj)
		{
		obj->SetActiveNow();		
		}

	return KErrNone;	
	}

//  End of File
