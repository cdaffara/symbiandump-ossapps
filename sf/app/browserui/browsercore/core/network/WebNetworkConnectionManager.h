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
*
*/

#ifndef WEBNETWORKCONNECTIONMANAGER_H_
#define WEBNETWORKCONNECTIONMANAGER_H_
#include "brtglobal.h"
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>
#ifdef QT_MOBILITY_SYSINFO
#include <qsysteminfo.h>
#endif // QT_MOBILITY_SYSINFO

QTM_USE_NAMESPACE

namespace WRT {
	
class WebNetworkSession;

class WRT_BROWSER_EXPORT WebNetworkConnectionManager : public QObject
{
    Q_OBJECT
    
public:
    WebNetworkConnectionManager(QObject *parent = 0);
    virtual ~WebNetworkConnectionManager();
    
    void updateConfigurations();
    QNetworkConfiguration defaultConfiguration() const;
    void createSession(QNetworkConfiguration config);
    void deleteSession();
    bool isOfflined() const { return m_offlined; }

#ifdef QT_MOBILITY_SYSINFO
Q_SIGNALS:
	  void networkOnlineStateChanged(bool isOnline);
	  void networkSessionNameChanged(QSystemNetworkInfo::NetworkMode mode, const QString &netName);
#endif // QT_MOBILITY_SYSINFO

protected Q_SLOTS:
    virtual void handleConfigurationUpdateCompleted();
    virtual void handleConfigurationAdded(const QNetworkConfiguration& config);
    virtual void handleConfigurationRemoved(const QNetworkConfiguration& config);
    virtual void handleOnlineStateChanged(bool isOnline);
    virtual void handleConfigurationChanged(const QNetworkConfiguration& config);
    virtual void handleSessionConfigurationChanged(const QNetworkConfiguration &config);
    virtual void handleSessionRoamingRejected();
    virtual void handleHideSecureIcon();
       
private:
#ifdef QT_MOBILITY_SYSINFO
	  void initializeMapString(void);
	  
	  QMap<QString, QSystemNetworkInfo::NetworkMode> m_mapStringNetworkMode; 
#endif // QT_MOBILITY_SYSINFO

    QNetworkConfigurationManager m_NetworkConfigurationManager;
    WebNetworkSession *m_WebNetworkSession;
    bool m_offlined;
    bool m_roamingRejected;
};

} // namesspace WRT
#endif /* WEBNETWORKCONNECTIONMANAGER_H_ */
