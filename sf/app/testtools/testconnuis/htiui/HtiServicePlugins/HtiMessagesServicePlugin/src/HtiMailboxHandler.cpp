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
* Description:  Implementation of HTI mailbox settings handling.
*
*/


// INCLUDE FILES
#include <cmconnectionmethod.h>
#include <cmconnectionmethoddef.h>
#include <cmdestination.h>
#include <cmmanager.h>
#include <cemailaccounts.h>
#include <etelmm.h>
#include <iapprefs.h>
#include <imapset.h>
#include <mtmuibas.h>
#include <SendUiConsts.h>
#include <pop3set.h>
#include <smtpset.h>
#include <txtrich.h>

#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>
#include "HtiMessagesServicePlugin.h"
#include "HtiMailboxHandler.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KMinCreateMsgLength = 36;
const TInt KMinDeleteMsgLength = 3;

const TInt KImeiLength = 15;

const TInt KDefaultImapPortSSL = 993;
const TInt KDefaultPopPortSSL  = 995;
const TInt KDefaultSmtpPortSSL = 465;

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT8( KErrorInvalidParameters, "Invalid command parameters" );
_LIT8( KErrorCreateFailed, "Mailbox creation failed" );
_LIT8( KErrorDeleteFailed, "Mailbox deletion failed" );

#ifdef __WINS__
_LIT( KWinsImei, "123456789012345" );
#else
_LIT( KTsyName, "phonetsy" );
#endif

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiMailboxHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CHtiMailboxHandler* CHtiMailboxHandler::NewL()
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::NewL" );
    CHtiMailboxHandler* self = new (ELeave) CHtiMailboxHandler();
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::NewL" );
    return self;
    }


// ----------------------------------------------------------------------------
// CHtiMailboxHandler::CHtiMailboxHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
CHtiMailboxHandler::CHtiMailboxHandler()
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::CHtiMailboxHandler" );

    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::CHtiMailboxHandler" );
    }


// -----------------------------------------------------------------------------
// CHtiMailboxHandler::~CHtiMailboxHandler
// Destructor.
// -----------------------------------------------------------------------------
CHtiMailboxHandler::~CHtiMailboxHandler()
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::~CHtiMailboxHandler" );
    delete iMailboxName;
    delete iIncomingServer;
    delete iIncomingApName;
    delete iIncomingUserName;
    delete iIncomingPassword;
    delete iOutgoingServer;
    delete iOutgoingApName;
    delete iOutgoingUserName;
    delete iOutgoingPassword;
    delete iOwnMailAddress;
    delete iOwnName;
    delete iSignatureText;
    delete iImapFolderPath;
    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::~CHtiMailboxHandler" );
    }


// -----------------------------------------------------------------------------
// CHtiMailboxHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CHtiMailboxHandler::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::ConstructL" );

    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::ConstructL" );
    }


// -----------------------------------------------------------------------------
// CHtiMailboxHandler::SetDispatcher
// Sets the dispatcher pointer.
// -----------------------------------------------------------------------------

void CHtiMailboxHandler::SetDispatcher( MHtiDispatcher* aDispatcher )
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::SetDispatcher" );
    iDispatcher = aDispatcher;
    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::SetDispatcher" );
    }


// -----------------------------------------------------------------------------
// CHtiMailboxHandler::ProcessMessageL
// Parses the received message and calls handler functions.
// -----------------------------------------------------------------------------
void CHtiMailboxHandler::ProcessMessageL( const TDesC8& aMessage,
    THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::ProcessMessageL" );

    // Zero length message and command code validity already checked
    // in HtiMessagesServicePlugin.

    if ( aMessage[0] == CHtiMessagesServicePlugin::ECreateMailBox )
        {
        if ( aMessage.Length() < KMinCreateMsgLength )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            iCmManager.OpenL();
            TRAPD( err, HandleCreateMailboxL(
                    aMessage.Right( aMessage.Length() - 1 ) ) );
            if ( err != KErrNone )
                {
                if ( err == KErrArgument )
                    {
                    SendErrorMessageL( err, KErrorInvalidParameters );
                    }
                else
                    {
                    HTI_LOG_FORMAT( "Create error %d", err );
                    SendErrorMessageL( err, KErrorCreateFailed );
                    }
                }
            else
                {
                SendOkMsgL( KNullDesC8 );
                }
            ResetVariables();
            }
        }

    else
        {
        if ( aMessage.Length() < KMinDeleteMsgLength )
            {
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            }
        else
            {
            TRAPD( err, HandleDeleteMailboxL(
                    aMessage.Right( aMessage.Length() - 1 ) ) );
            if ( err != KErrNone )
                {
                HTI_LOG_FORMAT( "Delete error %d", err );
                SendErrorMessageL( err, KErrorDeleteFailed );
                }
            else
                {
                SendOkMsgL( KNullDesC8 );
                }
            }
        }

    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::ProcessMessageL" );
    }

