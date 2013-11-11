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

#include <HbMainWindow>
#include <HbInstance>
#include <HbView>

#include "t_hsdomainmodel.h"
#include "hspage.h"
#include "hsscene.h"
#include "hsdomainmodeldatastructures.h"
#include "hswidgethost.h"
#include "hswidgetpositioningonwidgetadd.h"
#include "hswidgetcomponent.h"
#include "hsconfiguration.h"
#include "hsgui.h"
#include "hspagevisual.h"
#include "hswidgetpositioningonorientationchange.h"

void TestHsDomainModel::testHsPage()
{
    HsPageData pageData;
    HsPage *page=HsPage::createInstance(pageData);
    QVERIFY(page->databaseId());
    QVERIFY(page->load());
    QCOMPARE(page->widgets().count(),0);

    // we haven't added this to scene.
    QCOMPARE(page->pageIndex(),-1);

    QVERIFY(page->isRemovable());
    page->setRemovable(false);
    QVERIFY(!page->isRemovable());
    page->setRemovable(true);
    QVERIFY(page->isRemovable());
    delete page;
}


void TestHsDomainModel::testHsPageLoad()
{
    HsPage *page = new HsPage();
    QVERIFY(!page->load());
    delete page;

    HsPageData pageData;
    pageData.indexPosition = 0;
    page = HsPage::createInstance(pageData);
    QVERIFY(page);
    int pageId = page->databaseId();
    delete page;

    page = new HsPage();
    page->setDatabaseId(pageId);
    QVERIFY(page->load());
    QVERIFY(page->wallpaper());
    QVERIFY(page->deleteFromDatabase());
    delete page;

    page = HsPage::createInstance(pageData);
    QVERIFY(page);
    pageId = page->databaseId();
    delete page;
    page = new HsPage();
    page->setDatabaseId(pageId);
    QVERIFY(page->load());

    QVERIFY(page->deleteFromDatabase());
    delete page;

}

void TestHsDomainModel::testHsPageContentRects()
{
    HsPageData pageData;
    pageData.indexPosition = 0;
    HsPage *page = HsPage::createInstance(pageData);
    QVERIFY(page->databaseId());
    QVERIFY(page->load());
    QCOMPARE(page->widgets().count(),0);
    HsScene::instance()->addPage(page);
    HsGui *stored = HsGui::takeInstance();
    
    HbInstance::instance();
    HbMainWindow mainWindow;        
    HbView *view = mainWindow.addView(page->visual());
    mainWindow.views().first()->setContentFullScreen();
    mainWindow.show();
    HsGui *gui = HsGui::instance();
    // crashes symbian test
#ifndef Q_OS_SYMBIAN    
    QEventLoop waitUntilMainWindowCreated;
    QTimer eventLoopTimer;
    eventLoopTimer.setInterval(15000);
    eventLoopTimer.setSingleShot(true);    
    connect(&mainWindow, SIGNAL(viewReady()),
            &waitUntilMainWindowCreated, SLOT(quit()));
    connect(&eventLoopTimer, SIGNAL(timeout()),
            &waitUntilMainWindowCreated, SLOT(quit()));
    eventLoopTimer.start();
    waitUntilMainWindowCreated.exec();
    eventLoopTimer.stop();
#endif        
    QRectF layoutRect = mainWindow.layoutRect();
    qreal pageMargin = HSCONFIGURATION_GET(pageMargin);
    QRectF expectecRect;

    //Test global rect
    expectecRect = layoutRect;
    expectecRect.setTop(64);
    expectecRect.adjust(pageMargin, pageMargin, -pageMargin, -pageMargin);
    QRectF contentGeometry = page->contentGeometry();
    QCOMPARE(contentGeometry, expectecRect);

    //Test global rect at different orientation - improves branch coverage
    expectecRect = layoutRect;
    expectecRect.setWidth(layoutRect.height());
    expectecRect.setHeight(layoutRect.width());
    expectecRect.setTop(64);
    expectecRect.adjust(pageMargin, pageMargin, -pageMargin, -pageMargin);
    Qt::Orientation orientation =
        //HsScene::instance()->mainWindow()->orientation() ==
        gui->orientation() ==
            Qt::Vertical ? Qt::Horizontal : Qt::Vertical;
    contentGeometry = page->contentGeometry(orientation);
    QCOMPARE(contentGeometry, expectecRect);

    //Test local rect
    expectecRect = layoutRect;
    expectecRect.setTop(64);
    expectecRect.moveTopLeft(QPointF(0,0));
    expectecRect.adjust(pageMargin, pageMargin, -pageMargin, -pageMargin);
    QRectF contentRect = page->contentRect();
    QCOMPARE(contentRect, expectecRect);
    
    mainWindow.hide();
    mainWindow.removeView(view);
    delete HsGui::takeInstance();
    if(stored) {
        HsGui::setInstance(stored);
    }
    HsScene::instance()->removePage(page);
    delete page;
}

