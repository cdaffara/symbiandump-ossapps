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
#include <QDebug>
#include <QSignalSpy>
#include <QMetaType>

#include "cxutils.h"
#include "cxetestutils.h"
#include "cxefakesettingsstore.h"
#include "unittest_cxesettingsimp.h"
#include "cxecenrepkeys.h"

const char* FAIL_TEST_SETTING = "Fail test";
const char* EMPTY_STRING = "";
//Default EV compensation value for auto still and auto video scene mode
const int SCENE_AUTO_EV_VALUE = 0;
const int SIGNAL_TIMEOUT = 3000; //milliseconds

UnitTestCxeSettingsImp::UnitTestCxeSettingsImp()
    : mSettingsStore(NULL),
      mSettingsImp(NULL)
{
    qRegisterMetaType<CxeScene>("CxeScene");
    qRegisterMetaType<CxeError::Id>("CxeError::Id");
    qRegisterMetaType<Cxe::CameraMode>("Cxe::CameraMode");
}

UnitTestCxeSettingsImp::~UnitTestCxeSettingsImp()
{
}

void UnitTestCxeSettingsImp::init()
{
    mSettingsStore = new CxeFakeSettingsStore();
    mSettingsImp = new CxeSettingsImp(mSettingsStore);
}

void UnitTestCxeSettingsImp::cleanup()
{
    delete mSettingsImp;
    mSettingsImp = NULL;
}

/*
 * Testing loadSettings() with Cxe::ImageMode argument
 */
void UnitTestCxeSettingsImp::testLoadImageSettings()
{
    CX_DEBUG_ENTER_FUNCTION();
    QSignalSpy spy(mSettingsImp, SIGNAL(settingValueChanged(QString,QVariant)));
    mSettingsImp->loadSettings(Cxe::ImageMode);

    //signal should be emitted when loading settings
    QVERIFY(spy.count() > 0);

    //now let's take a look inside a signal content
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString().toAscii() == CxeSettingIds::IMAGE_SCENE);
    CX_DEBUG_EXIT_FUNCTION();
}

/*
 * Testing loadSettings() with Cxe::VideoMode argument
 */
void UnitTestCxeSettingsImp::testLoadVideoSettings()
{
    CX_DEBUG_ENTER_FUNCTION();
    QSignalSpy spy(mSettingsImp, SIGNAL(settingValueChanged(QString,QVariant)));
    mSettingsImp->loadSettings(Cxe::VideoMode);

    //signal should be emitted when loading settings
    QVERIFY(spy.count() > 0);

    //now let's take a look inside a signal content
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString().toAscii() == CxeSettingIds::VIDEO_SCENE);

    CX_DEBUG_EXIT_FUNCTION();
}

/*
 * Advanced test case for loadSettings()
 */
void UnitTestCxeSettingsImp::testLoadImageAndVideoSettings()
{
    CX_DEBUG_ENTER_FUNCTION();
    QSignalSpy spy(mSettingsImp, SIGNAL(settingValueChanged(QString,QVariant)));
    mSettingsImp->loadSettings(Cxe::VideoMode);

    //signal should be emitted when loading settings
    QVERIFY(spy.count() > 0);

    //now let's take a look inside a signal content
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString().toAscii() == CxeSettingIds::VIDEO_SCENE);
    spy.clear();

    mSettingsImp->loadSettings(Cxe::ImageMode);

    //signal should be emitted when loading settings
    QVERIFY(spy.count() > 0);

    //now let's take a look inside a signal content
    arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toString().toAscii() == CxeSettingIds::IMAGE_SCENE);

    CX_DEBUG_EXIT_FUNCTION();
}

/*
 * Testing inline get(const QString &key)
 * defined in cxesettings.h as template
 */
