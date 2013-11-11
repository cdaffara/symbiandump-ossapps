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
* Description: Music Player collection album art manager.
*
*/


#ifndef LOGSTHUMBNAILMANAGER_H
#define LOGSTHUMBNAILMANAGER_H

#include <QObject>
#include <qcache.h>
#include <QMap>
#include <QQueue>
#include <QPair>
#include <QIcon>
#include <QTimer>

class ThumbnailManager;
class HbIcon;

class LogsThumbIconManager : public QObject
{
    Q_OBJECT

public:

    explicit LogsThumbIconManager(QObject *parent=0);
    virtual ~LogsThumbIconManager();

    QIcon& contactIcon(const QString &avatarPath, int index);
    void cancel();
    QIcon& defaultIcon();

signals:
    void contactIconReady(int index);

public slots:
    void thumbnailReady(const QPixmap& pixmap, void *data, int id, int error);
    void thumbnailLoad();
    void timerTimeout();

private:
    ThumbnailManager                *mThumbnailManager;
    QCache<QString, QIcon>			mImageCache;
    QMap<int, QString>              mTnmReqMap;

    QQueue< QPair<QString, int> >   mRequestQueue;
    int                             mQueueCount;
    QTimer*                         mTimer;
    HbIcon*							mDefaultIcon;
    
private:
        
    friend class UT_LogsMatchesModel;
    friend class UT_LogsThumbnailManager;

};

#endif // LOGSTHUMBNAILMANAGER_H

