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


#include <etelmm.h>
#include <msvapi.h>

#include "engine.h"
#include "enginewrapper.h"
#include "creator_mailbox.h" 
#include "creator_traces.h"

static const TInt KMailboxesFieldLength = 160;

_LIT(KCreatorMailboxesPrefixName, "CR_");

_LIT(KDefaultIncomingServer, "mail.example.com");
_LIT(KDefaultOutgoingServer, "smtp.example.com");
_LIT(KInternet, "internet");


//----------------------------------------------------------------------------

CMailboxesParameters::CMailboxesParameters()
    {
    LOGSTRING("Creator: CMailboxesParameters::CMailboxesParameters");

    iIncomingPort = KUndef;
    iOutgoingPort = KUndef;
    iMaxEmailSize = KUndef;
    iSyncRate = KUndef;
    iAttachmentFetchSize = KUndef;
    iAttachmentSizeLimit = KUndef;
    iBodyTextSizeLimit = KUndef;
    iAttachmentFetchSize = KUndef;
    iInboxSyncLimit = -1; // -1 = All
    iMailboxSyncLimit = -1; // -1 = All    
    iToCCIncludeLimit = KUndef;
    iImapIdleTimeout = KUndef;
    
    /*iMailboxName = HBufC::New(KMailboxesFieldLength);
    iAccessPoint = HBufC::New(KMailboxesFieldLength);
    iMyMailAddress = HBufC::New(KMailboxesFieldLength);
    iOutgoingMailServer = HBufC::New(KMailboxesFieldLength);
    iUserName = HBufC::New(KMailboxesFieldLength);
    iPassword = HBufC::New(KMailboxesFieldLength);
    iIncomingMailServer = HBufC::New(KMailboxesFieldLength);*/
    }

CMailboxesParameters::~CMailboxesParameters()
    {
    LOGSTRING("Creator: CMailboxesParameters::~CMailboxesParameters");

    /*delete iIncomingMailServer;
    delete iPassword;
    delete iUserName;
    delete iOutgoingMailServer;
    delete iMyMailAddress;
    delete iAccessPoint;
    delete iMailboxName;*/
    
    delete iMailboxName;
    delete iIncomingLoginName;
    delete iIncomingPassword;
    delete iIncomingServerName;
    delete iIncomingConnectionMethod;
    delete iIncomingFolderPath;
    delete iPathSeparator;
    delete iOutgoingLoginName;
    delete iOutgoingPassword;
    delete iOutgoingServerName;
    delete iOutgoingConnectionMethod;
    delete iOwnEmailAddress;
    delete iOwnEmailAlias;
    delete iReceiptAddress;
    delete iReplyToAddress;
    }

//----------------------------------------------------------------------------

CCreatorMailboxes* CCreatorMailboxes::NewL(CCreatorEngine* aEngine)
    {
    CCreatorMailboxes* self = CCreatorMailboxes::NewLC(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

CCreatorMailboxes* CCreatorMailboxes::NewLC(CCreatorEngine* aEngine)
    {
    CCreatorMailboxes* self = new (ELeave) CCreatorMailboxes;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }

CCreatorMailboxes::CCreatorMailboxes()
    {    
    }

void CCreatorMailboxes::ConstructL(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorMailboxes::ConstructL");

    iEngine = aEngine;
    }

CCreatorMailboxes::~CCreatorMailboxes()
    {
    LOGSTRING("Creator: CCreatorMailboxes::~CCreatorMailboxes");
    if ( iEngine && iEntryIds.Count() )
        {
        TRAP_IGNORE( iEngine->WriteEntryIdsToStoreL( iEntryIds, KUidDictionaryUidMailbox ) );
        }
    iEntryIds.Reset();
    iEntryIds.Close();
    
    if (iParameters)
        delete iParameters;
    }

//----------------------------------------------------------------------------

TBool CCreatorMailboxes::AskDataFromUserL(TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorMailboxes::AskDataFromUserL");
    
    CCreatorModuleBase::AskDataFromUserL( aCommand );//ignore retval
    
    if ( aCommand == ECmdDeleteMailboxes )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all mailboxes?"), this, ECreatorModuleDelete );
        }
    else if ( aCommand ==  ECmdDeleteCreatorMailboxes )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all mailboxes created with Creator?"), this, ECreatorModuleDelete );
        }
    return iEngine->GetEngineWrapper()->EntriesQueryDialog( &iEntriesToBeCreated, _L("How many entries to create?"), EFalse, this, ECreatorModuleStart );
    }


//----------------------------------------------------------------------------

