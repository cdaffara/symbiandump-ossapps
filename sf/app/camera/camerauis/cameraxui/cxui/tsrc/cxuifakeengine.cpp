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
#include "cxuifakeengine.h"                // CxeEngineSymbian
#include "cxecameradevicecontrolsymbian.h"  // CxeCameraDeviceControlSymbian
#include "cxuifakestillcapturecontrol.h"
#include "cxevideocapturecontrolsymbian.h"
#include "cxestillcapturecontrolsymbian.h"
#include "cxuifakeviewfindercontrol.h"
#include "cxezoomcontrolsymbian.h"
#include "cxutils.h"
#include "cxesettingsimp.h"
#include "cxefeaturemanagerimp.h"
#include "cxesettingsmodel.h"
#include "cxesoundplayersymbian.h"
#include "cxuifakeautofocuscontrol.h"

//  Member Functions

CxuiFakeEngine::CxuiFakeEngine()
 :  mViewfinderControl( NULL ),
    mFakeStillCaptureControl( NULL ),
    mVideoCaptureControl( NULL ),
    mZoomControl( NULL ),
    mSettings( NULL )
{
    CX_DEBUG_ENTER_FUNCTION();

    // Create all control classes
    mFakeCameraDeviceControl = new CxuiFakeCameraDeviceControl();

    CxeCameraDevice* cameraDevice = mFakeCameraDeviceControl->cameraDevice();

    mSettingsModel = new CxeSettingsModel();
    mSettings = new CxeSettingsImp(*mSettingsModel);
    mFeatureManager = new CxeFeatureManagerImp(*mSettingsModel);
    mSoundPlayer = new CxeSoundPlayerSymbian(*mFakeCameraDeviceControl);
    mFilenameGenerator = new CxeFilenameGeneratorSymbian(*mSettings, mode());

    mViewfinderControl = new CxuiFakeViewfinderControl(*cameraDevice, *mFakeCameraDeviceControl);

    mFakeStillCaptureControl = new CxuiFakeStillCaptureControl(
            *cameraDevice,
            *mViewfinderControl,
            *mFakeCameraDeviceControl,
            *mSoundPlayer,
            *mFilenameGenerator);

    mFakeAutoFocusControl = new CxuiFakeAutoFocusControl(*cameraDevice);

    mVideoCaptureControl = new CxeVideoCaptureControlSymbian(
            *cameraDevice,
            *mViewfinderControl,
            *mFakeCameraDeviceControl,
            *mSoundPlayer,
            *mFilenameGenerator);

    // connecting camera mode changes to updating capture sounds
    connect(mFakeCameraDeviceControl, SIGNAL(initModeComplete(CxeError::Id)),
            mSoundPlayer, SLOT(updateCaptureSound()));

    // connecting scene setting change callbacks to ViewfinderControl
    connect(mSettings, SIGNAL(settingValueChanged(const QString&,QVariant)),
            mViewfinderControl, SLOT(handleSettingValueChanged(const QString&,QVariant)));

    connect(mSettings, SIGNAL(sceneChanged(CxeScene&)),
            mViewfinderControl, SLOT(handleSceneChanged(CxeScene&)));

    // enabling scene setting change callbacks to stillcapturecontrol
    connect(mSettings, SIGNAL(settingValueChanged(const QString&,QVariant)),
            mFakeStillCaptureControl, SLOT(handleSettingValueChanged(const QString&,QVariant)));

    connect(mSettings, SIGNAL(sceneChanged(CxeScene&)),
            mFakeStillCaptureControl, SLOT(handleSceneChanged(CxeScene&)));

    // enabling scene setting change callbacks to videocapturecontrol
    connect(mSettings, SIGNAL(settingValueChanged(const QString&,QVariant)),
            mVideoCaptureControl, SLOT(handleSettingValueChanged(const QString&,QVariant)));

    connect(mSettings, SIGNAL(sceneChanged(CxeScene&)),
            mVideoCaptureControl, SLOT(handleSceneChanged(CxeScene&)));


    // enabling callbacks to Autofocus control
    connect(mSettings, SIGNAL(sceneChanged(CxeScene&)),
            mFakeAutoFocusControl, SLOT(handleSceneChanged(CxeScene&)) );

    // Connect signals for ECam events
    // TJ
    connect( mFakeCameraDeviceControl, SIGNAL(cameraEvent(int,int)),
             mFakeStillCaptureControl, SLOT(handleCameraEvent(int,int)) );
    connect( mFakeCameraDeviceControl, SIGNAL(cameraEvent(int,int)),
            mVideoCaptureControl, SLOT(handleCameraEvent(int,int)) );
    connect( mFakeCameraDeviceControl, SIGNAL(cameraEvent(int,int)),
            mFakeAutoFocusControl, SLOT(handleCameraEvent(int,int)) );

    // Connect signal for device ready events
    connect( mFakeCameraDeviceControl, SIGNAL(deviceReady()),
             this, SLOT(doInit()) );

    // Connect ECam image buffer ready event
    connect( mFakeCameraDeviceControl, SIGNAL(imageBufferReady(MCameraBuffer*,int)),
            mFakeStillCaptureControl, SLOT(handleImageData(MCameraBuffer*,int)) );

    // Connect ECam VF frame ready event
    connect( mFakeCameraDeviceControl, SIGNAL(vfFrameReady(MCameraBuffer*,int)),
             mViewfinderControl, SLOT(handleVfFrame(MCameraBuffer*,int)) );

    // Connect image and video init complete signals to CameraDeviceControl initModeComplete
    connect(mFakeStillCaptureControl, SIGNAL(imagePrepareComplete(CxeError::Id)),
             mFakeCameraDeviceControl, SIGNAL(initModeComplete(CxeError::Id)) );

    connect(mVideoCaptureControl, SIGNAL(videoPrepareComplete(CxeError::Id)),
             mFakeCameraDeviceControl, SIGNAL(initModeComplete(CxeError::Id)) );

    CX_DEBUG_EXIT_FUNCTION();
}

