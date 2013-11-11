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
#ifndef CXECAMERADEVICECONTROL_H
#define CXECAMERADEVICECONTROL_H

#include <QObject>
#include "cxenamespace.h"
#include "cxeerror.h"

class CxeViewfinderControl;

/**
 * CxeCameraDeviceControl is responsible for reserving and powering on the camera module.
 * It also routes all camera adaptation events to other engine internal controls.
 * The device control API is also used to control the operating mode (Cxe::CameraMode) of
 * the engine
 */
class CxeCameraDeviceControl : public QObject
{
    Q_OBJECT
public:

    /**
     * Camera device control state.
     */
    enum State
        {
        //! Camera power off/unreserved
        Idle            = 0x01,

        //! Reserving and powering on camera
        Initializing    = 0x02,
        
        //! Camera device is reserved and powered on
        Ready           = 0x04,

        //! Camera device release pending
        PendingRelease  = 0x08
        };

    /**
     * Get current camera mode.
     */
    virtual Cxe::CameraMode mode() const = 0;

    /**
     * Set current camera mode.
     */
    virtual void setMode(Cxe::CameraMode mode) = 0;

    virtual void init() = 0;

    /**
     * Reserve camera device for exclusive use.
     */
    virtual void reserve() = 0;

    /**
     * Cancel all operations and release camera for other applications to use.
     * Camera module is also powered off.
     */
    virtual void release() = 0;

    /**
     * Get current camera index (primary or secondary).
     *
     * @return Current camera index
     */
    virtual Cxe::CameraIndex cameraIndex() const = 0;

    /**
     * Switch between primary and secondary camera.
     *
     * @param index New camera index
     */
    virtual CxeError::Id switchCamera(Cxe::CameraIndex index) = 0;

    /**
     * Get current device control state.
     *
     * @return Current state
     */
    virtual State state() const = 0;

signals:
    /**
     * Initializing current mode has completed. The engine is now ready to capture
     * images or videos (unless a non-zero error code was given.
     *
     * @param error Error code
     */
    void initModeComplete(CxeError::Id error);

    /**
     * Camera device control state has changed.
     *
     * @param newState New device control state
     * @param error    Error code
     */
    void stateChanged(CxeCameraDeviceControl::State newState, CxeError::Id error);

protected:
    CxeCameraDeviceControl() {}

private:
    Q_DISABLE_COPY(CxeCameraDeviceControl)
};

Q_DECLARE_METATYPE(CxeCameraDeviceControl::State)

#endif // CXECAMERADEVICECONTROL_H
