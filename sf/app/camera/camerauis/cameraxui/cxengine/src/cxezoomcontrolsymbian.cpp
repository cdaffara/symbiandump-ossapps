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

#include "cxezoomcontrolsymbian.h"
#include "cxecameradevicecontrol.h"
#include "cxefeaturemanager.h"
#include "cxesettings.h"
#include "cxutils.h"
#include "cxenamespace.h"
#include "cxestate.h"
#include "cxeexception.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxezoomcontrolsymbianTraces.h"
#endif

namespace
{
    const int NOT_DEFINED = -1;
}

// ---------------------------------------------------------------------------
// CxeZoomControlSymbian::CxeZoomControlSymbian
//
// ---------------------------------------------------------------------------
//
CxeZoomControlSymbian::CxeZoomControlSymbian(CxeCameraDevice &cameraDevice,
                                             CxeCameraDeviceControl &cameraDeviceControl,
                                             CxeSettings &settings,
                                             CxeFeatureManager &featureManager) :
    CxeStateMachine("CxeZoomControlSymbian"),
    mCameraDevice(cameraDevice),
    mCameraDeviceControl(cameraDeviceControl),
    mSettings(settings)
{
    CX_DEBUG_ENTER_FUNCTION();

    qRegisterMetaType<CxeZoomControl::State> ();

    // Initialize states and member variables
    initializeStates();

    // getting still max zoom limits from feature manager
    featureManager.configuredValues(CxeVariationKeys::STILL_MAX_ZOOM_LIMITS, mStillMaxZoomLimits);
    featureManager.configuredValues(CxeVariationKeys::VIDEO_MAX_ZOOM_LIMITS, mVideoMaxZoomLimits);

    connect(&mCameraDevice,
            SIGNAL(prepareForCameraDelete()),
            this,
            SLOT(reset()));

    init();

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxeZoomControlSymbian::~CxeZoomControlSymbian
//
// ---------------------------------------------------------------------------
//
CxeZoomControlSymbian::~CxeZoomControlSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    // clean up
    mDigitalZoomValues.Close();

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxeZoomControlSymbian::min
// Get the minimum zoom index
// ---------------------------------------------------------------------------
//
int CxeZoomControlSymbian::min() const
{
    return mMinZoomLevel;
}

// ---------------------------------------------------------------------------
// CxeZoomControlSymbian::max
// Get the maximum zoom index
// ---------------------------------------------------------------------------
//
int CxeZoomControlSymbian::max() const
{
    return mMaxZoomLevel;
}

void CxeZoomControlSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Initialize the variables
    mMinZoomLevel = 0;
    mMaxZoomLevel = 0;
    mCurrentZoomLevel = 0;
    mDigitalZoomValues.Close();

    setState(Idle);

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxeZoomControlSymbian::reset
// Re-initializes the zoom control
// ---------------------------------------------------------------------------
//
void CxeZoomControlSymbian::reset()
{
    CX_DEBUG_ENTER_FUNCTION();

    init();
    emit zoomLevelChanged(mCurrentZoomLevel);

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* slot to prepare zoom control for still mode
*/
void CxeZoomControlSymbian::prepareZoomForStill(int ecamStillResolutionIndex)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEZOOMCONTROLSYMBIAN_PREPARESTILL_IN, "msg: e_CX_PREPARE_ZOOM 1");

    if (mCameraDeviceControl.state() == CxeCameraDeviceControl::Ready) {
        reset();
        // For image mode
        CCamera::TFormat format = CCamera::EFormatExif;
        TBool isInfluencePossible = EFalse;
        int error = KErrNone;

        // Get the zoom steps for image mode
        TRAP(error, mCameraDevice.advancedSettings()->GetDigitalZoomStepsForStillL (
                            mDigitalZoomValues,
                            mDigitalZoomValueInfo,
                            ecamStillResolutionIndex,
                            format,
                            isInfluencePossible));


        try {
            int imageQualityIndex = mSettings.get<int>(CxeSettingIds::IMAGE_QUALITY);
            if (mStillMaxZoomLimits.count() > 0 && imageQualityIndex < mStillMaxZoomLimits.count()) {
                // fetching the zoom limit based on the image quality
                mMaxZoomLevel = mStillMaxZoomLimits[imageQualityIndex];
            }
        } catch (CxeException &e) {
            mMaxZoomLevel = NOT_DEFINED;
        }

        finalizeZoomPreparation(error);
    }

    OstTrace0(camerax_performance, CXEZOOMCONTROLSYMBIAN_PREPARESTILL_OUT, "msg: e_CX_PREPARE_ZOOM 0");
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* slot to prepare zoom control for video mode
*/
void CxeZoomControlSymbian::prepareZoomForVideo()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEZOOMCONTROLSYMBIAN_PREPAREVIDEO_IN, "msg: e_CX_PREPARE_ZOOM 1");

    if (mCameraDeviceControl.state() == CxeCameraDeviceControl::Ready) {
        reset();
        // if CxeCameraDeviceControl is ready, this pointer is valid
        CX_ASSERT_ALWAYS(mCameraDevice.advancedSettings());
        int error = KErrNone;
        TRAP(error,
                mCameraDevice.advancedSettings()->GetDigitalZoomStepsL(mDigitalZoomValues, mDigitalZoomValueInfo);
            );

        if (mVideoMaxZoomLimits.count() > 0) {
            // fetching the zoom limit based on the video quality
            mMaxZoomLevel = mVideoMaxZoomLimits[0];
        } else {
            mMaxZoomLevel = NOT_DEFINED;
        }

        finalizeZoomPreparation(error);
    }

    OstTrace0(camerax_performance, CXEZOOMCONTROLSYMBIAN_PREPAREVIDEO_OUT, "msg: e_CX_PREPARE_ZOOM 0");
    CX_DEBUG_EXIT_FUNCTION();
}



