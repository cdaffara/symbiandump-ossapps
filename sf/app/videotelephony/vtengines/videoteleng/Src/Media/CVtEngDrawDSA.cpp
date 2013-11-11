/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Remote video handler.
*
*/


// INCLUDE FILES

#include "CVtEngDrawDSA.h"
#include "CVtEngEventManager.h"
#include "VtEngPanic.h"
#include "MVtEngFrameObserver.h"
#include "CVtEngSettings.h"
#include "VtEngUtils.h"
#include <videotelephonyvariant.hrh>
#include <tdisplaysinkparamsdsa.h>
#include <cvtlogger.h>

// LOCAL CONSTANTS

// Default display mode.
const TDisplayMode KVtEngDefaultDisplayMode( EColor64K );

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::NewL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
CVtEngDrawDSA* CVtEngDrawDSA::NewL(
    MDisplaySinkObserver* aObserver )
    {
    CVtEngDrawDSA* self = new ( ELeave ) CVtEngDrawDSA( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::~CVtEngDrawDSA
// Destructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CVtEngDrawDSA::~CVtEngDrawDSA()
    {
    __VTPRINTENTER( "RVD(DSA).~" )
    Cancel();
    delete iBitmap1;
    delete iBitmap2;
    if ( iImageScaler )
        {
        iImageScaler->Release();
        }
    __VTPRINTEXIT( "RVD(DSA).~" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::CVtEngDrawDSA
// C++ constructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CVtEngDrawDSA::CVtEngDrawDSA( MDisplaySinkObserver* aObserver ) :
    CVtEngDraw( aObserver ), iBitmapSetAvail( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseConstructL
// Base construction.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseConstructL( )
    {
    __VTPRINTENTER( "RVD(DSA).BaseConstructL" )
    TSize vtEngRemoteVideoDefaultSize( KVtEngRemoteVideoDefaultWidth,
        KVtEngRemoteVideoDefaultHeight );

    iBitmap1 = new ( ELeave ) CFbsBitmap();
    User::LeaveIfError( iBitmap1->Create(
        vtEngRemoteVideoDefaultSize, KVtEngDefaultDisplayMode ) );
    iBitmap2 = new ( ELeave ) CFbsBitmap();
    User::LeaveIfError( iBitmap2->Create(
        vtEngRemoteVideoDefaultSize, KVtEngDefaultDisplayMode ) );
    iImageScaler = VtEngScalerFactory::CreateScalerL( *this );
    __VTPRINT2( DEBUG_MEDIA | DEBUG_CONSTRUCT,
        "RVD.ConL scaler ok %d", iImageScaler != NULL )
    __VTPRINTEXIT( "RVD(DSA).BaseConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::CreateSinkL
// Instantiates display sink.
// -----------------------------------------------------------------------------
//
MDisplaySink* CVtEngDrawDSA::CreateSinkL()
    {
    __VTPRINTENTER( "RVD(DSA).CreateSinkL" )
    TDisplaySinkParamsDSA params;
    GetSinkParams( params );
    MDisplaySink* sink = ::CreateSinkL( params, KNullDesC8 );
    __VTPRINTEXITR( "RVD(DSA).CreateSinkL flags=%d", params.iFlags )
    return sink;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseDoCancel
// DoCancel for derived class.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseDoCancel()
    {
    __VTPRINTENTER( "RVD(DSA).BaseDoCancel" )
    __VTPRINTEXIT( "RVD(DSA).BaseDoCancel" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseVerifyConfigL
// Checks current configuration against DSA implementation.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseVerifyConfigL()
    {
    __VTPRINTENTER( "RVD(DSA).BaseVerifyConfigL" )
    __VTPRINTEXIT( "RVD(DSA).BaseVerifyConfigL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseRestartL
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseRestartL()
    {
    __VTPRINTENTER( "RVD(DSA).BaseRestartL" )
    __VTPRINTEXIT( "RVD(DSA).BaseRestartL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseAbortNow
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseAbortNow()
    {
    __VTPRINTENTER( "RVD(DSA).BaseAbortNow" )
    __VTPRINTEXIT( "RVD(DSA).BaseAbortNow" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseStartDrawL
// Starts the drawing.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseStartDrawL()
    {
    __VTPRINTENTER( "RVD(DSA).BaseStartDrawL" )
    if ( iDSA )
        {
        SetFlag( EReadyInternal );
        if ( !IsActive() )
            {
            Activate();
            iDisplaySink->SetBitmapAvailable( MDisplaySink::EFirstBitmap );
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                "RVD(DSA).BaseStartDrawL bmp avail done" )
            iWatcher->Start( KVtEngWatcherTimeout, KVtEngWatcherTimeout,
                iCallback );
            }
        SetFlag( ERemoteVideoDrawing );
        }
    else
        {
        User::Leave( KErrNotReady );
        }
    __VTPRINTEXIT( "RVD(DSA).BaseStartDrawL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseStopDraw
// Stop the drawing.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseStopDraw()
    {
    // Cancel the timer in any case.
    __VTPRINTENTER( "RVD(DSA).BaseStopDraw" )
    iWatcher->Cancel();
    if ( iDSA )
        {
        ClearFlag( ERemoteVideoDrawing | EStarted );
        iDSA->Cancel();
        }

    // Stop also scaling
    if ( iImageScaler )
        {
        iImageScaler->Cancel();
        }

    Cancel();
    __VTPRINTEXIT( "RVD(DSA).BaseStopDraw" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseSetConfigL
// Sets new configuration.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseSetConfigL( const TVtEngRenderConfig& aConfig )
    {
    __VTPRINTENTER( "RVD(DSA).BaseSetConfigL" )
    Mem::FillZ( &iConfig, sizeof( TVtEngRenderConfigDSA ) );
    iConfig = aConfig;
    ConfigUpdatedL();
    __VTPRINTEXIT( "RVD(DSA).BaseSetConfigL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseSetUIForegroundL
// Sets UI foreground status.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseSetUIForegroundL( TBool aForeground )
    {
    __VTPRINTENTER( "RVD(DSA).BaseSetUIForegroundL" )
    if ( aForeground )
        {
        TBool wasSentToBackground( IsFlag( ESentToBackground ) );
        SetFlag( EFlagUIForeground );
        ClearFlag( ESentToBackground );
        if ( wasSentToBackground )
            {
            BaseRefreshBitmapsL();
            Restart( RDirectScreenAccess::ETerminateCancel );
            BaseStartDrawL();
            }
        }
    else
        {
        BaseStopDraw();
        ClearFlag( EFlagUIForeground );
        SetFlag( ESentToBackground );
        }
    __VTPRINTEXIT( "RVD(DSA).BaseSetUIForegroundL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseVideoFrameSizeChangedL
// Called when incoming frame buffer size changes.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseVideoFrameSizeChangedL( const TSize& /*aTo*/ )
    {
    __VTPRINTENTER( "RVD(DSA).BaseVideoFrameSizeChangedL" )
    __VTPRINTEXIT( "RVD(DSA).BaseVideoFrameSizeChangedL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseRequestLastFrame
// Request update last remote video frame through MVtEngFrameObserver::vtSetFrame
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseRequestLastFrame()
    {
    __VTPRINTENTER( "RVD(DSA).BaseRequestLastFrame" )
    __VTPRINTEXIT( "RVD(DSA).BaseRequestLastFrame" )    
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::Config
// Returns current configuration.
// -----------------------------------------------------------------------------
//
TVtEngRenderConfig& CVtEngDrawDSA::Config()
    {
    return iConfig;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::RunL
// CActive heir execution method.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::RunL()
    {
    __VTPRINTENTER( "RVD(DSA).RunL" )
    __VTPRINT2( DEBUG_MEDIA, "  bmpNo=%d", iBitmapNo )
    iBitmapNo = iStatus.Int() == 0 ?
        MDisplaySink::ESecondBitmap :
        MDisplaySink::EFirstBitmap ;

    iBitmapSetAvail = EFalse;

    TBool dropFrame( EFalse );

    if ( !iDSUpdated )
        {
        __VTPRINT( DEBUG_MEDIA, "RVD(DSA).RunL !Updated" )
        dropFrame = ETrue;
        }
    else if ( !IsFlag( EFlagUIForeground ) )
        {
        __VTPRINT( DEBUG_MEDIA, "RVD(DSA).RunL !Flag( EFlagUIForeground )" )
        dropFrame = ETrue;
        }

    if ( dropFrame )
        {
        iBitmapSetAvail = ETrue;
        Activate();
        iDisplaySink->SetBitmapAvailable( iBitmapNo );
        __VTPRINTEXIT( "RVD(DSA).RunL" )
        return;
        }

    // Image received, reset counter.
    iCheckpoint = KVtEngWatcherThreshold;

    __VTPRINT2( DEBUG_MEDIA, "RVD(DSA).RunL iStatus=%d", iStatus.Int() )
    if ( iStatus >= KErrNone )
        {
        __VTPRINT2( DEBUG_MEDIA, "RVD(DSA).RunL flags=%d", iFlags )
        TBool firstFrame( !IsFlag ( EFirstFrameReceived ) );
        SetFlag( EFirstFrameReceived );

        // Remote video is about to start?
        if ( !IsFlag( EStarted ) && IsFlag( EReadyInternal ) )
            {
            SetFlag( EStarted );
            iDSA->StartL();
            if ( !iConfig.iClipRect.IsEmpty() )
                {
                SetFlag( EReadyForeground );
                iDSA->Gc()->SetClippingRect( iConfig.iClipRect );
                }
            __VTPRINT( DEBUG_MEDIA, "RVD(DSA).RunL notify rvd start" )
            //CVtEngEventManager::NotifyEvent( KVtEngRemoteVideoStarted );
            }
        if ( firstFrame )
            {
            CVtEngEventManager::NotifyEvent( KVtEngRemoteVideoStarted );
            }
        if ( IsFlag( EStarted ) )
            {
            __VTPRINT( DEBUG_MEDIA, "RVD(DSA).RunL refresh" )
            BaseRefreshL();
            }
        }
    else
        {
        __VTPRINT( DEBUG_MEDIA, "RVD.RunL notify rvd problem" )
        CVtEngEventManager::NotifyEvent( KVtEngRemoteRenderingProblem );
        __VTPRINTEXIT( "RVD(DSA).RunL" )
        return;
        }

    __VTPRINT( DEBUG_MEDIA, "RVD.RunL setting active" )
    Activate();

    // Set bitmap only if not to be scaled
    if( !iImageScaler )
        {
        if ( !iBitmapSetAvail )
            {
            iDisplaySink->SetBitmapAvailable( iBitmapNo );
            iBitmapSetAvail = ETrue;
            }
        __VTPRINT( DEBUG_MEDIA, "RVD(DSA).ScalingCompleted set bmp available" )
        }

    __VTPRINTEXIT( "RVD(DSA).RunL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::ScalingCompleted
// Callback to receive scaled images.
// Draws received image.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::ScalingCompleted(
    CFbsBitmap* aBitmap,
    TInt aError )
    {
    __VTPRINTENTER( "RVD(DSA).ScalingCompleted" )
    __VTPRINT2( DEBUG_MEDIA, "RVD(DSA).ScalingReady %d", aError )
    __VTPRINT2( DEBUG_MEDIA, "RVD(DSA).ScalingReady image %d", aBitmap ? 1 : 0 )
    if ( aError == KErrNone &&
         aBitmap != NULL )
        {
        DrawBitmap( *aBitmap );
        if ( !iBitmapSetAvail )
            {
            // Set bitmap available if not already set
            __VTPRINT( DEBUG_MEDIA, "RVD(DSA).ScalingCompleted set bmp available" )
            Activate();
            iDisplaySink->SetBitmapAvailable( iBitmapNo );
            iBitmapSetAvail = ETrue;
            }
        __VTPRINT( DEBUG_MEDIA, "RVD(DSA).ScalingCompleted available?" )
        }
    else if ( aError != KErrCancel )
        {
        // Notification of failed scaling.
        CVtEngEventManager::NotifyEvent( KVtEngRemoteRenderingProblem );
        __VTPRINT( DEBUG_MEDIA, "RVD(DSA).ScalingReady problems" )
        }
    __VTPRINTEXITR( "RVD(DSA).ScalingCompleted err=%d", aError )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseRefreshL
// If using scaler, start the scaling otherwise draw a bitmap.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseRefreshL()
    {
    __VTPRINTENTER( "RVD(DSA).BaseRefreshL" )
    if ( iConfig.iWindow )
        {
        if ( IsFlag( EReady ) && IsFlag( EFirstFrameReceived ) &&
                IsFlag( EFlagUIForeground ) )
            {
            CFbsBitmap* current = iBitmapNo != 0 ? iBitmap1 : iBitmap2;
            if ( current )
                {
                if ( iImageScaler )
                    {
                    __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                        "RVD(DSA).BaseRefreshL scaling" )
                    TInt index = 0;
                    if ( current == iBitmap2 )
                        {
                        index = 1;
                        }
                    iImageScaler->Cancel();
                    iImageScaler->ScaleL( index );
                    }
                else
                    {
                    DrawBitmap( *current );
                    }
                }
            }
        }
    __VTPRINTEXIT( "RVD(DSA).BaseRefreshL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::BaseRefreshBitmapsL
// Refreshes bitmaps sizes and display mode if needed.
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::BaseRefreshBitmapsL()
    {
    __VTPRINTENTER( "RVD(DSA).BaseRefreshBitmapsL" )
    const TDisplayMode currentMode( iConfig.iWsSreenDevice->DisplayMode() );
    TBool refreshDisplayMode = EFalse;
    TSize newSize( KVtEngRemoteVideoDefaultWidth,
        KVtEngRemoteVideoDefaultHeight );

    // Use new size if supplied
    if ( iSourceSize.iHeight > 0 && iSourceSize.iWidth > 0 )
        {
        newSize = iSourceSize;
        iSourceSize.iHeight = 0;
        iSourceSize.iWidth = 0;
        }

    __VTPRINT( DEBUG_MEDIA , "================== RVD.RFBM ==================" )
    __VTPRINT2( DEBUG_MEDIA , "    newSize.iWidth: %d", newSize.iWidth)
    __VTPRINT2( DEBUG_MEDIA , "    newSize.iHeight: %d ", newSize.iHeight )
    __VTPRINT( DEBUG_MEDIA , "================== RVD.RFBM ==================" )

    TBool refreshSize = EFalse;
    if ( iBitmap1 )
        {
        refreshDisplayMode = ( iBitmap1->DisplayMode() != currentMode );
        refreshSize = ( newSize != iBitmap1->SizeInPixels() );
        }

    // Delete and create the bitmaps
    if ( refreshDisplayMode || refreshSize || !iBitmap1 || !iBitmap2 )
        {
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
            "RVD(DSA).BaseRefreshBitmapsL bmp create" )
        if ( iImageScaler )
            {
            iImageScaler->Cancel();
            }

        iConfig.iObserver->vtSetFrame(
            MVtEngFrameObserver::ERemoteVideoFrame, 0 );
        delete iBitmap1;
        iBitmap1 = NULL;
        delete iBitmap2;
        iBitmap2 = NULL;

        iBitmap1 = new ( ELeave ) CFbsBitmap();
        User::LeaveIfError( iBitmap1->Create(
            newSize, currentMode ) );
        iBitmap2 = new ( ELeave ) CFbsBitmap();
        User::LeaveIfError( iBitmap2->Create(
            newSize, currentMode ) );
        ClearFlag( EFirstFrameReceived );
        }

    // set the params for DS
    TDisplaySinkParamsDSA params;
    GetSinkParams( params );

     // Update the scaler
    if ( iImageScaler )
        {
        // Try to configure first, if returns true
        // configure again
        __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
            "RVD(DSA).BaseRefreshBitmapsL cnfg" )

        iImageScaler->ConfigureL( currentMode,
        	iConfig.iRect.Size(), params.iBitmap1Handle,
        	params.iBitmap2Handle );

        if ( IsFlag( EFirstFrameReceived ) )
            {
            __VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
                "CVtEngDrawDSA::RefreshBitmapsL RFR" )
            BaseRefreshL();
            }

        }
	__VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
	"RVD(DSA).BaseRefreshBitmapsL USP" )
	__VTPRINT( DEBUG_MEDIA | DEBUG_DETAIL,
	"RVD(DSA).BaseRefreshBitmapsL USP flag" )
    iDisplaySink->UpdateSinkParamsL( params, iDSUpdated );
    Activate();
    iDisplaySink->SetBitmapAvailable( MDisplaySink::EFirstBitmap );
    __VTPRINTEXIT( "RVD(DSA).BaseRefreshBitmapsL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::DrawBitmap
// Draws bitmap
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::DrawBitmap( CFbsBitmap& aBitmap )
    {
    __VTPRINTENTER( "RVD(DSA).DrawBitmap" )
    __ASSERT_ALWAYS( aBitmap.Handle(),
        Panic( EVtEngPanicInvalidBitmapHandle ) );
    __ASSERT_ALWAYS( iDSA, Panic( EVtEngPanicInvalidPointer ) );
    iDSA->Gc()->SetFaded( iConfig.iWindow->IsFaded() );
    iDSA->Gc()->BitBlt( iConfig.iRect.iTl, &aBitmap );
    iDSA->ScreenDevice()->Update();
    iConfig.iObserver->vtSetFrame( MVtEngFrameObserver::ERemoteVideoFrame,
        &aBitmap );
	__VTPRINTEXITR( "RVD(DSA).DrawBitmap h=%d", aBitmap.Handle() )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::GetSinkParams
// -----------------------------------------------------------------------------
//
void CVtEngDrawDSA::GetSinkParams( TDisplaySinkParamsDSA& aSinkParams )
    {
    __VTPRINTENTER( "RVD(DSA).GetSinkParams" )
    aSinkParams.iThreadId = RThread().Id();
    aSinkParams.iRequestStatusPtr = &iStatus;
    aSinkParams.iObserver = iObserver;
    aSinkParams.iBitmap1Handle = iBitmap1->Handle();
    aSinkParams.iBitmap2Handle = iBitmap2->Handle();
    aSinkParams.iFlags = TDisplaySinkParams::EDisplaySinkDSA;
	__VTPRINTEXIT( "RVD(DSA).GetSinkParams" )
    }

//  End of File
