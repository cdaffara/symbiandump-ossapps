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
#ifndef CXECAMERADEVICE_H
#define CXECAMERADEVICE_H

//  Include Files

#include <QObject>
#include <ecam.h> // CCamera
#include <ecam/camerasnapshot.h>
#include <ecamadvsettings.h> // CCamera::CCameraAdvancedSettings
#include "cxeerror.h"
#include "cxenamespace.h"

class MCameraOrientation;
class MCameraFaceTracking;
class MCameraUseCaseHint;

//  Class Definitions
class CxeCameraDevice : public QObject
{
    Q_OBJECT

    public:
        CxeCameraDevice();
        virtual ~CxeCameraDevice();

        CCamera* camera();
        CCamera::CCameraAdvancedSettings *advancedSettings();
        CCamera::CCameraImageProcessing *imageProcessor();
        MCameraOrientation *cameraOrientation();
        CCamera::CCameraSnapshot *cameraSnapshot();
        MCameraFaceTracking *faceTracking();
        MCameraUseCaseHint *useCaseHintApi();

        virtual void setCamera( CCamera *aCamera );
        virtual void deleteCamera();
        virtual void releaseCamera();
        virtual void reserveCamera();
        virtual CxeError::Id newCamera(Cxe::CameraIndex cameraIndex, MCameraObserver2 *observer);

    protected:

    signals:
        void prepareForCameraDelete();
        void cameraAllocated(CxeError::Id error);
        void prepareForRelease();
        void aboutToReserve();

    private:
        CxeError::Id initResources();
        void releaseResources();

    private:
        CCamera *mCamera;
        CCamera::CCameraAdvancedSettings *mAdvancedSettings;
        CCamera::CCameraImageProcessing  *mImageProcessor;
        CCamera::CCameraSnapshot         *mCameraSnapshot;
        MCameraOrientation               *mCameraOrientation; // not owned
        MCameraFaceTracking              *mFaceTracking;      // not owned
        MCameraUseCaseHint               *mUseCaseHintApi;    // not owned

    friend class CxeCameraDeviceControlSymbian;
};


#endif  // CXECAMERADEVICE_H