TInt CCreatorMailboxes::CreateMailboxEntryL(CMailboxesParameters *aParameters)
    {
    LOGSTRING("Creator: CCreatorMailboxes::CreateMailboxEntryL");

      // clear any existing parameter definations
    delete iParameters;
    iParameters = NULL;
    
    CMailboxesParameters* parameters = aParameters;
    
    // random data needed if no predefined data available
    if (!parameters)
        {
        iParameters = new(ELeave) CMailboxesParameters;
        parameters = iParameters;

        TPtrC company(iEngine->RandomString(CCreatorEngine::ECompany));
        TPtrC firstName(iEngine->RandomString(CCreatorEngine::EFirstName));
        
        delete parameters->iMailboxName;
        parameters->iMailboxName = 0;
        parameters->iMailboxName = HBufC::NewL(KMailboxesFieldLength);
        parameters->iMailboxName->Des() = KCreatorMailboxesPrefixName;
        parameters->iMailboxName->Des().Append( company );
        parameters->iMailboxName->Des().Append(_L(" #"));
        parameters->iMailboxName->Des().AppendNum(iEngine->RandomNumber(1, 100));
        
        delete parameters->iIncomingLoginName;
        parameters->iIncomingLoginName = 0;
        parameters->iIncomingLoginName = HBufC::NewL(KMailboxesFieldLength); 
        delete parameters->iOutgoingLoginName;
        parameters->iOutgoingLoginName = 0;
        parameters->iOutgoingLoginName = HBufC::NewL(KMailboxesFieldLength); 
        parameters->iIncomingLoginName->Des() = iEngine->RandomString(CCreatorEngine::EFirstName);
        parameters->iIncomingLoginName->Des().LowerCase();
        parameters->iOutgoingLoginName->Des().Copy(parameters->iIncomingLoginName->Des());
        
        delete parameters->iIncomingPassword;
        parameters->iIncomingPassword = 0;
        parameters->iIncomingPassword = HBufC::NewL(KMailboxesFieldLength); 
        delete parameters->iOutgoingPassword;
        parameters->iOutgoingPassword = 0;
        parameters->iOutgoingPassword = HBufC::NewL(KMailboxesFieldLength); 
        parameters->iIncomingPassword->Des() = iEngine->RandomString(CCreatorEngine::ESurname);
        parameters->iIncomingPassword->Des().LowerCase();
        parameters->iOutgoingPassword->Des().Copy(parameters->iIncomingPassword->Des());
        
        delete parameters->iIncomingServerName;
        parameters->iIncomingServerName = 0;
        parameters->iIncomingServerName = HBufC::NewL(KMailboxesFieldLength); 
        parameters->iIncomingServerName->Des() = _L("incoming.");
        parameters->iIncomingServerName->Des().Append(company);
        parameters->iIncomingServerName->Des().Append(_L(".com"));
        parameters->iIncomingServerName->Des().LowerCase();
                
        delete parameters->iIncomingConnectionMethod;
        parameters->iIncomingConnectionMethod = 0;
        parameters->iIncomingConnectionMethod = HBufC::NewL(KInternet().Length());
        parameters->iIncomingConnectionMethod->Des() = KInternet;  // use always this one, if it's not found, default will be used instead
        
        delete parameters->iOutgoingServerName;
        parameters->iOutgoingServerName = 0;
        parameters->iOutgoingServerName = HBufC::NewL(KMailboxesFieldLength); 
        parameters->iOutgoingServerName->Des() = _L("smtp.");
        parameters->iOutgoingServerName->Des().Append(company);
        parameters->iOutgoingServerName->Des().Append(_L(".com"));
        parameters->iOutgoingServerName->Des().LowerCase();
        
        delete parameters->iOutgoingConnectionMethod;
        parameters->iOutgoingConnectionMethod = 0;
        parameters->iOutgoingConnectionMethod = HBufC::NewL(KInternet().Length());
        parameters->iOutgoingConnectionMethod->Des() = KInternet;  // use always this one, if it's not found, default will be used instead

        delete parameters->iOwnEmailAddress;
        parameters->iOwnEmailAddress = 0;
        parameters->iOwnEmailAddress = iEngine->CreateEmailAddressLC();
        parameters->iOwnEmailAddress->Des().LowerCase();
        CleanupStack::Pop(); // parameters->iOwnEmailAddress
        
        delete parameters->iOwnEmailAlias;
        parameters->iOwnEmailAlias = 0;
        parameters->iOwnEmailAlias = HBufC::NewL(firstName.Length());        
        parameters->iOwnEmailAlias->Des().Copy(firstName);       

        delete parameters->iReceiptAddress;
        parameters->iReceiptAddress = 0;
        parameters->iReceiptAddress = iEngine->CreateEmailAddressLC();
        parameters->iReceiptAddress->Des().LowerCase();
        CleanupStack::Pop(); // parameters->iReceiptAddress
        
        delete parameters->iReplyToAddress;
        parameters->iReplyToAddress = 0;
        parameters->iReplyToAddress = iEngine->CreateEmailAddressLC();
        parameters->iReplyToAddress->Des().LowerCase();
        CleanupStack::Pop(); // parameters->iReplyToAddress
        
        parameters->iIncomingSSLWrapper = iEngine->RandomNumber(0, 1);
        parameters->iIncomingSecureSockets = iEngine->RandomNumber(0, 1);        
        parameters->iAcknowledgeReceipts = iEngine->RandomNumber(0, 1);
        parameters->iAutoSendOnConnect = iEngine->RandomNumber(0, 1);
        parameters->iDeleteEmailsAtDisconnect = iEngine->RandomNumber(0, 1);
        parameters->iImapIdleCommand = iEngine->RandomNumber(0, 1);
        parameters->iMarkSeenInSync = iEngine->RandomNumber(0, 1);
        parameters->iEnableExpungeMode = iEngine->RandomNumber(0, 1);
        parameters->iUseApop = iEngine->RandomNumber(0, 1);
        parameters->iDisconnectedUserMode = iEngine->RandomNumber(0, 1);                
        parameters->iOutgoingSSLWrapper = iEngine->RandomNumber(0, 1);
        parameters->iOutgoingSecureSockets = iEngine->RandomNumber(0, 1);
        parameters->iIncludeSignature = EFalse;
        parameters->iAddVCard = EFalse;
        parameters->iRequestReceipts = iEngine->RandomNumber(0, 1);
        parameters->iSmtpAuth = iEngine->RandomNumber(0, 1);
        
        parameters->iIncomingPort = iEngine->RandomNumber(25, 500);
        parameters->iAttachmentSizeLimit = iEngine->RandomNumber(1, KMaxTInt);
        parameters->iBodyTextSizeLimit = iEngine->RandomNumber(1, KMaxTInt);
        parameters->iAttachmentFetchSize = iEngine->RandomNumber(1, 40960);
        parameters->iImapIdleTimeout = iEngine->RandomNumber(10, 2000);
        parameters->iMaxEmailSize = iEngine->RandomNumber(1024, KMaxTInt);
        parameters->iSyncRate = iEngine->RandomNumber(10, 500);        
        parameters->iInboxSyncLimit = iEngine->RandomNumber(-1, 256);
        parameters->iMailboxSyncLimit = iEngine->RandomNumber(-1, 256);
        parameters->iOutgoingPort = KDefaultSmtpPort;
        parameters->iToCCIncludeLimit = iEngine->RandomNumber(0, 30);                      
        
        parameters->iGetEmailOptions = (CMailboxesParameters::TGetEmailOption) iEngine->RandomNumber(CMailboxesParameters::EUndef, CMailboxesParameters::EGetEmailOptionLast-1);
        parameters->iSubscribeType = (TFolderSubscribeType) iEngine->RandomNumber(EUpdateNeither, EUpdateBoth);
        parameters->iSyncType = (TFolderSyncType) iEngine->RandomNumber(EUseCombination, EUseRemote);
        parameters->iSendCopyToSelf = (TImSMTPSendCopyToSelf) iEngine->RandomNumber(ESendNoCopy, ESendCopyAsBccRecipient);
        parameters->iSendOption = (TImSMTPSendMessageOption) iEngine->RandomNumber(ESendMessageImmediately, ESendMessageOnRequest);
        parameters->iMailboxType = (TMailboxType) iEngine->RandomNumber(EMailboxPOP3, EMailboxIMAP4);
        }
   
    TInt err = KErrNone;
    
    // make sure we have some IMEI stored
    if (iIMEI.Length() == 0)
        {
        TRAP(err, GetIMEIFromThePhoneL());
        }

    // init 
    CImImap4Settings* imap4Set = new(ELeave) CImImap4Settings;
    CleanupStack::PushL(imap4Set);
    CImPop3Settings* pop3Set = new(ELeave) CImPop3Settings;
    CleanupStack::PushL(pop3Set);
    CImSmtpSettings* smtpSet = new(ELeave) CImSmtpSettings;
    CleanupStack::PushL(smtpSet);
    CImIAPPreferences* outgoingIAPSet = CImIAPPreferences::NewLC();
    CImIAPPreferences* incomingIAPSet = CImIAPPreferences::NewLC();
    CEmailAccounts* account = CEmailAccounts::NewLC();
  
    // Get SMTP settings:
    PopulateSmtpSettingsL(*account, *smtpSet, *outgoingIAPSet, *parameters);
  
    // appending mailbox type to array to be able to use
    // correct type when deleting mailboxes
    iEntryIds.AppendL( parameters->iMailboxType );
    
    if (parameters->iMailboxType == EMailboxIMAP4)
        {
        // Get IMAP settings:
        PopulateImapSettingsL(*account, *imap4Set, *incomingIAPSet, *parameters);
        TImapAccount imapAccountId = account->CreateImapAccountL(parameters->iMailboxName->Des(), *imap4Set, *incomingIAPSet, EFalse);
        TSmtpAccount smtpAccountId = account->CreateSmtpAccountL(imapAccountId, *smtpSet, *outgoingIAPSet, EFalse);
        account->SetDefaultSmtpAccountL(smtpAccountId);

        iEntryIds.AppendL( imapAccountId.iImapAccountId );
        iEntryIds.AppendL( imapAccountId.iImapService );
        iEntryIds.AppendL( imapAccountId.iSmtpService );      
        iEntryIds.AppendL( smtpAccountId.iSmtpAccountId );
        iEntryIds.AppendL( smtpAccountId.iSmtpService );
        iEntryIds.AppendL( smtpAccountId.iRelatedService );
        
        StoreIMEIToEntryL( imapAccountId.iImapService );
        StoreIMEIToEntryL( smtpAccountId.iSmtpService );
        }
    else
        {
        // Get POP settings:
        PopulatePopSettingsL(*account, *pop3Set, *incomingIAPSet, *parameters);
        TPopAccount popAccountId = account->CreatePopAccountL(parameters->iMailboxName->Des(), *pop3Set, *incomingIAPSet, EFalse);
        TSmtpAccount smtpAccountId = account->CreateSmtpAccountL(popAccountId, *smtpSet, *outgoingIAPSet, EFalse);     
        account->SetDefaultSmtpAccountL(smtpAccountId);
        
        iEntryIds.AppendL( popAccountId.iPopAccountId );
        iEntryIds.AppendL( popAccountId.iPopService );
        iEntryIds.AppendL( popAccountId.iSmtpService );
        iEntryIds.AppendL( smtpAccountId.iSmtpAccountId );
        iEntryIds.AppendL( smtpAccountId.iSmtpService );
        iEntryIds.AppendL( smtpAccountId.iRelatedService );

        StoreIMEIToEntryL( popAccountId.iPopService );
        StoreIMEIToEntryL( smtpAccountId.iSmtpService );
        }  
    CleanupStack::PopAndDestroy(6); //imap4Set, pop3Set, smtpSet, outgoingIAPSet, incomingIAPSet, account 
        
    return err;
    }

