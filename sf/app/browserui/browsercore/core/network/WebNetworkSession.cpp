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

#include "WebNetworkSession.h"
#include "webpagecontroller.h"

namespace WRT {
	
/*!
    Constructs a Web Network Session based on \a QNetworkConfiguration with the given \a parent.

    \sa QNetworkConfiguration
*/
WebNetworkSession::WebNetworkSession(const QNetworkConfiguration &config, QObject *parent)
    : QObject(parent)
{   
    m_NetworkSession = new QNetworkSession(config);
    
    // set up the handlers for QNetworkSession signals
    connect(m_NetworkSession, SIGNAL(preferredConfigurationChanged(const QNetworkConfiguration&, bool)),
            this, SLOT(handlePreferredConfigurationChanged(const QNetworkConfiguration&, bool)));   
    connect(m_NetworkSession, SIGNAL(newConfigurationActivated()), this, SLOT(handleNewConfigurationActivated()));
    connect(m_NetworkSession, SIGNAL(stateChanged(QNetworkSession::State)),
            this, SLOT(handleStateChanged(QNetworkSession::State)));
    connect(m_NetworkSession, SIGNAL(opened()), this, SLOT(handleOpened()));
    connect(m_NetworkSession, SIGNAL(closed()), this, SLOT(handleClosed()));
    connect(m_NetworkSession, SIGNAL(error(QNetworkSession::SessionError)), 
            this, SLOT(handleError(QNetworkSession::SessionError)));
     
    m_NetworkSession->open();
}

/*!
    Frees the resources associated with the WebNetworkSession object.
*/
WebNetworkSession::~WebNetworkSession()
{
	  // Close the network connection session before delete.
	  m_NetworkSession->close();
    delete m_NetworkSession;
}

/*! 
    Handle the preferredConfigurationChanged signal from Network Session.
    
    It emits networkNameChnaged signal if it migrates to the new QNetworkConfiguration.
*/
void WebNetworkSession::handlePreferredConfigurationChanged(const QNetworkConfiguration &config, bool isSeamless)
{
    bool isSelected = true;
    QNetworkConfiguration activeConfig = activeConfiguration();
    
    if (isSeamless)
    {
        m_NetworkSession->migrate();
        qDebug() << "Migrate to new Network Connection: " << config.name();
        emit sessionConfigurationChanged(config); 
    }
    else
    {
    	  qDebug() << "active: " << activeConfig.bearerName() << "new: " <<  config.bearerName();
    	  // Is it upgrade ? Upgrade is defined as the cellular -> wireless LAN 
    	  if (((activeConfig.bearerName() == "2G") || (activeConfig.bearerName() == "CDMA2000")
    	  	  || (activeConfig.bearerName() == "WCDMA") || (activeConfig.bearerName() == "HSPA"))
    	  	  && (config.bearerName() == "WLAN"))
    	  {
    	  	  if (WebPageController::getSingleton()->secureState()) 
    	  	      isSelected = false;
    	  	      
    	  	  qDebug() << "secureState: " << WebPageController::getSingleton()->secureState();
    	  }
    	  	  
        if (isSelected)
        {
            m_NetworkSession->migrate();
            qDebug() << "Migrate to new Network Connection: " << config.name();
            emit sessionConfigurationChanged(config); 
        }
        else
        {
            m_NetworkSession->ignore();
            qDebug() << "Ignore new Network Connection: " << config.name();
            emit sessionRoamingRejected();
        }
    }
}

/*! 
    Handle the newConfigurationActivated from Network Session.
    
    It emits networknameChanged signal with current QNetworkConfiguration.
    
*/
void WebNetworkSession::handleNewConfigurationActivated()
{
    bool isConnected = true;
    
    // isConnectionGood = testConnection();
    if (isConnected)
    {
        m_NetworkSession->accept();
        qDebug() << "Accept new Network Connection";
        // flash the new connection network
    }
    else
    {
        m_NetworkSession->reject();
        // flash the old connection network
        qDebug() << "Reject new Network Connection";
    }

    emit sessionConfigurationChanged(activeConfiguration()); 
}

/*! 
    Handle the stateChanged signal from Network Session. If the session is based on a single
    access point configuration, the state of the session is the same state of the associated
    network interface. A QNetworkConfiguration::ServiceNetwork based session summarizes the 
    state of all its children and therefore returns the Connected state if at least one of its 
    sub configurations is connected.
    
    It emits networkSignalStrengthChanged signal with current QNetworkConfiguraiton 
    and QNetworkSession::State.
*/
void WebNetworkSession::handleStateChanged(QNetworkSession::State state)
{
    switch (state) {
        case QNetworkSession::Invalid:
            qDebug() << "Invalid";
            break;
        case QNetworkSession::NotAvailable:
            qDebug() << "Not Available";
            break;
        case QNetworkSession::Connecting:
            qDebug() << "Connecting";
            break;
        case QNetworkSession::Connected:
            qDebug() << "Connected";
            break;
        case QNetworkSession::Closing:
            qDebug() << "Closing";
            break;
        case QNetworkSession::Disconnected:
            qDebug() << "Disconnected";
            break;
        case QNetworkSession::Roaming:
            qDebug() << "Roaming";
            break;
        default:
            qDebug() << "Unknown";
    }
    emit sessionStateChanged(m_NetworkSession->configuration(), state); 
}

/*! 
    Handle the opened signal from Network Session.
    
    It emits networkNameChanged signal with activeConfiguration.
*/
void WebNetworkSession::handleOpened()
{   
    qDebug() << "Session Opened";

    emit sessionConfigurationChanged(activeConfiguration());
}

/*! 
    Handle the closed signal from Network Session.
*/
void WebNetworkSession::handleClosed()
{
    qDebug() << "Session Closed";
}

/*! 
    Handle the error signal from Network Session.
*/
void WebNetworkSession::handleError(QNetworkSession::SessionError error)
{
    switch (error)
    {
        case QNetworkSession::UnknownSessionError:
            qDebug() << "UnknownSessionError";
            break;
        case QNetworkSession::SessionAbortedError:
            qDebug() << "SessionAbortedError";
            break;
        case QNetworkSession::RoamingError:
            qDebug() << "RoamingError";
            break;
        case QNetworkSession::OperationNotSupportedError:
            qDebug() << "OperationNotSupportedError";
            break;
        case QNetworkSession::InvalidConfigurationError:
            qDebug() << "InvalidConfigurationError";
            break;
        default:
        	  qDebug() << "Unknown Error";
    }
}

QNetworkConfiguration WebNetworkSession::activeConfiguration(void)
{
	  QString activeIdentifier = m_NetworkSession->sessionProperty("ActiveConfiguration").toString();
	  QNetworkConfiguration config = m_NetworkSession->configuration();
	  QNetworkConfiguration activeConfig;
	  QList<QNetworkConfiguration> children;
	  
	  qDebug() << activeIdentifier;
	  
	  switch(config.type())
    {
        case QNetworkConfiguration::ServiceNetwork:
        	  qDebug() << "ServiceNetwork";
            children = config.children();
            /* Traverse all configuration to find the active configuration */
            foreach(QNetworkConfiguration tmpConfig, children)
            {
        	      qDebug() << tmpConfig.identifier();
                if (activeIdentifier == tmpConfig.identifier())
                {
            	      activeConfig = tmpConfig;
            	      break;
            	  }
            }
            break;  
        case QNetworkConfiguration::InternetAccessPoint:
        	  qDebug() << "InternetAccessPoint";
    	      activeConfig = config;
            break;
        case QNetworkConfiguration::UserChoice:
        	  qDebug() << "UserChoice";
        	  break;
        default:
        	  break;
    }
    
    return activeConfig;
}

} // namespace WRT