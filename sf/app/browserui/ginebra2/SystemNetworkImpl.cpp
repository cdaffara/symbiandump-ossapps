/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*  This file implements the SystemNetworkImpl class.
*/
#include <QList>
#include <QString>
#ifdef QT_MOBILITY_BEARER_MANAGEMENT
#include "WebNetworkConnectionManagerSingleton.h"
#endif // QT_MOBILITY_BEARER_MANAGEMENT
#include "NetworkDelegate.h"
#include "SystemNetworkImpl.h"
#include "Utilities.h"

namespace GVA {

#define WCDMA2GSM_WORKAROUND // work around for QtMobility issue

SystemNetworkImpl::SystemNetworkImpl()
    : m_currentMode(QSystemNetworkInfo::UnknownMode),
      m_sessionNetworkName("")
{
    // create Qt Mobility API objects for network information
    m_networkInfo = new QSystemNetworkInfo(this);

    // set up handlers for system network info signals
    safe_connect(m_networkInfo, SIGNAL(networkModeChanged(
        QSystemNetworkInfo::NetworkMode)), this,
        SLOT(handleNetworkModeChanged(QSystemNetworkInfo::NetworkMode)));
    safe_connect(m_networkInfo, SIGNAL(networkNameChanged(
        QSystemNetworkInfo::NetworkMode, const QString&)), this,
        SLOT(handleNetworkNameChanged(QSystemNetworkInfo::NetworkMode, const QString&)));
    safe_connect(m_networkInfo, SIGNAL(networkSignalStrengthChanged(
        QSystemNetworkInfo::NetworkMode, int)), this,
        SLOT(handleNetworkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode, int)));
    safe_connect(m_networkInfo, SIGNAL(networkStatusChanged(
        QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus)), this,
        SLOT(handleNetworkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus)));

#ifdef QT_MOBILITY_BEARER_MANAGEMENT
    // Get the singleton instance of WebNetworkConnectionManager 
    WRT::WebNetworkConnectionManager &webNetworkConnectionManager 
    	  = WRT::WebNetworkConnectionManagerSingleton::Instance();
    
    safe_connect(&webNetworkConnectionManager, SIGNAL(networkOnlineStateChanged(bool)),
        this, SLOT(handleNetworkOnlineStateChanged(bool)));
    safe_connect(&webNetworkConnectionManager, SIGNAL(networkSessionNameChanged(
        QSystemNetworkInfo::NetworkMode, const QString&)), this,
        SLOT(handleNetworkSessionNameChanged(QSystemNetworkInfo::NetworkMode, const QString&)));

      // Update all configurations
    webNetworkConnectionManager.updateConfigurations();
#endif // QT_MOBILITY_BEARER_MANAGEMENT
}

SystemNetworkImpl::~SystemNetworkImpl()
{
    delete m_networkInfo;
}

//! Gets the network name for the current network mode.
QString SystemNetworkImpl::getNetworkName() const
{
    QString netName;

    switch(m_currentMode) {
        case QSystemNetworkInfo::WlanMode:
        case QSystemNetworkInfo::EthernetMode:
        case QSystemNetworkInfo::BluetoothMode:
        case QSystemNetworkInfo::WimaxMode:
            // for wireless cases use name from session.
#ifdef QT_MOBILITY_BEARER_MANAGEMENT
            netName = m_sessionNetworkName;
#else
            netName = m_networkInfo->networkName(m_currentMode);
            // if WLAN SSID name is unknown show "WiFi"
            if (netName == "")
                netName = "WiFi";
#endif // QT_MOBILITY_BEARER_MANAGEMENT
            break;

        default:
            netName = m_networkInfo->networkName(m_currentMode);
            break;
    }

    qDebug() << "SystemNetworkImpl::getNetworkName: network name " << netName;
    return (netName);
}

//! Gets the network signal strength for the current network mode.
int SystemNetworkImpl::getNetworkSignalStrength() const
{
    int strength = m_networkInfo->networkSignalStrength(m_currentMode);
    
    // Strength in WLAN mode is reported as -1 by QtMobility
    if ((strength == -1) && (m_currentMode == QSystemNetworkInfo::WlanMode)) {
        strength = 100;
    }
    
    return (strength);
}

//! Handles the networkNetworkModeChanged signal from system network info.
/*!
  \param mode network mode of connection that changed
*/
void SystemNetworkImpl::handleNetworkModeChanged(
    QSystemNetworkInfo::NetworkMode mode)
{
    qDebug() << "SystemNetworkImpl::handleNetworkModeChanged" << "Mode:" << mode;
}