void CCreatorMailboxes::PopulateImapSettingsL( 
            CEmailAccounts& aEmailAccount, 
            CImImap4Settings& aImapSettings, 
            CImIAPPreferences& aIApSettings, 
            const CMailboxesParameters& aParameters )
    {
    // Get default settings first:
    aEmailAccount.PopulateDefaultImapSettingsL(aImapSettings, aIApSettings);
    
    TImIAPChoice iapChoice;
    iapChoice.iIAP = 0;
    if( aParameters.iIncomingConnectionMethod == 0 || 
        aParameters.iIncomingConnectionMethod->Des().Length() == 0 )
        {
        TRAP_IGNORE( iapChoice.iIAP = iEngine->AccessPointNameToIdL( KInternet ));
        }
    else
        {
        TRAP_IGNORE( iapChoice.iIAP = iEngine->AccessPointNameToIdL( aParameters.iIncomingConnectionMethod->Des()));
        }
    iapChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
    aIApSettings.AddIAPL(iapChoice);
    
    TBuf8<256> buf8;
    if( aParameters.iIncomingLoginName && aParameters.iIncomingLoginName->Length() > 0 )                
        buf8.Copy(aParameters.iIncomingLoginName->Des());
    else
        buf8.Copy(iEngine->RandomString(CCreatorEngine::EFirstName));
    aImapSettings.SetLoginNameL(buf8);  //8
    
    if( aParameters.iIncomingPassword && aParameters.iIncomingPassword->Length() > 0 )
        buf8.Copy(aParameters.iIncomingPassword->Des());    
    else
        buf8.Copy(iEngine->RandomString(CCreatorEngine::EFirstName));
    aImapSettings.SetPasswordL(buf8);  //8
    
    if( aParameters.iIncomingFolderPath && aParameters.iIncomingFolderPath->Length() > 0 )
        {
        buf8.Copy(aParameters.iIncomingFolderPath->Des());
        aImapSettings.SetFolderPathL(buf8);
        }
    
    if( aParameters.iPathSeparator && aParameters.iPathSeparator->Length() > 0 )
        aImapSettings.SetPathSeparator(aParameters.iIncomingFolderPath->Des()[0]);
    
    aImapSettings.SetDisconnectedUserMode(aParameters.iDisconnectedUserMode);
    aImapSettings.SetSynchronise(aParameters.iSyncType);
    aImapSettings.SetSubscribe(aParameters.iSubscribeType);
    aImapSettings.SetAutoSendOnConnect(aParameters.iAutoSendOnConnect);
    
    if(aParameters.iMaxEmailSize != KUndef )
        aImapSettings.SetMaxEmailSize(aParameters.iMaxEmailSize);
    
    aImapSettings.SetDeleteEmailsWhenDisconnecting(aParameters.iDeleteEmailsAtDisconnect);
    aImapSettings.SetAcknowledgeReceipts(aParameters.iAcknowledgeReceipts);
            
    if(aParameters.iGetEmailOptions == CMailboxesParameters::EGetBodyText )
        aImapSettings.SetGetMailOptions(EGetImap4EmailBodyText);
    else if(aParameters.iGetEmailOptions == CMailboxesParameters::EGetBodyTextAndAttachments )
        aImapSettings.SetGetMailOptions(EGetImap4EmailBodyTextAndAttachments);
    else if(aParameters.iGetEmailOptions == CMailboxesParameters::EGetAttachments )
        aImapSettings.SetGetMailOptions(EGetImap4EmailAttachments);
    else if(aParameters.iGetEmailOptions == CMailboxesParameters::EGetBodyAlternativeText )
        aImapSettings.SetGetMailOptions(EGetImap4EmailBodyAlternativeText);
    else
        aImapSettings.SetGetMailOptions(EGetImap4EmailHeaders); // Default
    
    if( aParameters.iInboxSyncLimit != KUndef )
        aImapSettings.SetInboxSynchronisationLimit(aParameters.iInboxSyncLimit);
    
    if( aParameters.iMailboxSyncLimit != KUndef )
        aImapSettings.SetMailboxSynchronisationLimit(aParameters.iMailboxSyncLimit);
    
    aImapSettings.SetUpdatingSeenFlags(aParameters.iMarkSeenInSync);
    
    if( aParameters.iSyncRate != KUndef )
        aImapSettings.SetSyncRateL(aParameters.iSyncRate);
    
    if( aParameters.iAttachmentFetchSize != KUndef )
        aImapSettings.SetFetchSizeL(aParameters.iAttachmentFetchSize);
    
    aImapSettings.SetImapIdle(aParameters.iImapIdleCommand);
    if( aParameters.iImapIdleTimeout != KUndef )
        aImapSettings.SetImapIdleTimeoutL(aParameters.iImapIdleTimeout);
    
    if( aParameters.iBodyTextSizeLimit != KUndef )
        aImapSettings.SetBodyTextSizeLimitL(aParameters.iBodyTextSizeLimit);
    
    if( aParameters.iAttachmentSizeLimit != KUndef )
        aImapSettings.SetAttachmentSizeLimitL(aParameters.iAttachmentSizeLimit);
    
#if(!defined __SERIES60_30__ && !defined __SERIES60_31__)
    aImapSettings.SetUseExpunge(aParameters.iEnableExpungeMode);
#endif
    
    if(aParameters.iIncomingServerName && aParameters.iIncomingServerName->Length() > 0 )
        aImapSettings.SetServerAddressL(aParameters.iIncomingServerName->Des());
    else
        aImapSettings.SetServerAddressL(KDefaultIncomingServer);
        
    if( aParameters.iIncomingPort != KUndef )
        aImapSettings.SetPort(aParameters.iIncomingPort);                 
      
    aImapSettings.SetSecureSockets(aParameters.iIncomingSecureSockets);
    aImapSettings.SetSSLWrapper(aParameters.iIncomingSSLWrapper);
    }

