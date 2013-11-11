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


#include <QGraphicsWidget>
#include <QSqlDatabase>
#include <HbInstance>
#include <HbPanGesture>

#include "t_hsdomainmodel.h"
#include "hsdomainmodeldatastructures.h"
#include "hswidgethost.h"
#include "hswidgethostvisual.h"
#include "hspage.h"
#include "hsscene.h"
#include "hswidgettoucharea.h"

void TestHsDomainModel::testHsWidgetHostConstruction()
{
    HsWidgetData data;
    data.pageId = 0;
    data.uri = "mockwidgetplugin";    
    HsWidgetHost *host = HsWidgetHost::createInstance(data);

    QVERIFY(host);   
    QVERIFY(!host->mPage);
    QVERIFY(!host->page());
    QVERIFY(!host->mWidget);
    QVERIFY(!host->mIsFinishing);
    QVERIFY(!host->mComponent);
    QCOMPARE(host->mDatabaseId, 1);
    QCOMPARE(host->databaseId(), 1);
    QVERIFY(host->mStateMachine);
    QVERIFY(!host->mStateMachine->isRunning());

    delete host;
}

void TestHsDomainModel::testHsWidgetHostStateMachine()
{
    HsWidgetData data;
    data.pageId = 0;
    data.uri = "mockwidgetplugin";    
    HsWidgetHost *host = HsWidgetHost::createInstance(data);
    QVERIFY(host);

    // State machine events.
    QSignalSpy event_startAndShowSpy(host, SIGNAL(event_startAndShow()));
    QSignalSpy event_startAndHideSpy(host, SIGNAL(event_startAndHide()));
    QSignalSpy event_unloadSpy(host, SIGNAL(event_unload()));
    QSignalSpy event_showSpy(host, SIGNAL(event_show()));
    QSignalSpy event_hideSpy(host, SIGNAL(event_hide()));
    QSignalSpy event_removeSpy(host, SIGNAL(event_remove()));
    QSignalSpy event_closeSpy(host, SIGNAL(event_close()));
    QSignalSpy event_finishedSpy(host, SIGNAL(event_finished()));
    QSignalSpy event_faultedSpy(host, SIGNAL(event_faulted()));

    QSignalSpy sm_finished(host->mStateMachine, SIGNAL(finished()));

    // Host events.
    QSignalSpy faultedSpy(host, SIGNAL(faulted()));
    QSignalSpy finishedSpy(host, SIGNAL(finished()));
        
    
    host->startWidget();

    QCOMPARE(event_startAndShowSpy.count(), 1);
    event_startAndShowSpy.clear();
    
    QCOMPARE(faultedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 0);
    
    QVERIFY(!host->mIsFinishing);
    QVERIFY(host->mComponent);
    QVERIFY(host->mWidget);

    host->hideWidget();

    QCOMPARE(event_hideSpy.count(), 1);
    event_hideSpy.clear();

    host->showWidget();

    QCOMPARE(event_showSpy.count(), 1);
    event_showSpy.clear();

    host->remove(); // Calls deleteLater()

    QCOMPARE(event_removeSpy.count(), 1);
    event_removeSpy.clear();

    QCOMPARE(sm_finished.count(), 1);
    sm_finished.clear();
}

void TestHsDomainModel::testHsWidgetHostCloseAndRemove()
{
    HsWidgetData widgetData;
    widgetData.pageId = 0;
    widgetData.uri = "mockwidgetplugin";    
    
    {
    HsWidgetHost *host = HsWidgetHost::createInstance(widgetData);
    QVERIFY(host);
    QSignalSpy event_removeSpy(host, SIGNAL(event_remove()));
    QSignalSpy event_closeSpy(host, SIGNAL(event_close()));

    host->close();
    QCOMPARE(event_removeSpy.count(), 0);
    QCOMPARE(event_closeSpy.count(), 0);
    }

    {
    HsWidgetHost *host = HsWidgetHost::createInstance(widgetData);
    QVERIFY(host);
    QSignalSpy event_removeSpy(host, SIGNAL(event_remove()));
    QSignalSpy event_closeSpy(host, SIGNAL(event_close()));
    
    host->remove();
    QCOMPARE(event_removeSpy.count(), 0);
    QCOMPARE(event_closeSpy.count(), 0);
    }

    {
    HsWidgetHost *host = HsWidgetHost::createInstance(widgetData);
    QVERIFY(host);
    QSignalSpy event_removeSpy(host, SIGNAL(event_remove()));
    QSignalSpy event_closeSpy(host, SIGNAL(event_close()));
    
    host->startWidget(false);
    host->close();
    QCOMPARE(event_removeSpy.count(), 0);
    QCOMPARE(event_closeSpy.count(), 1);
    }

    {
    HsWidgetHost *host = HsWidgetHost::createInstance(widgetData);
    QVERIFY(host);
    QSignalSpy event_removeSpy(host, SIGNAL(event_remove()));
    QSignalSpy event_closeSpy(host, SIGNAL(event_close()));

    host->startWidget();
    host->startWidget();
    host->remove();
    QCOMPARE(event_removeSpy.count(), 1);
    QCOMPARE(event_closeSpy.count(), 0);
    }
}