void UnitTestCxeSettingsImp::testGet()
{
    CX_DEBUG_ENTER_FUNCTION();

    // The idea of this test case is to test as much of different type of settings
    // as possible

    // Testing reading brightness setting value
    int brightnessValue;
    QVariant testValue;

    try {
        brightnessValue = mSettingsImp->CxeSettings::get<int>(CxeSettingIds::BRIGHTNESS);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }

    mSettingsStore->get(CxeSettingIds::BRIGHTNESS, testValue);
    QCOMPARE(brightnessValue, testValue.toInt());

    // Testing reading color tone setting value
    Cxe::Colortone colorTone;
    try {
        colorTone = mSettingsImp->CxeSettings::get<Cxe::Colortone>(CxeSettingIds::COLOR_TONE);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }

    QVariant colorTone2;
    mSettingsStore->get(CxeSettingIds::COLOR_TONE, colorTone2);
    QVERIFY(colorTone2 == colorTone);

    // Testing reading contrast setting value
    int contrastValue;
    try {
        contrastValue = mSettingsImp->CxeSettings::get<int>(CxeSettingIds::CONTRAST);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }

    mSettingsStore->get(CxeSettingIds::CONTRAST, testValue);
    QCOMPARE(testValue.toInt(), contrastValue);

    // Testing reading file name folder suffix setting value
    QString fnameValue;
    try {
        fnameValue = mSettingsImp->CxeSettings::get<QString>(CxeSettingIds::FNAME_FOLDER_SUFFIX);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }

    mSettingsStore->get(CxeSettingIds::FNAME_FOLDER_SUFFIX, testValue);
    QCOMPARE(testValue.toString(), fnameValue);

    // Testing reading scene data
    QVariantMap sceneData;

    // First set the still camera scene to auto
    mSettingsStore->set(CxeSettingIds::IMAGE_SCENE, Cxe::IMAGE_SCENE_AUTO);

    try {
        sceneData = mSettingsImp->CxeSettings::get<QVariantMap>(CxeSettingIds::IMAGE_SCENE_DATA);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }

    QCOMPARE(sceneData[CxeSettingIds::EV_COMPENSATION_VALUE].toInt(), SCENE_AUTO_EV_VALUE);

    // Now let's try same with video scene
    sceneData.clear();
    mSettingsStore->set(CxeSettingIds::VIDEO_SCENE, Cxe::VIDEO_SCENE_AUTO);
    try {
        sceneData = mSettingsImp->CxeSettings::get<QVariantMap>(CxeSettingIds::VIDEO_SCENE_DATA);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }

    QCOMPARE(sceneData[CxeSettingIds::EV_COMPENSATION_VALUE].toInt(), SCENE_AUTO_EV_VALUE);

    CX_DEBUG_EXIT_FUNCTION();
}

/*
 * Testing get(long int uid,
 *             unsigned long int key,
 *             Cxe::SettingKeyType type,
 *             QVariant &value)
 */
void UnitTestCxeSettingsImp::testGet2()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVariant value;

    mSettingsImp->get(CxSettingsCrUid, FileNameSuffixCr, Cxe::Repository, value);
    // 42 is just a test value from cxefakesettingsstore
    // we just check that it is correctly returned
    QCOMPARE(value.toInt(), 42);

    mSettingsImp->get(0, 0, Cxe::PublishAndSubscribe, value);
    // 42 is just a test value from cxefakesettingsstore
    // we just check that it is correctly returned
    QCOMPARE(value.toInt(), 42);
    CX_DEBUG_EXIT_FUNCTION();
}

/*
 * Testing get(const QString &key, const T &defaultValue)
 * defined in cxesettings.h as template
 */
void UnitTestCxeSettingsImp::testGet3()
{
    CX_DEBUG_ENTER_FUNCTION();
    Cxe::Colortone defaultTone = Cxe::ColortoneNormal;
    mSettingsStore->set(CxeSettingIds::COLOR_TONE, Cxe::ColortoneVivid);
    Cxe::Colortone colorTone = mSettingsImp->CxeSettings::get<Cxe::Colortone>(
            CxeSettingIds::COLOR_TONE, defaultTone);
    QVERIFY(colorTone == Cxe::ColortoneVivid);

    // Now when using this version of the overloaded get<int>()
    // with invalid key parameter, it should internally
    // catch the exception and not modify the default value
    int defaultValue = 0;
    int value = mSettingsImp->CxeSettings::get<int>(
            FAIL_TEST_SETTING, defaultValue);
    QVERIFY(value == defaultValue);
    CX_DEBUG_EXIT_FUNCTION();
}

/*
 * Testing inputting some garbage into CxeSettingsImp::get()
 */