void CCreatorMailboxes::PopulatePopSettingsL( 
            CEmailAccounts& aEmailAccount, 
            CImPop3Settings& aPopSettings, 
            CImIAPPreferences& aIApSettings, 
            const CMailboxesParameters& aParameters )
    {
    aEmailAccount.PopulateDefaultPopSettingsL(aPopSettings, aIApSettings);
    
    TImIAPChoice iapChoice;
    iapChoice.iIAP = 0;
    if( aParameters.iIncomingConnectionMethod == 0 || 
        aParameters.iIncomingConnectionMethod->Des().Length() == 0 )
        {
        TRAP_IGNORE( iapChoice.iIAP = iEngine->AccessPointNameToIdL( KInternet ));
        }
    else
        {
        TRAP_IGNORE( iapChoice.iIAP = iEngine->AccessPointNameToIdL( aParameters.iIncomingConnectionMethod->Des() ));
        }
    iapChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
    aIApSettings.AddIAPL(iapChoice);
    
    TBuf8<256> buf8;
    if( aParameters.iIncomingLoginName && aParameters.iIncomingLoginName->Length() > 0 )                
        buf8.Copy(aParameters.iIncomingLoginName->Des());
    else
        buf8.Copy(iEngine->RandomString(CCreatorEngine::EFirstName));
    aPopSettings.SetLoginNameL(buf8);  //8
        
    if( aParameters.iIncomingPassword && aParameters.iIncomingPassword->Length() > 0 )
        buf8.Copy(aParameters.iIncomingPassword->Des());    
    else
        buf8.Copy(iEngine->RandomString(CCreatorEngine::EFirstName));
    aPopSettings.SetPasswordL(buf8);  //8
    
    aPopSettings.SetAutoSendOnConnect( aParameters.iAutoSendOnConnect );
    aPopSettings.SetApop( aParameters.iUseApop );
    aPopSettings.SetDisconnectedUserMode( aParameters.iDisconnectedUserMode );
    aPopSettings.SetDeleteEmailsWhenDisconnecting( aParameters.iDeleteEmailsAtDisconnect );
    aPopSettings.SetAcknowledgeReceipts( aParameters.iAcknowledgeReceipts );
    
    if( aParameters.iMaxEmailSize != KUndef )
        aPopSettings.SetMaxEmailSize(aParameters.iMaxEmailSize);
    
    if(aParameters.iGetEmailOptions != CMailboxesParameters::EGetHeaders && 
            aParameters.iGetEmailOptions != CMailboxesParameters::EUndef )
        {
        aPopSettings.SetGetMailOptions(EGetPop3EmailMessages);
        aPopSettings.SetPopulationLimitL( -1 ); // -1 = body and attachment
        }
    else
        {
        aPopSettings.SetGetMailOptions(EGetPop3EmailHeaders); // Default
        aPopSettings.SetPopulationLimitL( -2 ); // -2 = headers only
        }
    
    if( aParameters.iInboxSyncLimit != KUndef )
        {
        aPopSettings.SetInboxSynchronisationLimit(aParameters.iInboxSyncLimit);
        }
    
    if(aParameters.iIncomingServerName && aParameters.iIncomingServerName->Length() > 0 )
        aPopSettings.SetServerAddressL(aParameters.iIncomingServerName->Des());
    else
        aPopSettings.SetServerAddressL(KDefaultIncomingServer);
            
    if( aParameters.iIncomingPort != KUndef )
        aPopSettings.SetPort(aParameters.iIncomingPort);                 
          
    aPopSettings.SetSecureSockets(aParameters.iIncomingSecureSockets);
    aPopSettings.SetSSLWrapper(aParameters.iIncomingSSLWrapper);    
    }