// ----------------------------------------------------------------------------
// CHtiMailboxHandler::HandleCreateMailboxL
// Creates new mailbox.
// ----------------------------------------------------------------------------
void CHtiMailboxHandler::HandleCreateMailboxL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::HandleCreateMailboxL" );

    ParseCreateMsgL( aData );

    TSmtpAccount smtpAcc;
    TInt result = FindSmtpAccount( *iMailboxName, smtpAcc );
    if ( result != KErrNotFound )
        {
        User::Leave( KErrAlreadyExists );
        }

    CImImap4Settings* imap4Set = new (ELeave) CImImap4Settings;
    CleanupStack::PushL( imap4Set );
    CImPop3Settings* pop3Set = new (ELeave) CImPop3Settings;
    CleanupStack::PushL( pop3Set );
    CImSmtpSettings* smtpSet = new (ELeave) CImSmtpSettings;
    CleanupStack::PushL( smtpSet );

    CImIAPPreferences* incomingIapSet = CImIAPPreferences::NewLC();
    CImIAPPreferences* outgoingIapSet = CImIAPPreferences::NewLC();

    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    accounts->PopulateDefaultSmtpSettingsL( *smtpSet, *outgoingIapSet );
    if ( iMailboxType == 0 )  // POP3
        {
        accounts->PopulateDefaultPopSettingsL( *pop3Set, *incomingIapSet );
        }
    else // IMAP4
        {
        accounts->PopulateDefaultImapSettingsL( *imap4Set, *incomingIapSet );
        }

    // Set access point settings
    TImIAPChoice imIAPChoice;
    imIAPChoice.iIAP = 0;
    imIAPChoice.iDialogPref = ECommDbDialogPrefPrompt;

    if ( iIncomingApName->Length() > 0 )
        {
        User::LeaveIfError(
            imIAPChoice.iIAP = AccessPointUIDL( *iIncomingApName ) );
        imIAPChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
        HTI_LOG_TEXT( "Incoming IAP user defined" );
        }
    incomingIapSet->AddIAPL( imIAPChoice );

    imIAPChoice.iIAP = 0;
    imIAPChoice.iDialogPref = ECommDbDialogPrefPrompt;

    if ( iOutgoingApName->Length() > 0 )
        {
        User::LeaveIfError(
            imIAPChoice.iIAP = AccessPointUIDL( *iOutgoingApName ) );
        imIAPChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
        HTI_LOG_TEXT( "Outgoing IAP user defined" );
        }
    outgoingIapSet->AddIAPL( imIAPChoice );

    // Set SMTP settings
    HTI_LOG_TEXT( "Setting SMTP settings" );
    smtpSet->SetServerAddressL( *iOutgoingServer );
    smtpSet->SetLoginNameL( *iOutgoingUserName );
    smtpSet->SetPasswordL( *iOutgoingPassword );
    smtpSet->SetEmailAddressL( *iOwnMailAddress );
    smtpSet->SetReplyToAddressL( *iOwnMailAddress );
    smtpSet->SetEmailAliasL( *iOwnName );
    if ( iSignatureText->Length() > 0 )
        {
        smtpSet->SetAddSignatureToEmail( ETrue );
        }
    else
        {
        smtpSet->SetAddSignatureToEmail( EFalse );
        }
    smtpSet->SetSendCopyToSelf( (TImSMTPSendCopyToSelf) iSendCopyToOwnAddress );
    smtpSet->SetSendMessageOption( (TImSMTPSendMessageOption) iSendOption );
    if ( iOutgoingSecurity == 0 )
        {
        smtpSet->SetSecureSockets( EFalse );
        }
    else
        {
        smtpSet->SetSecureSockets( ETrue );
        if ( iOutgoingSecurity == 1 )
            {
            smtpSet->SetSSLWrapper( EFalse );
            }
        else // 2 = SSL
            {
            smtpSet->SetSSLWrapper( ETrue );
            }
        }
    if ( iOutgoingPort > 0 )
        {
        smtpSet->SetPort( (TUint) iOutgoingPort );
        }
    else
        {
        if ( iOutgoingSecurity == 2 ) // SSL
            {
            smtpSet->SetPort( KDefaultSmtpPortSSL );
            }
        else
            {
            smtpSet->SetPort( KSMTPDefaultPortNumber );
            }
        }
    smtpSet->SetBodyEncoding( EMsgOutboxMIME );
    smtpSet->SetAddVCardToEmail( EFalse );
    smtpSet->SetRequestReceipts( EFalse );
    smtpSet->SetSMTPAuth( ETrue );

    TSmtpAccount smtpAccount;
    TPopAccount popAccount;
    TImapAccount imapAccount;

    // Set POP3 settings
    if ( iMailboxType == 0 )  // POP3
        {
        HTI_LOG_TEXT( "Setting POP3 settings" );
        pop3Set->SetServerAddressL( *iIncomingServer );
        pop3Set->SetLoginNameL( *iIncomingUserName );
        pop3Set->SetPasswordL( *iIncomingPassword );
        pop3Set->SetApop( iAPopSecureLogin );
        if ( iIncomingSecurity == 0 )
            {
            pop3Set->SetSecureSockets( EFalse );
            }
        else
            {
            pop3Set->SetSecureSockets( ETrue );
            if ( iIncomingSecurity == 1 )
                {
                pop3Set->SetSSLWrapper( EFalse );
                }
            else
                {
                pop3Set->SetSSLWrapper( ETrue );
                }
            }
        if ( iIncomingPort > 0 )
            {
            pop3Set->SetPort( (TUint) iIncomingPort );
            }
        else
            {
            if ( iIncomingSecurity == 2 ) // SSL
                {
                pop3Set->SetPort( KDefaultPopPortSSL );
                }
            else
                {
                pop3Set->SetPort( KPOP3DefaultPortNumber );
                }
            }
        if ( iRetrievedParts == 0 ) // Only headers
            {
            pop3Set->SetGetMailOptions( EGetPop3EmailHeaders );
            pop3Set->SetPopulationLimitL( -2 ); // -2 = headers only (EmailUtils.h)
            }
        else
            {
            pop3Set->SetGetMailOptions( EGetPop3EmailMessages );
            if ( iRetrievedParts == 1 ) // Less than (kb)
                {
                pop3Set->SetMaxEmailSize( iRetrieveSizeLimitKb );
                pop3Set->SetPopulationLimitL( iRetrieveSizeLimitKb );
                }
            else
                {
                pop3Set->SetPopulationLimitL( -1 ); // -1 = body and attachment (EmailUtils.h)
                }
            }

        if ( iEmailsToRetrieve > 0 )
            {
            pop3Set->SetInboxSynchronisationLimit( iEmailsToRetrieve );
            }
        else
            {
            // If limit is not set (zero), retrieves all
            pop3Set->SetInboxSynchronisationLimit( KErrNotFound );
            }

        pop3Set->SetAcknowledgeReceipts( EFalse );
        pop3Set->SetDisconnectedUserMode( ETrue );
        pop3Set->SetAutoSendOnConnect( ETrue );
        pop3Set->SetDeleteEmailsWhenDisconnecting( EFalse );

        popAccount = accounts->CreatePopAccountL(
            *iMailboxName, *pop3Set, *incomingIapSet, EFalse );
        HTI_LOG_FORMAT( "Created POP3 account %d", popAccount.iPopAccountId );
        smtpAccount = accounts->CreateSmtpAccountL(
            popAccount, *smtpSet, *outgoingIapSet, EFalse );
        HTI_LOG_FORMAT( "Created SMTP account %d", smtpAccount.iSmtpAccountId );
        }

    // Set IMAP4 settings
    else
        {
        HTI_LOG_TEXT( "Setting IMAP4 settings" );
        imap4Set->SetServerAddressL( *iIncomingServer );
        imap4Set->SetLoginNameL( *iIncomingUserName );
        imap4Set->SetPasswordL( *iIncomingPassword );
        if ( iIncomingSecurity == 0 )
            {
            imap4Set->SetSecureSockets( EFalse );
            }
        else
            {
            imap4Set->SetSecureSockets( ETrue );
            if ( iIncomingSecurity == 1 )
                {
                imap4Set->SetSSLWrapper( EFalse );
                }
            else
                {
                imap4Set->SetSSLWrapper( ETrue );
                }
            }
        if ( iIncomingPort > 0 )
            {
            imap4Set->SetPort( (TUint) iIncomingPort );
            }
        else
            {
            if ( iIncomingSecurity == 2 ) // SSL
                {
                imap4Set->SetPort( KDefaultImapPortSSL );
                }
            else
                {
                imap4Set->SetPort( KIMAPDefaultPortNumber );
                }
            }
        if ( iEmailsToRetrieve > 0 )
            {
            imap4Set->SetInboxSynchronisationLimit( iEmailsToRetrieve );
            imap4Set->SetMailboxSynchronisationLimit( iEmailsToRetrieve );
            }
        else
            {
            // If limit is not set (zero), retrieves all
            imap4Set->SetInboxSynchronisationLimit( KErrNotFound );
            imap4Set->SetMailboxSynchronisationLimit( KErrNotFound );
            }

        imap4Set->SetFolderPathL( *iImapFolderPath );

        imap4Set->SetGetMailOptions( EGetImap4EmailHeaders );
        imap4Set->SetPartialMailOptionsL( ENoSizeLimits );
        imap4Set->SetBodyTextSizeLimitL( KMaxTInt );
        imap4Set->SetAttachmentSizeLimitL( KMaxTInt );
        imap4Set->SetAcknowledgeReceipts( EFalse );
        imap4Set->SetDisconnectedUserMode( ETrue );
        imap4Set->SetSynchronise( EUseLocal );
        imap4Set->SetSubscribe( EUpdateNeither );
        imap4Set->SetAutoSendOnConnect( ETrue );
        imap4Set->SetDeleteEmailsWhenDisconnecting( EFalse );
        imap4Set->SetImapIdle( ETrue );
        imap4Set->SetUpdatingSeenFlags( ETrue );

        imapAccount = accounts->CreateImapAccountL(
            *iMailboxName, *imap4Set, *incomingIapSet, EFalse );
        HTI_LOG_FORMAT( "Created IMAP4 account %d", imapAccount.iImapAccountId );
        smtpAccount = accounts->CreateSmtpAccountL(
            imapAccount, *smtpSet, *outgoingIapSet, EFalse );
        HTI_LOG_FORMAT( "Created SMTP account %d", smtpAccount.iSmtpAccountId );
        }

    // Set as default if requested
     if ( iSetAsDefault )
        {
        accounts->SetDefaultSmtpAccountL( smtpAccount );
        }

    // accounts, outgoingIapSet, incomingIapSet, smtpSet, pop3Set, imap4set
    CleanupStack::PopAndDestroy( 6 );

    // Get IDs of created incoming mail account
    TMsvId mailboxId = 0;
    TUint32 accountId = 0;
    TUid protocol = TUid::Null();
    if ( iMailboxType == 0 ) // POP3
        {
        mailboxId = popAccount.iPopService;
        accountId = popAccount.iPopAccountId;
        protocol.iUid = KSenduiMtmPop3UidValue;
        }

    else // IMAP4
        {
        mailboxId = imapAccount.iImapService;
        accountId = imapAccount.iImapAccountId;
        protocol.iUid = KSenduiMtmImap4UidValue;
        iRetrievedParts = 0;
        }

    // Get the created mailbox entries
    HTI_LOG_TEXT( "Opening MSV session" );
    CMsvSession* session = CMsvSession::OpenSyncL( *this );
    CleanupStack::PushL( session );
    HTI_LOG_TEXT( "Getting mailbox MSV entries" );
    CMsvEntry* incomingEntry = session->GetEntryL( mailboxId );
    CleanupStack::PushL( incomingEntry );
    CMsvEntry* outgoingEntry = session->GetEntryL( smtpAccount.iSmtpService );
    CleanupStack::PushL( outgoingEntry );

    // Store account IDs and device IMEI to entries
    HTI_LOG_TEXT( "Storing account IDs and IMEI" );
    TBuf<KImeiLength> phoneImei;
    GetDeviceImeiL( phoneImei );
    HTI_LOG_FORMAT( "Got IMEI: %S", &phoneImei );
    TMsvEntry inTEntry = incomingEntry->Entry();
    inTEntry.iMtmData2 = accountId;
    inTEntry.iDescription.Set( phoneImei );
    incomingEntry->ChangeL( inTEntry );
    TMsvEntry outTEntry = outgoingEntry->Entry();
    outTEntry.iMtmData2 = smtpAccount.iSmtpAccountId;
    outTEntry.iDescription.Set( phoneImei );
    outgoingEntry->ChangeL( outTEntry );

    // Create the signature text if given
    if ( iSignatureText->Length() > 0 )
        {
        // Get signature store
        HTI_LOG_TEXT( "Getting signature store" );
        CMsvStore* store = outgoingEntry->EditStoreL();
        CleanupStack::PushL( store );

        // Create signarure rich text
        HTI_LOG_TEXT( "Creating signature CRichText" );
        CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL();
        CleanupStack::PushL( paraFormatLayer );
        CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL();
        CleanupStack::PushL( charFormatLayer );
        CRichText* signature = CRichText::NewL( paraFormatLayer,
                                                charFormatLayer );
        CleanupStack::PushL( signature );
        signature->InsertL( 0, *iSignatureText );

        // Store the signature
        store->StoreBodyTextL( *signature );
        store->CommitL();

        // signature, charFormatLayer, paraFormatLayer, store
        CleanupStack::PopAndDestroy( 4 );
        }

    CleanupStack::PopAndDestroy( 3 ); // outgoingEntry, incomingEntry, session
    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::HandleCreateMailboxL" );
    }


