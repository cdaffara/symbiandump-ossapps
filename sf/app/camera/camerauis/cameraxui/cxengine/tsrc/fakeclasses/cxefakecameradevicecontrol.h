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
#ifndef CXEFAKECAMERADEVICECONTROL_H
#define CXEFAKECAMERADEVICECONTROL_H

//  Include Files

#include <QObject>

#include "cxecameradevicecontrol.h"
#include "cxenamespace.h"
#include "cxeerror.h"

#include "ecam.h"

class CxeFakeCameraDeviceControl : public CxeCameraDeviceControl,
                                   public MCameraObserver2
{
    Q_OBJECT

public:

    CxeFakeCameraDeviceControl();
    virtual ~CxeFakeCameraDeviceControl();

public: // from CxeCameraDeviceControl
    Cxe::CameraMode mode() const;
    void setMode(Cxe::CameraMode mode);
    void init();
    void reserve();
    void release();
    Cxe::CameraIndex cameraIndex() const;
    CxeError::Id switchCamera(Cxe::CameraIndex index);
    CxeCameraDeviceControl::State state() const;

public: // methods for unit testing
    enum MethodIndex
    {
        SetMode,
        Reserve,
        Release,
        SwitchCamera
    };

    void setState(CxeCameraDeviceControl::State newState);
    QList<MethodIndex> callHistory() const;
    void resetCallHistory();

protected: // from MCameraObserver2
    void HandleEvent(const TECAMEvent &aEvent);
    void ViewFinderReady(MCameraBuffer &aCameraBuffer, TInt aError);
    void ImageBufferReady(MCameraBuffer &aCameraBuffer, TInt aError);
    void VideoBufferReady(MCameraBuffer &aCameraBuffer, TInt aError);

private:
    Cxe::CameraIndex mCameraIndex;
    Cxe::CameraMode mCameraMode;
    QList<MethodIndex> mCallHistory;
    CxeCameraDeviceControl::State mState;
};

#endif  // CXEFAKECAMERADEVICECONTROL_H

