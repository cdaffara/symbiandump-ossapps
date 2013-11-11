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

#include "hsscene.h"
#include "hsgui.h"
#include "hspage.h"
#include "hspageindicator.h"
#include "hsconfiguration.h"
#include "hsidlewidget.h"
#include "t_hsdomainmodel.h"
#include "hsdomainmodeldatastructures.h"

void TestHsDomainModel::testIdleWidgetConstruction()
{
    HsIdleWidget *iw = new HsIdleWidget();
    delete iw;
}

void TestHsDomainModel::testIdleWidgetSetGeometry()
{
    HsIdleWidget *iw = new HsIdleWidget();
    
    iw->setGeometry(QRectF(0,0,100,100));
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::SceneWallpaper);
    iw->setGeometry(QRectF(0,0,100,100));
    delete iw;    
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::PageWallpapers);
}

void TestHsDomainModel::testIdleWidgetSetSceneX()
{
    HsIdleWidget *iw = new HsIdleWidget();
    iw->setSceneX(100);
    QVERIFY(iw->sceneX() == 100);
    
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::SceneWallpaper);
    iw->setSceneX(200);
    QVERIFY(iw->sceneX() == 200);
    
    delete iw;    
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::PageWallpapers);
}

void TestHsDomainModel::testIdleWidgetSetActivePage()
{
    HsIdleWidget *iw = new HsIdleWidget();
    iw->mPageIndicator->initialize(3, 2);
    iw->setActivePage(0);
    QCOMPARE(iw->mPageIndicator->activeItemIndex(), 0);
    delete iw;   
}

void TestHsDomainModel::testIdleWidgetInsertPage()
{
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::PageWallpapers);
    HsIdleWidget *iw = new HsIdleWidget();
    HsPage *page = new HsPage();
    iw->insertPage(0, page);
    delete iw;
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::SceneWallpaper);
    iw = new HsIdleWidget();
    page = new HsPage();
    iw->insertPage(0, page);
    delete iw;
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::PageWallpapers);
}

void TestHsDomainModel::testIdleWidgetRemovePage()
{
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::PageWallpapers);
    HsIdleWidget *iw = new HsIdleWidget();

    HsPage *pageOne = new HsPage();
    HsPage *pageTwo = new HsPage();

    iw->insertPage(0, pageTwo);
    iw->insertPage(0, pageOne);
    iw->removePage(1);
    iw->removePage(0);
    delete iw;
    
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::SceneWallpaper);
    iw = new HsIdleWidget();

    pageOne = new HsPage();
    pageTwo = new HsPage();

    iw->insertPage(0, pageTwo);
    iw->insertPage(0, pageOne);
    iw->removePage(1);
    iw->removePage(0);
    delete iw;
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::PageWallpapers);
}

void TestHsDomainModel::testIdleWidgetLayers()
{
    HsIdleWidget *iw = new HsIdleWidget();
    iw->controlLayer();
    iw->pageLayer();
    iw->pageWallpaperLayer();
    iw->sceneLayer();
    delete iw;
}

void TestHsDomainModel::testIdleWidgetTrashbin()
{
    HsIdleWidget *iw = new HsIdleWidget();
    iw->trashBin();
    delete iw;
}

void TestHsDomainModel::testIdleWidgetPageIndicator()
{
    HsIdleWidget *iw = new HsIdleWidget();
    iw->pageIndicator();
    delete iw;
}

void TestHsDomainModel::testIdleWidgetParallaxFactor() 
{
    HsIdleWidget *iw = new HsIdleWidget();
    
    // factor == 1 when page count < 2
    QCOMPARE( (int)iw->parallaxFactor(), 1);
    
    createSceneWithPages(3, 0);
    // factor must be != 0 when page count >= 2
    QVERIFY(iw->parallaxFactor());
    delete iw;
    destroySceneAndPages();
}

void TestHsDomainModel::testIdleWidgetShowTrashBin() 
{
    HsIdleWidget *iw = new HsIdleWidget();
    iw->showTrashBin();
    delete iw;
}

void TestHsDomainModel::testIdleWidgetShowPageIndicator() 
{
    HsIdleWidget *iw = new HsIdleWidget();
    iw->showPageIndicator();
    delete iw;
}


void TestHsDomainModel::testIdleWidgetShowSnapLines() 
{
    HsIdleWidget *iw = new HsIdleWidget();
    QLineF verticalSnapLine(0,0,0,100);
    iw->showVerticalSnapLine(verticalSnapLine);
    QLineF horizontalSnapLine(0,0,100,0);
    iw->showHorizontalSnapLine(horizontalSnapLine);
    iw->hideVerticalSnapLine();
    iw->hideHorizontalSnapLine();
    delete iw;
}

void TestHsDomainModel::testIdleWidgetPolishEvent() 
{
    HsIdleWidget *iw = new HsIdleWidget();
    iw->polishEvent();
    
    createSceneWithPages(3, 0);
    iw->polishEvent();
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::SceneWallpaper);
    iw->polishEvent();
    HSCONFIGURATION_SET(setSceneType, HsConfiguration::PageWallpapers);
    
    delete iw;
    destroySceneAndPages();
}