// ----------------------------------------------------------------------------
// CHtiMailboxHandler::HandleDeleteMailboxL
// Deletes a mailbox.
// ----------------------------------------------------------------------------
void CHtiMailboxHandler::HandleDeleteMailboxL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::HandleDeleteMailboxL" );

    // Delete by name - name should be unique
    TInt nameLength = aData[0];
    if ( nameLength > KEmailAccountNameSize || nameLength == 0 ||
         aData.Length() != ( nameLength + 1 ) )
        {
        User::Leave( KErrArgument );
        }

    TBuf<KEmailAccountNameSize> accountName;
    accountName.Copy( aData.Mid( 1, nameLength ) );
    HTI_LOG_FORMAT( "Searching account with name: %S", &accountName );

    TSmtpAccount smtpAcc;
    User::LeaveIfError( FindSmtpAccount( accountName, smtpAcc ) );
    HTI_LOG_FORMAT( "Found SMTP: %d", smtpAcc.iSmtpAccountId );
    TMsvId relatedAcc = smtpAcc.iRelatedService;

    CEmailAccounts* accounts = CEmailAccounts::NewLC();

    // Delete the SMTP account
    accounts->DeleteSmtpAccountL( smtpAcc );

    // Search for related POP & IMAP acconts and delete if found
    TInt result = KErrNone;
    TPopAccount popAcc;
    TRAP( result, accounts->GetPopAccountL( relatedAcc, popAcc ) );
    if ( result == KErrNone )
        {
        HTI_LOG_FORMAT( "Found related POP: %d", popAcc.iPopAccountId );
        accounts->DeletePopAccountL( popAcc );
        }

    TImapAccount imapAcc;
    TRAP( result, accounts->GetImapAccountL( relatedAcc, imapAcc ) );
    if ( result == KErrNone )
        {
        HTI_LOG_FORMAT( "Found related IMAP: %d", imapAcc.iImapAccountId );
        accounts->DeleteImapAccountL( imapAcc );
        }

    CleanupStack::PopAndDestroy( 1 ); // accounts
    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::HandleDeleteMailboxL" );
    }


