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
* Description:  Functional implementation of pointer event service
*
*/

// INCLUDE FILES
#include "HtiKeyEventServicePlugin.h"
#include "PointerEventHandler.h"


#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>


// CONSTANTS
_LIT8( KErrorMissingCommand, "Command was not given - message was empty" );
_LIT8( KErrorServiceNotReady, "Service is busy - possibly executing long running pointer events" );
_LIT8( KErrorInvalidParameters, "Command parameters not valid" );
_LIT8( KErrorUnrecognizedCommand, "Unrecognized command" );
_LIT8( KErrorInternalFailure, "Internal pointer command failure" );
_LIT8( KErrorGetHALData, "Can not get the HAL data" );

const TInt KTapCmdLength = 10;
const TInt KDragMultiCmdMinLength = 14;
const TInt KSinglePointerCmdLength = 4;
const TInt KAdvancedTapScreenCmdMinLength = 16;
const TInt KPinchZoomCmdMinLength = 30;


// ----------------------------------------------------------------------------
// CPointerEventHandler::NewL()
// ----------------------------------------------------------------------------
CPointerEventHandler* CPointerEventHandler::NewL()
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::NewL" );
    CPointerEventHandler* self = new (ELeave) CPointerEventHandler();
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CPointerEventHandler::Done" );
    return self;
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::CPointerEventHandler()
// ----------------------------------------------------------------------------
CPointerEventHandler::CPointerEventHandler()
    : CActive( CActive::EPriorityStandard ), iReady( ETrue ), iCommand( 0 ), iMultiTouchHandler(NULL),
      iState( EPointerUp )
    {
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::~CPointerEventHandler()
// ----------------------------------------------------------------------------
CPointerEventHandler::~CPointerEventHandler()
    {
    HTI_LOG_TEXT( "CPointerEventHandler destroy" );
    Cancel();
    iTimer.Close();
    iWsSession.Close();
    if ( iCoords )
        {
        iCoords->Close();
        }
    delete iCoords;
    delete iMultiTouchHandler;
	
	iAdvancedPointers.ResetAndDestroy();
	iDelayArray.ResetAndDestroy();  
	iAdvPointerMoveArray.ResetAndDestroy(); 
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::ConstructL()
// ----------------------------------------------------------------------------
void CPointerEventHandler::ConstructL()
    {
    HTI_LOG_TEXT( "CPointerEventHandler::ConstructL" );
    User::LeaveIfError( iWsSession.Connect() );
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// void CPointerEventHandler::SetDispatcher()
// ----------------------------------------------------------------------------
void CPointerEventHandler::SetDispatcher( MHtiDispatcher* aDispatcher )
    {
    iDispatcher = aDispatcher;
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::RunL()
// ----------------------------------------------------------------------------
void CPointerEventHandler::RunL()
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::RunL" );

    if ( iCommand == ETapScreen || iCommand == EAdvancedTapScreen )
        {
        ChangePointerStateL();
        }

    else if ( iCommand == ETapAndDrag  && iState == EPointerDown )
        {
        PointerMove();
        PointerUp();
        SendOkMsgL();
        iReady = ETrue;
        }

    else if ( iCommand == ETapAndDragMultipoint )
        {
        if ( iState == EPointerUp )  // Starting a new line
            {
            PointerDown();
            iTapCount--;
            iTimer.After( iStatus, iEventDelay );
            SetActive();
            }
        else
            {
            MoveToNextPointL(); // Continuing current line
            }
        }
    
    else if ( iCommand == EPinchZoom )
        {
        PointerMove();
        if ( !AdvancedStartDelay())
            {
            PointerUp();
            SendOkMsgL();
            iAdvPointerMoveArray.ResetAndDestroy(); 
            iReady = ETrue;
            }
        }		
    
    else if ( iCommand == EMultiTouch )
        {
        SendOkMsgL();
        iMultiTouchHandler->Clear();
        iReady = ETrue;
        }		
    HTI_LOG_FUNC_OUT( "CPointerEventHandler::RunL" );
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::RunError()
// ----------------------------------------------------------------------------
TInt CPointerEventHandler::RunError( TInt aError )
    {
    HTI_LOG_FORMAT( "CPointerEventHandler::RunError %d", aError );

    TInt dispatchResult = KErrNone;
    TRAP( dispatchResult, SendErrorMessageL( aError, KErrorInternalFailure ) );
    if ( dispatchResult != KErrNone )
        {
        HTI_LOG_FORMAT( "CPointerEventHandler::RunError: Failed to send error report to framework: %d.", dispatchResult );
        }
    iReady = ETrue;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::DoCancel()
// ----------------------------------------------------------------------------
void CPointerEventHandler::DoCancel()
    {
    iTimer.Cancel();
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::ProcessMessageL()
// ----------------------------------------------------------------------------
void CPointerEventHandler::ProcessMessageL( const TDesC8& aMessage,
    THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN("CPointerEventHandler::ProcessMessageL");
    HTI_LOG_FORMAT("Msg len: %d.", aMessage.Length());

    if ( !iReady )
        {
        SendErrorMessageL( ENotReadyCommand, KErrorServiceNotReady );
        return;
        }
    if ( aMessage.Length() == 0 )
        {
        SendErrorMessageL( EMissingCommand, KErrorMissingCommand );
        return;
        }

    User::ResetInactivityTime();
    iCommand = aMessage.Ptr()[0];
    switch ( iCommand )
        {
        case ETapScreen:
            HandleTapScreenL( aMessage.Right( aMessage.Length() - 1 ) );
            break;
        case ETapAndDrag:
            HandleTapAndDragL( aMessage.Right( aMessage.Length() - 1 ) );
            break;
        case ETapAndDragMultipoint:
            HandleTapAndDragMultipointL(
                aMessage.Right( aMessage.Length() - 1 ) );
            break;
        case EPressPointerDown:
        case ELiftPointerUp:
            HandlePointerDownOrUpL( aMessage.Right( aMessage.Length() - 1 ) );
            break;
		case EAdvancedTapScreen:
		    HandleAdvancedTapScreenL( aMessage.Right( aMessage.Length() - 1 ) );
            break;		
		case EPinchZoom: 
		    HandlePinchZoomL( aMessage.Right( aMessage.Length() - 1 ) );
            break;
		case EMultiTouch:
			HandleMultiTouchL(aMessage.Right( aMessage.Length() - 1 ));
			break;
        default:
            SendErrorMessageL( EUnrecognizedCommand,
                KErrorUnrecognizedCommand );
            break;
        }

    HTI_LOG_FUNC_OUT( "CPointerEventHandler::ProcessMessageL: Done" );
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::HandleTapScreenL()
// Handles single or multiple taps to one point.
// ----------------------------------------------------------------------------
void CPointerEventHandler::HandleTapScreenL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::HandleTapScreenL" );

    if ( aData.Length() != KTapCmdLength )
        {
        SendErrorMessageL( EInvalidParameters, KErrorInvalidParameters );
        return;
        }

    // Parse the parameters - correct length is already verified
    TInt offset = 0;
    iX = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "X coord = %d", iX );
    iY = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "Y coord = %d", iY );
    iEventDelay = ( aData[offset] + ( aData[offset+1] << 8 ) ) * 1000;
    offset += 2;
    HTI_LOG_FORMAT( "Time to hold down = %d", iEventDelay.Int() );
    iTapCount = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "Tap count = %d", iTapCount );
    iActionDelay = ( aData[offset] + ( aData[offset+1] << 8 ) ) * 1000;
    HTI_LOG_FORMAT( "Pause between taps = %d", iActionDelay.Int() );

    // Start tapping
    iReady = EFalse;
    ChangePointerStateL();

    HTI_LOG_FUNC_OUT( "CPointerEventHandler::HandleTapScreenL" );
    }
	


// ----------------------------------------------------------------------------
// CPointerEventHandler::HandleTapAndDragL()
// Handles a single drag and drop with straight line.
// ----------------------------------------------------------------------------
void CPointerEventHandler::HandleTapAndDragL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::HandleTapAndDragL" );

    if ( aData.Length() != KTapCmdLength )
        {
        SendErrorMessageL( EInvalidParameters, KErrorInvalidParameters );
        return;
        }

    TInt offset = 0;
    iX = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "X coord down = %d", iX );
    iY = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "Y coord down = %d", iY );
    TInt xUp = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "X coord up = %d", xUp );
    TInt yUp = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "Y coord up = %d", yUp );
    iEventDelay = ( aData[offset] + ( aData[offset+1] << 8 ) ) * 1000;
    HTI_LOG_FORMAT( "Drag time = %d", iEventDelay.Int() );

    iReady = EFalse;
    PointerDown();
    iX = xUp;
    iY = yUp;
    iTimer.After( iStatus, iEventDelay );
    SetActive();

    HTI_LOG_FUNC_OUT( "CPointerEventHandler::HandleTapAndDragL" );
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::HandleTapAndDragMultipointL()
// Handles drawing one or more curvy lines.
// ----------------------------------------------------------------------------
void CPointerEventHandler::HandleTapAndDragMultipointL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::HandleTapAndDragMultipointL" );

    TInt dataLength = aData.Length();
    if ( dataLength < KDragMultiCmdMinLength || dataLength % 2 != 0 )
        {
        SendErrorMessageL( EInvalidParameters, KErrorInvalidParameters );
        return;
        }

    TInt offset = 0;
    iEventDelay = ( aData[offset] + ( aData[offset+1] << 8 ) ) * 1000;
    offset += 2;
    HTI_LOG_FORMAT( "Time between events = %d", iEventDelay.Int() );
    iActionDelay = ( aData[offset] + ( aData[offset+1] << 8 ) ) * 1000;
    offset += 2;
    HTI_LOG_FORMAT( "Pause between lines = %d", iActionDelay.Int() );

    if ( iCoords == NULL )
        {
        iCoords = new ( ELeave ) RArray<TInt>();
        }
    iCoords->Reset();

    // Read integers from aData to the array, all integers are 2 bytes
    while ( offset < dataLength )
        {
        iCoords->AppendL( aData[offset] + ( aData[offset + 1] << 8 ) );
        offset += 2;
        }

    iReady = EFalse;
    iTapCount = ( *iCoords )[0];
    iCoords->Remove( 0 );
    iX = ( *iCoords )[0];
    iCoords->Remove( 0 );
    iY = ( *iCoords )[0];
    iCoords->Remove( 0 );
    HTI_LOG_FORMAT( "Point count for first line = %d", iTapCount );
    PointerDown();
    iTapCount--;
    iTimer.After( iStatus, iEventDelay );
    SetActive();

    HTI_LOG_FUNC_OUT( "CPointerEventHandler::HandleTapAndDragMultipointL" );
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::HandlePointerDownOrUpL()
// Handles pushing pointer down in one point or lifting it up.
// This is synchronous operation and sends OK message right after the event
// is simulated.
// ----------------------------------------------------------------------------
void CPointerEventHandler::HandlePointerDownOrUpL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::HandlePointerDownOrUpL" );
    if ( aData.Length() != KSinglePointerCmdLength )
        {
        SendErrorMessageL( EInvalidParameters, KErrorInvalidParameters );
        return;
        }

    // Parse the parameters - correct length is already verified
    TInt offset = 0;
    iX = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "X coord = %d", iX );
    iY = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "Y coord = %d", iY );

    if ( iCommand == EPressPointerDown )
        {
        PointerDown();
        }
    else
        {
        PointerUp();
        }
    SendOkMsgL();
    HTI_LOG_FUNC_OUT( "CPointerEventHandler::HandlePointerDownOrUpL" );
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::ChangePointerStateL()
// Decides whether to do  "pointer down" or "pointer up" event next or if
// operation is complete.
// This function is used by ETapScreen command.
// ----------------------------------------------------------------------------
void CPointerEventHandler::ChangePointerStateL()
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::ChangePointerStateL" );
    HTI_LOG_FORMAT( "Taps remaining = %d", iTapCount );
    if ( iTapCount < 1 )
        {
        SendOkMsgL();
        iReady = ETrue;
        return;
        }

    if ( iState == EPointerUp )
        {
        PointerDown();
        iTimer.After( iStatus, iEventDelay );
        SetActive();
        }
    else if ( iState == EPointerDown )
        {
        PointerUp();
        iTapCount--; // one tap done
        if ( iTapCount > 0 ) // do we continue tapping?
            {
            iTimer.After( iStatus, iActionDelay );
            SetActive();
            }
        else
            {
            SendOkMsgL();  // tapping done
            iReady = ETrue;
            }
        }
    HTI_LOG_FUNC_OUT( "CPointerEventHandler::ChangePointerStateL" );
    }
	
	// ----------------------------------------------------------------------------
	
