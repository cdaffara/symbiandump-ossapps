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
#ifndef HSDEVICEINFOLISTENER_H
#define HSDEVICEINFOLISTENER_H

#include <QObject>
//#include <QSystemDeviceInfo>
//#include <QSystemNetworkInfo>
#ifdef HSUTILS_TEST
#include "mocksysteminfo.h"
#else
#include <QSystemInfo>
QTM_USE_NAMESPACE
#endif

#include "hstest_global.h"
HOMESCREEN_TEST_CLASS(t_hsUtils)

class HsDeviceInfoListener : public QObject
{
    Q_OBJECT
    
    Q_ENUMS(HsDeviceInfoStatus)

public:    
    enum HsDeviceInfoStatus {
        NoService = 0, // no sim or out of coverage
        OfflineProfile,
        ServiceAvailable
    };
    
public:
    HsDeviceInfoListener(QObject *parent = 0);
    ~HsDeviceInfoListener();

public:
    QString operatorName() const;
    HsDeviceInfoStatus status() const;
    
signals:
    void networkNameChanged(const QString &name);
    void statusChanged(HsDeviceInfoListener::HsDeviceInfoStatus status);

private slots:
    void onNetworkStatusChanged(QSystemNetworkInfo::NetworkMode networkMode, QSystemNetworkInfo::NetworkStatus networkStatus);
    void onNetworkNameChanged(QSystemNetworkInfo::NetworkMode networkMode,const QString &networkName);
    void onCurrentProfileChanged(QSystemDeviceInfo::Profile profile);

private:
    QSystemNetworkInfo::NetworkStatus networkStatus();
    void updateCurrentNetworkMode();
    bool isConnected(QSystemNetworkInfo::NetworkMode mode);
    void updateStatus();
    QSystemDeviceInfo::SimStatus simStatus() const;
private:

    QSystemNetworkInfo *mNetworkInfo;
    QSystemDeviceInfo *mDeviceInfo;
    QSystemNetworkInfo::NetworkMode mCurrentNetworkMode;
    HsDeviceInfoStatus mStatus;
    HOMESCREEN_TEST_FRIEND_CLASS(t_hsUtils)
};

#endif // 
