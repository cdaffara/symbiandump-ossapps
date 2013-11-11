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
#ifndef CXEFAKEENGINE_H
#define CXEFAKEENGINE_H

#include <QObject>
#include "cxeengine.h"
#include "cxestillcapturecontrolsymbian.h"
#include "cxevideocapturecontrolsymbian.h"
#include "cxeautofocuscontrolsymbian.h"
#include "cxecameradevicecontrolsymbian.h"
#include "cxezoomcontrolsymbian.h"
#include "cxeimagedataqueuesymbian.h"
#include "cxefilenamegeneratorsymbian.h"
#include "cxefakestillcapturecontrol.h"
#include "cxefakecameradevicecontrol.h"


class CxeCameraDeviceControl;
class CxeCameraDeviceControlSymbian;
class CxeViewfinderControl;
class CxeFakeViewfinderControl;
class CxeStillCaptureControl;
class CxeStillCaptureControlSymbian;
class CxeVideoCaptureControl;
class CxeVideoCaptureControlSymbian;
class CxeAutoFocusControl;
class CxeFeatureManager;
class CxeAutoFocusControlSymbian;
class CxeSettings;
class CxeFeatureManager;
class CxeSettingsModel;
class CxeFakeStillCaptureControl;
class CxeFakeAutoFocusControl;

class CxeFakeEngine : public CxeEngine
{
    Q_OBJECT
public:
    CxeFakeEngine();
    ~CxeFakeEngine();

    CxeCameraDeviceControl &cameraDeviceControl();
    CxeViewfinderControl &viewfinderControl();
    CxeStillCaptureControlSymbian &stillCaptureControl();
    CxeVideoCaptureControlSymbian &videoCaptureControl();
    CxeAutoFocusControlSymbian &autoFocusControl();
    CxeZoomControlSymbian &zoomControl();
    CxeSettings &settings();
    CxeFeatureManager &featureManager();
    Cxe::CameraMode mode() const;
    void setMode(Cxe::CameraMode mode);
    void initMode(Cxe::CameraMode cameraMode);
    bool isEngineReady();

    // Added for testing purposes
    void initModeFake(Cxe::CameraMode cameraMode);
    static CxeFakeEngine* createFakeEngine();
    CxeFakeStillCaptureControl &fakeStillCaptureControl();

private slots:
    /**
     * Prepare current capture control (CxeVideoCaptureControl in video mode,
     * CxeStillCaptureControl in still image mode).
     */
    void doInit();

private:

    /**
     * Used by initMode() to check if we need to prepare image/video
     * capture using doInit().
     */
    bool initNeeded();

    /**
     * Used by initMode() to check if we need to start viewfinder.
     */
    bool startViewfinderNeeded();
    /**
     * Used by initMode() to check if we need to reserve camera.
     */
    bool reserveNeeded();


private:
    //CxeCameraDeviceControlSymbian *mCameraDeviceControl;
    CxeFakeCameraDeviceControl *mFakeCameraDeviceControl;
    CxeFakeViewfinderControl *mViewfinderControl;
    //CxeStillCaptureControlSymbian *mStillCaptureControl;
    CxeVideoCaptureControlSymbian *mVideoCaptureControl;
    CxeFakeAutoFocusControl *mFakeAutoFocusControl;
    //CxeAutoFocusControlSymbian *mAutoFocusControl;
    CxeZoomControlSymbian *mZoomControl;
    CxeSettings *mSettings;
    CxeFeatureManager *mFeatureManager;
    CxeSettingsModel *mSettingsModel;
    CxeSoundPlayerSymbian *mSoundPlayer;
    CxeFilenameGeneratorSymbian *mFilenameGenerator;

    // Members created for testing purposes
    CxeFakeStillCaptureControl *mFakeStillCaptureControl;
};


#endif  // CXEFAKEENGINE_H

