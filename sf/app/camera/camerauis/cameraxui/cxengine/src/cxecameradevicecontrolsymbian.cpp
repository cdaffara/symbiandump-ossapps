/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "cxecameradevicecontrolsymbian.h"
#include "cxesettingsimp.h"
#include "cxefeaturemanagerimp.h"
#include "cxutils.h"
#include "cxenamespace.h"
#include "cxeerrormappingsymbian.h"
#include "cxestate.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxecameradevicecontrolsymbianTraces.h"
#endif



CxeCameraDeviceControlSymbian::CxeCameraDeviceControlSymbian()
    : CxeStateMachine("CxeCameraDeviceControlSymbian"),
    mCameraDevice(NULL),
    mCameraIndex(Cxe::PrimaryCameraIndex),
    mCameraMode(Cxe::ImageMode)
{
    CX_DEBUG_ENTER_FUNCTION();

#ifdef FORCE_SECONDARY_CAMERA
    mCameraIndex = Cxe::SecondaryCameraIndex;
#endif
    qRegisterMetaType<CxeCameraDeviceControl::State>();
    initializeStates();

    mCameraDevice = new CxeCameraDevice();

    //!@todo Temporary delay before reserving camera to avoid timing issues
    connect(&mReserveTimer, SIGNAL(timeout()), this, SLOT(doReserve()));
    mReserveTimer.setSingleShot(true);

    CX_DEBUG_EXIT_FUNCTION();
}

CxeCameraDeviceControlSymbian::~CxeCameraDeviceControlSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    // release current camera
    releaseCurrentCamera();

    delete mCameraDevice;

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDeviceControlSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeError::Id err = mCameraDevice->newCamera(mCameraIndex, this);

    if (err) {
        emit initModeComplete(err);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDeviceControlSymbian::reserve()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice);

    if (state() == Idle) {
        setState(Initializing);
        //!@todo Temporary delay before reserving camera to avoid timing issues
        mReserveTimer.start(2000);
        OstTrace0(camerax_performance, CXECAMERADEVICECONTROLSYMBIAN_RESERVE, "msg: e_CX_RESERVE 1");
        CX_DEBUG(("HACK: Waiting for 2000ms before really doing reserve()"));
    } else if (state() == PendingRelease) {
        // if we get a reserve request and if there is a pending release
        // it is just fine to continue the init operation.
        setState(Initializing);
    } else {
        // nothing to do
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDeviceControlSymbian::doReserve()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice);
    mCameraDevice->reserveCamera();
    CX_DEBUG_EXIT_FUNCTION();
}


void CxeCameraDeviceControlSymbian::powerOn()
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG_ASSERT(mCameraDevice && mCameraDevice->camera());
    if (state() == Initializing) {
        mCameraDevice->camera()->PowerOn();
        OstTrace0(camerax_performance, CXECAMERADEVICECONTROLSYMBIAN_POWERON, "msg: e_CX_POWER_ON 1");
    }
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDeviceControlSymbian::HandleEvent(const TECAMEvent &aEvent)
{
    doHandleEvent( aEvent );
    emit cameraEvent( aEvent.iEventType.iUid, aEvent.iErrorCode );
}

