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

// INCLUDE FILES

// Symbian:
#include <e32base.h>
#include <txtetext.h>
#include <txtrich.h>
#include <s32mem.h>

#include <mmsvattachmentmanager.h>
#include <mmsvattachmentmanagersync.h>
#include <gsmerror.h>                       // KErrGsmOfflineOpNotAllowed
#include <featmgr.h>

#include <mtclreg.h>
#include <smsclnt.h>
#include <smscmds.h>
#include <smuthdr.h>
#include <csmsemailfields.h>
#include <csmsaccount.h>
#include <charconv.h>
#include <smut.h>
#include <cmsvmimeheaders.h>
#include <badesca.h>
#include <e32cmn.h>
#include <gsmuset.h>

// S60
#include <MsgMediaResolver.h>
#include <MsgMediaInfo.h>
#include <MsgTextInfo.h>

#include <mmsgenutils.h>
#include <vcard.h>
#include <SenduiMtmUids.h>
#include <MsgMimeTypes.h>
#include "UniSendingSettings.h"
#include "MessagingVariant.hrh"
#include "msgbiouids.h"
#include "unieditorsmsplugin_p.h"
#include "convergedmessage.h"
#include "convergedmessageid.h"
#include "convergedmessageattachment.h"

#include "MuiuOperationWait.h"
#include "UniEditorGenUtils.h"
#include "unidatamodelloader.h"
#include "unidatamodelplugininterface.h"
#include "msgcontacthandler.h"
#include <hbglobal.h> // for translation support
#include <xqconversions.h>
// resources

// CONSTANTS

// Used to set msg in unparsed state
const TInt KSmsPluginBioMsgUnparsed = 0;

// Description length for sms messages
const TInt KSmsMessageEntryDescriptionAmountOfChars = 60;

//used for descriptor array containing recipients
const TInt KRecipientsArrayGranularity = 8;

// For address information separation (start)
const TUint KMsgSmsAddressStartChar         ('<');

// For address information separation (end)
const TUint KMsgSmsAddressEndChar           ('>');

const TUint KUniSmsStartParenthesis ('(');
const TUint KUniSmsEndParenthesis (')');

// String length for Service centre name
const TInt KUniSmsSCStringLength = 50;

const TUid KUidMsvSMSHeaderStream_COPY_FROM_SMUTHDR_CPP  = {0x10001834};

// Amount of to be converted chars during extracting description
const TInt KSmsEdExtrDescReplaceCharacterCount = 3;

// Unicode char for linefeed regocnised by basic phones
const TUint KSmsEdUnicodeLFSupportedByBasicPhones = 0x000A;

// Copy max this many chars to TMsvEntry iDetails
const TInt  KMaxDetailsLength = 64;

// separators for iDetails field
_LIT( KAddressSeparator, ";" );

