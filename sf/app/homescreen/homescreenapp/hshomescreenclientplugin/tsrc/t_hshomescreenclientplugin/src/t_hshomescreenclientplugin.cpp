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
* Description:  Test class for hshomescreenclientplugin
*
*/

#include "t_hshomescreenclientplugin.h"
#include "hshomescreenclient.h"

#include <xqservicerequest.h>

#include <QSignalSpy>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsHomescreenClientPluginTest::initTestCase()
{ 
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsHomescreenClientPluginTest::cleanupTestCase()
{
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsHomescreenClientPluginTest::testConstruction()
{
    HsHomescreenClient *client = new HsHomescreenClient;
    QVERIFY(client);
    delete client;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsHomescreenClientPluginTest::testAddWidget()
{  
    HsHomescreenClient *client = new HsHomescreenClient;
    
    QSignalSpy requestFinishedSpy(client, SIGNAL(requestFinished()));
    
    // add invalid widget
    QVariantHash preferences;
    bool result = client->addWidget("invalidwidget", preferences);
    
    // but emit correct signals
    connect(this, SIGNAL(onRequestCompletedTest(QVariant)), client, SLOT(onRequestCompleted(QVariant)));
    emit onRequestCompletedTest(true);
    
    QVERIFY(requestFinishedSpy.count());
    QVERIFY(client->mRequestResult);
    
    delete client;    
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsHomescreenClientPluginTest::testAddInvalidWidget()
{  
    HsHomescreenClient *client = new HsHomescreenClient;
    
    QVariantHash preferences;
    bool result = client->addWidget("invalidwidget", preferences);
    
    QVERIFY(!result);
    QVERIFY(!client->mRequestResult);

    delete client;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsHomescreenClientPluginTest::testSetWallpaper()
{  
    HsHomescreenClient *client = new HsHomescreenClient;
    
    QSignalSpy requestFinishedSpy(client, SIGNAL(requestFinished()));
    
    bool result = client->setWallpaper("wallpaper");
    
    // but emit correct signals
    connect(this, SIGNAL(onRequestCompletedTest(QVariant)), client, SLOT(onRequestCompleted(QVariant)));
    emit onRequestCompletedTest(true);
    
    QVERIFY(requestFinishedSpy.count());
    QVERIFY(client->mRequestResult);
    
    delete client;    
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsHomescreenClientPluginTest::testSetWallpaper2()
{  
    HsHomescreenClient *client = new HsHomescreenClient;
    
    QSignalSpy requestFinishedSpy(client, SIGNAL(requestFinished()));
    
    bool result = client->setWallpaper("wallpaper", "wallpaper");
    
    // but emit correct signals
    connect(this, SIGNAL(onRequestCompletedTest(QVariant)), client, SLOT(onRequestCompleted(QVariant)));
    emit onRequestCompletedTest(true);
    
    QVERIFY(requestFinishedSpy.count());
    QVERIFY(client->mRequestResult);
    
    delete client;    
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsHomescreenClientPluginTest::testOnRequestError()
{  
    HsHomescreenClient *client = new HsHomescreenClient;
    
    QSignalSpy requestFinishedSpy(client, SIGNAL(requestFinished()));
    
    client->onRequestError(-1);
    
    QVERIFY(requestFinishedSpy.count());
    QVERIFY(!client->mRequestResult);
    
    delete client;    
}


QTEST_MAIN(HsHomescreenClientPluginTest)