//! Handles the networkSignalStrengthChanged signal from system network info.
/*!
  \param mode network mode of connection that changed
  \param strength new signal strength
*/
void SystemNetworkImpl::handleNetworkSignalStrengthChanged(
    QSystemNetworkInfo::NetworkMode mode, int strength)
{
    qDebug() << "SystemNetworkImpl::handleNetworkSignalStrengthChanged" << "Mode:" << mode << "strength:" << strength;
    
    // Only send signal strength changes for current mode.
    if (mode == m_currentMode) {
        // Unknown mode could mean network error so send negative strength to indicate offline.
        if (m_currentMode == QSystemNetworkInfo::UnknownMode)
            strength = -1;

        qDebug() << "SystemNetworkImpl::handleNetworkSignalStrengthChanged" << "emit strength:" << strength;
        emit networkSignalStrengthChanged(strength);
    }
}

//! Handles the networkNameChanged signal from system network info.
/*!
  \param mode network mode of connection that changed
  \param name new network name
*/
void SystemNetworkImpl::handleNetworkNameChanged(
        QSystemNetworkInfo::NetworkMode mode, const QString& name)
{
    qDebug() << "SystemNetworkImpl::handleNetworkNameChanged" << "Mode:" << mode << "name:" << name;
    
    // Only send network name changes for current mode.
    if (mode == m_currentMode) {
        // Now update name.
        emit networkNameChanged(name);
    }
}

//! Handles the networkStatusChanged signal from system network info.
/*!
  \param mode network mode of connection that changed
  \param status network status of connection that changed
*/
void SystemNetworkImpl::handleNetworkStatusChanged(
        QSystemNetworkInfo::NetworkMode mode, QSystemNetworkInfo::NetworkStatus status)
{
    qDebug() << "SystemNetworkImpl::handleNetworkStatusChanged" << "mode: " << mode << "status: " << status; 
}

#ifdef QT_MOBILITY_BEARER_MANAGEMENT
//! Handles online state changed signal from the web network connection manager.
/*!
  \param mode network mode of connection that changed
*/
void SystemNetworkImpl::handleNetworkOnlineStateChanged(bool isOnline)
{
    qDebug() << "SystemNetworkImpl::handleOnlineStateChanged" << "isOnline:" << isOnline;
    
    // Offline indicates no active network configurations.
    if (!isOnline) {
        qDebug() << "SystemNetworkImpl::handleOnlineStateChanged: change mode to unknown, emit -1 str";
        m_currentMode = QSystemNetworkInfo::UnknownMode;
        // negative strength indicates offline to UI
        emit networkSignalStrengthChanged(-1);
    }
    // Online indicates at least 1 active network config but not necessarily 
    // one being used by the browser.
}

void SystemNetworkImpl::handleNetworkSessionNameChanged(QSystemNetworkInfo::NetworkMode mode, const QString& name)
{
    // UI must get non-negative strength to indicate online status
    int strength = m_networkInfo->networkSignalStrength(mode);
    
    qDebug() << "SystemNetworkImpl::handleNetworkSessionNameChanged" << "Mode:" << mode << "name:" << name << "strength:" << strength;
    
    switch (mode) {
        case QSystemNetworkInfo::WlanMode:
        case QSystemNetworkInfo::EthernetMode:
        case QSystemNetworkInfo::BluetoothMode:
        case QSystemNetworkInfo::WimaxMode:
            // for wireless cases use name from session.
            m_sessionNetworkName = name;
            break;

        default:
            // clear session name - not needed in this mode
            m_sessionNetworkName = "";
            break;
    }

#ifdef WCDMA2GSM_WORKAROUND
    // Work around for QtMobility issue. Bearer management reports WCDMA bearer but
    // QSystemNetworkInfo sees connection as GSM mode. 
    if ((mode == QSystemNetworkInfo::WcdmaMode) && (strength < 0)) {
        strength = m_networkInfo->networkSignalStrength(QSystemNetworkInfo::GsmMode);
        if (strength >= 0)
            mode = QSystemNetworkInfo::GsmMode;
    }
#endif

    qDebug() << "SystemNetworkImpl::handleNetworkSessionNameChanged: set mode to " << mode;
    m_currentMode = mode;

    // emit signal strength of new connection,
    // use wrapper access method for correct WLAN strength
    qDebug() << "SystemNetworkImpl::handleNetworkSessionNameChanged: emit str=" << getNetworkSignalStrength();
    emit networkSignalStrengthChanged(getNetworkSignalStrength());

    // Update network name on mode change.
    qDebug() << "SystemNetworkImpl::handleNetworkSessionNameChanged: emit network name= " << getNetworkName();
    emit networkNameChanged(getNetworkName());
}
#endif // QT_MOBILITY_BEARER_MANAGEMENT

} // GVA
