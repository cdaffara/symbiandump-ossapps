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


#include <QObject>
#include <QString>
#include <QSignalSpy>
#include <QVariantHash>
#include <QEventLoop>
#include <QTimer>

#include "t_hsdomainmodel.h"
#include "hswallpaperloader.h"
#include "hswallpaperloaderthread.h"

void TestHsDomainModel::testHsWallpaperLoaderConstruction()
{
    {
        HsWallpaperLoader loader;
        QVERIFY(!loader.parent());
    }

    {
        QObject parent;
        HsWallpaperLoader loader(&parent);
        QVERIFY(loader.parent() == &parent);
    }

    HsWallpaperLoader *loader = new HsWallpaperLoader;
    QVERIFY(loader);
    QVERIFY(!loader->parent());
    delete loader;

    QObject *parent = new QObject();
    loader = new HsWallpaperLoader(parent);
    QVERIFY(loader);
    QVERIFY(loader->parent() == parent);
    delete parent;
}

void TestHsDomainModel::testHsWallpaperLoaderStartWithFinished()
{
    //Test preparation
    cleanupTargetWallpaperFiles();

    HsWallpaperLoader *loader = new HsWallpaperLoader;
    QSignalSpy finishedSpy(loader, SIGNAL(finished()));

    //TEST START WITHOUT THREADS
    loader->start();
    QCOMPARE(finishedSpy.count(), 1);
    QVERIFY(!loader->isRunning());
    finishedSpy.clear();

    //TEST START WITH ONE THREAD SUCCESSFUL
    //Check that test file exist
    QVERIFY(QFile::exists(sourceTestWallpaper()));
    loader->setSourcePath(sourceTestWallpaper());

    QVariantHash targets;
    targets.insert(tempTestWallpaper(), QSize(360, 640));
    loader->setTargets(targets);

    QEventLoop waitFinished;
    QTimer eventLoopTimer;
    eventLoopTimer.setInterval(15000);
    eventLoopTimer.setSingleShot(true);
    connect(loader, SIGNAL(finished()), &waitFinished, SLOT(quit()));
    connect(&eventLoopTimer, SIGNAL(timeout()), &waitFinished, SLOT(quit()));
    loader->start();
    QVERIFY(loader->isRunning());
    eventLoopTimer.start();
    waitFinished.exec();

    QCOMPARE(finishedSpy.count(), 1);
    QVERIFY(eventLoopTimer.isActive());
    eventLoopTimer.stop();

    QVERIFY(!loader->isRunning());
    //Check that temp file is created and result is true
    QVERIFY(QFile::exists(tempTestWallpaper()));
    //Clear created temp file
    cleanupTargetWallpaperFiles();

    delete loader;
}

void TestHsDomainModel::testHsWallpaperLoaderStartWithFailed()
{
    //Test preparation
    cleanupTargetWallpaperFiles();

    HsWallpaperLoader *loader = new HsWallpaperLoader;
    QSignalSpy failedSpy(loader, SIGNAL(failed()));

    //TEST START WITH ONE FAILING THREAD AND ONE SUCCESSFUL
    QVERIFY(QFile::exists(sourceTestWallpaper()));
    loader->setSourcePath(sourceTestWallpaper());

    QVariantHash targets;
    targets.insert(tempTestWallpaper(), QSize(360, 640));
    targets.insert(QString(), QSize(640, 360)); //Save fails for empty string in thread
    loader->setTargets(targets);

    QEventLoop waitFailed;
    QTimer eventLoopTimer;
    eventLoopTimer.setInterval(15000);
    eventLoopTimer.setSingleShot(true);
    connect(loader, SIGNAL(failed()), &waitFailed, SLOT(quit()));
    connect(&eventLoopTimer, SIGNAL(timeout()), &waitFailed, SLOT(quit()));
    loader->start();
    QVERIFY(loader->isRunning());
    eventLoopTimer.start();
    waitFailed.exec();

    QCOMPARE(failedSpy.count(), 1);
    QVERIFY(eventLoopTimer.isActive());
    eventLoopTimer.stop();

    QVERIFY(!loader->isRunning());
    //Check that temp file is created and result is true
    QVERIFY(QFile::exists(tempTestWallpaper()));

    //Clear created temp file
    cleanupTargetWallpaperFiles();
    delete loader;

}

void TestHsDomainModel::testHsWallpaperLoaderCancel()
{
    //Test preparation
    cleanupTargetWallpaperFiles();

    HsWallpaperLoader *loader = new HsWallpaperLoader;
    QSignalSpy failedSpy(loader, SIGNAL(failed()));
    QSignalSpy finishedSpy(loader, SIGNAL(finished()));

    //TEST START WITH ONE FAILING THREAD AND ONE SUCCESSFUL
    QVERIFY(QFile::exists(sourceTestWallpaper()));
    loader->setSourcePath(sourceTestWallpaper());

    QVariantHash targets;
    targets.insert(tempTestWallpaper(), QSize(360, 640));
    targets.insert(QString(), QSize(640, 360)); //This fails due to empty target file path.
    loader->setTargets(targets);

    loader->start();
    QVERIFY(loader->isRunning());
    loader->cancel();
    QVERIFY(!loader->isRunning());
    QCOMPARE(failedSpy.count(), 0);
    QCOMPARE(finishedSpy.count(), 0);

    //Clear temp file if it was created
    cleanupTargetWallpaperFiles();

    delete loader;
}
