/*
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
 * Description:
 *
 */

#ifndef NMAPIMAILBOXSETTINGSDATA_H_
#define NMAPIMAILBOXSETTINGSDATA_H_

#include <QVariant>
#include <nmapidef.h>

namespace EmailClientApi
{

const QString NmApiMailboxTypeImap = "imap";
const QString NmApiMailboxTypePop = "pop";

const QString NmApiAuthNone = "none";
const QString NmApiAuthSameAsIncoming = "SameAsIncoming";
const QString NmApiAuthUserAuthentication = "UserAuthentication";

const QString NmApiAutomatic = "automatic";
const QString NmApiAlways = "always";
const QString NmApiHomeOnly = "homeOnly";
const QString NmApiOff = "off";

const QString NmApiStartTls = "StartTls";
const QString NmApiSSLTls = "SSLTls";
const QString NmApiSecurityOff = NmApiOff;

const QString NmApiKeepUpToDate = "Keep-Up-To-Date";
const QString NmApiSaveEnergy = "Save-Energy";
const QString NmApiFetchManually = "Manual-Fetch";
const QString NmApiUserDefined = "User-Defined";

const QString EmailProfileOffsetKUTD = "KUTD";
const QString EmailProfileOffsetSE = "SE";
const QString EmailProfileOffsetMF = "MF";
const QString EmailProfileOffsetUD = "UD";

enum NmApiWeekDays
{
    Mon = 0x01, Tue = 0x02, Wed = 0x04, Thu = 0x08, Fri = 0x10,
    Sat = 0x20, Sun = 0x40
};

enum NmApiRefreshPeriods
{
    WhenMailboxOpens = 0, Every5minutes = 5, Every15minutes = 15,
    EveryHour = 60, Every4Hours = 240
};

enum NmApiMailboxSettingKey
{
    IncomingLoginName = 0, // String
    IncomingPassword, // String
    MailboxName, // String
    EmailAddress, // String
    ReplyAddress, // String
    EmailAlias, // String
    MyName, // String    
    DownloadPictures, // Integer: 0=Off, 1=On
    MessageDivider, // Integer: 0=Off, 1=On
    ReceptionActiveProfile, // String: Reception profile
    ReceptionUserDefinedProfile, // Integer: 0=Disabled, 1=Enabled
    ReceptionInboxSyncWindow, // Integer: 0=All messages
    ReceptionGenericSyncWindowInMessages, // Integer: 0=All messages
    ReceptionWeekDays, // Integer bitmask of weekdays
    ReceptionDayStartTime, // Integer: 0-23
    ReceptionDayEndTime, // Integer: 0-23
    ReceptionRefreshPeriodDayTime, // Integer: 5,15,60,240,0="When open mailbox"
    ReceptionRefreshPeriodOther, // Integer: 5,15,60,240,0="When open mailbox"
    UserNameHidden, // Integer: 0=Off, 1=On
    IncomingMailServer, // String
    IncomingMailUsesAuthentication, // String    "none", "UserAuthentication"
    IncomingMailSecurityType, // String  "StartTls", "SSLTls", "none"
    OutgoingMailServer, // String
    OutgoingMailUsesAuthentication, // String "none", "SameAsIncoming", "UserAuthentication"
    OutgoingMailSecurityType, // String 	"StartTls", "SSLTls", "none"
    IncomingPort, // Integer
    OutgoingPort, // Integer
    FolderPath, // String: Empty string means 'Default'
    AlwaysOnlineState, // string "always", "homeOnly", "off"
    EmailNotificationState, // string "automatic", "homeOnly", "off"
    FirstEmnReceived, // Integer: 0=false,1=true
    EmnReceivedNotSynced, // Integer: 0=false,1=true
    AoLastSuccessfulUpdate, // QDateTime
    AoLastUpdateFailed, // Integer: 0=false, 1=true
    AoUpdateSuccessfulWithCurSettings, // Integer: 0=false, 1=true
    IncomingSecureSockets, // Boolean
    IncomingSSLWrapper, // Boolean
    OutgoingLoginName, // String
    OutgoingPassword, // String
    UseOutgoingAuthentication, // Boolean
    OutgoingSecureSockets, // Boolean
    OutgoingSSLWrapper // Boolean
};

class NmApiMailboxSettingsDataPrivate;
class NMAPI_EXPORT NmApiMailboxSettingsData
{

public:    
    NmApiMailboxSettingsData();
    ~NmApiMailboxSettingsData();

    void setMailboxId(quint64 mailboxId);
    quint64 mailboxId() const;
    void setValue(int key, const QVariant &settingValue);
    bool getValue(int key, QVariant &settingValue) const;
    bool validateData() const;
    void clearSettings();
    QList<int> listSettings() const;

protected:    
    NmApiMailboxSettingsDataPrivate *d;

};

}//end namespace

#endif /* NMAPIMAILBOXSETTINGSDATA_H_ */
