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
* Description: Unit test for MpGlobalPopupHandler
*
*/

#ifndef TESTMPGLOBALPOPUPHANDLER_H
#define TESTMPGLOBALPOPUPHANDLER_H

#include <QtTest/QtTest>

class MpGlobalPopupHandler;

class TestMpGlobalPopupHandler : public QObject
{
    Q_OBJECT

public:

    TestMpGlobalPopupHandler();
    ~TestMpGlobalPopupHandler();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
private slots:
    void testHandleLibraryAboutToUpdate(); 
    void testScanCountChanged();
    void testHandleScanFinished();
    void testLaunchUnableToCotinueDueUsb();
    void testLaunchUsbBlockingNote();
    void testLaunchMTPInfoDialog();
    void testOutstandingPopupClosing();
    void testLaunchScanFinishedDialog();
    void testLaunchDiskFullDialog();
    void testSetOutstandingPopup(); 

private:

    MpGlobalPopupHandler          *mTest;
    QTranslator                   *mMpTranslator; 

};

#endif  // TESTMPGLOBALPOPUPHANDLER_H
