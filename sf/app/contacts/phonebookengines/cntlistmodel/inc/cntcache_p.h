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
* Description: Private data and helper classes used by class CntCache.
*
*/

#ifndef CNTCACHE_P_H
#define CNTCACHE_P_H

#include <QSharedData>
#include <QSet>
#include <HbIcon>
#include <qcontactmanager.h>
#include "cntinfoprovider.h"

class ThumbnailManager;

QTM_USE_NAMESPACE

/*!
    Private shared data for the CntContactInfo class.
 */
class CntContactInfoData : public QSharedData
{
public:
     CntContactInfoData() : id(-1), fields(0) { }
     ~CntContactInfoData() { }

public:
    int id;
    int fields;
    QString name;
    QString text;
    HbIcon icon1;
    HbIcon icon2;
};

/*!
    Cache item that holds info for one contact: text and two icon names.
 */
class CntInfoCacheItem
{
public:
    int cacheOrder;
    int contactId;
    int latestRow;
    QString text;
    QString icons[2];
};

/*!
    Cache item that holds one icon. Data member isFetched is false until the
    icon has been fetched asynchronously.
 */
class CntIconCacheItem
{
public:
    int cacheOrder;
    QString iconName;
    bool isFetched;
    QSet<int> contactIds;
    HbIcon icon;
};

/*!
    Worker class that fetches contact info and icons in the background.
    CntCacheThread uses info provider plugins and thumbnail manager to retrieve
    the actual data. This class' responsibilities are 1) fetch the requested
    data in a timely manner and 2) interfere with the UI as little as possible.
    This is mainly orchestrated by the client, who calls postponeJobs() when
    the UI is active.
    
    If the client sends many requests (e.g. during a long scrolling operation
    in the UI), then the oldest jobs will be cancelled. However, the cancelled jobs
    will be informed back to the client later so that it can choose to reschedule
    the jobs.
 */
class CntCacheThread : public QObject
{
    friend class TestCntCache;
    Q_OBJECT
public:
    CntCacheThread();
    ~CntCacheThread();

    void scheduleInfoJob(int contactId, int priority);
    void scheduleIconJob(const QString& iconName, int priority);
    void postponeJobs(int milliseconds = 0);
    bool event(QEvent *event);

public slots:
    void resumeJobs();

signals:
    void infoFieldUpdated(int contactId, ContactInfoField infoField, const QString& infoValue);
    void infoCancelled(int contactId);
    void iconUpdated(const QString& iconName, const HbIcon& icon);
    void iconCancelled(const QString& iconName);
    void allJobsDone();
    
private slots:
    void onInfoFieldReady(CntInfoProvider* sender, int contactId,
                          ContactInfoField field, const QString& text);
    void onIconReady(const QPixmap& pixmap, void *data, int id, int error);

private:
    void processJobs();
    int infoJobIndex(int contactId);
    int takeNextInfoJob();
    int iconJobIndex(QString iconName);
    QString takeNextIconJob();

private:
    QContactManager* mContactManager;       // for fetching QContact objects
    ThumbnailManager* mThumbnailManager;    // for fetching icons

    // maps info providers to their responsibilities
    QMap<CntInfoProvider*, ContactInfoFields> mInfoProviders;

    bool mProcessingJobs;                   // true from when job loop event has been posted until job loop exits
    int mJobsPostponed;                     // are jobs postponed (no / for some time / until further notice)
    QList< QPair<int,int> > mInfoJobs;      // list of all info jobs and their priorities
    QList<int> mCancelledInfoJobs;          // list of all cancelled info jobs
    QList< QPair<QString,int> > mIconJobs;  // list of all icon jobs and their priorities
    QList<QString> mCancelledIconJobs;      // list of all cancelled icon jobs
    int mIconRequestId;                     // the id for the last request to thumbnail manager
    QString mIconRequestName;               // the name of the icon last requested from thumbnail manager
    QTimer *mTimer;                         // timer used when postponing jobs
};

#endif
