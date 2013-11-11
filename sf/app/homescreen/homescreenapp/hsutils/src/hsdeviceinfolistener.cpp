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

#include "hsdeviceinfolistener.h"
#include <QDebug>

#ifdef Q_OS_SYMBIAN
#include <e32property.h>
#include <startupdomainpskeys.h>
#endif

/*!
    \class HsDeviceInfoListener
    \ingroup group_hsutils
    \brief 
*/

/*!

*/
HsDeviceInfoListener::HsDeviceInfoListener(QObject *parent)
    : QObject(parent),
      mCurrentNetworkMode(QSystemNetworkInfo::UnknownMode),
      mStatus(NoService)
{
    mNetworkInfo = new QSystemNetworkInfo(this);
    mDeviceInfo = new QSystemDeviceInfo(this);
    
    connect(mNetworkInfo, SIGNAL(networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus)), SLOT(onNetworkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus)));
    connect(mNetworkInfo, SIGNAL(networkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &)), SLOT(onNetworkNameChanged(QSystemNetworkInfo::NetworkMode,const QString &)));
    connect(mDeviceInfo, SIGNAL(currentProfileChanged(QSystemDeviceInfo::Profile)), SLOT(onCurrentProfileChanged(QSystemDeviceInfo::Profile)));
    
    updateCurrentNetworkMode();
    updateStatus();
}

/*!

*/
HsDeviceInfoListener::~HsDeviceInfoListener()
{
}

/*!

*/
QString HsDeviceInfoListener::operatorName() const
{
    return QSystemNetworkInfo::networkName(mCurrentNetworkMode);
}

/*!

*/
HsDeviceInfoListener::HsDeviceInfoStatus HsDeviceInfoListener::status() const
{
    return mStatus;
}

/*!

*/
void HsDeviceInfoListener::onNetworkStatusChanged(QSystemNetworkInfo::NetworkMode networkMode, QSystemNetworkInfo::NetworkStatus networkStatus)
{
    Q_UNUSED(networkMode);
    Q_UNUSED(networkStatus);
    updateCurrentNetworkMode();
    updateStatus();
}

/*!

*/
void HsDeviceInfoListener::onNetworkNameChanged(QSystemNetworkInfo::NetworkMode networkMode,const QString &networkName)
{
    Q_UNUSED(networkMode);
    updateCurrentNetworkMode();
    if(mCurrentNetworkMode != QSystemNetworkInfo::UnknownMode) {
        qDebug() << "HsDeviceInfoListener::onNetworkNameChanged() - " << networkName;
        emit networkNameChanged(networkName);
    }
    updateStatus();
}

/*!

*/
void HsDeviceInfoListener::onCurrentProfileChanged(QSystemDeviceInfo::Profile profile)
{
    Q_UNUSED(profile);
    updateStatus();
}

/*!

*/
QSystemNetworkInfo::NetworkStatus HsDeviceInfoListener::networkStatus()
{
    return mNetworkInfo->networkStatus(mCurrentNetworkMode);
}

/*!

*/
void HsDeviceInfoListener::updateCurrentNetworkMode()
{
    //We are keen only GSM and WCDMA network connections.
    //Other networks (WLAN etc.) are treated as UnknowMode.
    qDebug() << "HsDeviceInfoListener::updateCurrentNetworkMode() - Entry: mCurrentNetworkMode == "
             << mCurrentNetworkMode;
    if(isConnected(QSystemNetworkInfo::GsmMode)) {
        mCurrentNetworkMode = QSystemNetworkInfo::GsmMode;
    } else if (isConnected(QSystemNetworkInfo::WcdmaMode)) {
        mCurrentNetworkMode = QSystemNetworkInfo::WcdmaMode;
    } else {
        mCurrentNetworkMode = QSystemNetworkInfo::UnknownMode;
    }
    qDebug() << "HsDeviceInfoListener::updateCurrentNetworkMode() - Exit: mCurrentNetworkMode == "
             << mCurrentNetworkMode;
}

/*!

*/
bool HsDeviceInfoListener::isConnected(QSystemNetworkInfo::NetworkMode mode)
{
    bool connected = false;

    qDebug() << "HsDeviceInfoListener::isConnected():";
    qDebug() << "   - Mode:" << (mode == QSystemNetworkInfo::WcdmaMode ? "WcdmaMode" : "GsmMode");
    QSystemNetworkInfo::NetworkStatus status = mNetworkInfo->networkStatus(mode);
    switch (status) {
        case QSystemNetworkInfo::Connected:
            qDebug() << "   - Status: Connected";
            //Fall through
        case QSystemNetworkInfo::HomeNetwork:
            qDebug() << "   - Status: HomeNetwork";
            //Fall through
        case QSystemNetworkInfo::Roaming:
            qDebug() << "   - Status: Roaming";
            connected = true;
            break;
        default:
            qDebug() << "   - Status: " << status;
            break;
    };

    qDebug() << "   - Is connected:" << connected;
    
    return connected;
}

/*!

*/
void HsDeviceInfoListener::updateStatus()
{
    qDebug() << "HsDeviceInfoListener::updateStatus() - Entry: mStatus == "
             << mStatus;

    HsDeviceInfoStatus currentStatus;
    if ( mDeviceInfo->currentProfile() == QSystemDeviceInfo::OfflineProfile &&
         simStatus() != QSystemDeviceInfo::SimNotAvailable) {
        currentStatus = OfflineProfile;
    } else if (mCurrentNetworkMode == QSystemNetworkInfo::UnknownMode) {
        currentStatus = NoService;
    } else {
        currentStatus = ServiceAvailable;
    }

    if(mStatus != currentStatus) {
        mStatus = currentStatus;
        emit statusChanged(mStatus);
    }
    qDebug() << "HsDeviceInfoListener::updateStatus() - Exit: mStatus == "
             << mStatus;
}

/*!

*/
QSystemDeviceInfo::SimStatus HsDeviceInfoListener::simStatus() const
{
    QSystemDeviceInfo::SimStatus qtSimStatus = QSystemDeviceInfo::SimNotAvailable;

#ifdef Q_OS_SYMBIAN
    TInt simCardStatus(ESimNotPresent);
    RProperty symbianSimStatus;
    qt_symbian_throwIfError( symbianSimStatus.Attach( KPSUidStartup, KPSSimStatus ) );
    qt_symbian_throwIfError( symbianSimStatus.Get( simCardStatus ) );
    symbianSimStatus.Close();
    
    switch (simCardStatus) {
        case ESimUsable:
            // The Sim card is fully usable.
            qtSimStatus = QSystemDeviceInfo::SingleSimAvailable;
            qDebug() << "HsDeviceInfoListener::simStatus() - SingleSimAvailable";
            break;
        default:
            //ESimReadable == The SIM card is not fully usable, but the emergency number can be read.
            //ESimNotReady == The Sim card is present but not ready or usable.
            //ESimNotPresent == The Sim card is not present.
            //ESimNotSupported == SIM/RUIM is not supported. Some CDMA phones do not support a SIM/RUIM at all.
            qtSimStatus = QSystemDeviceInfo::SimNotAvailable;
            qDebug() << "HsDeviceInfoListener::simStatus() - SimNotAvailable";
            break;
    };
#else
    qtSimStatus = mDeviceInfo->simStatus();
#endif

    return qtSimStatus;
}