void CxeCameraDeviceControlSymbian::doHandleEvent(const TECAMEvent &aEvent)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("Cxe: event 0x%08x, %d", aEvent.iEventType.iUid, aEvent.iErrorCode));

    if(aEvent.iEventType == KUidECamEventReserveComplete) {
        handleReserveComplete(aEvent.iErrorCode);
    }
    else if(aEvent.iEventType == KUidECamEventPowerOnComplete) {
        handlePowerOnComplete(aEvent.iErrorCode);
    }
    else if(aEvent.iEventType == KUidECamEventCameraNoLongerReserved)
        {
        handleNoLongerReserved(aEvent.iErrorCode);
        }
    else
        {
        // We don't care about this event -- ignore
        }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDeviceControlSymbian::handleReserveComplete(int error)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXECAMERADEVICECONTROLSYMBIAN_HANDLERESERVECOMPLETE, "msg: e_CX_RESERVE 0" );

    if(state() == PendingRelease) {
        release();
    } else {
        if (!error) {
            powerOn();
        } else {
            emit initModeComplete(CxeErrorHandlingSymbian::map(error));
            setState(Idle);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDeviceControlSymbian::handlePowerOnComplete(int error)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXECAMERADEVICECONTROLSYMBIAN_HANDLEPOWERONCOMPLETE, "msg: e_CX_POWER_ON 0");

    if(state() == PendingRelease) {
        release();
    } else {
        if (!error) {
            setState(Ready);
            emit deviceReady(); // this will trigger prepare for either still or video mode
        } else {
            setState(PendingRelease);
            release();
            emit initModeComplete(CxeErrorHandlingSymbian::map(error));
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDeviceControlSymbian::handleNoLongerReserved(int /* error */)
{
    setState( Idle );
    //! @todo: handle cases where camera is lost
}


/*
* switch between camera <-> primary <-> secondary
*/

CxeError::Id CxeCameraDeviceControlSymbian::switchCamera(Cxe::CameraIndex cameraIndex)
{
    CX_DEBUG_ENTER_FUNCTION();

#ifdef FORCE_SECONDARY_CAMERA
    // 2nd camera forced use, make us believe camera never needs to be swithed.
    CX_DEBUG(("CxeCameraDeviceControlSymbian::switchCamera() second camera forced, skip switch"));
    cameraIndex = mCameraIndex;
#endif

    // during mode change, we have to re-create camera reference and release resources.
    // in all other cases if the camera indexes are same, it means that camera reference
    // is already created and hence we can return immediately.
    if (cameraIndex == mCameraIndex) {
        CX_DEBUG_EXIT_FUNCTION();
        return CxeError::None;
    }

    releaseCurrentCamera();

    CxeError::Id err = mCameraDevice->newCamera(cameraIndex, this);

    if (err == CxeError::None){
        // CCamera object created, so make sure that appropriate index is used
        mCameraIndex = cameraIndex;
    }

    if (!err) { // if there is no error creating new camera, then start Reserve
        reserve();
    }

    CX_DEBUG_EXIT_FUNCTION();
    return err;
}

void CxeCameraDeviceControlSymbian::release()
{
    CX_DEBUG_ENTER_FUNCTION();

    //!@todo Temporary delay before reserving camera to avoid timing issues
    if (mReserveTimer.isActive()) {
        // release() was called when the timer was active
        mReserveTimer.stop();
        mCameraDevice->releaseCamera();
        setState(Idle);
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    if (state() == Idle) {
        // nothing to do
        return;
    }

    if (state() == Initializing) {
        setState(PendingRelease);
    } else {
        mCameraDevice->releaseCamera();
        setState(Idle);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*
* Release resources w.r.t current camera
*/
void CxeCameraDeviceControlSymbian::releaseCurrentCamera()
{
    CX_DEBUG_ENTER_FUNCTION();

    release();
    // delete old camera instance, this will inturn generates "prepareForCameraDelete" event
    // across all interested classes which wants to do cleanup for new camera reference change
    mCameraDevice->deleteCamera();

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDeviceControlSymbian::ViewFinderReady(MCameraBuffer &aCameraBuffer, TInt aError)
{
    CX_DEBUG_ENTER_FUNCTION();
    emit vfFrameReady(&aCameraBuffer, aError);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDeviceControlSymbian::ImageBufferReady(MCameraBuffer& aCameraBuffer, TInt aError)
{
    CX_DEBUG_ENTER_FUNCTION();

    emit imageBufferReady( &aCameraBuffer, aError );

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDeviceControlSymbian::VideoBufferReady(MCameraBuffer &/*aCameraBuffer*/, TInt /*aError*/)
{
    CX_DEBUG_IN_FUNCTION();
}

Cxe::CameraIndex CxeCameraDeviceControlSymbian::cameraIndex() const
{
    return mCameraIndex;
}

Cxe::CameraMode CxeCameraDeviceControlSymbian::mode() const
{
    return mCameraMode;
}

void CxeCameraDeviceControlSymbian::setMode(Cxe::CameraMode mode)
{
    mCameraMode = mode;
}

CxeCameraDeviceControl::State CxeCameraDeviceControlSymbian::state() const
{
    return static_cast<State>(stateId());
}

void CxeCameraDeviceControlSymbian::handleStateChanged(int newStateId, CxeError::Id error)
{
    emit stateChanged(static_cast<State>(newStateId), error);
}

void CxeCameraDeviceControlSymbian::initializeStates()
{
    // addState( id, name, allowed next states )
    addState(new CxeState( Idle , "Idle", Initializing));
    addState(new CxeState( Initializing, "Initializing", PendingRelease | Ready | Idle));
    addState(new CxeState( Ready, "Ready", Idle));
    addState(new CxeState( PendingRelease, "PendingRelease", Initializing | Idle));

    setInitialState(Idle);
}

CxeCameraDevice *CxeCameraDeviceControlSymbian::cameraDevice()
{
    return mCameraDevice;
}

// end of file