// ----------------------------------------------------------------------------
// CPointerEventHandler::HandleAdvancedTapScreenL()
// ****cherry.
// ----------------------------------------------------------------------------

void CPointerEventHandler::HandleAdvancedTapScreenL( const TDesC8& aData )
    {
    
    HTI_LOG_FUNC_IN( "CPointerEventHandler::HandleAdvancedTapScreenL" );

    if ( aData.Length() < KAdvancedTapScreenCmdMinLength ) //KAdvancedTapScreenCmdMinLength needs to be defined
        {
        SendErrorMessageL( EInvalidParameters, KErrorInvalidParameters );
        return;
        }
	TInt dataLength = aData.Length();
	
    // Parse the parameters - correct length is already verified
	TInt offset = 0;
	
	iEventDelay = ( aData[offset] + ( aData[offset+1] << 8 ) ) * 1000;
    offset += 2;
    HTI_LOG_FORMAT( "Time to hold down = %d", iEventDelay.Int() );
    iTapCount = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "Tap count = %d", iTapCount );
    iActionDelay = ( aData[offset] + ( aData[offset+1] << 8 ) ) * 1000;
	offset += 2;
    HTI_LOG_FORMAT( "Pause between taps = %d", iActionDelay.Int() );		
	TInt pointerCount = aData[offset] + ( aData[offset+1] << 8 );
	offset += 2;
	HTI_LOG_FORMAT( "Pointer Count = %d", pointerCount );
	
    iAdvancedPointers.ResetAndDestroy();

    // Read integers from aData to the array, all integers are 2 bytes
    for ( TInt i = 0; i < pointerCount; i++ )
        {
		TAdvancedPointer* advancedPointer = new (ELeave) TAdvancedPointer;
		
		advancedPointer->PointerNum = aData[offset];
		offset += 1;
		HTI_LOG_FORMAT( "%d Pointer", advancedPointer->PointerNum );
		
		advancedPointer->X = aData[offset] + ( aData[offset+1] << 8 );
		offset += 2;
		HTI_LOG_FORMAT( "X coord down = %d", advancedPointer->X );
		
		advancedPointer->Y = aData[offset] + ( aData[offset+1] << 8 );
		offset += 2;
		HTI_LOG_FORMAT( "Y coord down = %d", advancedPointer->Y );

		advancedPointer->Z = aData[offset] + ( aData[offset+1] << 8 );
		offset += 2;
		HTI_LOG_FORMAT( "Z coord down = %d", advancedPointer->Z );
		
        iAdvancedPointers.AppendL( advancedPointer );
        }
	
    // Start tapping
    iReady = EFalse;
    ChangePointerStateL();

    HTI_LOG_FUNC_OUT( "CPointerEventHandler::HandleAdvancedTapScreenL" );
    
    }
	