// LOCALIZED CONSTANTS
// String to be shown in Drafts view for VCard sms
#define BUSINESS_CARD hbTrId("txt_messaging_list_business_card")

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
UniEditorSmsPluginPrivate* UniEditorSmsPluginPrivate::NewL()
    {
    UniEditorSmsPluginPrivate* self = new ( ELeave ) UniEditorSmsPluginPrivate();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
UniEditorSmsPluginPrivate::~UniEditorSmsPluginPrivate()
{
    if(iGenUtils)
    {
        delete iGenUtils;
        iGenUtils = NULL;
    }
    
    if(iEmailOverSmsC)
    {
        delete iEmailOverSmsC;
        iEmailOverSmsC = NULL;
    }

    if(iRecipients)
    {
        delete iRecipients;
        iRecipients = NULL;
    }

    if(iRichText)
    {
        delete iRichText;
        iRichText = NULL;
    }

    if(iCharFormatLayer)
    {
        delete iCharFormatLayer;
        iCharFormatLayer = NULL;
    }

    if(iParaFormatLayer)
    {
        delete iParaFormatLayer;
        iParaFormatLayer = NULL;
    }

    if(iSmsHeader)
    {
        delete iSmsHeader;
        iSmsHeader = NULL;
    }

    if(iSmsMtm)
    {
        delete iSmsMtm;
        iSmsMtm = NULL;
    }

    if(iMtmRegistry)
    {
        delete iMtmRegistry;
        iMtmRegistry = NULL;
    }

   if(iSession)
    {
        delete iSession;
        iSession = NULL;
    }
}

// -----------------------------------------------------------------------------
// C++ default constructor
// -----------------------------------------------------------------------------
//
UniEditorSmsPluginPrivate::UniEditorSmsPluginPrivate() :
    iSession(0),
    iMtmRegistry(0),
    iSmsMtm(0),
    iSmsHeader(0),
    iParaFormatLayer(0),
    iCharFormatLayer(0),
    iRichText(0),
    iRecipients(0),
    iBioMsg( EFalse ),
    iEmailOverSmsC(0),
    iUnicodeMode( EFalse ),
    iOfflineSupported( EFalse ),
    iCharSupportType(TUniSendingSettings::EUniCharSupportReduced),
    iGenUtils(0)    
    {
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//
void UniEditorSmsPluginPrivate::ConstructL()
    {
	iSession = CMsvSession::OpenSyncL(*this);
    iParaFormatLayer = CParaFormatLayer::NewL();
    iCharFormatLayer = CCharFormatLayer::NewL();
    iRichText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
    iEmailOverSmsC = CSmsNumber::NewL();

    CMsvEntry& entry = SmsMtmL()->Entry();
    entry.SetEntryL( KMsvRootIndexEntryId );

    TSmsUtilities::ServiceIdL( entry, iSmsServiceId );

    FeatureManager::InitializeLibL();
    if ( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
        {
        iOfflineSupported = ETrue;
        }
    else
        {
        iOfflineSupported = EFalse;
        }

    //Turkish SMS-PREQ2265 Specific
    if ( FeatureManager::FeatureSupported( KFeatureIdNltSupport ) )
        {
        iNLTFeatureSupport = ETrue;
        }
    else
        {
        iNLTFeatureSupport = EFalse;
        }

    FeatureManager::UnInitializeLib();
    
    iGenUtils = new UniEditorGenUtils();
    }

// -----------------------------------------------------------------------------
// LoadHeadersL
// @see header
// -----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::LoadHeadersL( CMsvStore* aStore )
    {
    delete iSmsHeader;
    iSmsHeader = NULL;

    if ( aStore && aStore->HasBodyTextL() )
        {
    	aStore->RestoreBodyTextL( *iRichText );
        }

	iSmsHeader = CSmsHeader::NewL( CSmsPDU::ESmsSubmit, *iRichText );

	if ( aStore && aStore->IsPresentL( KUidMsvSMSHeaderStream_COPY_FROM_SMUTHDR_CPP ) )
	    {
	    iSmsHeader->RestoreL( *aStore );
	    }
	else
	    {
        CSmsSettings* settings = CSmsSettings::NewLC();
    	CSmsAccount* account = CSmsAccount::NewLC();
    	account->LoadSettingsL( *settings );
    	CleanupStack::PopAndDestroy( account );
        iSmsHeader->SetSmsSettingsL( *settings );
        CleanupStack::PopAndDestroy( settings );
	    }
    }

// -----------------------------------------------------------------------------
// ConvertFromL
// @see header
// -----------------------------------------------------------------------------
ConvergedMessage* UniEditorSmsPluginPrivate::ConvertFromL(TMsvId aId,
    UniEditorPluginInterface::EditorOperation aOperation)
{
    ConvergedMessageId id(aId);
    ConvergedMessage* msg = new ConvergedMessage(id);
    CleanupStack::PushL(msg);
    DoConvertFromL(aId, msg, aOperation);
    CleanupStack::Pop(msg);
    return msg;
}

// -----------------------------------------------------------------------------
// DoConvertFromL
// @see header
// -----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::DoConvertFromL( TMsvId aId,
        ConvergedMessage* aMessage, 
        UniEditorPluginInterface::EditorOperation aOperation)
{
    // load message for processing
    SmsMtmL()->SwitchCurrentEntryL( aId );
    SmsMtmL()->LoadMessageL();

    aMessage->setMessageType(ConvergedMessage::Sms);

    // get parent entry
    TMsvEntry msvEntry( SmsMtmL()->Entry().Entry() );
    TMsvId parent = msvEntry.Parent();

    if( parent == KMsvDraftEntryIdValue )
    {
        aMessage->setLocation(ConvergedMessage::Draft);
        //TODO: need to give FORWARD etc info in convergedMessage
        aMessage->setDirection(ConvergedMessage::Outgoing);

        //Populate recipients
        populateRecipientsL(aMessage);

        //Populate message body
        populateMessageBodyL(aMessage,msvEntry);

        // read flag for reply-via-same-smsc usecase
        bool replyPath = SmsMtmL()->SmsHeader().Submit().ReplyPath();
        aMessage->setReplyPath(replyPath);
        if(replyPath)
        {
            // save smsc in the message
            QString scaddress = XQConversions::s60DescToQString(
                    SmsMtmL()->SmsHeader().Message().ServiceCenterAddress());
            aMessage->setOriginatingSC(scaddress);
        }
    }
    else if(aOperation == UniEditorPluginInterface::Forward)
    {
        //Populate message body
        populateMessageBodyL(aMessage,msvEntry);            
    }
    else if(aOperation == UniEditorPluginInterface::Reply)
    {
        populateMessageForReplyL(aMessage);
    }
    else if(aOperation == UniEditorPluginInterface::ReplyAll)
    {
        // control should never reach here for sms
        User::Leave(KErrArgument);
    }
    
    //This is required as the switch entry doesnot reset sms headers
    //so if we fwd an inbox msg and then try to save that content to drafts
    //we see some issue
    delete iSmsMtm;
    iSmsMtm = NULL;
    
    return;
}

// -----------------------------------------------------------------------------
// ConvertToL
// @see header
// -----------------------------------------------------------------------------
TMsvId UniEditorSmsPluginPrivate::ConvertToL( ConvergedMessage* message )
{
    // create message in draft folder
    TMsvId id = CreateNativeSmsL();    
    
    int err = KErrNone;
        TRAP(err,            
                // set sms data
                SetSmsDataL( message );
                // set sms settings
                SetSmsSettingsL( message );
                // save all changes for the entry
                SmsMtmL()->SaveMessageL();
        );

    // delete the created drafts entry because error 
    // happened during save
    if (err != KErrNone)
        {
        DeleteDraftsEntryL(id);
        id = -1;
        }

    return id;
}

// -----------------------------------------------------------------------------
// CreateNativeSmsL
// @see header
// -----------------------------------------------------------------------------
TMsvId UniEditorSmsPluginPrivate::CreateNativeSmsL()
{
    CMsvEntry* cEntry = SmsMtmL()->Session().GetEntryL(KMsvDraftEntryId);
    CleanupStack::PushL( cEntry );
    SmsMtmL()->SwitchCurrentEntryL( cEntry->EntryId() );

    TMsvEntry tEntry;
    tEntry.SetAttachment( EFalse );
    tEntry.iMtm = KSenduiMtmSmsUid;
    tEntry.iType = KUidMsvMessageEntry;
    tEntry.iRelatedId = iSmsServiceId;
    tEntry.iServiceId = KMsvLocalServiceIndexEntryId;
    tEntry.iDate.UniversalTime();
    tEntry.SetInPreparation( ETrue );
    tEntry.SetVisible( EFalse );

    cEntry->CreateL(tEntry);
    SmsMtmL()->SwitchCurrentEntryL(tEntry.Id());
    CleanupStack::PopAndDestroy( cEntry );
    return SmsMtmL()->Entry().Entry().Id();
}

// -----------------------------------------------------------------------------
// SetSmsDataL
// @see header
// -----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::SetSmsDataL(ConvergedMessage* message)
{
    // populate recipients, subject and emailfields
    SetSmsHeaderL(message);
    
    // populate the attachments in msg entry
    SetSmsAttachmentsL(message);
}

// -----------------------------------------------------------------------------
// SetSmsHeaderL
// @see header
// -----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::SetSmsHeaderL(ConvergedMessage* message)
{
    TBuf<KMaxDetailsLength> idetailsBuf;
    idetailsBuf.Zero();
    //Save for email over sms
    if ( iRecipients )
    {
        delete iRecipients;
        iRecipients = NULL;
    }
    iRecipients = new ( ELeave ) CDesCArrayFlat( KRecipientsArrayGranularity );

    CSmsEmailFields *emailFields = CSmsEmailFields::NewL();
    CleanupStack::PushL( emailFields );

    ConvergedMessageAddressList addrList = message->toAddressList();
    int addrCount = addrList.count();
    for(int i=0; i<addrCount; i++)
    {
        HBufC* addr = XQConversions::qStringToS60Desc( addrList.at(i)->address() );
        HBufC* alt_alias = XQConversions::qStringToS60Desc( addrList.at(i)->alias() );
        if(addr->Length() > 0)
        {
            CleanupStack::PushL(addr);
            if(alt_alias->Length() > 0)
                CleanupStack::PushL(alt_alias);
            HBufC* pureAddr = TMmsGenUtils::PureAddress( *addr ).AllocLC();
            HBufC* aliasAddr = TMmsGenUtils::Alias( *addr ).AllocLC();
            TInt appendLen = KMaxDetailsLength-idetailsBuf.Length();
            TPtrC appendbuf;
            if ( IsEmailAddress( *pureAddr ) )
            {
                emailFields->AddAddressL( *pureAddr );
                appendbuf.Set(pureAddr->Des().Left(appendLen));
                iRecipients->AppendL( *addr );
            }
            else
            {
                if(aliasAddr->Length() > 0)
                {
                    SmsMtmL()->AddAddresseeL( *pureAddr, *aliasAddr );
                    appendbuf.Set(aliasAddr->Des().Left(appendLen));
                    iRecipients->AppendL(
                            *TMmsGenUtils::GenerateAddressL(*pureAddr, *aliasAddr));
                }
                else
                {
                    if(alt_alias->Length() > 0)
                    {
                        SmsMtmL()->AddAddresseeL( *pureAddr, *alt_alias );
                        appendbuf.Set(alt_alias->Des().Left(appendLen));
                        iRecipients->AppendL(
                                *TMmsGenUtils::GenerateAddressL(*pureAddr, *alt_alias));
                    }
                    else
                    {
                        SmsMtmL()->AddAddresseeL( *pureAddr );
                        appendbuf.Set(pureAddr->Des().Left(appendLen));
                        iRecipients->AppendL( *addr );
                    }
                }
            }
            // copy the append-buffer to form idetails
            if(appendLen > 0)
            {
                idetailsBuf.Append( appendbuf );
            }

            // cleanup
            CleanupStack::PopAndDestroy(2, pureAddr );            
            if(alt_alias->Length() > 0)
            {
                CleanupStack::PopAndDestroy( alt_alias );
            }
            CleanupStack::PopAndDestroy( addr );
            
            // append separator
            if(i != addrCount-1)
            {
                if(KMaxDetailsLength-idetailsBuf.Length() > 0)
                {
                    idetailsBuf.Append( KAddressSeparator() );                
                }
            }
        }
    }
    
    if(idetailsBuf.Length() > 0)
    {
        // set iDetails
        CMsvEntry& entry = SmsMtmL()->Entry();
        TMsvEntry msvEntry( entry.Entry() );
        msvEntry.iDetails.Set( idetailsBuf );
        entry.ChangeL( msvEntry );
        SmsMtmL()->SaveMessageL();
    }

    // set subject
    if ( !message->subject().isEmpty() )
    {
        HBufC* subj = XQConversions::qStringToS60Desc( message->subject() );
        if( subj )
        {
            CleanupStack::PushL( subj );
            emailFields->SetSubjectL( *subj );
            CleanupStack::PopAndDestroy( subj );
        }
    }

    SmsMtmL()->SmsHeader().SetEmailFieldsL( *emailFields );
    CleanupStack::PopAndDestroy( emailFields );
}

// -----------------------------------------------------------------------------
// SetSmsBodyL
// @see header
// -----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::SetSmsBodyL(ConvergedMessage* message)
{
    HBufC8* bodytext = XQConversions::qStringToS60Desc8( message->bodyText() );
    if( bodytext->Length() > 0)
    {
        CleanupStack::PushL( bodytext );
        RDesReadStream stream(*bodytext);
        CleanupClosePushL( stream );

        CPlainText::TImportExportParam param;
        param.iForeignEncoding = KCharacterSetIdentifierUtf8;
        param.iOrganisation = CPlainText::EOrganiseByParagraph;
        CPlainText::TImportExportResult result;

        SmsMtmL()->Body().Reset();
        SmsMtmL()->Body().ImportTextL( 0, stream, param, result );

        CleanupStack::PopAndDestroy(&stream);
        CleanupStack::PopAndDestroy( bodytext );
    }
}

// -----------------------------------------------------------------------------
// SetSmsAttachmentsL
// @see header
// -----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::SetSmsAttachmentsL(ConvergedMessage* message)
    {
    // set body text
    SetSmsBodyL( message );

    // look for attachments
    ConvergedMessageAttachmentList attachmentList = message->attachments();
    int attachmentCount = attachmentList.count();

    ConvergedMessageAttachment* attachment = NULL;
    if( attachmentCount > 1)
        {
        //Programming error in caller code
		// sms can have at-max 1 attachment e.g. vcard, vcal etc
        User::Leave( KErrArgument );
        }
    else if( attachmentCount == 1)
        {
        attachment = attachmentList.at(0);
        }
    // set entry info for drafts viewing
    TMsvEntry tEntry = SmsMtmL()->Entry().Entry();
    CMsvStore* store = SmsMtmL()->Entry().EditStoreL();
    CleanupStack::PushL( store );
    // fill localized strings for smart-msgs e.g. 'Business Card' for vcards
    QString descr;
    if ( attachment )
        {
         // create msv attachment in store
        CreateAttachmentL(store,
                *XQConversions::qStringToS60Desc(attachment->filePath()));
        
        // check for mimetype of the attachment
        TPtrC8 mimetype;
        int attSize;
        QString attMimeType;
        TMsgMediaType attMediaType = EMsgMediaUnknown;
        iGenUtils->getFileInfoL(attachment->filePath(),
                attSize, attMimeType, attMediaType);
        mimetype.Set( *XQConversions::qStringToS60Desc8(attMimeType) );
        
        if ( mimetype.CompareF( KMsgMimeTextPlain ) == 0 )
            {
            iBioMsg=EFalse;
            }
        else if ( mimetype.CompareF(KMsgMimeVCard) == 0 )
            {
            iBioMsg=ETrue;
            tEntry.iBioType = KMsgBioUidVCard.iUid;
            SmsMtmL()->BioTypeChangedL( KMsgBioUidVCard );
            descr = BUSINESS_CARD;
            }
        else if ( mimetype.CompareF(KMsgMimeVCal ) == 0 ||
                  mimetype.CompareF(KMsgMimeICal ) == 0 )
            {
            iBioMsg=ETrue;
            tEntry.iBioType = KMsgBioUidVCalendar.iUid;
            SmsMtmL()->BioTypeChangedL( KMsgBioUidVCalendar );            
            }
        else
            {
            // do not expect any other mimetype
            User::Leave( KErrArgument );
            }
        }
    
    // populate description for this msg (from subject/body content)
    CSmsHeader& header = SmsMtmL()->SmsHeader();
    TBuf<KSmsMessageEntryDescriptionAmountOfChars> buf;
    buf.Zero();
    if(!iBioMsg)
    {
        ExtractDescriptionFromMessageL(
                header.Message(),
                buf,
                KSmsMessageEntryDescriptionAmountOfChars );    
    }
    else if(!descr.isNull())
    {
        buf.Copy( *XQConversions::qStringToS60Desc(descr) );
    }
    tEntry.iDescription.Set( buf );

    // save/commit all changes
    SmsMtmL()->SaveMessageL( *store, tEntry );
    SmsMtmL()->Entry().ChangeL( tEntry );
    store->Commit();
    CleanupStack::PopAndDestroy( store );
    }

// -----------------------------------------------------------------------------
// CreateAttachmentL
// @see header
// -----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::CreateAttachmentL(CMsvStore* aStore, const TDesC& aFilePath)
    {
    MMsvAttachmentManager* attachmentManager = &( aStore->AttachmentManagerL() );
    CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( attachment );

    int attSize;
    QString mimeType;
    TMsgMediaType mediaType = EMsgMediaUnknown;
    QString filepath = XQConversions::s60DescToQString( aFilePath );
    iGenUtils->getFileInfoL(filepath, attSize, mimeType, mediaType);
    
    attachment->SetSize( attSize );
    attachment->SetMimeTypeL( *XQConversions::qStringToS60Desc8(mimeType) );
    
    CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
    attachmentManager->AddAttachmentL( aFilePath, attachment, wait->iStatus );
    wait->Start();
    CleanupStack::PopAndDestroy( wait );
    CleanupStack::Pop( attachment );
    
    aStore->CommitL();
    }

// -----------------------------------------------------------------------------
// SetSmsSettingsL
// @see header
// -----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::SetSmsSettingsL(ConvergedMessage* message)
    {
    CSmsSettings* sendOptions = CSmsSettings::NewL();
    CleanupStack::PushL( sendOptions );

    // "ConvertToL" might be called right after constructor.
    // In this case iSmsHeader is still NULL. Need to initialise.
    if ( !iSmsHeader )
    {
        CMsvStore* store = SmsMtmL()->Entry().EditStoreL();
        CleanupStack::PushL( store );
        LoadHeadersL( store );
        CleanupStack::PopAndDestroy( store );
    }
    iSmsHeader->GetSmsSettingsL( *sendOptions );

    sendOptions->CopyL( *sendOptions );
        
    if ( iBioMsg )
    {
        // make sure bio messages have no conversion
        sendOptions->SetMessageConversion( ESmsConvPIDNone );
    }

    // Update some global SMS settings affecting all messages.
    // These might have changed from the ones retrieved when
    // the message was created and so needs to be updated.
    CSmsSettings* defaultSettings = CSmsSettings::NewLC();

    CSmsAccount* account = CSmsAccount::NewLC();
    account->LoadSettingsL( *defaultSettings );
    CleanupStack::PopAndDestroy( account );

    sendOptions->SetDeliveryReport( defaultSettings->DeliveryReport() );
    sendOptions->SetSmsBearer( defaultSettings->SmsBearer() );
    sendOptions->SetValidityPeriod( defaultSettings->ValidityPeriod() );
    sendOptions->SetReplyPath( message->replyPath() );

    if (defaultSettings->CharacterSet()
            == TSmsDataCodingScheme::ESmsAlphabetUCS2)
    {
        iCharSupportType = TUniSendingSettings::EUniCharSupportFull;
    }
    else
    {
        iCharSupportType = TUniSendingSettings::EUniCharSupportReduced;
    }

    iSmsHeader->SetSmsSettingsL( *sendOptions );

    // Move all the stuff from iSmsHeader::SmsSettings to SmsMtm::SmsHeader::SmsSettings
    SmsMtmL()->SmsHeader( ).SetSmsSettingsL( *sendOptions );

    // if reply-path is present, then lets use it
    if(message->replyPath())
    {
    QString scaddress = message->originatingSC();
    SmsMtmL()->SmsHeader( ).Message( ).
        SetServiceCenterAddressL( *XQConversions::qStringToS60Desc(scaddress) );
    }
    else if(ValidateSCNumberL())
    {    // Or if default SC is existant then only set the default SC
    SmsMtmL()->SmsHeader( ).Message( ).
        SetServiceCenterAddressL( defaultSettings->GetServiceCenter(defaultSettings->DefaultServiceCenter()).Address() );
    }
    CleanupStack::PopAndDestroy(defaultSettings);
    CleanupStack::PopAndDestroy( sendOptions );
    

    //Initialize the settings
    SetEncodingSettingsL( iUnicodeMode, ESmsEncodingNone, 
            iCharSupportType);
    
    //get bodytext
    TInt smslength = SmsMtmL()->Body().LdDocumentLength();
    HBufC* body = HBufC::NewLC(smslength);
    TPtrC ptr;        
    ptr.Set(SmsMtmL()->Body().Read(0,smslength));    
    
    TInt numOfRemainingChars;
    TInt numOfPDUs;
    TBool unicodeMode;
    TSmsEncoding alternativeEncodingType;
    
    //This is to reset the charset type
    GetNumPDUsL(ptr,numOfRemainingChars,numOfPDUs,
            unicodeMode,alternativeEncodingType);
    
    if (iNLTFeatureSupport)
    {
        //Turkish SMS-PREQ2265 Specific
        TSmsEncoding currAlternateEncoding =
                iSmsHeader->Message().Alternative7bitEncoding();
        SmsMtmL()->SmsHeader().Message().SetAlternative7bitEncoding(
            currAlternateEncoding);
    }
    
    //Write the settings into message header
    TSmsUserDataSettings smsSettings;
    CSmsMessage& smsMsg = SmsMtmL()->SmsHeader().Message();

    if (iUnicodeMode)
    {
        smsSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabetUCS2);
    }
    else
    {
        smsSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
    }

    smsSettings.SetTextCompressed(EFalse);

    smsMsg.SetUserDataSettingsL( smsSettings );

    CleanupStack::PopAndDestroy(body);
}

// -----------------------------------------------------------------------------
// SendL
// @see header
// -----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::SendL( TMsvId aId )
{
    SmsMtmL()->SwitchCurrentEntryL( aId );
    SmsMtmL()->LoadMessageL();
    MoveMessagesToOutboxL();
}

// -----------------------------------------------------------------------------
// ValidateServiceL
// @see header
// -----------------------------------------------------------------------------
TBool UniEditorSmsPluginPrivate::ValidateServiceL( TBool aEmailOverSms )
{
    TBool valid = ValidateSCNumberL();

    if ( aEmailOverSms )
    {
        valid = ValidateSCNumberForEmailOverSmsL();
    }

    return valid;
}

// -----------------------------------------------------------------------------
// IsServiceValidL
// @see header
// -----------------------------------------------------------------------------
TBool UniEditorSmsPluginPrivate::IsServiceValidL()
    {
    // Not implemented.
    return ETrue;
    }

// -----------------------------------------------------------------------------
// SmsMtmL
// @see header
// -----------------------------------------------------------------------------
CSmsClientMtm* UniEditorSmsPluginPrivate::SmsMtmL()
    {
    if ( !iSmsMtm )
        {
        if ( !iMtmRegistry )
            {
            iMtmRegistry = CClientMtmRegistry::NewL( *iSession );
            }
        iSmsMtm = static_cast<CSmsClientMtm*>( iMtmRegistry->NewMtmL( KSenduiMtmSmsUid ) );
        }
    return iSmsMtm;
    }

// ----------------------------------------------------------------------------
// MoveMessagesToOutboxL
// @see header
// ----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::MoveMessagesToOutboxL()
    {
    if ( !iRecipients || !iRecipients->Count() )
        {
        User::Leave( KErrGeneral );
        }
    
    // manage attachment and body content for sms
    // e.g. in case of smartmsgs, we will need to populate attachment's
    // content into msg body and discard the attachment
    CMsvStore* store = SmsMtmL()->Entry().EditStoreL();
    CleanupStack::PushL(store);
    MMsvAttachmentManager* attachmentManager = &(store->AttachmentManagerL());
    MMsvAttachmentManagerSync& managerSync =
            store->AttachmentManagerExtensionsL();
    TInt attCount = attachmentManager->AttachmentCount();
    if (attCount > 1)
        {
        //Programming error in caller code
        User::Leave(KErrArgument);
        }
    else if (attCount == 1)
        {
        // get mimetype
        CMsgMediaResolver* mediaResolver = CMsgMediaResolver::NewL();
        CleanupStack::PushL(mediaResolver);
        mediaResolver->SetCharacterSetRecognition(EFalse);
        RFile filehandle = mediaResolver->FileHandleL(
                attachmentManager->GetAttachmentInfoL(0)->FilePath());
        CleanupClosePushL(filehandle);
        TPtrC8 mimetype;
        TDataType datatype;
        mediaResolver->RecognizeL(filehandle, datatype);
        mimetype.Set(datatype.Des8());
        if (mimetype.CompareF(KMsgMimeTextPlain) == 0)
            {
            CreatePlainTextSMSL(filehandle);
            CleanupStack::Pop(&filehandle);
            filehandle.Close();
            managerSync.RemoveAttachmentL(0);
            }
        else if (mimetype.CompareF(KMsgMimeVCard) == 0)
            {
            CreateVCardSMSL(filehandle);
            CleanupStack::Pop(&filehandle);
            filehandle.Close();         
            }
        else if (mimetype.CompareF(KMsgMimeVCal) == 0 || 
                 mimetype.CompareF(KMsgMimeICal) == 0)
            {
            CreateVCalSMSL(filehandle);
            CleanupStack::Pop(&filehandle);
            filehandle.Close();
            managerSync.RemoveAttachmentL(0);
            }
        else
            {
            User::Leave(KErrArgument);
            }
        CleanupStack::PopAndDestroy(mediaResolver);
        }
    // commit changes
    store->CommitL();
    CleanupStack::PopAndDestroy(store);

    //we must create an entry selection for message copies
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );

    CMsvEntry& entry = SmsMtmL()->Entry();
    TMsvEntry msvEntry( entry.Entry() );

    if ( iOfflineSupported && iGenUtils->IsPhoneOfflineL() )
    	{
    	msvEntry.SetSendingState( KMsvSendStateSuspended );
        msvEntry.iError = KErrGsmOfflineOpNotAllowed;
    	}
    else
    	{
    	msvEntry.SetSendingState( KMsvSendStateWaiting );
    	}

    CSmsHeader& header = SmsMtmL()->SmsHeader();
    TBuf<KSmsMessageEntryDescriptionAmountOfChars> buf;

    if (!iBioMsg  )
        {
        ExtractDescriptionFromMessageL(
            header.Message(),
            buf,
            KSmsMessageEntryDescriptionAmountOfChars );
        msvEntry.iDescription.Set( buf );
        }

    CSmsNumber* rcpt = CSmsNumber::NewL();
    CleanupStack::PushL( rcpt );

    TPtrC name;
    TPtrC address;
    TBool cantExit = ETrue;
    while ( cantExit )
        {
        HBufC* addressBuf = NULL;
        TPtr addressPtr( 0, 0);

        NameAndAddress( iRecipients->MdcaPoint(0) , name, address );

        // set To-field stuff into the Details of the entry
        if ( name.Length() )
            {
            msvEntry.iDetails.Set( name );
            }
        else
            {
            // Internal data structures always holds the address data in western format.
            // UI is responsible of doing language specific conversions.
            addressBuf = HBufC::NewLC( address.Length() );
            addressPtr.Set( addressBuf->Des() );
            addressPtr.Copy( address );

            iGenUtils->ConvertDigitsTo( addressPtr, EDigitTypeWestern );
            msvEntry.iDetails.Set( addressPtr );
            }
        TMsvId copyId;

        if ( iRecipients->Count() == 1 )
            {
            //Note that we came here also in case of many recipients. ...eventually.

            if ( IsEmailAddress( address ) )
                {
                FillEmailInformationDataL( header, address );
                //Let's remove the recipient and replace it with Email over SMS gateway address
                //But let's first cehck if it exists
                if( SmsMtmL( )->AddresseeList().Count() )
                	{
                    SmsMtmL( )->RemoveAddressee( 0 );
                	}
                SmsMtmL()->AddAddresseeL(
	            iEmailOverSmsC->Address( ) ,
		        KNullDesC( ) );
                }
            else
                {
                InsertSubjectL( header, SmsMtmL()->Body() );
                }

            entry.ChangeL( msvEntry );
            SmsMtmL()->SaveMessageL();
            // Move it
            copyId = MoveMessageEntryL( KMsvGlobalOutBoxIndexEntryId );
            cantExit = EFalse;
            }
        else
            {// Many recipients in array

            // Own copy function for Emails
            // This is because EmailOverSms messages can
            // contain adresses longer than 21 digits
            if ( IsEmailAddress( address ) )
                {
                copyId = CreateMessageInOutboxL(
                msvEntry, address );

                }
            else // For MSISDN's
                {
                rcpt->SetAddressL( address );
                if ( name.Length() )
                    { // add name only if we have alias
                    rcpt->SetNameL( name );
                    }

                copyId = CreateMessageInOutboxL(
                    msvEntry, *rcpt, SmsMtmL()->Body());

                SmsMtmL()->RemoveAddressee( 0 );
                }
            //If hundreds of recipient, make sure viewserver
            //timers are reseted
            if ( iRecipients->Count() > 100 && ( iRecipients->Count() ) % 30 == 0 )
                {
                User::ResetInactivityTime();
                }

            iRecipients->Delete(0);
            }
        if ( addressBuf )
            {
            CleanupStack::PopAndDestroy( addressBuf );
            }

        // let's add the entry id into the cmsventryselection
        selection->AppendL( copyId );
        }
    CleanupStack::PopAndDestroy( rcpt );

    //Let's free some memory
    if ( iRecipients )
        {
        delete iRecipients;
        iRecipients = NULL;
        }

    SetScheduledSendingStateL( selection );
    CleanupStack::PopAndDestroy( selection );
    }


