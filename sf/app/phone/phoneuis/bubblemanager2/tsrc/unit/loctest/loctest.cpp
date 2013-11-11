/*!
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
* Description: Test cases to measure localisation metrics.
*
*/

#include <QtGui>
#include <QtTest/QtTest>

#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbview.h>
#include <restricted/hbtextmeasurementutility_r.h> // hb "configure --developer" required
#include <restricted/hbfeaturemanager_r.h>
#include <hbdeviceprofile.h>

#include "bubbletest.h"
#include "bubblemanager2.h"

const int WAIT_TIME = 1000;

class LocTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testPortraitStrings();
    void changeOrientation();
    void testLandscapeStrings();

private:
    void testLabel(int bubbleId, const char *id);
    void testCli(int bubbleId, const char *id);

private:
    HbMainWindow*  mainWindow;
    BubbleManager* bubbleManager; // widget under test
    HbAction*      toolbarAction;
    HbView*        view; // not owned
};

void LocTest::initTestCase()
{
    mainWindow = new HbMainWindow;

    bubbleManager = new BubbleManager();
    connect(mainWindow,SIGNAL(orientationChanged(Qt::Orientation)),
            bubbleManager,SLOT(handleOrientationChange(Qt::Orientation)));

    view = mainWindow->addView( bubbleManager );

    HbToolBar *toolBar = new HbToolBar();
    toolbarAction = new HbAction(this);
    toolBar->addAction( toolbarAction );
    toolBar->addAction( toolbarAction );
    mainWindow->currentView()->setToolBar(toolBar);

    mainWindow->show();

    HbFeatureManager::instance()->setFeatureStatus(
        HbFeatureManager::TextMeasurement, true);

    QTest::qWait( 3000 );
}

void LocTest::cleanupTestCase()
{
    delete mainWindow;
    delete toolbarAction;
}

void LocTest::testPortraitStrings()
{
    view->setTitle( "Measuring portrait" );

    // texts fields are measured according to most narrow situation

    // create test bubble
    bubbleManager->startChanges();
    int bubbleId = bubbleManager->createCallHeader();
    bubbleManager->setState( bubbleId, BubbleManager::Incoming );
    HbAction action1("Answer", this);
    action1.setSoftKeyRole(QAction::PositiveSoftKey);
    bubbleManager->addAction( bubbleId, &action1 );
    HbAction action2("Reject", this);
    action2.setSoftKeyRole(QAction::NegativeSoftKey);
    bubbleManager->addAction( bubbleId, &action2 );
    bubbleManager->setCallObjectFromTheme(bubbleId);
    bubbleManager->setCallFlag(bubbleId,BubbleManagerIF::NoCiphering,true);
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );

    // measure label texts

    // First line texts with ciphering indicator
    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::Outgoing );
    bubbleManager->endChanges();

    testLabel(bubbleId,"txt_phone_other_calling");

    testLabel(bubbleId,"txt_phone_other_calling_video_call");

    testLabel(bubbleId,"txt_phone_other_calling_1_call");

    testLabel(bubbleId,"txt_phone_other_attempting");

    // Second line texts, no indicators -> full width
    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::Incoming );
    bubbleManager->endChanges();

    testLabel(bubbleId,"txt_phone_other_incoming_call");

    testLabel(bubbleId,"txt_phone_other_waiting");

    testLabel(bubbleId,"txt_phone_other_on_hold");

    testLabel(bubbleId,"txt_phone_other_disconnected");

    testLabel(bubbleId,"txt_phone_other_incoming_video_call");

    testLabel(bubbleId,"txt_phone_other_incoming_l1_call");

    // First line CLI texts with ciphering indicator
    testCli(bubbleId,"txt_phone_other_conference_call");

    testCli(bubbleId,"txt_phone_other_remote_sim");

    testCli(bubbleId,"txt_phone_other_emergency_call");

    // First line CLI texts with ciphering and divert indicators
    bubbleManager->startChanges();
    bubbleManager->setCallFlag( bubbleId, BubbleManager::Diverted, true );
    bubbleManager->endChanges();

    testCli(bubbleId,"txt_phone_other_unknown_number");

    testCli(bubbleId,"txt_phone_other_private_number");

    testCli(bubbleId,"txt_phone_other_payphone");

    bubbleManager->startChanges();
    bubbleManager->setCallFlag( bubbleId, BubbleManager::Diverted, false );
    bubbleManager->endChanges();

    // write results
    QTest::qWait( WAIT_TIME );
    HbDeviceProfile profile = HbDeviceProfile::profile(mainWindow);
    QFileInfo info(QCoreApplication::applicationFilePath());
    HbTextMeasurementUtility* utility = HbTextMeasurementUtility::instance();
    utility->writeReport(profile, info.baseName());
    utility->reset();

    QTest::qWait( WAIT_TIME );
    bubbleManager->startChanges();
    bubbleManager->removeCallHeader( bubbleId );
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );
}

