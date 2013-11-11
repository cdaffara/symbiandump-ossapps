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
#ifndef UT_LOGSVIEWMANAGER_H
#define UT_LOGSVIEWMANAGER_H


#include <QObject>

class HbMainWindow;
class LogsViewManager;
class LogsMainWindow;
class LogsServiceHandler;
class LogsServiceHandlerOld;
class LogsAppSettings;

class UT_LogsViewManager : public QObject
{
    Q_OBJECT
     
private slots:

/*
 * In addition, there are four private slots that are not treated as testfunctions. 
 * They will be executed by the testing framework and can be used to initialize and clean up 
 * either the entire test or the current test function.
 * 
 * initTestCase() will be called before the first testfunction is executed.
 * cleanupTestCase() will be called after the last testfunction was executed.
 * init() will be called before each testfunction is executed.
 * cleanup() will be called after every testfunction.
*/
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();


private slots: //test methods
     
    void testConstructorDestructor();
    void testActivateView();
    void testchangeMatchesView();
    void testchangeRecentView();
    void testExitApplication();
    void testStartingWithService();
    void testHandleOrientationChanged();
    void testCompleteViewActivation();
    void testSaveActivity();
    void testLoadActivity();
    void testAppGainedForeground();
    void testActivityRequested();
    void testBgStartupForegroundGained();
    
private:
 
    LogsMainWindow* mMainWindow;
    LogsViewManager* mLogsViewManager;
    
    LogsServiceHandler* mService;
    LogsServiceHandlerOld* mServiceOld;
    
    LogsAppSettings* mSettings;
};

#endif //UT_LOGSVIEWMANAGER_H
