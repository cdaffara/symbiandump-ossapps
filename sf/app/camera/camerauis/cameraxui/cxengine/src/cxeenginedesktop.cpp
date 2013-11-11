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
#include "cxeenginedesktop.h"
#include "cxecameradevicecontroldesktop.h"
#include "cxestillcapturecontroldesktop.h"
#include "cxevideocapturecontroldesktop.h"
#include "cxeviewfindercontroldesktop.h"
#include "cxefilenamegeneratordesktop.h"
#include "cxeautofocuscontroldesktop.h"
#include "cxezoomcontroldesktop.h"
#include "cxequalitypresetsdesktop.h"
#include "cxutils.h"
#include "cxesettingsimp.h"
#include "cxefeaturemanagerimp.h"
#include "cxesettingsstoredesktop.h"
#include "cxesensoreventhandlerdesktop.h"
#include "cxefilesavethreaddesktop.h"
#include "cxecameradevicedesktop.h"
#include "cxememorymonitor.h"
#include "cxegeotaggingtrail.h"


//  Member Functions

CxeEngineDesktop::CxeEngineDesktop()
    : mCameraDeviceControl(NULL),
      mViewfinderControl(NULL),
      mSnapshotControl(NULL),
      mStillCaptureControl(NULL),
      mVideoCaptureControl(NULL),
      mAutoFocusControl(NULL),
      mZoomControl(NULL),
      mImageDataQueue(NULL),
      mSettings(NULL),
      mFeatureManager(NULL),
      mFilenameGenerator(NULL),
      mSensorEventHandler(NULL),
      mQualityPresets(NULL),
      mFileSaveThread(NULL),
      mCameraDevice(NULL),
      mDiskMonitor(NULL),
      mMemoryMonitor(NULL),
      mGeoTaggingTrail(NULL)
{
    CX_DEBUG_IN_FUNCTION();
}


/*!
    Create all control classes and connect relevant signals
*/
void CxeEngineDesktop::construct()
{
    CX_DEBUG_ENTER_FUNCTION();

    createControls();
    connectSignals();

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
    Create all control classes
*/
void CxeEngineDesktop::createControls()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mVideoCaptureControl) {
        CxeCameraDeviceControlDesktop *deviceControl =
                new CxeCameraDeviceControlDesktop();

        mCameraDeviceControl = deviceControl;

        mQualityPresets = new CxeQualityPresetsDesktop();
        CX_DEBUG_ASSERT(mQualityPresets);

        CxeSettingsStoreDesktop *settingsStore = new CxeSettingsStoreDesktop();

        //ownership of settings store transferred to the settings.
        mSettings = new CxeSettingsImp(settingsStore);

        // Loading current camera mode value from settings store and updating
        // devicecontrol
        Cxe::CameraMode cameraMode = mSettings->get<Cxe::CameraMode>(CxeSettingIds::CAMERA_MODE, Cxe::ImageMode);
        // set current camera mode to devicecontrol.
        mCameraDeviceControl->setMode(cameraMode);

        static_cast<CxeSettingsImp*>(mSettings)->loadSettings(mode());

        mFeatureManager = new CxeFeatureManagerImp(*mSettings);

        mMemoryMonitor = new CxeMemoryMonitor(*mFeatureManager);

        // sensor event handler initialization
        mSensorEventHandler = new CxeSensorEventHandlerDesktop();

        mFilenameGenerator = new CxeFilenameGeneratorDesktop();

        mFileSaveThread = new CxeFileSaveThreadDesktop(this);

        mCameraDevice = new CxeCameraDeviceDesktop();

        mViewfinderControl = new CxeViewfinderControlDesktop(*mCameraDevice);

        mAutoFocusControl = new CxeAutoFocusControlDesktop();
        
        mStillCaptureControl = new CxeStillCaptureControlDesktop(
            *mCameraDevice, *mViewfinderControl,
            *mCameraDeviceControl, *mFilenameGenerator,
            *mAutoFocusControl, *mFileSaveThread);

        mZoomControl = new CxeZoomControlDesktop(*mCameraDeviceControl);

        mVideoCaptureControl = new CxeVideoCaptureControlDesktop(
            *mCameraDevice,*mViewfinderControl,
            *mCameraDeviceControl, *mFilenameGenerator,
            *mQualityPresets);
   
        mGeoTaggingTrail = new CxeGeoTaggingTrail(*mStillCaptureControl,
                                                  *mVideoCaptureControl, *mSettings);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Connect internal signals for control classes
*/

void CxeEngineDesktop::connectSignals()
{
    CX_DEBUG_ENTER_FUNCTION();

    // connecting scene setting change callbacks to ViewfinderControl
    connect(mSettings,
            SIGNAL(settingValueChanged(const QString&,QVariant)),
            mViewfinderControl,
            SLOT(handleSettingValueChanged(const QString&,QVariant)));

    connect(mSettings,
            SIGNAL(sceneChanged(CxeScene&)),
            mViewfinderControl,
            SLOT(handleSceneChanged(CxeScene&)));

    // enabling scene setting change callbacks to Autofocus control
    connect(mSettings,
            SIGNAL(sceneChanged(CxeScene&)),
            mAutoFocusControl,
            SLOT(handleSceneChanged(CxeScene&)));

    // connecting Autofocus state change callbacks to stillcapturecontrol
    connect(mAutoFocusControl,
            SIGNAL(stateChanged(CxeAutoFocusControl::State, CxeError::Id)),
            mStillCaptureControl,
            SLOT(handleAutofocusStateChanged(CxeAutoFocusControl::State,CxeError::Id)));

    // Connect signals for ECam events
    connect(mCameraDeviceControl,
            SIGNAL(cameraEvent(int,int)),
            mVideoCaptureControl,
            SLOT(handleCameraEvent(int,int)));

    connect(mCameraDeviceControl,
            SIGNAL(cameraEvent(int,int)),
            mAutoFocusControl,
            SLOT(handleCameraEvent(int,int)));

    // Connect signal for device ready events
    connect(mCameraDeviceControl,
            SIGNAL(deviceReady()),
            this,
            SLOT(doInit()));

    // Connect image and video init complete signals to
    // CameraDeviceControl initModeComplete
    connect(mStillCaptureControl,
            SIGNAL(imagePrepareComplete(CxeError::Id)),
            mCameraDeviceControl,
            SIGNAL(initModeComplete(CxeError::Id)));

    connect(mVideoCaptureControl,
            SIGNAL(videoPrepareComplete(CxeError::Id)),
            mCameraDeviceControl,
            SIGNAL(initModeComplete(CxeError::Id)));

    // Zoom is prepared once the image/video emits prepare zoom signals
    connect(mStillCaptureControl,
            SIGNAL(prepareZoomForStill(int)),
            mZoomControl,
            SLOT(prepareZoomForStill(int)));
    
    connect(mVideoCaptureControl,
            SIGNAL(prepareZoomForVideo()),
            mZoomControl,
            SLOT(prepareZoomForVideo()));

    // connect camera device control prepare for release signal to stop location trail slot
    connect(mCameraDevice,
            SIGNAL(prepareForRelease()),
            mGeoTaggingTrail,
            SLOT(stop()), Qt::UniqueConnection);

    // init camera device control. We init the camera device control
    // when all necessary engine classes are constructed.
    mCameraDeviceControl->init();

    CX_DEBUG_EXIT_FUNCTION();
}

CxeEngineDesktop::~CxeEngineDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mGeoTaggingTrail;
    delete mAutoFocusControl;
    delete mZoomControl;
    delete mStillCaptureControl;
    delete mVideoCaptureControl;
    delete mViewfinderControl;
    delete mFilenameGenerator;
    delete mMemoryMonitor;
    delete mFeatureManager;
    delete mSettings;
    delete mCameraDeviceControl;
    delete mQualityPresets;
    delete mCameraDevice;
    CX_DEBUG_EXIT_FUNCTION();
}

