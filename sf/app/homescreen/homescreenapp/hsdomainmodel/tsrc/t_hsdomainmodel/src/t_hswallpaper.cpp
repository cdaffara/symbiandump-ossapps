/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "hswallpaper.h"
#include "hsscene.h"
#include "hspage.h"

void TestHsDomainModel::testHsWallpaperConstruction()
{
    {
        HsSceneWallpaper wallpaper(HsScene::instance());
        QVERIFY(!wallpaper.parent());
    }

    HsPage *page = new HsPage();
    page->setDatabaseId(1);
    {
        HbWidget parent;
        HsPageWallpaper wallpaper(page, &parent);
        QVERIFY(wallpaper.parentItem() == &parent);
    }

    HsPageWallpaper *pagewallpaper = new HsPageWallpaper(page);
    QVERIFY(pagewallpaper);
    QVERIFY(!pagewallpaper->parent());
    delete pagewallpaper;

    HbWidget *parent = new HbWidget();
    HsSceneWallpaper *scenewallpaper = new HsSceneWallpaper(HsScene::instance(), parent);
    QVERIFY(scenewallpaper);
    QVERIFY(scenewallpaper->parentItem() == parent);
    delete parent;

    delete page;
}

void TestHsDomainModel::testHsWallpaperSetNRemoveDefaultImage()
{
    //Default image is set already at construction phase while scene DB id
    //is not set.
    QVERIFY(QFile::exists(sourceTestWallpaper()));
    HsSceneWallpaper *wallpaper = new HsSceneWallpaper(HsScene::instance());
    QVERIFY(QDir::toNativeSeparators(wallpaper->mPortraitImagePath) == sourceTestWallpaper());
    wallpaper->setDefaultImage(); //Coverage improvement. Default image already set at construction phase.
    QVERIFY(QDir::toNativeSeparators(wallpaper->mPortraitImagePath) == sourceTestWallpaper());
    QVERIFY(wallpaper->mIsDefaultImage);
    wallpaper->remove();
    QVERIFY(!wallpaper->mIsDefaultImage);
    QVERIFY(wallpaper->mPortraitImagePath.isEmpty());
    QVERIFY(wallpaper->mLandscapeImagePath.isEmpty());
    delete wallpaper;
}

void TestHsDomainModel::testHsWallpaperSetNRemoveImage()
{
    mWallpaperTypeScene = false; //Use page wallpaper at this test
    //Prepare test case by checking that source file exists and
    //deleting any page specific wallpapers existing in target folder
    QVERIFY(QFile::exists(sourceTestWallpaper()));
    cleanupTargetWallpaperFiles();

    HsPage *page = new HsPage();
    page->setDatabaseId(1);
    //Construction phase sets default image since Id based image does not exist, yet.
    HsPageWallpaper *wallpaper = new HsPageWallpaper(page);
    QVERIFY(QDir::toNativeSeparators(wallpaper->mPortraitImagePath) == sourceTestWallpaper());

    //TEST EMPTY SOURCE STRING
    QSignalSpy setImageFailedSpy(wallpaper, SIGNAL(imageSetFailed()));
    wallpaper->setImage(QString());
    QCOMPARE(setImageFailedSpy.count(), 1);

    QSignalSpy imageSetSpy(wallpaper, SIGNAL(imageSet()));
    QEventLoop waitFinished;
    QTimer eventLoopTimer;
    eventLoopTimer.setInterval(15000);
    eventLoopTimer.setSingleShot(true);
    connect(wallpaper, SIGNAL(imageSet()), &waitFinished, SLOT(quit()));
    connect(&eventLoopTimer, SIGNAL(timeout()), &waitFinished, SLOT(quit()));
    //TEST SETIMAGE
    wallpaper->setImage(sourceTestWallpaper());
    eventLoopTimer.start();
    waitFinished.exec();

    QCOMPARE(imageSetSpy.count(), 1);
    QVERIFY(eventLoopTimer.isActive());
    eventLoopTimer.stop();

    //Now we have Id based images.
    QVERIFY(QDir::toNativeSeparators(wallpaper->mPortraitImagePath) == targetPortraitTestWallpaper());
    QVERIFY(QDir::toNativeSeparators(wallpaper->mLandscapeImagePath) == targetLandscapeTestWallpaper());
    QVERIFY(QFile::exists(targetPortraitTestWallpaper()));
    QVERIFY(QFile::exists(targetLandscapeTestWallpaper()));

    //TEST REMOVE
    wallpaper->remove();
    QVERIFY(wallpaper->mPortraitImagePath.isEmpty());
    QVERIFY(wallpaper->mLandscapeImagePath.isEmpty());
    QVERIFY(!QFile::exists(targetPortraitTestWallpaper()));
    QVERIFY(!QFile::exists(targetLandscapeTestWallpaper()));

    delete wallpaper;
    delete page;
}

void TestHsDomainModel::testHsWallpaperLoaderFailed()
{
    cleanupTargetWallpaperFiles();

    HsPage *page = new HsPage();
    page->setDatabaseId(1);
    //Construction phase sets default image since Id based image does not exist, yet.
    HsPageWallpaper *wallpaper = new HsPageWallpaper(page);
    QSignalSpy loaderFailedSpy(wallpaper, SIGNAL(imageSetFailed()));
    wallpaper->onLoaderFailed();
    QCOMPARE(loaderFailedSpy.count(), 1);

    delete wallpaper;
    delete page;
}

void TestHsDomainModel::testHsWallpaperScene()
{
    cleanupTargetWallpaperFiles();
    //Construction phase sets default image since Id based image does not exist, yet.
    HsSceneWallpaper *wallpaper = new HsSceneWallpaper(HsScene::instance());
    QVariantHash papers = wallpaper->createTargets(".");
    QVERIFY(papers.count()==2);
    wallpaper->setScene(0);
    QVERIFY(HsScene::instance() == wallpaper->mScene);

    delete wallpaper;
}
void TestHsDomainModel::testHsWallpaperPage()
{
    cleanupTargetWallpaperFiles();

    HsPage *page = new HsPage();
    page->setDatabaseId(1);
    //Construction phase sets default image since Id based image does not exist, yet.
    HsPageWallpaper *wallpaper = new HsPageWallpaper(page);
    wallpaper->setPage(0);
    QVERIFY(page == wallpaper->mPage);
    delete wallpaper;
    delete page;
}

void TestHsDomainModel::testHsWallpaperSetImages()
{
    HsPageWallpaper wp(0);
    QSignalSpy spyIS(&wp, SIGNAL(imageSet()));
    QSignalSpy spyISF(&wp, SIGNAL(imageSetFailed()));
    
    wp.setImages(QString(), QString());

    QCOMPARE(spyIS.count(), 0);
    QCOMPARE(spyISF.count(), 1);    
}
