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
* This file implements the SystemDeviceImpl class.
*/
#include <QList>
#include <QString>
#include "SystemDeviceImpl.h"
#include "Utilities.h"

namespace GVA {

SystemDeviceImpl::SystemDeviceImpl()
{
    // create Qt Mobility API objects for device info
    m_deviceInfo = new QSystemDeviceInfo(this);

    // When the m_deviceInfo signals battery level changed,
    // DeviceDelegate emits batteryLevelChanged
    safe_connect(m_deviceInfo, SIGNAL(batteryLevelChanged(int)),
                 this, SIGNAL(batteryLevelChanged(int)));

    // set current charging state then keep it up to date with signal handler
    m_batteryCharging = (m_deviceInfo->currentPowerState() ==
        QSystemDeviceInfo::WallPowerChargingBattery) ? true : false;
    safe_connect(m_deviceInfo, SIGNAL(powerStateChanged(QSystemDeviceInfo::PowerState)),
                 this, SLOT(handlePowerStateChanged(QSystemDeviceInfo::PowerState)));
}

SystemDeviceImpl::~SystemDeviceImpl()
{
    // clean up
    delete m_deviceInfo;
}

//! Get the current battery level.
int SystemDeviceImpl::getBatteryLevel() const
{
    return (m_deviceInfo->batteryLevel());
}

//! Handles the powerStateChanged signal from system device info.
/*!
  \param state new power state
*/
void SystemDeviceImpl::handlePowerStateChanged(QSystemDeviceInfo::PowerState state)
{
    bool batteryCharging =
        (state == QSystemDeviceInfo::WallPowerChargingBattery) ? true : false;

    if (batteryCharging != m_batteryCharging) {
        m_batteryCharging = batteryCharging;
        //qDebug() << "DeviceDelegate: new charging = " << m_batteryCharging;
        // emit battery level - subscriber will get charging state if desired
        emit batteryLevelChanged(m_deviceInfo->batteryLevel());
    }
}

} // GVA