// ----------------------------------------------------------------------------
// CHtiMailboxHandler::ParseCreateMsgL
// Parses the parameters from the create account message.
// ----------------------------------------------------------------------------
void CHtiMailboxHandler::ParseCreateMsgL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::ParseCreateMsgL" );
    /*  ACCOUNT SETTINGS
     *   Field                              Size      Values    Mandatory
     *   ---------------------------------------------------------------------
     *   Mailbox type                       1 byte    0,1       YES
     *   Mailbox name length                1 byte    1-30      YES
     *   Mailbox name                       1-30                YES
     *   Incoming mail server length        1 byte    3-50      YES
     *   Incoming mail server               3-50                YES
     *   Incoming access point name length  1 byte    0-255     YES
     *   Incoming access point name         0-255               NO
     *   Incoming user name length          1 byte    0-50      YES
     *   Incoming user name                 0-50                NO
     *   Incoming password length           1 byte    0-50      YES
     *   Incoming password                  0-50                NO
     *   Incoming security                  1 byte    0,1,2     YES
     *   Incoming port                      2 bytes   0-999     YES
     *   A POP secure login                 1 byte    0,1       YES
     *   Outgoing mail server length        1 byte    1-50      YES
     *   Outgoing mail server               1-50                YES
     *   Outgoing access point name length  1 byte    0-255     YES
     *   Outgoing access point name         0-255               NO
     *   Outgoing user name length          1 byte    0-50      YES
     *   Outgoing user name                 0-50                NO
     *   Outgoing password length           1 byte    0-50      YES
     *   Outgoing password                  0-50                NO
     *   Outgoing security                  1 byte    0,1,2     YES
     *   Outgoing port                      2 bytes   0-999     YES
     *   Own mail address length            1 byte    1-100     YES
     *   Own mail address                   1-100               YES
     *   Own name length                    1 byte    0-100     YES
     *   Own name                           0-100               NO
     *   Send message option                1 byte    0,1,2     YES
     *   Copy to own mail address           1 byte    0,1,2,3   YES
     *   Signature length                   2 bytes   0-500     YES
     *   Signature text                     0-500               NO
     *   New mail indicators                1 byte    0,1       YES
     *   Retrieved parts                    1 byte    0,1,2     YES
     *   Retrieve size limit                2 bytes   1-999     YES
     *   Emails to retrieve                 2 bytes   0-999     YES
     *   IMAP4 folder path length           1 byte    0-100     YES
     *   IMAP4 folder path                  0-100               NO
     *   Automatic update                   1 byte    0,1,2     YES
     *   Set as default for outgoing mail   1 byte    0,1       YES
     */

    TInt dataLength = aData.Length();
    HTI_LOG_FORMAT( "Data length = %d", dataLength );
    TInt offset = 0;
    iMailboxType = aData[offset];
    HTI_LOG_FORMAT( "Mailbox type = %d", iMailboxType );
    if ( iMailboxType != 0 && iMailboxType != 1 )
        {
        User::Leave( KErrArgument );
        }
    offset++;

    iMailboxName = ParseStringL( aData, offset );
    offset += iMailboxName->Length() + 1;

    iIncomingServer = ParseStringL( aData, offset );
    offset += iIncomingServer->Length() + 1;


    iIncomingApName = ParseStringL( aData, offset );
    offset += iIncomingApName->Length() + 1;

    iIncomingUserName = ParseString8L( aData, offset );
    offset += iIncomingUserName->Length() + 1;

    iIncomingPassword = ParseString8L( aData, offset );
    offset += iIncomingPassword->Length() + 1;

    // Check that it's safe to read next 4 bytes
    if ( offset + 3 >= dataLength )
        {
        User::Leave( KErrArgument );
        }

    iIncomingSecurity = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Incoming security = %d", iIncomingSecurity );

    iIncomingPort = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "Incoming port = %d", iIncomingPort );

    iAPopSecureLogin = (TBool) aData[offset];
    offset++;
    HTI_LOG_FORMAT( "A POP secure login = %d", iAPopSecureLogin );

    iOutgoingServer = ParseStringL( aData, offset );
    offset += iOutgoingServer->Length() + 1;

    iOutgoingApName = ParseStringL( aData, offset );
    offset += iOutgoingApName->Length() + 1;

    iOutgoingUserName = ParseString8L( aData, offset );
    offset += iOutgoingUserName->Length() + 1;

    iOutgoingPassword = ParseString8L( aData, offset );
    offset += iOutgoingPassword->Length() + 1;

    // Check that it's safe to read next 3 bytes
    if ( offset + 2 >= dataLength )
        {
        User::Leave( KErrArgument );
        }

    iOutgoingSecurity = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Outgoing security = %d", iOutgoingSecurity );

    iOutgoingPort = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "Outgoing port = %d", iOutgoingPort );

    iOwnMailAddress = ParseStringL( aData, offset );
    offset += iOwnMailAddress->Length() + 1;

    iOwnName = ParseStringL( aData, offset );
    offset += iOwnName->Length() + 1;

    // Check that it's safe to read next 2 bytes
    if ( offset + 1 >= dataLength )
        {
        User::Leave( KErrArgument );
        }

    iSendOption = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Send option = %d", iSendOption );

    iSendCopyToOwnAddress = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Send copy = %d", iSendCopyToOwnAddress );

    iSignatureText = ParseStringL( aData, offset, 2 );
    offset += iSignatureText->Length() + 2;

    // Check that it's safe to read next 6 bytes
    if ( offset + 5 >= dataLength )
        {
        User::Leave( KErrArgument );
        }

    iNewMailIndicators = (TBool) aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Indicators = %d", iNewMailIndicators );

    iRetrievedParts = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Retrieved parts = %d", iRetrievedParts );

    iRetrieveSizeLimitKb = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "Size limit = %d", iRetrieveSizeLimitKb );

    iEmailsToRetrieve = aData[offset] + ( aData[offset+1] << 8 );
    offset += 2;
    HTI_LOG_FORMAT( "Emails to retrieve = %d", iEmailsToRetrieve );

    iImapFolderPath = ParseString8L( aData, offset );
    offset += iImapFolderPath->Length() + 1;

    // Check that it's safe to read next 2 bytes
    if ( offset + 1 >= dataLength )
        {
        User::Leave( KErrArgument );
        }

    iAutomaticUpdate = aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Automatic update = %d", iAutomaticUpdate );

    iSetAsDefault = (TBool) aData[offset];
    offset++;
    HTI_LOG_FORMAT( "Set as default = %d", iSetAsDefault );

    HTI_LOG_FORMAT( "Final offset= %d", offset );
    // Check that there's no extra bytes
    if ( offset != dataLength )
        {
        User::Leave( KErrArgument );
        }

    // Validate fields - limits based on S60 3.0 mailbox settings UI
    if ( iMailboxName->Length()      < 1   ||
         iMailboxName->Length()      > 30  ||
         iIncomingServer->Length()   < 3   ||    // a.b
         iIncomingServer->Length()   > 50  ||
         iOutgoingServer->Length()   < 3   ||    // a.b
         iOutgoingServer->Length()   > 50  ||
         iOwnMailAddress->Length()   < 5   ||    // a@b.c
         iOwnMailAddress->Length()   > 100 ||
         iIncomingUserName->Length() > 50 ||
         iIncomingPassword->Length() > 50 ||
         iOutgoingUserName->Length() > 50 ||
         iOutgoingPassword->Length() > 50 ||
         iOwnName->Length()          > 100 ||
         iSignatureText->Length()    > 500 ||
         iImapFolderPath->Length()   > 100 ||
         iIncomingPort               > 999 ||
         iOutgoingPort               > 999 ||
         iRetrieveSizeLimitKb        > 999 ||
         iEmailsToRetrieve           > 999 ||
         iIncomingSecurity           > 2   ||
         iOutgoingSecurity           > 2   ||
         iSendOption                 > 2   ||
         iSendCopyToOwnAddress       > 3   ||
         iRetrievedParts             > 2   ||
         iAutomaticUpdate            > 2      )
        {
        User::Leave( KErrArgument );
        }

    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::ParseCreateMsgL" );
    }


