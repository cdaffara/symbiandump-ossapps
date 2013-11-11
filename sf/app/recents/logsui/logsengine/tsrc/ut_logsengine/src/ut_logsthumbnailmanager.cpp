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
* Description:
*
*/


#include "ut_logsthumbnailmanager.h"
#include "logsthumbnailmanager.h"
#include <e32cmn.h> //KNullDesC
#include <QIcon>
#include <QSignalSpy>
#include <QtTest/QtTest>
#include <thumbnailmanager_qt.h>
#include <hbicon.h>


 const QString path1 = "c:\\data\\images\\bg_1.png";
 const QString path3 = "e:\\images\\non.jpeg";



void UT_LogsThumbnailManager::initTestCase()
    {
    mIconMgr = new LogsThumbIconManager();
    
    }

void UT_LogsThumbnailManager::cleanupTestCase()
    {
    delete mIconMgr;
    mIconMgr = NULL;
    }

void UT_LogsThumbnailManager::init()
    {
    initTestCase();
    }
     
void UT_LogsThumbnailManager::cleanup()
    {
    cleanupTestCase();
    }
    
void UT_LogsThumbnailManager::testConstructor()
{
}

void UT_LogsThumbnailManager::testNonExistingIcon()
    {
    QSignalSpy spy(mIconMgr, SIGNAL(contactIconReady(int)));
    QVERIFY(spy.isValid());
    QCOMPARE( spy.count(), 0 );
    mIcon = &mIconMgr->contactIcon(path3, 0);
    QVERIFY(mIcon->isNull()); // default icon used first
    QVERIFY(mIcon == HbIcon::defaultIcon());
    
    mIconMgr->cancel();
    
    mIcon = &mIconMgr->contactIcon(QString(), 0);
    QVERIFY(mIcon->isNull());
    QVERIFY(mIcon == HbIcon::defaultIcon());
    
    }
   
void UT_LogsThumbnailManager::testOneExistingIcon()
    {
    QSignalSpy spy(mIconMgr, SIGNAL(contactIconReady(int)));
    QVERIFY(spy.isValid());
    QCOMPARE( spy.count(), 0 );
    mIcon = &mIconMgr->contactIcon(path1, 10);
    QVERIFY(mIcon->isNull());
    QVERIFY(mIcon == HbIcon::defaultIcon());
    
    QPixmap pixmap(path1);
    int index = 10;
    int *clientData = new int(index);
    int reqId = mIconMgr->mThumbnailManager->getThumbnail(path1, clientData, 0);
    mIconMgr->mTnmReqMap.insert(reqId, path1);
    mIconMgr->thumbnailReady(pixmap, clientData, 1, 0);
    //
    mIcon = &mIconMgr->contactIcon(path1, 10);
    QVERIFY(mIcon->isNull());
    QVERIFY(mIcon != HbIcon::defaultIcon());
    //
    mIconMgr->cancel();
    //
    mIcon = &mIconMgr->contactIcon(path1, 10);
    QVERIFY(mIcon->isNull());
    QVERIFY(mIcon == HbIcon::defaultIcon());
    reqId = mIconMgr->mThumbnailManager->getThumbnail(path1, clientData, 0);
    mIconMgr->mTnmReqMap.insert(reqId, path1);
    mIconMgr->thumbnailReady(pixmap, clientData, 2, -1);
    mIconMgr->cancel();
    
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).toInt() == 10);
    mIcon = &mIconMgr->contactIcon(path1, 10);
    QVERIFY(mIcon->isNull());
    QVERIFY(mIcon == HbIcon::defaultIcon());
    }
    
void UT_LogsThumbnailManager::testCancel()
    {
    QSignalSpy spy(mIconMgr, SIGNAL(contactIconReady(int)));
    QVERIFY(spy.isValid());
    QCOMPARE( spy.count(), 0 );
    mIcon = &mIconMgr->contactIcon(path1, 10);
    QVERIFY(mIcon->isNull());
    QVERIFY(mIcon == HbIcon::defaultIcon());
    mIcon = &mIconMgr->contactIcon(path1, 11);
    QVERIFY(mIcon->isNull());
    QVERIFY(mIcon == HbIcon::defaultIcon());
    mIcon = &mIconMgr->contactIcon(path3, 0);
    QVERIFY(mIcon->isNull());
    QVERIFY(mIcon == HbIcon::defaultIcon());
    mIconMgr->thumbnailLoad();
    mIconMgr->cancel();
    QCOMPARE(spy.count(), 0);
    }

 

