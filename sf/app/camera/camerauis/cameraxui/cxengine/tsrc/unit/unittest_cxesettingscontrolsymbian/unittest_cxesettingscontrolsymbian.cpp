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

#include <QTest>

#include "cxenamespace.h"
#include "cxutils.h"
#include "cxeautofocuscontrol.h"
#include "cxefakesettings.h"
#include "cxefakecameradevice.h"
#include "cxefakecameradevicecontrol.h"
#include "cxesettingscontrolsymbian.h"
#include "unittest_cxesettingscontrolsymbian.h"


using namespace Cxe;
using namespace CxeSettingIds;


UnitTestCxeSettingsControlSymbian::UnitTestCxeSettingsControlSymbian()
{
}

UnitTestCxeSettingsControlSymbian::~UnitTestCxeSettingsControlSymbian()
{
}

void UnitTestCxeSettingsControlSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    mCameraDeviceControl = new CxeFakeCameraDeviceControl();
    mCameraDevice = new CxeFakeCameraDevice();
    mCameraDevice->newCamera(mCameraDeviceControl->cameraIndex(), mCameraDeviceControl);

    mSettings = new CxeFakeSettings();
    mSettingsControl = new CxeSettingsControlSymbian(*mCameraDevice, *mSettings);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeSettingsControlSymbian::cleanup()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mSettingsControl;
    mSettingsControl = NULL;

    delete mSettings;
    mSettings = NULL;

    delete mCameraDevice;
    mCameraDevice = NULL;

    delete mCameraDeviceControl;
    mCameraDeviceControl = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeSettingsControlSymbian::testHandleSettingValueChanged()
{
    CX_DEBUG_ENTER_FUNCTION();

    mSettingsControl->handleSettingValueChanged(FLASH_MODE, QVariant(1));
    mSettingsControl->handleSettingValueChanged(WHITE_BALANCE, QVariant(1));
    mSettingsControl->handleSettingValueChanged(LIGHT_SENSITIVITY, QVariant(1));
    mSettingsControl->handleSettingValueChanged(EXPOSURE_MODE, QVariant(1));
    mSettingsControl->handleSettingValueChanged(SHARPNESS, QVariant(1));
    mSettingsControl->handleSettingValueChanged(COLOR_TONE, QVariant(1));
    mSettingsControl->handleSettingValueChanged(EV_COMPENSATION_VALUE, QVariant(1));
    mSettingsControl->handleSettingValueChanged(CONTRAST, QVariant(1));
    mSettingsControl->handleSettingValueChanged(BRIGHTNESS, QVariant(1));
    mSettingsControl->handleSettingValueChanged(FRAME_RATE, QVariant(1));

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeSettingsControlSymbian::testHandleSceneChanged()
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeScene videoScene;

    videoScene.insert(CxeSettingIds::SCENE_ID, Cxe::VIDEO_SCENE_AUTO);
    videoScene.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Hyperfocal);
    videoScene.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    videoScene.insert(CxeSettingIds::EXPOSURE_MODE, ExposureAuto);
    videoScene.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    videoScene.insert(CxeSettingIds::CONTRAST, 0);
    videoScene.insert(CxeSettingIds::FRAME_RATE, 0);
    videoScene.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);

    mSettingsControl->handleSceneChanged(videoScene);

    CxeScene imageScene;

    imageScene.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_NIGHT);
    imageScene.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Auto);
    imageScene.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imageScene.insert(CxeSettingIds::EXPOSURE_MODE, ExposureNight);
    imageScene.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imageScene.insert(CxeSettingIds::CONTRAST, 0);
    imageScene.insert(CxeSettingIds::SHARPNESS, SharpnessNormal);
    imageScene.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imageScene.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imageScene.insert(CxeSettingIds::BRIGHTNESS, 0);
    imageScene.insert(CxeSettingIds::FLASH_MODE, FlashOff);

    mSettingsControl->handleSceneChanged(imageScene);

    CX_DEBUG_EXIT_FUNCTION();
}


// main() function
// If need event loop for waiting signals, use QTEST_MAIN.
// Need also active scheduler, so can't use QTEST_APPLESS_MAIN
QTEST_MAIN(UnitTestCxeSettingsControlSymbian);

// end of file