void CCreatorMailboxes::PopulateSmtpSettingsL( 
        CEmailAccounts& aEmailAccount, 
        CImSmtpSettings& aSmtpSettings, 
        CImIAPPreferences& aIApSettings, 
        const CMailboxesParameters& aParameters )
    {    
    // Get default settings first: 
    aEmailAccount.PopulateDefaultSmtpSettingsL(aSmtpSettings, aIApSettings);
    TImIAPChoice iapChoice;
    iapChoice.iIAP = 0;
    if( aParameters.iOutgoingConnectionMethod == 0 || 
        aParameters.iOutgoingConnectionMethod->Des().Length() == 0 )
        {
        TRAP_IGNORE( iapChoice.iIAP = iEngine->AccessPointNameToIdL( KInternet ));
        }
    else
        {
        TRAP_IGNORE( iapChoice.iIAP = iEngine->AccessPointNameToIdL( aParameters.iOutgoingConnectionMethod->Des() ));
        }
    iapChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
    aIApSettings.AddIAPL(iapChoice);
    
    // set SMTP preferences
    if( aParameters.iOwnEmailAlias && aParameters.iOwnEmailAlias->Length() > 0 )
        aSmtpSettings.SetEmailAliasL(aParameters.iOwnEmailAlias->Des());
    
    if( aParameters.iOwnEmailAddress && aParameters.iOwnEmailAddress->Length() > 0 )    
        aSmtpSettings.SetEmailAddressL(aParameters.iOwnEmailAddress->Des());
    
    if( aParameters.iReplyToAddress && aParameters.iReplyToAddress->Length() > 0 )
        aSmtpSettings.SetReplyToAddressL(aParameters.iReplyToAddress->Des());
    
    if( aParameters.iReceiptAddress && aParameters.iReceiptAddress->Length() > 0 )
        aSmtpSettings.SetReceiptAddressL(aParameters.iReceiptAddress->Des());    
    
    aSmtpSettings.SetBodyEncoding(EMsgOutboxMIME);
    aSmtpSettings.SetAddVCardToEmail(aParameters.iAddVCard);
    aSmtpSettings.SetAddSignatureToEmail(aParameters.iIncludeSignature);
    aSmtpSettings.SetRequestReceipts(aParameters.iRequestReceipts);
    aSmtpSettings.SetSendCopyToSelf(aParameters.iSendCopyToSelf);    
    aSmtpSettings.SetSendMessageOption(aParameters.iSendOption);        
            
    TBuf8<256> buf8;
    if( aParameters.iOutgoingLoginName && aParameters.iOutgoingLoginName->Length() > 0 )
        {
        buf8.Copy(aParameters.iOutgoingLoginName->Des());
        aSmtpSettings.SetLoginNameL(buf8);  //8
        }
    
    if( aParameters.iOutgoingPassword && aParameters.iOutgoingPassword->Length() > 0)
        {
        buf8.Copy(aParameters.iOutgoingPassword->Des());
        aSmtpSettings.SetPasswordL(buf8);  //8
        }
    
    aSmtpSettings.SetSMTPAuth(aParameters.iSmtpAuth);
    if( aParameters.iToCCIncludeLimit != KUndef )
         aSmtpSettings.SetToCcIncludeLimitL(aParameters.iToCCIncludeLimit);
    
    if( aParameters.iOutgoingServerName && aParameters.iOutgoingServerName->Length() > 0 )
        aSmtpSettings.SetServerAddressL(aParameters.iOutgoingServerName->Des());
    else
        aSmtpSettings.SetServerAddressL(KDefaultOutgoingServer);
    
    if( aParameters.iOutgoingPort != KUndef )
        aSmtpSettings.SetPort(aParameters.iOutgoingPort);
                
    aSmtpSettings.SetSecureSockets(aParameters.iOutgoingSecureSockets);
    aSmtpSettings.SetSSLWrapper(aParameters.iOutgoingSSLWrapper);
    }

