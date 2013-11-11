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
#include <QFile>

#include "t_hsdomainmodel.h"
#include "hswallpaperloaderthread.h"

void TestHsDomainModel::testHsWallpaperLoaderThreadConstruction()
{
    {
        HsWallpaperLoaderThread thread;
        QVERIFY(!thread.parent());
    }

    {
        QObject parent;
        HsWallpaperLoaderThread thread(&parent);
        QVERIFY(thread.parent() == &parent);
    }

    HsWallpaperLoaderThread *thread = new HsWallpaperLoaderThread;
    QVERIFY(thread);
    QVERIFY(!thread->parent());
    delete thread;

    QObject *parent = new QObject();
    thread = new HsWallpaperLoaderThread(parent);
    QVERIFY(thread);
    QVERIFY(thread->parent() == parent);
    delete parent;
}

void TestHsDomainModel::testHsWallpaperLoaderThreadRun()
{
    //Test preparation
    cleanupTargetWallpaperFiles();

    HsWallpaperLoaderThread *thread = new HsWallpaperLoaderThread;

    //TEST OF FILE WHICH DOES NOT EXIST
    QVERIFY(!QFile::exists(notExistingTestWallpaper()));
    thread->setSourcePath(notExistingTestWallpaper());
    thread->setTargetSize(QSize(360, 640));
    thread->setTargetPath(tempTestWallpaper());
    thread->run();
    //Check that temp file is not created and result is false
    QVERIFY(!thread->result());
    QVERIFY(!QFile::exists(tempTestWallpaper()));

    //TEST OF FILE WHICH EXISTS
    //Check that test file exist
    QVERIFY(QFile::exists(sourceTestWallpaper()));
    thread->setSourcePath(sourceTestWallpaper());
    thread->setTargetSize(QSize(360, 640));
    thread->setTargetPath(tempTestWallpaper());
    thread->run();
    //Check that temp file is created and result is true
    QVERIFY(thread->result());
    QVERIFY(QFile::exists(tempTestWallpaper()));
    //Clear created temp file
    cleanupTargetWallpaperFiles();

    delete thread;
}
