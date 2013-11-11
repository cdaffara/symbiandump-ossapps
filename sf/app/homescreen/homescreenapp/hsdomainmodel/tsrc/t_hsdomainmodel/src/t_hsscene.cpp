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

#include "t_hsdomainmodel.h"
#include "hsscene.h"
#include "hsdomainmodeldatastructures.h"
#include "hswidgethost.h"
#include "hspage.h"
#include "hsconfiguration.h"

void TestHsDomainModel::testSceneDefaultValues()
{    
    HsScene *scene = new HsScene;
    
    QCOMPARE(scene->databaseId(), -1); 
    QCOMPARE(scene->pages().count(), 0);
    QVERIFY(scene->isOnline()); 
    
    QVERIFY(!scene->wallpaper());
    
    QVERIFY(!scene->activeWidget()); 
    QVERIFY(!scene->activePage()); 
    QCOMPARE(scene->activePageIndex(), -1);     
    QVERIFY(!scene->setActivePageIndex(1)); 
    QVERIFY(!scene->setActivePageIndex(-1)); 
    QVERIFY(!scene->setActivePage(0)); 
    
    QVERIFY(!scene->removePage(0)); 
    QVERIFY(!scene->addPage(0)); 
    
    delete scene;
}

void TestHsDomainModel::testSceneStaticInterface()
{    
    HsScene *stored = HsScene::takeInstance();
    
    HsScene *scene = new HsScene;
    QVERIFY(!HsScene::instance());
    HsScene::setInstance(scene);
    QVERIFY(HsScene::instance() == scene);
    HsScene::setInstance(scene);
    QVERIFY(HsScene::instance() == scene);
    HsScene *scene2 = HsScene::takeInstance();
    QVERIFY(!HsScene::instance());
    QVERIFY(scene == scene2);
    delete scene;
    
    HsScene::setInstance(stored);
}

void TestHsDomainModel::testSceneLoad()
{
    // no page
    HsScene *scene = HsScene::instance();
    QVERIFY(!scene->load());

    createPagesToDatabase(1);
    scene = HsScene::instance();
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::SceneWallpaper);
    QVERIFY(scene->load());
    QVERIFY(scene->wallpaper());
    // set configuration back to original
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::PageWallpapers);
    destroySceneAndPages();
}


void TestHsDomainModel::testSceneAddPages()
{
    createSceneWithPages(0, 0);
    HsScene *scene = HsScene::instance();    
    HsPage *page = new HsPage();
    QVERIFY(!scene->addPage(page)); 
    delete page;
    
    HsPageData pageData;
    pageData.indexPosition = 0;
    page = HsPage::createInstance(pageData);
    QVERIFY(scene->addPage(page)); 
    QVERIFY(scene->setActivePage(page)); 
    QVERIFY(scene->setActivePageIndex(0)); 
    QVERIFY(scene->addPage(page)); 
    QCOMPARE(scene->pages().count(), 1);

    // add page to left
    HsPageData pageData2;
    pageData2.indexPosition = 0;
    HsPage *page2 = HsPage::createInstance(pageData2);
    QVERIFY(scene->addPage(page2));
    QCOMPARE(scene->pages().count(), 2);
    QVERIFY(scene->removePage(page2));
    QCOMPARE(scene->pages().count(), 1);

    int dbId = page->databaseId();
    page->setDatabaseId(dbId-1);
    QVERIFY(scene->removePage(page)); 
    page->setDatabaseId(dbId);
    QVERIFY(scene->removePage(page)); 
    QVERIFY(!scene->setActivePage(page)); 
    QCOMPARE(scene->pages().count(), 0);
    
    delete HsScene::mInstance;
    HsScene::mInstance = 0;
}


void TestHsDomainModel::testSceneOnline()
{
    createSceneWithPages(3,0);
    HsScene *scene = HsScene::instance();
    
    HsWidgetData widgetData;
    widgetData.pageId = HsScene::instance()->activePage()->databaseId();
    widgetData.uri = "mockwidgetplugin";
    
    HsWidgetHost *widget = HsWidgetHost::createInstance(widgetData);
    QVERIFY(widget);
    widget->setPage(HsScene::instance()->activePage());
    widget->startWidget();
    QVERIFY(widget->mStateMachine->isRunning());
    HsScene::instance()->activePage()->addNewWidget(widget);
    
    QVERIFY(widget->mWidget->property("isOnline").isValid());
    QVERIFY(widget->mWidget->property("isOnline").toBool());
    scene->setOnline(true);
    QVERIFY(widget->mWidget->property("isOnline").toBool());
    scene->setOnline(false);
    QVERIFY(!widget->mWidget->property("isOnline").toBool());
    
    QVERIFY(!scene->activeWidget());
    scene->setActiveWidget(widget);
    QCOMPARE(scene->activeWidget(), widget);
    
    delete HsScene::mInstance;
    HsScene::mInstance = 0;
}

void TestHsDomainModel::testSceneEventFilter()
{
    createSceneWithPages(3, 0);
    HsScene *scene = HsScene::instance();
    
    QEvent activateEvent(QEvent::ApplicationActivate);
    QEvent deactivateEvent(QEvent::ApplicationDeactivate);
    QEvent randomEvent(QEvent::AccessibilityHelp);

    scene->eventFilter(0, &activateEvent);
    scene->eventFilter(0, &deactivateEvent);
    scene->eventFilter(0, &randomEvent);

    destroySceneAndPages();
}
