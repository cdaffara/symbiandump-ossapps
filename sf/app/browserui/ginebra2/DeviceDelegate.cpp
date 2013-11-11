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
*   This file implements the DeviceDelegate class.
*/
#include <QList>
#include <QString>
#include "DeviceDelegate.h"
#include "Utilities.h"

namespace GVA {

DeviceDelegate::DeviceDelegate(DeviceImpl *deviceImpl = NULL)
{
      m_deviceImpl = deviceImpl;
      if (m_deviceImpl)
      {
          // When the m_deviceInfo signals battery level changed,
        // DeviceDelegate emits batteryLevelChanged
        safe_connect(m_deviceImpl, SIGNAL(batteryLevelChanged(int)),
            this, SIGNAL(batteryLevelChanged(int)));
      }

    // this will be the name javascript uses to access properties and signals
    // from this class
    setObjectName("deviceDelegate");
}

DeviceDelegate::~DeviceDelegate()
{
      // cleanup
      if (m_deviceImpl)
        delete m_deviceImpl;
}

//! Gets the current battery level.
int DeviceDelegate::getBatteryLevel() const
{
    return m_deviceImpl->getBatteryLevel();
}

//! Returns true if the battery is charging.
bool DeviceDelegate::isBatteryCharging() const
{
    return m_deviceImpl->isBatteryCharging();
}

} // GVA

