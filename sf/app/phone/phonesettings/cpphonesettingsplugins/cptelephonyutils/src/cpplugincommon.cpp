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


#include <etelmm.h>
#include <xqsysinfo.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <e32base.h>
#include <etelmm.h>
#include <exterror.h>               
#include <gsmerror.h>
#include <settingsinternalcrkeys.h>
#include <LogsDomainCRKeys.h>
#include <BTSapDomainPSKeys.h>
#include <NumberGroupingCRKeys.h>
#include <hbglobal.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <telephonyvariant.hrh>
#include <telinternalcrkeys.h>
#include <ctsydomainpskeys.h>
#include "cptelephonyutilsdefs.h"
#include "cpplugincommon.h"
#include "cppluginlogging.h"

const int CenrepKeyValueOff = 0;
const int CenrepKeyValueOn = 1;

const int SoftRejectTextDefault = 0; 
const int SoftRejectTextUserDefined = 1; 

/*!
  Tools::voipSupported
*/
bool Tools::voipSupported()
{
    return XQSysInfo::isSupported(KFeatureIdCommonVoip);
}

/*!
  Tools::videoSupported
*/
bool Tools::videoSupported()
{
    return XQSysInfo::isSupported(KFeatureIdCsVideoTelephony);
}

SettingsWrapper::SettingsWrapper(QObject *parent): 
    QObject(parent),
    m_settings(NULL),
    m_deviceInfo(NULL)
{
    m_settings = new XQSettingsManager(this);
    m_deviceInfo = new QSystemDeviceInfo(this);
}

SettingsWrapper::~SettingsWrapper()
{
    
}

bool SettingsWrapper::showCallDuration()
{
    bool showDuration; 
    if (CenrepKeyValueOn == readCenrepValue(KCRUidLogs.iUid, KLogsShowCallDuration).toInt()) {
        showDuration = true; 
    } else {
        showDuration = false;
    }
    
    DPRINT << "show call duration:" << showDuration;
    return showDuration;
}

int SettingsWrapper::setShowCallDuration(bool value)
{
    int cenrepValue; 
    DPRINT << "show call duration:" << value;

    if (value) {
        cenrepValue = CenrepKeyValueOn;
    } else {
        cenrepValue = CenrepKeyValueOff; 
    }
    return writeCenrepValue(KCRUidLogs.iUid, KLogsShowCallDuration, cenrepValue );
}

int SettingsWrapper::readVtVideoSending()
{
    DPRINT << ": IN";
    return readCenrepValue(KCRUidTelephonySettings.iUid, KSettingsVTVideoSending).toInt();
}

int SettingsWrapper::writeVtVideoSending(int value)
{
    DPRINT << ": IN";
    return writeCenrepValue(KCRUidTelephonySettings.iUid, KSettingsVTVideoSending, value);
}

void SettingsWrapper::readSoftRejectText(QString &text, bool &userDefined )
{
    if (SoftRejectTextDefault ==
        readCenrepValue(KCRUidTelephonySettings.iUid, KSettingsSoftRejectDefaultInUse ).toInt()) {
        userDefined = false; 
    } else {
        userDefined = true; 
    }
        
    text = readCenrepString(KCRUidTelephonySettings.iUid, KSettingsSoftRejectText);
    DPRINT << "text:" << text << " ,userDefined:" << userDefined;
}

int SettingsWrapper::writeSoftRejectText(const QString &text, bool userDefined )
{
    int err = writeCenrepValue(KCRUidTelephonySettings.iUid, KSettingsSoftRejectText, text);
    int cenrepValue; 
    if (userDefined) {
        cenrepValue = SoftRejectTextUserDefined; 
    } else {
        cenrepValue = SoftRejectTextDefault; 
    }
    err |= writeCenrepValue(KCRUidTelephonySettings.iUid, KSettingsSoftRejectDefaultInUse, cenrepValue);
    DPRINT << "text:" << text << " ,userDefined:" << userDefined << " , err:" << err;
    return err;
}

 bool SettingsWrapper::numberGroupingSupported() const
 {
     return readCenrepValue(KCRUidNumberGrouping.iUid, KNumberGrouping).toBool();
 }
 
 bool SettingsWrapper::forbiddenIconSupported() const
 {
     int keyValue = readCenrepValue(KCRUidTelVariation.iUid, KTelVariationFlags).toInt();
     return (KTelephonyLVFlagForbiddenIcon & keyValue);
 }

QVariant SettingsWrapper::readCenrepValue(
    const long int uid, const unsigned long int key) const
{
    XQSettingsKey settingsKey(XQSettingsKey::TargetCentralRepository, uid, key);
    QVariant ret = m_settings->readItemValue(settingsKey);
    DPRINT << "ret: " << ret;
    return ret;
}

QVariant SettingsWrapper::readPubSubValue(
    const long int uid, const unsigned long int key) const
{
    XQSettingsKey settingsKey(XQSettingsKey::TargetPublishAndSubscribe, uid, key);
    QVariant ret = m_settings->readItemValue(settingsKey);
    DPRINT << "PubSub ret: " << ret;
    return ret;
}

QString SettingsWrapper::readCenrepString(
    const long int uid, const unsigned long int key) const
{
    XQSettingsKey settingsKey(XQSettingsKey::TargetCentralRepository, uid, key);
    QString text = m_settings->readItemValue(settingsKey, XQSettingsManager::TypeString).toString();
    DPRINT << "text: " << text;
    return text;
}

int SettingsWrapper::writeCenrepValue(
    const long int uid, const unsigned long int key, const QVariant &settingsKeyValue ) const
{
    DPRINT << "uid:" << uid << ", key:" << key << ", settingsKeyValue:" << settingsKeyValue;
    XQSettingsKey settingsKey(XQSettingsKey::TargetCentralRepository, uid, key);
    int err = m_settings->writeItemValue(settingsKey, settingsKeyValue );
    DPRINT << "err: " << err;
    return err;
}

bool SettingsWrapper::isFeatureCallWaitingDistiquishNotProvisionedEnabled()
{
    bool enabled = readCenrepValue(KCRUidPhoneSettings.iUid, KPSetCallWaiting).toBool();
    DPRINT << "enabled: " << enabled;
    return enabled;
}

bool SettingsWrapper::isPhoneOffline() const
{
    bool networkConnectionAllowed = readCenrepValue(KCRUidCoreApplicationUIs.iUid,
                                KCoreAppUIsNetworkConnectionAllowed).toBool();
    // 0 = Offline Mode, 1 = Online Mode
    DPRINT << "networkConnectionAllowed: " << networkConnectionAllowed;
    return !networkConnectionAllowed;
}

bool SettingsWrapper::isOngoingCall() const
{
    bool callOngoing(false);
    if (EPSCTsyCallStateNone < 
            readPubSubValue(KPSUidCtsyCallInformation.iUid, KCTsyCallState).toInt()) {
        callOngoing = true; 
    }
    return callOngoing;
}