void UnitTestCxeSettingsImp::testGetGarbage()
{
    CX_DEBUG_ENTER_FUNCTION();
    int intTestValue = 0;
    CxeError::Id errorId;

    try {
        mSettingsImp->CxeSettings::get<int>(EMPTY_STRING, intTestValue);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        errorId = CxeError::Id(exception.error());
        QCOMPARE(errorId, CxeError::NotFound);
    }

    try {
        mSettingsImp->CxeSettings::get<int>(FAIL_TEST_SETTING, intTestValue);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        errorId = CxeError::Id(exception.error());
        QCOMPARE(errorId, CxeError::NotFound);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*
 * Testing inline void set(const QString &key, const T &value)
 * defined in cxesettings.h
 */
void UnitTestCxeSettingsImp::testSet()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Point of this test case is to try all versions of the set()
    // method defined in cxesettings.inl

    // Testing set<int>() version
    QVariant value;
    try {
        mSettingsImp->set(CxeSettingIds::COLOR_TONE, Cxe::ColortoneBlackAndWhite);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }
    mSettingsStore->get(CxeSettingIds::COLOR_TONE, value);
    QCOMPARE(value.toInt(), (int)Cxe::ColortoneBlackAndWhite);

    // Testing set<QString>() version
    QString stringValue = "some text";
    try {
        mSettingsImp->set(CxeSettingIds::FNAME_FOLDER_SUFFIX, stringValue);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }
    mSettingsStore->get(CxeSettingIds::FNAME_FOLDER_SUFFIX, value);
    QCOMPARE(value.toString(), stringValue);

    // Testing set<bool>() version
    bool boolValue = true;
    try {
        mSettingsImp->set(CxeSettingIds::STILL_SHOWCAPTURED, boolValue);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }
    mSettingsStore->get(CxeSettingIds::STILL_SHOWCAPTURED, value);
    QCOMPARE(value.toBool(), boolValue);

    // Testing set<qreal>() version
    qreal qrealValue = -1.25;
    try {
        mSettingsImp->set(CxeSettingIds::EV_COMPENSATION_VALUE, qrealValue);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append(exception.error());
        QFAIL(message.toAscii());
    }
    mSettingsStore->get(CxeSettingIds::EV_COMPENSATION_VALUE, value);
    QCOMPARE(value.toReal(), qrealValue);

    // Testing set<QVariantMap>() version
    CxeScene testSceneData;

    testSceneData.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_AUTO);
    testSceneData.insert(CxeSettingIds::WHITE_BALANCE, Cxe::WhitebalanceAutomatic);
    testSceneData.insert(CxeSettingIds::EXPOSURE_MODE, Cxe::ExposureAuto);
    testSceneData.insert(CxeSettingIds::COLOR_TONE, Cxe::ColortoneNormal);
    testSceneData.insert(CxeSettingIds::CONTRAST, 5);
    testSceneData.insert(CxeSettingIds::SHARPNESS, Cxe::SharpnessNormal);
    testSceneData.insert(CxeSettingIds::LIGHT_SENSITIVITY, Cxe::LightSensitivityAutomatic);
    testSceneData.insert(CxeSettingIds::EV_COMPENSATION_VALUE, -2);
    testSceneData.insert(CxeSettingIds::BRIGHTNESS, 8);
    testSceneData.insert(CxeSettingIds::FLASH_MODE, Cxe::FlashAuto);
    testSceneData.insert(CxeSettingIds::FACE_TRACKING, 1);

    // First set the still camera scene to auto
    mSettingsStore->set(CxeSettingIds::IMAGE_SCENE, Cxe::IMAGE_SCENE_AUTO);

    // Now setting the value
    try {
        //This should throw an exception with CxeError::NotSupported
        mSettingsImp->set(CxeSettingIds::IMAGE_SCENE_DATA, testSceneData);
    }
    catch (CxeException exception) {
        QString message = "Exception thrown, error id = ";
        message.append((int)exception.error());
        QCOMPARE(exception.error(), (int)CxeError::NotSupported);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*
 * This will test calling void method of reset()
 */
void UnitTestCxeSettingsImp::testReset()
{
    CX_DEBUG_ENTER_FUNCTION();
    mSettingsImp->reset();
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeSettingsImp::testGetVariationSetting()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVariant value;
    CxeError::Id error = mSettingsImp->getVariationValue(CxeVariationKeys::STILL_MAX_ZOOM_LIMITS, value);
    QCOMPARE(error, CxeError::None);

    error = mSettingsImp->getVariationValue(FAIL_TEST_SETTING, value);
    QCOMPARE(error, CxeError::NotFound);
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeSettingsImp::testListenForSetting()
{
    CX_DEBUG_ENTER_FUNCTION();

    // This test case will initialize listening for certain setting changes
    // and will verify that listener has succesfully deployed the signal

    // First let's initialize a signal spy
    QSignalSpy spy(mSettingsImp, SIGNAL(settingValueChanged(QString, QVariant)));

    // Initializing the original value to make sure it changes later on
    mSettingsStore->set(CxeSettingIds::CAMERA_MODE, Cxe::ImageMode);

    // Now start listening and verify that starting was succesfull
    bool result = mSettingsImp->listenForSetting(CxeSettingIds::CAMERA_MODE, this, SLOT(testSlot()));
    QVERIFY(result);

    // Now change the setting and wait for the signal
    //mSettingsStore->set(CxeSettingIds::CAMERA_MODE, Cxe::VideoMode);
    mSettingsImp->set(CxeSettingIds::CAMERA_MODE, Cxe::VideoMode);


    // Check that signal was emitted
    QVERIFY(CxeTestUtils::waitForSignal(spy, SIGNAL_TIMEOUT));

    // One more check with invalid input parameter
    result = mSettingsImp->listenForSetting(FAIL_TEST_SETTING, this, SLOT(testSlot()));
    // This should still return true, since the function adds listener to any
    // setting, regardless if the setting key is valid or not. This is ok.
    QVERIFY(result);

    // Now disconnecting the listener and checking if the signal is still emitted
    disconnect(mSettingsImp, SIGNAL(settingValueChanged(QString,QVariant)), this, SLOT(testSlot()));
    delete mSettingsImp;
    mSettingsImp = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}

/*
 * Just a dummy slot
 */
void UnitTestCxeSettingsImp::testSlot()
{
    CX_DEBUG_IN_FUNCTION();
}

// main() function non-GUI testing
QTEST_MAIN(UnitTestCxeSettingsImp)

