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
#include "hshostedwidgetfactory.h"
#include "hsconfiguration.h"

void TestHsDomainModel::testHsWidgetHostVisualConstruction()
{
    HsWidgetHostVisual *visual = new HsWidgetHostVisual();

    QVERIFY(visual);
    QVERIFY(!visual->mWidget);
    delete visual;
}


void TestHsDomainModel::testHsWidgetHostVisualShape()
{
    HsWidgetData data;
    data.pageId = 0;
    data.uri = "mockwidgetplugin";    
    HsWidgetHost *host = HsWidgetHost::createInstance(data);

    host->startWidget();
    QPainterPath path = host->visual()->shape();
    QCOMPARE(path.boundingRect(),QRectF(QPointF(0,0), HsConfiguration::instance()->minimumWidgetSizeInPixels()));
    
    host->visual()->mWidget->resize(QSizeF(100,100));
    path = host->visual()->shape();
    QCOMPARE(path.boundingRect(),QRectF(0,0,100,100));
    
    
    host->remove();
    
    data.pageId = 0;
    data.uri = "mockwidgetplugintoobig";    
    host = HsWidgetHost::createInstance(data);
    host->startWidget();
    path = host->visual()->shape();
    QCOMPARE(path.boundingRect(),QRectF(QPointF(0,0), HsConfiguration::instance()->maximumWidgetSizeInPixels()));
    host->remove();
}

void TestHsDomainModel::testHsWidgetHostVisualSetWidget()
{
    QObject *widget = HsHostedWidgetFactory::instance()->createWidget("mockwidgetplugin");
    HsWidgetHostVisual *visual = new HsWidgetHostVisual();
    visual->setWidget(widget);
    QVERIFY(visual->mWidget == widget);
    delete visual;
}

void TestHsDomainModel::testHsWidgetHostVisualSetVisualModel()
{
    HsWidgetHost *host = new HsWidgetHost(-1);
    HsWidgetHostVisual *visual = new HsWidgetHostVisual();
    visual->setVisualModel(host);
    QVERIFY(visual->mVisualModel == host);
    delete visual;
    host->remove();
}

void TestHsDomainModel::testHsWidgetHostVisualEffects()
{
    HsWidgetData data;
    data.pageId = 0;
    data.uri = "mockwidgetplugin";    
    HsWidgetHost *host = HsWidgetHost::createInstance(data);

    host->startWidget();
    host->visual()->startDragEffect();
    host->visual()->startDropEffect();
    host->remove();
}


void TestHsDomainModel::testHsWidgetHostVisualEventFilter()
{
    HsWidgetHostVisual *visual = new HsWidgetHostVisual();
    QSignalSpy resizedSpy(visual, SIGNAL(resized()));
    QEvent *someEvent = new QEvent(QEvent::ApplicationActivated);
    visual->eventFilter(0, someEvent);
    QCOMPARE(resizedSpy.count(), 0);
    delete visual;
    delete someEvent;
}


void TestHsDomainModel::testHsWidgetHostVisualGestureEvent()
{
    HsWidgetHostVisual *visual = new HsWidgetHostVisual();
    QList<QGesture*> gestureList;

    // test empty gesture list
    visual->gestureEvent(new QGestureEvent(gestureList));

    // test pan
    HbPanGesture *panGesture = new HbPanGesture();
    gestureList << panGesture;
    QGestureEvent *gestureEvent = new QGestureEvent(gestureList);
    visual->gestureEvent(gestureEvent);

    delete panGesture;
    delete visual;
}

void TestHsDomainModel::testHsWidgetHostVisualSetupTouchArea()
{
    HsWidgetHostVisual *visual = new HsWidgetHostVisual();
    visual->setupTouchArea();
    delete visual;
}

void TestHsDomainModel::testHsWidgetHostVisualSetupEffects()
{
    HsWidgetHostVisual *visual = new HsWidgetHostVisual();
    visual->setupEffects();
    delete visual;
}

void TestHsDomainModel::testHsWidgetHostVisualSetNewSize()
{
    HsWidgetHostVisual *visual = new HsWidgetHostVisual();
    visual->setNewSize(QSizeF(100,100));
    QVERIFY(visual->size() == QSizeF(100,100));
    delete visual;
}