//----------------------------------------------------------------------------

void CCreatorMailboxes::StoreIMEIToEntryL(const TMsvId aMailboxId)
    {
    LOGSTRING("Creator: CCreatorMailboxes::StoreIMEIToEntryL");

    CMsvSession* session = CMsvSession::OpenSyncL(*this);
    CleanupStack::PushL(session);
    
    CMsvEntry* centry = session->GetEntryL(aMailboxId);
    CleanupStack::PushL(centry);

    TMsvEntry tentry = centry->Entry();
    tentry.iDescription.Set(iIMEI);
    centry->ChangeL(tentry);

    CleanupStack::PopAndDestroy(2); // centry, session
    }

//----------------------------------------------------------------------------

void CCreatorMailboxes::GetIMEIFromThePhoneL()
    {
    LOGSTRING("Creator: CCreatorMailboxes::GetIMEIFromThePhoneL");

#ifndef __WINS__

    RTelServer telServer;
    User::LeaveIfError( telServer.Connect() );
    CleanupClosePushL(telServer);

    TInt numPhones = 0;
    User::LeaveIfError( telServer.EnumeratePhones( numPhones ) );
    if ( numPhones < 1 )
        {
        User::Leave( KErrNotFound );
        }
    RTelServer::TPhoneInfo info;
    User::LeaveIfError( telServer.GetPhoneInfo( 0, info ) );
    RMobilePhone mobilePhone;
    User::LeaveIfError( mobilePhone.Open( telServer, info.iName ) );
    CleanupClosePushL( mobilePhone );

    //mobilePhone.Initialise();
    TUint32 identityCaps;
    User::LeaveIfError( mobilePhone.GetIdentityCaps( identityCaps ) );

    if ( identityCaps & RMobilePhone::KCapsGetSerialNumber )
        {
        TRequestStatus status;
        RMobilePhone::TMobilePhoneIdentityV1 mobilePhoneIdentity;

        mobilePhone.GetPhoneId( status, mobilePhoneIdentity );

        User::WaitForRequest( status );
        User::LeaveIfError( status.Int() );

        iIMEI.Copy( mobilePhoneIdentity.iSerialNumber );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }

    CleanupStack::PopAndDestroy( &mobilePhone );
    CleanupStack::PopAndDestroy( &telServer );

#else

    // use fixed IMEI for emulator
    _LIT( KIMAEmulatorImei, "123456789012345" );
    iIMEI.Copy( KIMAEmulatorImei );

#endif // __WINS__

    }
    