// ----------------------------------------------------------------------------
// MoveMessageEntryL
// @see header
// ----------------------------------------------------------------------------
TMsvId UniEditorSmsPluginPrivate::MoveMessageEntryL( TMsvId aTarget )
    {
    TMsvEntry msvEntry( SmsMtmL()->Entry().Entry() );
    TMsvId id = msvEntry.Id();

    if ( msvEntry.Parent() != aTarget )
        {
        TMsvSelectionOrdering sort;
        sort.SetShowInvisibleEntries( ETrue );
        CMsvEntry* parentEntry= CMsvEntry::NewL( *iSession, msvEntry.Parent(), sort );
        CleanupStack::PushL( parentEntry );

        // Copy original from the parent to the new location
        CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();

        CMsvOperation* op = parentEntry->MoveL(
            msvEntry.Id(),
            aTarget,
            wait->iStatus );

        CleanupStack::PushL( op );
        wait->Start();
        TMsvLocalOperationProgress prog = McliUtils::GetLocalProgressL( *op );
        User::LeaveIfError( prog.iError );

        id = prog.iId;

        CleanupStack::PopAndDestroy( op );
        CleanupStack::PopAndDestroy( wait );
        CleanupStack::PopAndDestroy( parentEntry );
        }

    return id;
    }

