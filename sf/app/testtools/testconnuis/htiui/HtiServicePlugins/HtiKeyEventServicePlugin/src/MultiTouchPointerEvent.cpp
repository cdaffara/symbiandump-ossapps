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
* Description: Functional implementation of one touch(pointer sequence) event service.
*
*/

// INCLUDE FILES
#include "HtiKeyEventServicePlugin.h"
#include "MultiTouchPointerEvent.h"
#include "MultiTouchPointerEventHandler.h"

#include <HtiLogging.h>


// CONSTANTS
_LIT8( KErrorInternalFailure, "Internal pointer command failure" );
_LIT (KPrintInfoString, "PrintInfo event id=%d time=%d type=%d X=%d Y=%d Z=%d");

// ----------------------------------------------------------------------------
// CMultiTouchPointerUnit::NewL()
// ----------------------------------------------------------------------------
CMultiTouchPointerEvent* CMultiTouchPointerEvent::NewL(TUint8 aTouchNumber,
        CMultiTouchPointerEventHandler* aEventHandler)
    {
    HTI_LOG_FUNC_IN( "CMultiTouchPointerEvent::NewL" );
    CMultiTouchPointerEvent* self = new (ELeave) CMultiTouchPointerEvent(aTouchNumber, aEventHandler);
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CMultiTouchPointerEvent::NewL" );
    return self;
    }

// ----------------------------------------------------------------------------
// CMultiTouchPointerEvent::CMultiTouchPointerEvent()
// ----------------------------------------------------------------------------
CMultiTouchPointerEvent::CMultiTouchPointerEvent(TUint8 aTouchNumber,
        CMultiTouchPointerEventHandler *aEventHandler)
    : CActive(CActive::EPriorityStandard),
      iEventHandler(aEventHandler),
      iTouchNumber(aTouchNumber)
    {
    HTI_LOG_TEXT( "CMultiTouchPointerEvent constructor" );
    }

// ----------------------------------------------------------------------------
// CMultiTouchPointerEvent::~CMultiTouchPointerEvent()
// ----------------------------------------------------------------------------
CMultiTouchPointerEvent::~CMultiTouchPointerEvent()
    {
    HTI_LOG_TEXT( "CMultiTouchPointerEvent destructor" );
    Cancel();
    iTimer.Close();
	
    iTimeArray.ResetAndDestroy(); 
	iAdvPointerArray.ResetAndDestroy(); 
    }

// ----------------------------------------------------------------------------
// CMultiTouchPointerEvent::ConstructL()
// ----------------------------------------------------------------------------
void CMultiTouchPointerEvent::ConstructL()
    {
    HTI_LOG_TEXT( "CMultiTouchPointerEvent::ConstructL" );
    iTimeArray.Reset(); 

    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }


// ----------------------------------------------------------------------------
// CMultiTouchPointerEvent::StartTouch()
// ----------------------------------------------------------------------------
void CMultiTouchPointerEvent::StartTouch()
    {
    HTI_LOG_FUNC_IN( "CMultiTouchPointerEvent::StartTouch" );
    TBool bcontinue=ETrue;
    while (iTimeArray.Count()>0 && bcontinue)
        {
        TTimeIntervalMicroSeconds32* time=iTimeArray[0];
        iTimeArray.Remove(0);        
    	HTI_LOG_FORMAT( "Event time=%d ", time->Int() );    	
        if (time->Int()==0) 
            {
			// execute immediately            
			SimulatePointerEvent(); 
            }
        else
        	{
			// wait for specified time
			iTimer.After( iStatus, *time );
			SetActive();        
			bcontinue=EFalse;
        	}
        delete time;
        }
    
    if (iTimeArray.Count()==0)
    	{
		HTI_LOG_FORMAT( "Notify touch %d complete", iTouchNumber );
		iEventHandler->NotifyTouchComplete(iTouchNumber);  
    	}
    HTI_LOG_FUNC_OUT( "CMultiTouchPointerEvent::StartTouch" );
    }

// ----------------------------------------------------------------------------
// void CMultiTouchPointerEvent::InsertPointArrayL()
// ----------------------------------------------------------------------------
void CMultiTouchPointerEvent::InsertPointArrayL(TInt aX,TInt aY,TInt aZ,TRawEvent::TType aEventType)
    {

    TAdvancedPointer* point = new (ELeave) TAdvancedPointer;
    CleanupStack::PushL(point);
    iAdvPointerArray.AppendL(point);
    CleanupStack::Pop();

    point->X=aX;
    point->Y=aY;
    point->Z=aZ;
    point->EventType=aEventType;

    }

// ----------------------------------------------------------------------------
// void CMultiTouchPointerEvent::InsertTimeArrayL()
// ----------------------------------------------------------------------------
void CMultiTouchPointerEvent::InsertTimeArrayL(TTimeIntervalMicroSeconds32 aDelayTime)
    {

    TTimeIntervalMicroSeconds32* time=new (ELeave) TTimeIntervalMicroSeconds32(aDelayTime);
    iTimeArray.AppendL(time);

    }

