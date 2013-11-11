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

#ifndef NMIPSSETTINGITEMS_H
#define NMIPSSETTINGITEMS_H

namespace IpsServices {
     
     /*!
         Enumeration for identifying mailbox settings.
     */
    enum SettingItem {
        IncomingLoginName = 0,                  // String
        IncomingPassword,                       // String
        MailboxName,                            // String
        EmailAddress,                           // String
        ReplyAddress,                           // String
        EmailAlias,                             // String
        MyName,                                 // String
        DownloadPictures,                       // Integer: 0=Off, 1=On
        MessageDivider,                         // Integer: 0=Off, 1=On
        ReceptionActiveProfile,                 // Integer: Keep-Up-To-Date=0, Save Energy=1, Manual Fetch=2, User Defined=3
        ReceptionUserDefinedProfile,            // Integer: 0=Disabled, 1=Enabled
        ReceptionInboxSyncWindow,               // Integer: 0=All messages
        ReceptionGenericSyncWindowInMessages,   // Integer: 0=All messages
        ReceptionWeekDays,                      // Integer bitmask: 0x01=Mon,0x02=Tue,0x04=Wed,0x08=Thu,0x10=Fri,0x20=Sat,0x40=Sun
        ReceptionDayStartTime,                  // Integer: 0-23
        ReceptionDayEndTime,                    // Integer: 0-23
        ReceptionRefreshPeriodDayTime,          // Integer: 5,15,60,240,0="When open mailbox"
        ReceptionRefreshPeriodOther,            // Integer: 5,15,60,240,0="When open mailbox"
        UserNameHidden,                         // Integer: 0=Off, 1=On
        IncomingMailServer,                     // String
        OutgoingMailServer,                     // String
        IncomingPort,                           // Integer
        OutgoingPort,                           // Integer
        FolderPath,                             // String: Empty string means 'Default'
        AlwaysOnlineState,                      // Integer: 0=always,1=homeonly,2=off
        EmailNotificationState,                 // Integer: 0=automatic,1=homeonly,2=off
        FirstEmnReceived,                       // Integer: 0=false,1=true
        EmnReceivedNotSynced,                   // Integer: 0=false,1=true
        AoLastSuccessfulUpdateL,                // Integer: low part of 64bit integer
        AoLastSuccessfulUpdateH,                // Integer: high part of 64bit integer
        AoLastUpdateFailed,                     // Integer: 0=false, 1=true
        AoUpdateSuccessfulWithCurSettings,      // Integer: 0=false, 1=true
        IncomingSecureSockets,                  // Boolean
        IncomingSSLWrapper,                     // Boolean
        OutgoingLoginName,                      // String
        OutgoingPassword,                       // String
        SMTPAuthentication,                     // Boolean
        OutgoingSecureSockets,                  // Boolean
        OutgoingSSLWrapper,                     // Boolean
        Connection                              // Unsigned integer, Network Destination ID (SNAP)
    }; 
    
    /**
     * Utility structure TAOInfo.
     * Contains info was last update successful ( TBool )
     * and the date and time of last successful update ( TTime )     
     */
    struct TAOInfo
        {
        TBool iLastUpdateFailed; 
        TTime iLastSuccessfulUpdate;
        TBool iUpdateSuccessfulWithCurSettings;
        };
    
    /**
     * Supported always online states.
     */
    enum TIpsSetDataAoStates
        {
        EMailAoAlways = 0,
        EMailAoHomeOnly,
        EMailAoOff
        };

    /**
     * Supported OMA EMN states
     */
    enum TIpsSetDataEmnStates
        {
        EMailEmnAutomatic = 0,
        EMailEmnHomeOnly,
        EMailEmnOff
        };
    
    /** 
     * Security types 
     */
    enum TIpsSetDataSecurityTypes
        {
        EMailStartTls = 0,
        EMailSslTls,
        EMailSecurityOff
        };

    /** 
     * Account types 
     */
    enum TIpsSetAccountTypes
        {
        EMailPop = 0,
        EMailImap
        };
    
    /** 
     *  SMTP Authentication types 
     */
    enum TIpsSetSMTPAuthenticationTypes
        {
        EMailAuthNone = 0,
        EMailAuthSameAsIncoming,
        EMailAuthUserAuthentication
        };
    
    const int standardPop3Port = 110;   // POP3 - port 110
    const int standardImap4Port = 143;  // IMAP - port 143
    const int standardSmtpPort = 25;    // SMTP - port 25
    const int secureSmtpPort = 465;     // Secure SMTP (SSMTP) - port 465
    const int secureImap4Port = 585;    // Secure IMAP (IMAP4-SSL) - port 585
    const int imap4OverSslPort = 993;   // IMAP4 over SSL (IMAPS) - port 993
    const int securePop3Port = 995;     // Secure POP3 (SSL-POP) - port 995
    
    const int NmIpsSettingsDefault = 0;         //Radio button list default index
    const int NmIpsSettingsUserDefined = 1;     //Radio button list user defined index

} // namespace

#endif // NMIPSSETTINGITEMS_H
