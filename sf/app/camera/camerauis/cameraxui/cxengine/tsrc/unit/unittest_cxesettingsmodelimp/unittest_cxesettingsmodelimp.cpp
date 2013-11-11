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

#include "cxeerror.h"
#include "e32err.h"
#include "cxenamespace.h"
#include "cxesettingsmodelimp.h"
#include "cxefakesettingsstore.h"
#include "unittest_cxesettingsmodelimp.h"

// constants
static const char* INVALID_KEY = "invalid key";
static const char* EMPTY_KEY = "";

UnitTestCxeSettingsModelImp::UnitTestCxeSettingsModelImp()
{
}

UnitTestCxeSettingsModelImp::~UnitTestCxeSettingsModelImp()
{
}

// Run before each individual test case
void UnitTestCxeSettingsModelImp::init()
{
    CxeSettingsStore *fakeSettingsStore = new CxeFakeSettingsStore();
    mSettingsModel = new CxeSettingsModelImp(fakeSettingsStore);
}

// Run after each individual test case
void UnitTestCxeSettingsModelImp::cleanup()
{
    delete mSettingsModel;
    mSettingsModel = NULL;
}

// test case to test Setting Values
// 1. default values of setting keys
// 2. set values to setting keys
// 3. checking if set values are set correctly
// 4. covers also cases where we test with invalid cases
void UnitTestCxeSettingsModelImp::testSettingValues()
{
    CxeError::Id error;
    QVariant result;

    // case 0: check default value of a real setting for e.g. FNAME_IMAGE_COUNTER
    error = mSettingsModel->getSettingValue(CxeSettingIds::FNAME_IMAGE_COUNTER, result);
    QVERIFY(error == CxeError::None);
    QCOMPARE(result.toInt(), 0);

    // case 1: setting a value to a valid key
    error = mSettingsModel->set(CxeSettingIds::FNAME_IMAGE_COUNTER, 12);
    QVERIFY(error == CxeError::None);

    // case 2: testing if the value set is right to sharpness key
    error = mSettingsModel->getSettingValue(CxeSettingIds::FNAME_IMAGE_COUNTER, result);
    QVERIFY(error == CxeError::None);
    QCOMPARE(result.toInt(), 12);

    // case 3: trying to set a value to an invalid key which doesnt exit
    error = mSettingsModel->set(INVALID_KEY, QVariant(10));
    QVERIFY(error == CxeError::NotFound);

    // case 4: trying to get a value from invalid key
    error = mSettingsModel->getSettingValue(INVALID_KEY, result);
    QVERIFY(error == CxeError::NotFound);

    // case 5: try with an empty string
    error = mSettingsModel->set(EMPTY_KEY, QVariant(10));
    QVERIFY(error == CxeError::NotFound);

    // case 6: try with an empty string
    error = mSettingsModel->getSettingValue(EMPTY_KEY, result);
    QVERIFY(error == CxeError::NotFound);
}

// test case to test RuntimeKeys
// 1. default values of runtime keys
// 2. try set values to runtime keys
// 3. covers also cases where we test with invalid cases
void UnitTestCxeSettingsModelImp::testRuntimeValues()
{
    CxeError::Id error;
    QVariant result;
    QVariantList values;

    // case 1: check the default value of runtime
    error = mSettingsModel->getRuntimeValue(CxeRuntimeKeys::PRIMARY_CAMERA_CAPTURE_KEYS, result);
    QVERIFY(error == CxeError::None);
    // checking value
    if( error == CxeError::None ) {
        values = qVariantValue<QList<QVariant> >(result);
        QCOMPARE(values[0].toInt(), 1);
    }

    // case 2: trying to get a value from invalid key
    error = mSettingsModel->getRuntimeValue(INVALID_KEY, result);
    QVERIFY(error == CxeError::NotFound);

    // case 3: trying to set a value to a runtime key
    error = mSettingsModel->set(CxeRuntimeKeys::PRIMARY_CAMERA_CAPTURE_KEYS, 10);
    QVERIFY(error == CxeError::NotFound);

    // case 4: try with an empty string
    error = mSettingsModel->set(EMPTY_KEY, 10);
    QVERIFY(error == CxeError::NotFound);

    // case 5: try with an empty string
    error = mSettingsModel->getRuntimeValue(EMPTY_KEY, result);
    QVERIFY(error == CxeError::NotFound);

}


// test case to test Image scene Setting Values
// 1. default values of setting keys
// 2. set values to setting keys
// 3. checking if set values are set correctly
// 4. covers also cases where we test with invalid cases
void UnitTestCxeSettingsModelImp::testImageSceneSettingValues()
{
    CxeError::Id error;
    QVariant result;
    // testing for image mode settings
    mSettingsModel->cameraModeChanged(Cxe::ImageMode);

    // case 0: check sharpness default value
    error = mSettingsModel->getSettingValue(CxeSettingIds::SHARPNESS, result);
    QVERIFY(error == CxeError::None);
    QCOMPARE(result.toInt(), 0);

    // case 1: setting a value to a valid key
    error = mSettingsModel->set(CxeSettingIds::SHARPNESS, QVariant(100));
    QVERIFY(error == CxeError::None);

    // case 2: testing if the value set is right to sharpness key
    error = mSettingsModel->getSettingValue(CxeSettingIds::SHARPNESS, result);
    QVERIFY(error == CxeError::None);
    QCOMPARE(result.toInt(), 100);

    // case 3: trying to set a value to an invalid key which doesnt exit
    error = mSettingsModel->set(INVALID_KEY, 10);
    QVERIFY(error == CxeError::NotFound);

    // case 4: trying to get a value from invalid key
    error = mSettingsModel->getSettingValue(INVALID_KEY, result);
    QVERIFY(error == CxeError::NotFound);

    // case 5: try with an empty string
    error = mSettingsModel->set(EMPTY_KEY, 10);
    QVERIFY(error == CxeError::NotFound);

    // case 6: try with an empty string
    error = mSettingsModel->getSettingValue(EMPTY_KEY, result);
    QVERIFY(error == CxeError::NotFound);
}