void TestHsDomainModel::testHsPageAddRemoveWidget()
{
    HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsAnchorPointInBottomRight);

    HsPageData pageData;
    pageData.indexPosition = 0;
    HsPage *page = HsPage::createInstance(pageData);
    QVERIFY(page);
    int pageId = page->databaseId();

    HsWidgetData widgetData;
    widgetData.pageId = pageId;
    widgetData.uri = "uri1";
    HsWidgetHost *widget = HsWidgetHost::createInstance(widgetData);
    QVERIFY(widget);

    widget->setPage(page);
    QVERIFY(!page->addNewWidget(0));
    QVERIFY(page->addNewWidget(widget));
    QVERIFY(page->addNewWidget(widget));
    QCOMPARE(page->newWidgets().count(), 1);
    QCOMPARE(page->widgets().count(), 0);

    page->layoutNewWidgets();
    QCOMPARE(page->newWidgets().count(), 0);
    QCOMPARE(page->widgets().count(), 1);
    QCOMPARE(page->newWidgets().count(), 0);
    QCOMPARE(page->widgets().count(), 1);  
    page->resetNewWidgets();  

    HsWidgetHost *widget2 = HsWidgetHost::createInstance(widgetData);
    QVERIFY(widget2);
    widget2->setPage(page);
    QVERIFY(page->addNewWidget(widget2));
    QCOMPARE(page->newWidgets().count(),1);
    QCOMPARE(page->widgets().count(),1);

    // test layouting for landscape orientation
    HsGui::instance()->setOrientation(Qt::Horizontal);
    // active widget for branch testing
    HsScene::instance()->setActiveWidget(widget);
    page->layoutNewWidgets();
    // set active widget to original
    HsScene::instance()->setActiveWidget(0);

    QCOMPARE(page->newWidgets().count(), 0);
    QCOMPARE(page->widgets().count(), 2);
    QCOMPARE(page->newWidgets().count(), 0);
    HsGui::instance()->setOrientation(Qt::Vertical);

    page->layoutNewWidgets();
    QCOMPARE(page->newWidgets().count(), 0);
    QCOMPARE(page->widgets().count(), 2);

    QVERIFY(!page->removeWidget(0));
    QVERIFY(page->removeWidget(widget2));

    delete page;

    page = new HsPage();
    page->setDatabaseId(pageId);
    QVERIFY(page->load());
    QVERIFY(page->deleteFromDatabase());
    delete page;

    HsWidgetPositioningOnWidgetAdd::setInstance(0);

}


void TestHsDomainModel::testHsPageAddExistingWidget()
{
    HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsAnchorPointInBottomRight);

    HsPageData pageData;

    pageData.indexPosition = 0;
    HsPage *page = HsPage::createInstance(pageData);
    QVERIFY(page);
    int pageId = page->databaseId();

    pageData.indexPosition = 1;
    HsPage *page2 = HsPage::createInstance(pageData);
    QVERIFY(page2);
    int pageId2 = page2->databaseId();

    HsWidgetData widgetData;
    widgetData.pageId = pageId;
    widgetData.uri = "mockwidgetplugin";

    HsWidgetHost *widget = HsWidgetHost::createInstance(widgetData);
    QVERIFY(widget);
    widget->setPage(page);
    QVERIFY(page->addNewWidget(widget));

    HsWidgetHost *widget2 = HsWidgetHost::createInstance(widgetData);
    QVERIFY(widget2);
    widget2->setPage(page);
    QVERIFY(page->addNewWidget(widget2));

    // coverages up with active widget
    HsScene::instance()->setActiveWidget(widget);
    page->layoutNewWidgets();
    QCOMPARE(page->widgets().count(), 2);
    HsScene::instance()->setActiveWidget(0);

    QVERIFY(page->removeWidget(widget2));
    QVERIFY(page2->addExistingWidget(widget2));
    QVERIFY(page2->addExistingWidget(widget2));

    QVERIFY(!page->addExistingWidget(NULL));

    delete page;

    page = new HsPage();
    page->setDatabaseId(pageId);
    QVERIFY(page->load());
    // Widget doesn't have presentation yet.    
    QCOMPARE(page->widgets().count(), 0); 
    QCOMPARE(page->mNewWidgets.count(), 1);
    page2 = new HsPage();
    page2->setDatabaseId(pageId2);
    QVERIFY(page2->load());
    // Widget doesn't have presentation yet.
    QCOMPARE(page2->widgets().count(), 0);
    QCOMPARE(page2->mNewWidgets.count(), 1);
    delete page;
    delete page2;

    HsWidgetPositioningOnWidgetAdd::setInstance(0);

}


