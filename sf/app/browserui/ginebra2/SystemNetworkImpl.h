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
*  This file defines the SystemNetworkImpl class.
*/
#ifndef SYSTEMNETWORKIMPLEMENTOR_H
#define SYSTEMNETWORKIMPLEMENTOR_H

#include <QObject>
#include <QString>
#include <qsysteminfo.h>
#ifdef QT_MOBILITY_BEARER_MANAGEMENT
#include "WebNetworkConnectionManagerSingleton.h"
#endif // QT_MOBILITY_BEARER_MANAGEMENT
#include "NetworkDelegate.h"

QTM_USE_NAMESPACE

namespace GVA {

class SystemNetworkImpl : public NetworkImpl
{
      Q_OBJECT
public:
    SystemNetworkImpl();
    ~SystemNetworkImpl();

    virtual QString getNetworkName() const;
    virtual int getNetworkSignalStrength() const;

private slots:
      void handleNetworkModeChanged(QSystemNetworkInfo::NetworkMode mode);
      void handleNetworkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode mode, int strength);
      void handleNetworkNameChanged(QSystemNetworkInfo::NetworkMode mode, const QString& name);
      void handleNetworkStatusChanged(QSystemNetworkInfo::NetworkMode mode,
        QSystemNetworkInfo::NetworkStatus status);
#ifdef QT_MOBILITY_BEARER_MANAGEMENT
      void handleNetworkOnlineStateChanged(bool isOnline);
      void handleNetworkSessionNameChanged(QSystemNetworkInfo::NetworkMode mode, const QString& name);
#endif // QT_MOBILITY_BEARER_MANAGEMENT

private:
      QSystemNetworkInfo *m_networkInfo;
      QSystemNetworkInfo::NetworkMode m_currentMode;
      QString m_sessionNetworkName;
};

} // GVA

#endif // SYSTEMNETWORKIMPLEMENTOR_H