CxuiFakeEngine::~CxuiFakeEngine()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mViewfinderControl;
    delete mZoomControl;
    delete mVideoCaptureControl;
    delete mSoundPlayer;
    delete mFilenameGenerator;

    CX_DEBUG_EXIT_FUNCTION();
}

CxeCameraDeviceControl &CxuiFakeEngine::cameraDeviceControl()
{
    return *mFakeCameraDeviceControl;
}

CxeViewfinderControl &CxuiFakeEngine::viewfinderControl()
{
    return *mViewfinderControl;
}

CxeStillCaptureControlSymbian &CxuiFakeEngine::stillCaptureControl()
{
    return *mFakeStillCaptureControl;
}

CxeVideoCaptureControlSymbian &CxuiFakeEngine::videoCaptureControl()
{
    return *mVideoCaptureControl;
}

CxeAutoFocusControlSymbian &CxuiFakeEngine::autoFocusControl()
{
    return *mFakeAutoFocusControl;
}

CxeZoomControlSymbian &CxuiFakeEngine::zoomControl()
{
    return *mZoomControl;
}

// Get the settings handle
CxeSettings &CxuiFakeEngine::settings()
{
    return *mSettings;
}

CxeFeatureManager& CxuiFakeEngine::featureManager()
{
    return *mFeatureManager;
}

void CxuiFakeEngine::doInit()
{
    CX_DEBUG_ENTER_FUNCTION();
    // load settings whenever we change mode or start camera or switch camera
    //static_cast<CxeSettingsImp*>(mSettings)->loadSettings(mode());
    mFilenameGenerator->init(mode());

    if ( mode() == Cxe::ImageMode ) {
        mFakeStillCaptureControl->init();
    } else if ( mode() == Cxe::VideoMode ) {
        mFakeStillCaptureControl->deinit();
        mVideoCaptureControl->init();
    }
}

Cxe::CameraMode CxuiFakeEngine::mode() const
{
    return mFakeCameraDeviceControl->mode();
}

void CxuiFakeEngine::initModeFake(Cxe::CameraMode cameraMode)
{
    mFakeCameraDeviceControl->setMode( cameraMode );
    if( cameraMode == Cxe::ImageMode) {
        mFakeStillCaptureControl->setFakeState(CxeStillCaptureControl::Ready);
    }

}