void TestHsDomainModel::testHsPageWidgetFinished()
{
    HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsAnchorPointInBottomRight);

    HsPageData pageData;
    HsPage *page=HsPage::createInstance(pageData);
    QVERIFY(page);
    int pageId = page->databaseId();
    HsScene::instance()->mActivePage = page;

    HsWidgetData widgetData;
    widgetData.pageId = pageId;
    widgetData.uri = "mockwidgetplugin";

    HsWidgetHost *widget = HsWidgetHost::createInstance(widgetData);
    QVERIFY(widget);
    widget->setPage(page);
    widget->startWidget();
    QVERIFY(widget->mStateMachine->isRunning());
    QVERIFY(page->addNewWidget(widget));
    page->layoutNewWidgets();

    widget->onFinished();
    qApp->processEvents();

    QCOMPARE(page->newWidgets().count(),0);
    QCOMPARE(page->widgets().count(),0);

    // new widget
    widget = HsWidgetHost::createInstance(widgetData);
    QVERIFY(widget);
    widget->setPage(page);
    widget->startWidget();
    QVERIFY(widget->mStateMachine->isRunning());
    QVERIFY(page->addNewWidget(widget));
    QCOMPARE(page->newWidgets().count(),1);
    QCOMPARE(page->widgets().count(),0);

    widget->onFinished();
    qApp->processEvents();

    QCOMPARE(page->newWidgets().count(),0);
    QCOMPARE(page->widgets().count(),0);

    delete page;

    HsWidgetPositioningOnWidgetAdd::setInstance(0);
}

void TestHsDomainModel::testHsPageIsActivePage()
{
    HsPageData pageData;
    pageData.indexPosition = 0;
    HsPage *page = HsPage::createInstance(pageData);
    QVERIFY(page);

    QVERIFY(!page->isActivePage());
    QVERIFY(page->isDefaultPage());
    delete page;
}

void TestHsDomainModel::testHsPageWidgetSignals()
{
    HsPageData pageData;
    HsPage *page = HsPage::createInstance(pageData);
    QVERIFY(page);

    HsWidgetData widgetData;
    widgetData.pageId = page->databaseId();
    widgetData.uri = "mockwidgetplugin";
    HsWidgetHost *widget = HsWidgetHost::createInstance(widgetData);
    QVERIFY(widget);

    page->addExistingWidget(widget);
    widget->setPage(page);
    widget->startWidget();

    QCOMPARE(page->mWidgets.count(), 1);
    QCOMPARE(page->mUnavailableWidgets.count(), 0);

    emit widget->mComponent->unavailable();

    QCOMPARE(page->mWidgets.count(), 0);
    QCOMPARE(page->mUnavailableWidgets.count(), 1);

    emit widget->mComponent->available();

    QCOMPARE(page->mWidgets.count(), 1);
    QCOMPARE(page->mUnavailableWidgets.count(), 0);

    // test unavailability when new widgets exists
    HsWidgetHost *widget2 = HsWidgetHost::createInstance(widgetData);
    page->addNewWidget(widget2);
    widget2->setPage(page);
    widget2->startWidget();
    QCOMPARE(page->mWidgets.count(), 1);
    QCOMPARE(page->mNewWidgets.count(), 1);
    QCOMPARE(page->mUnavailableWidgets.count(), 0);
    emit widget2->mComponent->unavailable();
    QCOMPARE(page->mWidgets.count(), 0);
    QCOMPARE(page->mNewWidgets.count(), 0);
    QCOMPARE(page->mUnavailableWidgets.count(), 2);
    page->removeWidget(widget);
    page->removeWidget(widget2);

    delete page;
}

void TestHsDomainModel::testHsPageOrientationChange()
{
   HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsAnchorPointInBottomRight);

    HsWidgetPositioningOnOrientationChange::setInstance(
        new HsAdvancedWidgetPositioningOnOrientationChange);

    HsPageData pageData;
    HsPage *page = HsPage::createInstance(pageData);
    QVERIFY(page);
    
    // test when no widgets on page
    page->onOrientationChanged(Qt::Horizontal);

    // add existing widget
    HsWidgetData widgetData;
    widgetData.pageId = page->databaseId();
    widgetData.uri = "mockwidgetplugin";
    HsWidgetHost *existingWidget = HsWidgetHost::createInstance(widgetData);
    QVERIFY(existingWidget);
    page->addExistingWidget(existingWidget);

    // add new widget without presentation
    HsWidgetData widgetData2;
    widgetData2.pageId = page->databaseId();
    widgetData2.uri = "mockwidgetplugintoosmall";
    HsWidgetHost *newWidget = HsWidgetHost::createInstance(widgetData2);
    QVERIFY(newWidget);
    page->addNewWidget(newWidget);
    newWidget->removePresentation(Qt::Horizontal);

    page->onOrientationChanged(Qt::Horizontal);
    page->onOrientationChanged(Qt::Vertical);

    delete page;
    HsWidgetPositioningOnOrientationChange::setInstance(0);
    HsWidgetPositioningOnWidgetAdd::setInstance(0);
}

