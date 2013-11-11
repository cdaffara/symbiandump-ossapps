/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Tests for HsHomeScreenStatePlugin class.
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include "hshomescreenstateplugin.h"
#include "hsidlestate.h"
#include "hsloadscenestate.h"
#include "hscontentservice.h"
#include "hspage.h"
#include "hswidgethost.h"

/**
* @test Test class for homescreen::hsutils modules.
*/
class HomeScreenStatePluginTest : public QObject
{
    Q_OBJECT

signals:
    void finishStateMachine();

private slots:

    /**
     * initialize
     */
    void initTestCase();
    void cleanup();
    void init();
    void cleanupTestCase();

    /**
     * hswallpaperselectionstate.cpp
     */
    void testWallpaperSelectionStateConstruction();
    void testWallpaperSelectionStateImageSelection();
    void testWallpaperSelectionStateImageAssignment();
    void testWallpaperSelectionStateSimpleMethods();

    /**
     * hsloadscenestate.cpp
     */
    void testLoadSceneStateConstructors();
    void testLoadSceneStateOnEntryExit();
#ifdef Q_OS_SYMBIAN
    void testLoadSceneStateHandleKeyChange();
#endif
    /**
     * hsrootstate.cpp
     */
    void testLoadRootStateConstructors();
    void testLoadRootStateOnEntryExit();

    /**
     * t_hsbackuprestorestate.cpp
     */
    void testLoadBackupRestoreStateConstructors();
    void testLoadBackupRestoreStateOnEntryExit();

    /**
     * hsidlestate.cpp
     */
    void testIdleStateConstruction();
    void testIdleStateEntryAndExit();
    void testIdleStatePageLayerPos();
    void testIdleStatePageChangeAnimation();
    void testIdleStatePageChangeZones();
    void testIdleStateAddPage();
    void testIdleStateRemoveActivePage();
    void testIdleStateRemovePage();
    void testIdleStateToggleConnection();
    void testIdleStateTapPage();
    void testIdleStatePanPage();
    void testIdleStateTapWidget();
    void testIdleStateDragWidget();
    void testIdleStateDeleteWidget();
    void testIdleStateMisc();
    void testIdleStateChromeBehaviour();
    void testEventFilter();
    void testIdleStateSnapToObject();
    void testIdleStateSnapToObjectSnapEffectDisabled();
    void testIdleStateSnapToObjectSnappingDisabled();
    void testOnVerticalSnapLineTimerTimeout();
    void testOnHorizontalSnapLineTimerTimeout();
    void testShowHorizontalLine();
    void testEditorAndVKB();
    
   

private:
    void createSceneAndWindow(int pageCount, int defaultPageIndex = 0, int maximumPageCount = 8);
    void deleteSceneAndWindow();

private:
    HbMainWindow *mMainWindow;
    int mImageProcessingDelay;
};
