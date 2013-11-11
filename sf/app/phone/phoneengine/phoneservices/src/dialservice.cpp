/*!
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
* Description:  dial service
*
*/

#include <xqserviceutil.h>
#include <e32base.h>
#include <qregexp.h>
#include <featmgr.h>
#include <settingsinternalcrkeys.h>
#include <xqsettingsmanager.h>
#include <e32capability.h>
#include "dialservice.h"
#include "qtphonelog.h"

// CONSTANTS
const XQSettingsKey dialPrefixMode(XQSettingsKey::TargetCentralRepository, KCRUidTelephonySettings.iUid, KSettingsDialPrefixChangeMode);
const XQSettingsKey dialPrefixString(XQSettingsKey::TargetCentralRepository, KCRUidTelephonySettings.iUid, KSettingsDialPrefixText);

DialService::DialService(MPECallControlIF &call, MPECallSettersIF &parameters, QObject* parent) :
    XQServiceProvider(QLatin1String("phoneui.com.nokia.symbian.ICallDial"), parent), m_call (call), m_parameters (parameters), m_settingsManager(0)
{
    publishAll();
    m_settingsManager = new XQSettingsManager(this);
    Q_ASSERT(m_settingsManager != 0);
}

DialService::~DialService()
{
}

int DialService::dial(const QString& number)
{
    PHONE_DEBUG2("DialService::dial number:", number);
    if (!hasCapability()) {
        return KErrPermissionDenied;
    }
    QString phoneNumber = modifyPhoneNumber(number);
    TPtrC16 numberPtr(reinterpret_cast<const TUint16*>(phoneNumber.utf16()));
    m_parameters.SetPhoneNumber (numberPtr);
    m_parameters.SetCallTypeCommand (EPECallTypeCSVoice);
    return m_call.HandleDialServiceCall ();
}

int DialService::dial(const QString& number, int contactId)
{
    PHONE_DEBUG4("DialService::dial number:", number, "contactId:", contactId);
	if (!hasCapability()) {
		return KErrPermissionDenied;
    }
    QString phoneNumber = modifyPhoneNumber(number);
    TPtrC16 numberPtr (reinterpret_cast<const TUint16*>(phoneNumber.utf16()));
    m_parameters.SetPhoneNumber (numberPtr);
    m_parameters.SetCallTypeCommand (EPECallTypeCSVoice);
    m_parameters.SetContactId2 (contactId);
    return m_call.HandleDialServiceCall ();
}

int DialService::dialVideo(const QString& number)
{
    PHONE_DEBUG2("DialService::dialVideo number:", number);
	if (!hasCapability()) {
		return KErrPermissionDenied;
    }
    QString phoneNumber = modifyPhoneNumber(number);
    TPtrC16 numberPtr(reinterpret_cast<const TUint16*>(phoneNumber.utf16()));
    m_parameters.SetPhoneNumber (numberPtr);
    m_parameters.SetCallTypeCommand (EPECallTypeVideo);
    return m_call.HandleDialServiceCall ();
}

int DialService::dialVideo(const QString& number, int contactId)
{
    PHONE_DEBUG4("DialService::dialVideo number:", number, "contactId:", contactId);
    if (!hasCapability()) {
		return KErrPermissionDenied;
    }
    QString phoneNumber = modifyPhoneNumber(number);
    TPtrC16 numberPtr (reinterpret_cast<const TUint16*>(phoneNumber.utf16()));
    m_parameters.SetPhoneNumber (numberPtr);
    m_parameters.SetCallTypeCommand (EPECallTypeVideo);
    m_parameters.SetContactId2 (contactId);
    return m_call.HandleDialServiceCall ();
}

int DialService::dialVoip(const QString& address)
{
    PHONE_DEBUG2("DialService::dialVoip number:", address);
    if (!hasCapability()) {
		return KErrPermissionDenied;
    }
    TPtrC16 numberPtr(reinterpret_cast<const TUint16*>(address.utf16 ()));
    m_parameters.SetPhoneNumber(numberPtr);
    m_parameters.SetCallTypeCommand(EPECallTypeVoIP);
    return m_call.HandleDialServiceCall();
}