//----------------------------------------------------------------------------

void CCreatorMailboxes::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/) // from MMsvSessionObserver
    {
    }

//----------------------------------------------------------------------------
void CCreatorMailboxes::DeleteAllL()
    {
    LOGSTRING("Creator: CCreatorMailboxes::DeleteAllL");
    
    CEmailAccounts* accountMgr = CEmailAccounts::NewLC();
    
    // Delete pop accounts
    RArray<TPopAccount> popAccountsToDelete;
    CleanupClosePushL( popAccountsToDelete );
    accountMgr->GetPopAccountsL( popAccountsToDelete );
    for ( TInt i = 0; i < popAccountsToDelete.Count(); i++ )
        {
        accountMgr->DeletePopAccountL( popAccountsToDelete[i] );
        }
    CleanupStack::PopAndDestroy( &popAccountsToDelete );
    
    // Delete imap accounts
    RArray<TImapAccount> imapAccountsToDelete;
    CleanupClosePushL( imapAccountsToDelete );
    accountMgr->GetImapAccountsL( imapAccountsToDelete );
    for ( TInt i = 0; i < imapAccountsToDelete.Count(); i++ )
        {
        accountMgr->DeleteImapAccountL( imapAccountsToDelete[i] );
        }
    CleanupStack::PopAndDestroy( &imapAccountsToDelete );
    
    // Delete smtp accounts
    RArray<TSmtpAccount> smtpAccountsToDelete;
    CleanupClosePushL( smtpAccountsToDelete );
    accountMgr->GetSmtpAccountsL( smtpAccountsToDelete );
    for ( TInt i = 0; i < smtpAccountsToDelete.Count(); i++ )
        {
        accountMgr->DeleteSmtpAccountL( smtpAccountsToDelete[i] );
        }
    CleanupStack::PopAndDestroy( &smtpAccountsToDelete );

    iEntryIds.Reset();
    
    // all entries deleted, remove the Mailbox related registry
    iEngine->RemoveStoreL( KUidDictionaryUidMailbox );
    
    CleanupStack::PopAndDestroy( accountMgr ); 
    }

