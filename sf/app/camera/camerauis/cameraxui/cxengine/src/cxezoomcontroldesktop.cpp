/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cxezoomcontroldesktop.h"
#include "cxecameradevicecontrol.h"
#include "cxutils.h"
#include "cxenamespace.h"
#include "cxestate.h"

const int NOT_DEFINED = -1;

/*!
 * Constructor
 */
CxeZoomControlDesktop::CxeZoomControlDesktop(CxeCameraDeviceControl &cameraDeviceControl) :
    mCameraDeviceControl(cameraDeviceControl)
{
    CX_DEBUG_ENTER_FUNCTION();

    qRegisterMetaType<CxeZoomControl::State> ();

    init();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Destructor
 */
CxeZoomControlDesktop::~CxeZoomControlDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Get the minimum zoom index
 */
int CxeZoomControlDesktop::min() const
{
    return mMinZoomLevel;
}

/*!
 * Get the maximum zoom index
 */
int CxeZoomControlDesktop::max() const
{
    return mMaxZoomLevel;
}

/*!
 * Init
 */
void CxeZoomControlDesktop::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Initialize the variables
    mMinZoomLevel = 0;
    mMaxZoomLevel = 0;
    mCurrentZoomLevel = 0;

    mState = Idle;

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Re-initializes the zoom control
 */
void CxeZoomControlDesktop::reset()
{
    CX_DEBUG_ENTER_FUNCTION();

    init();
    emit zoomLevelChanged(mCurrentZoomLevel);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* slot to prepare zoom control for still mode
*/
void CxeZoomControlDesktop::prepareZoomForStill(int ecamStillResolutionIndex)
{
    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(ecamStillResolutionIndex);

    if (mCameraDeviceControl.state() == CxeCameraDeviceControl::Ready) {
        reset();
        int error;
        int imageQualityIndex;
        CxeError::Id cxErr;
        if (cxErr == CxeError::None && mStillMaxZoomLimits.count() > 0) {
            // fetching the zoom limit based on the image quality
            mMaxZoomLevel = mStillMaxZoomLimits[imageQualityIndex];
        } else {
            mMaxZoomLevel = NOT_DEFINED;
        }
        
        finalizeZoomPreparation(error);
    }
    
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* slot to prepare zoom control for video mode
*/
void CxeZoomControlDesktop::prepareZoomForVideo()
{
    CX_DEBUG_ENTER_FUNCTION();
    
    if (mCameraDeviceControl.state() == CxeCameraDeviceControl::Ready) {
        reset();
        int error;
        if (mVideoMaxZoomLimits.count() > 0) {
            // fetching the zoom limit based on the video quality
            mMaxZoomLevel = mVideoMaxZoomLimits[0];
        } else {
            mMaxZoomLevel = NOT_DEFINED;
        }

        finalizeZoomPreparation(error);
    }
    

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Completes zoom control preparation.
*/
void CxeZoomControlDesktop::finalizeZoomPreparation(int error)
{
    CX_DEBUG_ENTER_FUNCTION();
    
    CX_DEBUG(("Maximum zoom limit: %d", mMaxZoomLevel));
    
    if (!error) {
        // Note that this is called only after setting the resolution.
        // emit the zoom changed signal so that clients can do any updates if necessary.
        emit zoomLevelChanged(mCurrentZoomLevel);

        mState = Ready;
        emit stateChanged(mState, CxeError::None);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Zooms to the level specified in the parameter
*/
void CxeZoomControlDesktop::zoomTo(int value)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("Zoom to: %d, current zoom: %d", value, mCurrentZoomLevel));

    // If not currently zooming and value falls between min & max
    // then zoom to the passed level. Also there's no need to zoom if new zoom
    // level is same as the current.
    if (state() == CxeZoomControl::Ready && (value <= mMaxZoomLevel)
            && (value >= mMinZoomLevel) && (mCurrentZoomLevel != value)) {

        // Could also check the number of values in the array to avoid out of bounds indexing,
        // though this is done in alternate way by the min and max limits.
        mCurrentZoomLevel = value;

        // Set the appropriate zooming state
        if (value < mCurrentZoomLevel) {
             mState = ZoomingOut;
        } else {
            mState = ZoomingIn;
        }
        emit stateChanged(mState, CxeError::None);

        //! @todo: Wait for zoom callback: ECamEventZoomStateChanged ?
        mState = Ready;
        emit stateChanged(mState, CxeError::None);

        // Emit the zoom level change signal
        emit zoomLevelChanged(mCurrentZoomLevel);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

CxeZoomControl::State CxeZoomControlDesktop::state() const
{
    return mState;
}