// ----------------------------------------------------------------------------
// CHtiMailboxHandler::ParseStringL
// Helper for parsing one string from the message.
// ----------------------------------------------------------------------------
HBufC* CHtiMailboxHandler::ParseStringL( const TDesC8& aData,
                                         TInt aStrLenOffset,
                                         TInt aStrLenBytes )
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::ParseStringL" );
    if ( aStrLenOffset >= aData.Length() ||
         aStrLenBytes > 2 || aStrLenBytes < 1 )
        {
        User::Leave( KErrArgument );
        }
    TInt strLen = 0;
    if ( aStrLenBytes == 1 )
        {
        strLen = aData[aStrLenOffset];
        }
    else
        {
        strLen = aData[aStrLenOffset] + ( aData[aStrLenOffset +1 ] << 8 );
        }

    if ( aData.Length() <= aStrLenOffset + strLen )
        {
        User::Leave( KErrArgument );
        }
    HTI_LOG_FORMAT( "String length = %d", strLen );
    HBufC* result = HBufC::NewL( strLen );
    result->Des().Copy( aData.Mid( aStrLenOffset + aStrLenBytes, strLen ) );
    HTI_LOG_FORMAT( "Parsed: %S", result );
    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::ParseStringL" );
    return result;
    }


// ----------------------------------------------------------------------------
// CHtiMailboxHandler::ParseString8L
// Helper for parsing one string from the message.
// ----------------------------------------------------------------------------
HBufC8* CHtiMailboxHandler::ParseString8L( const TDesC8& aData,
                                           TInt aStrLenOffset,
                                           TInt aStrLenBytes )
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::ParseString8L" );
    if ( aStrLenOffset >= aData.Length() ||
         aStrLenBytes > 2 || aStrLenBytes < 1  )
        {
        User::Leave( KErrArgument );
        }
    TInt strLen = 0;
    if ( aStrLenBytes == 1 )
        {
        strLen = aData[aStrLenOffset];
        }
    else
        {
        strLen = aData[aStrLenOffset] + ( aData[aStrLenOffset + 1] << 8 );
        }
    if ( aData.Length() <= aStrLenOffset + strLen )
        {
        User::Leave( KErrArgument );
        }
    HTI_LOG_FORMAT( "String length = %d", strLen );
    HBufC8* result = aData.Mid( aStrLenOffset + aStrLenBytes, strLen ).AllocL();
    HTI_LOG_FORMAT( "Parsed: %S", result );
    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::ParseString8L" );
    return result;
    }


