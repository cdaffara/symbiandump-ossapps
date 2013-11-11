/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for NGA Drawer.
*
*/

// INCLUDE FILES

#include "CVtEngDrawNGA.h"
#include "CVtEngEventManager.h"
#include "VtEngPanic.h"
#include "MVtEngFrameObserver.h"
#include "CVtEngSettings.h"
#include "VtEngUtils.h"
#include <videotelephonyvariant.hrh>
#include <tdisplaysinkparamsnga.h>
#include <cvtlogger.h>
#include <graphics/surfaceconfiguration.h>
#include <graphics/suerror.h>

// Reversed enumeration for the bitmaps.
static const MDisplaySink::TBitmapNo KVtEngBitmapNoReversed[] =
        {
        MDisplaySink::ESecondBitmap,
        MDisplaySink::EFirstBitmap
        };

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::TVTSurfaceBuffer::TVTSurfaceBuffer
//
// -----------------------------------------------------------------------------
//
TVTSurfaceBuffer::TVTSurfaceBuffer( TUint8* aBuffer, TInt aSurfaceNo )
    : iBuffer( aBuffer ), iSurfaceBufferNo(aSurfaceNo)
    {
    __VTPRINTENTER( "RVD(NGA).TVTSurfaceBuffer.Ctor" )
    __VTPRINTEXIT( "RVD(NGA).TVTSurfaceBuffer.Ctor" )
    }
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::TVTSurfaceBuffer::Buffer
//
// -----------------------------------------------------------------------------
//
TUint8*& TVTSurfaceBuffer::Buffer()
    {
    return iBuffer;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::TVTSurfaceBuffer::SurfaceNo
//
// -----------------------------------------------------------------------------
//
TInt TVTSurfaceBuffer::SurfaceNo()
    {
    return iSurfaceBufferNo;
    }
        
void TVTSurfaceBuffer::Set( TUint8* aBuffer, TInt aSurfaceNo )
    {
    iBuffer = aBuffer;
    iSurfaceBufferNo = aSurfaceNo;
    }
    
void TVTSurfaceBuffer::UnSet()
    {
    iBuffer = NULL;
    iSurfaceBufferNo = KErrNotFound;
    }
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::NewL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
CVtEngDrawNGA* CVtEngDrawNGA::NewL(
    MDisplaySinkObserver* aObserver )
    {
    CVtEngDrawNGA* self = new ( ELeave ) CVtEngDrawNGA( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::~CVtEngDrawNGA
// Destructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CVtEngDrawNGA::~CVtEngDrawNGA()
    {
    __VTPRINTENTER( "RVD(NGA).~" )
    Cancel();   
    
    if(iSurfaceCreated)
        {
        
        if (iSurfaceManager)
            {
            iConfig.iRemoteWindow->RemoveBackgroundSurface(ETrue);
            iConfig.iWsSession->UnregisterSurface(0, iSurfaceId);
            if ( iSurfaceChunk != NULL )
                {
                iSurfaceChunk->Close();
                }
            delete iSurfaceChunk;
            iSurfaceChunk = NULL;
            iSurfaceManager->CloseSurface(iSurfaceId);
            iSurfaceUpdateSession.CancelAllUpdateNotifications();
            iSurfaceUpdateSession.Close();
                       
             /* Close the surface manager handle */
            if ( iSurfaceManager != NULL )
                {
                iSurfaceManager->Close();
                }
           
            delete iSurfaceManager;
            iSurfaceManager = NULL;
            }
           
        for ( TInt i = KVtEngMaxSurfaceBuffers-1; i >= 0 ; i-- )
            {
            if (iCallBackTable[i])
                {
                iCallBackTable[i]->Cancel();
                delete iCallBackTable[i];
                iCallBackTable[i] = NULL;
                }
            }
    
        iSurfaceBuffers[ 0 ].UnSet();
        iSurfaceBuffers[ 1 ].UnSet();
        iSurfaceBuffer0.UnSet();
        iSurfaceBuffer1.UnSet();
        }
    delete iBufferWaiter;   
    iBufferWaiter = NULL;
    iWaitingBuffers.Reset();
    
    __VTPRINTEXIT( "RVD(NGA).~" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::CVtEngDrawNGA
// C++ constructor. Cannot leave.
// -----------------------------------------------------------------------------
//
CVtEngDrawNGA::CVtEngDrawNGA( MDisplaySinkObserver* aObserver ) :
    CVtEngDraw( aObserver ),
    iSurfaceFormat( EUidPixelFormatYUV_420Planar )
    
    {
    iConfig.iRemoteWindow = NULL;
    iConfig.iWsSession = NULL;
    iSurfaceCreated = EFalse;
    iWaitingBuffers.Reset();
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseConstructL
// Base construction.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseConstructL( )
    {
    __VTPRINTENTER( "RVD(NGA).BaseConstructL" )
    DoBaseConstructL();
    __VTPRINTEXIT( "RVD(NGA).BaseConstructL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::CreateSinkL
// Instantiates display sink.
// -----------------------------------------------------------------------------
//
MDisplaySink* CVtEngDrawNGA::CreateSinkL()
    {
    __VTPRINTENTER( "RVD(NGA).CreateSinkL" )
    TDisplaySinkParamsNGA params;
    GetSinkParams( params );
    MDisplaySink* sink = ::CreateSinkL( params, KNullDesC8 );
    if ( IsFlag( EBaseConstructCalled ) )
        {
        sink->Pause();
        SetFlag( ESinkParamUpdateRequired | ESinkResumeRequired );
        }    
    __VTPRINTEXITR( "RVD(NGA).CreateSinkL flags=%d", params.iFlags )
    return sink;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseRefreshL
// If using scaler, start the scaling otherwise draw a bitmap.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseRefreshL()
    {
    __VTPRINTENTER( "RVD(NGA).BaseRefreshL" )
    __VTPRINTEXIT( "RVD(NGA).BaseRefreshL" )
    }
   
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseRefreshBitmapsL
// Refreshes bitmaps sizes and display mode if needed.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseRefreshBitmapsL()
    {
    __VTPRINTENTER( "RVD(NGA).BaseRefreshBitmapsL" )
    BaseConstructL();
    CreateSurfaceL();
    // set the params for DS
    UpdateSinkParamsL();
    ClearFlag( EFirstFrameReceived );    
    __VTPRINTEXIT( "RVD(NGA).BaseRefreshBitmapsL" )
    }   
    
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseDoCancel
// DoCancel for derived class.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseDoCancel()
    {
    __VTPRINTENTER( "RVD(NGA).BaseDoCancel" )
    __VTPRINTEXIT( "RVD(NGA).BaseDoCancel" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseVerifyConfigL
// Checks current configuration against DSA implementation.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseVerifyConfigL()
    {
    __VTPRINTENTER( "RVD(NGA).BaseVerifyConfigL" )
    __VTPRINTEXIT( "RVD(NGA).BaseVerifyConfigL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseRestartL
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseRestartL()
    {
    __VTPRINTENTER( "RVD(NGA).BaseRestartL" )
    __VTPRINTEXIT( "RVD(NGA).BaseRestartL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseAbortNow
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseAbortNow()
    {
    __VTPRINTENTER( "RVD(NGA).BaseAbortNow" )
    __VTPRINTEXIT( "RVD(NGA).BaseAbortNow" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseStartDrawL
// Starts the drawing.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseStartDrawL()
    {
    __VTPRINTENTER( "RVD(NGA).BaseStartDrawL" )  
    if ( IsFlag( EBaseConstructCalled ) )
        {
        SetFlag( EBaseBaseStartDrawCalled );
        }
    else
        {
        DoBaseStartDrawL();
        }
    __VTPRINTEXIT( "RVD(NGA).BaseStartDrawL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseStopDraw
// Stop the drawing.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseStopDraw()
    {
    // Cancel the timer in any case.
    __VTPRINTENTER( "RVD(NGA).BaseStopDraw" )
    ClearFlag( EBaseBaseStartDrawCalled );
    iWatcher->Cancel();
    iBufferWaiter->Cancel();
    ClearFlag( ERemoteVideoDrawing | EStarted );
    for ( TInt i = KVtEngMaxSurfaceBuffers-1; i >= 0 ; i-- )
        {
        if (iCallBackTable[i])
            {
            iCallBackTable[i]->Cancel();
            }
        }
    Cancel();
    
    __VTPRINTEXIT( "RVD(NGA).BaseStopDraw" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseSetConfigL
// Sets new configuration.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseSetConfigL( const TVtEngRenderConfig& aConfig )
    {
    __VTPRINTENTER( "RVD(NGA).BaseSetConfigL" )
    Mem::FillZ( &iConfig, sizeof( TVtEngRenderConfigNGA ) );
    iConfig = aConfig;
    if ( IsFlag( EBaseConstructCalled ) )
        {
        SetFlag( EBaseSetConfigCalled );
        }
    else
        {
        ConfigUpdatedL();
        }
    __VTPRINTEXIT( "RVD(NGA).BaseSetConfigL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseSetUIForegroundL
// Sets UI foreground status.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseSetUIForegroundL( TBool aForeground )
    {
    __VTPRINTENTER( "RVD(NGA).BaseSetUIForegroundL" )
    __VTPRINTEXIT( "RVD(NGA).BaseSetUIForegroundL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseRequestLastFrame
// Request update last remote video frame through MVtEngFrameObserver::vtSetFrame
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseRequestLastFrame()
    {
    __VTPRINTENTER( "RVD(NGA).BaseRequestLastFrame" )
    __VTPRINTEXIT( "RVD(NGA).BaseRequestLastFrame" )    
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BaseVideoFrameSizeChangedL
// Called when incoming frame buffer size changes.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BaseVideoFrameSizeChangedL( const TSize& /*aTo*/ )
    {
    __VTPRINTENTER( "RVD(NGA).BaseVideoFrameSizeChangedL" )
    BaseConstructL();
    
    TInt err;
    
    iConfig.iRemoteWindow->RemoveBackgroundSurface(ETrue);
    iConfig.iWsSession->UnregisterSurface(0, iSurfaceId);
    
    iSurfaceChunk->Close();
    delete iSurfaceChunk;
    iSurfaceChunk = NULL;
    iSurfaceManager->CloseSurface(iSurfaceId);
    iSurfaceUpdateSession.CancelAllUpdateNotifications();
    iSurfaceUpdateSession.Close();
    
    /* Close the surface manager handle */
    iSurfaceManager->Close();
    delete iSurfaceManager;
    iSurfaceManager = 0;
    
    for ( TInt i = KVtEngMaxSurfaceBuffers-1; i >= 0 ; i-- )
        {
            if (iCallBackTable[i])
                {
                iCallBackTable[i]->Cancel();
                delete iCallBackTable[i];
                iCallBackTable[i] = NULL;
                }
        }
    iSurfaceBuffers[ 0 ].UnSet();
    iSurfaceBuffers[ 1 ].UnSet();
    iSurfaceBuffer0.UnSet();
    iSurfaceBuffer1.UnSet();
    iWaitingBuffers.Reset();
    
    err = iSurfaceUpdateSession.Connect();    
    User::LeaveIfError(err);
    
    iSurfaceManager = new RSurfaceManager();
    
    User::LeaveIfNull(iSurfaceManager); 
    err = iSurfaceManager->Open();
    User::LeaveIfError(err);
    
    RSurfaceManager::TSurfaceCreationAttributesBuf attributes;
    attributes().iPixelFormat           = iSurfaceFormat; 
    attributes().iSize                  = iSourceSize;
    attributes().iBuffers               = KVtEngMaxSurfaceBuffers;
    
    //attributes().iStride                = iSourceSize.iWidth*2;
    attributes().iStride                = iSourceSize.iWidth*3/2;
    
    attributes().iOffsetToFirstBuffer   = 0;
    attributes().iAlignment             = 4;
    attributes().iContiguous            = EFalse;
    attributes().iMappable              = ETrue;
    
    err = iSurfaceManager->CreateSurface(attributes, iSurfaceId);
    User::LeaveIfError(err);  
    
    // Map to chunk
    iSurfaceChunk = new RChunk();
    User::LeaveIfNull(iSurfaceChunk);    
    err = iSurfaceManager->MapSurface(iSurfaceId, *iSurfaceChunk);
    User::LeaveIfError(err);    
    
    // Get the info from the surfaceManager
    RSurfaceManager::TInfoBuf info;
    err = iSurfaceManager->SurfaceInfo(iSurfaceId, info);
    User::LeaveIfError(err);    
    
    TInt offset;
    iSurfaceManager->GetBufferOffset(iSurfaceId, 0, offset);
    iSurfaceBuffer0.Set(iSurfaceChunk->Base() + offset, 0 );
    iSurfaceManager->GetBufferOffset(iSurfaceId, 1, offset);
    iSurfaceBuffer1.Set(iSurfaceChunk->Base() + offset, 1 );
    
    iConfig.iWsSession->RegisterSurface(0, iSurfaceId);  
    TSurfaceConfiguration surfaceConfig;
    surfaceConfig.SetSurfaceId( iSurfaceId );
    surfaceConfig.SetOrientation( CFbsBitGc::EGraphicsOrientationRotated270);
    iConfig.iRemoteWindow->SetBackgroundSurface( surfaceConfig, ETrue );
    //iConfig.iRemoteWindow->SetBackgroundSurface(iSurfaceId);
    
    iCallBackTable[0] = new(ELeave) CActiveCallBack(
            TCallBack(SurfaceBuffer0Ready, this), 
            CActive::EPriorityStandard-1);
    CActiveScheduler::Add(iCallBackTable[0]);
    
    iCallBackTable[1] = new(ELeave) CActiveCallBack(
            TCallBack(SurfaceBuffer1Ready, this), 
            CActive::EPriorityStandard-1);
    CActiveScheduler::Add(iCallBackTable[1]);
    
    iSurfaceCreated = ETrue;    
    
    ClearFlag( EInitializePostingSurfaceCalled );   
    
    // set the params for DS
    UpdateSinkParamsL();
    ClearFlag( EFirstFrameReceived );  

    __VTPRINTEXIT( "RVD(NGA).BaseVideoFrameSizeChangedL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawDSA::Config
// Returns current configuration.
// -----------------------------------------------------------------------------
//
TVtEngRenderConfig& CVtEngDrawNGA::Config()
    {
    return iConfig;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::RunL
// CActive heir execution method.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::RunL()
    {
    __VTPRINTENTER( "RVD(NGA).RunL" )
    
    TInt bitmapNo( iStatus.Int() );

    if ( bitmapNo < KErrNone )
        {
        __VTPRINT( DEBUG_MEDIA, "RVD(NGA).RunL notify rvd problem" )
        CVtEngEventManager::NotifyEvent( KVtEngRemoteRenderingProblem );
        return;
        }

    TVtEngBuffer& buffer = iSurfaceBuffers[ bitmapNo ];
    
    if ( !iDSUpdated )
        {
        __VTPRINT( DEBUG_MEDIA, "RVD(NGA).RunL !Updated" )
        __VTPRINTEXIT( "RVD(NGA).RunL" )
        return;
        }
    else if ( buffer.SurfaceBuffer() == NULL )
        {
        __VTPRINT( DEBUG_MEDIA, "RVD(NGA).RunL signalled with NULL buffer" )
        TInt result( iBufferWaiter->FetchBuffer() );
        User::LeaveIfError( result );
        __VTPRINTEXIT( "RVD(NGA).RunL" )
        return;
        }
    
    // Image received, reset counter.
    iCheckpoint = KVtEngWatcherThreshold;

    __VTPRINT2( DEBUG_MEDIA, "RVD(NGA).RunL flags=%d", iFlags )
    TBool firstFrame( !IsFlag ( EFirstFrameReceived ) );
    SetFlag( EFirstFrameReceived );
    if ( firstFrame )
        {
        CVtEngEventManager::NotifyEvent( KVtEngRemoteVideoStarted );
        }

    TInt surfaceno = buffer.SurfaceBuffer()->SurfaceNo();
    __VTPRINT2( DEBUG_MEDIA, "RVD(NGA).RunL received buffer surface id %d", surfaceno )
    if ( iSubmitPending )
        {
        __VTPRINT( DEBUG_MEDIA, "RVD(NGA).RunL submit pending")
        iWaitingBuffers.AddLast(buffer);
        }
    else
        {
        iCallBackTable[surfaceno]->iStatus = KRequestPending;
        iCallBackTable[surfaceno]->SetActive();
        iSurfaceUpdateSession.NotifyWhenDisplayed(iCallBackTable[surfaceno]->iStatus, iTimeStamp);
        iSubmitPending = ETrue;
        iSurfaceUpdateSession.SubmitUpdate( KAllScreens, iSurfaceId, surfaceno, NULL ); 
        buffer.UnSet();
        }
    if ( iSurfaceBuffers[ KVtEngBitmapNoReversed[ bitmapNo ] ].IsSet() )
        {
        __VTPRINT2( DEBUG_MEDIA, "RVD(NGA).RunL reactivating for buffer %d",
            KVtEngBitmapNoReversed[ bitmapNo ] )
        Activate();
        DisplaySink().NextFrame();
        }    
    TInt result = iBufferWaiter->FetchBuffer();
    User::LeaveIfError( result );    
    
    __VTPRINTEXIT( "RVD(NGA).RunL" )
    }


// -----------------------------------------------------------------------------
// CVtEngDrawNGA::BufferReadyL
// BufferReadyL
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::BufferReadyL( TVTSurfaceBuffer* aBuffer )
    {
    __VTPRINTENTER( "RVD(NGA).BufferReadyL" )
    SetAvailableToSink( aBuffer );
    __VTPRINTEXIT( "RVD(NGA).BufferReadyL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::FetchError
// Buffer fetching error
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::FetchError( TInt )
    {
    __VTPRINTENTER( "RVD(NGA).FetchError" )
    CVtEngEventManager::NotifyEvent( KVtEngRemoteRenderingProblem );
    __VTPRINTEXIT( "RVD(NGA).FetchError" )
    }    
   
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::DoBaseConstructL
// Base construction implementation
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::DoBaseConstructL()
    {
    __VTPRINTENTER( "RVD(NGA).DoBaseConstructL" )
    delete iBufferWaiter;
    iBufferWaiter = NULL;

    iBufferWaiter =
        new ( ELeave ) CVtEngBufferWaiter( *this );
        
    for ( TInt i = MDisplaySink::EFirstBitmap;
          i < MDisplaySink::EFirstBitmap + KVtEngMaxSurfaceBuffers; i++ )
        {
        iSurfaceBuffers[ i ].UnSet();
        }     
    ClearFlag( EBaseConstructCalled );          
    __VTPRINTEXIT( "RVD(NGA).DoBaseConstructL" )
    }
          
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::UpdateSinkParamsL
// Updates sink params
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::UpdateSinkParamsL()
    {
    __VTPRINTENTER( "RVD(NGA).UpdateSinkParamsL" )
    if ( IsFlag( EBaseConstructCalled ) )
        {
        SetFlag( ESinkParamUpdateRequired );
        }
    else
        {
        DoUpdateSinkParamsL();
        }
    __VTPRINTEXIT( "RVD(NGA).UpdateSinkParamsL" )
    }

    
 // -----------------------------------------------------------------------------
// CVtEngDrawNGA::DoBaseStartDrawL
//
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::DoBaseStartDrawL()
    {
    __VTPRINTENTER( "RVD(NGA).DoBaseStartDrawL" )
    SetFlag( EReadyInternal );
    if ( !iWatcher->IsActive() )
       {
       iWatcher->Start( KVtEngWatcherTimeout, KVtEngWatcherTimeout,
             iCallback );
       }
    
    DoSurfaceBuffer0Ready();
    DoSurfaceBuffer1Ready();
    
    User::LeaveIfError( iBufferWaiter->FetchBuffer() );
    
        
    SetFlag( ERemoteVideoDrawing );             
    ClearFlag( EBaseBaseStartDrawCalled );
    __VTPRINTEXIT( "RVD(NGA).DoBaseStartDrawL" )
    }
                   
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::DoCreateSurfaceL
// Create surface implementation.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::DoCreateSurfaceL()
    {
    __VTPRINTENTER( "RVD(NGA).DoCreateSurfaceL" )  
    TInt err;
    
    if(iSurfaceCreated)
        {
                
        if (iSurfaceManager)
            {
            /* Change log for recreate surface objects
             * Surface staffs should NOT be recreated once created,
             * but unregister and register needs to be done since the 
             * bounding window change its position and size. At this
             * moment, reset the iWaitingBuffers and reset the callbacks
             * are recommended.
             */
            iConfig.iRemoteWindow->RemoveBackgroundSurface(ETrue);
            iConfig.iWsSession->UnregisterSurface(0, iSurfaceId);
            
            /* Comment out below code to prevent delete the surface objects 
             * at this current stage, code below should NOT be removed due to
             * potential future uses.
             */
            /*
            iSurfaceChunk->Close();
            delete iSurfaceChunk;
            iSurfaceChunk = NULL;
            iSurfaceManager->CloseSurface(iSurfaceId);
            iSurfaceUpdateSession.CancelAllUpdateNotifications();
            iSurfaceUpdateSession.Close();
            */
            
            /* Close the surface manager handle */
            /*
            iSurfaceManager->Close();
            delete iSurfaceManager;
            iSurfaceManager = 0;
            */
            }
        for ( TInt i = KVtEngMaxSurfaceBuffers-1; i >= 0 ; i-- )
            {
                if (iCallBackTable[i])
                    {
                    iCallBackTable[i]->Cancel();
                    delete iCallBackTable[i];
                    iCallBackTable[i] = NULL;
                    }
            }
        /* Comment out below code to prevent delete the surface buffers, 
         * otherwise the last buffer would not store the last frame. 
         * Code below should NOT be removed due to potential future uses.
         */
        /*
        iSurfaceBuffers[ 0 ].UnSet();
        iSurfaceBuffers[ 1 ].UnSet();
        iSurfaceBuffer0.UnSet();
        iSurfaceBuffer1.UnSet();
        */
        iWaitingBuffers.Reset();
        }
    /* Add else branch to escape the recreate of the surface objects, 
     * it could be changed in future.
     */
    else
        {
        err = iSurfaceUpdateSession.Connect();    
        User::LeaveIfError(err);
        
        iSurfaceManager = new RSurfaceManager();
        
        User::LeaveIfNull(iSurfaceManager); 
        err = iSurfaceManager->Open();
        User::LeaveIfError(err);
        
        RSurfaceManager::TSurfaceCreationAttributesBuf attributes;
        attributes().iPixelFormat           = iSurfaceFormat; 
        attributes().iSize                  = iSourceSize;
        attributes().iBuffers               = KVtEngMaxSurfaceBuffers;
        
        //attributes().iStride                = iSourceSize.iWidth*2;
        attributes().iStride                = iSourceSize.iWidth*3/2;
        
        attributes().iOffsetToFirstBuffer   = 0;
        attributes().iAlignment             = 4;
        attributes().iContiguous            = EFalse;
        attributes().iMappable              = ETrue;
        
        err = iSurfaceManager->CreateSurface(attributes, iSurfaceId);
        User::LeaveIfError(err);  
        
        // Map to chunk
        iSurfaceChunk = new RChunk();
        User::LeaveIfNull(iSurfaceChunk);    
        err = iSurfaceManager->MapSurface(iSurfaceId, *iSurfaceChunk);
        User::LeaveIfError(err);    
        
        // Get the info from the surfaceManager
        RSurfaceManager::TInfoBuf infoBuf;
        err = iSurfaceManager->SurfaceInfo( iSurfaceId, infoBuf );
        User::LeaveIfError(err);
        RSurfaceManager::TSurfaceInfoV01 info = infoBuf();

        __VTPRINT2( DEBUG_MEDIA, "chunk size=%d", iSurfaceChunk->Size() )

        TRgb color( 0, 0, 0 );

        TInt offset;

        iSurfaceManager->GetBufferOffset( iSurfaceId, 0, offset );
        __VTPRINT2( DEBUG_MEDIA, "offset0=%d", offset )
        ClearSurfacebuffer( iSurfaceChunk->Base(), offset, info, color );
        iSurfaceBuffer0.Set(iSurfaceChunk->Base() + offset, 0 );
        
        iSurfaceManager->GetBufferOffset( iSurfaceId, 1, offset );
        __VTPRINT2( DEBUG_MEDIA, "offset1=%d", offset )
        ClearSurfacebuffer( iSurfaceChunk->Base(), offset, info, color );
        iSurfaceBuffer1.Set(iSurfaceChunk->Base() + offset, 1 );
        }
    
    iConfig.iWsSession->RegisterSurface(0, iSurfaceId);   
    TSurfaceConfiguration surfaceConfig;
    surfaceConfig.SetSurfaceId( iSurfaceId );
    surfaceConfig.SetOrientation( CFbsBitGc::EGraphicsOrientationRotated270);
    iConfig.iRemoteWindow->SetBackgroundSurface( surfaceConfig, ETrue );
    //iConfig.iRemoteWindow->SetBackgroundSurface(iSurfaceId);

    iConfig.iRemoteWindow->BeginRedraw();
    iConfig.iRemoteWindow->EndRedraw();
    
    iCallBackTable[0] = new(ELeave) CActiveCallBack(
            TCallBack(SurfaceBuffer0Ready, this), 
            CActive::EPriorityStandard-1);
    CActiveScheduler::Add(iCallBackTable[0]);
    
    iCallBackTable[1] = new(ELeave) CActiveCallBack(
            TCallBack(SurfaceBuffer1Ready, this), 
            CActive::EPriorityStandard-1);
    CActiveScheduler::Add(iCallBackTable[1]);
    
    iSurfaceCreated = ETrue;    
    
    ClearFlag( EInitializePostingSurfaceCalled );
    __VTPRINTEXIT( "RVD(NGA).DoCreateSurfaceL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::ClearSurface
// clearing a surface buffer to given colour
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::ClearSurfacebuffer( 
    TUint8* aStartAddr, 
    TInt aOffset, 
    const RSurfaceManager::TSurfaceInfoV01 aInfo, 
    TRgb &aColor )
{
    TUint8 red   = aColor.Red();
    TUint8 green = aColor.Green();
    TUint8 blue  = aColor.Blue();

    // formulae borrowed from http://msdn.microsoft.com/en-us/library/ms893078.aspx
    TUint8 yval = ( (  66 * (TInt32)red + 129 * (TInt32)green +  25 * (TInt32)blue + 128) >> 8) +  16;
    TUint8 uval = ( ( -38 * (TInt32)red -  74 * (TInt32)green + 112 * (TInt32)blue + 128) >> 8) + 128;
    TUint8 vval = ( ( 112 * (TInt32)red -  94 * (TInt32)green -  18 * (TInt32)blue + 128) >> 8) + 128;

    TInt i = 0;

    aStartAddr += aOffset;

    while ( i < (aInfo.iSize.iHeight*aInfo.iSize.iWidth) )
    {
    *aStartAddr = yval;
    aStartAddr++;
    i++;
    }
    i=0;

    while ( i < ((aInfo.iSize.iHeight*aInfo.iSize.iWidth)>>2) )
        {
        *aStartAddr = uval;
        aStartAddr++;
        i++;
        }
    i=0;

    while ( i < ((aInfo.iSize.iHeight*aInfo.iSize.iWidth)>>2) )
        {
        *aStartAddr = vval;
        aStartAddr++;
        i++;
        }
}
    
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::DoUpdateSinkParamsL
// Updates sink params
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::DoUpdateSinkParamsL()
    {
    __VTPRINTENTER( "RVD(NGA).DoUpdateSinkParamsL" )
    TDisplaySinkParamsNGA params;
    GetSinkParams( params );
    DisplaySink().UpdateSinkParamsL( params, iDSUpdated );
    ClearFlag( ESinkParamUpdateRequired );
    if ( IsFlag( ESinkResumeRequired ) )
        {
        DisplaySink().Resume();
        ClearFlag( ESinkResumeRequired );
        }
    __VTPRINTEXIT( "RVD(DP).DoUpdateSinkParamsL" )
    }
      
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::CreateSurfaceL
// Create surface.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::CreateSurfaceL()
    {
    __VTPRINTENTER( "RVD(NGA).CreateSurfaceL" )
    if ( IsFlag( EBaseConstructCalled ) )
        {
        SetFlag( EInitializePostingSurfaceCalled );
        }
    else
        {
        DoCreateSurfaceL();
        }
    __VTPRINTEXIT( "RVD(NGA).CreateSurfaceL" )
    }
           
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::GetSinkParams
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::GetSinkParams( TDisplaySinkParamsNGA& aSinkParams )
    { 
    __VTPRINTENTER( "RVD(NGA).GetSinkParams" )
    Mem::FillZ( &aSinkParams, sizeof( TDisplaySinkParamsNGA ) );
    aSinkParams.iThreadId = RThread().Id();
    aSinkParams.iRequestStatusPtr = &iStatus;
    aSinkParams.iObserver = iObserver;
    aSinkParams.iSurfaceBuffer1 = &iSurfaceBuffers[ 0 ].Buffer();
    aSinkParams.iSurfaceBuffer2 = &iSurfaceBuffers[ 1 ].Buffer();
    aSinkParams.iFrameSize = iSourceSize;
    aSinkParams.iSurfaceFormat = iSurfaceFormat;
    aSinkParams.iFlags = TDisplaySinkParams::EDisplaySinkNGA;
    __VTPRINTEXITR( "RVD(NGA).GetSinkParams flags=%d",
        (TInt) aSinkParams.iFlags )	  
    }
    
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::DisplaySink
// Returns reference to sink.
// -----------------------------------------------------------------------------
//
MDisplaySink& CVtEngDrawNGA::DisplaySink()
    {
    return *iDisplaySink;
    }
     
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::SetToSink
// Sets posting buffer  to sink.
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::SetAvailableToSink( TVTSurfaceBuffer* aBuffer )
    {
    __VTPRINTENTER( "RVD(NGA).SetAvailableToSink" )
    for ( TInt i = MDisplaySink::EFirstBitmap;
          i < MDisplaySink::EFirstBitmap + KVtEngMaxSurfaceBuffers; i++ )
        {
        if ( !iSurfaceBuffers[ i ].IsSet() )
            {
            iSurfaceBuffers[ i ].Set( aBuffer );
            Activate();
            DisplaySink().SetBitmapAvailable(
                static_cast< MDisplaySink::TBitmapNo >( i ) );
            break;
            }
        }
    __VTPRINTEXIT( "RVD(NGA).SetAvailableToSink" )
    }  

TInt CVtEngDrawNGA::SurfaceBuffer0Ready( TAny* aAny )
    {
    __VTPRINTENTER( "RVD(NGA).SurfaceBuffer0Ready" )
    __VTPRINTEXIT( "RVD(NGA).SurfaceBuffer0Ready" )
    return ((CVtEngDrawNGA*)aAny)->DoSurfaceBuffer0Ready();
    }

TBool CVtEngDrawNGA::DoSurfaceBuffer0Ready()
    {
    __VTPRINTENTER( "RVD(NGA).DoSurfaceBuffer0Ready" )
    if ( iSubmitPending )
        {
        iSubmitPending = EFalse;
        }
    if (!iWaitingBuffers.IsEmpty())
        {
        TVtEngBuffer* buffer = iWaitingBuffers.First();
        TInt surfaceno = buffer->SurfaceBuffer()->SurfaceNo();
        __VTPRINT2( DEBUG_MEDIA, "RVD(NGA).DoSurfaceBuffer0Ready, surface buffer %d is in waitingqueue", surfaceno )
        iCallBackTable[surfaceno]->iStatus = KRequestPending;
        iCallBackTable[surfaceno]->SetActive();
        iSurfaceUpdateSession.NotifyWhenDisplayed(iCallBackTable[surfaceno]->iStatus, iTimeStamp);
        iSubmitPending = ETrue;
        iSurfaceUpdateSession.SubmitUpdate( KAllScreens, iSurfaceId, surfaceno, NULL ); 
        iWaitingBuffers.Remove(*buffer);
        buffer->UnSet();
        }
    iBufferWaiter->BufferAvailable(iSurfaceBuffer0);
    __VTPRINTEXIT( "RVD(NGA).DoSurfaceBuffer0Ready" )
    return ETrue;
    }
        
TInt CVtEngDrawNGA::SurfaceBuffer1Ready( TAny* aAny )
    {
    __VTPRINTENTER( "RVD(NGA).SurfaceBuffer1Ready" )
    __VTPRINTEXIT( "RVD(NGA).SurfaceBuffer1Ready" )
    return ((CVtEngDrawNGA*)aAny)->DoSurfaceBuffer1Ready();
    }

TBool CVtEngDrawNGA::DoSurfaceBuffer1Ready()
    {
    __VTPRINTENTER( "RVD(NGA).DoSurfaceBuffer1Ready" )
    if ( iSubmitPending )
        {
        iSubmitPending = EFalse;
        }
    
    if (!iWaitingBuffers.IsEmpty())
        {
        TVtEngBuffer* buffer = iWaitingBuffers.First();
        TInt surfaceno = buffer->SurfaceBuffer()->SurfaceNo();
        __VTPRINT2( DEBUG_MEDIA, "RVD(NGA).DoSurfaceBuffer0Ready, surface buffer %d is in waitingqueue", surfaceno )
        iCallBackTable[surfaceno]->iStatus = KRequestPending;
        iCallBackTable[surfaceno]->SetActive();
        iSurfaceUpdateSession.NotifyWhenDisplayed(iCallBackTable[surfaceno]->iStatus, iTimeStamp);
        iSubmitPending = ETrue;
        iSurfaceUpdateSession.SubmitUpdate( KAllScreens, iSurfaceId, surfaceno, NULL ); 
        iWaitingBuffers.Remove(*buffer);
        buffer->UnSet();
        }
    iBufferWaiter->BufferAvailable(iSurfaceBuffer1);
    __VTPRINTEXIT( "RVD(NGA).DoSurfaceBuffer1Ready" )
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::TVtEngBuffer::TVtEngBuffer
//
// -----------------------------------------------------------------------------
//
CVtEngDrawNGA::TVtEngBuffer::TVtEngBuffer()
    : iBuffer( 0 ), iSurfaceBuffer(0)
    {
    __VTPRINTENTER( "RVD(NGA).Buffer.Ctor" )
    __VTPRINTEXIT( "RVD(NGA).Buffer.Ctor" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::TVtEngBuffer::Set
//
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::TVtEngBuffer::Set( TVTSurfaceBuffer* aBuffer )
    {
    __VTPRINTENTER( "RVD(NGA).Buffer.Set" )
    iSurfaceBuffer = aBuffer;
    iBuffer = aBuffer->Buffer();
    __VTPRINTEXIT( "RVD(NGA).Buffer.Set" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::TVtEngBuffer::UnSet
//
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::TVtEngBuffer::UnSet()
    {
    __VTPRINTENTER( "RVD(NGA).Buffer.UnSet" )
    iSurfaceBuffer = 0;
    iBuffer = 0;
    __VTPRINTEXIT( "RVD(NGA).Buffer.UnSet" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::TVtEngBuffer::IsSet
//
// -----------------------------------------------------------------------------
//
TBool CVtEngDrawNGA::TVtEngBuffer::IsSet() const
    {
    return ( iSurfaceBuffer != NULL );
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::TVtEngBuffer::Buffer
//
// -----------------------------------------------------------------------------
//
TVTSurfaceBuffer* &CVtEngDrawNGA::TVtEngBuffer::SurfaceBuffer()
    {
    return iSurfaceBuffer;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::TVtEngBuffer::Buffer
//
// -----------------------------------------------------------------------------
//
TUint8* &CVtEngDrawNGA::TVtEngBuffer::Buffer()
    {
    return iBuffer;
    }
// -----------------------------------------------------------------------------
// CVtEngDrawNGA::CVtEngBufferWaiter::CVtEngBufferWaiter
// -----------------------------------------------------------------------------
//
CVtEngDrawNGA::CVtEngBufferWaiter::CVtEngBufferWaiter
    //( CVideoRenderer& aVideoRenderer, MVtEngNGABufferWaiterObserver& aObserver )
    ( MVtEngNGABufferWaiterObserver& aObserver )
    : CActive( CActive::EPriorityStandard ),
      iObserver( &aObserver )
    {
    __VTPRINTENTER( "RVD(NGA).BufferWaiter.Ctor" )
    for ( TInt i = 0; i < KVtEngMaxSurfaceBuffers; i++ )
        {
        this->iBuffers[ i ] = NULL;
        }         
    CActiveScheduler::Add( this );
    __VTPRINTEXIT( "RVD(NGA).BufferWaiter.Ctor" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::CVtEngBufferWaiter::~CVtEngBufferWaiter
// -----------------------------------------------------------------------------
//
CVtEngDrawNGA::CVtEngBufferWaiter::~CVtEngBufferWaiter()
    {
    __VTPRINTENTER( "RVD(NGA).BufferWaiter.~" )
    Cancel();
    __VTPRINTEXIT( "RVD(NGA).BufferWaiter.~" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::CVtEngBufferWaiter::Activate
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::CVtEngBufferWaiter::Activate()
    {
    __VTPRINTENTER( "RVD(NGA).BufferWaiter.Activate" )
    iStatus = KRequestPending;
    SetActive();
    __VTPRINTEXIT( "RVD(NGA).BufferWaiter.Activate" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::CVtEngBufferWaiter::FetchBuffer
// -----------------------------------------------------------------------------
//
TInt CVtEngDrawNGA::CVtEngBufferWaiter::FetchBuffer()
    {
    __VTPRINTENTER( "RVD(NGA).BufferWaiter.FetchBuffer" )
    if ( IsActive() )
    {
    	__VTPRINTEXIT( "RVD(NGA).BufferWaiter.FetchBuffer 1" )
      return KErrNone;
    }
    
    Activate();
    if (iStatus == KRequestPending)
        {
        for ( TInt i = 0; i < KVtEngMaxSurfaceBuffers; i++ )
            {
            if(this->iBuffers[ i ] != NULL)
                {
                TRequestStatus* status = &iStatus;
                User::RequestComplete( status, KErrNone );   
                break;  	
                }
            }
        }            
    __VTPRINTEXIT( "RVD(NGA).BufferWaiter.FetchBuffer 2" )    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::CVtEngBufferWaiter::BufferAvailable
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::CVtEngBufferWaiter::BufferAvailable(TVTSurfaceBuffer& aBuffer)
{
	__VTPRINTENTER( "RVD(NGA).BufferWaiter.BufferAvailable" )
    for ( TInt i = 0; i < KVtEngMaxSurfaceBuffers; i++ )
    {
      if(this->iBuffers[ i ] == NULL  )
      {
      this->iBuffers[ i ]= &aBuffer;
      break;  	
      }
    } 
    
    if(iStatus == KRequestPending )
    {
      for ( TInt i = 0; i < KVtEngMaxSurfaceBuffers; i++ )
      {
        if(this->iBuffers[ i ] != NULL)
        {
          TRequestStatus* status = &iStatus;
          User::RequestComplete( status, KErrNone );   
          break;  	
        }
      }
    }    
     	
	__VTPRINTEXIT( "RVD(NGA).BufferWaiter.BufferAvailable" )    
}

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::CVtEngBufferWaiter::RunL
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::CVtEngBufferWaiter::RunL()
    {
    __VTPRINTENTER( "RVD(NGA).BufferWaiter.RunL" )
    TInt result( iStatus.Int() );
    if (  result == KErrNone )
    {
      for ( TInt i = 0; i < KVtEngMaxSurfaceBuffers; i++ )
      {
        if(this->iBuffers[ i ] != NULL)
        {   	
          TRAP_IGNORE( iObserver->BufferReadyL( this->iBuffers[ i ] ) );
          this->iBuffers[ i ] = NULL;
          break;
        }
      }
    }
    else
        {
        iObserver->FetchError( result );
        }
    __VTPRINTEXIT( "RVD(NGA).BufferWaiter.RunL" )
    }

// -----------------------------------------------------------------------------
// CVtEngDrawNGA::CVtEngBufferWaiter::DoCancel
// -----------------------------------------------------------------------------
//
void CVtEngDrawNGA::CVtEngBufferWaiter::DoCancel()
    {
    __VTPRINTENTER( "RVD(NGA).BufferWaiter.DoCancel" )    
    
   if ( iStatus == KRequestPending )
       {
       TRequestStatus* status = &iStatus;
       User::RequestComplete(status, KErrCancel);
       }
    
    __VTPRINTEXIT( "RVD(NGA).BufferWaiter.DoCancel" )
    }            
    
//  End of File
