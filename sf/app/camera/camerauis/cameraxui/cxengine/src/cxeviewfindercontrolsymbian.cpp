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
#include <w32std.h> // RWindow, RWsSession.
#include <coemain.h> // CCoeEnv
#include <coecntrl.h>
#include <ecam/ecamdirectviewfinder.h>
#include "cxutils.h"
#include "cxecameradevicecontrolsymbian.h" // CxeCameraDevice
#include "cxeviewfindercontrolsymbian.h"
#include "cxesettings.h"
#include "cxesettingsmappersymbian.h"
#include "cxeerrormappingsymbian.h"
#include "cxestate.h"
#include "cxevideocontainer.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxeviewfindercontrolsymbianTraces.h"
#endif



/**
* CxeViewfinderControlSymbian::CxeViewfinderControlSymbian
*/
CxeViewfinderControlSymbian::CxeViewfinderControlSymbian( CxeCameraDevice &cameraDevice,
        CxeCameraDeviceControl &cameraDeviceControl )
    : CxeStateMachine("CxeViewfinderControlSymbian"),
      mCameraDevice( cameraDevice ),
      mCameraDeviceControl( cameraDeviceControl ),
      mUiWindow(NULL),
      mVideoWindow(NULL),
      mVideoContainer(NULL),
      mDirectViewfinder( NULL ),
      mDirectViewfinderInUse( true )
{
    CX_DEBUG_ENTER_FUNCTION();

    qRegisterMetaType<CxeViewfinderControlSymbian::State>();
    initializeStates();

    // connect signals from cameraDevice, so we recieve events when camera reference changes
    connect(&mCameraDevice, SIGNAL(prepareForCameraDelete()),
            this,SLOT(prepareForCameraDelete()));

    connect(&mCameraDevice, SIGNAL(prepareForRelease()),
            this,SLOT(prepareForRelease()));

    connect(&mCameraDevice, SIGNAL(cameraAllocated(CxeError::Id)),
            this,SLOT(handleCameraAllocated(CxeError::Id)));

    CX_DEBUG_EXIT_FUNCTION();
}



/**
* CxeViewfinderControlSymbian::~CxeViewfinderControlSymbian()
*/
CxeViewfinderControlSymbian::~CxeViewfinderControlSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    // do something
    stop(); // stop the vf
    releaseCurrentViewfinder(); // release resoruces

    delete mVideoContainer;
    mUiWindow = NULL;
    mVideoWindow = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}