// ----------------------------------------------------------------------------
// CHtiMailboxHandler::AccessPointUIDL
// Gets the UID of Access Point named by aApName.
// Returns KErrNotFound if AP not found.
// ----------------------------------------------------------------------------
TInt CHtiMailboxHandler::AccessPointUIDL( const TDesC& aApName )
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::AccessPointUIDL" );
    TInt iapId = KErrNotFound;

    // Search from uncategorised first
    RArray<TUint32> array = RArray<TUint32>();
    iCmManager.ConnectionMethodL( array );
    CleanupClosePushL( array );
    TInt i = 0;
    while ( i < array.Count() && iapId == KErrNotFound )
        {
        RCmConnectionMethod cm = iCmManager.ConnectionMethodL( array[i] );
        CleanupClosePushL( cm );
        HBufC* name = cm.GetStringAttributeL( CMManager::ECmName );
        HTI_LOG_FORMAT( "Found name: %S", name );
        CleanupStack::PushL( name );
        if ( aApName.Compare( *name ) == 0 )
            {
            iapId = cm.GetIntAttributeL( CMManager::ECmId );
            HTI_LOG_FORMAT( "Match: UID = %d", iapId );
            }
        CleanupStack::PopAndDestroy(); // name
        CleanupStack::PopAndDestroy(); // cm
        i++;
        }
    CleanupStack::PopAndDestroy(); // array

    // If not found from uncategorised, search from all destinations
    if ( iapId == KErrNotFound )
        {
        HTI_LOG_TEXT( "Not found from uncategorised" );
        RArray<TUint32> destIdArray = RArray<TUint32>();
        iCmManager.AllDestinationsL( destIdArray );
        CleanupClosePushL( destIdArray );
        i = 0;
        while ( i < destIdArray.Count() && iapId == KErrNotFound )
            {
            RCmDestination dest = iCmManager.DestinationL( destIdArray[i] );
            CleanupClosePushL( dest );
            TInt j = 0;
            while ( j < dest.ConnectionMethodCount() && iapId == KErrNotFound )
                {
                HBufC* name = dest.ConnectionMethodL( j ).GetStringAttributeL(
                        CMManager::ECmName );
                CleanupStack::PushL( name );
                HTI_LOG_FORMAT( "Found name: %S", name );
                if ( aApName.Compare( *name ) == 0 )
                    {
                    iapId = dest.ConnectionMethodL( j ).GetIntAttributeL(
                            CMManager::ECmId );
                    HTI_LOG_FORMAT( "Match: UID = %d", iapId );
                    }
                CleanupStack::PopAndDestroy(); // name
                j++;
                }
            CleanupStack::PopAndDestroy(); // dest
            i++;
            }
        CleanupStack::PopAndDestroy(); // destIdArray
        }

    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::AccessPointUIDL" );
    return iapId;
    }