// ----------------------------------------------------------------------------
// CPointerEventHandler::HandlePinchZoomL()
// ****cherry
// ----------------------------------------------------------------------------
void CPointerEventHandler::HandlePinchZoomL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::HandlePinchZoomL" );

    if ( aData.Length() < KPinchZoomCmdMinLength ) //KPinchZoomCmdMinLength needs to be defined
        {
        SendErrorMessageL( EInvalidParameters, KErrorInvalidParameters );
        return;
        }
	TInt dataLength = aData.Length();
	
    // Parse the parameters - correct length is already verified
	TInt offset = 0;
	
	TTimeIntervalMicroSeconds32 eventDelay = ( aData[offset] + ( aData[offset+1] << 8 ) ) * 1000;
	offset += 2;
    HTI_LOG_FORMAT( "Event time = %d", eventDelay.Int() );
    
    if (eventDelay.Int()<=0)
        {
        SendErrorMessageL( EInvalidParameters, KErrorInvalidParameters );
        return;        
        }
    
    TInt stepCount = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "Step Count = %d", stepCount );
        
	TInt pointerCount = aData[offset] + ( aData[offset+1] << 8 );
	offset += 2;
	HTI_LOG_FORMAT( "Pointer Count = %d", pointerCount );
	
	iAdvPointerMoveArray.ResetAndDestroy();	

    // Read integers from aData to the array, all integers are 2 bytes
    for ( TInt i = 0; i < pointerCount; i++ )
        {
        TInt pointNumber,X1, Y1, Z1,X2,Y2, Z2 ;
        
        // invalid pointer array 
        if ((dataLength-offset)<3*2*2+1)
            {
            SendErrorMessageL( EInvalidParameters, KErrorInvalidParameters );
            return;        
            }        
        // start point	
		pointNumber = aData[offset];
		offset += 1;
		HTI_LOG_FORMAT( "%d Pointer Start", pointNumber );
		
		X1 = aData[offset] + ( aData[offset+1] << 8 );
		offset += 2;
		HTI_LOG_FORMAT( "X coord down = %d", X1 );
		
		Y1 = aData[offset] + ( aData[offset+1] << 8 );
		offset += 2;
		HTI_LOG_FORMAT( "Y coord down = %d", Y1 );

		Z1 = aData[offset] + ( aData[offset+1] << 8 );
		offset += 2;
		HTI_LOG_FORMAT( "Z coord down = %d", Z1 );		

		// end point     
		X2 = aData[offset] + ( aData[offset+1] << 8 );
        offset += 2;
        HTI_LOG_FORMAT( "X coord down = %d", X2 );
        
        Y2 = aData[offset] + ( aData[offset+1] << 8 );
        offset += 2;
        HTI_LOG_FORMAT( "Y coord down = %d", Y2 );

        Z2 = aData[offset] + ( aData[offset+1] << 8 );
        offset += 2;
        HTI_LOG_FORMAT( "Z coord down = %d", Z2 );
        
        AdvanceAddMiddlePointL(pointNumber,X1, Y1, Z1,X2,Y2, Z2,stepCount );        

        }
    	
    AdvancedAddDelayArray(eventDelay,stepCount);
   
    iReady = EFalse;
    PointerDown();

    if (!AdvancedStartDelay())
        {
        SendErrorMessageL( EInvalidParameters, KErrorInvalidParameters );
        return;        
        }

    HTI_LOG_FUNC_OUT( "CPointerEventHandler::HandlePinchZoomL" );
    }	
