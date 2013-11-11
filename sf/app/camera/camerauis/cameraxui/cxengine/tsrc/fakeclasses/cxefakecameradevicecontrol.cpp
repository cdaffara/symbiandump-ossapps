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
#include "cxefakecameradevicecontrol.h"
#include "cxenamespace.h"
#include "cxutils.h"

CxeFakeCameraDeviceControl::CxeFakeCameraDeviceControl()
    : CxeCameraDeviceControl(),
    mCameraIndex(Cxe::PrimaryCameraIndex),
    mCameraMode(Cxe::ImageMode),
    mState(CxeCameraDeviceControl::Idle)
{
    CX_DEBUG_IN_FUNCTION();
}

CxeFakeCameraDeviceControl::~CxeFakeCameraDeviceControl()
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
    Get current camera mode.
*/
Cxe::CameraMode CxeFakeCameraDeviceControl::mode() const
{
    return mCameraMode;
}

/*!
    Set current camera mode.
*/
void CxeFakeCameraDeviceControl::setMode(Cxe::CameraMode mode)
{
    mCallHistory.append(SetMode);
    mCameraMode = mode;
}

/*!
    Initialize
*/
void CxeFakeCameraDeviceControl::init()
{
}

/*!
    Reserve camera hardware
*/
void CxeFakeCameraDeviceControl::reserve()
{
    mCallHistory.append(Reserve);
}

/*!
    Cancel all operations and release camera for other applications to use.
    Camera module is also powered off.
*/
void CxeFakeCameraDeviceControl::release()
{
    mCallHistory.append(Release);
}

/**
 * Get current camera index (primary or secondary).
 *
 * @return Current camera index
 */
Cxe::CameraIndex CxeFakeCameraDeviceControl::cameraIndex() const {

    return mCameraIndex;
}

/**
 * Switch between primary and secondary camera.
 * @todo what happens if initMode is called during an ongoing operation?
 *
 * @param index New camera index
 */
CxeError::Id CxeFakeCameraDeviceControl::switchCamera( Cxe::CameraIndex index )
{
    mCallHistory.append(SwitchCamera);

    mCameraIndex = index;

    return CxeError::None;
}

/**
 * Get current device control state.
 *
 * @return Current state
 */
CxeCameraDeviceControl::State CxeFakeCameraDeviceControl::state() const {

    return mState;
}

void CxeFakeCameraDeviceControl::setState(
        CxeCameraDeviceControl::State newState)
{
    mState = newState;

    emit stateChanged(mState, CxeError::None);
    if (mState == CxeCameraDeviceControl::Ready) {
        emit initModeComplete(CxeError::None);
    }
}

void CxeFakeCameraDeviceControl::HandleEvent(const TECAMEvent &/*aEvent*/)
{
}

void CxeFakeCameraDeviceControl::ViewFinderReady(
        MCameraBuffer &/*aCameraBuffer*/, TInt /*aError*/)
{
}
void CxeFakeCameraDeviceControl::ImageBufferReady(
        MCameraBuffer &/*aCameraBuffer*/, TInt /*aError*/)
{
}
void CxeFakeCameraDeviceControl::VideoBufferReady(
        MCameraBuffer &/*aCameraBuffer*/, TInt /*aError*/)
{
}

QList<CxeFakeCameraDeviceControl::MethodIndex> CxeFakeCameraDeviceControl::callHistory() const
{
    return mCallHistory;
}

void CxeFakeCameraDeviceControl::resetCallHistory()
{
    mCallHistory.clear();
}

// end of file
