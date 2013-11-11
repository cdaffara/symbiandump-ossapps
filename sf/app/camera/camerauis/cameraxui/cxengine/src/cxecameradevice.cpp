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
#include <ECamOrientationCustomInterface2.h>
#include <ecamfacetrackingcustomapi.h>
#include <ecamusecasehintcustomapi.h>

#include "cxecameradevice.h"
#include "cxeerrormappingsymbian.h"
#include "cxutils.h"
#include "cxedummycamera.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxecameradeviceTraces.h"
#endif

const int KCameraPriority = -1;


CxeCameraDevice::CxeCameraDevice() :
    mCamera(NULL),
    mAdvancedSettings(NULL),
    mImageProcessor(NULL),
    mCameraSnapshot(NULL),
    mCameraOrientation(NULL),
    mFaceTracking(NULL)
{

}

CxeCameraDevice::~CxeCameraDevice()
{
    CX_DEBUG_ENTER_FUNCTION();
    // Exiting, release all resources
    releaseResources();
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDevice::deleteCamera()
{
    CX_DEBUG_ENTER_FUNCTION();

    emit prepareForCameraDelete();

    // preparing for new camera, release old resources
    releaseResources();

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDevice::releaseCamera()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mCamera) {
        emit prepareForRelease();
        mCamera->Release();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDevice::reserveCamera()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mCamera) {
        emit aboutToReserve();
        mCamera->Reserve();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDevice::setCamera( CCamera *camera )
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mCamera != camera) {

        // new camera, delete old resources
        releaseResources();

        mCamera = camera;

        // initialize resources
        CxeError::Id err = initResources();

        emit cameraAllocated(err);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

CCamera* CxeCameraDevice::camera()
{
    return mCamera;
}

CCamera::CCameraAdvancedSettings* CxeCameraDevice::advancedSettings()
{
    return mAdvancedSettings;
}

CCamera::CCameraImageProcessing* CxeCameraDevice::imageProcessor()
{
    return mImageProcessor;
}


CCamera::CCameraSnapshot* CxeCameraDevice::cameraSnapshot()
{
    return mCameraSnapshot;
}


MCameraOrientation* CxeCameraDevice::cameraOrientation()
{
    return mCameraOrientation;
}

MCameraFaceTracking* CxeCameraDevice::faceTracking()
{
    return mFaceTracking;
}

MCameraUseCaseHint *CxeCameraDevice::useCaseHintApi()
{
    return mUseCaseHintApi;
}

CxeError::Id CxeCameraDevice::initResources()
{
    CX_DEBUG_ENTER_FUNCTION();

    TInt status = KErrNone;

    if (mCamera) {
        OstTrace0(camerax_performance, CXECAMERADEVICE_EXTENSIONS_1, "msg: e_CX_GET_CCAMERA_EXTENSIONS 1");

        CX_DEBUG(("Get CCamera extensions.."));

        mCameraOrientation = static_cast<MCameraOrientation*>(
            mCamera->CustomInterface(KCameraOrientationUid));
        CX_DEBUG(("MCameraOrientation interface 0x%08x", mCameraOrientation));

        mFaceTracking = static_cast<MCameraFaceTracking*>(
            mCamera->CustomInterface(KCameraFaceTrackingUid));
        CX_DEBUG(("MCameraFaceTracking interface 0x%08x", mFaceTracking));

        mUseCaseHintApi = static_cast<MCameraUseCaseHint*>(
            mCamera->CustomInterface(KCameraUseCaseHintUid));
        CX_DEBUG(("MCameraUseCaseHint interface 0x%08x", mUseCaseHintApi));

        TRAPD(errorAdvSet, mAdvancedSettings =
              CCamera::CCameraAdvancedSettings::NewL(*mCamera));
        CX_DEBUG(("CCameraAdvancedSettings status: %d", errorAdvSet));

        TRAPD(errorImgPr, mImageProcessor =
              CCamera::CCameraImageProcessing::NewL(*mCamera));
        CX_DEBUG(("CCameraImageProcessing status: %d", errorImgPr));
        Q_UNUSED(errorImgPr); // Avoid release build unused variable warning.

        TRAPD(errorSnap, mCameraSnapshot =
              CCamera::CCameraSnapshot::NewL(*mCamera));
        CX_DEBUG(("CCameraSnapshot status: %d", errorSnap));

        // Check all statuses and set first error code encountered.
        // Imageprocessing is not supported by 2nd camera so we just ignore the error.
        status = errorAdvSet != KErrNone
                 ? errorAdvSet : errorSnap;
        CX_DEBUG(("Total status: %d", status));

        OstTrace0(camerax_performance, CXECAMERADEVICE_EXTENSIONS_2, "msg: e_CX_GET_CCAMERA_EXTENSIONS 0");
    }

    CX_DEBUG_EXIT_FUNCTION();
    return CxeErrorHandlingSymbian::map(status);
}

void CxeCameraDevice::releaseResources()
{
    CX_DEBUG_ENTER_FUNCTION();

    // not owned.
    mCameraOrientation = NULL;
    mFaceTracking = NULL;
    mUseCaseHintApi = NULL;

    delete mCameraSnapshot;
    mCameraSnapshot = NULL;

    delete mAdvancedSettings;
    mAdvancedSettings = NULL;

    delete mImageProcessor;
    mImageProcessor = NULL;

    delete mCamera;
    mCamera = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}

CxeError::Id CxeCameraDevice::newCamera( Cxe::CameraIndex cameraIndex, MCameraObserver2 *observer )
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXECAMERADEVICE_NEWCAMERA_IN, "msg: e_CX_CREATE_CCAMERA 1");

    CX_DEBUG(("Cxe: using camera index %d", cameraIndex));

    CX_DEBUG_ASSERT(cameraIndex == Cxe::PrimaryCameraIndex || cameraIndex == Cxe::SecondaryCameraIndex);

    CCamera* camera = NULL;

#if defined(CXE_USE_DUMMY_CAMERA) || defined(__WINSCW__)
    TRAPD(err, camera = CxeDummyCamera::NewL(*observer, cameraIndex,
                                             KCameraPriority, 2));
    CX_DEBUG(("CxeCameraDevice::newCamera <> new CxeDummyCamera"));
#else
    TRAPD(err, camera = CCamera::New2L(*observer,
                                       cameraIndex, KCameraPriority));
    CX_DEBUG(("CxeCameraDevice::newCamera <> new CCamera"));
#endif

    OstTrace0(camerax_performance, CXECAMERADEVICE_NEWCAMERA_OUT, "msg: e_CX_CREATE_CCAMERA 0");

    if (!err) {
        setCamera(camera);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return CxeErrorHandlingSymbian::map(err);
}

// end of file
