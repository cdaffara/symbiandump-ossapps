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
* Description:  Mock implementation for system info for testing
*
*/


#include <QVariant>

#include "mocksysteminfo.h"


QSystemNetworkInfo::QSystemNetworkInfo(QObject *parent) : QObject(parent)
{
}

QSystemNetworkInfo::~QSystemNetworkInfo()
{
}

QSystemNetworkInfo::NetworkStatus QSystemNetworkInfo::networkStatus(QSystemNetworkInfo::NetworkMode mode)
{
// property 'testNetworkMode' -> GsmMode / WcdmaMode, others return NoNetworkAvailable
// property 'testNetworkStatus' -> NoNetworkAvailable / Connected / HomeNetwork, others return NoNetworkAvailable
    QSystemNetworkInfo::NetworkStatus networkStatus = NoNetworkAvailable;
    int modeProperty = property("testNetworkMode").toInt();
    int statusProperty = property("testNetworkStatus").toInt();
    if ( modeProperty == mode ) {
        networkStatus = (QSystemNetworkInfo::NetworkStatus) statusProperty;
    }
    return networkStatus;
}

int QSystemNetworkInfo::networkSignalStrength(QSystemNetworkInfo::NetworkMode mode)
{
    Q_UNUSED(mode);
    return -1;
}

QString QSystemNetworkInfo::networkName(QSystemNetworkInfo::NetworkMode mode)
{
    Q_UNUSED(mode);
    return QString("Testing");
}


QSystemDeviceInfo::QSystemDeviceInfo(QObject *parent) : QObject(parent)
{
}

QSystemDeviceInfo::~QSystemDeviceInfo()
{
}

QSystemDeviceInfo::SimStatus QSystemDeviceInfo::simStatus()
{
    if (property("simAvailable").toBool()) {
        return SingleSimAvailable;
    }
    return SimNotAvailable;
}

QSystemDeviceInfo::Profile QSystemDeviceInfo::currentProfile()
{
    if (property("offlineMode").toBool()) {
        return OfflineProfile;
    }
    return NormalProfile;
}

// End of file

