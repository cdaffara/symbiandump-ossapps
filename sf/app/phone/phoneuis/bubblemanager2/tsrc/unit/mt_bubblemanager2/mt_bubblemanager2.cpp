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
* Description:
*
*/

#include <QtGui>
#include <QtTest/QtTest>

#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbview.h>

#include "bubbletest.h"
#include "bubblemanager2.h"

const int WAIT_TIME = 200;

class mt_BubbleManager2 : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testBasicMtCall();
    void testBasicMoCall();
    void testWaitingCall();
    void testConferenceCall();
    void testEmergencyCall();

private:
    void mouseClickWidget(const QString& name);
    void mouseClickWidgetAction(const QString& name, HbAction* action);
    QGraphicsWidget* getWidgetByName(const QString& name);

private:
    HbMainWindow*  mainWindow;
    BubbleManager* bubbleManager; // widget under test
    HbAction*      toolbarAction;
    HbView*        view; // not owned
};

void mt_BubbleManager2::initTestCase()
{
    mainWindow = new HbMainWindow;

    bubbleManager = new BubbleManager();

    view = mainWindow->addView( bubbleManager );

    HbToolBar *toolBar = new HbToolBar();
    toolbarAction = new HbAction(this);
    toolBar->addAction( toolbarAction );
    toolBar->addAction( toolbarAction );
    //toolBar->setOrientation(mainWindow->orientation());
    mainWindow->currentView()->setToolBar(toolBar);

    mainWindow->show();
}

void mt_BubbleManager2::cleanupTestCase()
{
    delete mainWindow;
    delete toolbarAction;
}

void mt_BubbleManager2::testBasicMtCall()
{
    view->setTitle( "Basic MT call" );
    bubbleManager->startChanges();
    int bubbleId = bubbleManager->createCallHeader();
    QVERIFY( bubbleId == 0 );
    bubbleManager->setState( bubbleId, BubbleManager::Incoming );
    bubbleManager->setCli( bubbleId, "John Doe", Qt::ElideRight );
    bubbleManager->setSecondaryCli( bubbleId, "0407284096" );
    bubbleManager->setNumberType( bubbleId, BubbleManager::Mobile );
    bubbleManager->setCallFlags( bubbleId, BubbleManager::Diverted );
    bubbleManager->setCallFlag( bubbleId, BubbleManager::NoCiphering, true );
    bubbleManager->setLabel( bubbleId,"calling");
    bubbleManager->setCallObjectFromTheme(bubbleId);
    HbAction action1("Answer", this);
    action1.setSoftKeyRole(QAction::PositiveSoftKey);
    bubbleManager->addAction( bubbleId, &action1 );
    HbAction action2("Reject", this);
    action2.setSoftKeyRole(QAction::NegativeSoftKey);
    bubbleManager->addAction( bubbleId, &action2 );
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );
    mouseClickWidgetAction("eb:leftButton",&action1);
    mouseClickWidgetAction("eb:rightButton",&action2);
    QTest::qWait( WAIT_TIME );

    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::Active );
    bubbleManager->setLabel( bubbleId,"");
    bubbleManager->setCallTime( bubbleId, "00:00" );
    bubbleManager->setCallFlag( bubbleId, BubbleManager::NoCiphering, false );
    bubbleManager->clearActions(bubbleId);
    HbAction action3("Hold", this);
    bubbleManager->addAction( bubbleId, &action3 );
    HbAction action4("End call", this);
    action4.setSoftKeyRole(QAction::NegativeSoftKey);
    bubbleManager->addAction( bubbleId, &action4 );
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );
    bubbleManager->updateCallTime( bubbleId, "00:01" );
    bubbleManager->updateCLI(bubbleId, "John Doe", Qt::ElideRight);
    QTest::qWait( WAIT_TIME );
    bubbleManager->updateCallTime( bubbleId, "00:02" );
    mouseClickWidgetAction("eb:leftButton",&action3);
    mouseClickWidgetAction("eb:rightButton",&action4);
    QTest::qWait( WAIT_TIME );

    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::OnHold );
    bubbleManager->setLabel( bubbleId,"on hold");
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );
    
    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::Disconnected );
    bubbleManager->setLabel( bubbleId,"disconnected");
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );
    bubbleManager->startChanges();
    bubbleManager->removeCallHeader( bubbleId );
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );
}