// ----------------------------------------------------------------------------
// void CPointerEventHandler::HandleMultiTouchL()
// ----------------------------------------------------------------------------
void CPointerEventHandler::HandleMultiTouchL( const TDesC8& aData )
	{
	HTI_LOG_FUNC_IN( "CPointerEventHandler::HandleMultiTouchL" );	
	
	if (iMultiTouchHandler == NULL)
		iMultiTouchHandler=CMultiTouchPointerEventHandler::NewL(*this);	
	
    if ( !iMultiTouchHandler->HandleMultiTouchL ( aData ) )
    	{
		iMultiTouchHandler->Clear();
		SendErrorMessageL( EInvalidParameters, KErrorInvalidParameters );
    	}
    else
		iReady = EFalse;
	
	HTI_LOG_FUNC_OUT( "CPointerEventHandler::HandleMultiTouchL" );
	}
// ----------------------------------------------------------------------------
// void CPointerEventHandler::NotifyMultiTouchComplete()
// ----------------------------------------------------------------------------
void CPointerEventHandler::NotifyMultiTouchComplete()
    {
    HTI_LOG_FUNC_IN("CPointerEventHandler::NotifyMultiTouchComplete"); 
    
    // wait for 1000 microsecond then clear multi touch and send ok msg 
    TTimeIntervalMicroSeconds32 time(1000);
    iTimer.After( iStatus, time );
    SetActive();    
    
    HTI_LOG_FUNC_OUT("CPointerEventHandler::NotifyMultiTouchComplete");
    }
