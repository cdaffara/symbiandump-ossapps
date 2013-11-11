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
#ifndef CXEENGINEDESKTOP_H
#define CXEENGINEDESKTOP_H

#include <QObject> 
#include "cxeengine.h"

class CxeCameraDeviceControl;
class CxeViewfinderControl;
class CxeSnapshotControl;
class CxeStillCaptureControl;
class CxeVideoCaptureControl;
class CxeAutoFocusControl;
class CxeZoomControl;
class CxeImageDataQueue;
class CxeFeatureManager;
class CxeSettings;
class CxeFilenameGenerator;
class CxeFileSaveThread;
class CxeSensorEventHandler;
class CxeQualityPresets;
class CxeCameraDeviceDesktop;
class CxeDiskMonitor;
class CxeMemoryMonitor;
class CxeGeoTaggingTrail;

class CxeEngineDesktop : public CxeEngine
{
    Q_OBJECT
public:
    CxeEngineDesktop();
    virtual ~CxeEngineDesktop();

    void construct();

    void createControls();

    void connectSignals();
public: // from CxeEngine
    virtual CxeCameraDeviceControl &cameraDeviceControl();
    virtual CxeViewfinderControl &viewfinderControl();
    virtual CxeSnapshotControl &snapshotControl();
    virtual CxeStillCaptureControl &stillCaptureControl();
    virtual CxeVideoCaptureControl &videoCaptureControl();
    virtual CxeAutoFocusControl &autoFocusControl();
    virtual CxeZoomControl &zoomControl();
    virtual CxeSettings &settings();
    virtual CxeFeatureManager &featureManager();
    virtual CxeMemoryMonitor &memoryMonitor();
    virtual CxeGeoTaggingTrail &geoTaggingTrail();
    virtual Cxe::CameraMode mode() const;
    void setMode(Cxe::CameraMode mode);
    virtual void initMode(Cxe::CameraMode cameraMode);
    virtual bool isEngineReady();

protected:
    CxeCameraDeviceControl *mCameraDeviceControl;
    CxeViewfinderControl *mViewfinderControl;
    CxeSnapshotControl *mSnapshotControl;
    CxeStillCaptureControl *mStillCaptureControl;
    CxeVideoCaptureControl *mVideoCaptureControl;
    CxeAutoFocusControl *mAutoFocusControl;
    CxeZoomControl *mZoomControl;
    CxeImageDataQueue *mImageDataQueue;
    CxeSettings *mSettings;
    CxeFeatureManager *mFeatureManager;
    CxeFilenameGenerator *mFilenameGenerator;
    CxeSensorEventHandler *mSensorEventHandler;
    CxeQualityPresets *mQualityPresets;
    CxeFileSaveThread *mFileSaveThread;
    CxeCameraDeviceDesktop *mCameraDevice;
    CxeDiskMonitor *mDiskMonitor;
    CxeMemoryMonitor *mMemoryMonitor;
    CxeGeoTaggingTrail *mGeoTaggingTrail;

};

#endif // CXEENGINEDESKTOP_H