void mt_BubbleManager2::testBasicMoCall()
{
    view->setTitle( "Basic M0 call" );
    bubbleManager->startChanges();
    int bubbleId = bubbleManager->createCallHeader();
    QVERIFY( bubbleId == 0 );
    bubbleManager->setState( bubbleId, BubbleManager::Outgoing );
    bubbleManager->setCli( bubbleId, "Mary Poppins", Qt::ElideRight );
    bubbleManager->setSecondaryCli( bubbleId, "0507284096" );
    bubbleManager->setNumberType( bubbleId, BubbleManager::Mobile );
    bubbleManager->setLabel( bubbleId,"Calling");
    bubbleManager->setCallFlag( bubbleId, BubbleManager::NoCiphering, true );
    bubbleManager->setCallObjectFromTheme(bubbleId);
    HbAction action1("End call", this);
    action1.setSoftKeyRole(QAction::NegativeSoftKey);
    bubbleManager->clearActions(bubbleId);
    bubbleManager->addAction( bubbleId, &action1 );
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );
    mouseClickWidgetAction("eb:centerButton", &action1 );
    QTest::qWait( WAIT_TIME );

    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::Alerting );
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );
    
    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::Active );
    bubbleManager->setLabel( bubbleId,"");
    bubbleManager->setCallTime( bubbleId, "00:00" );
    bubbleManager->setCallFlag( bubbleId, BubbleManager::NoCiphering, false);
    bubbleManager->clearActions(bubbleId);
    HbAction action2("Hold", this);
    bubbleManager->addAction( bubbleId, &action2 );
    HbAction action3("End call", this);
    action3.setSoftKeyRole(QAction::NegativeSoftKey);
    bubbleManager->addAction( bubbleId, &action3 );
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );
    bubbleManager->updateCallTime( bubbleId, "00:01" );
    mouseClickWidgetAction("eb:leftButton",&action2);
    mouseClickWidgetAction("eb:rightButton",&action3);
    QTest::qWait( WAIT_TIME );

    bubbleManager->setPhoneMuted(true);
    QTest::qWait( WAIT_TIME );
    bubbleManager->setPhoneMuted(false);

    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::Disconnected );
    bubbleManager->setLabel( bubbleId,"disconnected");
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );
    bubbleManager->startChanges();
    bubbleManager->removeCallHeader( bubbleId );
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );
}

