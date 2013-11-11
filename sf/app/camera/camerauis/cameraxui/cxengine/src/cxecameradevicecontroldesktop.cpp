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
#include "cxecameradevicecontroldesktop.h"
#include "cxeviewfindercontroldesktop.h"
#include "cxutils.h"

CxeCameraDeviceControlDesktop::CxeCameraDeviceControlDesktop() : mIndex(Cxe::PrimaryCameraIndex), mMode(Cxe::ImageMode), mState(Idle)
{
    CX_DEBUG_IN_FUNCTION()
}

CxeCameraDeviceControlDesktop::~CxeCameraDeviceControlDesktop()
{
    CX_DEBUG_IN_FUNCTION()
}

/**
 * Get current camera mode.
 */
Cxe::CameraMode CxeCameraDeviceControlDesktop::mode() const
{
   return mMode;
}
/**
 * Set current camera mode.
 */
void CxeCameraDeviceControlDesktop::setMode(Cxe::CameraMode mode)
{
    CX_DEBUG_ENTER_FUNCTION();
    mMode = mode;
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeCameraDeviceControlDesktop::init()
{
    CX_DEBUG_ENTER_FUNCTION();
    setState(CxeCameraDeviceControl::Ready);
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Reserve camera device for exclusive use.
 */
void CxeCameraDeviceControlDesktop::reserve()
{
    CX_DEBUG_IN_FUNCTION()
}

/**
 * Cancel all operations and release camera for other applications to use.
 * Camera module is also powered off.
 */
void CxeCameraDeviceControlDesktop::release()
{
    CX_DEBUG_ENTER_FUNCTION();
    setState(CxeCameraDeviceControl::Idle);
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Get current camera index (primary or secondary).
 *
 * @return Current camera index
 */
Cxe::CameraIndex CxeCameraDeviceControlDesktop::cameraIndex() const
{
    return mIndex;
}

/**
 * Switch between primary and secondary camera.
 *
 * @param index New camera index
 */
CxeError::Id CxeCameraDeviceControlDesktop::switchCamera(Cxe::CameraIndex index)
{
    CX_DEBUG_ENTER_FUNCTION();
    mIndex = index;
    setState(CxeCameraDeviceControl::Ready);
    return CxeError::None;
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Get current device control state.
 *
 * @return Current state
 */
CxeCameraDeviceControl::State CxeCameraDeviceControlDesktop::state() const
{
    return mState;
}

void CxeCameraDeviceControlDesktop::setState(CxeCameraDeviceControl::State stateId, CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    mState = stateId;
    emit stateChanged(mState, error);
    CX_DEBUG_EXIT_FUNCTION();
}
// end of file
