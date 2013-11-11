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
* Description:  Wallpaper test.
*
*/


#include "t_hsdomainmodel.h"
#include "hsbackuprestoreobserver.h"
#include "hsscene.h"
#include <QPainter>
#ifdef Q_OS_SYMBIAN
#include <connect/sbdefs.h>
#include <e32property.h>
#include <connect/abclient.h>
#include <XQSettingsKey>
#include "hsbackuprestoreobserver_p.h"

using namespace conn;
#endif //Q_OS_SYMBIAN
/*!

*/
void TestHsDomainModel::testBackupRestoreObserver()
{
    HsBackupRestoreObserver *observer = HsBackupRestoreObserver::instance();
    QVERIFY(observer);
    QVERIFY(observer->mInstance != 0);
    QVERIFY(observer->mInstance == HsBackupRestoreObserver::instance());
    delete observer->mInstance;
    observer->mInstance = 0;
}


void TestHsDomainModel::testBackupRestoreObserverStarted()
{
    HsBackupRestoreObserver *observer = HsBackupRestoreObserver::instance();
    QSignalSpy observerSpy(observer,SIGNAL(event_backupRestoreStarted()));
    observer->backupRestoreStarted();
    observer->readyForBUR();
    QVERIFY(observer->mBUROngoing);
    QVERIFY(observer->checkBUR());
    QVERIFY(observerSpy.count()==1);
    delete observer->mInstance;
    observer->mInstance = 0;}

void TestHsDomainModel::testBackupRestoreObserverCompleted()
{
    HsBackupRestoreObserver *observer = HsBackupRestoreObserver::instance();
    QSignalSpy observerSpy(observer,SIGNAL(event_backupRestoreCompleted()));
    observer->backupRestoreCompleted();
    QVERIFY(!observer->mBUROngoing);
    QVERIFY(!observer->checkBUR());
    QVERIFY(observerSpy.count()==1);
    delete observer->mInstance;
    observer->mInstance = 0;
}

#ifdef Q_OS_SYMBIAN
void TestHsDomainModel::testBackupRestoreObserverPsKeyObserver()
{
    HsBackupRestoreObserver *observer = HsBackupRestoreObserver::instance();
    XQSettingsKey brk(XQSettingsKey::TargetPublishAndSubscribe,
                      KUidSystemCategory.iUid, KUidBackupRestoreKey);
    observer->mD->handleKeyChange(brk, conn::EBURBackupFull);
    QVERIFY(observer->mBUROngoing);
    QVERIFY(observer->checkBUR());

    observer->mD->handleKeyChange(brk, conn::EBURNormal);
    QVERIFY(!observer->mBUROngoing);
    QVERIFY(!observer->checkBUR());
    delete observer->mInstance;
    observer->mInstance = 0;
}
#endif //Q_OS_SYMBIAN