void mt_BubbleManager2::testWaitingCall()
{
    view->setTitle( "Waiting call" );
    bubbleManager->startChanges();
    int bubbleId1 = bubbleManager->createCallHeader();
    QVERIFY( bubbleId1 == 0 );
    bubbleManager->setState( bubbleId1, BubbleManager::Active );
    bubbleManager->setCli( bubbleId1, "Mary Poppins", Qt::ElideRight );
    bubbleManager->setSecondaryCli( bubbleId1, "0507284096" );
    bubbleManager->setCallObjectFromTheme(bubbleId1);
    bubbleManager->setCallTime( bubbleId1, "00:00" );
    bubbleManager->clearActions(bubbleId1);
    HbAction action1("Hold", this);
    bubbleManager->addAction( bubbleId1, &action1 );
    HbAction action2("End call", this);
    action2.setSoftKeyRole(QAction::NegativeSoftKey);
    bubbleManager->addAction( bubbleId1, &action2 );
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );

    // waiting call
    bubbleManager->startChanges();
    int bubbleId2 = bubbleManager->createCallHeader();
    QVERIFY( bubbleId2 == 1 );
    bubbleManager->setState( bubbleId2, BubbleManager::Waiting );
    bubbleManager->setCli( bubbleId2, "John Doe", Qt::ElideRight );
    bubbleManager->setLabel( bubbleId2,"Waiting");
    bubbleManager->setSecondaryCli( bubbleId2, "0501234567" );
    bubbleManager->setCallObjectFromTheme(bubbleId2);
    bubbleManager->clearActions(bubbleId2);
    HbAction action3("Answer", this);
    action3.setSoftKeyRole(QAction::PositiveSoftKey);
    bubbleManager->addAction( bubbleId2, &action3 );
    HbAction action4("Reject", this);
    action4.setSoftKeyRole(QAction::NegativeSoftKey);
    bubbleManager->addAction( bubbleId2, &action4 );
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );
    mouseClickWidgetAction("eb:leftButton",&action3);
    mouseClickWidgetAction("eb:rightButton",&action4);
    bubbleManager->updateCallTime(bubbleId1,"00:01");

    // two calls
    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId1, BubbleManager::OnHold );
    bubbleManager->setLabel( bubbleId1,"On hold");
    bubbleManager->setState( bubbleId2, BubbleManager::Active );
    bubbleManager->setLabel( bubbleId2,"");
    bubbleManager->clearActions(bubbleId2);
    bubbleManager->addAction( bubbleId2, &action1 );
    bubbleManager->addAction( bubbleId2, &action2 );
    HbAction action5("Swap", this);
    bubbleManager->setExpandAction( bubbleId1, &action5 );
    bubbleManager->setExpandAction( bubbleId2, &action5 );
    bubbleManager->endChanges();

    int expanded = bubbleManager->expandedBubble();
    QVERIFY( expanded == bubbleId2 );

    QTest::qWait( WAIT_TIME );
    mouseClickWidgetAction("collapsedBubble",&action5);
    expanded = bubbleManager->expandedBubble();
    QVERIFY( expanded == bubbleId1 );

    QTest::qWait( WAIT_TIME );
    bubbleManager->setBubbleSelectionDisabled(true);
    mouseClickWidget("collapsedBubble");
    expanded = bubbleManager->expandedBubble();
    QVERIFY( expanded == bubbleId1 );
    bubbleManager->setBubbleSelectionDisabled(false);

    // waiting call (third call)
    bubbleManager->startChanges();
    int bubbleId3 = bubbleManager->createCallHeader();
    QVERIFY( bubbleId3 == 2 );
    bubbleManager->setState( bubbleId3, BubbleManager::Waiting );
    bubbleManager->setCli( bubbleId3, "Mary Poppins", Qt::ElideRight );
    bubbleManager->setLabel( bubbleId3,"Waiting");
    bubbleManager->setSecondaryCli( bubbleId3, "0501234567" );
    bubbleManager->setCallObjectFromTheme(bubbleId3);
    bubbleManager->clearActions(bubbleId3);
    HbAction action6("Replace", this);
    bubbleManager->addAction( bubbleId3, &action6 );
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );
    expanded = bubbleManager->expandedBubble();
    QVERIFY( expanded == bubbleId3 );

    // click top most bubble - bubble2
    mouseClickWidgetAction("collapsedBubble2",&action5);
    expanded = bubbleManager->expandedBubble();
    QVERIFY( expanded == bubbleId2 );
    QTest::qWait( WAIT_TIME );

    bubbleManager->startChanges();
    bubbleManager->removeCallHeader( bubbleId3 );
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );
    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId2, BubbleManager::Disconnected );
    bubbleManager->setLabel( bubbleId2,"disconnected");
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );
    bubbleManager->startChanges();
    bubbleManager->removeCallHeader( bubbleId1 );
    bubbleManager->removeCallHeader( bubbleId2 );
    bubbleManager->endChanges();
}

