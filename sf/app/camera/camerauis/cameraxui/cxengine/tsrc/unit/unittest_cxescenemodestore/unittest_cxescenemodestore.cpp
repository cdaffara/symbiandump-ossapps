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
 * Description: Unit tests for CxeSceneModeStore class
 *
 */

#include <QtCore/QString>
#include <QtTest>

#include "cxenamespace.h"
#include "cxeerror.h"
#include "cxeexception.h"
#include "unittest_cxescenemodestore.h"

const char* FAIL_TEST_SETTING = "Fail test";

UnitTestCxeSceneModeStore::UnitTestCxeSceneModeStore() :
        mSceneModeStore(NULL)
{
}

UnitTestCxeSceneModeStore::~UnitTestCxeSceneModeStore()
{
}

void UnitTestCxeSceneModeStore::init()
{
    mSceneModeStore = new CxeSceneModeStore();
}

void UnitTestCxeSceneModeStore::cleanup()
{
    delete mSceneModeStore;
    mSceneModeStore = NULL;
}

void UnitTestCxeSceneModeStore::testSceneSettingValue()
{
    QVariant value;
    CxeError::Id err = mSceneModeStore->sceneSettingValue(Cxe::ImageMode, CxeSettingIds::COLOR_TONE, value);
    QCOMPARE(err, CxeError::None);

    err = mSceneModeStore->sceneSettingValue(Cxe::VideoMode, CxeSettingIds::COLOR_TONE, value);
    QCOMPARE(err, CxeError::None);
}

void UnitTestCxeSceneModeStore::testSceneSettingValueNotFound()
{
    QVariant value;
    CxeError::Id err = mSceneModeStore->sceneSettingValue(Cxe::ImageMode, FAIL_TEST_SETTING, value);
    QCOMPARE(err, CxeError::NotFound);
}

void UnitTestCxeSceneModeStore::testCurrentScene()
{
    CxeScene scene;
    try {
        scene = mSceneModeStore->currentScene(Cxe::ImageMode);
    }
    catch (CxeException exception){
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }
    QVERIFY(true);
}

void UnitTestCxeSceneModeStore::testCurrentScene2()
{
    CxeScene scene;
    try {
        scene = mSceneModeStore->currentScene(Cxe::VideoMode);
    }
    catch (CxeException exception){
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }
    QVERIFY(true);
}

void UnitTestCxeSceneModeStore::testCurrentSceneId()
{
    QString sceneId = mSceneModeStore->currentSceneId(Cxe::ImageMode);
    QVERIFY(sceneId != "");
    sceneId = mSceneModeStore->currentSceneId(Cxe::VideoMode);
    QVERIFY(sceneId != "");
}

void UnitTestCxeSceneModeStore::testGetScene()
{
    CxeScene scene;
    try {
        scene = mSceneModeStore->getScene(Cxe::ImageMode, Cxe::IMAGE_SCENE_AUTO);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }
    QVERIFY(true);
}

void UnitTestCxeSceneModeStore::testSetCurrentScene()
{
    try {
        mSceneModeStore->setCurrentScene(Cxe::ImageMode, Cxe::IMAGE_SCENE_NIGHT);
    }
    catch (CxeException exception){
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }
    QString scene = mSceneModeStore->currentSceneId(Cxe::ImageMode);
    QVERIFY(scene == Cxe::IMAGE_SCENE_NIGHT);

    mSceneModeStore->setCurrentScene(Cxe::VideoMode, Cxe::VIDEO_SCENE_LOWLIGHT);
    scene = mSceneModeStore->currentSceneId(Cxe::VideoMode);
    QVERIFY(scene == Cxe::VIDEO_SCENE_LOWLIGHT);
}

void UnitTestCxeSceneModeStore::testSetSceneSettingValue()
{
    CxeError::Id err = mSceneModeStore->setSceneSettingValue(Cxe::ImageMode, CxeSettingIds::SHARPNESS,
                                                             Cxe::SharpnessHard);
    QCOMPARE(err, CxeError::None);

    err = mSceneModeStore->setSceneSettingValue(Cxe::ImageMode, FAIL_TEST_SETTING,
                                                             Cxe::SharpnessHard);
    QCOMPARE(err, CxeError::NotFound);

}

QTEST_MAIN(UnitTestCxeSceneModeStore);

