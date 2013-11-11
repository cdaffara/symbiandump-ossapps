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
* Description:  Test title resolver.
*
*/

#ifndef ONLY_MENU_TESTCASES

#include "t_hsutils.h"
#include "hsdeviceinfolistener.h"


const QString KTestingOperatorName = QString("Testing");
const QString KTestingOperatorName2 = QString("Testing2");



// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testDeviceInfoListener()
    {
    HsDeviceInfoListener *infoListener = new HsDeviceInfoListener();
    QCOMPARE(infoListener->operatorName(), KTestingOperatorName);
    QCOMPARE(infoListener->status(), HsDeviceInfoListener::NoService);
    QCOMPARE(infoListener->networkStatus(), QSystemNetworkInfo::UndefinedStatus);
    infoListener->mNetworkInfo->setProperty("testNetworkMode", QVariant((int)QSystemNetworkInfo::GsmMode));
    infoListener->mNetworkInfo->setProperty("testNetworkStatus", QVariant(QSystemNetworkInfo::Roaming));
    infoListener->updateCurrentNetworkMode();
    QCOMPARE(infoListener->networkStatus(), QSystemNetworkInfo::Roaming);

    delete infoListener;
    }

void t_hsUtils::testDeviceInfoListenerNetworkMode_data()
    {
    QTest::addColumn<int>("Mode");
    QTest::addColumn<int>("Status");
    QTest::addColumn<int>("Expected");
    QTest::newRow("Gsm1")
        << (int)QSystemNetworkInfo::GsmMode
        << (int)QSystemNetworkInfo::NoNetworkAvailable
        << (int)HsDeviceInfoListener::NoService;
    QTest::newRow("Gsm2")
        << (int)QSystemNetworkInfo::GsmMode
        << (int)QSystemNetworkInfo::Connected
        << (int)HsDeviceInfoListener::ServiceAvailable;
    QTest::newRow("Gsm3")
        << (int)QSystemNetworkInfo::GsmMode
        << (int)QSystemNetworkInfo::HomeNetwork
        << (int)HsDeviceInfoListener::ServiceAvailable;
    QTest::newRow("Gsm4")
        << (int)QSystemNetworkInfo::GsmMode
        << (int)QSystemNetworkInfo::Roaming
        << (int)HsDeviceInfoListener::ServiceAvailable;
    QTest::newRow("Wcdma1")
        << (int)QSystemNetworkInfo::WcdmaMode
        << (int)QSystemNetworkInfo::NoNetworkAvailable
        << (int)HsDeviceInfoListener::NoService;
    QTest::newRow("Wcdma2")
        << (int)QSystemNetworkInfo::WcdmaMode
        << (int)QSystemNetworkInfo::Connected
        << (int)HsDeviceInfoListener::ServiceAvailable;
    QTest::newRow("Wcdma3")
        << (int)QSystemNetworkInfo::WcdmaMode
        << (int)QSystemNetworkInfo::HomeNetwork
        << (int)HsDeviceInfoListener::ServiceAvailable;
    QTest::newRow("Wcdma4")
        << (int)QSystemNetworkInfo::WcdmaMode
        << (int)QSystemNetworkInfo::Roaming
        << (int)HsDeviceInfoListener::ServiceAvailable;

    }
void t_hsUtils::testDeviceInfoListenerNetworkMode()
    {
    QFETCH(int, Mode);
    QFETCH(int, Status);
    QFETCH(int, Expected);

    HsDeviceInfoListener *infoListener = new HsDeviceInfoListener();
    infoListener->mNetworkInfo->setProperty("testNetworkMode", QVariant(Mode));
    infoListener->mNetworkInfo->setProperty("testNetworkStatus", QVariant(Status));
    infoListener->updateCurrentNetworkMode();
    infoListener->updateStatus();
    QCOMPARE(infoListener->operatorName(), KTestingOperatorName);
    QCOMPARE((int)infoListener->status(), Expected);
    delete infoListener;
    }

void t_hsUtils::testDeviceInfoListenerOfflineProfile()
    {
    HsDeviceInfoListener *infoListener = new HsDeviceInfoListener();
    infoListener->mDeviceInfo->setProperty("offlineMode", QVariant(true));
    infoListener->updateCurrentNetworkMode();
    infoListener->updateStatus();

    if (infoListener->simStatus() == QSystemDeviceInfo::SimNotAvailable) {
    	QCOMPARE(infoListener->status(), HsDeviceInfoListener::NoService);
    }
    else {
    	QCOMPARE(infoListener->status(), HsDeviceInfoListener::OfflineProfile);
    }
    delete infoListener;
    }

void t_hsUtils::testDeviceInfoListenerNetworkStatusChanged()
    {
    qRegisterMetaType<HsDeviceInfoListener::HsDeviceInfoStatus>(
        "HsDeviceInfoListener::HsDeviceInfoStatus");

    HsDeviceInfoListener *infoListener = new HsDeviceInfoListener();
    QSignalSpy spy(infoListener, SIGNAL(statusChanged(HsDeviceInfoListener::HsDeviceInfoStatus)));

    infoListener->onNetworkStatusChanged(QSystemNetworkInfo::GsmMode, QSystemNetworkInfo::Roaming);
    QCOMPARE(spy.count(), 0);

    infoListener->mNetworkInfo->setProperty("testNetworkMode", QVariant((int)QSystemNetworkInfo::GsmMode));
    infoListener->mNetworkInfo->setProperty("testNetworkStatus", QVariant(QSystemNetworkInfo::Roaming));
    infoListener->onNetworkStatusChanged(QSystemNetworkInfo::GsmMode, QSystemNetworkInfo::Roaming);
    QCOMPARE(spy.count(), 1);

    delete infoListener;
    }

void t_hsUtils::testDeviceInfoListenerNetworkNameChanged()
    {
    HsDeviceInfoListener *infoListener = new HsDeviceInfoListener();
    QSignalSpy spy(infoListener, SIGNAL(networkNameChanged(const QString &)));

    infoListener->onNetworkNameChanged(QSystemNetworkInfo::GsmMode, KTestingOperatorName2);
    // this should not emit anything because UnknownMode
    QCOMPARE(spy.count(), 0);

    infoListener->mNetworkInfo->setProperty("testNetworkMode", QVariant((int)QSystemNetworkInfo::GsmMode));
    infoListener->mNetworkInfo->setProperty("testNetworkStatus", QVariant(QSystemNetworkInfo::Roaming));
    infoListener->onNetworkNameChanged(QSystemNetworkInfo::GsmMode, KTestingOperatorName);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), KTestingOperatorName);

    delete infoListener;
    }

void t_hsUtils::testDeviceInfoListenerNetworkCurrentProfileChanged()
    {
    HsDeviceInfoListener *infoListener = new HsDeviceInfoListener();
    infoListener->mDeviceInfo->setProperty("offlineMode", QVariant(true));
    infoListener->updateCurrentNetworkMode();
    qRegisterMetaType<HsDeviceInfoListener::HsDeviceInfoStatus>(
        "HsDeviceInfoListener::HsDeviceInfoStatus");
    QSignalSpy spy(infoListener, SIGNAL(statusChanged(HsDeviceInfoListener::HsDeviceInfoStatus)));

    infoListener->mDeviceInfo->setProperty("simAvailable", QVariant(true));
    if (infoListener->simStatus() != QSystemDeviceInfo::SimNotAvailable) {
        infoListener->onCurrentProfileChanged(QSystemDeviceInfo::OfflineProfile);
        QCOMPARE(spy.count(), 1);
    }
    delete infoListener;
    }

#endif // ONLY_MENU_TESTCASES