// ----------------------------------------------------------------------------
// CreateMessageInOutboxL
// Use this function for non-email messages
// @see header
// ----------------------------------------------------------------------------
TMsvId UniEditorSmsPluginPrivate::CreateMessageInOutboxL(
    const TMsvEntry& aEntry,
    const CSmsNumber& aRecipient,
    const CRichText& aBody )
    {
    // Initialize the richtext object
    CRichText* richText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
    CleanupStack::PushL( richText );

    // Initialise header and store
    CSmsHeader* header = CSmsHeader::NewL( CSmsPDU::ESmsSubmit, *richText );
    CleanupStack::PushL( header );
    CMsvStore* sourceStore = SmsMtmL()->Entry().ReadStoreL();
    CleanupStack::PushL( sourceStore );

    // Read store
    header->RestoreL( *sourceStore );

    // Initialise number with parameters
    CSmsNumber* rcpt = CSmsNumber::NewL( aRecipient );
    CleanupStack::PushL( rcpt );
    header->Recipients().ResetAndDestroy();
    header->Recipients().AppendL( rcpt );
    CleanupStack::Pop( rcpt );

    // Create entry to Outbox
    TMsvEntry entry( aEntry );
    entry.iMtmData3 = KSmsPluginBioMsgUnparsed;
    CMsvEntry* outbox = iSession->GetEntryL( KMsvGlobalOutBoxIndexEntryId );
    CleanupStack::PushL( outbox );
    outbox->CreateL( entry );
    iSession->CleanupEntryPushL( entry.Id() );
    outbox->SetEntryL( entry.Id());

    //Initialize target store
    CMsvStore* targetStore;
    targetStore = outbox->EditStoreL();
    CleanupStack::PushL( targetStore );

    //Add attachment
    MMsvAttachmentManager& attaManager = sourceStore->AttachmentManagerL();
    RFile tmpFile;

    //Check if attachment exists and add it
    if( sourceStore->AttachmentManagerL().AttachmentCount() )
    	{
        tmpFile = attaManager.GetAttachmentFileL( 0 );
    	CleanupClosePushL( tmpFile );

    	MMsvAttachmentManager& targetAttaMan = targetStore->AttachmentManagerL();
		CMsvAttachment* targetAtta = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
		CleanupStack::PushL( targetAtta );

        CMuiuOperationWait* waiter = CMuiuOperationWait::NewLC();
		targetAttaMan.AddAttachmentL( tmpFile, targetAtta, waiter->iStatus );
		waiter->Start();

     	CleanupStack::PopAndDestroy( waiter ); //waiter
		CleanupStack::Pop(targetAtta );// targetAtta
		CleanupStack::Pop( &tmpFile );//tmpFile
    	}

    TInt totalLength( aBody.DocumentLength() );
    HBufC* bodyText = HBufC::NewLC ( totalLength );
    TPtr bodyTextPtr ( bodyText->Des() );

    aBody.Extract( bodyTextPtr, 0, totalLength );
    richText->InsertL( 0, bodyTextPtr );
    CleanupStack::PopAndDestroy( bodyText );

    InsertSubjectL( *header, *richText );

    targetStore->StoreBodyTextL( *richText );

    header->StoreL( *targetStore );
    targetStore->CommitL();

    // Usually SMCM takes care of updating iSize, but now when msg is
    // created to Outbox for several recipients this has to be done manually.
    entry.iSize = targetStore->SizeL();
    entry.iRelatedId = iSmsServiceId;
    entry.iServiceId = KMsvLocalServiceIndexEntryId;
    outbox->ChangeL( entry );
    CleanupStack::PopAndDestroy( targetStore );

    iSession->CleanupEntryPop();
    CleanupStack::PopAndDestroy( outbox );
    CleanupStack::PopAndDestroy( sourceStore );
    CleanupStack::PopAndDestroy( header );
    CleanupStack::PopAndDestroy( richText );
    return entry.Id();
    }