void mt_BubbleManager2::testConferenceCall()
{
    view->setTitle( "Test conference" );
    bubbleManager->startChanges();
    // held call
    int bubbleId1 = bubbleManager->createCallHeader();
    bubbleManager->setState( bubbleId1, BubbleManager::OnHold );
    bubbleManager->setCli( bubbleId1, "Mary Poppins", Qt::ElideRight );
    bubbleManager->setSecondaryCli( bubbleId1, "0507284096" );
    bubbleManager->setCallObjectFromTheme(bubbleId1);
    bubbleManager->setLabel( bubbleId1,"On hold");
    bubbleManager->clearActions(bubbleId1);
    HbAction action1("Activate", this);
    bubbleManager->addAction( bubbleId1, &action1 );
    HbAction action2("End call", this);
    action2.setSoftKeyRole(QAction::NegativeSoftKey);
    bubbleManager->addAction( bubbleId1, &action2 );
    // active call
    int bubbleId2 = bubbleManager->createCallHeader();
    bubbleManager->setState( bubbleId2, BubbleManager::Waiting );
    bubbleManager->setCli( bubbleId2, "John Doe", Qt::ElideRight );
    bubbleManager->setSecondaryCli( bubbleId2, "0501234567" );
    bubbleManager->setCallObjectFromTheme(bubbleId2);
    bubbleManager->clearActions(bubbleId2);
    HbAction action3("Hold", this);
    bubbleManager->addAction( bubbleId2, &action3 );
    HbAction action4("End call", this);
    action4.setSoftKeyRole(QAction::NegativeSoftKey);
    bubbleManager->addAction( bubbleId2, &action4 );
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );

    bubbleManager->startChanges();
    int bubbleIdC = bubbleManager->createConference(bubbleId1,bubbleId2);
    bubbleManager->setCli(bubbleIdC,"Conference call",Qt::ElideRight);
    bubbleManager->setExpandedConferenceCallHeader(true);
    bubbleManager->setState(bubbleIdC,BubbleManager::Active);
    bubbleManager->setState(bubbleId1,BubbleManager::Active);
    bubbleManager->setState(bubbleId2,BubbleManager::Active);
    bubbleManager->setCallTime(bubbleIdC,"00:00");
    HbAction action5("End conference");
    action5.setSoftKeyRole(QAction::NegativeSoftKey);
    bubbleManager->addAction(bubbleIdC,&action5);
    bubbleManager->clearParticipantListActions();
    HbAction action6("Private");
    bubbleManager->addParticipantListAction(&action6);
    HbAction action7("Drop");
    bubbleManager->addParticipantListAction(&action7);
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );
    bubbleManager->updateCallTime(bubbleIdC,"00:01");
    mouseClickWidgetAction("eb:centerButton",&action5);
    QVERIFY(bubbleManager->conferenceRowCount()==2);
    QVERIFY(bubbleManager->expandedBubble()==bubbleIdC);
    QTest::qWait( WAIT_TIME );

    // add call to conference
    bubbleManager->startChanges();
    int bubbleId3 = bubbleManager->createCallHeader();
    bubbleManager->setState( bubbleId3, BubbleManager::Active );
    bubbleManager->setCli( bubbleId3, "123457", Qt::ElideLeft );
    bubbleManager->setCallObjectFromTheme(bubbleId3);
    bubbleManager->clearActions(bubbleId3);
    HbAction action8("Hold", this);
    bubbleManager->addAction( bubbleId3, &action8 );
    HbAction action9("End call", this);
    action9.setSoftKeyRole(QAction::NegativeSoftKey);
    bubbleManager->addAction( bubbleId3, &action9 );
    bubbleManager->setExpandedConferenceCallHeader(false);
    bubbleManager->setState(bubbleIdC,BubbleManager::OnHold);
    bubbleManager->setLabel(bubbleIdC,"On hold",Qt::ElideRight);
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );
    bubbleManager->startChanges();
    bubbleManager->setExpandedConferenceCallHeader(true);
    bubbleManager->addRowToConference(bubbleId3);
    bubbleManager->setState(bubbleIdC,BubbleManager::Active);
    // to test two button layout
    bubbleManager->clearActions(bubbleIdC);
    bubbleManager->addAction(bubbleIdC,&action8);
    bubbleManager->addAction(bubbleIdC,&action5);
    bubbleManager->endChanges();
    Q_ASSERT(bubbleManager->conferenceRowCount()==3);
    QTest::qWait( WAIT_TIME );
    mouseClickWidgetAction("eb:leftButton",&action8);
    mouseClickWidgetAction("eb:rightButton",&action5);

    // click conference list
    QTest::mouseClick(mainWindow->viewport(),Qt::LeftButton,0,QPoint(100,100));
    QTest::qWait( 2*WAIT_TIME );

    // hold/unhold conference
    bubbleManager->startChanges();
    bubbleManager->setState( bubbleIdC, BubbleManager::OnHold );
    bubbleManager->setState( bubbleId1, BubbleManager::OnHold );
    bubbleManager->setState( bubbleId2, BubbleManager::OnHold );
    bubbleManager->setState( bubbleId3, BubbleManager::OnHold );
    bubbleManager->endChanges();
    QTest::qWait( 2*WAIT_TIME );
    bubbleManager->startChanges();
    bubbleManager->setState( bubbleIdC, BubbleManager::Active );
    bubbleManager->setState( bubbleId1, BubbleManager::Active );
    bubbleManager->setState( bubbleId2, BubbleManager::Active );
    bubbleManager->setState( bubbleId3, BubbleManager::Active );
    bubbleManager->endChanges();

    // remove call from conference
    bubbleManager->startChanges();
    bubbleManager->removeRowFromConference(bubbleId3);
    bubbleManager->removeCallHeader(bubbleId3);
    bubbleManager->endChanges();
    Q_ASSERT(bubbleManager->conferenceRowCount()==2);
    QTest::qWait( WAIT_TIME );

    // to be implemented API methods
    bubbleManager->startChanges();
    bubbleManager->setSelectionInConference(0);
    bubbleManager->setSelectionIdInConference(bubbleId1);
    bubbleManager->selectionInConference();
    bubbleManager->selectionIdInConference();
    bubbleManager->moveHighlightOneUpInConference();
    bubbleManager->moveHighlightOneDownInConference();
    bubbleManager->setParticipantListCli(bubbleId1,
        BubbleManagerIF::ParticipantListCliText);
    bubbleManager->endChanges();

    bubbleManager->startChanges();
    bubbleManager->removeConference();
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );

    bubbleManager->startChanges();
    bubbleManager->removeCallHeader(bubbleId1);
    bubbleManager->removeCallHeader(bubbleId2);
    bubbleManager->endChanges();
}

