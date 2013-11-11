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
* Description:   TestVideoSettingsPlugin class implementation
* 
*/

// INCLUDE FILES
#include <QtTest/QtTest>
#include <qdebug.h>
#include <hbapplication.h>
#include <cpitemdatahelper.h>

#include "videosettingsgroup.h"
#include "testvideosettingsplugin.h"

// trick to get access to protected/private members.
#define private public
#include "videosettingsplugin.h"
#undef private

// ---------------------------------------------------------------------------
// initTestCase
// ---------------------------------------------------------------------------
//
void TestVideoSettingsPlugin::initTestCase()
{
    mItemHelper = new CpItemDataHelper;
}

// ---------------------------------------------------------------------------
// cleanupTestCase
// ---------------------------------------------------------------------------
//
void TestVideoSettingsPlugin::cleanupTestCase()
{
    delete mItemHelper;
    mItemHelper = 0;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoSettingsPlugin::init()
{
    mPlugin = new VideoSettingsPlugin();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoSettingsPlugin::cleanup()
{
    delete mPlugin;
    mPlugin = 0;
}

// ---------------------------------------------------------------------------
// testCreateSettingFormItemData
// ---------------------------------------------------------------------------
//
void TestVideoSettingsPlugin::testCreateSettingFormItemData()
{
    QList<CpSettingFormItemData*> list = mPlugin->createSettingFormItemData(*mItemHelper);
    QCOMPARE( list.count(), 1 );
    VideoSettingsGroup* group = static_cast<VideoSettingsGroup*>(list.at(0));
    QVERIFY( group );
    QVERIFY( &(group->mItemDataHelper) == mItemHelper ); 
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
/*    HbApplication::setKeypadNavigationEnabled(false);
    HbMainWindow mainWindow;
    mainWindow.show();*/
    
    TestVideoSettingsPlugin tc;
    
    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideosettingsplugin.txt";

    return QTest::qExec(&tc, 3, pass);
}

// end of file