// ---------------------------------------------------------
// CMsgSmsEditorAppUi::CreateMessageInOutboxL
// Creates message in outbox in case of multiple recipients
// with some e-mail over SMS addresses
// ---------------------------------------------------------
TMsvId UniEditorSmsPluginPrivate::CreateMessageInOutboxL(
    const TMsvEntry& aEntry,
    const TDesC& aAddress )
    {
    CRichText* richText = CRichText::NewL( iParaFormatLayer, iCharFormatLayer );
    CleanupStack::PushL( richText );
    // Initialise header and store
    CSmsHeader* header = CSmsHeader::NewL( CSmsPDU::ESmsSubmit, *richText );
    CleanupStack::PushL( header );
    CMsvStore* store = SmsMtmL()->Entry().ReadStoreL();

    CleanupStack::PushL( store );
    // Read store
    header->RestoreL( *store );
    CleanupStack::PopAndDestroy( store );
    // Initialise number
    CSmsNumber* rcpt = CSmsNumber::NewL();
    CleanupStack::PushL( rcpt );
    header->Recipients().ResetAndDestroy();
    // Save Email specific information in header
    FillEmailInformationDataL( *header, aAddress );
    // Fill the recipient data for Email
    // Address = Email gateway
    // Alias = The real address
    rcpt->SetAddressL( iEmailOverSmsC->Address()  );
    rcpt->SetNameL( aAddress ); // This takes only 21 chars

    header->Recipients().AppendL( rcpt );
    CleanupStack::Pop( rcpt );
    // Create entry to Outbox
    TMsvEntry entry( aEntry );
    entry.iMtmData3 = KSmsPluginBioMsgUnparsed;

    CMsvEntry* outbox = iSession->GetEntryL( KMsvGlobalOutBoxIndexEntryId );
    CleanupStack::PushL( outbox );
    outbox->CreateL( entry );
    iSession->CleanupEntryPushL( entry.Id());
    outbox->SetEntryL( entry.Id());
    // Save
    store = outbox->EditStoreL();
    CleanupStack::PushL( store );
    header->StoreL( *store );

    richText->Reset();
    richText->InsertL( 0 , SmsMtmL()->Body().Read( 0 ) );

    store->StoreBodyTextL( *richText );
    store->CommitL();
    // Usually SMCM takes care of updating iSize, but now when msg is
    // created to Outbox for several recipients this has to be done manually.
    entry.iSize = store->SizeL();
    entry.iRelatedId = iSmsServiceId;
    entry.iServiceId = KMsvLocalServiceIndexEntryId;
    outbox->ChangeL( entry );

    CleanupStack::PopAndDestroy( store );
    iSession->CleanupEntryPop();
    CleanupStack::PopAndDestroy( outbox );
    CleanupStack::PopAndDestroy( header );
    CleanupStack::PopAndDestroy( richText );
    return entry.Id();
    }

