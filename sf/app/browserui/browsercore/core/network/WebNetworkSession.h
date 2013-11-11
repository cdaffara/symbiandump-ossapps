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

#ifndef WEBNETWORKSESSION_H_
#define WEBNETWORKSESSION_H_
#include "brtglobal.h"
#include <qnetworksession.h>

#if (QT_VERSION < 0x040700)
QTM_USE_NAMESPACE
#endif

namespace WRT {

class WRT_BROWSER_EXPORT WebNetworkSession : public QObject
{
Q_OBJECT
    
public: 
    WebNetworkSession(const QNetworkConfiguration &config, QObject *parent = 0);
    virtual ~WebNetworkSession();
    QNetworkConfiguration configuration() const { return m_NetworkSession->configuration(); }
    bool isOpen() const { return m_NetworkSession->isOpen(); }
    void open() { m_NetworkSession->open(); }
    
Q_SIGNALS:
	  void sessionConfigurationChanged(const QNetworkConfiguration &config);
	  void sessionStateChanged(const QNetworkConfiguration &config, 
	  	       QNetworkSession::State state);
	  void sessionRoamingRejected();
        
protected Q_SLOTS:
    virtual void handlePreferredConfigurationChanged(const QNetworkConfiguration &config, bool isSeamless);
    virtual void handleNewConfigurationActivated();
    virtual void handleStateChanged(QNetworkSession::State state);
    virtual void handleOpened();
    virtual void handleClosed();
    virtual void handleError(QNetworkSession::SessionError error);
   
private:
    QNetworkConfiguration activeConfiguration();
	
    QNetworkSession *m_NetworkSession;
};

} // namespace WRT

#endif /* WEBNETWORKSESSION_H_ */
