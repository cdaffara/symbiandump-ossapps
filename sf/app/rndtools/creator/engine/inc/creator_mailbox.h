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





#ifndef __CREATORMAILBOXES_H__
#define __CREATORMAILBOXES_H__

#include "engine.h"
#include "creator_modulebase.h"


#include <e32base.h>
#include <SenduiMtmUids.h>
#include <smtpset.h>
#include <imapset.h>
#include <pop3set.h>
#include <mtmuibas.h>
#include <iapprefs.h>
#include <cemailaccounts.h>

class CCreatorEngine;
class CMailboxesParameters;

const TInt KDefaultSmtpPort = 25;

enum TMailboxType
    {
    EMailboxPOP3,
    EMailboxIMAP4
    };

class CCreatorMailboxes : public CCreatorModuleBase, public MMsvSessionObserver
    {
public: 
    static CCreatorMailboxes* NewL(CCreatorEngine* aEngine);
    static CCreatorMailboxes* NewLC(CCreatorEngine* aEngine);
    ~CCreatorMailboxes();

private:
    CCreatorMailboxes();
    void ConstructL(CCreatorEngine* aEngine); // from MCreatorModuleBase

    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3); // from MMsvSessionObserver
    void StoreIMEIToEntryL(const TMsvId aMailboxId);
    void GetIMEIFromThePhoneL();
    
    void PopulateSmtpSettingsL( 
            CEmailAccounts& aEmailAccount, 
            CImSmtpSettings& aSmtpSettings, 
            CImIAPPreferences& aIApSettings, 
            const CMailboxesParameters& aParameters );
    
    void PopulateImapSettingsL( 
            CEmailAccounts& aEmailAccount, 
            CImImap4Settings& aImapSettings, 
            CImIAPPreferences& aIApSettings, 
            const CMailboxesParameters& aParameters );
    
    void PopulatePopSettingsL( 
            CEmailAccounts& aEmailAccount, 
            CImPop3Settings& aPopSettings, 
            CImIAPPreferences& aIApSettings, 
            const CMailboxesParameters& aParameters );


public:
    TBool AskDataFromUserL( TInt aCommand ); // from MCreatorModuleBase
    
    TInt CreateMailboxEntryL(CMailboxesParameters *aParameters);
    void DeleteAllL();
    void DeleteAllCreatedByCreatorL();


private:
    CMailboxesParameters* iParameters;
    TBuf<50> iIMEI;
    RArray<TInt> iEntryIds;

public:
    };


class CMailboxesParameters : public CCreatorModuleBaseParameters
    {
public: 
//    HBufC*          iMailboxName;
//    HBufC*          iAccessPoint;
//    HBufC*          iMyMailAddress;
//    HBufC*          iOutgoingMailServer;
//    TBool           iSendMessageImmediately;
//    TBool           iSendCopyToSelf;
//    TBool           iIncludeSignature;
//    HBufC*          iUserName;
//    HBufC*          iPassword;
//    HBufC*          iIncomingMailServer;
//    TMailboxType    iMailboxType;  // IMAP4 or POP3
//    TBool           iSecurityOn; // SecureSockets
//    TBool           iSSLWrapper; // SSL Wrapper
//    TBool           iAPOPSecureLogin; // POP3 only
//    TBool           iRetrieveAttachment; // IMAP4 only
//    TInt32          iRetrieveHeaders;  // IMAP4 only

    enum TGetEmailOption
    {
    EUndef = 0,
    EGetHeaders,
    EGetBodyText,
    EGetBodyTextAndAttachments,
    EGetAttachments,
    EGetBodyAlternativeText,
    EGetEmailOptionLast
    };

    HBufC*  iMailboxName;    
    HBufC*  iIncomingLoginName;
    HBufC*  iIncomingPassword;
    HBufC*  iIncomingServerName;
    HBufC*  iIncomingConnectionMethod;
    HBufC*  iIncomingFolderPath;
    HBufC*  iPathSeparator;
    HBufC*  iOutgoingLoginName;
    HBufC*  iOutgoingPassword;
    HBufC*  iOutgoingServerName;
    HBufC*  iOutgoingConnectionMethod;
    HBufC*  iOwnEmailAddress;
    HBufC*  iOwnEmailAlias;
    HBufC*  iReceiptAddress;
    HBufC*  iReplyToAddress;
    
    TInt    iIncomingPort;
    TInt    iAttachmentSizeLimit;
    TInt    iBodyTextSizeLimit;
    TInt    iAttachmentFetchSize;
    TInt    iImapIdleTimeout;
    TInt    iMaxEmailSize;
    TInt    iSyncRate;
    TInt    iInboxSyncLimit; // -1 = All
    TInt    iMailboxSyncLimit; // -1 = All
    TInt    iOutgoingPort;
    TInt    iToCCIncludeLimit;
    
    TBool   iIncomingSSLWrapper;
    TBool   iIncomingSecureSockets;
    TBool   iAcknowledgeReceipts;
    TBool   iAutoSendOnConnect;
    TBool   iDeleteEmailsAtDisconnect;
    TBool   iImapIdleCommand;
    TBool   iMarkSeenInSync;
    TBool   iEnableExpungeMode;
    TBool   iUseApop;
    TBool   iDisconnectedUserMode;
    TBool   iOutgoingSSLWrapper;
    TBool   iOutgoingSecureSockets;
    TBool   iIncludeSignature;
    TBool   iAddVCard;
    TBool   iRequestReceipts;
    TBool   iSmtpAuth;
    
    TGetEmailOption iGetEmailOptions;   
    TFolderSubscribeType iSubscribeType;    
    TFolderSyncType iSyncType;  
    TImSMTPSendCopyToSelf iSendCopyToSelf;
    TImSMTPSendMessageOption iSendOption;    
    TMailboxType iMailboxType;  // IMAP4 or POP3
    
public:
    CMailboxesParameters();
    ~CMailboxesParameters();
    };



#endif // __CREATORMAILBOXES_H__
