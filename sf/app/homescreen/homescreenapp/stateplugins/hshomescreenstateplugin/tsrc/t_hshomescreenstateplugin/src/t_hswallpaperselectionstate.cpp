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

#include <QStateMachine>

#include <HbInstance>
#include <HbMainWindow>

#include "t_hshomescreenstateplugin.h"
#include "hswallpaperselectionstate.h"
#include "hsscene.h"
#include "hspage.h"
#include "hsspinnerdialog.h"
#include "hsconfiguration.h"
#include "hsgui.h"

#ifdef Q_OS_SYMBIAN
#include "hsimagehandlingclient.h"
#else
#include "xqaiwgetimageclient.h"
#endif

void HomeScreenStatePluginTest::testWallpaperSelectionStateConstruction()
{
    HsWallpaperSelectionState *s = new HsWallpaperSelectionState;
    QVERIFY(!s->mImageHandler);
    QVERIFY(!s->mWallpaper);
    QVERIFY(!s->mWaitDialog);
    QCOMPARE(s->findChildren<QState *>().count(), 5);
    delete s;
}

void HomeScreenStatePluginTest::testWallpaperSelectionStateImageSelection()
{
    HsWallpaperSelectionState *s = new HsWallpaperSelectionState;
        
    // Generate mImageHandler.
    s->action_selectingImage_start();
    
    QSignalSpy assignImageSpy(s, SIGNAL(event_assignImage()));
    QSignalSpy errorSpy(s, SIGNAL(event_error()));

    s->mImageHandler->setProperty("testSetToFail", false);
    s->mImageHandler->fetch();
    
    QCOMPARE(assignImageSpy.count(), 1);
    assignImageSpy.clear();
    QCOMPARE(errorSpy.count(), 0);

    s->mImageHandler->setProperty("testSetToFail", true);
    s->mImageHandler->fetch();
    
    QCOMPARE(assignImageSpy.count(), 0);    
    QCOMPARE(errorSpy.count(), 1);
    errorSpy.clear();

    delete s;
}

void HomeScreenStatePluginTest::testWallpaperSelectionStateImageAssignment()
{    
    createSceneAndWindow(1, 0);    
    HsWallpaperSelectionState *s = new HsWallpaperSelectionState;
        
    QSignalSpy waitInputSpy(s, SIGNAL(event_waitInput()));
    s->action_assigningImage_start();
    QCOMPARE(waitInputSpy.count(), 1);
    QVERIFY(s->mWallpaper);
    s->action_assigningImage_cleanup();
    QVERIFY(!s->mWallpaper);

    delete s;
    deleteSceneAndWindow();    
}

void HomeScreenStatePluginTest::testWallpaperSelectionStateSimpleMethods()
{
    HbInstance::instance();
    HbMainWindow *window = new HbMainWindow;
    window->move(0, 0);
    window->show();
    
    HsWallpaperSelectionState *s = new HsWallpaperSelectionState;

    // Cleanup
    s->action_selectingImage_start();
    s->action_imageHandler_cleanup();
    QVERIFY(!s->mImageHandler);

    delete s;
    s = new HsWallpaperSelectionState;

    // Wait dialog
    s->action_assigningImage_showWaitDialog();
    QVERIFY(s->mWaitDialog);
    s->mWaitDialog->stop();
    s->action_assigningImage_hideWaitDialog();
    QVERIFY(!s->mWaitDialog);

    delete s;
    s = new HsWallpaperSelectionState;

    QSignalSpy waitInputSpy(s, SIGNAL(event_waitInput()));
    s->action_errorMessage_show();
    QCOMPARE(waitInputSpy.count(), 1);    
    waitInputSpy.clear();
    
    s->onImageSet();
    QCOMPARE(waitInputSpy.count(), 1);
    waitInputSpy.clear();

    QSignalSpy errorSpy(s, SIGNAL(event_error()));

    s->onImageSetFailed();
    QCOMPARE(errorSpy.count(), 1);

    delete s;
    // main window deleted -> HsGui must be deleted also
    delete HsGui::takeInstance();
    delete window;
}

