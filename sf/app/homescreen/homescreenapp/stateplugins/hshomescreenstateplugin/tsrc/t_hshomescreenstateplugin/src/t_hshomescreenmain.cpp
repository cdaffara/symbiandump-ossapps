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
* Description:  Main test class for hsHomeScreenStatePlugin library.
*
*/

#include <QMetaType>
#include <QAction>
#include <QState>

#include <HbMainWindow>
#include <HbInstance>
#include <HbView>

#include "t_hshomescreenstateplugin.h"
#include "hshomescreenstateplugin.h"
#include "testwidget.h"
#include "hsdatabase.h"
#include "hsscene.h"
#include "hsgui.h"
#include "hswidgethost.h"
#include "hsdomainmodeldatastructures.h"
#include "hsconfiguration.h"

#include "hswidgetpositioningonorientationchange.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hswidgetpositioningonwidgetmove.h"

#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#endif


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HomeScreenStatePluginTest::initTestCase()
{
    mMainWindow = 0;

#ifdef Q_OS_SYMBIAN
    mImageProcessingDelay = 5000;
#else
    mImageProcessingDelay = 2000;
#endif

    HsWidgetPositioningOnOrientationChange::setInstance(
        new HsAdvancedWidgetPositioningOnOrientationChange);

    HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsAnchorPointInBottomRight);

    HsWidgetPositioningOnWidgetMove::setInstance(new HsSnapToLines);

    HsDatabase::setInstance(new HsDatabase);

    HsConfiguration::setInstance(new HsConfiguration);
   
}

void HomeScreenStatePluginTest::cleanupTestCase()
{
    
}

void HomeScreenStatePluginTest::cleanup()
{
    //Scene already deleted as child of mainwindow
    HsScene *scene = HsScene::takeInstance();
    delete scene;
}

void HomeScreenStatePluginTest::init()
{

}

void HomeScreenStatePluginTest::createSceneAndWindow(int pageCount, int defaultPageIndex, int maximumPageCount)
{
    HSCONFIGURATION_SET(setMaximumPageCount, maximumPageCount);

    HbInstance::instance();
    mMainWindow = new HbMainWindow;
    mMainWindow->move(0, 0);
    mMainWindow->show();
    
    QEventLoop waitUntilMainWindowCreated;
    QTimer eventLoopTimer;
    eventLoopTimer.setInterval(15000);
    eventLoopTimer.setSingleShot(true);
    connect(mMainWindow, SIGNAL(viewReady()),
            &waitUntilMainWindowCreated, SLOT(quit()));
    connect(&eventLoopTimer, SIGNAL(timeout()), 
            &waitUntilMainWindowCreated, SLOT(quit()));
    eventLoopTimer.start();
    waitUntilMainWindowCreated.exec();
    eventLoopTimer.stop();

    HsScene *scene = HsScene::instance();
    scene->setProperty("pageCount", pageCount);
    scene->setProperty("defaultPageIndex", defaultPageIndex);
    scene->setProperty("maximumPageCount", maximumPageCount);
    scene->load();
}

void HomeScreenStatePluginTest::deleteSceneAndWindow()
{
    delete mMainWindow;
    mMainWindow = 0;
    delete HsGui::takeInstance();
}

#ifdef Q_OS_SYMBIAN
//QTEST_MAIN corrected since crashes if TRAP not in correct place.
//Will be corrected in later (estimate 4.6.0) Qt release for Symbian.
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    int error = 0;
    TRAPD(err, \
    QTEST_DISABLE_KEYPAD_NAVIGATION \
    HomeScreenStatePluginTest tc; \
    error = QTest::qExec(&tc, argc, argv););
    Q_UNUSED(err);
    return error;
}
#else //Q_OS_SYMBIAN
QTEST_MAIN(HomeScreenStatePluginTest)
#endif //Q_OS_SYMBIAN