void mt_BubbleManager2::testEmergencyCall()
{
    view->setTitle( "Emergency call" );
    bubbleManager->startChanges();
    int bubbleId = bubbleManager->createCallHeader();
    QVERIFY( bubbleId == 0 );
    bubbleManager->setState( bubbleId, BubbleManager::Outgoing );
    bubbleManager->setCli( bubbleId, "emergency call", Qt::ElideRight );
    bubbleManager->setNumberType( bubbleId, BubbleManager::Mobile );
    bubbleManager->setLabel( bubbleId,"Attempting");
    bubbleManager->setCallFlag(bubbleId, BubbleManager::EmergencyCall, true);
    bubbleManager->setCallObjectFromTheme(bubbleId);
    HbAction action1("End call", this);
    action1.setSoftKeyRole(QAction::NegativeSoftKey);
    bubbleManager->clearActions(bubbleId);
    bubbleManager->addAction( bubbleId, &action1 );
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );
    mouseClickWidgetAction("eb:centerButton",&action1);
    QTest::qWait( WAIT_TIME );

    bubbleManager->startChanges();
    bubbleManager->setCli( bubbleId, "Emergency call", Qt::ElideRight );
    bubbleManager->setState( bubbleId, BubbleManager::Alerting );
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );

    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::Active );
    bubbleManager->setLabel( bubbleId,"");
    bubbleManager->setCallTime( bubbleId, "00:00" );
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );

    bubbleManager->startChanges();
    bubbleManager->setState( bubbleId, BubbleManager::Disconnected );
    bubbleManager->setLabel( bubbleId,"disconnected");
    bubbleManager->endChanges();

    QTest::qWait( WAIT_TIME );
    bubbleManager->startChanges();
    bubbleManager->removeCallHeader( bubbleId );
    bubbleManager->endChanges();
    QTest::qWait( WAIT_TIME );
}

void mt_BubbleManager2::mouseClickWidget(
    const QString& name )
{
    QGraphicsWidget* widget = getWidgetByName(name);

    if ( widget ) {
        QPointF widgetPos = widget->scenePos() +
                            widget->rect().center();

        QPoint windowPos = mainWindow->mapFromScene( widgetPos );

        QTest::mouseClick( mainWindow->viewport(), Qt::LeftButton, 0, windowPos );
        QTest::qWait( WAIT_TIME );
    } else {
        QFAIL( "Widget cannot be accessed!" );
    }
}

void mt_BubbleManager2::mouseClickWidgetAction(
    const QString& name,
    HbAction* action )
{
    QSignalSpy spy( action, SIGNAL( triggered() ) );

    QGraphicsWidget* widget = getWidgetByName(name);

    if ( widget ) {
        QPointF widgetPos = widget->scenePos() +
                            widget->rect().center();

        QPoint windowPos = mainWindow->mapFromScene( widgetPos );

        QTest::mouseClick( mainWindow->viewport(), Qt::LeftButton, 0, windowPos );
        QTest::qWait( WAIT_TIME );
        QVERIFY( spy.count() == 1 );
    } else {
        QFAIL( "Widget cannot be accessed!" );
    }
}

QGraphicsWidget* mt_BubbleManager2::getWidgetByName(const QString& name)
{
    Q_ASSERT(mainWindow!=0);

    QGraphicsWidget* widget = 0;

    QList<QGraphicsItem*> items = mainWindow->scene()->items();
    foreach (QGraphicsItem* item, items) {
        if (item->isWidget()) {
            QGraphicsWidget *w = static_cast<QGraphicsWidget*>(item);
            if (w->objectName()==name) {
                widget = w;
            }
        }
    }

    return widget;
}

BUBBLE_TEST_MAIN(mt_BubbleManager2)
#include "mt_bubblemanager2.moc"