/**
* CxeViewfinderControlSymbian::setWindow
*/
void CxeViewfinderControlSymbian::setWindow(WId windowId)
{
    CX_DEBUG_ENTER_FUNCTION();

    mUiWindow = static_cast<CCoeControl*>(windowId)->DrawableWindow();

    CX_DEBUG(("mUiWindow is 0x%08x", mUiWindow));
    TSize windowSize = mUiWindow->Size();

    CX_DEBUG(("mUiWindow size %dx%d", windowSize.iWidth, windowSize.iHeight));

    // Set the window rect
    TPoint point = TPoint(0,0);
    mWindowRect = TRect(point, windowSize);

    CX_DEBUG(("CxeViewfinderControlSymbian::setWindow() mWindowRect iTl=(%d, %d) iBr=(%d, %d)",
              mWindowRect.iTl.iX, mWindowRect.iTl.iY, mWindowRect.iBr.iX, mWindowRect.iBr.iY));

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Returns Device's Display resolution
*/
QSize CxeViewfinderControlSymbian::deviceDisplayResolution() const
{
    TSize windowSize(640, 360); // magic: default size if mUiWindow is NULL

    if (mUiWindow) {
        windowSize = mUiWindow->Size();
    }

    QSize displaySize = QSize(windowSize.iWidth, windowSize.iHeight);

    if (displaySize.height() > displaySize.width()) {
        // Window server orientation might differ from the Orbit UI orientation.
        // Swap width and height if needed.
        displaySize.transpose();
    }

    CX_DEBUG(("deviceDisplayResolution returning %dx%d", displaySize.width(),
                                                         displaySize.height()));

    return displaySize;
}


/**
* Stop viewfinder
*/
void CxeViewfinderControlSymbian::stop()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Running) {
        CX_DEBUG_ASSERT( mCameraDevice.camera() );
        CX_DEBUG( ( "Viewfinder is running stopping it" ) );
        mCameraDevice.camera()->StopViewFinder();
        setState( Ready );
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/**!
* Start the viewfinder
*/
CxeError::Id CxeViewfinderControlSymbian::start()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEVIEWFINDERCONTROL_START_1, "msg: e_CX_VIEWFINDER_START 1");

    TInt err = KErrNone;

    if ( state() == Running ) {
        CX_DEBUG( ( "Viewfinder already running - ignored start()" ) );
        CX_DEBUG_EXIT_FUNCTION();
        return CxeError::None;
    }

    if (!mVideoWindow) {
        TRAP(err, createViewfinderWindowL());
        if (err != KErrNone) {
            CX_DEBUG_EXIT_FUNCTION();
            return CxeErrorHandlingSymbian::map(err);
        }
    }

    if (state() == Uninitialized) {
        err = initViewfinder();
    }

    // apply any settings here
    if ( mDirectViewfinderInUse && !err && mDirectViewfinder && state() == Ready ) {

        // for now only direct vf
        switch( mDirectViewfinder->ViewFinderState() ) {
        case CCamera::CCameraDirectViewFinder::EViewFinderInActive:
            {
            CX_DEBUG(("Calling StartViewFinderDirectL"));

            // Make UI surface transparent so viewfinder is visible
            if (mUiWindow) {
                mUiWindow->SetSurfaceTransparency(ETrue);
            }

            // A local copy of the viewfinder coordinates (in WServ
            // coordinates). Needed because calling StartViewFinderDirectL()
            // will modify the rectangle and we don't want mWindowRect to be
            // modified.
            TRect activeViewfinderRect = mWindowRect;

            CX_DEBUG(("activeViewfinderRect iTl=(%d, %d) iBr=(%d, %d)",
                activeViewfinderRect.iTl.iX, activeViewfinderRect.iTl.iY,
                activeViewfinderRect.iBr.iX, activeViewfinderRect.iBr.iY));

            TRAP(err, mCameraDevice.camera()->StartViewFinderDirectL(
                CCoeEnv::Static()->WsSession(),
                *CCoeEnv::Static()->ScreenDevice(),
                *mVideoWindow, activeViewfinderRect));
            OstTrace0(camerax_performance, CXEVIEWFINDERCONTROL_STARTUP, "msg: e_CX_STARTUP 0");
            setState(Running);
            break;
            }
        case CCamera::CCameraDirectViewFinder::EViewFinderPause:
            TRAP( err, mDirectViewfinder->ResumeViewFinderDirectL() );
            if (!err) {
                setState( Running );
            }
            break;
        case CCamera::CCameraDirectViewFinder::EViewFinderActive:
            // Already running. Not considered as error.
            setState( Running );
            break;
        default:
            err = KErrNotSupported;
        }
    }
    else if ( !mDirectViewfinder ) {
        // Start bmp vf
        //TSize size(320, 240);
        TSize size(mWindowRect.Width(), mWindowRect.Height());
        CX_DEBUG(("Starting bitmap vf with size %d x %d", size.iWidth, size.iHeight));
        TRAP( err, mCameraDevice.camera()->StartViewFinderBitmapsL( size ) );
        CX_DEBUG(("Bitmap viewfinder modified to size %d x %d", size.iWidth, size.iHeight));
        if ( err ) {
            CX_DEBUG( ("StartViewfinderBitmapsL - err:%d", err) );
        }
        else {
            setState( Running );
            TRAP( err, mCameraDevice.camera()->SetViewFinderMirrorL(true) );
        }
    }
    else {
        // vf already running. nothing to do
    }

    OstTrace0(camerax_performance, CXEVIEWFINDERCONTROL_START_2, "msg: e_CX_VIEWFINDER_START 0");
    CX_DEBUG_EXIT_FUNCTION();
    return CxeErrorHandlingSymbian::map(err);
}


/**
* Intialize the viewfinder based on the VF mode
*/
int CxeViewfinderControlSymbian::initViewfinder()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEVIEWFINDERCONTROL_INIT_1, "msg: e_CX_VIEWFINDER_INIT 1");

    TInt err = KErrNone;
    if (state() != Uninitialized) {
        return err;
    }

    // For now only direct vf. If there is any need for supporting other VF modes, condition checks
    // and handling of new states are needed here.

    // release first, the prev/current view finder instance.
    releaseCurrentViewfinder();

    mDirectViewfinderInUse = true;
    /*
    if ( mCameraDeviceControl.cameraIndex() == Cxe::PrimaryCameraIndex ) {
        mDirectViewfinderInUse = true;
    }
    else {
        mDirectViewfinderInUse = false;
    }*/

    if ( mDirectViewfinderInUse ) {
        // creating an new instance of vf
        TRAP( err, mDirectViewfinder = CCamera::CCameraDirectViewFinder::NewL( *mCameraDevice.camera() ) );
    }

    if ( !err ) {
        setState( Ready );
    }
    else {
        CX_DEBUG( ("error: %d", err ) );
    }

