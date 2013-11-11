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

#ifndef HSWALLPAPERLOADER_H
#define HSWALLPAPERLOADER_H

#include <QObject>
#include <QVariantHash>

#include "hsdomainmodel_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class QThread;

class HsWallpaperLoaderThread;

class HSDOMAINMODEL_EXPORT HsWallpaperLoader : public QObject
{
    Q_OBJECT

public:
    HsWallpaperLoader(QObject *parent = 0);
    ~HsWallpaperLoader();

    void setSourcePath(const QString &path) { mSourcePath = path; }
    
    QVariantHash targets() const { return mTargets; }
    void setTargets(const QVariantHash &targets) { mTargets = targets; }
    
    bool isRunning() const { return mIsRunning; }

signals:
    void finished();
    void failed();

public slots:
    void start();
    void cancel();

private:
    Q_DISABLE_COPY(HsWallpaperLoader)
    void cleanup();
    
private slots:
    void onThreadFinished();

private:
    bool mIsRunning;
    QString mSourcePath;
    QVariantHash mTargets;    
    QList<HsWallpaperLoaderThread *> mThreads;
    int mNumberOfFinishedThreads;
    
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSWALLPAPERLOADER_H