// test case to test Image scene Setting Values
// 1. default values of setting keys
// 2. set values to setting keys
// 3. checking if set values are set correctly
// 4. covers also cases where we test with invalid cases
void UnitTestCxeSettingsModelImp::testVideoSceneSettingValues()
{
    CxeError::Id error;
    QVariant result;
    // testing for image mode settings
    mSettingsModel->cameraModeChanged(Cxe::VideoMode);

    // case 0: check colortone default value
    error = mSettingsModel->getSettingValue(CxeSettingIds::COLOR_TONE, result);
    QVERIFY(error == CxeError::None);
    QVERIFY(result.toInt() == Cxe::ColortoneNormal);

    error = mSettingsModel->set(CxeSettingIds::COLOR_TONE, Cxe::ColortoneNegative);
    QVERIFY(error == CxeError::None);

    // case 2: testing if the value set to invalid sharpness key
    error = mSettingsModel->getSettingValue(CxeSettingIds::COLOR_TONE, result);
    QVERIFY(error == CxeError::None);
    QVERIFY(result.toInt() == Cxe::ColortoneNegative);

    // case 1: setting a value to a invalid key
    error = mSettingsModel->set(CxeSettingIds::SHARPNESS, 100);
    QVERIFY(error == CxeError::NotFound);

    // case 2: testing if the value set to invalid sharpness key
    error = mSettingsModel->getSettingValue(CxeSettingIds::SHARPNESS, result);
    QVERIFY(error == CxeError::NotFound);


    // case 3: trying to set a value to an invalid key which doesnt exit
    error = mSettingsModel->set(INVALID_KEY, 10);
    QVERIFY(error == CxeError::NotFound);

    // case 4: trying to get a value from invalid key
    error = mSettingsModel->getSettingValue(INVALID_KEY, result);
    QVERIFY(error == CxeError::NotFound);

    // case 5: try with an empty string
    error = mSettingsModel->set(EMPTY_KEY, 10);
    QVERIFY(error == CxeError::NotFound);

    // case 6: try with an empty string
    error = mSettingsModel->getSettingValue(EMPTY_KEY, result);
    QVERIFY(error == CxeError::NotFound);
}



// test case to test ImageScene
// 1. default values of image scene
// 2. try set new image scene and checking if its succesful
// 3. covers also cases where we test with invalid cases
void UnitTestCxeSettingsModelImp::testImageScene()
{
    CxeScene result;
    CxeError::Id error;

    // do initialization
    mSettingsModel->cameraModeChanged(Cxe::ImageMode);

    // case 1: check the default image scene sharpness value
    result = mSettingsModel->currentImageScene();
    QVERIFY(result[CxeSettingIds::SCENE_ID] == Cxe::IMAGE_SCENE_AUTO);
    QVERIFY(result[CxeSettingIds::SHARPNESS] == Cxe::SharpnessNormal);

    // case 2: trying to set a new image scene
    QString sceneId = Cxe::IMAGE_SCENE_PORTRAIT;
    error = mSettingsModel->setImageScene(sceneId);
    QVERIFY(error == CxeError::None);

    // case 3: testing if NIGHT image scene is set correctly. checking its sharpness value
    result = mSettingsModel->currentImageScene();
    QVERIFY(result[CxeSettingIds::SCENE_ID] == sceneId);
    QVERIFY(result[CxeSettingIds::SHARPNESS] == Cxe::SharpnessSoft);

    // case 3: trying to set a image scene with invalid value
    error = mSettingsModel->setImageScene(INVALID_KEY);
    QVERIFY(error == CxeError::NotFound);

    // case 4: try with an empty string
    error = mSettingsModel->setImageScene(EMPTY_KEY);
    QVERIFY(error == CxeError::NotFound);

}


// test case to test VideoScene
// 1. default values of video scene
// 2. try set new video scene and checking if its succesful
// 3. covers also cases where we test with invalid cases
void UnitTestCxeSettingsModelImp::testVideoScene()
{
    CxeScene result;
    CxeError::Id error;

    // do initialization
    mSettingsModel->cameraModeChanged(Cxe::VideoMode);

    // case 1: check the default image scene
    result = mSettingsModel->currentVideoScene();
    QVERIFY(result[CxeSettingIds::SCENE_ID] == Cxe::VIDEO_SCENE_AUTO);
    QVERIFY(result[CxeSettingIds::FRAME_RATE] == 0);

    // case 2: trying to set a new image scene
    QString sceneId = Cxe::VIDEO_SCENE_LOWLIGHT;
    error = mSettingsModel->setVideoScene(sceneId);
    QVERIFY(error == CxeError::None);

    // case 3: testing if NIGHT image scene is set correctly
    result = mSettingsModel->currentVideoScene();
    QVERIFY(result[CxeSettingIds::SCENE_ID] == sceneId);
    QVERIFY(result[CxeSettingIds::FRAME_RATE] == 15);

    // case 3: trying to set a image scene with invalid value
    error = mSettingsModel->setVideoScene(INVALID_KEY);
    QVERIFY(error == CxeError::NotFound);

    // case 4: try with an empty string
    error = mSettingsModel->setVideoScene(EMPTY_KEY);
    QVERIFY(error == CxeError::NotFound);

}


// main() function non-GUI testing
QTEST_APPLESS_MAIN(UnitTestCxeSettingsModelImp);
