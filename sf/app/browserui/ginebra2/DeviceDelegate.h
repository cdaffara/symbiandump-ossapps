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
*  This file defines the DeviceDelegate, DeviceImpl and DefaultDeviceImpl classes.
*/
#ifndef DEVICEDELEGATE_H
#define DEVICEDELEGATE_H

#include <QObject>
#include <QString>

#ifdef QT_MOBILITY_SYSINFO
#define DEVICEIMPL SystemDeviceImpl
#else
#define DEVICEIMPL DefaultDeviceImpl
#endif // QT_MOBILITY_SYSINFO

namespace GVA {

/*!
  Class to provide device information. It uses QtMobility QSystemDeviceInfo
  to provide information about battery level.
*/
class DeviceImpl : public QObject
{
      Q_OBJECT
public:
      DeviceImpl() :  m_batteryCharging(false) {};
      virtual ~DeviceImpl() {};

    virtual int getBatteryLevel() const = 0;
    virtual bool isBatteryCharging() const = 0;

  signals:
    // Sent when the battery level or charging state changes.
    void batteryLevelChanged(int);

protected:
    bool m_batteryCharging;
};

class DefaultDeviceImpl : public DeviceImpl
{
      Q_OBJECT
public:
      DefaultDeviceImpl() {};
      ~DefaultDeviceImpl() {};

    virtual int getBatteryLevel() const { return 100; }; // can't get real level, return full
    virtual bool isBatteryCharging() const { return m_batteryCharging; };
};

class DeviceDelegate : public QObject {
    Q_OBJECT
public:
    DeviceDelegate(DeviceImpl *deviceImpl);
    virtual ~DeviceDelegate();

    // properties accessible to javascript snippets
    Q_PROPERTY(int batteryLevel READ getBatteryLevel)
    Q_PROPERTY(bool batteryCharging READ isBatteryCharging)

    int getBatteryLevel() const;
    bool isBatteryCharging() const;

signals:
    // Sent when the battery level or charging state changes.
    void batteryLevelChanged(int);

private:
      DeviceImpl *m_deviceImpl;
};

} // GVA

#endif // DEVICEDELEGATE_H
