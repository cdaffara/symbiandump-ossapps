/*!
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
* Description:  Network handling.
*
*/

#include <exception>
#include <QString>
#include "penetworkhandler.h"
#include "mpedatastore.h"
#include "qtphonelog.h"

PeNetworkHandler::PeNetworkHandler(MPEPhoneModelInternal& aModel) :
    iModel(aModel)
{
    m_networkInfo = new QSystemNetworkInfo();
            
    QSystemNetworkInfo::NetworkStatus nwStatus = 
            m_networkInfo->networkStatus(QSystemNetworkInfo::GsmMode);
    
    if (nwStatus == QSystemNetworkInfo::NoNetworkAvailable) {
        nwStatus = m_networkInfo->networkStatus(QSystemNetworkInfo::WcdmaMode);
    }
    
    if (nwStatus == QSystemNetworkInfo::NoNetworkAvailable) {
        nwStatus = m_networkInfo->networkStatus(QSystemNetworkInfo::CdmaMode);
    }
    
    QObject::connect(m_networkInfo, SIGNAL(
            networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus)), 
            this, SLOT(networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus)));

    iModel.DataStore()->SetNetworkRegistrationStatus( 
        static_cast< TNWNetworkRegistrationStatus > ( convertStatus(nwStatus) ) );
}

PeNetworkHandler::~PeNetworkHandler()
{   
    delete m_networkInfo;
}

void PeNetworkHandler::networkStatusChanged(
        QSystemNetworkInfo::NetworkMode mode, QSystemNetworkInfo::NetworkStatus status)
{ 
    if (mode == QSystemNetworkInfo::GsmMode ||
        mode == QSystemNetworkInfo::WcdmaMode ||
        mode == QSystemNetworkInfo::CdmaMode) {
        sendStatus(status);
    }
}

void PeNetworkHandler::sendStatus(
        QSystemNetworkInfo::NetworkStatus status)
{
    iModel.DataStore()->SetNetworkRegistrationStatus( 
        static_cast< TNWNetworkRegistrationStatus > ( convertStatus(status) ) );
    iModel.SendMessage(MEngineMonitor::EPEMessageNetworkRegistrationStatusChange);
}

TNWNetworkRegistrationStatus PeNetworkHandler::convertStatus(
        QSystemNetworkInfo::NetworkStatus status)
{
    TNWNetworkRegistrationStatus ret;
    switch(status) {
    case QSystemNetworkInfo::UndefinedStatus:
        ret = ENWStatusRegistrationUnknown;
    break;
    case QSystemNetworkInfo::NoNetworkAvailable:
        ret = ENWStatusNotRegisteredNoService;
    break;
    case QSystemNetworkInfo::EmergencyOnly:
        ret = ENWStatusNotRegisteredEmergencyOnly;
    break;
    case QSystemNetworkInfo::Searching:
        ret = ENWStatusNotRegisteredSearching;
    break;
    case QSystemNetworkInfo::Busy:
        ret = ENWStatusRegisteredBusy;
    break;
    case QSystemNetworkInfo::Connected:
    case QSystemNetworkInfo::HomeNetwork:
        ret = ENWStatusRegisteredOnHomeNetwork;
    break;
    case QSystemNetworkInfo::Denied:
        ret = ENWStatusRegistrationDenied;
    break;
    case QSystemNetworkInfo::Roaming:
        ret = ENWStatusRegisteredRoaming;
    break;
    default:
        ret = ENWStatusRegistrationUnknown;
    break;
    }
    
    return ret;
}

