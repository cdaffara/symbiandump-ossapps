/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <networkhandlingproxy.h>
#include <cnwsession.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <settingsinternalcrkeys.h>
#include "infowidgetnetworkhandler.h"
#include "infowidgetlogging.h"

/*!
  \class InfoWidgetNetworkHandler
  \brief Handles network specific functionality of 
         Operator info widget
*/

// Local constants 
const int KMcnValueOff = 0;  
const int KMcnValueOn = 1; 

/*!
    InfoWidgetNetworkHandler::InfoWidgetNetworkHandler()
*/
InfoWidgetNetworkHandler::InfoWidgetNetworkHandler(QObject *parent) 
    :QObject(parent), 
    m_nwSession(NULL) 
{
    DPRINT;
    if (!createSession()) {
        DCRITICAL << ": session creation failed!"; 
    } 
}

/*!
    InfoWidgetNetworkHandler::~InfoWidgetNetworkHandler()
*/
InfoWidgetNetworkHandler::~InfoWidgetNetworkHandler()
{
    DPRINT;
    try {
        // Disable MCN setting, 
        // display client is being deleted
        disableMcn(); 
    } catch (const std::exception& ex) {
        DCRITICAL << ": exception detected: " << ex.what();  
    }      
     
}    

/*!
    InfoWidgetNetworkHandler::createSession()
*/
bool InfoWidgetNetworkHandler::createSession()
{
    DPRINT;
    bool success(false);
    if (!m_nwSession) {
        QT_TRAP_THROWING(m_nwSession.reset(
                CreateL(*this, m_nwInfo)));
        if (m_nwSession) {
            DPRINT << ": session created"; 
            success = true;
        }
    } else {
        DWARNING << ": session already created!"; 
    }
    
    return success; 
}

/*!
    InfoWidgetNetworkHandler::suspend()
*/
void InfoWidgetNetworkHandler::suspend()
{
    DPRINT;
}

/*!
    InfoWidgetNetworkHandler::resume()
*/
void InfoWidgetNetworkHandler::resume()
{
    DPRINT;
}

/*!
    InfoWidgetNetworkHandler::serviceProviderName()
*/
QString InfoWidgetNetworkHandler::serviceProviderName() const 
{
    QString qBuf;
    qBuf = QString((QChar*)m_nwInfo.iSPName.Ptr(), 
        m_nwInfo.iSPName.Length());
    DPRINT << ": serviceProviderName: " << qBuf;
    
    return qBuf; 
}

/*!
    InfoWidgetNetworkHandler::serviceProviderNameDisplayRequired()
*/
bool InfoWidgetNetworkHandler::serviceProviderNameDisplayRequired() const 
{
    bool displayRequired(true); 
    if (m_nwInfo.iServiceProviderNameDisplayReq == 
            RMobilePhone::KDisplaySPNNotRequired) {
        displayRequired = false; 
    } 
    return displayRequired; 
}

/*!
    InfoWidgetNetworkHandler::homeZoneTextTag()
*/
QString InfoWidgetNetworkHandler::homeZoneTextTag() const 
{
    QString qBuf;
    qBuf = QString((QChar*)m_nwInfo.iViagTextTag.Ptr(), 
        m_nwInfo.iViagTextTag.Length());
    return qBuf; 
}

/*!
    InfoWidgetNetworkHandler::homeZoneIndicatorType()
*/
int InfoWidgetNetworkHandler::homeZoneIndicatorType() const 
{
    int indicatorType = m_nwInfo.iViagIndicatorType;
    return indicatorType; 
}

/*!
    InfoWidgetNetworkHandler::enableMcn()
*/
void InfoWidgetNetworkHandler::enableMcn() 
{
    DPRINT;
    if (!readMcnDisplayState()) {
        DPRINT << ": enabling mcn display cenrep";
        writeMcnDisplayState(true); 
    }
}

/*!
    InfoWidgetNetworkHandler::disableMcn()
*/
void InfoWidgetNetworkHandler::disableMcn()
{
    DPRINT;
    if (readMcnDisplayState()) {
        DPRINT << ": disabling mcn display cenrep";
        writeMcnDisplayState(false); 
    }
}

/*!
    InfoWidgetNetworkHandler::mcnName()
*/
QString InfoWidgetNetworkHandler::mcnName() const 
{
    QString qBuf;
    qBuf = QString((QChar*)m_nwInfo.iMCNName.Ptr(),
        m_nwInfo.iMCNName.Length());
    DPRINT << ": mcnName: " << qBuf;
    return qBuf; 
}

