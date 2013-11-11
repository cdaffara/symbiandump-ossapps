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
* Description:  Tests for HsIdleState class.
*
*/

#include <QStateMachine>
#include <QGestureEvent>

#include <HbMainWindow>
#include <hbview.h>
#include <hbmenu.h>
#include <HbMessageBox>
#include <HbAction>
#include <HbTapAndHoldGesture>
#include <HbPanGesture>
#include "t_hshomescreenstateplugin.h"
#include "hsscene.h"
#include "hsgui.h"
#include "hspage.h"
#include "hswidgethost.h"
#include "hswidgethostvisual.h"
#include "hstitleresolver.h"
#include "hsconfiguration.h"
#include "hsidlewidget.h"

void HomeScreenStatePluginTest::testIdleStateConstruction()
{
    QState *parent = new QState;
    HsIdleState *is = new HsIdleState(parent);

    QVERIFY(is->parent() == parent);
    QVERIFY(!is->mNavigationAction);

    QVERIFY(!is->findChildren<QState *>().isEmpty());

    delete parent;
}

void HomeScreenStatePluginTest::testIdleStateEntryAndExit()
{
    createSceneAndWindow(1, 0);

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);

    QFinalState *fs = new QFinalState;
    sm->addState(fs);

    is->addTransition(this, SIGNAL(finishStateMachine()), fs);

    sm->start();
   // QApplication::processEvents();
    QCoreApplication::sendPostedEvents();

    emit finishStateMachine();

    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStatePageLayerPos()
{
    createSceneAndWindow(1, 0);

    HsIdleState *is = new HsIdleState;
    QVERIFY(qFuzzyCompare(is->pageLayerXPos(0), qreal(0)));

    delete is;
    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStatePageChangeAnimation()
{
    createSceneAndWindow(2, 0);

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);

    QFinalState *fs = new QFinalState;
    sm->addState(fs);

    is->addTransition(this, SIGNAL(finishStateMachine()), fs);

    sm->start();
    QCoreApplication::sendPostedEvents();
    is->startPageChangeAnimation(1, 10);

    // Start two anims in a row
    is->startPageChangeAnimation(1, 10);
    is->startPageChangeAnimation(2, 10);

    emit finishStateMachine();

    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStatePageChangeZones()
{
    createSceneAndWindow(1, 0);
    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    
    //Test left page change zone's left border
    is->mWidgetHotSpot = QPointF(0, 0);
    QVERIFY(is->isInLeftPageChangeZone());
    QVERIFY(!is->isInRightPageChangeZone());
    QVERIFY(is->isInPageChangeZone());

    //Test left page change zone's right border
    is->mWidgetHotSpot = QPointF(HSCONFIGURATION_GET(pageChangeZoneWidth) - 1, 0);
    QVERIFY(is->isInLeftPageChangeZone());
    QVERIFY(!is->isInRightPageChangeZone());
    QVERIFY(is->isInPageChangeZone());

    //Test not in either of the page change zones
    is->mWidgetHotSpot = QPointF(HSCONFIGURATION_GET(pageChangeZoneWidth), 0);
    QVERIFY(!is->isInLeftPageChangeZone());
    QVERIFY(!is->isInRightPageChangeZone());
    QVERIFY(!is->isInPageChangeZone());

    //Test right page change zone's right border
    HsScene *scene = HsScene::instance();

    sm->stop();
    delete sm;
    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStateAddPage()
{
    createSceneAndWindow(2, 0, 3); //pagecount 2, default page 0, max pages 3

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    QString menuaction;
    bool found = false;
    HbView *idleView = HsGui::instance()->idleView();
    for(int i=0; i < idleView->menu()->actions().count(); i++){
        menuaction = idleView->menu()->actions()[i]->text();
        if(menuaction.left(27) == "txt_homescreen_opt_add_page"){
            found = true;
        }
    }
    if(!found){
        QFAIL("txt_homescreen_opt_add_page menu action not found!");
    }

    emit is->event_addPage();

    HsScene *scene = HsScene::instance();
    QCOMPARE(scene->pages().count(), 3);
    // Active page index 0, the new page is created to the right side of it.
    QCOMPARE(scene->activePageIndex(), 1);

    found = false;
    for(int i=0; i < idleView->menu()->actions().count(); i++){
        menuaction = idleView->menu()->actions()[i]->text();
        if(menuaction.left(27) == "txt_homescreen_opt_add_page"){
            found = true;
        }
    }
    if(found){
        QFAIL("txt_homescreen_opt_add_page menu action should not be found!");
    }

    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStateRemoveActivePage()
{
    createSceneAndWindow(3, 2);

    HsIdleState *is = new HsIdleState;
    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    HsScene *scene = HsScene::instance();
    int numOfPages = scene->pages().count();

    emit is->event_preRemovePage();
    QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
    QVERIFY(scene->pages().count() == numOfPages-1);
    QVERIFY(scene->activePageIndex() == 1);
    
    sm->stop();
    
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStateRemovePage()
{
    createSceneAndWindow(2, 0);
    HsScene::instance()->setActivePageIndex(1); // default page 0 cannot be removed

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
   
    QCoreApplication::sendPostedEvents();
    
    QString menuaction;
    bool found = false;
    HbView *idleView = HsGui::instance()->idleView();
    for(int i=0; i < idleView->menu()->actions().count(); i++){
        menuaction = idleView->menu()->actions()[i]->text();
        if(menuaction.left(30) == "txt_homescreen_opt_remove_page"){
            found = true;
        }
    }
    if(!found){
        QFAIL("txt_homescreen_opt_remove_page menu action not found!");
    }

    emit is->event_preRemovePage();
    
    QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);

    HsScene *scene = HsScene::instance();
    QCOMPARE(scene->pages().count(), 1); 
    QCOMPARE(scene->activePageIndex(), 0);

    found = false;
    idleView = HsGui::instance()->idleView();
    for(int i=0; i < idleView->menu()->actions().count(); i++){
        menuaction = idleView->menu()->actions()[i]->text();
        if(menuaction.left(30) == "txt_homescreen_opt_remove_page"){
            found = true;
        }
    }
    if(found){ // Se above comment.
        QFAIL("txt_homescreen_opt_remove_page menu action shound not be found!");
    }

    sm->stop();
    
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStateToggleConnection()
{
    createSceneAndWindow(1, 0);

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    
    HsScene *scene = HsScene::instance();

    QVERIFY(scene->isOnline());
    emit is->event_toggleConnection();
    QVERIFY(!scene->isOnline());
    
    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStateTapPage()
{
    createSceneAndWindow(HSCONFIGURATION_GET(maximumPageCount), 0);

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    
    HbTapAndHoldGesture *gesture = new HbTapAndHoldGesture;
    gesture->setHotSpot(QPointF(10, 100));
    QList<QGesture *> gestures;
    gestures << gesture;
    QGestureEvent *gestureEvent = new QGestureEvent(gestures);

    QVERIFY(!is->mSceneMenu);
    is->onPageTapAndHoldFinished(gestureEvent);
    QVERIFY(is->mSceneMenu);
    QVERIFY(is->mSceneMenu->actions().count() == 2);  //Add page action should not exist

    delete gestureEvent;
    gestures.clear(); 
    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStatePanPage()
{
    createSceneAndWindow(2, 0);

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    
    HbPanGesture *gesture = new HbPanGesture;
    gesture->setHotSpot(QPointF(10, 100));
    QList<QGesture *> gestures;
    gestures << gesture;
    QGestureEvent *gestureEvent = new QGestureEvent(gestures);

    //Start Panning
    QSignalSpy moveSceneSpy(is, SIGNAL(event_moveScene()));
    is->onPagePanStarted(gestureEvent);
    QCOMPARE(moveSceneSpy.count(), 1);

    //Continue Panning
    gesture->setHotSpot(QPointF(50, 100));
    is->onPagePanUpdated(gestureEvent);

    //Finish Panning
    gesture->setHotSpot(QPointF(150, 100));
    QSignalSpy waitInputSpy(is, SIGNAL(event_waitInput()));
    is->onPagePanFinished(gestureEvent);
    QCOMPARE(waitInputSpy.count(), 1);

    delete gestureEvent;
    gestures.clear(); 
    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStateTapWidget()
{
    createSceneAndWindow(2, 0);

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    
    HsScene *scene = HsScene::instance();
    HsWidgetHost *widget = scene->activePage()->widgets().first();
    is->onWidgetTapStarted(QPointF(0,0), widget);
    // widget is not activated on tap, only on tap and hold
    QVERIFY(!scene->activeWidget());

    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStateDragWidget()
{
    createSceneAndWindow(2, 0, 3);
    QStateMachine *sm = new QStateMachine;
    HsIdleState *is = new HsIdleState;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    // try to move widget left from page 1, which isn't possible
    HbTapAndHoldGesture *gesture = new HbTapAndHoldGesture;
    gesture->setHotSpot(QPointF(10, 100));
    QList<QGesture *> gestures;
    gestures << gesture;
    QGestureEvent *gestureEvent = new QGestureEvent(gestures);
    QSignalSpy moveWidgetSpy(is, SIGNAL(event_moveWidget()));
    HsScene *scene = HsScene::instance();
    HsWidgetHost *widget = scene->activePage()->widgets().first();
    is->onWidgetTapAndHoldFinished(gestureEvent, widget);
    QCOMPARE(moveWidgetSpy.count(), 1);
    moveWidgetSpy.clear();
    
    is->onWidgetMoveUpdated(QPointF(10, 150), widget);
    is->onWidgetMoveUpdated(QPointF(150, 150), widget);
    QSignalSpy waitInputSpy(is, SIGNAL(event_MoveWidgetDelete()));
    is->onWidgetMoveFinished(QPointF(150, 150), widget);
    QCOMPARE(waitInputSpy.count(), 1);
    waitInputSpy.clear();
     
    // move widget to second page
    gesture->setHotSpot(QPointF(150, 150));
    is->onWidgetTapAndHoldFinished(gestureEvent, widget);
    QCOMPARE(moveWidgetSpy.count(), 1);
    moveWidgetSpy.clear();
    is->onWidgetMoveUpdated(QPointF(355, 150), widget);
    is->onWidgetMoveUpdated(QPointF(150, 150), widget);
    
    // move widget back to first page
    is->onWidgetMoveUpdated(QPointF(10, 150), widget);
    is->onWidgetMoveUpdated(QPointF(150, 150), widget);
    
    // move widget to second page
    is->onWidgetMoveUpdated(QPointF(350, 150), widget);
    is->onWidgetMoveUpdated(QPointF(150, 150), widget);

    // try to add page without releasing mouse button
    is->onWidgetMoveUpdated(QPointF(350, 150), widget);
    is->onWidgetMoveUpdated(QPointF(150, 150), widget);
    is->onWidgetMoveFinished(QPointF(150, 150), widget);
    QCOMPARE(waitInputSpy.count(), 1);
    waitInputSpy.clear();

    // add page 3
    gesture->setHotSpot(QPointF(150, 150));
    is->onWidgetTapAndHoldFinished(gestureEvent, widget);
    QCOMPARE(moveWidgetSpy.count(), 1);
    moveWidgetSpy.clear();
    is->onWidgetMoveUpdated(QPointF(350, 150), widget);
    is->onWidgetMoveUpdated(QPointF(150, 150), widget);
    is->onWidgetMoveFinished(QPointF(150, 150), widget);
    QCOMPARE(waitInputSpy.count(), 1);
    waitInputSpy.clear();

    // try to move widget over trash bin
    gesture->setHotSpot(QPointF(150, 150));
    is->onWidgetTapAndHoldFinished(gestureEvent, widget);
    QCOMPARE(moveWidgetSpy.count(), 1);
    moveWidgetSpy.clear();
    is->onWidgetMoveUpdated(QPointF(180, 600), widget);
    is->onWidgetMoveUpdated(QPointF(180, 605), widget);
    is->onWidgetMoveUpdated(QPointF(180, 610), widget);
    is->onWidgetMoveUpdated(QPointF(180, 615), widget);
    is->onWidgetMoveUpdated(QPointF(180, 620), widget);
    is->onWidgetMoveUpdated(QPointF(180, 625), widget);
    is->onWidgetMoveUpdated(QPointF(180, 630), widget);
    is->onWidgetMoveUpdated(QPointF(180, 635), widget);
    is->onWidgetMoveUpdated(QPointF(150, 150), widget);
    is->onWidgetMoveFinished(QPointF(150, 150), widget);
    QCOMPARE(waitInputSpy.count(), 1);
    waitInputSpy.clear();
    
    is->pageChangeAnimationFinished();
  

    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStateDeleteWidget()
{
    createSceneAndWindow(2, 0, 3);
    QStateMachine *sm = new QStateMachine;
    HsIdleState *is = new HsIdleState;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    //QApplication::processEvents();
    QCoreApplication::sendPostedEvents();
    // try to move widget left from page 1, which isn't possible
    HbTapAndHoldGesture *gesture = new HbTapAndHoldGesture;
    gesture->setHotSpot(QPointF(10, 100));
    QList<QGesture *> gestures;
    gestures << gesture;
    QGestureEvent *gestureEvent = new QGestureEvent(gestures);
    QSignalSpy moveWidgetSpy(is, SIGNAL(event_moveWidget()));
    HsScene *scene = HsScene::instance();
    HsWidgetHost *widget = scene->activePage()->widgets().first();
    is->onWidgetTapAndHoldFinished(gestureEvent, widget);
    QCOMPARE(moveWidgetSpy.count(), 1);
    moveWidgetSpy.clear();
    
    // test cases where active widget is not set and stop widget drag (should not be possible in real life)
    QSignalSpy waitDeleteSpy(is, SIGNAL(event_MoveWidgetDelete()));
    QSignalSpy waitInputSpy(is, SIGNAL(event_waitInput()));
    scene->setActiveWidget(0);
    is->onWidgetMoveFinished(QPointF(150, 150), widget);
    QCOMPARE(waitInputSpy.count(), 1);
    QCOMPARE(waitDeleteSpy.count(), 1);
    
    is->pageChangeAnimationFinished();
  

    sm->stop();
    delete sm;

    deleteSceneAndWindow();
    
    
}

void HomeScreenStatePluginTest::testIdleStateMisc()
{
    createSceneAndWindow(2, 0);

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    
    is->onAddContentFromOptionsMenuActionTriggered();
    is->onAddContentFromContextMenuActionTriggered();
    is->action_idle_orientationChanged();

    HsScene *scene = HsScene::instance();
    scene->setActiveWidget(scene->activePage()->widgets().first());
    is->updateSnapAlgorithmParameters();

    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStateChromeBehaviour()
{
    createSceneAndWindow(1, 0);
    QScopedPointer<HsIdleState> is(new HsIdleState());

    is->action_idle_setupView();

    QString newTitle("title");

    is->action_idle_setupTitle();

    is->mTitleResolver->metaObject()->invokeMethod(is->mTitleResolver,
                                                   "titleChanged",
                                                   Q_ARG(QString, newTitle));
    HbView *idleView = HsGui::instance()->idleView();

    QCOMPARE(idleView->title(), newTitle);

    QString t2("new title");

    is->action_idle_cleanupTitle();

    is->mTitleResolver->metaObject()->invokeMethod(is->mTitleResolver,
                                                   "titleChanged",
                                                   Q_ARG(QString, t2));

    QCOMPARE(idleView->title(), newTitle);

    is->action_idle_setupTitle();

    is->mTitleResolver->metaObject()->invokeMethod(is->mTitleResolver,
                                                   "titleChanged",
                                                   Q_ARG(QString, t2));

    QCOMPARE(idleView->title(), t2);

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testEventFilter()
{
	// Create with no active page
    createSceneAndWindow(1, -1);
	HsIdleState *is = new HsIdleState();
	QEvent e(QEvent::ApplicationActivate);
	is->eventFilter(0, &e);

	HsScene *s = HsScene::instance();
	HsPage *page = new HsPage();
	HsWidgetHost *wh = new HsWidgetHost(1);
	page->addNewWidget(wh);
	s->addPage(page);
	s->setActivePage(page);

	is->eventFilter(0, &e);

	is->action_idle_setupView();

	QEvent ee(QEvent::ApplicationDeactivate);
	QSignalSpy spy(is, SIGNAL(event_waitInput()));
	is->eventFilter(0, &ee);

	QCOMPARE(1, spy.count());

	deleteSceneAndWindow();
	delete is;
}

void HomeScreenStatePluginTest::testIdleStateSnapToObject()
{
    createSceneAndWindow(2, 0);

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    
    //The page has one widget, remove the widget.
    HsScene *scene = HsScene::instance();
    HsWidgetHost *widget = scene->activePage()->widgets().first();
    scene->activePage()->removeWidget(widget);

    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStateSnapToObjectSnapEffectDisabled()
{
    createSceneAndWindow(2, 0);

    HSCONFIGURATION_SET(setSnapEffectsEnabled, false);
    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
	QCoreApplication::sendPostedEvents();

    //The page has one widget, remove the widget
    HsScene *scene = HsScene::instance();
    HsWidgetHost *widget = scene->activePage()->widgets().first();
    scene->activePage()->removeWidget(widget);

    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testIdleStateSnapToObjectSnappingDisabled()
{
    createSceneAndWindow(2, 0);

    HSCONFIGURATION_SET(setSnapEnabled, false);
    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
	QCoreApplication::sendPostedEvents();

    //The page has one widget, remove the widget
    HsScene *scene = HsScene::instance();
    HsWidgetHost *widget = scene->activePage()->widgets().first();
    scene->activePage()->removeWidget(widget);

    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testOnVerticalSnapLineTimerTimeout()
{
    createSceneAndWindow(1, 0);

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    is->onVerticalSnapLineTimerTimeout();
    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testOnHorizontalSnapLineTimerTimeout()
{
    createSceneAndWindow(1, 0);

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    is->onHorizontalSnapLineTimerTimeout();
    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testShowHorizontalLine()
{
    createSceneAndWindow(1, 0);

    HsIdleState *is = new HsIdleState;

    QStateMachine *sm = new QStateMachine;
    sm->addState(is);
    sm->setInitialState(is);
    sm->start();
    QCoreApplication::sendPostedEvents();
    is->showHorizontalLine();
    sm->stop();
    delete sm;

    deleteSceneAndWindow();
}

void HomeScreenStatePluginTest::testEditorAndVKB()
{
    createSceneAndWindow(1, 0);

    HsIdleState *is = new HsIdleState;
    
    is->closeVirtualKeyboard();
    
    HsScene *scene = HsScene::instance();
    HsWidgetHost *widget = scene->activePage()->widgets().first();
    
    // scene is not set for the widgets yet.
    QVERIFY(!is->isEditor(QPointF(0,0), widget));
    
    delete is;
    
    deleteSceneAndWindow();
}