// ----------------------------------------------------------------------------
// SetScheduledSendingStateL
// @see header
// ----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::SetScheduledSendingStateL( CMsvEntrySelection* aSelection )
    {
    const TMsvEntry msvEntry = SmsMtmL()->Entry().Entry();
    if ( msvEntry.SendingState() == KMsvSendStateWaiting )
        {
        // Add entry to task scheduler
        TBuf8<1> dummyParams;

        CMuiuOperationWait* waiter = CMuiuOperationWait::NewLC();
        waiter->iStatus = KRequestPending;

        CMsvOperation* op= SmsMtmL()->InvokeAsyncFunctionL(
            ESmsMtmCommandScheduleCopy,
            *aSelection,
            dummyParams,
            waiter->iStatus );
        CleanupStack::PushL( op );
        waiter->Start();

        CleanupStack::PopAndDestroy( op );
        CleanupStack::PopAndDestroy( waiter );
        }
    }

// ----------------------------------------------------------------------------
// NameAndAddress
// @see header
// ----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::NameAndAddress( const TDesC& aMsvAddress, TPtrC& aName, TPtrC& aAddress )
    {
    TInt addressStart = aMsvAddress.LocateReverse( KMsgSmsAddressStartChar );
    TInt addressEnd = aMsvAddress.LocateReverse( KMsgSmsAddressEndChar );

    if ( addressStart != KErrNotFound && addressEnd != KErrNotFound
        && addressEnd > addressStart )
        {
        // verified address, will be used as selected from contacts manager
        aName.Set( aMsvAddress.Ptr(), addressStart );
        aAddress.Set(
            aMsvAddress.Mid( addressStart + 1 ).Ptr(),
            ( addressEnd - addressStart ) -1 );
        if ( !aAddress.Length())
            {
            aAddress.Set( aName );
            aName.Set( KNullDesC ); // empty string
            }
        }
    else
        {
        // unverified string, will be used as entered in the header field
        aName.Set( KNullDesC ); // empty string
        aAddress.Set( aMsvAddress.Ptr(), aMsvAddress.Length() ); // a whole string to address
        }

    if ( aName.CompareF( aAddress ) == 0 )
        {
        aName.Set( KNullDesC ); // empty string
        }
    }

// ----------------------------------------------------------------------------
// UniEditorSmsPluginPrivate::ExtractDescriptionFromMessageL
// @see header
// ----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::ExtractDescriptionFromMessageL(
    const CSmsMessage& aMessage,
    TDes& aDescription,
    TInt aMaxLength)
    {
    // subject was already put in emailfields
    HBufC* subject = SmsMtmL()->SmsHeader().EmailFields().Subject().AllocL();
    if ( subject->Length() > 0 )
        {
        aDescription = subject->Des().Left(aMaxLength);
        }
    else
        {// Extract from message body
        aMessage.Buffer().Extract(
            aDescription,
            0,
            Min(
                aMaxLength,
                aMessage.Buffer().Length()));
        }

    //replace paragraphs with spaces.
    TBuf<KSmsEdExtrDescReplaceCharacterCount> replaceChars;
    replaceChars.Zero();
    replaceChars.Append( CEditableText::EParagraphDelimiter );
    replaceChars.Append( KSmsEdUnicodeLFSupportedByBasicPhones );
    replaceChars.Append( CEditableText::ELineBreak );
    iGenUtils->ReplaceCharacters( aDescription, replaceChars, CEditableText::ESpace );
    aDescription.Trim();
    }

// ----------------------------------------------------------------------------
// CreatePlainTextSMSL
// @see header
// ----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::CreatePlainTextSMSL( RFile& aFile)
    {
    RFileReadStream stream( aFile );
    CleanupClosePushL( stream );

    CPlainText::TImportExportParam param;
    param.iForeignEncoding = KCharacterSetIdentifierUtf8;
    param.iOrganisation = CPlainText::EOrganiseByParagraph;

    CPlainText::TImportExportResult result;

    SmsMtmL()->Body().ImportTextL( 0, stream, param, result );

    CleanupStack::PopAndDestroy( &stream );
    }

// ----------------------------------------------------------------------------
// InsertSubjectL
// Insert subject for non email addresses into the body
// ----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::InsertSubjectL( CSmsHeader& aHeader, CRichText& aText  )
    {
    // subject was already put in emailfields
    HBufC* subject = SmsMtmL()->SmsHeader().EmailFields().Subject().AllocL();
    if ( ( subject->Length() > 0 ) && !iBioMsg )
       {
        CleanupStack::PushL(subject);
        TInt writePosition = subject->Length()+2;//+2 for the parentesis

        if ( subject->Locate( KUniSmsStartParenthesis )!= KErrNotFound ||
            subject->Locate( KUniSmsEndParenthesis ) != KErrNotFound)
            {
            HBufC* modifiableSubject = subject->Alloc();
            CleanupStack::PushL(modifiableSubject);
            TPtr ptr = modifiableSubject->Des();

            TBuf<1> replaceChars;
            replaceChars.Zero();
            replaceChars.Append( KUniSmsStartParenthesis );
            // Replace '(' chars with '<'
            iGenUtils->ReplaceCharacters( ptr, replaceChars, TChar('<') );

            replaceChars.Zero();
            replaceChars.Append( KUniSmsEndParenthesis );
            // Replace ')' chars with '>'
            iGenUtils->ReplaceCharacters( ptr, replaceChars, TChar('>') );

            aText.InsertL( 0, KUniSmsStartParenthesis );
            aText.InsertL( 1, ptr );
            aText.InsertL( writePosition-1, KUniSmsEndParenthesis );
            CleanupStack::PopAndDestroy( modifiableSubject );
            }
        else
            {
            aText.InsertL( 0, KUniSmsStartParenthesis );
            aText.InsertL( 1, *subject );
            aText.InsertL( writePosition-1, KUniSmsEndParenthesis );
            }

         CleanupStack::PopAndDestroy( subject ); 

        }

    // Clears the CSmsHeaders EmailFields for non Email addresses
    CSmsEmailFields* emailFields = CSmsEmailFields::NewL();
    CleanupStack::PushL( emailFields );
    aHeader.SetEmailFieldsL( *emailFields );
    CleanupStack::PopAndDestroy( emailFields );
    }

// ----------------------------------------------------------------------------
// CreateVCardSMS
// @see header
// ----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::CreateVCardSMSL( RFile& aFile )
    {
    TInt fileSize = 0;
    TInt err ( aFile.Size( fileSize ) );
    User::LeaveIfError(err);

    // Create two buffers: 8-bit for reading from file and 16-bit for
    // converting to 16-bit format
    HBufC8* buf8 = HBufC8::NewLC( fileSize );
    TPtr8 ptr8 = buf8->Des();
    HBufC16* buf16 = HBufC16::NewLC( fileSize );
    TPtr16 ptr16 = buf16->Des();

    for (TInt err = aFile.Read(ptr8);
        ptr8.Length() > 0;
        err = aFile.Read(ptr8))
        {
        User::LeaveIfError(err);
        ptr16.Copy(ptr8);
        SmsMtmL()->Body().InsertL(SmsMtmL()->Body().DocumentLength(), ptr16);
        }

    // Cleanup and return
    CleanupStack::PopAndDestroy( buf16 );
    CleanupStack::PopAndDestroy( buf8 );
    }

// ----------------------------------------------------------------------------
// CreateVCalSMS
// @see header
// ----------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::CreateVCalSMSL( RFile& aFile )
    {
    TInt err (KErrNone);
    TInt fileSize = 0;
    err = aFile.Size( fileSize );
    User::LeaveIfError(err);

    HBufC8* buf8 = HBufC8::NewLC( fileSize );
    TPtr8 ptr8 = buf8->Des();

    err = aFile.Read( ptr8 );
    User::LeaveIfError(err);

    HBufC16* buf16 = HBufC16::NewLC( fileSize );
    TPtr16 ptr16 = buf16->Des();

    ptr16.Copy(ptr8);
    SmsMtmL()->Body().InsertL(0, ptr16);

    CleanupStack::PopAndDestroy( buf16 );
    CleanupStack::PopAndDestroy( buf8 );
    }

// ----------------------------------------------------------------------------
// UniEditorSmsPluginPrivate::ValidateSCNumberL
// @see header
// ----------------------------------------------------------------------------
TBool UniEditorSmsPluginPrivate::ValidateSCNumberL()
    {
    TBool valid( EFalse );
    CSmsAccount* smsAccount = CSmsAccount::NewLC();
    CSmsSettings* settings = CSmsSettings::NewLC();
    
    smsAccount->LoadSettingsL(*settings);

    int serviceCenterCount = settings->ServiceCenterCount();
    
    if( serviceCenterCount > 0 )
    {
      TInt defaultIndex = settings->DefaultServiceCenter();
      if( settings->GetServiceCenter(defaultIndex).Address().Length() > 0 )
      {
          valid = ETrue;    
      }
    }
    else if(settings->ReplyPath())    
    {
        valid = ETrue;
    }
    
    CleanupStack::PopAndDestroy(2);
    return valid;
    }