void CxuiFakeEngine::initMode(Cxe::CameraMode cameraMode)
{
    CX_DEBUG_ENTER_FUNCTION();

      if (mode() == cameraMode) {
          CX_DEBUG(("initMode() called for current mode"));

          // We're already in the requested mode. However, it is possible
          // that we need to reserve and power on camera and/or prepare
          // the capture control.
          if (reserveNeeded()) {
              // Camera needs to be reserved. Initialization will continue
              // when we get the deviceReady() signal.
              CX_DEBUG(("initMode() - calling reserve()"));
              mFakeCameraDeviceControl->reserve();
          } else if (initNeeded()) {
              // Camera is reserved and ready, but we need to prepare still image control or
              // video capture control
              CX_DEBUG(("initMode() - calling doInit()"));
              doInit();
          } else if (startViewfinderNeeded()) {
              // Everything is ready and prepared, but we viewfinder is not yet running
              CX_DEBUG(("initMode() - calling viewfinder start()"));
              mViewfinderControl->start();
          } else {
              CX_DEBUG(("initMode() - no actions needed"));
              // Do nothing.
              // We are either already ready to capture or the we're pending
              // on prepare complete callback.
          }
      } else {
          CX_DEBUG(("initMode() called for mode switch"));

          // Mode switch
          mFakeCameraDeviceControl->setMode(cameraMode);

          if (reserveNeeded()) {
              CX_DEBUG(("initMode() - calling reserve()"));
              mFakeCameraDeviceControl->reserve();
          } else if ( mFakeCameraDeviceControl->state() == CxeCameraDeviceControl::Ready ) {
              CX_DEBUG(("initMode() - calling doInit()"));
              // Camera device is ready... we only need to prepare video or still
              // capture control, depending on current mode.
              doInit();
          } else {
              CX_DEBUG(("initMode() - no actions needed"));
              // Do nothing.
              // Camera device control is in Initializing state. Initialization will continue
              // when we get the deviceReady() callback.
          }
      }

      CX_DEBUG_EXIT_FUNCTION();
}

bool CxuiFakeEngine::initNeeded()
{
    bool result = false;

    if (mode() == Cxe::ImageMode &&
            mFakeCameraDeviceControl->state() == CxeCameraDeviceControl::Ready &&
        mFakeStillCaptureControl->state() == CxeStillCaptureControl::Uninitialized) {
        // We're in still image mode, camera is reserved and ready, but
        // we need to prepare still image control
        result = true;
    } else if (mode() == Cxe::VideoMode &&
            mFakeCameraDeviceControl->state() == CxeCameraDeviceControl::Ready) {
        if (mVideoCaptureControl->state() == CxeVideoCaptureControl::Open ||
            mVideoCaptureControl->state() == CxeVideoCaptureControl::Idle) {
            // We're in video mode, camera is reserved and ready, but
            // we need to open and prepare video recorder.
            result = true;
        }
    }

    return result;
}

bool CxuiFakeEngine::startViewfinderNeeded()
{
    bool result = false;

    if (mViewfinderControl->state() != CxeViewfinderControl::Running) {
        if (mode() == Cxe::ImageMode &&
                mFakeStillCaptureControl->state() == CxeStillCaptureControl::Ready) {
            // We're in still image mode and ready for capturing... we just need to
            // start viewfinder.
            result = true;
        } else if (mode() == Cxe::VideoMode &&
                   mVideoCaptureControl->state() == CxeVideoCaptureControl::Ready) {
            // We're in video mode and ready for capturing... we just need to
            // start viewfinder.
            result = true;
        }
    }

    return result;
}

bool CxuiFakeEngine::reserveNeeded()
{
    CX_DEBUG_ENTER_FUNCTION();
    // If camera device control is in Idle state, we need to call reserve()
    return (mFakeCameraDeviceControl->state() == CxeCameraDeviceControl::Idle);
}

// This should be the only exported method
CxuiFakeEngine *CxuiFakeEngine::createFakeEngine()
    {
    CX_DEBUG_ENTER_FUNCTION();

    CxuiFakeEngine* res = new CxuiFakeEngine();

    CX_DEBUG_EXIT_FUNCTION();
    return res;
    }


CxuiFakeStillCaptureControl &CxuiFakeEngine::fakeStillCaptureControl()
{
    return *mFakeStillCaptureControl;
}

// End of file
