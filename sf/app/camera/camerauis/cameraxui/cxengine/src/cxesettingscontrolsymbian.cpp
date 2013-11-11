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

#include <ecam.h>
#include <ecamadvsettings.h>
#include <ecamimageprocessing.h>
#include <ecam/ecamconstants.h>
#include <ecamfacetrackingcustomapi.h>

#include <QString>
#include <QVariant>

#include "cxutils.h"
#include "cxenamespace.h"
#include "cxecameradevice.h"
#include "cxesettings.h"
#include "cxesettingsmappersymbian.h"
#include "cxesettingscontrolsymbian.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxesettingscontrolsymbianTraces.h"
#endif


/*!
* Constructor
*/
CxeSettingsControlSymbian::CxeSettingsControlSymbian(
        CxeCameraDevice &cameraDevice, CxeSettings &settings)
    : mCameraDevice(cameraDevice),
      mSettings(settings)
{
    // connect scene / setting change callbacks to settings control
    connect(&mSettings,
            SIGNAL(settingValueChanged(const QString&,QVariant)),
            this,
            SLOT(handleSettingValueChanged(const QString&,QVariant)));

    mSettings.listenForSetting(CxeSettingIds::IMAGE_SCENE_DATA, this, SLOT(handleSceneChanged(const QVariant&)));
    mSettings.listenForSetting(CxeSettingIds::VIDEO_SCENE_DATA, this, SLOT(handleSceneChanged(const QVariant&)));

}

/*!
* Destructor
*/
CxeSettingsControlSymbian::~CxeSettingsControlSymbian()
{
}

/*!
* Handle new setting value.
* New value is set to camera.
* \param settingId The id of the updated setting
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::handleSettingValueChanged(
        const QString &settingId, QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (settingId == CxeSettingIds::COLOR_TONE) {
        updateColorToneSetting(newValue);
    } else if (settingId == CxeSettingIds::WHITE_BALANCE) {
        updateWhiteBalanceSetting(newValue);
    } else if (settingId == CxeSettingIds::LIGHT_SENSITIVITY) {
            updateLightSensitivitySetting(newValue);
    } else if (settingId == CxeSettingIds::SHARPNESS) {
        updateSharpnessSetting(newValue);
    } else if (settingId == CxeSettingIds::CONTRAST) {
        updateContrastSetting(newValue);
    } else if (settingId == CxeSettingIds::BRIGHTNESS) {
        updateBrightnessSetting(newValue);
    } else if (settingId == CxeSettingIds::EXPOSURE_MODE) {
        updateExposureModeSetting(newValue);
    } else if (settingId == CxeSettingIds::EV_COMPENSATION_VALUE) {
            updateExposureCompensationSetting(newValue);
    } else if (settingId == CxeSettingIds::FACE_TRACKING) {
            updateFaceTrackingSetting(newValue);
    } else {
        // do nothing
    }

    // Image mode specific
    if (settingId == CxeSettingIds::FLASH_MODE) {
        updateFlashSetting(newValue);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle new scene being set.
* Scene settings are checked and new values are set to camera.
* \param scene New scene containing scene specific settings.
*/
void CxeSettingsControlSymbian::handleSceneChanged(const QVariant &sceneData)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeScene scene = sceneData.value<CxeScene>();
    OstTrace0(camerax_performance, CXESETTINGSCONTROL_SCENE_1, "msg: e_CX_SCENE_SETTINGS_TO_CAMERA 1");

    CX_DEBUG(("CxeSettingsControlSymbian <> new scene [%s]",
              scene[CxeSettingIds::SCENE_ID].value<QString>().toAscii().constData()));

    foreach (const QString& settingId, scene.keys()) {
        handleSettingValueChanged(settingId, scene[settingId]);
    }

    OstTrace0(camerax_performance, CXESETTINGSCONTROL_SCENE_2, "msg: e_CX_SCENE_SETTINGS_TO_CAMERA 0");
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update color tone setting value to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateColorToneSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

//! @todo: Should be fixed in Dummy engine
#if !defined(__WINSCW__) && !defined(CXE_USE_DUMMY_CAMERA)

    CX_ASSERT_ALWAYS(mCameraDevice.imageProcessor());

    CCamera::CCameraImageProcessing::TEffect effect =
            CxeSettingsMapperSymbian::Map2CameraEffect(
                    static_cast<Cxe::Colortone>(newValue.toInt()));

    mCameraDevice.imageProcessor()->SetTransformationValue(
            KUidECamEventImageProcessingEffect, effect);