// ----------------------------------------------------------------------------
// CPointerEventHandler::AdvancedStartDelay()
// ----------------------------------------------------------------------------
TBool CPointerEventHandler::AdvancedStartDelay()
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::AdvancedStartDelay" );
    TBool ret=EFalse;
    if (iDelayArray.Count()>0)
        {
        TTimeIntervalMicroSeconds32* time=iDelayArray[0];
        iDelayArray.Remove(0);
        ret=ETrue;
  
        iTimer.After( iStatus, *time );
        delete time;
        SetActive();
        }
    HTI_LOG_FUNC_OUT( "CPointerEventHandler::AdvancedStartDelay" );
    return ret;
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::AdvanceAddMiddlePointL()
// ----------------------------------------------------------------------------
void CPointerEventHandler::AdvanceAddMiddlePointL(TInt aPointNumber,TInt aX1,TInt aY1, TInt aZ1, 
        TInt aX2,TInt aY2, TInt aZ2 , TInt aStepCount )
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::AdvanceAddMiddlePointL" );
    TInt dx=(aX2-aX1)/aStepCount;
    TInt dy=(aY2-aY1)/aStepCount;  
    
    for (TInt i=0;i<=aStepCount+1;i++)
        {
        TAdvancedPointer* point = new (ELeave) TAdvancedPointer;
        CleanupStack::PushL(point);
        iAdvPointerMoveArray.AppendL(point);
        CleanupStack::Pop();
        
        point->PointerNum=aPointNumber;
        if (i<aStepCount)
            {
            point->X=aX1+i*dx;
            point->Y=aY1+i*dy;
            point->Z=aZ1;
            }
        else
            {
            point->X=aX2;
            point->Y=aY2;
            point->Z=aZ2;
            }             
                  
        } 
    
    HTI_LOG_FUNC_OUT( "CPointerEventHandler::AdvanceAddMiddlePointL" );
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::AdvancedAddDelayArray()
// ----------------------------------------------------------------------------
void CPointerEventHandler::AdvancedAddDelayArray(TTimeIntervalMicroSeconds32 aDelay , TInt aStepCount )
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::AdvancedAddDelayArray" );
    
    TInt interval=aDelay.Int()/aStepCount;
    iDelayArray.ResetAndDestroy();    
    
    for (TInt i=0;i<aStepCount;i++)
        {
        TTimeIntervalMicroSeconds32* time = new (ELeave) TTimeIntervalMicroSeconds32(interval);
        CleanupStack::PushL(time);
        iDelayArray.AppendL(time);
        CleanupStack::Pop(time);
        } 
    HTI_LOG_FUNC_OUT( "CPointerEventHandler::AdvancedAddDelayArray" );
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::MoveToNextPointL()
// Takes the next point from the coordinate array and initiates pointer moving
// to that point.
// This function is used by ETapAndDragMultipoint command and called from
// the RunL().
// ----------------------------------------------------------------------------
void CPointerEventHandler::MoveToNextPointL()
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::MoveToNextPointL" );
    HTI_LOG_FORMAT( "Points remaining for this line = %d", iTapCount );

    if ( iTapCount == 0 ) // End of current line
        {
        PointerUp();
        if ( iCoords->Count() < 5 ) // point count & at least 2 points
            {
            // This was the last line, we are done
            SendOkMsgL();
            iReady = ETrue;
            }
        else
            {
            // New line starts: take the point count of this line and
            // first coordinates.
            iTapCount = ( *iCoords )[0];
            iCoords->Remove( 0 );
            iX = ( *iCoords )[0];
            iCoords->Remove( 0 );
            iY = ( *iCoords )[0];
            iCoords->Remove( 0 );
            HTI_LOG_FORMAT( "Point count for new line = %d", iTapCount );
            iTimer.After( iStatus, iActionDelay );
            SetActive();
            }
        }

    else  // Current line continues: take next point coords and move
        {
        iX = ( *iCoords )[0];
        iCoords->Remove( 0 );
        iY = ( *iCoords )[0];
        iCoords->Remove( 0 );
        PointerMove();
        iTapCount--;
        iTimer.After( iStatus, iEventDelay );
        SetActive();
        }

    HTI_LOG_FUNC_OUT( "CPointerEventHandler::MoveToNextPointL" );
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::PointerDown()
// ----------------------------------------------------------------------------
void CPointerEventHandler::PointerDown()
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::PointerDown" );
    SimulatePointerEvent( TRawEvent::EButton1Down );
    iState = EPointerDown;
    HTI_LOG_FUNC_OUT( "CPointerEventHandler::PointerDown" );
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::PointerUp()
// ----------------------------------------------------------------------------
void CPointerEventHandler::PointerUp()
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::PointerUp" );
    SimulatePointerEvent( TRawEvent::EButton1Up );
    iState = EPointerUp;
    HTI_LOG_FUNC_OUT( "CPointerEventHandler::PointerUp" );
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::PointerMove()
// ----------------------------------------------------------------------------
void CPointerEventHandler::PointerMove()
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::PointerMove" );
    SimulatePointerEvent( TRawEvent::EPointerMove );
    iState = EPointerMoving;
    HTI_LOG_FUNC_OUT( "CPointerEventHandler::PointerMove" );
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::SimulatePointerEvent()
// Sends the pointer event as a raw event.
// ----------------------------------------------------------------------------
void CPointerEventHandler::SimulatePointerEvent( TRawEvent::TType aType )
    {
    HTI_LOG_FUNC_IN( "CPointerEventHandler::SimulatePointerEvent" );
    
    TRawEvent rawEvent;
    
    if ( iCommand == EAdvancedTapScreen )
        {
        TInt i;
         for ( i = 0; i < iAdvancedPointers.Count(); i++ )
             {
             rawEvent.SetPointerNumber( iAdvancedPointers[i]->PointerNum ); 
             rawEvent.Set( aType, iAdvancedPointers[i]->X, iAdvancedPointers[i]->Y, iAdvancedPointers[i]->Z);    
             iWsSession.SimulateRawEvent( rawEvent );    
             }      
        }
    else if ( iCommand == EPinchZoom  )
        {
        TInt i,index,pointnum=-1;
        RPointerArray<TAdvancedPointer> array;
        for ( i = 0; i < iAdvPointerMoveArray.Count(); i++ )
            {
            TAdvancedPointer* point=iAdvPointerMoveArray[i];   
            if (point->PointerNum!=pointnum)
                {
                pointnum=point->PointerNum;                
                rawEvent.SetPointerNumber( point->PointerNum ); 
                rawEvent.Set( aType, point->X, point->Y, point->Z); 	
                iWsSession.SimulateRawEvent( rawEvent );	
                
                HTI_LOG_FORMAT( "SimulateAdvanced event=%d ", aType );
                HTI_LOG_FORMAT( "SimulateAdvanced PointerNum=%d", point->PointerNum );
                HTI_LOG_FORMAT( "SimulateAdvanced X=%d ", point->X );
                HTI_LOG_FORMAT( "SimulateAdvanced Y=%d ", point->Y );
                HTI_LOG_FORMAT( "SimulateAdvanced Z=%d", point->Z );
                
                array.Append(point);
                }
            }
        for (i=0;i<array.Count();i++)
            {
            index=iAdvPointerMoveArray.Find(array[i]);
            if (index!=KErrNotFound)
                iAdvPointerMoveArray.Remove(index);
            }
        array.ResetAndDestroy();
        }
    else
        {	
        rawEvent.Set( aType, iX, iY );
        iWsSession.SimulateRawEvent( rawEvent );
        }
    
    iWsSession.Flush();
    HTI_LOG_FUNC_OUT( "CPointerEventHandler::SimulatePointerEvent" );
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::IsMultitouch()
// ----------------------------------------------------------------------------
TBool CPointerEventHandler::IsMultitouch()
    {
    HTI_LOG_FUNC_IN("CPointerEventHandler::IsMultitouch");
    TBool isMultitouch = EFalse;
    if ( iCommand == EAdvancedTapScreen || iCommand == EPinchZoom )
        {
        isMultitouch = ETrue;
        }
    HTI_LOG_FUNC_OUT("CPointerEventHandler::IsMultitouch");
    return isMultitouch;
    }    
    
// ----------------------------------------------------------------------------
// CPointerEventHandler::SendOkMsgL()
// ----------------------------------------------------------------------------
void CPointerEventHandler::SendOkMsgL()
    {
    HTI_LOG_FUNC_IN("CPointerEventHandler::SendOkMsgL");

    User::LeaveIfNull( iDispatcher );
    TBuf8<1> response;
    response.Append( EResultOk );
    HBufC8* respH8 = response.AllocL();
    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
        respH8, KKeyEventServiceUid ) );

    HTI_LOG_FUNC_OUT("CPointerEventHandler::SendOkMsgL");
    }

// ----------------------------------------------------------------------------
// CPointerEventHandler::SendErrorMessageL()
// ----------------------------------------------------------------------------
void CPointerEventHandler::SendErrorMessageL(
        TInt aError, const TDesC8& aDescription )
    {
    HTI_LOG_FUNC_IN("CPointerEventHandler::SendErrorMessageL");
    User::LeaveIfNull( iDispatcher );
    User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
        aError, aDescription, KKeyEventServiceUid ) );
    HTI_LOG_FUNC_OUT("CPointerEventHandler::SendErrorMessageL");
    }

// End of file