int DialService::dialVoip(const QString& address, int contactId)
{
    PHONE_DEBUG4("DialService::dialVoip number:", address, "contactId:", contactId);
    if (!hasCapability()) {
		return KErrPermissionDenied;
    }
    TPtrC16 numberPtr (reinterpret_cast<const TUint16*>(address.utf16 ()));
    m_parameters.SetPhoneNumber(numberPtr);
    m_parameters.SetCallTypeCommand(EPECallTypeVoIP);
    m_parameters.SetContactId2(contactId);
    return m_call.HandleDialServiceCall();
}

int DialService::dialVoipService(const QString& address, int serviceId)
{
    PHONE_DEBUG4("DialService::dialVoipService number:", address, "serviceId:", serviceId);
    if (!hasCapability()) {
		return KErrPermissionDenied;
    }
    TPtrC16 numberPtr (reinterpret_cast<const TUint16*>(address.utf16 ()));
    m_parameters.SetPhoneNumber(numberPtr);
    m_parameters.SetCallTypeCommand(EPECallTypeVoIP);
    m_parameters.SetServiceIdCommand(serviceId);
    return m_call.HandleDialServiceCall();
}

int DialService::dialVoipService(
        const QString& address, int serviceId, int contactId)
{
    PHONE_DEBUG2("DialService::dialVoipService number:", address);
    PHONE_DEBUG2("DialService::dialVoipService serviceId:", serviceId);
    PHONE_DEBUG2("DialService::dialVoipService contactId:", contactId);
    if (!hasCapability()) {
		return KErrPermissionDenied;
    }
    TPtrC16 numberPtr (reinterpret_cast<const TUint16*>(address.utf16 ()));
    m_parameters.SetPhoneNumber(numberPtr);
    m_parameters.SetCallTypeCommand(EPECallTypeVoIP);
    m_parameters.SetServiceIdCommand(serviceId);
    m_parameters.SetContactId2(contactId);
    return m_call.HandleDialServiceCall();
}

QString DialService::modifyPhoneNumber(const QString &number)
{
    PHONE_DEBUG2("DialService::modifyPhoneNumber number:", number);
    QString modifiedNumber = simplified(number);
    modifiedNumber = japanPrefixModifications(modifiedNumber);

    return modifiedNumber;
}

QString DialService::simplified(const QString &number)
{
    QString simplifiedNumber = number;
    QRegExp rx(QString("[\\s,.\\[\\]\\(\\)\\-]"));
    simplifiedNumber.remove(rx);
    return simplifiedNumber;
}

QString DialService::japanPrefixModifications(const QString &number)
{
    PHONE_DEBUG2("DialService::japanPrefixModifications number:", number);
    bool checkPrefix = FeatureManager::FeatureSupported(KFeatureIdJapanPrefixChange);
    if(checkPrefix == false) {
    	  // prefix change setting is off so don't play with the number
        return number;
    }

    QString modifiedNumber = number;

    int prefixMode = m_settingsManager->readItemValue(dialPrefixMode, XQSettingsManager::TypeInt).toInt();
    PHONE_DEBUG2("DialService::japanPrefixModifications prefixMode:", prefixMode);

    if((m_settingsManager->error() == XQSettingsManager::NoError) && (prefixMode > 0)) {
        // No error fetching value and setting is on
        QString prefixString = m_settingsManager->readItemValue(dialPrefixString, XQSettingsManager::TypeString).toString();
        PHONE_DEBUG2("DialService::japanPrefixModifications prefixString:", prefixString);

        if(m_settingsManager->error() == XQSettingsManager::NoError) {
            if(modifiedNumber.startsWith("+")) {
                modifiedNumber = modifiedNumber.remove(0, 1);
                modifiedNumber = modifiedNumber.insert(0, prefixString);
            }
        }
    }

    PHONE_DEBUG2("DialService::japanPrefixModifications modifiedNumber:", modifiedNumber);
    return modifiedNumber;
}

bool DialService::hasCapability()
{
    XQRequestInfo req = requestInfo();
    QSet<int> caps = req.clientCapabilities();

    if (!caps.contains(ECapabilityNetworkServices)) {
        PHONE_TRACE1("NetworkServices capability missing");
        return false;
    }
    if (!caps.contains(ECapabilityNetworkControl)) {
        PHONE_TRACE1("NetworkControl capability missing");
        return false;
    }
	return true;
}