// ---------------------------------------------------------
// UniEditorSmsPluginPrivate::ValidateSCNumberForEmailOverSmsL
// @see header
// ---------------------------------------------------------
TBool UniEditorSmsPluginPrivate::ValidateSCNumberForEmailOverSmsL()
    {
    TBool confNeeded( EFalse );
    // Read the email settings
    TBuf<KUniSmsSCStringLength> emailSmscNumber;
    TBuf<KUniSmsSCStringLength> emailGateWayNumber;
    TBool notUsed( EFalse );
    // The file may not exist
    TInt readResult = iGenUtils->ReadEmailOverSmsSettingsL(
                        emailSmscNumber,
                        emailGateWayNumber,
                        notUsed );
    if ( KErrNone == readResult )
        {
        // Check that both have valid values
        // In any otther case we need to show the conf pop-up window
        if ( emailSmscNumber != KNullDesC && emailGateWayNumber != KNullDesC)
            {
            confNeeded = ETrue;
            }
        }

	return confNeeded;
    }

// ---------------------------------------------------------
// UniEditorSmsPluginPrivate::FillEmailInformationDataL
// @see header
// ---------------------------------------------------------
void UniEditorSmsPluginPrivate::FillEmailInformationDataL(
    CSmsHeader& aHeader,
    const TPtrC& aAddress )
    {
    CSmsEmailFields* emailFields = CSmsEmailFields::NewL();
    CleanupStack::PushL( emailFields );

    // The Email SMSC may differ from sms SMSC
    aHeader.Message().SetServiceCenterAddressL( iEmailOverSmsC->Name() );

    // Check if there is need to save as EmailFieds with header
    if ( aAddress.Length() )
        {
        // Set the address
        emailFields->AddAddressL( aAddress );
        }

    // subject was already put in emailfields
    HBufC* subject = SmsMtmL()->SmsHeader().EmailFields().Subject().AllocL();
    if ( subject->Length() > 0 )
        { // Handle the subject
        CleanupStack::PushL( subject );
        TPtr text = subject->Des();

        TBuf<1> replaceChars;
        replaceChars.Zero();
        replaceChars.Append( KUniSmsStartParenthesis );
        // Replace '(' chars with '<'
        iGenUtils->ReplaceCharacters( text, replaceChars, TChar('<') );

        replaceChars.Zero();
        replaceChars.Append( KUniSmsEndParenthesis );
        // Replace ')' chars with '>'
        iGenUtils->ReplaceCharacters( text, replaceChars, TChar('>') );

        // For Emails save it to CSmsEmailFields
        emailFields->SetSubjectL( text );
        CleanupStack::PopAndDestroy( subject );
        }

    aHeader.SetEmailFieldsL( *emailFields );
    CleanupStack::PopAndDestroy( emailFields );
    }



// ----------------------------------------------------
//  UniEditorSmsPluginPrivate::IsEmailAddress()
// @see header
// ----------------------------------------------------
TBool UniEditorSmsPluginPrivate::IsEmailAddress( const TPtrC& aAddress ) const
    {
    TBool isEmailAddress( EFalse );
    if (  aAddress.Locate('@')  != KErrNotFound)
        {
        isEmailAddress = ETrue;
        }
    return isEmailAddress;
    }

//------------------------------------------------------------------------------
// UniEditorSmsPluginPrivate::SetEncodingSetings
// Turkish SMS-PREQ2265 Specific
// To Set encoding settings like encoding type, character support
// and alternative encoding if any
//
// IMPORTANT NOTE:
// This function is usually called from CV and UE to reset/set alternative
// encoding or char support
// when corresponding feilds change. Hence aUnicodeMode is always set to false
//------------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::SetEncodingSettingsL( TBool aUnicodeMode,
    TSmsEncoding aAlternativeEncodingType, 
    TInt aCharSupportType)
    {
    
    if ( !iSmsHeader )
    {
        iSmsHeader = CSmsHeader::NewL( CSmsPDU::ESmsSubmit, *iRichText );

        CSmsSettings* settings = CSmsSettings::NewLC();
        CSmsAccount* account = CSmsAccount::NewLC();
        account->LoadSettingsL( *settings );
        CleanupStack::PopAndDestroy( account );
        iSmsHeader->SetSmsSettingsL( *settings );
        CleanupStack::PopAndDestroy( settings );    
    }

    TSmsUserDataSettings smsSettings;
    CSmsMessage& smsMsg = iSmsHeader->Message();

    iUnicodeMode = aUnicodeMode;
    iCharSupportType = aCharSupportType; 
    iAlternativeEncodingType = aAlternativeEncodingType;

    if (iUnicodeMode)
    {
        smsSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabetUCS2);
    }
    else
    {
        smsSettings.SetAlphabet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
    }

    smsSettings.SetTextCompressed(EFalse);

    smsMsg.SetUserDataSettingsL( smsSettings );
    //First try without any alternate encoding
    smsMsg.SetAlternative7bitEncoding( aAlternativeEncodingType );
    }

//------------------------------------------------------------------------------
// UniEditorSmsPluginPrivate::GetNumPDUs
// Turkish SMS-PREQ2265 Specific
// To get PDU Info: extracts details of number of PDUs, number of remaining
// chars in last PDU
// and encoding types used.
//------------------------------------------------------------------------------
void UniEditorSmsPluginPrivate::GetNumPDUsL(
        TDesC& aBuf,
        TInt& aNumOfRemainingChars,
        TInt& aNumOfPDUs,
        TBool& aUnicodeMode,
        TSmsEncoding & aAlternativeEncodingType )
    {
    TInt numOfUnconvChars, numOfDowngradedChars, isAltEncSupported;
    TSmsEncoding currentAlternativeEncodingType;
    
    CSmsMessage& smsMsg = iSmsHeader->Message();

    // need to set the input buffer to SMS buffer through iRichText(which is reference to SMS Buffer object)
    iRichText->Reset();
    iRichText->InsertL(0, aBuf);

    //call SMS stack API to get PDU info
    smsMsg.GetEncodingInfoL( aNumOfPDUs, numOfUnconvChars, numOfDowngradedChars, aNumOfRemainingChars );

    //Algo to switch to Unicode if required
    while( (numOfUnconvChars || numOfDowngradedChars) && !iUnicodeMode )
        {
        currentAlternativeEncodingType = smsMsg.Alternative7bitEncoding();
        if( currentAlternativeEncodingType != iAlternativeEncodingType )
            {
            //try with this new alternative encoding type
            isAltEncSupported = smsMsg.SetAlternative7bitEncoding( iAlternativeEncodingType );
            if( isAltEncSupported == KErrNotSupported )
                {
                // if required alternative encoding plugin is not supported, retain the existing encoding mechanism.
                iAlternativeEncodingType = currentAlternativeEncodingType;
                continue;
                }
            }
        else if( numOfUnconvChars || iCharSupportType == TUniSendingSettings::EUniCharSupportFull)
            {
            //switch to Unicode
            //iUnicodeMode = ETrue;
            SetEncodingSettingsL( ETrue, iAlternativeEncodingType, TUniSendingSettings::EUniCharSupportFull);
            }
        else
            {
            //Get out of while loop and return the results
            break;
            }
        //get the PDU info with new settings
        iRichText->Reset();
        iRichText->InsertL(0, aBuf);
        smsMsg.GetEncodingInfoL( aNumOfPDUs, numOfUnconvChars, numOfDowngradedChars, aNumOfRemainingChars );
        }

    /*
     * Enable the below code to debug if something wrong with characters sent even in unicode mode
     */

    aUnicodeMode = iUnicodeMode;
    aAlternativeEncodingType = iAlternativeEncodingType;
    if(iUnicodeMode)
        {
        //In case of Unicode mode, SMS Stack returns number of available free User Data units.
        //Need to convert them w.r.t characters(each char takse 2 UD units).
        aNumOfRemainingChars = aNumOfRemainingChars/2;
        }
    }

// ----------------------------------------------------
//  UniEditorSmsPluginPrivate::DeleteDraftsEntryL()
// @see header
// ----------------------------------------------------
void UniEditorSmsPluginPrivate::DeleteDraftsEntryL( TMsvId aId )
    {
    CMsvEntry* pEntry = iSession->GetEntryL(KMsvDraftEntryIdValue);
    CleanupStack::PushL(pEntry);
    pEntry->DeleteL( aId );
    CleanupStack::PopAndDestroy(pEntry);
    }