#else
    Q_UNUSED(newValue)
#endif

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update white balance setting value to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateWhiteBalanceSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mCameraDevice.camera());

    CCamera::TWhiteBalance wb = CxeSettingsMapperSymbian::Map2CameraWb(
            static_cast<Cxe::Whitebalance>(newValue.toInt()));

    TRAP_IGNORE(mCameraDevice.camera()->SetWhiteBalanceL(wb));

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update light sensitivity (ISO) setting value to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateLightSensitivitySetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice.advancedSettings());

    int iso = newValue.toInt();

    CCamera::CCameraAdvancedSettings::TISORateType type;
    if (iso == 0) {
        // Automatic ISO rate
        type = CCamera::CCameraAdvancedSettings::EISOAutoUnPrioritised;
    } else {
        // Manual ISO rate
        type = CCamera::CCameraAdvancedSettings::EISOManual;
    }
    TRAP_IGNORE(mCameraDevice.advancedSettings()->SetISORateL(type, iso));

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update sharpness setting value to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateSharpnessSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

//! @todo: Should be fixed in Dummy engine
#if !defined(__WINSCW__) && !defined(CXE_USE_DUMMY_CAMERA)

    CX_ASSERT_ALWAYS(mCameraDevice.imageProcessor());

    // Scale UI values of -2..2 to ECAM range -100..100.
    int newSharpness = newValue.toReal() * 50;

    CX_DEBUG(("Setting sharpness [uid:0x%08x] to value [%d]",
              KUidECamEventImageProcessingAdjustSharpness, newSharpness));
    mCameraDevice.imageProcessor()->SetTransformationValue(
            KUidECamEventImageProcessingAdjustSharpness, newSharpness);

#else
    Q_UNUSED(newValue)
#endif

   CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update contrast setting value to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateContrastSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice.imageProcessor());

    // Scale UI values of -2..2 to ECAM range -100..100.
    int newContrast = newValue.toReal() * 50;

    CX_DEBUG(("Setting contrast [uid:0x%08x] to value [%d]",
              KUidECamEventImageProcessingAdjustContrast, newContrast));

    mCameraDevice.imageProcessor()->SetTransformationValue(
            KUidECamEventImageProcessingAdjustContrast, newContrast);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update brightness setting value to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateBrightnessSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice.imageProcessor());

    // Scale UI values of -10..10 to ECAM range -100..100.
    int newBrightness = newValue.toInt() * 10;

    CX_DEBUG(("Setting brightness [uid:0x%08x] to value [%d]",
              KUidECamEventImageProcessingAdjustBrightness, newBrightness));

    mCameraDevice.imageProcessor()->SetTransformationValue(
            KUidECamEventImageProcessingAdjustBrightness, newBrightness);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update Exposure mode to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateExposureModeSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mCameraDevice.advancedSettings());

    CCamera::TExposure exposure =
            CxeSettingsMapperSymbian::Map2CameraExposureMode(
                    static_cast<Cxe::ExposureMode>(newValue.toInt()));

    mCameraDevice.advancedSettings()->SetExposureMode(exposure);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update Exposure Compensation mode to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateExposureCompensationSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice.advancedSettings());

    // Exposure compensation is a float value, e.g. "-1.5".
    // ECAM interface takes integer values, so KECamFineResolutionFactor from
    // ecamconstants.h needs to be used as scaler.
    int ev = newValue.toReal() * KECamFineResolutionFactor;

    mCameraDevice.advancedSettings()->SetExposureCompensationStep(ev);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update flash mode to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateFlashSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice.camera());

    CCamera::TFlash flash = CxeSettingsMapperSymbian::Map2CameraFlash(
            static_cast<Cxe::FlashMode>(newValue.toInt()));

    TRAP_IGNORE(mCameraDevice.camera()->SetFlashL(flash));

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Update face tracking mode to the camera device
* \param newValue A new value for the updated setting
*/
void CxeSettingsControlSymbian::updateFaceTrackingSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    MCameraFaceTracking *faceTracking = mCameraDevice.faceTracking();
    if (faceTracking) {
        TRAP_IGNORE(faceTracking->SetFaceTrackingL(newValue.toInt()));
    }
    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
