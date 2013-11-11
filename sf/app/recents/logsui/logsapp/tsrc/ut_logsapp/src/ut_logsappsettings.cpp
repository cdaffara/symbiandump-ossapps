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

//USER
#include "ut_logsappsettings.h"
#include "logsappsettings.h"
#include <xqsettingsmanager.h>

//SYSTEM
#include <QtTest/QtTest>

void UT_LogsAppSettings::initTestCase()
{

}

void UT_LogsAppSettings::cleanupTestCase()
{

}


void UT_LogsAppSettings::init()
{
    int argc = 0;
    char* argv = 0;
    mSettings = new LogsAppSettings(argc, &argv);
}

void UT_LogsAppSettings::cleanup()
{
    delete mSettings;
    mSettings = 0;
}

void UT_LogsAppSettings::testLogsFeaturePreloadingEnabled()
{
    QVERIFY( !mSettings->logsFeaturePreloadingEnabled() );
    delete mSettings;
    mSettings = 0;
    int argc = 2;
    char* argv[] = {"-logsbootup", "B"};
    mSettings = new LogsAppSettings(argc, argv);
    QVERIFY( mSettings->logsFeaturePreloadingEnabled() );
}

void UT_LogsAppSettings::testLogsFeatureFakeExitEnabled()
{
    QVERIFY( mSettings->mFeatureFakeExitEnabledVal == -1 );
    
    // No such key
    XQSettingsManager::mFailed = true;
    QVERIFY( !mSettings->logsFeatureFakeExitEnabled() );
    
    // Verify that key is read only once
    XQSettingsManager::mFailed = false;
    XQSettingsManager::mCurrentVal = 1;
    QVERIFY( !mSettings->logsFeatureFakeExitEnabled() );
    
    // First read when key is there and value means enabled
    mSettings->mFeatureFakeExitEnabledVal = -1;
    QVERIFY( mSettings->logsFeatureFakeExitEnabled() );
    
    // Value means disabled
    XQSettingsManager::mCurrentVal = 0;
    mSettings->mFeatureFakeExitEnabledVal = -1;
    QVERIFY( !mSettings->logsFeatureFakeExitEnabled() );
}


