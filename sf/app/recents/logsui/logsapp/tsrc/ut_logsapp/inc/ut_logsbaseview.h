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
#ifndef UT_LOGSBASEVIEW_H
#define UT_LOGSBASEVIEW_H


#include <QObject>
#include "logsviewmanagerstub.h"

class LogsBaseView;
class LogsComponentRepository;
class LogsMainWindow;


class UT_LogsBaseView : public QObject
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
    void testActivated();
    void testDeactivated();
    void testResetView();
    void testShowFilterMenu();
    void testOpenDialpad();
    void testSetDialpadPostion();
    void testDialpadClosed();
    void testDialpadOpened();
    void testDialpadEditorTextChanged();
    void testChangeFilter();
    void testOpenContactsApp();
    void testModel();
    void testInitiateCall();
    void testInitiateCallback();
    void testShowListItemMenu();
    void testPopulateListItemMenu();
    void testUpdateMenuVisibility();
    void testSetMenuVisible();
    void testSaveContact();
    void testUpdateCall();
    void testUpdateMessage();
    void testUpdateContact();
    void testActivateEmptyListIndicator();
    void testDeactivateEmptyListIndicator();
    void testUpdateEmptyListLabelVisibility();
    void testUpdateListLayoutName();
    void testUpdateListSize();
    void testHandleOrientationChanged();
    void testHandleExit();
    void testIsExitAllowed();
    void testAddToContacts();
    void testSendMessageToCurrentNum();
    void testSaveNumberInDialpadToContacts();
    void testDeleteEvent();
    void testDeleteEventAnswer();
    void testMatchWithActivityId();
    void testSaveActivity();
    void testLoadActivity();
    void testClearActivity();
    void testEnsureListPositioning();
    void testCancelServiceRequest();
    
private:
 
    LogsBaseView* mBaseView;
    LogsComponentRepository* mRepository;
    LogsMainWindow* mMainWindow;
    LogsViewManagerStub* mViewManager;
};

#endif //UT_LogsBaseView_H
