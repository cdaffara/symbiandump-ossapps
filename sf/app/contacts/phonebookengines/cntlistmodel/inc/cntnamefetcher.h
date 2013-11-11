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

#ifndef CNTNAMEFETCHER_H
#define CNTNAMEFETCHER_H

#include <QObject>
#include <QContact>
#include <QDataStream>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <cntuids.h>

#include <e32base.h>
#include <s32mem.h>
#include <e32std.h>
#include <QObject>
#include <QThread>
#include <QEvent>
#include <hbapplication.h>

QTM_USE_NAMESPACE

class CntSrvSession;

/*!
    Cache item that holds the formatted name of one contact.
 */
class CntNameCacheItem
{
public:
    CntNameCacheItem(QContactLocalId id, const QString &firstName, const QString &lastName, CntNameOrder nameFormat);
    ~CntNameCacheItem();
    QContactLocalId contactId() const { return mContactId; }
    QString name() const { return mName; }
    QString firstName() const;
    QString lastName() const;
    void setNameFormat(CntNameOrder newFormat);
    void operator=(const CntNameCacheItem &other);
    void externalize(QDataStream &stream);
    static CntNameCacheItem* internalize(QDataStream &stream, CntNameOrder nameFormat);

private:
    void setFormattedName(const QString &firstName, const QString &lastName, CntNameOrder nameFormat);

private:
    QContactLocalId mContactId; // database contact id for this name
    int mFirstNamePosition;     // length << 16 | offset
    int mLastNamePosition;      // length << 16 | offset
    QString mName;              // formatted name
};

/*!
     Issues requests to CntSrv.
  */
class CntSrvConnection : public QObject
{
    Q_OBJECT
public:
    CntSrvConnection();
    ~CntSrvConnection();
    void setAsynchronous();
    bool executeSqlQuery(const QString &sqlQuery, CntNameOrder nameFormat, int sizeHintKB);
    QList<CntNameCacheItem *> names() { return mNames; }

protected:
    bool event(QEvent *event);

signals:
    void namesRead();
    void namesSorted();

private:
    QThread mThread;
    QList<CntNameCacheItem *> mNames;
    CntSrvSession *mSession;
    QString mSqlQuery;
    CntNameOrder mNameFormat;
    int mSizeHintKB;
    bool mIsAsynchronous;
};

/*!
    Can fetches all contact names in sorted order:
       - from file cache (= any changes since last app exit are not reflected in this list)
       - from database syncronously
       - from database asyncronously
    Fetching from the file cache takes about 0.2 seconds for 10000 contacts.
    Fetching from the database takes about 2 seconds for 10000 contacts.
 */
class CntNameFetcher : public QObject
{
    Q_OBJECT
public:
    CntNameFetcher();
    ~CntNameFetcher();
    CntNameCacheItem * readOneName(QContactLocalId contactId) const;
    void readAllNamesAsynch();
    bool readNamesFromCache(QList<CntNameCacheItem *> &names);
    bool writeNamesToCache(const QList<CntNameCacheItem *> &names) const;
    void sortNames(QList<CntNameCacheItem *> &names) const;
    static bool compareNames(const CntNameCacheItem *a, const CntNameCacheItem *b);
    
signals:
    void nameFormatChanged(CntNameOrder nameFormat);
    void databaseAccessComplete();
    void namesAvailable(QList<CntNameCacheItem *> contactNames);
    
private slots:
    void setNameFormat(const XQSettingsKey &key, const QVariant &value);
    void sendCompletionSignal();
    
private:
    CntSrvConnection *mDbConnection;
    CntSrvConnection *mAsynchDbConnection;
    XQSettingsManager *mSettingsManager;
    XQSettingsKey *mNameFormatSetting;
    CntNameOrder mNameFormat;
    int mBufferSizeEstimate;
};

#endif
