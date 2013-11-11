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
#include "hstitleresolver.h"


const QString KTestingOperatorName = QString("Testing");
const QString KTestingOperatorName2 = QString("Testing2");

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testTitleResolver()
    {
    HsTitleResolver *titleResolver = new HsTitleResolver();
    QCOMPARE(titleResolver->title(), titleResolver->mNoServiceText);

    titleResolver->mDeviceInfoListener->mNetworkInfo->setProperty("testNetworkMode", QVariant((int)QSystemNetworkInfo::GsmMode));
    titleResolver->mDeviceInfoListener->mNetworkInfo->setProperty("testNetworkStatus", QVariant(QSystemNetworkInfo::Roaming));
    titleResolver->mDeviceInfoListener->updateCurrentNetworkMode();
    titleResolver->mDeviceInfoListener->updateStatus();

    QCOMPARE(titleResolver->title(), KTestingOperatorName);

    titleResolver->mOperatorName.clear();
    titleResolver->mDeviceInfoListener->updateCurrentNetworkMode();
    titleResolver->mDeviceInfoListener->updateStatus();
    QCOMPARE(titleResolver->title(), KTestingOperatorName);

    delete titleResolver;

    }

void t_hsUtils::testTitleResolverOnStatusChanged()
    {
    HsTitleResolver *titleResolver = new HsTitleResolver();

    qRegisterMetaType<HsDeviceInfoListener::HsDeviceInfoStatus>(
        "HsDeviceInfoListener::HsDeviceInfoStatus");
    QSignalSpy spy(titleResolver, SIGNAL(titleChanged(const QString &)));

    titleResolver->onStatusChanged(HsDeviceInfoListener::ServiceAvailable);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), KTestingOperatorName);
    QCOMPARE(titleResolver->title(), KTestingOperatorName);
    spy.clear();
    titleResolver->onStatusChanged(HsDeviceInfoListener::ServiceAvailable);
    QCOMPARE(spy.count(), 0);

    titleResolver->onStatusChanged(HsDeviceInfoListener::OfflineProfile);
    QCOMPARE(spy.count(), 1);
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), titleResolver->mOfflineText);
    QCOMPARE(titleResolver->title(), titleResolver->mOfflineText);

    delete titleResolver;
    }

void t_hsUtils::testTitleResolverOnNetworkNameChanged()
    {
    HsTitleResolver *titleResolver = new HsTitleResolver();
    QCOMPARE(titleResolver->title(), titleResolver->mNoServiceText);

    qRegisterMetaType<HsDeviceInfoListener::HsDeviceInfoStatus>(
        "HsDeviceInfoListener::HsDeviceInfoStatus");
    QSignalSpy spy(titleResolver, SIGNAL(titleChanged(const QString &)));

    titleResolver->onNetworkNameChanged(KTestingOperatorName);
    // this should not emit anything because UnknownMode
    QCOMPARE(spy.count(), 0);

    titleResolver->mCurrentStatus = HsDeviceInfoListener::ServiceAvailable;
    titleResolver->onNetworkNameChanged(KTestingOperatorName2);
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), KTestingOperatorName2);

    spy.clear();
    titleResolver->onNetworkNameChanged(KTestingOperatorName);
    QCOMPARE(spy.count(), 1);
    arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), KTestingOperatorName);

    spy.clear();
    titleResolver->onNetworkNameChanged(KTestingOperatorName);
    QCOMPARE(spy.count(), 0);

    delete titleResolver;
    }

#endif //ONLY_MENU_TESTCASES
