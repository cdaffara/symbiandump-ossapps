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
* Description:  Video Source subsystem.
*
*/


// INCLUDE FILES

#include <e32svr.h>
#include <bautils.h>
#include <imageconversion.h>
#include <cvtimageconverter.h>
#include <rphcltserver.h>
#include <cphcltimagehandler.h>
#include <w32std.h>

#include "CVSStillImageDataProvider.h"
#include "CVSDataSourceImp.h"
#include "VSPanic.h"
#include "cvtimagebitmap.h"
#include "CVSMMFDataBuffer.h"

#include "cmultiframeprovider.h"

// EXTERNAL FUNCTION PROTOTYPES

// MACROS

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// LOCAL CONSTANTS AND MACROS

static const TInt KMinZoom = 0;
static const TInt KMaxZoom = 0;
static const TInt KMaxDigitalZoom = 0;
static const TReal32 KMinZoomFactor = 0.0;
static const TReal32 KMaxZoomFactor = 0.0;
static const TReal32 KMaxDigitalZoomFactor = 1.0;
static const TInt KNumVideoSizesSupported = 1;
static const TInt KNumVideoFrameSizesSupported = 1;
static const TInt KNumVideoFrameRatesSupported = 1;
static const TInt KMaxFramesPerBufferSupported = 1;
static const TInt KQcifWidth = 176;
static const TInt KQcifHeight = 144;
static const TReal32 KFrameRate = 15.0;
static const TUint32 KVSViewFinderUpdateRate = 1000000/15; // 15 times per second

// TYPE DEFINITIONS

// Timer expired callback

typedef void (CVSStillImageDataProvider::*TTimerElapsed) ();

// INTERNAL CLASSES

/**
*  Timer class that calls given callback method when timer request has been
*  completed. If the timer request is cancelled then callback will not be
*  called.
*
*  @lib videosource.lib
*/
class CVSFrameRateTimer : public CTimer
    {
    public: // Constructors and destructor

        /**
        * Static factory function to create instance of this class.
        * @param "aRequester" ?description
        * @exception Can leave with one of the system wide error codes.
        * @return Pointer to new instance.
        */
        static CVSFrameRateTimer* NewL( CVSStillImageDataProvider* aRequester );

    public: // New functions

    public: // Functions from base classes

        /**
        * Starts timer request. When timer request has been completed given
        * callback method will be called.
        * @param "anInterval" Interval after which event is to occur,
        * in microseconds.
        * @param "aFunction" Callback method that will be called when timer
        * request has been completed.
        */
        void After(
            TTimeIntervalMicroSeconds32 anInterval,
            TTimerElapsed aFunction  );

    public: // Constructors and destructor

        /**
        * Destructor.
        */
        ~CVSFrameRateTimer();

    protected:  // New functions

    protected:  // Functions from base classes

    private:    // Constructors and destrcutor

        /**
        * Constructor.
        * @param "aRequester" ?description
        */
        CVSFrameRateTimer( CVSStillImageDataProvider* aRequester );

    private:    // New functions

    private:    // Functions from base classes

        /**
        * From CActive. See CActive for description.
        */
        void RunL();

    public: // Data

    protected:  // Data

    private:    // Data

        // Pointer to requester (i.e. through which callback call will be made)
        CVSStillImageDataProvider* iRequester; // not owned

        // Pointer to callback method.
        TTimerElapsed iFunction;
    };

// ============================ MEMBER FUNCTIONS ===============================

// ============================ CVSFrameRateTimer ==============================

// -----------------------------------------------------------------------------
// CVSFrameRateTimer::NewL( CVSStillImageDataProvider* aRequester )
// -----------------------------------------------------------------------------
//
CVSFrameRateTimer* CVSFrameRateTimer::NewL(
    CVSStillImageDataProvider* aRequester )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSFrameRateTimer::NewL() >>"), RThread().Id().operator TUint()));
    CVSFrameRateTimer* self = new (ELeave) CVSFrameRateTimer( aRequester );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSFrameRateTimer::NewL() <<"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CVSFrameRateTimer::CVSFrameRateTimer( CVSStillImageDataProvider* aRequester )
