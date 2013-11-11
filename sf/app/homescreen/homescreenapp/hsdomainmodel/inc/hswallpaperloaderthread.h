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

#ifndef HSWALLPAPERLOADERTHREAD_H
#define HSWALLPAPERLOADERTHREAD_H

#include <QThread>
#include <QString>
#include <QRect>
#include "hsdomainmodel_global.h"
#include "hstest_global.h"

HOMESCREEN_TEST_CLASS(TestHsDomainModel)

class HSDOMAINMODEL_EXPORT HsWallpaperLoaderThread : public QThread
{
    Q_OBJECT

public:
    HsWallpaperLoaderThread(QObject *parent = 0);
    ~HsWallpaperLoaderThread();

    void setSourcePath(const QString &path) { mSourcePath = path; }
    QString sourcePath() const { return mSourcePath; }

    void setTargetPath(const QString &path) { mTargetPath = path; }
    QString targetPath() const { return mTargetPath; }

    void setTargetSize(const QSize &size) { mTargetSize = size; }
    QSize targetSize() const { return mTargetSize; }

    bool result() const { return mResult; }

protected:
    void run();

private:
    Q_DISABLE_COPY(HsWallpaperLoaderThread)

private:
    QString mSourcePath;
    QString mTargetPath;
    QSize mTargetSize;
    bool mResult;
    HOMESCREEN_TEST_FRIEND_CLASS(TestHsDomainModel)
};

#endif // HSWALLPAPERLOADERTHREAD_H
