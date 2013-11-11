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
#ifndef CXEENGINESYMBIAN_H
#define CXEENGINESYMBIAN_H

#include <QObject>
#include "cxeengine.h"
#include "cxefilenamegenerator.h"

class CxeCameraDevice;
class CxeCameraDeviceControl;
class CxeViewfinderControl;
class CxeSnapshotControl;
class CxeStillCaptureControl;
class CxeVideoCaptureControl;
class CxeSettingsControlSymbian;
class CxeAutoFocusControl;
class CxeFeatureManager;
class CxeAutoFocusControl;
class CxeSettings;
class CxeFeatureManager;
class CxeSensorEventHandler;
class CxeFilenameGenerator;
class CxeQualityPresets;
class CxeFileSaveThread;
class CxeDiskMonitor;
class CxeMemoryMonitor;
class CxeGeoTaggingTrail;


class CxeEngineSymbian : public CxeEngine
{
    Q_OBJECT
public:
    CxeEngineSymbian();
    virtual ~CxeEngineSymbian();

    CxeCameraDeviceControl &cameraDeviceControl();
    CxeViewfinderControl &viewfinderControl();
    CxeSnapshotControl &snapshotControl();
    CxeStillCaptureControl &stillCaptureControl();
    CxeVideoCaptureControl &videoCaptureControl();
    CxeAutoFocusControl &autoFocusControl();
    CxeZoomControl &zoomControl();
    CxeSettings &settings();
    CxeSensorEventHandler &sensorEventHandler();
    CxeFeatureManager &featureManager();
    CxeMemoryMonitor &memoryMonitor();
    CxeGeoTaggingTrail &geoTaggingTrail();
    Cxe::CameraMode mode() const;
    void setMode(Cxe::CameraMode mode);
    void initMode(Cxe::CameraMode cameraMode);
    bool isEngineReady();

    void construct();

signals:
    void reserveStarted();

protected:
    virtual void createControls();

private slots:
    void doInit();
    void initGeotagging();

private:
    void connectSignals();
    bool reserveNeeded();
    bool initNeeded();
    bool startViewfinderNeeded();
    void reserve();
    void saveMode();

protected:
    CxeCameraDeviceControl *mCameraDeviceControl;
    CxeCameraDevice *mCameraDevice; // not own
    CxeViewfinderControl *mViewfinderControl;
    CxeSnapshotControl *mSnapshotControl;
    CxeStillCaptureControl *mStillCaptureControl;
    CxeVideoCaptureControl *mVideoCaptureControl;
    CxeSettingsControlSymbian *mSettingsControl;
    CxeAutoFocusControl *mAutoFocusControl;
    CxeZoomControl *mZoomControl;
    CxeSettings *mSettings;
    CxeFeatureManager *mFeatureManager;
    CxeFilenameGenerator *mFilenameGenerator;
    CxeSensorEventHandler* mSensorEventHandler;
    CxeQualityPresets *mQualityPresets;
    CxeFileSaveThread *mFileSaveThread;
    CxeDiskMonitor *mDiskMonitor;
    CxeMemoryMonitor *mMemoryMonitor;
    CxeGeoTaggingTrail *mGeoTaggingTrail;
};


#endif  // CXEENGINESYMBIAN_H


