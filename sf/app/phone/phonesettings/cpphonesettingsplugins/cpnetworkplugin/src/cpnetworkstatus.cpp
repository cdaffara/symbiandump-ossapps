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
#include <cpsettingformitemdata.h>
#include <hbicon.h>
#include "cpnetworkstatus.h"
#include "cppluginlogging.h"
#include "cpplugincommon.h"


/*!
  CpNetworkStatus::CpNetworkStatus
 */
CpNetworkStatus::CpNetworkStatus() : 
    QObject(0),
    m_cpSettingsWrapper(NULL),
    m_settingFormItemData(NULL)
{
    DPRINT << ": IN";
    
    m_networkInfo.reset(new QSystemNetworkInfo);
    
    m_deviceInfo.reset(new QSystemDeviceInfo);
    
    connect(m_networkInfo.data(), 
            SIGNAL(networkNameChanged(
                    QSystemNetworkInfo::NetworkMode, QString)), 
            this, 
            SLOT(networkNameChanged(
                    QSystemNetworkInfo::NetworkMode, QString)));
    
    connect(m_networkInfo.data(), 
            SIGNAL(networkStatusChanged(
                    QSystemNetworkInfo::NetworkMode, 
                    QSystemNetworkInfo::NetworkStatus)),
            this, 
            SLOT(networkStatusChanged(
                    QSystemNetworkInfo::NetworkMode, 
                    QSystemNetworkInfo::NetworkStatus)));

    m_cpSettingsWrapper = new SettingsWrapper;

    DPRINT << ": OUT";
}


/*!
  CpNetworkStatus::~CpNetworkStatus
 */
CpNetworkStatus::~CpNetworkStatus()
{
    DPRINT << ": IN";
    
    disconnect(m_networkInfo.data(), 
            SIGNAL (networkNameChanged(
                    QSystemNetworkInfo::NetworkMode,QString)),
            this, 
            SLOT(networkNameChanged(
                    QSystemNetworkInfo::NetworkMode, QString)));
                
    disconnect(m_networkInfo.data(), 
            SIGNAL(networkStatusChanged(
                    QSystemNetworkInfo::NetworkMode, 
                    QSystemNetworkInfo::NetworkStatus)), 
            this, 
            SLOT(networkStatusChanged(
                    QSystemNetworkInfo::NetworkMode, 
                    QSystemNetworkInfo::NetworkStatus)));

    delete m_cpSettingsWrapper;
    m_cpSettingsWrapper = NULL;
    
    DPRINT << ": OUT";
}

    
/*!
  CpNetworkStatus::statusText
 */
QString CpNetworkStatus::statusText() const
{
    DPRINT << ": IN";
    
    QString statusText;
    
    if ((QSystemDeviceInfo::SimNotAvailable == m_deviceInfo->simStatus()) ||
        ( m_cpSettingsWrapper->isPhoneOffline())) {
        statusText = hbTrId("txt_cp_dblist_mobile_network_val_off");
    } else if (connectedToNetwork()) {
        statusText = m_networkInfo->networkName(QSystemNetworkInfo::WcdmaMode);
        if(statusText.isEmpty()) {
            statusText = m_networkInfo->networkName(QSystemNetworkInfo::GsmMode);
        }
    } else {
        statusText = hbTrId("txt_cp_dblist_mobile_network_val_not_connected");
    }
    
    DPRINT << ": OUT";
    return statusText;
}

/*!
  CpNetworkStatus::statusIcon
 */
HbIcon CpNetworkStatus::statusIcon() const
{
    DPRINT << ": IN";
    
    HbIcon statusIcon(statusIconLocicalName());
    
    DPRINT << ": OUT";
    return statusIcon;
}

/*!
  CpNetworkStatus::statusIconLocicalName
 */
QString CpNetworkStatus::statusIconLocicalName() const
{
    DPRINT << ": IN";
    
    QString statusIcon("");
    
    if ((QSystemDeviceInfo::SimNotAvailable == m_deviceInfo->simStatus()) ||
       (m_cpSettingsWrapper->isPhoneOffline())) {
        statusIcon = QString("qtg_large_network_off");
    } else if (connectedToNetwork()) {
        statusIcon = QString("qtg_large_network");
    } else {
        statusIcon = QString("qtg_large_network_off");
    }
    
    DPRINT << ": OUT";
    return statusIcon;
}

/*!
  CpNetworkStatus::networkNameChanged
 */
void CpNetworkStatus::networkNameChanged(
        QSystemNetworkInfo::NetworkMode mode, const QString &)
{
    DPRINT << ": IN";
    
    if (m_settingFormItemData &&
        ((QSystemNetworkInfo::GsmMode == mode) || 
        (QSystemNetworkInfo::WcdmaMode == mode) || 
        (QSystemNetworkInfo::UnknownMode == mode))) {
        m_settingFormItemData->setDescription(statusText());
        m_settingFormItemData->setIcon(statusIconLocicalName());
    }
    
    DPRINT << ": OUT";
}

/*!
  CpNetworkStatus::networkStatusChanged
 */
void CpNetworkStatus::networkStatusChanged(
        QSystemNetworkInfo::NetworkMode mode, QSystemNetworkInfo::NetworkStatus)
{
    DPRINT << ": IN";
    
    if (m_settingFormItemData &&
        ((QSystemNetworkInfo::GsmMode == mode) || 
        (QSystemNetworkInfo::WcdmaMode == mode) || 
        (QSystemNetworkInfo::UnknownMode == mode))) {
        m_settingFormItemData->setDescription(statusText());
        m_settingFormItemData->setIcon(statusIconLocicalName());
    }
    
    DPRINT << ": OUT";
}
/*!
  CpNetworkStatus::setSettingFormItemData
 */
void CpNetworkStatus::setSettingFormItemData(CpSettingFormItemData *data)
{
    m_settingFormItemData = data;
}

/*!
  CpNetworkStatus::connectedToNetwork
 */
bool CpNetworkStatus::connectedToNetwork() const
{
    DPRINT << ": IN";
    
    bool connected(false);
    QSystemNetworkInfo::NetworkStatus wcdmaStatus = 
            m_networkInfo->networkStatus(QSystemNetworkInfo::WcdmaMode);
    QSystemNetworkInfo::NetworkStatus gsmStatus = 
            m_networkInfo->networkStatus(QSystemNetworkInfo::GsmMode);
    
    if (QSystemNetworkInfo::Connected == wcdmaStatus ||
        QSystemNetworkInfo::HomeNetwork == wcdmaStatus ||
        QSystemNetworkInfo::Roaming == wcdmaStatus ||
        QSystemNetworkInfo::Connected == gsmStatus ||
        QSystemNetworkInfo::HomeNetwork == gsmStatus ||
        QSystemNetworkInfo::Roaming == gsmStatus) {
        connected = true;
    }
    
    DPRINT << ": OUT";
    return connected;
}

