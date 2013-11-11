/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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


#include "ut_cpkeyscreen.h"
#include "cpkeyscreenmodel.h"
#include "cpkeyscreenconstants.h"
#include "cpkeyscreenplugin.h"
#include "cpkeyscreenview.h"
#include <cpsettingformentryitemdata.h>
#include <cpitemdatahelper.h>
#include <QtTest/QtTest>

/*!
     Initial test case's resource
 */
void TestCpKeyScreen::initTestCase()
{
    mModel = new CpKeyScreenModel();
    QVERIFY(mModel != 0);
}

/*!
     Cleanup test case's resource
 */
void TestCpKeyScreen::cleanupTestCase()
{
    delete mModel;
}

/*!
     1. FunctionName: CpKeyScreenModel() \n
     2. Description: Test constructor of CpKeyScreenModel \n
 */
void TestCpKeyScreen::TestConstructorAndDestructor()
{
    CpKeyScreenModel *model = new CpKeyScreenModel();
    QVERIFY(model != 0);
    delete model;
    model = 0;
}
/*!
     1. FucntionName: bool isKeyguardSupported() \n
     2. Description: Test key guard supported function \n
 */
void TestCpKeyScreen::TestKeyguardSupported()
{
    // NULL testing, result is decided by feature flag
    mModel->isKeyguardSupported();
}

/*!
     1. FucntionName: bool isScreensaverSupported() \n
     2. Description: Test screen saver supported function \n
 */
void TestCpKeyScreen::TestScreensaverSupported()
{
    // NULL testing, result is decided by feature flag
    mModel->isScreensaverSupported();
}

/*!
     1. FucntionName: bool isBrightnessSupported() \n
     2. Description: Test brightness supported function \n
 */
void TestCpKeyScreen::TestBrightnessSupported()
{
    // NULL testing, result is decided by feature flag
    mModel->isBrightnessSupported();
}

/*!
     1. FucntionName: bool isRotateSupported() \n
     2. Description: Test rotate supported function \n
 */
void TestCpKeyScreen::TestRotateSupported()
{
    // NULL testing, result is decided by feature flag
    mModel->isRotateSupported();
}

/*!
     1. FunctionName: bool setKeyguard(int value) \n
                      int keyguard() \n
     2. Description: test KeyGuard settings set and get \n
     3. Input: 1. valid value of set function, and verify by get function \n 
     4. ExpectedOutput: 1. same between set and get function
 */
void TestCpKeyScreen::TestSetAndGetKeyGuard()
{
    mModel->setKeyguard(KCpKeyscreenLock15s);
    QVERIFY(mModel->keyguard() == KCpKeyscreenLock15s);
        
}

/*!
     1. FunctionName: bool setKeyguard(int value) \n
                      int keyguard() \n
     2. Description: test KeyGuard settings set and get \n
     3. Input: 1. invalid value of set function, for example "-1" and verify by get function \n
               2. invalid value of set function, for example "100" and verify by get function \n
     4. ExpectedOutput: 1. same with the previous one.
 */
void TestCpKeyScreen::TestSetAndGetKeyGuardWithInvalidInput()
{
    int oldValue = mModel->keyguard();
    
    mModel->setKeyguard(-1);
    int valueAfterSet = mModel->keyguard();
    
    QVERIFY(oldValue == valueAfterSet);
    
    mModel->setKeyguard(100);
    valueAfterSet = mModel->keyguard();
    
    QVERIFY(oldValue == valueAfterSet);
}

/*!
     1. FunctionName: bool setRotate(bool value) \n
                      bool rotate() \n
     2. Description: test rotate settings set and get \n
     3. Input: 1. "true" value of set function and verify by get function \n 
     4. ExpectedOutput: 1. return true.
 */
void TestCpKeyScreen::TestSetAndGetRotateWithTrue()
{
    mModel->setRotate(true);
    QVERIFY(mModel->rotate() == true);
}

/*!
     1. FunctionName: bool setRotate(bool value) \n
                      bool rotate() \n
     2. Description: test rotate settings set and get \n
     3. Input: 1. "false" value of set function and verify by get function \n 
     4. ExpectedOutput: 1. return false.
 */
void TestCpKeyScreen::TestSetAndGetRotateWithFalse()
{
    mModel->setRotate(false);
    QVERIFY(mModel->rotate() == false);
}

