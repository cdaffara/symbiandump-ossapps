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
*  This file defines the NetworkDelegate, NetworkImpl and DefaultNetworkImpl classes.
*/
#ifndef NETWORKDELEGATE_H
#define NETWORKDELEGATE_H

#include <QObject>
#include <QString>

#if !defined(QT_MOBILITY_SYSINFO) && defined(QT_MOBILITY_BEARER_MANAGEMENT)
#include "WebNetworkConnectionManagerSingleton.h"
#endif

#ifdef QT_MOBILITY_SYSINFO
#define NETWORKIMPL SystemNetworkImpl
#else
#define NETWORKIMPL DefaultNetworkImpl
#endif // QT_MOBILITY_SYSINFO

namespace GVA {

/*!
  Class to provide device information. It uses QtMobility to provide
  information about battery level, network signal strength, and network name.
*/
class NetworkImpl : public QObject
{
    Q_OBJECT
public:
    NetworkImpl() {};
    virtual ~NetworkImpl() {};

    virtual QString getNetworkName() const = 0;
    virtual int getNetworkSignalStrength() const = 0;

signals:
      // Sent when the network name changes.
    void networkNameChanged(const QString&);
    // Sent when the network signal strength changes.
    void networkSignalStrengthChanged(int);
};

class DefaultNetworkImpl : public NetworkImpl
{
      Q_OBJECT
public:
    DefaultNetworkImpl() 
    {
#if !defined(QT_MOBILITY_SYSINFO) && defined(QT_MOBILITY_BEARER_MANAGEMENT)
     // Get the singleton instance of WebNetworkConnectionManager  
     WRT::WebNetworkConnectionManagerSingleton::Instance().updateConfigurations();
#endif    	
    };
    ~DefaultNetworkImpl() {};

    virtual QString getNetworkName() const { return ""; };
    virtual int getNetworkSignalStrength() const { return 100; };
};

/*!
  Class to provide device information. It uses QtMobility to provide
  information about battery level, network signal strength, and network name.
*/
class NetworkDelegate : public QObject
{
    Q_OBJECT
public:
    NetworkDelegate(NetworkImpl *networkImpl);
    ~NetworkDelegate();

    // properties accessible to javascript snippets
    Q_PROPERTY(int networkSignalStrength READ getNetworkSignalStrength)
    Q_PROPERTY(QString networkName READ getNetworkName)

    QString getNetworkName() const;
    int getNetworkSignalStrength() const;

signals:
      // Sent when the network name changes.
    void networkNameChanged(const QString&);
    // Sent when the network signal strength changes.
    void networkSignalStrengthChanged(int);

private:
      NetworkImpl *m_networkImpl;
};

} // GVA

#endif // NETWORKDELEGATE_H