void LocTest::changeOrientation()
{
    view->setTitle( "Changing orientation" );
    mainWindow->resize(640,360);
    mainWindow->setOrientation(Qt::Horizontal);
    QTest::qWait(3000);
}

void LocTest::testLandscapeStrings()
{
    view->setTitle( "Measuring landscape" );

    // texts fields are measured according to most narrow situation

    // create test bubble
    bubbleManager->startChanges();
    int bubbleId = bubbleManager->createCallHeader();
    bubbleManager->setState( bubbleId, BubbleManager::Incoming );
    HbAction action1("End call", this);
    action1.setSoftKeyRole(QAction::PositiveSoftKey);
    bubbleManager->addAction( bubbleId, &action1 );
    bubbleManager->setCallObjectFromTheme(bubbleId);
    bubbleManager->setCallFlag( bubbleId, BubbleManager::NoCiphering, true );
    bubbleManager->endChanges();

    // Single call "only" texts

    // Second line texts, no indicators -> full width
    testLabel(bubbleId,"txt_phone_other_incoming_call");

    testLabel(bubbleId,"txt_phone_other_incoming_video_call");

    testLabel(bubbleId,"txt_phone_other_incoming_l1_call");

    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::Outgoing );
    bubbleManager->endChanges();

    // First line texts, with ciphering indicator
    testLabel(bubbleId,"txt_phone_other_attempting");

    testLabel(bubbleId,"txt_phone_other_calling");

    testLabel(bubbleId,"txt_phone_other_calling_video_call");

    testLabel(bubbleId,"txt_phone_other_calling_1_call");

    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::Active );
    bubbleManager->endChanges();

    testCli(bubbleId,"txt_phone_other_emergency_call");

    // Add another call
    bubbleManager->startChanges();
    int bubbleId2 = bubbleManager->createCallHeader();
    bubbleManager->setState( bubbleId, BubbleManager::OnHold );
    bubbleManager->setState( bubbleId2, BubbleManager::Active );
    bubbleManager->setCallObjectFromTheme(bubbleId2);
    bubbleManager->endChanges();

    testLabel(bubbleId,"txt_phone_other_on_hold");

    testLabel(bubbleId,"txt_phone_other_disconnected");

    testCli(bubbleId,"txt_phone_other_conference_call");

    testCli(bubbleId,"txt_phone_other_remote_sim");

    // Add ciphering indicator
    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::Waiting );
    bubbleManager->setCallFlag( bubbleId, BubbleManager::Diverted, true );
    bubbleManager->endChanges();

    testCli(bubbleId,"txt_phone_other_unknown_number");

    testCli(bubbleId,"txt_phone_other_private_number");

    testCli(bubbleId,"txt_phone_other_payphone");

    testLabel(bubbleId,"txt_phone_other_waiting");

    // write results
    QTest::qWait( WAIT_TIME );
    HbDeviceProfile profile = HbDeviceProfile::profile(mainWindow);
    QFileInfo info(QCoreApplication::applicationFilePath());
    HbTextMeasurementUtility* utility = HbTextMeasurementUtility::instance();
    utility->writeReport(profile, info.baseName());
    utility->reset();

    QTest::qWait( WAIT_TIME );
    bubbleManager->startChanges();
    bubbleManager->removeCallHeader( bubbleId );
    bubbleManager->removeCallHeader( bubbleId2 );
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );
}

void LocTest::testLabel(int bubbleId, const char *id)
{
    HbTextMeasurementUtility* utility = HbTextMeasurementUtility::instance();
    bubbleManager->startChanges();
    bubbleManager->setLabel( bubbleId, hbTrId(id));
    bubbleManager->endChanges();
    utility->measureItems(100);
    QTest::qWait( WAIT_TIME );
    bubbleManager->startChanges();
    bubbleManager->setLabel( bubbleId, "");
    bubbleManager->endChanges();
}

void LocTest::testCli(int bubbleId, const char *id)
{
    HbTextMeasurementUtility* utility = HbTextMeasurementUtility::instance();
    bubbleManager->startChanges();
    bubbleManager->setCli( bubbleId, hbTrId(id), Qt::ElideRight);
    bubbleManager->endChanges();
    utility->measureItems(100);
    QTest::qWait( WAIT_TIME );
    bubbleManager->startChanges();
    bubbleManager->setCli( bubbleId, "", Qt::ElideRight);
    bubbleManager->endChanges();
}

BUBBLE_TEST_MAIN(LocTest)
#include "loctest.moc"