/**
* Completes zoom control preparation.
*/
void CxeZoomControlSymbian::finalizeZoomPreparation(int error)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("Num digital zoom steps: %d", mDigitalZoomValues.Count()));
    CX_DEBUG(("Maximum zoom limit: %d", mMaxZoomLevel));

    if (!error) {
        // Note that this is called only after setting the resolution.
        TCameraInfo cameraInfo;
        mCameraDevice.camera()->CameraInfo(cameraInfo);
        CX_DEBUG(("cameraInfo.iMinZoom: %d, cameraInfo.iMaxDigitalZoom: %d, cameraInfo.iMaxZoom: %d",
                            cameraInfo.iMinZoom, cameraInfo.iMaxDigitalZoom, cameraInfo.iMaxZoom ));

        CX_DEBUG(("cameraInfo.iMaxDigitalZoomFactor: %d", cameraInfo.iMaxDigitalZoomFactor));

        mMinZoomLevel = cameraInfo.iMinZoom;
        if ( (mMaxZoomLevel == NOT_DEFINED) || mMaxZoomLevel > cameraInfo.iMaxDigitalZoom) {
            CX_DEBUG(("Fetching maximum zoom limit value from ecam"));
            mMaxZoomLevel = cameraInfo.iMaxDigitalZoom;
        }
        // change the state, since zoom is now ready
        setState(Ready);

        mCameraDevice.advancedSettings()->SetDigitalZoom(mDigitalZoomValues[mCurrentZoomLevel]);

        // emit the zoom changed signal so that clients can do any updates if necessary.
        emit zoomLevelChanged(mCurrentZoomLevel);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxeZoomControlSymbian::zoomTo
// Zooms to the level specified in the parameter.
// ---------------------------------------------------------------------------
//
void CxeZoomControlSymbian::zoomTo(int value)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("Zoom to: %d, current zoom: %d", value, mCurrentZoomLevel));

    // If not currently zooming and value falls between min & max
    // then zoom to the passed level. Also there's no need to zoom if new zoom
    // level is same as the current.
    if (state() == CxeZoomControl::Ready && (value <= mMaxZoomLevel)
            && (value >= mMinZoomLevel) && (mCurrentZoomLevel != value)) {

        // Set the appropriate zooming state
        if (value < mCurrentZoomLevel) {
            setState(CxeZoomControl::ZoomingOut);
        } else {
            setState(CxeZoomControl::ZoomingIn);
        }

        // Could also check the number of values in the array to avoid out of bounds indexing,
        // though this is done in alternate way by the min and max limits.
        mCameraDevice.advancedSettings()->SetDigitalZoom(mDigitalZoomValues[value]);
        mCurrentZoomLevel = value;

        //! @todo: Wait for zoom callback: ECamEventZoomStateChanged ?
        // set the state to idle.
        setState(CxeZoomControl::Ready);

        // Emit the zoom level change signal
        emit zoomLevelChanged(mCurrentZoomLevel);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

CxeZoomControl::State CxeZoomControlSymbian::state() const
{
    return static_cast<State> (stateId());
}

void CxeZoomControlSymbian::handleStateChanged(int newStateId,
                                               CxeError::Id error)
{
    emit stateChanged(static_cast<State> (newStateId), error);
}

void CxeZoomControlSymbian::initializeStates()
{
    // addState( id, name, allowed next states )
    addState(new CxeState(Idle, "Idle", Ready));
    addState(new CxeState(ZoomingIn, "ZoomingIn", Ready | ZoomingOut | Idle));
    addState(new CxeState(ZoomingOut, "ZoomingOut", Ready | ZoomingIn | Idle));
    addState(new CxeState(Ready, "Ready", Idle | ZoomingOut | ZoomingIn));

    setInitialState(Idle);
}