/*!
     1. FunctionName: bool setBrightness(int value) \n
                      int brightness() \n
     2. Description: test brightness settings set and get \n
     3. Input: 1. valid value of set function, and verify by get function \n 
     4. ExpectedOutput: 1. same between set and get function
 */
void TestCpKeyScreen::TestSetAndGetBrightness()
{
    mModel->setBrightness(3);
    QVERIFY(mModel->brightness() == 3);
}

/*!
     1. FunctionName: bool setBrightness(int value) \n
                      int brightness() \n
     2. Description: test KeyGuard settings set and get \n
     3. Input: 1. invalid value of set function, for example "-1" and verify by get function \n
               2. invalid value of set function, for example "100" and verify by get function \n
     4. ExpectedOutput: 1. same with the previous one.
 */
void TestCpKeyScreen::TestSetAndGetBrightnessWithInvalidInput()
{
    int oldValue = mModel->brightness();
    
    mModel->setBrightness(-1);
    int valueAfterSet = mModel->brightness();
    
    QVERIFY(oldValue == valueAfterSet);
    
    mModel->setBrightness(100);
    valueAfterSet = mModel->brightness();
        
    QVERIFY(oldValue == valueAfterSet);    
}

/*!
     1. FunctionName: bool setScreensaver(bool value) \n
                      bool screensaver() \n
     2. Description: test screen saver settings set and get \n
     3. Input: 1. "true" value of set function and verify by get function \n 
     4. ExpectedOutput: 1. return true.
 */
void TestCpKeyScreen::TestSetAndGetScreenSaverWithTrue()
{
    mModel->setScreensaver(true);
    QVERIFY(mModel->screensaver() == true);
}

/*!
     1. FunctionName: bool setScreensaver(bool value) \n
                      bool screensaver() \n
     2. Description: test screen saver settings set and get \n
     3. Input: 1. "false" value of set function and verify by get function \n 
     4. ExpectedOutput: 1. return false.
 */
void TestCpKeyScreen::TestSetAndGetScreenSaverWithFalse()
{
    mModel->setScreensaver(false);
    QVERIFY(mModel->screensaver() == false);
}
/*!
     1. FunctionName: CpKeyScreenPlugin();
     2. Description: test CpKeyScreenPlugin's constructor and destructor
 */
void TestCpKeyScreen::TestPluginConstructorAndDestructor()
{
    CpKeyScreenPlugin *plugin = new CpKeyScreenPlugin();
    QVERIFY(plugin != 0);
    delete plugin;
    plugin = 0;
}

/*!
     1. FunctionName:     virtual QList<CpSettingFormItemData*> createSettingFormItemData(CpItemDataHelper &itemDataHelper) const;
     2. Description: interface of cpkeyscreen plugin
     3. Input: instance of itemDataHelper
     4. Expected Output: QList<CpSettingFormItemData*>
 */
void TestCpKeyScreen::TestPluginInterface()
{
    CpKeyScreenPlugin *plugin = new CpKeyScreenPlugin();
    QVERIFY(plugin != 0);
    CpItemDataHelper *helper = new CpItemDataHelper();
    QList<CpSettingFormItemData*> itemList = plugin->createSettingFormItemData(*helper);
    QVERIFY(itemList.count() == 1);
    CpSettingFormEntryItemData *entryItem = qobject_cast<CpSettingFormEntryItemData *>(itemList.at(0));
    QVERIFY(entryItem != 0);
    
    delete plugin;
    plugin = 0;
    delete helper;
    helper = 0;
    
    qDeleteAll(itemList.begin(),itemList.end());
    itemList.clear();
}

/*!
     1. FunctionName CpKeyScreenView
     2. Description: ui of key and screen plugin
     3. Input: a null parent and a QObject
     4. Expected Output: a instance of cpkeyscreenview
 */
void TestCpKeyScreen::TestViewConstructorAndDestructor()
{
    CpKeyScreenView *view = new CpKeyScreenView(0);
    QVERIFY(view != 0);
    
    CpKeyScreenView *view1 = new CpKeyScreenView(new HbWidget());
    QVERIFY(view1 != 0);
    
    delete view;
    view = 0;
    delete view1;
    view = 0;
}
QTEST_MAIN(TestCpKeyScreen)
