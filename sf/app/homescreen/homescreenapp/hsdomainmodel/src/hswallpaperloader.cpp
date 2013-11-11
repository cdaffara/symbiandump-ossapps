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

#include <QFileInfo>
#include <QVariantHash>

#include "hswallpaperloader.h"
#include "hswallpaperloaderthread.h"

/*!
    \class HsWallpaperLoader
    \ingroup group_hsdomainmodel
    \brief 
*/

/*!

*/
HsWallpaperLoader::HsWallpaperLoader(QObject *parent)
  : QObject(parent),
    mIsRunning(false),
    mNumberOfFinishedThreads(0)
{
}

/*!

*/
HsWallpaperLoader::~HsWallpaperLoader()
{
}

/*!

*/
void HsWallpaperLoader::start()
{
    if (mTargets.isEmpty()) {
        emit finished();
        return;
    }

    mNumberOfFinishedThreads = 0;
    
    HsWallpaperLoaderThread *thread = 0;
    QHashIterator<QString, QVariant> i(mTargets);
    while (i.hasNext()) {
        i.next();
        thread = new HsWallpaperLoaderThread;
        mThreads.append(thread);
        thread->setSourcePath(mSourcePath);
        thread->setTargetPath(i.key());
        thread->setTargetSize(i.value().toSize());
        connect(thread, SIGNAL(finished()), SLOT(onThreadFinished()));
        thread->start();
    }

    mIsRunning = true;
}

/*!

*/
void HsWallpaperLoader::cancel()
{
    foreach (QThread *thread, mThreads) {
        thread->disconnect(this);
        thread->wait();
    }
    cleanup();
}

/*!

*/
void HsWallpaperLoader::onThreadFinished()
{
    ++mNumberOfFinishedThreads;
    if (mNumberOfFinishedThreads == mThreads.count()) {
        bool ok = true;
        foreach (HsWallpaperLoaderThread *thread, mThreads) {
            if (!thread->result()) {
                ok = false;
                break;
            }
        }
        if (ok) {
            emit finished();
        } else {            
            emit failed();
        }
        cleanup();
    }
}

/*!

*/
void HsWallpaperLoader::cleanup()
{
    mIsRunning = false;
    mNumberOfFinishedThreads = 0;
    qDeleteAll(mThreads);
    mThreads.clear();
}