// ----------------------------------------------------
//  UniEditorSmsPluginPrivate::populateRecipientsL()
// @see header
// ----------------------------------------------------
void UniEditorSmsPluginPrivate::populateRecipientsL(
    ConvergedMessage *aMessage)
{
    TPtrC name;
    TPtrC address;

    const CSmsEmailFields& emailFields = SmsMtmL( )->SmsHeader( ).EmailFields();
    if( emailFields.HasAddress( ))
        { // If email address set -> copy them here
        const MDesCArray& emailRecipients = emailFields.Addresses();
        for( TInt id = 0; id < emailRecipients.MdcaCount( ); id++ )
            {
            HBufC* pureAddr = TMmsGenUtils::PureAddress( emailRecipients.MdcaPoint( id ) ).AllocLC();
            HBufC* aliasAddr = TMmsGenUtils::Alias( emailRecipients.MdcaPoint( id ) ).AllocLC();
            ConvergedMessageAddress messageAddress(
                    XQConversions::s60DescToQString(*pureAddr), 
                    XQConversions::s60DescToQString(*aliasAddr));
            CleanupStack::PopAndDestroy(2, pureAddr );
            aMessage->addToRecipient(messageAddress);
            }
        }

    // Copy non-email over sms addresses if needed
    const CMsvRecipientList& smsRecipients = SmsMtmL()->AddresseeList();
    int smsRecCount = smsRecipients.Count();
    for (int i = 0; i < smsRecCount; i++)
        { // Go thru all the recipients
        if( !emailFields.HasAddress( ) )
            { // and copy them only if email addresses did not exist
            HBufC* pureAddr =
                    TMmsGenUtils::PureAddress(smsRecipients[i]).AllocLC();
            HBufC* aliasAddr =
                    TMmsGenUtils::Alias(smsRecipients[i]).AllocLC();
            ConvergedMessageAddress messageAddress(
                    XQConversions::s60DescToQString(*pureAddr), 
                    XQConversions::s60DescToQString(*aliasAddr));
            CleanupStack::PopAndDestroy(2, pureAddr );
            aMessage->addToRecipient(messageAddress);
            }
        }

    if( emailFields.Subject( ).Length( ) )
        { // If email subject exists -> copy it
        aMessage->setSubject(XQConversions::s60DescToQString(
                emailFields.Subject()));
        }
}

// ----------------------------------------------------
// UniEditorSmsPluginPrivate::populateMessageBodyL()
// @see header
// ----------------------------------------------------
void UniEditorSmsPluginPrivate::populateMessageBodyL(
    ConvergedMessage* aMessage,
    TMsvEntry &aEntry)
{
    if (  aEntry.iBioType == KMsgBioUidVCard.iUid
            || aEntry.iBioType == KMsgBioUidVCalendar.iUid )
    {
        //In case of drafts the vcal is present inside msg store and
        //can be read directly
        if(aEntry.Parent() == KMsvDraftEntryIdValue)
        { 
            //In case of drafts the vcf/vcs file is stored inside store
            //so we can directly extract it
            CMsvStore* store = SmsMtmL()->Entry().EditStoreL();
            CleanupStack::PushL( store );
            MMsvAttachmentManager& manager = store->AttachmentManagerL();
            CMsvAttachment *attachment = manager.GetAttachmentInfoL(0);
            QString filepath = XQConversions::s60DescToQString(attachment->FilePath());
            ConvergedMessageAttachment* conv_attachment = 
                new ConvergedMessageAttachment(filepath, ConvergedMessageAttachment::EAttachment);
            ConvergedMessageAttachmentList conv_attList;
            conv_attList << conv_attachment;
            aMessage->addAttachments(conv_attList);
            CleanupStack::PopAndDestroy( store );
        }
        else
        {
            //For non drafts cases extract the vcf/vcs using datamodel interface
            UniDataModelLoader* pluginLoader = new UniDataModelLoader();
            UniDataModelPluginInterface* pluginInterface = 
                                pluginLoader->getDataModelPlugin(ConvergedMessage::BioMsg);
            pluginInterface->setMessageId(aEntry.Id());
            UniMessageInfoList attachments = pluginInterface->attachmentList();
            
            QString attachmentPath = attachments[0]->path();

            ConvergedMessageAttachment* conv_attachment = 
                new ConvergedMessageAttachment(attachmentPath, ConvergedMessageAttachment::EAttachment);
            ConvergedMessageAttachmentList conv_attList;
            conv_attList << conv_attachment;
            aMessage->addAttachments(conv_attList);

            foreach(UniMessageInfo* attachment,attachments)
            {
                delete attachment;
            }
            
            delete pluginLoader;
        }
    }
    else   //plain text
    {
        TInt totalLength( SmsMtmL()->Body().DocumentLength() );
        if ( totalLength > 0 )
        {
            HBufC* bodyText = HBufC::NewLC ( totalLength ); 
            TPtr bodyTextPtr ( bodyText->Des() ); 
            SmsMtmL()->Body().Extract( bodyTextPtr, 0, totalLength );
            aMessage->setBodyText(XQConversions::s60DescToQString(
                bodyTextPtr));
            CleanupStack::PopAndDestroy( bodyText );            
        }
    }
}

// ----------------------------------------------------
// UniEditorSmsPluginPrivate::HandleSessionEventL()
// @see header
// ----------------------------------------------------
void UniEditorSmsPluginPrivate::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
                                                  TAny* /*aArg1*/,
                                                  TAny* /*aArg2*/, TAny* /*aArg3*/)
{
// do nothing
}

// -----------------------------------------------------------------------------
// UniEditorSmsPluginPrivate::populateMessageForReplyL
// @see Header
// -----------------------------------------------------------------------------
//
void UniEditorSmsPluginPrivate::populateMessageForReplyL(
        ConvergedMessage* aMessage)
    {
    // find out if the message is incoming or outgoing
    CSmsPDU::TSmsPDUType smsPduType = SmsMtmL()->SmsHeader().Type();

    // for incoming message, populate sender address in To-field
    if(smsPduType == CSmsPDU::ESmsDeliver)
        {
        QString addr;
        fromAddress(addr);    
        if(!addr.isEmpty())
            {
            ConvergedMessageAddress messageAddress(addr);
            aMessage->addToRecipient(messageAddress);
            }
        // read flag for reply-via-same-smsc usecase
        bool replyPath = SmsMtmL()->SmsHeader().Deliver().ReplyPath();
        aMessage->setReplyPath(replyPath);
        if(replyPath)
            {
            // save smsc in the message
            QString scaddress = XQConversions::s60DescToQString(
                    SmsMtmL()->SmsHeader().Deliver().ServiceCenterAddress());
            aMessage->setOriginatingSC(scaddress);
            }
        }
    // else, for outgoing message, populate receiver address in To-field
    else if(smsPduType == CSmsPDU::ESmsSubmit)
        {
        populateRecipientsL(aMessage);
        }

    // resolve to-field contacts
    ConvergedMessageAddressList addrList = aMessage->toAddressList();
    int addrCount = addrList.count();
    for(int i=0; i<addrCount; i++)
        {
        ConvergedMessageAddress* addr = addrList.at(i);
        // resolve contact if alias is empty
        if(addr->alias().isEmpty())
            {
            QString alias;
            int count;
            if(-1 != MsgContactHandler::resolveContactDisplayName(
                                        addr->address(), alias, count))
                {
                addr->setAlias(alias);
                }
            }
        }
    }

//---------------------------------------------------------------
// UniEditorSmsPluginPrivate::fromAddress
// @see header
//---------------------------------------------------------------
void UniEditorSmsPluginPrivate::fromAddress(
        QString& messageAddress)
{
    CPlainText* pText = CPlainText::NewL();
    CleanupStack::PushL(pText);

    CSmsHeader* smsHeader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *pText);
    CleanupStack::PushL(smsHeader);

    CMsvEntry &cEntry = SmsMtmL()->Entry();

    CMsvStore* store = cEntry.ReadStoreL();
    CleanupStack::PushL(store);

    smsHeader->RestoreL(*store);

    messageAddress
            = XQConversions::s60DescToQString(smsHeader->FromAddress());
    CleanupStack::PopAndDestroy(3, pText);
}

//  End of File