void TestHsDomainModel::testHsWidgetHostPage()
{
    HsWidgetData widgetData;
    widgetData.pageId = 0;
    widgetData.uri = "mockwidgetplugin";    
    HsWidgetHost *host = HsWidgetHost::createInstance(widgetData);
    QVERIFY(host);

    QVERIFY(!host->page());

    HsPageData pageData;
    pageData.indexPosition = 0;
    HsPage *page = HsPage::createInstance(pageData);
    QVERIFY(page);

    QVERIFY(host->setPage(page));
    QVERIFY(host->mPage == page);
    QVERIFY(host->page() == page);

    QVERIFY(host->setPage(0));
    QVERIFY(!host->mPage);
    QVERIFY(!host->page());

    host->mDatabaseId = -1;
    QVERIFY(!host->setPage(page));
    QVERIFY(!host->mPage);
    QVERIFY(!host->page());

    delete page;

    delete host;
}
 
void TestHsDomainModel::testHsWidgetHostEffects()
{
    HsWidgetHost *host = new HsWidgetHost(-1);
    host->startDragEffect();
    host->startDropEffect();
}

void TestHsDomainModel::testHsWidgetHostShowAndHideActions()
{
    HsWidgetHost *host = new HsWidgetHost(-1);
    host->action_show();
    host->action_hide();
    host->mIsFinishing = true;
    host->action_show();
    host->action_hide();
    delete host;
}

void TestHsDomainModel::testHsWidgetHostPresentation()
{
    HsWidgetData widgetData;
    widgetData.pageId = 0;
    widgetData.uri = "mockwidgetplugin";    
    HsWidgetHost *host = HsWidgetHost::createInstance(widgetData);
    QVERIFY(host);
    host->startWidget();

    HsWidgetPresentationData data;
    data.orientation = Qt::Vertical;
    data.x = data.y = data.zValue = 0;
    
    QVERIFY(host->savePresentation(data));    
    QVERIFY(host->getPresentation(data));

    host->removePresentation(Qt::Vertical);

    QVERIFY(!host->getPresentation(data));

    host->remove();
}

void TestHsDomainModel::testHsWidgetHostOnError()
{
    HsWidgetHost *host = new HsWidgetHost(-1);    
    QSignalSpy event_faultedSpy(host, SIGNAL(event_faulted()));
    host->onError();
    QCOMPARE(event_faultedSpy.count(), 1);
    QVERIFY(host->mIsFinishing);
    delete host;
}

void TestHsDomainModel::testHsWidgetHostOnSetPreferences()
{
    HsWidgetData widgetData;
    widgetData.pageId = 0;
    widgetData.uri = "mockwidgetplugin";    
    HsWidgetHost *host = HsWidgetHost::createInstance(widgetData);
    QVERIFY(host);
    host->startWidget();
    host->onSetPreferences(QStringList());
    host->onSetPreferences(QStringList() << "mypref");
    host->remove();
}

void TestHsDomainModel::testHsWidgetHostShape()
{
    HsWidgetData widgetData;
    widgetData.pageId = 0;
    widgetData.uri = "mockwidgetplugin";    
    HsWidgetHost *host = HsWidgetHost::createInstance(widgetData);
    QVERIFY(host);
    host->startWidget();        
    host->remove();
}

void TestHsDomainModel::testHsWidgetHostShapeInvalidValues()
{
    HsWidgetData widgetData;
    widgetData.pageId = 0;
    widgetData.uri = "mockwidgetplugincheater";    
    HsWidgetHost *host = HsWidgetHost::createInstance(widgetData);
    QVERIFY(host);	
    host->startWidget();
    host->remove();
}

void TestHsDomainModel::testHsWidgetHostGestureEvent()
{
    HsWidgetHost *host = new HsWidgetHost(-1); 
    QList<QGesture*> gestureList;

    // test pan
    HbPanGesture *panGesture = new HbPanGesture();
    gestureList << panGesture;
    QGestureEvent *gestureEvent = new QGestureEvent(gestureList);
    Qt::GestureState state = panGesture->state();
    host->visual()->gestureEvent(gestureEvent);

    delete panGesture;
    delete host;
}