// ----------------------------------------------------------------------------
// CHtiMailboxHandler::FindSmtpAccount
// Searches for an SMTP account by name.
// Returns KErrNone if found, otherwise KErrNotFound or other error.
// ----------------------------------------------------------------------------
TInt CHtiMailboxHandler::FindSmtpAccount( const TDesC& aAccountName,
                                          TSmtpAccount& aAccount )
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::FindSmtpAccount" );

    TInt err = KErrNone;
    TInt popCount = 0;

    RArray<TSmtpAccount> smtpAccounts;
    CleanupClosePushL( smtpAccounts );
    popCount++; // smtpAccounts

    CEmailAccounts* accounts = NULL;
    TRAP( err, accounts = CEmailAccounts::NewL() );

    if ( err == KErrNone )
        {
        CleanupStack::PushL( accounts );
        popCount++; // accounts
        TRAP( err, accounts->GetSmtpAccountsL( smtpAccounts ) );
        }

    if ( err == KErrNone )
        {
        TInt count = smtpAccounts.Count();
        HTI_LOG_FORMAT( "Found %d SMTP accounts", count );
        err = KErrNotFound;
        for ( TInt i = 0; i < count; i++ )
            {
            aAccount = smtpAccounts[i];
            HTI_LOG_FORMAT( "Account name: %S", &aAccount.iSmtpAccountName );
            if ( aAccount.iSmtpAccountName.Compare( aAccountName ) == 0 )
                {
                HTI_LOG_FORMAT( "Match - MSV ID %d", aAccount.iSmtpService );
                err = KErrNone;
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy( popCount );
    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::FindSmtpAccount" );
    return err;
    }


// ----------------------------------------------------------------------------
// CHtiMailboxHandler::GetDeviceImeiL()
// Gets the device IMEI code and stores it to aResult.
// ----------------------------------------------------------------------------
//
void CHtiMailboxHandler::GetDeviceImeiL( TDes& aResult )
    {
    if ( aResult.MaxLength() < KImeiLength )
        {
        User::Leave( KErrBadDescriptor );
        }
#ifdef __WINS__
    aResult.Copy( KWinsImei );
#else
    RTelServer telServer;
    User::LeaveIfError( telServer.Connect() );
    CleanupClosePushL( telServer );
    User::LeaveIfError( telServer.LoadPhoneModule( KTsyName ) );

    TInt phoneCount = 0;
    User::LeaveIfError( telServer.EnumeratePhones( phoneCount ) );
    if ( phoneCount < 1 )
        {
        User::Leave( KErrNotFound );
        }

    RTelServer::TPhoneInfo phoneInfo;
    User::LeaveIfError( telServer.GetPhoneInfo( 0, phoneInfo ) );
    RMobilePhone mobilePhone;
    User::LeaveIfError( mobilePhone.Open( telServer, phoneInfo.iName ) );
    CleanupClosePushL( mobilePhone );

    RMobilePhone::TMobilePhoneIdentityV1 identity;
    TRequestStatus status;
    mobilePhone.GetPhoneId( status, identity );
    User::WaitForRequest( status );
    User::LeaveIfError( status.Int() );

    aResult.Copy( identity.iSerialNumber );
    CleanupStack::PopAndDestroy( 2 ); // mobilePhone, telServer
#endif
    }


// ----------------------------------------------------------------------------
// CHtiMailboxHandler::ResetVariables
// Clears the instance variables that store the create message parameters.
// ----------------------------------------------------------------------------
void CHtiMailboxHandler::ResetVariables()
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::ResetVariables" );
    delete iMailboxName;
    iMailboxName = NULL;
    delete iIncomingServer;
    iIncomingServer = NULL;
    delete iIncomingApName;
    iIncomingApName = NULL;
    delete iIncomingUserName;
    iIncomingUserName = NULL;
    delete iIncomingPassword;
    iIncomingPassword = NULL;
    delete iOutgoingServer;
    iOutgoingServer = NULL;
    delete iOutgoingApName;
    iOutgoingApName = NULL;
    delete iOutgoingUserName;
    iOutgoingUserName = NULL;
    delete iOutgoingPassword;
    iOutgoingPassword = NULL;
    delete iOwnMailAddress;
    iOwnMailAddress = NULL;
    delete iOwnName;
    iOwnName = NULL;
    delete iSignatureText;
    iSignatureText = NULL;
    delete iImapFolderPath;
    iImapFolderPath = NULL;

    iMailboxType          = KErrUnknown;
    iIncomingSecurity     = KErrUnknown;
    iIncomingPort         = KErrUnknown;
    iOutgoingSecurity     = KErrUnknown;
    iOutgoingPort         = KErrUnknown;
    iSendOption           = KErrUnknown;
    iSendCopyToOwnAddress = KErrUnknown;
    iRetrievedParts       = KErrUnknown;
    iRetrieveSizeLimitKb  = KErrUnknown;
    iEmailsToRetrieve     = KErrUnknown;
    iAutomaticUpdate      = KErrUnknown;

    iAPopSecureLogin   = EFalse;
    iNewMailIndicators = EFalse;
    iSetAsDefault      = EFalse;

    iCmManager.Close();

    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::ResetVariables" );
    }


// ----------------------------------------------------------------------------
// CHtiMailboxHandler::SendOkMsgL
// Helper function for sending response messages.
// ----------------------------------------------------------------------------
void CHtiMailboxHandler::SendOkMsgL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::SendOkMsgL" );
    User::LeaveIfNull( iDispatcher );
    HBufC8* temp = HBufC8::NewL( aData.Length() + 1 );
    TPtr8 response = temp->Des();
    response.Append( ( TChar ) CHtiMessagesServicePlugin::EResultOk );
    response.Append( aData );
    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
        temp, KHtiMessagesServiceUid ) );
    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::SendOkMsgL" );
    }


// ----------------------------------------------------------------------------
// CHtiMailboxHandler::SendErrorMessageL
// Helper function for sending error response messages.
// ----------------------------------------------------------------------------
void CHtiMailboxHandler::SendErrorMessageL( TInt aError, const TDesC8& aDescription )
    {
    HTI_LOG_FUNC_IN( "CHtiMailboxHandler::SendErrorMessageL" );
    User::LeaveIfNull( iDispatcher );
    User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
        aError, aDescription, KHtiMessagesServiceUid ) );
    HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::SendErrorMessageL" );
    }


// ----------------------------------------------------------------------------
// CHtiMailboxHandler::HandleSessionEventL
// From MMsvSessionObserver.
// ----------------------------------------------------------------------------
void CHtiMailboxHandler::HandleSessionEventL( TMsvSessionEvent /*aEvent*/,
                            TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/ )
    {
    // HTI_LOG_FUNC_IN( "CHtiMailboxHandler::HandleSessionEventL" );

    // HTI_LOG_FUNC_OUT( "CHtiMailboxHandler::HandleSessionEventL" );
    }

// End of file