#if defined(CXE_USE_DUMMY_CAMERA) || defined(__WINSCW__)
    //! @todo Temporary code for WINSCW, because CCamera::CCameraDirectViewFinder is not yet supported by dummy engine
    setState( Ready );
#endif

    OstTrace0(camerax_performance, CXEVIEWFINDERCONTROL_INIT_2, "msg: e_CX_VIEWFINDER_INIT 0");
    CX_DEBUG_EXIT_FUNCTION();
    return err;
}


/**
* Create the window for viewfinder to render into.
*/
void CxeViewfinderControlSymbian::createViewfinderWindowL()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEVIEWFINDERCONTROL_CREATE_WINDOW_1, "msg: e_CX_VIEWFINDER_CREATE_WINDOW 1");

    if (!mUiWindow) {
        CX_DEBUG( ( "mUiWindow not set - cannot create VF window!" ) );
        User::Leave(KErrNotReady);
    }

    // Make UI surface transparent so viewfinder is visible
    if (mUiWindow) {
        mUiWindow->SetSurfaceTransparency(ETrue);
    }

    delete mVideoContainer;
    mVideoContainer = NULL;
    mVideoContainer = new (ELeave) CxeVideoContainer();
    mVideoContainer->ConstructL();
    CX_DEBUG(("Viewfinder container created ok"));

    mVideoContainer->SetRect(mWindowRect);
    mVideoWindow = mVideoContainer->DrawableWindow();
    CX_DEBUG(("mVideoWindow is 0x%08x", mVideoWindow));
    CX_DEBUG(("mUiWindow ordinal position is: %d", mUiWindow->OrdinalPosition()));

    // Make sure UI window is on top of viewfinder
    CX_DEBUG(("Set viewfinder window ordinal.."));
    mVideoWindow->SetOrdinalPosition(-1);
    mUiWindow->SetOrdinalPosition(0);

    CX_DEBUG(("mVideoWindow ordinal position is: %d", mVideoWindow->OrdinalPosition()));
    CX_DEBUG(("mUiWindow ordinal position is: %d", mUiWindow->OrdinalPosition()));

    OstTrace0(camerax_performance, CXEVIEWFINDERCONTROL_CREATE_WINDOW_2, "msg: e_CX_VIEWFINDER_CREATE_WINDOW 0");
    CX_DEBUG_EXIT_FUNCTION();
}


void CxeViewfinderControlSymbian::prepareForRelease()
{
    CX_DEBUG_ENTER_FUNCTION();
    stop();
    CX_DEBUG_EXIT_FUNCTION();
}

/**
* Release Vf resources
*/
void CxeViewfinderControlSymbian::releaseCurrentViewfinder()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mDirectViewfinder;
    mDirectViewfinder = NULL;
    setState( Uninitialized );

    CX_DEBUG_EXIT_FUNCTION();
}

/**
* Ecam reference changing, release resources
*/
void CxeViewfinderControlSymbian::prepareForCameraDelete()
{
    CX_DEBUG_ENTER_FUNCTION();
    stop(); // first stop the viewfinder
    releaseCurrentViewfinder();
    CX_DEBUG_EXIT_FUNCTION();
}


/**
* Handle new camera instance.
*/
void CxeViewfinderControlSymbian::handleCameraAllocated(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (!error) {
        initViewfinder();
    }
    CX_DEBUG_EXIT_FUNCTION();
}

CxeViewfinderControl::State CxeViewfinderControlSymbian::state() const
{
    return static_cast<State>( stateId() );
}

void CxeViewfinderControlSymbian::handleStateChanged( int newStateId, CxeError::Id error )
{
    emit stateChanged( static_cast<State>( newStateId ), error );
}

void CxeViewfinderControlSymbian::initializeStates()
{
    // addState( id, name, allowed next states )
    addState( new CxeState( Uninitialized , "Uninitialized", Ready ) );
    addState( new CxeState( Ready , "Ready", Uninitialized | Running ) );
    addState( new CxeState( Running , "Running", Uninitialized | Ready ) );

    setInitialState( Uninitialized );
}

void CxeViewfinderControlSymbian::handleVfFrame(MCameraBuffer* /*buffer*/, int /*error*/)
{
    CX_DEBUG_IN_FUNCTION();
    CX_DEBUG( ( "Bitmap viewfinder not supported" ) );
    CX_ASSERT_ALWAYS(false);
}

// end of file