//----------------------------------------------------------------------------
void CCreatorMailboxes::DeleteAllCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorMailboxes::DeleteAllCreatedByCreatorL");
    
    iEntryIds.Reset();
    CEmailAccounts* accountMgr = CEmailAccounts::NewLC();
    
    // fetch ids of entries created by Creator
    iEngine->ReadEntryIdsFromStoreL( iEntryIds, KUidDictionaryUidMailbox );
    
    TInt i(0);
    while ( i < iEntryIds.Count() && iEntryIds.Count() > 6 ) 
        {
        // there should be at least 7 items in the array:
        // one for mailbox type
        // 3 for imap OR pop ids
        // 3 for smtp ids

        // delete entries
        
        TInt type = iEntryIds[i++]; // TMailboxType
        if ( type == EMailboxPOP3 )
            {
            TPopAccount popAccount;
            popAccount.iPopAccountId = iEntryIds[i++];
            popAccount.iPopService = iEntryIds[i++];            
            popAccount.iSmtpService = iEntryIds[i++];
            TRAP_IGNORE( accountMgr->DeletePopAccountL( popAccount ) );            
            }
        else if ( type == EMailboxIMAP4 )
            {
            TImapAccount imapAccount;
            imapAccount.iImapAccountId = iEntryIds[i++];
            imapAccount.iImapService = iEntryIds[i++];
            imapAccount.iImapService = iEntryIds[i++];
            TRAP_IGNORE( accountMgr->DeleteImapAccountL( imapAccount ) );
            }
        else 
            {
            // ignore unknown mailbox types
            i +=3;
            }
        
        // There should be one smtp service for each pop and imap service
        TSmtpAccount smtpAccount;
        smtpAccount.iSmtpAccountId = iEntryIds[i++];
        smtpAccount.iSmtpService = iEntryIds[i++];
        smtpAccount.iRelatedService = iEntryIds[i++];
        TRAP_IGNORE( accountMgr->DeleteSmtpAccountL( smtpAccount ) );
        }

    // reset must be done here, because iEntryIds is stored in destructor
    iEntryIds.Reset();
    
    // all entries deleted, remove the mailbox related registry
    iEngine->RemoveStoreL( KUidDictionaryUidMailbox );

    CleanupStack::PopAndDestroy( accountMgr ); 
    }
//----------------------------------------------------------------------------