/*!
    InfoWidgetNetworkHandler::mcnIndicatorType()
*/
int InfoWidgetNetworkHandler::mcnIndicatorType() const 
{
    DPRINT << ": mcnIndicatorType: " << m_nwInfo.iMCNIndicatorType;
    return m_nwInfo.iMCNIndicatorType;  
}

/*!
    InfoWidgetNetworkHandler::networkRegistrationStatus()
*/
int InfoWidgetNetworkHandler::networkRegistrationStatus() const 
{
    DPRINT << ": networkRegistrationStatus: " 
            << static_cast<int>(m_nwInfo.iRegistrationStatus);
    return static_cast<int>(m_nwInfo.iRegistrationStatus); 
}

/*!
    InfoWidgetNetworkHandler::isOnline()
    
    Check if network registration status is registered and 
    return status accordingly 
*/
bool InfoWidgetNetworkHandler::isOnline() const
{
    bool online(false); 

    if (!m_nwSession.isNull()) {
        switch (networkRegistrationStatus()) {
            case ENWRegisteredBusy: // Fall through
            case ENWRegisteredOnHomeNetwork: // Fall through
            case ENWRegisteredRoaming: 
                online = true;
                break; 
            default: 
                break; 
        }
    }
    DPRINT << ": online: " << online;
    return online; 
}

/*!
    InfoWidgetNetworkHandler::HandleNetworkMessage()
    
    Implementation for MNWMessageObserver callback
    function 
*/
void InfoWidgetNetworkHandler::HandleNetworkMessage( 
    const TNWMessages aMessage )
{   
    DPRINT << ": message: " << aMessage;
    
    bool acceptedMessage = false;
    switch (aMessage) {
        case ENWMessageCurrentCellInfoMessage: // Fall through 
        case ENWMessageServiceProviderNameChange: // Fall through
        case ENWMessageNetworkRegistrationStatusChange: // Fall through
            acceptedMessage = true;
            break;
        default:
            // not interesting message, filter out
            DPRINT << ": message filtered out";
            acceptedMessage = false;
    }
    
    if (acceptedMessage) {
        int result = 0;
        QT_TRYCATCH_ERROR(result, emit networkDataChanged());
        if (0 != result) {
            DPRINT << ": Exception occured while emitting signal:" << result;
        }
    }
}

/*!
    InfoWidgetNetworkHandler::HandleNetworkError()
    
    Implementation for MNWMessageObserver callback
    function 
*/
void InfoWidgetNetworkHandler::HandleNetworkError( 
    const TNWOperation aOperation, 
    TInt aErrorCode )
{
    DPRINT << ": operation: " << aOperation <<  
        " error code: " << aErrorCode;
    
    // Reset invalid data  
    switch (aOperation) {
        case MNWMessageObserver::ENWGetServiceProviderName:
            m_nwInfo.iServiceProviderNameDisplayReq = 
                    RMobilePhone::KDisplaySPNNotRequired;
            m_nwInfo.iSPName.Zero();
        break;
        default: 
            break; 
    }
    
    // Emit error signal 
    int result = 0;
    QT_TRYCATCH_ERROR(result, emit networkError( 
        static_cast<int>(aOperation), static_cast<int>(aErrorCode)));
    if (0 != result) {
        DPRINT << ": Exception occured while emitting signal:" << result;
    }
}

/*!
    Write Mcn cenrep key.   
*/
void InfoWidgetNetworkHandler::writeMcnDisplayState(bool enabled)
{
    QScopedPointer<XQSettingsManager> settingsManager(new XQSettingsManager); 
    XQSettingsKey settingsKey(XQSettingsKey::TargetCentralRepository, 
        KCRUidNetworkSettings.iUid, KSettingsMcnDisplay);
    int value; 
    if (enabled) {
        value = KMcnValueOn; 
    } else {
        value = KMcnValueOff; 
    }

    bool result = settingsManager->writeItemValue(settingsKey, value);
    DPRINT << ": write success: " << result << " mcn enabled: " << enabled;
     
}

/*!
    Read Mcn cenrep key.
*/
bool InfoWidgetNetworkHandler::readMcnDisplayState()
{
    QScopedPointer<XQSettingsManager> settingsManager(new XQSettingsManager); 
    XQSettingsKey settingsKey(XQSettingsKey::TargetCentralRepository, 
        KCRUidNetworkSettings.iUid, KSettingsMcnDisplay); 
    bool success(false); 
    int value = settingsManager->readItemValue(
            settingsKey,
            XQSettingsManager::TypeInt).toInt(&success);

    bool mcnEnabled(false); 
    if (success && value == KMcnValueOn) {
        mcnEnabled = true; 
    }
        
    DPRINT << ": read success: " << success << " mcn enabled: " << mcnEnabled;
    return mcnEnabled; 
}


// End of File. 
