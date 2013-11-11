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
* Description:  Functional implementation of advanced pointer multitouch service
*
*/

// INCLUDE FILES
#include "HtiKeyEventServicePlugin.h"
#include "MultiTouchPointerEventHandler.h"
#include "MultiTouchPointerEvent.h"

#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>

// CONSTANTS


// ----------------------------------------------------------------------------
// CMultiTouchPointerEventHandler::NewL()
// ----------------------------------------------------------------------------
CMultiTouchPointerEventHandler* CMultiTouchPointerEventHandler::NewL(MHtiMultiTouchObserver& aObserver)
    {
    HTI_LOG_FUNC_IN( "CMultiTouchPointerEventHandler::NewL" );
    CMultiTouchPointerEventHandler* self = new (ELeave) CMultiTouchPointerEventHandler(aObserver);
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CMultiTouchPointerEventHandler::NewL" );
    return self;
    }

// ----------------------------------------------------------------------------
// CMultiTouchPointerEventHandler::CMultiTouchPointerEventHandler()
// ----------------------------------------------------------------------------
CMultiTouchPointerEventHandler::CMultiTouchPointerEventHandler(MHtiMultiTouchObserver& aObserver)
	:iFinishedCount(0),	iObserver(aObserver)
    {
    HTI_LOG_TEXT( "CMultiTouchPointerEventHandler constructor" );
    }

// ----------------------------------------------------------------------------
// CMultiTouchPointerEventHandler::~CMultiTouchPointerEventHandler()
// ----------------------------------------------------------------------------
CMultiTouchPointerEventHandler::~CMultiTouchPointerEventHandler()
    {
    HTI_LOG_TEXT( "CMultiTouchPointerEventHandler destructor" );    
    Clear();
    iWsSession.Close();
    }

// ----------------------------------------------------------------------------
// CMultiTouchPointerEventHandler::ConstructL()
// ----------------------------------------------------------------------------
void CMultiTouchPointerEventHandler::ConstructL()
    {
    HTI_LOG_TEXT( "CMultiTouchPointerEventHandler::ConstructL" ); 
    User::LeaveIfError( iWsSession.Connect() );
    }
// ----------------------------------------------------------------------------
// CMultiTouchPointerEventHandler::Clear()
// ----------------------------------------------------------------------------
void CMultiTouchPointerEventHandler::Clear()
	{
	HTI_LOG_TEXT( "CMultiTouchPointerEventHandler::Clear" ); 
	iTouchPointerArray.ResetAndDestroy();
	iFinishedCount=0;
	}
// ----------------------------------------------------------------------------
// CMultiTouchPointerEventHandler::NotifyTouchComplete()
// ----------------------------------------------------------------------------
void CMultiTouchPointerEventHandler::NotifyTouchComplete(TUint8 aTouchNumber)
    {
    HTI_LOG_FUNC_IN("CMultiTouchPointerEventHandler::NotifyTouchComplete");
    
    iFinishedCount++;
    HTI_LOG_FORMAT( "touch number %d finished", aTouchNumber);
    HTI_LOG_FORMAT( "%d touch finished", iFinishedCount);
    
    if(iTouchPointerArray.Count() == iFinishedCount)
    	{
		// notify observer
		iObserver.NotifyMultiTouchComplete();
    	}
    
    HTI_LOG_FUNC_OUT("CMultiTouchPointerEventHandler::NotifyTouchComplete");
    }
// ----------------------------------------------------------------------------
// CMultiTouchPointerEventHandler::ParsePoint()
// ----------------------------------------------------------------------------
TBool CMultiTouchPointerEventHandler::ParsePoint( const TDesC8& aData, TInt& aoffset,
		TInt& aX, TInt& aY, TInt& aZ, 
		TTimeIntervalMicroSeconds32& aWait, TTimeIntervalMicroSeconds32& aHold )
    {
    HTI_LOG_FUNC_IN( "CMultiTouchPointerEventHandler::ParsePoint" );
    
    TInt dataLength = aData.Length();
    if ( dataLength-aoffset < 14 ) 
        return EFalse ;
    
    aX = aData[aoffset] + ( aData[aoffset+1] << 8 );
    aoffset += 2;
    
    aY = aData[aoffset] + ( aData[aoffset+1] << 8 );
    aoffset += 2;

    aZ = aData[aoffset] + ( aData[aoffset+1] << 8 );
    aoffset += 2;

    aWait = (aData[aoffset] + ( aData[aoffset+1] << 8 )
                + ( aData[aoffset+2] << 16 ) + (aData[aoffset+3] << 24) ) * 1000;
    aoffset += 4;      
    
    aHold = ( aData[aoffset] + ( aData[aoffset+1] << 8 )
                + ( aData[aoffset+2] << 16 ) + (aData[aoffset+3] << 24) ) * 1000;
    aoffset += 4;
    
    HTI_LOG_FUNC_OUT( "CMultiTouchPointerEventHandler::ParsePoint" );
    return ETrue;
    }
// ----------------------------------------------------------------------------
// CMultiTouchPointerEventHandler::ParseMove()
// ----------------------------------------------------------------------------
TBool CMultiTouchPointerEventHandler::ParseMove( const TDesC8& aData, TInt& aoffset,
		TTimeIntervalMicroSeconds32& aDragTime, TInt& aStepCount )
    {
    HTI_LOG_FUNC_IN( "CMultiTouchPointerEventHandler::ParseMove" );
    
    TInt dataLength = aData.Length();
    if ( dataLength-aoffset < 6 ) 
        return EFalse ;
    
    aDragTime = ( aData[aoffset] + ( aData[aoffset+1] << 8 )
				+ ( aData[aoffset+2] << 16 ) + (aData[aoffset+3] << 24) ) * 1000;
    aoffset += 4;
    
	aStepCount = aData[aoffset] + ( aData[aoffset+1] << 8 );
	aoffset += 2;
    
    HTI_LOG_FUNC_OUT( "CMultiTouchPointerEventHandler::ParseMove" );
    return ETrue;
    }