CxeCameraDeviceControl &CxeEngineDesktop::cameraDeviceControl()
{
    return *mCameraDeviceControl;
}

CxeViewfinderControl &CxeEngineDesktop::viewfinderControl()
{
    return *mViewfinderControl;
}

CxeSnapshotControl &CxeEngineDesktop::snapshotControl()
{
    return *mSnapshotControl;
}

CxeStillCaptureControl &CxeEngineDesktop::stillCaptureControl()
{
    return *mStillCaptureControl;
}

CxeVideoCaptureControl &CxeEngineDesktop::videoCaptureControl()
{
    return *mVideoCaptureControl;
}

CxeAutoFocusControl &CxeEngineDesktop::autoFocusControl()
{
    return *mAutoFocusControl;
}

CxeZoomControl &CxeEngineDesktop::zoomControl()
{
    return *mZoomControl;
}

// Get the settings handle
CxeSettings &CxeEngineDesktop::settings()
{
    return *mSettings;
}

CxeFeatureManager& CxeEngineDesktop::featureManager()
{
    return *mFeatureManager;
}

/*
* Returns true, if the engine is ready or else false.
*/
bool CxeEngineDesktop::isEngineReady()
{
    bool ready = true;
    return ready;
}

/*!
* Get memory monitor utility handle.
*/
CxeMemoryMonitor &CxeEngineDesktop::memoryMonitor()
{
    return *mMemoryMonitor;
}

/*!
 * Get geotaggingtrail handle
 */
CxeGeoTaggingTrail &CxeEngineDesktop::geoTaggingTrail()
{
    return *mGeoTaggingTrail;
}

Cxe::CameraMode CxeEngineDesktop::mode() const
{
    return mCameraDeviceControl->mode();
}

/*!
 * Set camera mode.
 */
void CxeEngineDesktop::setMode(Cxe::CameraMode mode)
{
    CX_DEBUG_ENTER_FUNCTION();
    mCameraDeviceControl->setMode(mode);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeEngineDesktop::initMode(Cxe::CameraMode cameraMode)
{
    CX_DEBUG_ENTER_FUNCTION();

    mCameraDeviceControl->setMode(cameraMode);

    // load settings whenever we change mode or start camera or switch camera
    CxeSettingsImp *settingsImp = qobject_cast<CxeSettingsImp*>(mSettings);
    if (settingsImp) {
        settingsImp->loadSettings(mode());
    }

    if (cameraMode == Cxe::ImageMode) {
        mVideoCaptureControl->deinit();
        mStillCaptureControl->init();
    } else {
        mStillCaptureControl->deinit();
        mVideoCaptureControl->init();
    }
    mFilenameGenerator->init(cameraMode);

    CX_DEBUG_EXIT_FUNCTION();
}

// End of file
