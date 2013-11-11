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
#ifndef CXECAMERADEVICECONTROLDESKTOP_H
#define CXECAMERADEVICECONTROLDESKTOP_H

#include <QObject>
#include <QMetaType>
#include "cxecameradevicecontrol.h"
#include "cxenamespace.h"
#include "cxeerror.h"

class CxeCameraDeviceControlDesktop : public CxeCameraDeviceControl
{
    Q_OBJECT

public:

    CxeCameraDeviceControlDesktop();
    ~CxeCameraDeviceControlDesktop();

public: // from CxeCameraDeviceControl
    virtual Cxe::CameraMode mode() const;
    virtual void setMode(Cxe::CameraMode mode);
    virtual void init();
    virtual void reserve();
    virtual void release();
    virtual Cxe::CameraIndex cameraIndex() const;
    virtual CxeError::Id switchCamera(Cxe::CameraIndex index);
    virtual CxeCameraDeviceControl::State state() const;

private:
    void setState(CxeCameraDeviceControl::State stateId, CxeError::Id error = CxeError::None);

private:
    Cxe::CameraIndex mIndex;
    Cxe::CameraMode mMode;
    State mState;

};

#endif // CXECAMERADEVICECONTROLDESKTOP_H