// ----------------------------------------------------------------------------
// CMultiTouchPointerEventHandler::HandleMultiTouchL()
// ----------------------------------------------------------------------------
TBool CMultiTouchPointerEventHandler::HandleMultiTouchL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CMultiTouchPointerEventHandler::HandleMultiTouchL" );
    TBool validparameter=EFalse;
    
    TInt dataLength = aData.Length();
    if ( dataLength < 1 )
        return validparameter;
	
    // Parse the parameters - correct length is already verified
	TInt offset = 0;

    TInt touchCount = aData[offset];
    offset += 1;
    HTI_LOG_FORMAT( "Number of touches: %d", touchCount );
    if (touchCount<=0)
    	return validparameter;

    for(TInt i=0; i<touchCount; i++)
        {
        //check valid data length
        if(dataLength-offset < 3)
            return validparameter;
        
        TInt pointerNumber = aData[offset];
        offset += 1;
        HTI_LOG_FORMAT( "Pointer Number %d", pointerNumber );
        
        TInt pointerCount = aData[offset] + ( aData[offset+1] << 8 );
        offset += 2;
        HTI_LOG_FORMAT( "Number of points: %d", pointerCount );
        
        if (pointerCount == 0 )
            return validparameter;
        
        CMultiTouchPointerEvent* touch = CMultiTouchPointerEvent::NewL(pointerNumber,this);        
        CleanupStack::PushL(touch);
        iTouchPointerArray.AppendL(touch);
        CleanupStack::Pop();
        
        TInt lastX, lastY, lastZ, stepCount; 
        TTimeIntervalMicroSeconds32 dragTime;
        
        for (TInt j=0;j<pointerCount;j++)
        	{
			// read point
			TInt X1, Y1, Z1; 
			TTimeIntervalMicroSeconds32 waitTime, holdTime;
			if (!ParsePoint( aData, offset,X1, Y1, Z1, waitTime, holdTime))
				return validparameter;
			
			HTI_LOG_FORMAT( "X coord down = %d", X1 );
			HTI_LOG_FORMAT( "Y coord down = %d", Y1 );
			HTI_LOG_FORMAT( "Z coord down = %d", Z1 );
			HTI_LOG_FORMAT( "Event wait time = %d", waitTime.Int() );   
			HTI_LOG_FORMAT( "Point holding time = %d", holdTime.Int() );
			
			if ( waitTime.Int() < 0 || holdTime.Int() < 0 )
				return validparameter;
			
			// add move event from last point to this point 
			if (j!=0)
				{
				HTI_LOG_FORMAT( "Point dragging time = %d", dragTime.Int() );
				HTI_LOG_FORMAT( "Step count between two points = %d", stepCount );
				touch->InterpolatePointL(lastX, lastY, lastZ,X1,Y1,Z1,dragTime,stepCount);
				}				
			
			// set last point
			lastX=X1;
			lastY=Y1;
			lastZ=Z1;
			
			// wait event
			touch->AddPointL(waitTime,X1,Y1,Z1,TRawEvent::EButton1Down);
			
			// hold event
			touch->AddPointL(holdTime, X1,Y1,Z1,TRawEvent::EPointerMove);
			
			if (j!=pointerCount-1)
				{
				// not last point
				if (!ParseMove(aData, offset, dragTime, stepCount))
					return validparameter;				
				if (dragTime.Int() < 0 || stepCount <= 0)
					return validparameter;        				
				}
			else
				{
				// last point
				touch->AddPointL(0,X1,Y1,Z1,TRawEvent::EButton1Up);
				}
        	}  
        
            touch->PrintInfo();

        } 

    
    for (TInt i=0;i<touchCount;i++)
        iTouchPointerArray[i]->StartTouch();
    
    validparameter=ETrue;
    
    HTI_LOG_FUNC_OUT( "CMultiTouchPointerEventHandler::HandleMultiTouchL" );
    return validparameter;
    }	

void CMultiTouchPointerEventHandler::SimulateEvent(TUint8 aTouchNumber, 
		TInt aX, TInt aY, TInt aZ, TRawEvent::TType aEventType)
    {
    HTI_LOG_FUNC_IN( "CMultiTouchPointerEventHandler::SimulateEvent" );
    
	TRawEvent rawEvent;
	rawEvent.SetPointerNumber( aTouchNumber ); 
	rawEvent.Set( aEventType, aX,  aY,  aZ);   

	HTI_LOG_FORMAT( "SimulatePointer touch number=%d ", aTouchNumber );
	HTI_LOG_FORMAT( "SimulatePointer event=%d ", aEventType );
	HTI_LOG_FORMAT( "SimulatePointer X=%d ", aX );
	HTI_LOG_FORMAT( "SimulatePointer Y=%d ", aY );
	HTI_LOG_FORMAT( "SimulatePointer Z=%d", aZ );
	
	iWsSession.SimulateRawEvent( rawEvent );
	iWsSession.Flush();
	
    HTI_LOG_FUNC_OUT( "CMultiTouchPointerEventHandler::SimulateEvent" );

    }

// End of file