// ----------------------------------------------------------------------------
// CMultiTouchPointerEvent::InterpolatePointL()
// ----------------------------------------------------------------------------
void CMultiTouchPointerEvent::InterpolatePointL(TInt aX1,TInt aY1, TInt aZ1, 
        TInt aX2,TInt aY2, TInt aZ2, TTimeIntervalMicroSeconds32 aDragTime, TInt aStepCount)
    {
    HTI_LOG_FUNC_IN( "CMultiTouchPointerEvent::InterpolatePointL" );
    TInt dx = (aX2-aX1)/aStepCount;
    TInt dy = (aY2-aY1)/aStepCount;  
    
    TTimeIntervalMicroSeconds32 dt = aDragTime.Int()/aStepCount;
    TInt X,Y,Z;
    for (TInt i=1;i<=aStepCount;i++)
        {         
        if (i<aStepCount)
            {
            X=aX1+i*dx;
            Y=aY1+i*dy;
            Z=aZ1;
            }
        else
            {
            X=aX2;
            Y=aY2;
            Z=aZ2;
            }             
        AddPointL(dt,X,Y,Z,TRawEvent::EPointerMove);        
        }
    
    HTI_LOG_FUNC_OUT( "CMultiTouchPointerEvent::InterpolatePointL" );
    }
// ----------------------------------------------------------------------------
// CMultiTouchPointerEvent::RunL()
// ----------------------------------------------------------------------------
void CMultiTouchPointerEvent::RunL()
    {
    HTI_LOG_FUNC_IN( "CMultiTouchPointerEvent::RunL" );
    SimulatePointerEvent();
    StartTouch();
    HTI_LOG_FUNC_OUT( "CMultiTouchPointerEvent::RunL" );
    }

// ----------------------------------------------------------------------------
// CMultiTouchPointerEvent::RunError()
// ----------------------------------------------------------------------------
TInt CMultiTouchPointerEvent::RunError( TInt aError )
    {
    HTI_LOG_FORMAT( "CMultiTouchPointerEvent::RunError %d", aError );

    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CMultiTouchPointerEvent::DoCancel()
// ----------------------------------------------------------------------------
void CMultiTouchPointerEvent::DoCancel()
    {
    HTI_LOG_FUNC_IN( "CMultiTouchPointerEvent::DoCancel" );
    iTimer.Cancel();
    HTI_LOG_FUNC_OUT( "CMultiTouchPointerEvent::DoCancel" );
    }

void CMultiTouchPointerEvent::PrintInfo()
	{
#ifdef __ENABLE_LOGGING__
	HTI_LOG_FUNC_IN( "CMultiTouchPointerEvent::PrintInfo" );
	
	HTI_LOG_FORMAT( "PrintInfo touch number=%d ", iTouchNumber );
	
	TInt size=iTimeArray.Count(); 
	HTI_LOG_FORMAT( "PrintInfo event array=%d ", size );	

	TBuf<255> buf;

	for (TInt i=0;i<size;i++)
		{
        buf.Format(KPrintInfoString, 
                i, iTimeArray[i]->Int(),iAdvPointerArray[i]->EventType ,
                iAdvPointerArray[i]->X ,iAdvPointerArray[i]->Y, iAdvPointerArray[i]->Z );
        HTI_LOG_DES(buf);
		}
	HTI_LOG_FUNC_OUT( "CMultiTouchPointerEvent::PrintInfo" );
#endif
	}
// ----------------------------------------------------------------------------
// CMultiTouchPointerEvent::SimulatePointerEvent()
// Sends the pointer event as a raw event.
// ----------------------------------------------------------------------------
void CMultiTouchPointerEvent::SimulatePointerEvent()
    {
    HTI_LOG_FUNC_IN( "CMultiTouchPointerEvent::SimulatePointerEvent" );    
       
	if (iAdvPointerArray.Count()>0)
        {
        TAdvancedPointer* pointer = iAdvPointerArray[0];    
        iAdvPointerArray.Remove(0);
        
        iEventHandler->SimulateEvent(iTouchNumber, pointer->X, pointer->Y, pointer->Z,pointer->EventType);        
        delete pointer;
        }

    HTI_LOG_FUNC_OUT( "CMultiTouchPointerEvent::SimulatePointerEvent" );

    }
void CMultiTouchPointerEvent::AddPointL(TTimeIntervalMicroSeconds32 aTime,
		TInt aX, TInt aY, TInt aZ, TRawEvent::TType aEventType)
	{
	InsertPointArrayL( aX, aY, aZ,aEventType);
	InsertTimeArrayL(aTime);
	}
// End of file
