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
* Description:  Test class for homescreen::core
*
*/


#include <QtTest/QtTest>

class HbMainWindow;

/**
* @test Test class for homescreen::hsapplication modules.
*/
class t_hsapplication : public QObject
{
    Q_OBJECT

private slots:

    /**
     * initialize
     */
    void initTestCase();
    void cleanupTestCase();

#ifdef Q_OS_SYMBIAN
    /**
     * hsrecoverymanager.cpp
     */
    void testHsRecoveryManagerConstruction();
    void testHsRecoveryManagerCrashCount();
    void testHsRecoveryManagerRestoreRomDatabase();
    void testHsRecoveryManagerExecute();
#endif

    /**
     * hshomescreen.cpp
     */
    void testCreateHomescreen();
    void testHomescreenStart();
    void testHomescreenStop();
    void testOnRuntimeStarted();
    void testOnRuntimeStopped();
    void testEventFilter();
    
    /**
     * hshomescreenclientserviceprovider.cpp
     */    
#ifdef Q_OS_SYMBIAN     
    void testCreateClientServiceProvider();
    void testAddWidget();
    void testSetWallpaper();
    void testWidgetComponentDescriptor();
    void testWidgetUninstalled();
#endif    

  /**
	 * HsStateMachine
	 */
   void testConstruction();
	 void testStartStop();
	 void testOnIdleStateEntered();
	 void testOnIdleStateExited();
	 void testRuntimeEventFilter();
   void testActivityRequested();

private:
    HbMainWindow *mWindow;
};