// -----------------------------------------------------------------------------
//
CVSFrameRateTimer::CVSFrameRateTimer( CVSStillImageDataProvider* aRequester )
: CTimer( EPriorityStandard ), iRequester( aRequester )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSFrameRateTimer::CVSFrameRateTimer() >>"), RThread().Id().operator TUint()));
    // Added to CActiveScheduler in CVSStillImageDataProvider::InitializeL
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSFrameRateTimer::CVSFrameRateTimer() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSFrameRateTimer::After(
//  TTimeIntervalMicroSeconds32 anInterval, TTimerElapsed aFunction )
// -----------------------------------------------------------------------------
//
void CVSFrameRateTimer::After(
    TTimeIntervalMicroSeconds32 anInterval,
    TTimerElapsed aFunction )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSFrameRateTimer::After() >>"), RThread().Id().operator TUint()));
    if ( iFunction == aFunction )
    	{
    	if ( !IsActive() )
    		{
      	iFunction = aFunction;
    		CTimer::After( anInterval );
    		}
    	}
    else
    	{
    	if ( !IsActive() )
    		{
      	iFunction = aFunction;
    		CTimer::After( anInterval );
    		}
    	else
    		{
    			User::Panic(_L("VideoSource"),KErrGeneral);
    		}
    	}
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSFrameRateTimer::After() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSFrameRateTimer::~CVSFrameRateTimer()
// -----------------------------------------------------------------------------
//
CVSFrameRateTimer::~CVSFrameRateTimer()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSFrameRateTimer::~CVSFrameRateTimer() >>"), RThread().Id().operator TUint()));
    Cancel();
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSFrameRateTimer::~CVSFrameRateTimer() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSFrameRateTimer::RunL()
// -----------------------------------------------------------------------------
//
void CVSFrameRateTimer::RunL()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSFrameRateTimer::RunL() >>"), RThread().Id().operator TUint()));
    if( iStatus.Int() == KErrNone )
        {
        (iRequester->*iFunction)();
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSFrameRateTimer::RunL() <<"), RThread().Id().operator TUint()));
    }

// ============================ CVSStillImageDataProvider ===============================

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::NewL(
//  MVSDataProviderObserver* aObserver, MVSBufferPool* aPool )
// -----------------------------------------------------------------------------
//
CVSStillImageDataProvider* CVSStillImageDataProvider::NewL(
    MVSDataProviderObserver* aObserver,
    MVSBufferPool* aPool )
    {
    __IF_DEBUG(Print(_L("VideoSource: [%d] CVSStillImageDataProvider::NewL() >>"), RThread().Id().operator TUint()));
    CVSStillImageDataProvider* self =
    new (ELeave) CVSStillImageDataProvider( aObserver, aPool );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    __IF_DEBUG(Print(_L("VideoSource: [%d] CVSStillImageDataProvider::NewL() <<"), RThread().Id().operator TUint()));
    return self;
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::~CVSStillImageDataProvider()
// -----------------------------------------------------------------------------
//
CVSStillImageDataProvider::~CVSStillImageDataProvider()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::~CVSStillImageDataProvider() this %x >>"), RThread().Id().operator TUint(), this));
    iFs.Close();
    delete iVFTimer;
    delete iActiveWait;
    delete iViewer;
    iFreezeCS.Close();
    delete iScaled;
    ReleaseYUVData();
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::~CVSStillImageDataProvider() this %x <<"), RThread().Id().operator TUint(), this));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::InitializeL( const TDesC8& aInitParams )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::InitializeL( const TDesC8& aInitParams )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::InitializeL() >>"), RThread().Id().operator TUint()));

    CommonInitializeL();

    TPckgBuf< TVSStillImageDataProviderParams > pb;
    pb.Copy( aInitParams );

    iInitType = pb().iInitialize;
    iPSState = EPSInitializing;

    switch( pb().iInitialize )
        {
        /** Initialize from blank image */
        case TVSStillImageDataProviderParams::EInitializeAsBlankImage:
            BlankImageInitializeL();
            break;

        /** Initialize from General Settings */
        case TVSStillImageDataProviderParams::EInitializeFromGeneralSettings:
            GeneralSettingsInitializeL( EGSStillImage );
            break;

        /** Initilaize from General Settings*/
        case TVSStillImageDataProviderParams::EInitializeAsDefaultStillImage:
            GeneralSettingsInitializeL( EGSDefaultStillImage );
            break;

        /** Initialize from file */
        case TVSStillImageDataProviderParams::EInitializeFromFile:
            FileImageInitializeL( pb().iFileName );
            break;
        }

    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::InitializeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::CommonInitializeL()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::CommonInitializeL()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::CommonInitializeL() >>"), RThread().Id().operator TUint()));
    iVFTimer = CVSFrameRateTimer::NewL( this );
    CActiveScheduler::Add( iVFTimer );
    iActiveWait = new (ELeave) CVSActiveWait< CVSStillImageDataProvider > ( this );

    //Create viewer
    iViewer = CMultiframeProvider::NewL( this , iDisplayMode );
    User::LeaveIfError( iFreezeCS.CreateLocal() );

    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::CommonInitializeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::GeneralSettingsInitializeL()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::GeneralSettingsInitializeL( const TGeneralSettingsImageType aType )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::GeneralSettingsInitializeL() >>"), RThread().Id().operator TUint()));
		iViewer->IniatializeGSL( aType );
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::GeneralSettingsInitializeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::BlankImageInitializeL()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::BlankImageInitializeL()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::BlankImageInitializeL() %x >>"), RThread().Id().operator TUint(), this));
		iViewer->IniatializeBlankL();
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::BlankImageInitializeL() %x <<"), RThread().Id().operator TUint(), this));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::FileImageInitializeL
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::FileImageInitializeL( const TFileName& aFileName )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::FileImageInitializeL(): %S >>"), RThread().Id().operator TUint(), &aFileName ));
		iViewer->InitializeL( &aFileName );
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::FileImageInitializeL() %x <<"), RThread().Id().operator TUint(), this));
    }


// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::DigitalZoomFactor() const
// -----------------------------------------------------------------------------
//
TInt CVSStillImageDataProvider::DigitalZoomFactor() const
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::DigitalZoomFactor() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::DigitalZoomFactor() <<"), RThread().Id().operator TUint()));
    return KMaxDigitalZoom;
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SetDigitalZoomFactorL(
//  TInt /*aDigitalZoomFactor*/ )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SetDigitalZoomFactorL(
    TInt /*aDigitalZoomFactor*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SetDigitalZoomFactorL() >>"), RThread().Id().operator TUint()));
    User::Leave( KErrNotSupported );
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SetDigitalZoomFactorL() <<"), RThread().Id().operator TUint()));
    }
// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::StartViewFinderBitmapsL( TSize& aSize )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::StartViewFinderBitmapsL( TSize& aSize )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::StartViewFinderBitmapsL() >>"), RThread().Id().operator TUint()));
    __ASSERT_DEBUG( iPSState > EPSUninitialized,
        Panic( EVSPanicProviderNotReady ) );
    iVFState = EVFPlaying;
    iViewer->ClearVFScalingTargetSize();
    iTargetSize = aSize;
    iViewer->SetVFScalingTargetSize( aSize );
    iViewer->ScaleCopy( aSize, ETrue );
    iViewer->IncreaseDataConsumer();
    iViewer->NextFrame();
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::StartViewFinderBitmapsL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::StartViewFinderBitmapsL(
//  TSize& /*aSize*/, TRect& /*aClipRect*/ )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::StartViewFinderBitmapsL(
    TSize& /*aSize*/,
    TRect& /*aClipRect*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::StartViewFinderBitmapsL() >>"), RThread().Id().operator TUint()));
    User::Leave( KErrNotSupported );
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::StartViewFinderBitmapsL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::StopViewFinder()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::StopViewFinder()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::StopViewFinder() >>"), RThread().Id().operator TUint()));
    // Make sure scaler is cancelled (just in case!)
    iViewer->ClearVFScalingTargetSize();
    iViewer->DecreaseDataConsumer();
    iViewer->SetVFStop();
    // No double stopping..
    if( iVFState != EVFStopped )
        {
        iVFState = EVFStopped;
        iVFTimer->Cancel();
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::StopViewFinder() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::ViewFinderActive() const
// -----------------------------------------------------------------------------
//
TBool CVSStillImageDataProvider::ViewFinderActive() const
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::ViewFinderActive() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::ViewFinderActive() <<"), RThread().Id().operator TUint()));
    return ( iVFState == EVFPlaying );
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::StartViewFinderDirectL(
//  RWsSession& /*aWs*/, CWsScreenDevice& /*aScreenDevice*/,
//  RWindowBase& /*aWindow*/, TRect& /*aScreenRect*/ )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::StartViewFinderDirectL(
    RWsSession& /*aWs*/,
    CWsScreenDevice& /*aScreenDevice*/,
    RWindowBase& /*aWindow*/,
    TRect& /*aScreenRect*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::StartViewFinderDirectL() >>"), RThread().Id().operator TUint()));
    User::Leave(KErrNotSupported);
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::StartViewFinderDirectL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::StartViewFinderDirectL(
//  RWsSession& /*aWs*/, CWsScreenDevice& /*aScreenDevice*/,
//  RWindowBase& /*aWindow*/, TRect& /*aScreenRect*/, TRect& /*aClipRect*/ )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::StartViewFinderDirectL(
    RWsSession& /*aWs*/,
    CWsScreenDevice& /*aScreenDevice*/,
    RWindowBase& /*aWindow*/,
    TRect& /*aScreenRect*/,
    TRect& /*aClipRect*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::StartViewFinderDirectL() >>"), RThread().Id().operator TUint()));
    User::Leave(KErrNotSupported);
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::StartViewFinderDirectL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::ProviderInfo( TVSDataProviderInfo& aInfo )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::ProviderInfo( TVSDataProviderInfo& aInfo )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::ProviderInfo() >>"), RThread().Id().operator TUint()));
    Mem::FillZ( &aInfo, sizeof( aInfo ) );
    aInfo.iHardwareVersion = TVersion( 0, 0, 0 );
    aInfo.iSoftwareVersion = TVersion( 0, 0, 0 );
    aInfo.iOrientation = TCameraInfo::EOrientationUnknown;
    aInfo.iOptionsSupported = TCameraInfo::EViewFinderBitmapsSupported    |
                              TCameraInfo::EVideoCaptureSupported;
    aInfo.iFlashModesSupported = CCamera::EFlashNone;
    aInfo.iExposureModesSupported = CCamera::EExposureAuto;
    aInfo.iWhiteBalanceModesSupported = CCamera::EWBAuto;
    aInfo.iMinZoom = KMinZoom;
    aInfo.iMaxZoom = KMaxZoom;
    aInfo.iMaxDigitalZoom = KMaxDigitalZoom;
    aInfo.iMinZoomFactor = KMinZoomFactor;
    aInfo.iMaxZoomFactor = KMaxZoomFactor;
    aInfo.iMaxDigitalZoomFactor = KMaxDigitalZoomFactor;
    aInfo.iImageFormatsSupported = CCamera::EFormatFbsBitmapColor64K |
                                   CCamera::EFormatFbsBitmapColor16M;
    aInfo.iNumVideoFrameSizesSupported = KNumVideoSizesSupported;
    aInfo.iNumVideoFrameSizesSupported = KNumVideoFrameSizesSupported;
    aInfo.iNumVideoFrameRatesSupported = KNumVideoFrameRatesSupported;
    aInfo.iVideoFrameFormatsSupported = CCamera::EFormatYUV420Planar;
    aInfo.iMaxFramesPerBufferSupported = KMaxFramesPerBufferSupported;
    aInfo.iFreezeSupported = EFalse;

    aInfo.iSupportedColorTones =
        CCamera::CCameraImageProcessing::EEffectNone;

    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::ProviderInfo() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SetViewFinderMirrorL
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SetViewFinderMirrorL( TBool /*aMirror*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::ViewFinderMirror
// -----------------------------------------------------------------------------
//
TBool CVSStillImageDataProvider::ViewFinderMirror() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::FreezeL
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::FreezeL()
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::UnfreezeL
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::UnfreezeL()
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::IsFrozen
// -----------------------------------------------------------------------------
//
TBool CVSStillImageDataProvider::IsFrozen() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::PrimeL
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::PrimeL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::PrimeL() &%x >>"), RThread().Id().operator TUint(), this));
    SourcePrimeL();
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::PrimeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::FillBufferL(
//  CMMFBuffer* aBuffer, MVTVideoSink* aConsumer, TMediaId aMediaId )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::FillBufferL(
    CMMFBuffer* /*aBuffer*/,
    MVTVideoSink* /*aConsumer*/,
    TMediaId /*aMediaId*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::FillBufferL() &%x >>"), RThread().Id().operator TUint(),this));
    if ( iPSState != EPSPlaying )
        {
        User::Leave( KErrNotReady );
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::FillBufferL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::BufferEmptiedL( CMMFBuffer* /*aBuffer*/ )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::BufferEmptiedL( CMMFBuffer* /*aBuffer*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::BufferEmptiedL() >>"), RThread().Id().operator TUint()));
    User::Leave( KErrNotSupported );
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::BufferEmptiedL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SourceThreadLogon(
//  MAsyncEventHandler& /*aEventHandler*/ )
// -----------------------------------------------------------------------------
//
TInt CVSStillImageDataProvider::SourceThreadLogon(
    MAsyncEventHandler& /*aEventHandler*/ )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SourceThreadLogon() &%x >>"), RThread().Id().operator TUint(),this));
    __ASSERT_DEBUG( iPSState > EPSUninitialized,
        Panic( EVSPanicProviderNotReady ) );
    TRAPD( err, CreateProtoTimerL() );
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SourceThreadLogon() <<"), RThread().Id().operator TUint()));
    return err;
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SourceThreadLogoff()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SourceThreadLogoff()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SourceThreadLogoff() &%x >>"), RThread().Id().operator TUint(),this));
    ReleaseProtoTimer();
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SourceThreadLogoff() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SourcePrimeL()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SourcePrimeL()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SourcePrimeL() &%x >>"), RThread().Id().operator TUint(),this));
    iPSState = EPSPrimed;
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SourcePrimeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SourcePlayL()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SourcePlayL()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SourcePlayL() &%x  >>"), RThread().Id().operator TUint(),this));
    iPSState = EPSPlaying;
    iFrameCount = 0;
    ResetStartTime();
    iViewer->IncreaseDataConsumer();
    iProtoTimer->After( iProtoUpdateRate, &CVSStillImageDataProvider::ProtoTimer );
    iViewer->NextFrame();
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SourcePlayL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SourcePauseL()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SourcePauseL()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SourcePauseL() &%x >>"), RThread().Id().operator TUint(),this));
    if ( iProtoTimer != NULL )
    	{
    	iProtoTimer->Cancel();
    	}
    iPSState = EPSPrimed;
    iViewer->DecreaseDataConsumer();
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SourcePauseL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SourceStopL()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SourceStopL()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SourceStopL() &%x >>"), RThread().Id().operator TUint(),this));
    if ( iProtoTimer != NULL )
    	{
    	iProtoTimer->Cancel();
    	}
    iPSState = EPSStopped;
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SourceStopL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SetFormatL( const TDesC8& aFormat )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SetFormatL( const TDesC8& aFormat )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::SetFormatL() &%x >>"), RThread().Id().operator TUint(),this));
    if ( aFormat != KVtVideoMIMETypeYUV420 )
        {
        User::Leave( KErrNotSupported );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::SetFormatL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SetFrameRateL( TReal32 aFrameRate )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SetFrameRateL( TReal32 aFrameRate )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::SetFrameRateL() &%x >>"), RThread().Id().operator TUint(),this));
    if ( aFrameRate != KFrameRate )
        {
        User::Leave( KErrNotSupported );
        }
    iProtoUpdateRate = static_cast<TInt>( 1000000.0 / aFrameRate );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::SetFrameRateL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SetVideoFrameSizeL( const TSize& aSize )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SetVideoFrameSizeL( const TSize& aSize )
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::SetVideoFrameSizeL() &%x >>"), RThread().Id().operator TUint(),this));
    if( aSize != TSize( KQcifWidth, KQcifHeight ) )
        {
        User::Leave( KErrNotSupported );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::SetVideoFrameSizeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::GetVideoFrameSizeL( TSize& aSize ) const
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::GetVideoFrameSizeL( TSize& aSize ) const
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::GetVideoFrameSizeL() &%x >>"), RThread().Id().operator TUint(),this));
    aSize = TSize( KQcifWidth, KQcifHeight );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::GetVideoFrameSizeL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::VFTimer()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::VFTimer()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::VFTimer() >>"), RThread().Id().operator TUint()));
    if( iVFState == EVFPlaying )
        {
        Observer().vsViewFinderFrameReady( iVFBitmap->Bitmap() );
        iVFTimer->After( KVSViewFinderUpdateRate, &CVSStillImageDataProvider::VFTimer );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::VFTimer() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::ProtoTimer()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::ProtoTimer()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::ProtoTimer() this %x >>"), RThread().Id().operator TUint(), this));
    iFrameCount++;
    CVSMMFDataBuffer* buffer = NULL;
    TRAPD( err, buffer = BufferPool().GetBufferL( ETrue ) );
    if ( err != KErrNone )
        {
        NotifyError( err );
        }
    if( buffer )
        {
        buffer->SetFrameNumber( iFrameCount );
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::ProtoTimer(): iFrameCount %d"), RThread().Id().operator TUint(), iFrameCount));
        buffer->SetTimeToPlay( TimeToPlay() );
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::ProtoTimer(): SetTimeToPlay() = $%08x:$%08x"), RThread().Id().operator TUint(), I64HIGH( buffer->TimeToPlay().Int64() ), I64LOW( buffer->TimeToPlay().Int64() ) ));
       iFreezeCS.Wait();
       buffer->Data() = *iYUVBuffer;
       iFreezeCS.Signal();
       TRAPD( err, Consumer()->BufferFilledL( buffer ) );
       if( err != KErrNone )
          {
          TInt leaveCode = KErrNone;
          TRAP( leaveCode, BufferPool().FreeBufferL( buffer ) );
          NotifyError( err );
          }
       }
    else
        {
        __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::ProtoTimer(): GetBufferL() returned NULL"), RThread().Id().operator TUint()));
        }
    // Just to make sure that we're actually playing. We also check that
    // iProtoTimer still exists BECAUSE BufferFilledL() may call
    // SourceThreadLogoff() if a switch is pending
    if( iProtoTimer && ( iPSState == EPSPlaying ) )
        {
        iProtoTimer->After( iProtoUpdateRate, &CVSStillImageDataProvider::ProtoTimer );
        }
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::ProtoTimer() this %x <<"), RThread().Id().operator TUint(), this));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::CreateProtoTimerL()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::CreateProtoTimerL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::CreateProtoTimerL() >>"), RThread().Id().operator TUint()));
    iProtoTimer = CVSFrameRateTimer::NewL( this );
    CActiveScheduler::Add( iProtoTimer );
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::CreateProtoTimerL() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::ReleaseProtoTimer()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::ReleaseProtoTimer()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::ReleaseProtoTimer() >>"), RThread().Id().operator TUint()));
    delete iProtoTimer;
    iProtoTimer = 0;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::ReleaseProtoTimer() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::CreateYUVDataL()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::CreateYUVDataL()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::CreateYUVDataL() this %x >>"), RThread().Id().operator TUint(), this));
    CVSFbsBitmapIYUVConverter* conv =
    CVSFbsBitmapIYUVConverter::NewL( iYUVBitMap->Bitmap() );
    CleanupStack::PushL( conv );
    conv->ProcessL();
    iFreezeCS.Wait();
    ReleaseYUVData();
    iYUVBuffer = conv->YUVData().AllocL();
    iFreezeCS.Signal();
    CleanupStack::PopAndDestroy(); //  conv
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::CreateYUVDataL() iYUVBuffer %x <<"), RThread().Id().operator TUint(), iYUVBuffer));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::ReleaseYUVData()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::ReleaseYUVData()
    {
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::ReleaseYUVData() >>"), RThread().Id().operator TUint()));
    delete iYUVBuffer;
    iYUVBuffer = 0;
    __IF_DEBUG(Print(_L("VideoSource[%d]: CVSStillImageDataProvider::ReleaseYUVData() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::CVSStillImageDataProvider(
//  MVSDataProviderObserver* aObserver )
// -----------------------------------------------------------------------------
//
CVSStillImageDataProvider::CVSStillImageDataProvider(
    MVSDataProviderObserver* aObserver,
    MVSBufferPool* aPool ) :
        CVSDataProvider( aObserver, aPool ), iProtoUpdateRate( 1000000.0 / KFrameRate )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::CVSStillImageDataProvider() >>"), RThread().Id().operator TUint()));
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::CVSStillImageDataProvider() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::ConstructL()
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::ConstructL()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::ConstructL() this %x >>"), RThread().Id().operator TUint(), this));
    CVSDataProvider::ConstructL();
    User::LeaveIfError( iFs.Connect() );
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CleanupClosePushL( wsSession );
    CWsScreenDevice* wsScreenDevice = new ( ELeave ) CWsScreenDevice( wsSession );
    CleanupStack::PushL( wsScreenDevice );
    User::LeaveIfError( wsScreenDevice->Construct() );
    //iDisplayMode = wsScreenDevice->DisplayMode();/* = EColor16MU;*/
    iDisplayMode = EColor16MU;
    CleanupStack::PopAndDestroy( 2 ); // wsSession, wsScreenDevice
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::ConstructL() this %x <<"), RThread().Id().operator TUint(), this));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::GetVideoFrameSize(
//  TSize& aSize, TInt /*aSizeIndex*/, const TDesC8& aFormat )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::GetVideoFrameSize(
    TSize& aSize,
    TInt /*aSizeIndex*/,
    const TDesC8& aFormat )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::GetVideoFrameSize() >>"), RThread().Id().operator TUint()));
    if ( aFormat == KVtVideoMIMETypeYUV420 )
        {
        aSize.SetSize( KQcifWidth, KQcifHeight );
        }
    else
        {
        aSize.SetSize( 0, 0 );
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::GetVideoFrameSize() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::GetVideoFrameRate(
//  TReal32& aRate, TInt /*aRateIndex*/, const TDesC8& aFormat,
//  const TSize& aSize )
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::GetVideoFrameRate(
    TReal32& aRate,
    TInt /*aRateIndex*/,
    const TDesC8& aFormat,
    const TSize& aSize )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::GetVideoFrameRate() >>"), RThread().Id().operator TUint()));
    if ( aFormat == KVtVideoMIMETypeYUV420 &&
        aSize == TSize( KQcifWidth, KQcifHeight ) )
        {
        aRate = KFrameRate;
        }
    else
        {
        aRate = 0.0;
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::GetVideoFrameRate() <<"), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::DisplayMode
// -----------------------------------------------------------------------------
//
TDisplayMode CVSStillImageDataProvider::DisplayMode() const
	{
#ifdef _DEBUG
    TUint threadId( RThread().Id() );
    switch( iDisplayMode )
        {
        case EColor4K:
            RDebug::Print( _L( "VideoSource [%d]: DisplayMode() = EColor4K" ), threadId );
            break;
        case EColor64K:
            RDebug::Print( _L( "VideoSource [%d]: DisplayMode() = EColor64K" ), threadId );
            break;
        case EColor16M:
            RDebug::Print( _L( "VideoSource [%d]: DisplayMode() = EColor16M" ), threadId );
            break;
        case EColor16MU:
            RDebug::Print( _L( "VideoSource [%d]: DisplayMode() = EColor16MU" ), threadId );
            break;
        default:
            RDebug::Print( _L( "VideoSource [%d]: DisplayMode() = %d" ), threadId, iDisplayMode );
            break;
        }
#endif // _DEBUG
    return iDisplayMode;
	}

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SetContrastL
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SetContrastL(TInt /*aContrast*/)
	{
	__IF_DEBUG(Print(_L("VideoSource [%d]:CVSStillImageDataProvider::SetContrastL() LEAVE"), RThread().Id().operator TUint()));
	User::Leave( KErrNotSupported );
	}
// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::GetContrast
// -----------------------------------------------------------------------------
//
TInt CVSStillImageDataProvider::GetContrast(TInt& /*aContrast*/) const
	{
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SetBrightnessL
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SetBrightnessL(TInt /*aBrightness*/)
	{
	__IF_DEBUG(Print(_L("VideoSource [%d]:CVSStillImageDataProvider::SetBrightnessL() LEAVE"), RThread().Id().operator TUint()));
	User::Leave( KErrNotSupported );
	}
// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::GetBrightness
// -----------------------------------------------------------------------------
//
TInt CVSStillImageDataProvider::GetBrightness(TInt& /*aBrightness*/ ) const
	{
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SetWhiteBalanceL
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SetWhiteBalanceL( CCamera::TWhiteBalance /*aWhiteBalance*/ )
	{
	__IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SetWhiteBalanceL() LEAVE"), RThread().Id().operator TUint()));
	User::Leave( KErrNotSupported );
	}
// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::GetWhiteBalance
// -----------------------------------------------------------------------------
//
 TInt CVSStillImageDataProvider::GetWhiteBalance(CCamera::TWhiteBalance& /*aWhiteBalance*/) const
	{
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::SetColorToneL
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::SetColorToneL( CCamera::CCameraImageProcessing::TEffect /*aValue*/ )
	{
	__IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::SetColorToneL() LEAVE"), RThread().Id().operator TUint()));
	User::Leave( KErrNotSupported );
	}
// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::GetColorTone
// -----------------------------------------------------------------------------
//
TInt CVSStillImageDataProvider::GetColorTone( CCamera::CCameraImageProcessing::TEffect& /*aColorTone*/ ) const
	{
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::InitializeReady
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::InitializeReady()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::InitializeReady()>> "), RThread().Id().operator TUint()));
    if ( EPSUndoInitializing == iPSState )
        {
        NotifyError(KErrCancel);
        UndoCommonInitialized();
        iPSState = EPSUninitialized;
        }
    else
        {
        iPSState = EPSStopped;
        Observer().vsProviderReady();
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::InitializeReady()<< "), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::RefreshYUVData
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::RefreshYUVData( TImageShareDataBuffer& aYUVBitMap )
	{
	__IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::RefreshYUVData()>> "), RThread().Id().operator TUint()));
		iYUVBitMap = aYUVBitMap.iBitmap;
		//Create YUV buffer only in play state
		TRAP_IGNORE ( CreateYUVDataL() );
		aYUVBitMap.iIsBitmapFree = ETrue;
		__IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::RefreshYUVData()<< "), RThread().Id().operator TUint()));
	}

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::RefreshViewFinder
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::RefreshViewFinder( TImageShareDataBuffer& aVFBitMap )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::RefreshViewFinder()>> "), RThread().Id().operator TUint()));
    iVFBitmap = aVFBitMap.iBitmap;
    aVFBitMap.iIsBitmapFree = ETrue;
    // Allow VF refresh only if size is what
    // has beed aquired and play state is on
    if ( ( iTargetSize == iVFBitmap->Size() ) && ( iVFState == EVFPlaying ) )
        {
        iVFTimer->After( 1, &CVSStillImageDataProvider::VFTimer );
        }
    else
        {
        __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::RefreshViewFinder() conditon doesn't match, iVFState[%d] "), RThread().Id().operator TUint(), iVFState));
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::RefreshViewFinder()<< "), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::NotifyImageHandlingError
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::NotifyImageHandlingError( TInt aError )
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::NotifyImageHandlingError()>> "), RThread().Id().operator TUint()));
    NotifyError( aError );
    if ( ( KErrCancel == aError ) && ( EPSUndoInitializing == iPSState ) )
        {
        UndoCommonInitialized();
        iPSState = EPSUninitialized;
        }
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::NotifyImageHandlingError()<< "), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::CancelInitializing
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::CancelInitializing()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::CancelInitializing()>> "), RThread().Id().operator TUint()));
    if ( iPSState != EPSInitializing )
        {
        __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::CancelInitializing(), iPSState is not valid,%d>> "), RThread().Id().operator TUint(), iPSState));
        return;
        }
    
    if ( iInitType != TVSStillImageDataProviderParams::EInitializeFromFile )
        {
        __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::CancelInitializing(), iInitType is not valid,%d>> "), RThread().Id().operator TUint(), iInitType));
        return;
        }
    if ( iViewer->CancelInitializing() )
        {
        // In this condition, convert was cancelled , no any message will receive from multiframe provider, 
        // so delete multiFrame provider, and change IPSState, NotifyError to VT Engine
        NotifyError(KErrCancel);
        UndoCommonInitialized();
        iPSState = EPSUninitialized;
        }
    else
        {
        // In this condition, cnverting is finished, and Scale will cancel(CVtImageScaler->Cancel()), but in CVtImageScaler::DoCancel(),
        // there is a error notify exist, so we must wait the error message from multiframe provider,
        // we change the iPSState to EPSUndoInitializing for waiting error message.
        // Error message will be received in function CVSStillImageDataProvider::NotifyImageHandlingError
        iPSState = EPSUndoInitializing;
        }
    
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::CancelInitializing()<< "), RThread().Id().operator TUint()));
    }

// -----------------------------------------------------------------------------
// CVSStillImageDataProvider::UndoCommonInitialized
// -----------------------------------------------------------------------------
//
void CVSStillImageDataProvider::UndoCommonInitialized()
    {
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::UndoCommonInitialized()>> "), RThread().Id().operator TUint()));

    iFreezeCS.Close();
    if ( NULL != iVFTimer )
        {
        iVFTimer->Cancel();
        }
    
    delete iViewer;
    iViewer = NULL;
    
    delete iActiveWait;
    iActiveWait = NULL;
    
    delete iVFTimer;
    iVFTimer = NULL;
    __IF_DEBUG(Print(_L("VideoSource [%d]: CVSStillImageDataProvider::UndoCommonInitialized()<< "), RThread().Id().operator TUint()));

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
