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
#ifndef UT_LOGSRECENTCALLSVIEW_H
#define UT_LOGSRECENTCALLSVIEW_H


#include <QObject>
#include "logsviewmanagerstub.h"

class LogsRecentCallsView;
class LogsComponentRepository;
class LogsMainWindow;
class QModelIndex;

class UT_LogsRecentCallsView : public QObject
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
     
    void testConstructor();
    void testInitView();
    void testActivated();
    void testDeactivated();
    void testInitListWidget();
    void testUpdateFilter();
    void testUpdateViewName();
    void testChangeFilter();
    void testInitiateCallback();
    void testCallKeyPressed();
    void testShowCallDetails();
    void testOpenDialpad(); 
    void testDialpadEditorTextChanged();
    void testGestureEvent();
    void testViewChangeByFlicking();    
    void testModel();
    void testShowListItemMenu();
    void testUpdateListItemData();
    void testPopulateListItemMenu();
    void testUpdateCall();
    void testUpdateMessage();
    void testUpdateContact();
    void testActivateEmptyListIndicator();
    void testUpdateMenu();
    void testUpdateWidgetsSizeAndLayout();
    void testGetListItemTextWidth();
    void testDialpadClosed();
    void testClearList();
    void testClearListAnswer();
    void testIsExitAllowed();
    void testContactSearch();
    void testDialpadOpened();
    void testSaveActivity();
    void testLoadActivity();
    
private: //helper functions
    
    inline QModelIndex makeValidFilterIndex(LogsRecentCallsView& view);
    
private:
 
    LogsRecentCallsView* mRecentCallsView;
    LogsComponentRepository* mRepository;
    LogsMainWindow* mMainWindow;
    LogsViewManagerStub* mViewManager;
};

#endif //UT_LOGSRECENTCALLSVIEW_H