void TestHsDomainModel::testHsPageDeleteFromDatabase()
{
    HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsAnchorPointInBottomRight);

    HsPageData pageData;
    pageData.indexPosition = 0;
    HsPage *page = HsPage::createInstance(pageData);
    HsWidgetData widgetData;
    widgetData.pageId = page->databaseId();
    widgetData.uri = "mockwidgetplugin";
    HsWidgetHost *widget = HsWidgetHost::createInstance(widgetData);

    // new widgets not deleted
    page->addNewWidget(widget);
    QCOMPARE(page->newWidgets().count(), 1);
    page->deleteFromDatabase();
    QCOMPARE(page->newWidgets().count(), 0);

    // widgets not deleted
    page->addNewWidget(widget);

    // test set online when new widgets exists
    page->setOnline();
    page->layoutNewWidgets();
    // test show widgets
    page->showWidgets();
    // test hide widgets
    page->hideWidgets();
    // test set online when widgets exists
    page->setOnline();

    QCOMPARE(page->mWidgets.count(), 1);
    QVERIFY(page->deleteFromDatabase());
    QCOMPARE(page->mWidgets.count(), 0);

    // unavailable widgets not deleted
    page->mUnavailableWidgets.append(widget);
    QCOMPARE(page->mUnavailableWidgets.count(), 1);
    QVERIFY(page->deleteFromDatabase());
    QCOMPARE(page->mUnavailableWidgets.count(), 0);

    delete widget;
    delete page;
    
}

void TestHsDomainModel::testHsPageOnPageMarginChanged()
{
    HsWidgetPositioningOnWidgetAdd::setInstance(
        new HsAnchorPointInBottomRight);


    //Create empty page
    HsPageData pageData;
    pageData.indexPosition = 0;
    HsPage *page = HsPage::createInstance(pageData);
    QVERIFY(page);
    int pageId = page->databaseId();
    page->onPageMarginChanged("dummy");
    page->onPageMarginChanged("pageMargin"); //No widgets

    //Add first widget
    HsWidgetData widgetData;
    widgetData.pageId = pageId;
    widgetData.uri = "uri1";
    HsWidgetHost *widget = HsWidgetHost::createInstance(widgetData);
    QVERIFY(widget);
    widget->setPage(page);
    QVERIFY(!page->addNewWidget(0));
    QVERIFY(page->addNewWidget(widget));
    QVERIFY(page->addNewWidget(widget));
    QCOMPARE(page->newWidgets().count(), 1);
    QCOMPARE(page->widgets().count(), 0);
    page->onPageMarginChanged("pageMargin"); // Only mNewWidgets
    page->layoutNewWidgets();
    QCOMPARE(page->newWidgets().count(), 0);
    QCOMPARE(page->widgets().count(), 1);
    QCOMPARE(page->newWidgets().count(), 0);
    QCOMPARE(page->widgets().count(), 1);  
    page->resetNewWidgets();   

    //Add second widget
    HsWidgetHost *widget2 = HsWidgetHost::createInstance(widgetData);
    QVERIFY(widget2);
    widget2->setPage(page);
    QVERIFY(page->addNewWidget(widget2));
    QCOMPARE(page->newWidgets().count(),1);
    QCOMPARE(page->widgets().count(),1);
    page->onPageMarginChanged("pageMargin"); // Both mWidgets and mNewWidgets
    page->layoutNewWidgets();
    page->onPageMarginChanged("pageMargin"); //Only mWidgets
    QCOMPARE(page->newWidgets().count(), 0);
    QCOMPARE(page->widgets().count(), 2);

    delete page;

    HsWidgetPositioningOnWidgetAdd::setInstance(0);
}

void TestHsDomainModel::testHsPageIndex()
{
    HsPageData pageData;
    pageData.indexPosition = 0;
    HsPage *page = HsPage::createInstance(pageData);
    QVERIFY(page->load());
    QCOMPARE(page->mPageVisual->pageIndex(),-1);
    HsScene::instance()->addPage(page);
    QCOMPARE(page->mPageVisual->pageIndex(),0);

    pageData.indexPosition = 1;
    HsPage *page2 = HsPage::createInstance(pageData);
    QVERIFY(page2->load());
    HsScene::instance()->addPage(page2);
    QCOMPARE(page2->mPageVisual->pageIndex(),1);
    
    HsScene::instance()->removePage(page);
    delete page;
    HsScene::instance()->removePage(page2);
    delete page2;
}

