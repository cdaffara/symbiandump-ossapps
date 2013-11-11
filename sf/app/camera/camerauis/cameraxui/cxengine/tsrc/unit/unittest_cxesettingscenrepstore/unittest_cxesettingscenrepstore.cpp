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

#include <QTest>

#include "cxeerror.h"
#include "cxutils.h"
#include "cxesettingscenrepstore.h"
#include "unittest_cxesettingscenrepstore.h"
#include "cxenamespace.h"
#include "cxecenrepkeys.h"

// constants
static const char* INVALID_KEY = "invalid key";
static const char* EMPTY_KEY = "";

/**
* UnitTestCxeSettingsCenrepStore::UnitTestCxeSettingsCenrepStore()
*/
UnitTestCxeSettingsCenrepStore::UnitTestCxeSettingsCenrepStore()
: mSettingsCenrepStore(NULL)
{
}

/**
* UnitTestCxeSettingsCenrepStore::~UnitTestCxeSettingsCenrepStore()
*/
UnitTestCxeSettingsCenrepStore::~UnitTestCxeSettingsCenrepStore()
{
    cleanup();
}

/**
* UnitTestCxeSettingsCenrepStore::init()
*/
void UnitTestCxeSettingsCenrepStore::init()
{
    mSettingsCenrepStore = new CxeSettingsCenRepStore();
}

/**
* UnitTestCxeSettingsCenrepStore::cleanup()
*/
void UnitTestCxeSettingsCenrepStore::cleanup()
{
    delete mSettingsCenrepStore;
    mSettingsCenrepStore = NULL;
}


/**
* UnitTestCxeSettingsCenrepStore::testSettingValues()
*/
void UnitTestCxeSettingsCenrepStore::testSettingValues()
{
    QString settingkey = CxeSettingIds::FNAME_MONTH_FOLDER;
    QString scenekey = CxeSettingIds::COLOR_TONE;
    QVariant result;
    QVariant value = QVariant(2);
    CxeError::Id error;
    
    // case 0: check default value of a runtime key, real setting key and scene key
    error = mSettingsCenrepStore->get(settingkey, result);
    QVERIFY(error == CxeError::None);
    QCOMPARE(result.toInt(), 1);
    
    // scene key shouldnt be read from cenrepstore 
    error = mSettingsCenrepStore->get(scenekey, result);
    QVERIFY(error == CxeError::NotFound);

    
    // case 1: try setting a value to a run-time key and real setting key
    error = mSettingsCenrepStore->set(settingkey, value);
    QVERIFY(error == CxeError::None);
    
    error = mSettingsCenrepStore->set(scenekey, value);
    QVERIFY(error == CxeError::NotFound);
    
    // case 2: try setting with invalid key
    error = mSettingsCenrepStore->set(INVALID_KEY, value);
    QVERIFY(error == CxeError::NotFound);
    
    error = mSettingsCenrepStore->set(EMPTY_KEY, value);
    QVERIFY(error == CxeError::NotFound);
    
    // case 3: try reading a value with invalid key
    error = mSettingsCenrepStore->get(INVALID_KEY, value);
    QVERIFY(error == CxeError::NotFound);
    
    error = mSettingsCenrepStore->get(EMPTY_KEY, value);
    QVERIFY(error == CxeError::NotFound);

    // case 4: check if the new value is properly set to a real setting key
    error = mSettingsCenrepStore->get(settingkey, result);
    QVERIFY(error == CxeError::None);
    QCOMPARE(result.toInt(), 2);
}

/**
* UnitTestCxeSettingsCenrepStore::testloadSettings
*/
void UnitTestCxeSettingsCenrepStore::testloadSettings()
{
    QList<QString> keys;
    keys.append(CxeVariationKeys::STILL_MAX_ZOOM_LIMITS);
    keys.append(CxeVariationKeys::VIDEO_MAX_ZOOM_LIMITS);
    keys.append(CxeVariationKeys::FREE_MEMORY_LEVELS);
    QHash<QString, QVariantList> runtimesettings =  mSettingsCenrepStore->loadVariationSettings(keys);
    
    foreach(QString runtimekey, keys) {
        QVariant result;
        CxeError::Id error = mSettingsCenrepStore->get(runtimekey, result);
        QVERIFY(error == CxeError::None);
        QCOMPARE(result.toInt(), 1);
    }
    
}


/*
 * UnitTestCxeSettingsCenrepStore::testReset()
 */
void UnitTestCxeSettingsCenrepStore::testReset()
{
    // Case 0: Set a value to a known setting
    QString settingkey = CxeSettingIds::FNAME_MONTH_FOLDER;
    QVariant value = QVariant(2);
    QVariant result;
    CxeError::Id error;
    
    error = mSettingsCenrepStore->set(settingkey, value);
    QVERIFY(error == CxeError::None);
    
    // now test if the value is set right
    error = mSettingsCenrepStore->get(settingkey, result);
    QVERIFY(error == CxeError::None);
    QCOMPARE(result.toInt(), 2);
    
    // Case 1: reset the cenrep
    mSettingsCenrepStore->reset();
    
    //! TODO: handle the reset case when the real implementation of reset changes.
    // for now reset doesnt reset the cenrep store as there is no real way of doing it
    // from xqsettingsmanager.
}

// main() function
QTEST_APPLESS_MAIN(UnitTestCxeSettingsCenrepStore);

// end of file
