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


#include "engine.h"
#include "enginewrapper.h"

#include "creator_message.h" 
#include "creator_traces.h"
#include "creator_contactsetcache.h"
#include "creator_phonebook.h"
#include <apgcli.h>
#include <MuiuServiceUtilities.h>
#include <utf.h>

#include <mmf/common/mmfcontrollerpluginresolver.h> // for CleanupResetAndDestroyPushL

//static const TInt KMessagesFieldLength = 1024;

//_LIT(KCreatorMessagesPrefixName, "CR_");
//_LIT(KCreatorMessagesPrefixFolderName, "CR_FLDR_");


const TUid KUidBIOBusinessCardMsg =  {0x10005531}; 
_LIT(KEmpty, "");

//----------------------------------------------------------------------------

CMessagesParameters::CMessagesParameters()
    {
    LOGSTRING("Creator: CMessagesParameters::CMessagesParameters");

    iSenderAddress = HBufC::New(128);
    iRecipientAddress = HBufC::New(128);
    iMessageSubject = HBufC::New(128);
    iMessageBodyText = HBufC::New(10240);
    TRAP_IGNORE(iAttachments = new(ELeave) CArrayFixFlat<TInt>(64));
    iCreateAsUnread = ETrue;
    }

CMessagesParameters::~CMessagesParameters()
    {
    LOGSTRING("Creator: CMessagesParameters::~CMessagesParameters");


    delete iMessageBodyText;
    delete iMessageSubject;
    delete iRecipientAddress;
    delete iSenderAddress;
    delete iAttachments;
    iAttachmentPaths.ResetAndDestroy();
    iAttachmentPaths.Close(); 
    iRecipientAddressArray.ResetAndDestroy();
    iRecipientAddressArray.Close();
    iRecipientLinkIds.Close();
    iSenderLinkIds.Close();
    }

//----------------------------------------------------------------------------

CCreatorMessages* CCreatorMessages::NewL(CCreatorEngine* aEngine)
    {
    CCreatorMessages* self = CCreatorMessages::NewLC(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

CCreatorMessages* CCreatorMessages::NewLC(CCreatorEngine* aEngine)
    {
    CCreatorMessages* self = new (ELeave) CCreatorMessages;
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    return self;
    }

CCreatorMessages::CCreatorMessages()
    {
    }

void CCreatorMessages::ConstructL(CCreatorEngine* aEngine)
    {
    LOGSTRING("Creator: CCreatorMessages::ConstructL");
    iEngine = aEngine;
    
    TRAP_IGNORE(iAttachments = new(ELeave) CArrayFixFlat<TInt>(64));
    iWaiter = CAsyncWaiter::NewL();
    }

CCreatorMessages::~CCreatorMessages()
    {
    LOGSTRING("Creator: CCreatorMessages::~CCreatorMessages");

    if ( iEngine && iEntryIds.Count() )
        {
        TRAP_IGNORE( iEngine->WriteEntryIdsToStoreL( iEntryIds, KUidDictionaryUidMessages ) );
        }
    iEntryIds.Reset();
    iEntryIds.Close();
    
    delete iParameters;
    delete iWaiter;
    delete iTmpPhoneNumber;
    delete iTmpEmail;
    delete iAttachments;

    iRecipientArray.ResetAndDestroy();
    iRecipientArray.Close();
    iSenderArray.ResetAndDestroy();
    iSenderArray.Close();
    }

//----------------------------------------------------------------------------

void CCreatorMessages::QueryDialogClosedL(TBool aPositiveAction, TInt aUserData)
    {
    LOGSTRING("Creator: CCreatorMessages::QueryDialogClosedL");  
    
    if( aPositiveAction == EFalse )
        {
        iEngine->ShutDownEnginesL();
        return;
        }
    
    const TDesC* showText = &KSavingText;
    TBool finished(EFalse);
    TBool retval(ETrue);
    switch(aUserData)
        {
        case ECreatorMessagesDelete:
            showText = &KDeletingText;
            iEntriesToBeCreated = 1;
            finished = ETrue;
            break;
        case ECreatorMessagesStart:
            // message type query
            retval = iEngine->GetEngineWrapper()->ListQueryDialog(_L("Message type"), R_MESSAGE_TYPE_QUERY, (TInt*) &iMessageType, this, ECreatorMessagesMessageType);
            break;
        case ECreatorMessagesMessageType:
            retval = iEngine->GetEngineWrapper()->ListQueryDialog(_L("Folder type"), R_FOLDER_TYPE_QUERY, (TInt*) &iFolderType, this, ECreatorMessagesFolderType);
            break;
        case ECreatorMessagesFolderType:
            // query create as unread
            retval = iEngine->GetEngineWrapper()->ListQueryDialog(_L("Message status"), R_UNREAD_QUERY, (TInt*)&iCreateAsUnread, this, ECreatorMessagesMessageStatus);
            break;
        case ECreatorMessagesMessageStatus:
            if( iMessageType == ESMS || iMessageType == EMMS || iMessageType == EEmail )
                {
                iDefinedMessageLength = 100;
                retval = iEngine->GetEngineWrapper()->EntriesQueryDialog(&iDefinedMessageLength, _L("Amount of characters in message body?"), ETrue, 
                    this, ECreatorMessagesCharsInBody
                    );
                break;
                }
            else
                {
                iDefinedMessageLength = 0;
                // goto query attachments ... :-) DO NOT break;
                }
        case ECreatorMessagesCharsInBody:
            // query attachments
            iAttachments->Reset();
            if( iMessageType == EMMS || iMessageType == EEmail)
                {
                retval = iEngine->GetEngineWrapper()->ListQueryDialog(_L("Choose attachment:"), R_ATTACHMENT_MULTI_SELECTION_QUERY, 
                    iAttachments, this, ECreatorMessagesAttachment
                    );
                }
            else if( iMessageType == EAMS )
                {
                iAttachments->AppendL( TInt(0) );
                retval = iEngine->GetEngineWrapper()->ListQueryDialog(_L("Choose attachment:"), R_AMS_ATTACHMENT_SINGLE_SELECTION_QUERY, 
                    &iAttachments->At(0), this, ECreatorMessagesAttachment
                    );
                }
            else if( iMessageType == EIrMessage || iMessageType == EBTMessage )
                {
                iAttachments->AppendL( TInt(0) );
                retval = iEngine->GetEngineWrapper()->ListQueryDialog(_L("Choose attachment:"), R_ATTACHMENT_SINGLE_SELECTION_QUERY,
                    &iAttachments->At(0), this, ECreatorMessagesAttachment
                    );
                }
            else
                {
                finished = ETrue;
                }
            break;
        case ECreatorMessagesAttachment:
            finished = ETrue;
            if(iMessageType== EMMS || iMessageType == EEmail)
                {
                // "none" is selected
                if (iAttachments->At(0) == 0)
                    {
                    iAttachments->Reset();
                    }
                else  // otherwise fix indexes
                    {
                    for (TInt i=0; i<iAttachments->Count(); i++)
                        iAttachments->At(i)--;  // decrease value by one 

                    }
                }
            break;
        default:
            //some error
            retval = EFalse;
            break;
        }
    if( retval == EFalse )
        {
        iEngine->ShutDownEnginesL();
        }
    else if( finished )
        {
        // add this command to command array
        iEngine->AppendToCommandArrayL(iCommand, NULL, iEntriesToBeCreated);
        // started exucuting commands
        iEngine->ExecuteFirstCommandL( *showText );
        }
    }
//----------------------------------------------------------------------------

TBool CCreatorMessages::AskDataFromUserL(TInt aCommand)
    {
    LOGSTRING("Creator: CCreatorMessages::AskDataFromUserL");

    CCreatorModuleBase::AskDataFromUserL(aCommand);
    
    if ( aCommand == ECmdDeleteMessages )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all messages?"), this, ECreatorMessagesDelete );
        }
    else if ( aCommand ==  ECmdDeleteCreatorMessages )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog( _L("Delete all messages created with Creator?"), this, ECreatorMessagesDelete  );
        }

    return iEngine->GetEngineWrapper()->EntriesQueryDialog( &iEntriesToBeCreated, _L("How many entries to create?"), EFalse, this, ECreatorMessagesStart );
    }

//----------------------------------------------------------------------------

TInt CCreatorMessages::CreateRandomMessageEntryL(TInt aCommand)
    {
    LOGSTRING2("Creator: CCreatorMessages::CreateRandomMessageEntryL - Command: %d", aCommand);

    TInt err = KErrNone;
    
    iDefinedMessageLength = iEngine->RandomNumber(100, 300);


    switch (aCommand)
        {
	    case ECmdCreateRandomEntrySMSInbox:     { iMessageType=ESMS; iFolderType=EInbox; err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntrySMSDrafts:    { iMessageType=ESMS; iFolderType=EDrafts; err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntrySMSOutbox:    { iMessageType=ESMS; iFolderType=EOutbox; err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntrySMSSent:      { iMessageType=ESMS; iFolderType=ESent; err=CreateMessageEntryL(NULL); break;}
	    
        case ECmdCreateRandomEntryMMSInbox:     { iMessageType=EMMS; iFolderType=EInbox; err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryMMSDrafts:    { iMessageType=EMMS; iFolderType=EDrafts; err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryMMSOutbox:    { iMessageType=EMMS; iFolderType=EOutbox; err=CreateMessageEntryL(NULL); break;}    
	    case ECmdCreateRandomEntryMMSSent:      { iMessageType=EMMS; iFolderType=ESent; err=CreateMessageEntryL(NULL); break;}
	    
        case ECmdCreateRandomEntryEmailInbox:   { iMessageType=EEmail; iFolderType=EInbox; err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryEmailDrafts:  { iMessageType=EEmail; iFolderType=EDrafts; err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryEmailOutbox:  { iMessageType=EEmail; iFolderType=EOutbox; err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryEmailSent:    { iMessageType=EEmail; iFolderType=ESent; err=CreateMessageEntryL(NULL); break;}
	    
        case ECmdCreateRandomEntryBIOInbox:     { iMessageType=ESmartMessage; iFolderType=EInbox; err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryBIODrafts:    { iMessageType=ESmartMessage; iFolderType=EDrafts; err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryBIOOutbox:    { iMessageType=ESmartMessage; iFolderType=EOutbox; err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryBIOSent:      { iMessageType=ESmartMessage; iFolderType=ESent; err=CreateMessageEntryL(NULL); break;}
	    
        case ECmdCreateRandomEntryIRInbox:      { iMessageType=EIrMessage; iFolderType=EInbox; iAttachments->Reset(); iAttachments->AppendL( iEngine->RandomNumber(CCreatorEngine::EJPEG_25kB, CCreatorEngine::LAST_FILE_ID-1) ); err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryIRDrafts:     { iMessageType=EIrMessage; iFolderType=EDrafts; iAttachments->Reset(); iAttachments->AppendL( iEngine->RandomNumber(CCreatorEngine::EJPEG_25kB, CCreatorEngine::LAST_FILE_ID-1) ); err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryIROutbox:     { iMessageType=EIrMessage; iFolderType=EOutbox; iAttachments->Reset(); iAttachments->AppendL( iEngine->RandomNumber(CCreatorEngine::EJPEG_25kB, CCreatorEngine::LAST_FILE_ID-1) ); err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryIRSent:       { iMessageType=EIrMessage; iFolderType=ESent; iAttachments->Reset(); iAttachments->AppendL( iEngine->RandomNumber(CCreatorEngine::EJPEG_25kB, CCreatorEngine::LAST_FILE_ID-1) ); err=CreateMessageEntryL(NULL); break;}
	    
        case ECmdCreateRandomEntryBTInbox:      { iMessageType=EBTMessage; iFolderType=EInbox; iAttachments->Reset(); iAttachments->AppendL( iEngine->RandomNumber(CCreatorEngine::EJPEG_25kB, CCreatorEngine::LAST_FILE_ID-1) ); err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryBTDrafts:     { iMessageType=EBTMessage; iFolderType=EDrafts; iAttachments->Reset(); iAttachments->AppendL( iEngine->RandomNumber(CCreatorEngine::EJPEG_25kB, CCreatorEngine::LAST_FILE_ID-1) ); err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryBTOutbox:     { iMessageType=EBTMessage; iFolderType=EOutbox; iAttachments->Reset(); iAttachments->AppendL( iEngine->RandomNumber(CCreatorEngine::EJPEG_25kB, CCreatorEngine::LAST_FILE_ID-1) ); err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryBTSent:       { iMessageType=EBTMessage; iFolderType=ESent; iAttachments->Reset(); iAttachments->AppendL( iEngine->RandomNumber(CCreatorEngine::EJPEG_25kB, CCreatorEngine::LAST_FILE_ID-1) ); err=CreateMessageEntryL(NULL); break;}
        
	    case ECmdCreateRandomEntryAMSInbox:     { iMessageType=EAMS; iFolderType=EInbox; iAttachments->Reset(); iAttachments->AppendL( CCreatorEngine::EMP3_250kB); err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryAMSDrafts:     { iMessageType=EAMS; iFolderType=EDrafts; iAttachments->Reset(); iAttachments->AppendL( CCreatorEngine::EMP3_250kB); err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryAMSOutbox:     { iMessageType=EAMS; iFolderType=EOutbox; iAttachments->Reset(); iAttachments->AppendL( CCreatorEngine::EMP3_250kB); err=CreateMessageEntryL(NULL); break;}
	    case ECmdCreateRandomEntryAMSSent:     { iMessageType=EAMS; iFolderType=ESent; iAttachments->Reset(); iAttachments->AppendL( CCreatorEngine::EMP3_250kB); err=CreateMessageEntryL(NULL); break;}

	    default: { User::Panic(_L("MessageCommand"), 901);  break;}
        }

    return err;
    }

//----------------------------------------------------------------------------

TInt CCreatorMessages::CreateMessageEntryL(CMessagesParameters *aParameters, TBool /*aTakeUserGivenParameters*/)
    {
    LOGSTRING("Creator: CCreatorMessages::CreateMessageEntryL");
    
    delete iParameters;
    iParameters = 0;   
    
    CMessagesParameters* parameters = aParameters;
           
    // random data needed if no predefined data available
    if (!parameters)
        {
        iParameters = new(ELeave) CMessagesParameters;
        parameters = iParameters;
        // check if values from ui needed
        //if (aTakeUserGivenParameters)
        //    {            
            parameters->iMessageType = iMessageType;
            parameters->iFolderType = iFolderType;
            parameters->iDefinedMessageLength = iDefinedMessageLength;
            parameters->iCreateAsUnread = iCreateAsUnread;

            for (TInt i=0; i<iAttachments->Count(); i++)
                parameters->iAttachments->AppendL(iAttachments->At(i)); 
        //    }
        //else
        //    {
            // message length
        //    parameters->iDefinedMessageLength = iEngine->RandomNumber(100, 300);
        //    }    

        if (parameters->iMessageType == EEmail)
            {
            parameters->iSenderAddress->Des().Copy( iEngine->RandomString(CCreatorEngine::EFirstName) );
            parameters->iSenderAddress->Des().Append( _L("@") );
            parameters->iSenderAddress->Des().Append( iEngine->RandomString(CCreatorEngine::ECompany) );
            parameters->iSenderAddress->Des().Append( _L(".com") );

            parameters->iRecipientAddress->Des().Copy( iEngine->RandomString(CCreatorEngine::EFirstName) );
            parameters->iRecipientAddress->Des().Append( _L("@") );
            parameters->iRecipientAddress->Des().Append( iEngine->RandomString(CCreatorEngine::ECompany) );
            parameters->iRecipientAddress->Des().Append( _L(".net") );
            }
        else if (parameters->iMessageType == EIrMessage || parameters->iMessageType == EBTMessage)
            {
            parameters->iSenderAddress->Des().Copy( iEngine->RandomString(CCreatorEngine::EFirstName) );
            parameters->iRecipientAddress->Des().Copy( iEngine->RandomString(CCreatorEngine::EFirstName) );
            }
        else
            {
            parameters->iSenderAddress->Des().Copy( iEngine->RandomString(CCreatorEngine::EPhoneNumber) );
            parameters->iRecipientAddress->Des().Copy( iEngine->RandomString(CCreatorEngine::EPhoneNumber) );
            }

        // subject
        parameters->iMessageSubject->Des() = iEngine->RandomString(CCreatorEngine::EMessageSubject);

        // message body
        parameters->iMessageBodyText->Des().Copy(_L(""));
        TPtrC randText = iEngine->RandomString(CCreatorEngine::EMessageText);
        for (TInt i=0; i<parameters->iDefinedMessageLength / randText.Length() +1; i++)
            {
            parameters->iMessageBodyText->Des().Append( randText );
            parameters->iMessageBodyText->Des().Append( _L(" ") );
            }
        parameters->iMessageBodyText->Des() = parameters->iMessageBodyText->Des().Left(parameters->iDefinedMessageLength);

        // BIO message type (from bitsids.h)  // TO-DO: add support for all BIO message types...
        parameters->iBIOMessageType = KUidBIOBusinessCardMsg;
        }
    
    

    TInt err = KErrNone;

    switch (parameters->iMessageType)
        {
        case ESMS:          { err = CreateSMSEntryL(*parameters);  break; }
        case EMMS:          { err = CreateMMSEntryL(*parameters);  break; }
        case EAMS:          { err = CreateAMSEntryL(*parameters);  break; }
        case EEmail:        { err = CreateEmailEntryL(*parameters);  break; }
        case ESmartMessage: { err = CreateSmartMessageEntryL(*parameters);  break; }
        case EIrMessage:    { err = CreateObexEntryL( TUid::Uid(KUidMsgTypeIrTInt32), *parameters );  break; }    
        case EBTMessage:    { err = CreateObexEntryL( TUid::Uid(KUidMsgTypeBtTInt32), *parameters );  break; }
        default:            { User::Panic(_L("MsgType"), 851);  break;}

        }

    iSenderArray.ResetAndDestroy();
    iRecipientArray.ResetAndDestroy();
    return err;
    }

//----------------------------------------------------------------------------

TInt CCreatorMessages::CreateSMSEntryL(const CMessagesParameters& parameters)
    {
    LOGSTRING("Creator: CCreatorMessages::CreateSMSEntryL");

    TInt err = KErrNone;

               
    // init
    CMsvSession* session = CMsvSession::OpenSyncL(*this);
    CleanupStack::PushL(session);
    CClientMtmRegistry* registry = CClientMtmRegistry::NewL(*session);
    CleanupStack::PushL(registry);

    // get the client mtm and return if it isn't supported in the system        
    CSmsClientMtm* clientMtm = NULL;
    TRAP(err, clientMtm = static_cast<CSmsClientMtm*>(registry->NewMtmL(KUidMsgTypeSMS)));
    if (err || !clientMtm)
        {
        iEngine->GetEngineWrapper()->ShowErrorMessage(_L("SMS: Message type module not found"));
        CleanupStack::PopAndDestroy(2);
        User::Leave(KErrNotFound);
        }  
    CleanupStack::PushL(clientMtm);   

    // create a new object to access an existing entry
    CMsvEntry* msvEntry = CMsvEntry::NewL(*session, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
    CleanupStack::PushL(msvEntry);
  
    // get default service
    TMsvId defaultServiceId = 0;
    TRAP(err, defaultServiceId = clientMtm->DefaultServiceL());
    if (err)
        {
        iEngine->GetEngineWrapper()->ShowErrorMessage(_L("SMS: Define a SMS centre first"));
        CleanupStack::PopAndDestroy(2);
        User::Leave(KErrNotFound);
        } 
    
    // set folder type
    switch (parameters.iFolderType)
        {
        case EInbox:    { msvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);   break; }
        case EDrafts:   { msvEntry->SetEntryL(KMsvDraftEntryId);              break; }
        case EOutbox:   { msvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);  break; }
        case ESent:     { msvEntry->SetEntryL(KMsvSentEntryId);               break; }
        default:        { User::Panic(_L("Folder Type"), 871);                break; } 
        }
        
    // mtm takes ownership of entry context	
    CleanupStack::Pop(msvEntry);
    clientMtm->SetCurrentEntryL(msvEntry);    
    
    // create a new message
    clientMtm->CreateMessageL(defaultServiceId);

    // set the from field to sms header
    // !!! This will cause CRASH
    /*if (parameters.iFolderType == EInbox)
        {
        CSmsHeader* smsHeader = &clientMtm->SmsHeader();
        delete smsHeader; // <-- This will cause CRASH
        smsHeader = NULL;
        smsHeader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, clientMtm->Body());        
        if( parameters.iSenderAddress )
            {
            smsHeader->SetFromAddressL( parameters.iSenderAddress->Des() );
            }
        else
            {
            smsHeader->SetFromAddressL(KEmpty);
            }
        }       */

    // set body
    clientMtm->Body().Reset();
    clientMtm->Body().InsertL(0, parameters.iMessageBodyText->Des());        

    // get the entry of the message
    TMsvEntry messageEntry = clientMtm->Entry().Entry();

    // set the details field
    if (parameters.iFolderType == EInbox)
        {
        SetSenderToEntryDetailsL(messageEntry, parameters, EFalse);        
        }
    else
        {
        SetRecipientToEntryDetailsL(messageEntry, parameters, EFalse);
        // Add all recipients to clientMtm
        // iRecipientArray is up-to-date so don't call AddRecipientsL here 
        for( TInt i = 0; i < iRecipientArray.Count(); i++ )
            {
            clientMtm->AddAddresseeL( iRecipientArray[i]->Des() );
            }
        }

    // set the sending state for outbox message
    // to avoid SMSMtmUI:15 panic in messages application
    if (parameters.iFolderType == EOutbox )
        {
        messageEntry.SetSendingState( KMsvSendStateWaiting );
        }

    // set the description field same as the first part of the message body
    messageEntry.iDescription.Set( parameters.iMessageBodyText->Des().Left(KSmsDescriptionLength) );

    // save the changes done above
    clientMtm->Entry().ChangeL(messageEntry);

    // save the message     
    clientMtm->SaveMessageL();
    	
	// final fine tuning
    messageEntry.SetAttachment(EFalse);
    messageEntry.iDate.HomeTime();
    messageEntry.SetVisible(ETrue);
    messageEntry.SetInPreparation(EFalse);
   
    if (parameters.iCreateAsUnread)
        {
        messageEntry.SetUnread(ETrue);
        messageEntry.SetNew(ETrue);
        }
    else
        {
        messageEntry.SetUnread(EFalse);
        messageEntry.SetNew(EFalse);
        }
    
    messageEntry.SetComplete(ETrue);
    messageEntry.iServiceId = defaultServiceId;
    messageEntry.iRelatedId = 0;
    clientMtm->Entry().ChangeL(messageEntry);

    // id has been generated, store it for being able to delete
    // only entries created with Creator
    iEntryIds.AppendL( messageEntry.Id() );
    
    CleanupStack::PopAndDestroy(3); //session, registry, clientMtm
        
    return err;

    }

//----------------------------------------------------------------------------

TInt CCreatorMessages::CreateMMSEntryL(const CMessagesParameters& parameters)
    {
    LOGSTRING("Creator: CCreatorMessages::CreateMMSEntryL");

    TInt err = KErrNone;
        
    // init
    CMsvSession* session = CMsvSession::OpenSyncL(*this);
    CleanupStack::PushL(session);
    CClientMtmRegistry* registry = CClientMtmRegistry::NewL(*session);
    CleanupStack::PushL(registry);

    // get the client mtm and return if it isn't supported in the system        
    CMmsClientMtm* clientMtm = NULL;
    TRAP(err, clientMtm = static_cast<CMmsClientMtm*>(registry->NewMtmL(KUidMsgTypeMultimedia)));
    if (err || !clientMtm)
        {
        iEngine->GetEngineWrapper()->ShowErrorMessage(_L("MMS: Message type module not found"));
        CleanupStack::PopAndDestroy(2);
        User::Leave(KErrNotFound);
        }  
    CleanupStack::PushL(clientMtm);   

    // create a new object to access an existing entry
    CMsvEntry* msvEntry = CMsvEntry::NewL(*session, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
    CleanupStack::PushL(msvEntry);
  
    // get default service
    TMsvId defaultServiceId = 0;
    TRAP(err, defaultServiceId = clientMtm->DefaultServiceL());
    if (err)
        {
        iEngine->GetEngineWrapper()->ShowErrorMessage(_L("MMS: Define MMS settings first"));
        CleanupStack::PopAndDestroy(4);
        User::Leave(KErrNotFound);
        } 
    
    // set folder type
    switch (parameters.iFolderType)
        {
        case EInbox:    { msvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);   break; }
        case EDrafts:   { msvEntry->SetEntryL(KMsvDraftEntryId);              break; }
        case EOutbox:   { msvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);  break; }
        case ESent:     { msvEntry->SetEntryL(KMsvSentEntryId);               break; }
        default:        { User::Panic(_L("Folder Type"), 871);                break; } 
        }
        
    // mtm takes ownership of entry context	
    CleanupStack::Pop(msvEntry);
    clientMtm->SetCurrentEntryL(msvEntry);    
    
    // create a new message
    clientMtm->CreateMessageL(defaultServiceId);

    // set subject
    clientMtm->SetSubjectL( parameters.iMessageSubject->Des() );
            
    // set some mms defaults
    clientMtm->SetMessageClass(EMmsClassPersonal);
    clientMtm->SetExpiryInterval(86400);
    clientMtm->SetDeliveryTimeInterval(3600);
    clientMtm->SetMessagePriority(EMmsPriorityLow);
    clientMtm->SetSenderVisibility(EMmsMaximumSenderVisibility);
    clientMtm->SetDeliveryReport(EMmsDeliveryReportYes);
    clientMtm->SetReadReply(EMmsReadReplyYes);        
    
    // set the sender / addressee field
    if (parameters.iFolderType == EInbox)
        {        
        AddMtmSenderL(*clientMtm, parameters, EFalse);                
        }
    
    // Add all recipients to clientMtm
    AddRecipientsL( *clientMtm, parameters, EFalse );  
        
    // get an access to the message store
    CMsvStore* store = msvEntry->EditStoreL();
    CleanupStack::PushL(store);                  
   

    // set body as an attachment
    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL(mimeHeaders);
    mimeHeaders->SetContentTypeL( _L8("text") );
    mimeHeaders->SetContentSubTypeL( _L8("plain") );
    mimeHeaders->SetMimeCharset( KMmsUtf8 );
    mimeHeaders->SetSuggestedFilenameL( _L("body.txt") );
    
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile); // ownership of attachmentInfo will be transferred
    CleanupStack::PushL(attachmentInfo);
    attachmentInfo->SetAttachmentNameL( _L("body.txt") );
    attachmentInfo->SetMimeTypeL( _L8("text/plain") );
    mimeHeaders->StoreL( *attachmentInfo );
    
    RFile textFile;

    CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
    attachMan.CreateAttachmentL(_L("body.txt"), textFile, attachmentInfo, waiter->iStatus );
    CleanupStack::Pop(); // waiter
    CleanupStack::Pop(); // attachmentInfo
    CleanupStack::PushL(waiter);
    waiter->StartAndWait();
    CleanupStack::PopAndDestroy(waiter);        
   
    HBufC8* tempBuf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( parameters.iMessageBodyText->Des() );
    CleanupStack::PushL(tempBuf);
    textFile.Write( tempBuf->Des() );        
    textFile.Close();
    CleanupStack::PopAndDestroy(); //tempBuf
    
    CleanupStack::PopAndDestroy(); //mimeHeaders

    
    // get the entry of the message
    TMsvEntry messageEntry = clientMtm->Entry().Entry();        
    
    // set the details field
    if (parameters.iFolderType == EInbox)
        {
        SetSenderToEntryDetailsL(messageEntry, parameters, EFalse);        
        }
    else
        {
        SetRecipientToEntryDetailsL(messageEntry, parameters, EFalse);
        }    

    // set the description field same as the message subject
    messageEntry.iDescription.Set( parameters.iMessageSubject->Des().Left(KSmsDescriptionLength) );

    // save the changes done above
    clientMtm->Entry().ChangeL(messageEntry);        
    

      // save the attachments
    HandleAttachementsL(parameters, store, err );
//    for (TInt i=0; i<parameters.iAttachments->Count(); i++)
//        {
//        TFileName sourceFileName = iEngine->TestDataPathL( (CCreatorEngine::TTestDataPath) parameters.iAttachments->At(i) );
//
//        TParse parser;
//        parser.Set(sourceFileName, NULL, NULL);
//        TFileName shortFileName = parser.NameAndExt();
//
//        // get the mime type
//        RApaLsSession ls;
//        User::LeaveIfError(ls.Connect());
//        CleanupClosePushL<RApaLsSession>(ls);
//        TUid appUid;
//        TDataType dataType;
//        ls.AppForDocument(sourceFileName, appUid, dataType);
//        CleanupStack::PopAndDestroy(); //ls
//        TPtrC8 mimeType = dataType.Des8();
//        
//        // attachment settings
//        MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
//        CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile); // ownership of attachmentInfo will be transferred
//        attachmentInfo->SetAttachmentNameL( shortFileName );        
//        attachmentInfo->SetMimeTypeL( mimeType );        
//
//        // save
//        CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
//        
//        TRAP(err, attachMan.AddAttachmentL(sourceFileName, attachmentInfo, waiter->iStatus ));
//        if (err == KErrNone)
//            waiter->StartAndWait();
//        else
//            LOGSTRING2("Creator: CCreatorMessages::CreateMMSEntryL failed to attach %S", &sourceFileName);
//
//        CleanupStack::PopAndDestroy(waiter);  
//        }

    // save the changes made to the message store
    store->CommitL();
    CleanupStack::PopAndDestroy(store);   
           
    
    // save the message      
    clientMtm->SaveMessageL();
    	
	// final fine tuning
	messageEntry.SetAttachment(ETrue);
    messageEntry.iDate.HomeTime();
    messageEntry.SetVisible(ETrue);
    messageEntry.SetInPreparation(EFalse);
    if (parameters.iCreateAsUnread)
        {
        messageEntry.SetUnread(ETrue);
        messageEntry.SetNew(ETrue);
        }
    else
        {
        messageEntry.SetUnread(EFalse);
        messageEntry.SetNew(EFalse);
        }
    messageEntry.SetComplete(ETrue);
    messageEntry.iServiceId = defaultServiceId;
    messageEntry.iRelatedId = 0;
    
    if (parameters.iFolderType == EDrafts) 
        messageEntry.SetReadOnly(EFalse);
    else
        messageEntry.SetReadOnly(ETrue);

	messageEntry.iMtmData1 = KMmsMessageMRetrieveConf | KMmsMessageMobileTerminated;
	
    clientMtm->Entry().ChangeL(messageEntry);
    
    // id has been generated, store it for being able to delete
    // only entries created with Creator
    iEntryIds.AppendL( messageEntry.Id() );
    
    CleanupStack::PopAndDestroy(3); //session, registry, clientMtm        
        

    return err;
    }

//----------------------------------------------------------------------------
void CCreatorMessages::HandleAttachementsL(const CMessagesParameters& parameters, CMsvStore* store, TInt err )
    {
    // save the attachments (added with id's)
  for (TInt i=0; i<parameters.iAttachments->Count(); i++)
      {
      TFileName sourceFileName = iEngine->TestDataPathL( (CCreatorEngine::TTestDataPath) parameters.iAttachments->At(i) );
      HandleAttachementL(store, err, sourceFileName );

      }    
  //Adding attachmentpaths
  for (TInt i=0; i<parameters.iAttachmentPaths.Count(); i++)
      {
      HBufC* point = parameters.iAttachmentPaths[i];
      TFileName sourceFileName(point->Des());      
      HandleAttachementL(store, err, sourceFileName );
      }
    }
//----------------------------------------------------------------------------

void CCreatorMessages::HandleAttachementL(CMsvStore* store, TInt err, TFileName& sourceFileName )
    {
    TParse parser;
    parser.Set(sourceFileName, NULL, NULL);
    TFileName shortFileName = parser.NameAndExt();

    // get the mime type
    RApaLsSession ls;
    User::LeaveIfError(ls.Connect());
    CleanupClosePushL<RApaLsSession>(ls);
    TUid appUid;
    TDataType dataType;
    ls.AppForDocument(sourceFileName, appUid, dataType);
    CleanupStack::PopAndDestroy(); //ls
    TPtrC8 mimeType = dataType.Des8();
    
    // attachment settings
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile); // ownership of attachmentInfo will be transferred
    CleanupStack::PushL(attachmentInfo);
    attachmentInfo->SetAttachmentNameL( shortFileName );        
    attachmentInfo->SetMimeTypeL( mimeType );
    
    if ( mimeType == _L8("text/plain") )
        {
        // CMsvMimeHeaders needs to be stored in case of text file 
        CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
        CleanupStack::PushL(mimeHeaders);
        mimeHeaders->SetContentTypeL( _L8("text") );
        mimeHeaders->SetContentSubTypeL( _L8("plain") );
        mimeHeaders->SetMimeCharset( KMmsUtf8 );
        mimeHeaders->StoreL( *attachmentInfo );
        CleanupStack::PopAndDestroy( mimeHeaders );
        }

    // save
    
    CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
    TRAP(err, attachMan.AddAttachmentL(sourceFileName, attachmentInfo, waiter->iStatus ));
    if (err == KErrNone)
    	{
        waiter->StartAndWait();
        CleanupStack::PopAndDestroy(waiter);
        CleanupStack::Pop(); // attachmentInfo, the ownership is transferred
    	}
    else
    	{
    	CleanupStack::PopAndDestroy(waiter);
    	CleanupStack::PopAndDestroy(); // attachmentInfo, ownership is not transferred (leave occurs)
        LOGSTRING2("Creator: CCreatorMessages::HandleAttachementsL failed to attach %S", &sourceFileName);
    	}
    }
      //----------------------------------------------------------------------------

TInt CCreatorMessages::CreateAMSEntryL(const CMessagesParameters& parameters)
    {
    LOGSTRING("Creator: CCreatorMessages::CreateAMSEntryL");

    TInt err = KErrNone;

        
    // init
    CMsvSession* session = CMsvSession::OpenSyncL(*this);
    CleanupStack::PushL(session);
    CClientMtmRegistry* registry = CClientMtmRegistry::NewL(*session);
    CleanupStack::PushL(registry);

    // get the client mtm and return if it isn't supported in the system        
    CMmsClientMtm* clientMtm = NULL;
    TRAP(err, clientMtm = static_cast<CMmsClientMtm*>(registry->NewMtmL(KUidMsgTypeMultimedia)));
    if (err || !clientMtm)
        {
        iEngine->GetEngineWrapper()->ShowErrorMessage(_L("MMS: Message type module not found"));
        CleanupStack::PopAndDestroy(2);
        User::Leave(KErrNotFound);
        }  
    CleanupStack::PushL(clientMtm);   

    // create a new object to access an existing entry
    CMsvEntry* msvEntry = CMsvEntry::NewL(*session, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
    CleanupStack::PushL(msvEntry);
  
    // get default service
    TMsvId defaultServiceId = 0;
    TRAP(err, defaultServiceId = clientMtm->DefaultServiceL());
    if (err)
        {
        iEngine->GetEngineWrapper()->ShowErrorMessage(_L("MMS: Define MMS settings first"));
        CleanupStack::PopAndDestroy(2);
        User::Leave(KErrNotFound);
        } 
    
    // set folder type
    switch (parameters.iFolderType)
        {
        case EInbox:    { msvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);   break; }
        case EDrafts:   { msvEntry->SetEntryL(KMsvDraftEntryId);              break; }
        case EOutbox:   { msvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);  break; }
        case ESent:     { msvEntry->SetEntryL(KMsvSentEntryId);               break; }
        default:        { User::Panic(_L("Folder Type"), 871);                break; } 
        }
        
    // mtm takes ownership of entry context	
    CleanupStack::Pop(msvEntry);
    clientMtm->SetCurrentEntryL(msvEntry);    
    
    // create a new message
    clientMtm->CreateMessageL(defaultServiceId);

    // set subject
    clientMtm->SetSubjectL(parameters.iMessageSubject->Des() );
            
    // set some mms defaults
    clientMtm->SetMessageClass(EMmsClassPersonal);
    clientMtm->SetExpiryInterval(86400);
    clientMtm->SetDeliveryTimeInterval(3600);
    clientMtm->SetMessagePriority(EMmsPriorityLow);
    clientMtm->SetSenderVisibility(EMmsMaximumSenderVisibility);
    clientMtm->SetDeliveryReport(EMmsDeliveryReportYes);
    clientMtm->SetReadReply(EMmsReadReplyYes);        
    
    // set the sender / addressee field
    if (parameters.iFolderType == EInbox)
        {
        AddMtmSenderL(*clientMtm, parameters, EFalse);                
        }

    // Add all recipients to clientMtm
    AddRecipientsL( *clientMtm, parameters, EFalse );
    
        
    // get an access to the message store
    CMsvStore* store = msvEntry->EditStoreL();
    CleanupStack::PushL(store);                  
   
    // get the entry of the message
    TMsvEntry messageEntry = clientMtm->Entry().Entry();        
    
    // set the details field
    if (parameters.iFolderType == EInbox)
        {
        SetSenderToEntryDetailsL(messageEntry, parameters, EFalse);        
        }  
    else
        {
        SetRecipientToEntryDetailsL(messageEntry, parameters, EFalse);
        }

    // set the description field same as the message subject
    messageEntry.iDescription.Set( parameters.iMessageSubject->Des().Left(KSmsDescriptionLength) );
    messageEntry.iBioType = KUidMsgSubTypeMmsAudioMsg.iUid;
    
    // save the changes done above
    clientMtm->Entry().ChangeL(messageEntry);        
    

    TFileName sourceFileName = iEngine->TestDataPathL( CCreatorEngine::EAMR_20kB );
    TParse parser;
    parser.Set(sourceFileName, NULL, NULL);
    TFileName shortFileName = parser.NameAndExt();

    // get the mime type
    RApaLsSession ls;
    User::LeaveIfError(ls.Connect());
    CleanupClosePushL<RApaLsSession>(ls);
    TUid appUid;
    TDataType dataType;
    ls.AppForDocument(sourceFileName, appUid, dataType);
    CleanupStack::PopAndDestroy(); //ls
    TPtrC8 mimeType = dataType.Des8();
        
    // attachment settings
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile); // ownership of attachmentInfo will be transferred
    attachmentInfo->SetAttachmentNameL( shortFileName );        
    attachmentInfo->SetMimeTypeL( mimeType );        

    // save
    CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
      
    TRAP(err, attachMan.AddAttachmentL(sourceFileName, attachmentInfo, waiter->iStatus ));
    if (err == KErrNone)
        waiter->StartAndWait();
    else
        LOGSTRING2("Creator: CCreatorMessages::CreateAMSEntryL failed to attach %S", &sourceFileName);

    CleanupStack::PopAndDestroy(waiter);  

    // save the changes made to the message store
    store->CommitL();
    CleanupStack::PopAndDestroy(store);   
           
    
    // save the message      
    clientMtm->SaveMessageL();
    	
	// final fine tuning
	messageEntry.SetAttachment(ETrue);
    messageEntry.iDate.HomeTime();
    messageEntry.SetVisible(ETrue);
    messageEntry.SetInPreparation(EFalse);
    if (parameters.iCreateAsUnread)
        {
        messageEntry.SetUnread(ETrue);
        messageEntry.SetNew(ETrue);
        }
    else
        {
        messageEntry.SetUnread(EFalse);
        messageEntry.SetNew(EFalse);
        }
    messageEntry.SetComplete(ETrue);
    messageEntry.iServiceId = defaultServiceId;
    messageEntry.iRelatedId = 0;
    
    if (parameters.iFolderType == EDrafts) 
        messageEntry.SetReadOnly(EFalse);
    else
        messageEntry.SetReadOnly(ETrue);

	messageEntry.iMtmData1 = KMmsMessageMRetrieveConf | KMmsMessageMobileTerminated;
	
    clientMtm->Entry().ChangeL(messageEntry);
    
    // id has been generated, store it for being able to delete
    // only entries created with Creator
    iEntryIds.AppendL( messageEntry.Id() );
    
    CleanupStack::PopAndDestroy(3); //session, registry, clientMtm        
        

    return err;
    }

//----------------------------------------------------------------------------

TInt CCreatorMessages::CreateEmailEntryL(const CMessagesParameters& parameters)
    {
    LOGSTRING("Creator: CCreatorMessages::CreateEmailEntryL");

    TInt err = KErrNone;

    // init
    CMsvSession* session = CMsvSession::OpenSyncL(*this);
    CleanupStack::PushL(session);
    CClientMtmRegistry* registry = CClientMtmRegistry::NewL(*session);
    CleanupStack::PushL(registry);
    
    // get the client mtm and return if it isn't supported in the system        
    CSmtpClientMtm* clientMtm = NULL;
    TRAP(err, clientMtm = static_cast<CSmtpClientMtm*>(registry->NewMtmL(KUidMsgTypeSMTP)));
    
    if (err || !clientMtm)
        {
        iEngine->GetEngineWrapper()->ShowErrorMessage(_L("Email: Message type module not found"));
        CleanupStack::PopAndDestroy(2);
        User::Leave(KErrNotFound);
        }  
    CleanupStack::PushL(clientMtm);   

    // create a new object to access an existing entry
    CMsvEntry* msvEntry = CMsvEntry::NewL(*session, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
    CleanupStack::PushL(msvEntry);
  
    // get default service
    TMsvId defaultServiceId(0);
    TRAP(err, defaultServiceId = clientMtm->DefaultServiceL());
    if (err)
        {
        iEngine->GetEngineWrapper()->ShowErrorMessage(_L("Email: Define a mailbox first"));
        CleanupStack::PopAndDestroy(2);
        User::Leave(KErrNotFound);
        } 
                
    // set folder type
    switch (parameters.iFolderType)
        {
        case EInbox:    { msvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);   break; }
        case EDrafts:   { msvEntry->SetEntryL(KMsvDraftEntryId);              break; }
        case EOutbox:   { msvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);  break; }
        case ESent:     { msvEntry->SetEntryL(KMsvSentEntryId);               break; }
        case EMailbox:  { msvEntry->SetEntryL(iUserSelectedMailbox);          break; }
        default:        { User::Panic(_L("Folder Type"), 871);                break; } 
        }
        
    // mtm takes ownership of entry context	
    CleanupStack::Pop(msvEntry);
    clientMtm->SetCurrentEntryL(msvEntry); 

    // create a new message
    clientMtm->CreateMessageL(defaultServiceId);

    // set subject
    clientMtm->SetSubjectL( parameters.iMessageSubject->Des() );
    
    // set body
    clientMtm->Body().Reset();
    clientMtm->Body().InsertL(0, parameters.iMessageBodyText->Des()); 

    // get the entry of the message
    TMsvEntry messageEntry = clientMtm->Entry().Entry();
    
    // set the address fields
    if (parameters.iFolderType == EInbox)
        {
        AddSenderToMtmAddresseeL(*clientMtm, parameters, ETrue );
        SetSenderToEntryDetailsL(messageEntry, parameters, ETrue);
        messageEntry.iMtm = KUidMsgTypeIMAP4;  // or any other than KUidMsgTypeSMTP to display 'from' field instead of 'to' field 
        }
    else
        {
        // Add all recipients to clientMtm
        AddRecipientsL( *clientMtm, parameters, ETrue );
        SetRecipientToEntryDetailsL(messageEntry, parameters, EFalse);        
        }

    // set the description field same as the message subject
    messageEntry.iDescription.Set( parameters.iMessageSubject->Des().Left(KSmsDescriptionLength) );

    // save the changes done above
    clientMtm->Entry().ChangeL(messageEntry);
    
    // get an access to the message store
    CMsvStore* store = msvEntry->EditStoreL();
    CleanupStack::PushL(store); 
    
    // save the attachments
    HandleAttachementsL(parameters, store, err);
//    for (TInt i=0; i<parameters.iAttachments->Count(); i++)
//        {
//        TFileName sourceFileName = iEngine->TestDataPathL( (CCreatorEngine::TTestDataPath) parameters.iAttachments->At(i) );
//
//        TParse parser;
//        parser.Set(sourceFileName, NULL, NULL);
//        TFileName shortFileName = parser.NameAndExt();
//
//        // get the mime type
//        RApaLsSession ls;
//        User::LeaveIfError(ls.Connect());
//        CleanupClosePushL<RApaLsSession>(ls);
//        TUid appUid;
//        TDataType dataType;
//        ls.AppForDocument(sourceFileName, appUid, dataType);
//        CleanupStack::PopAndDestroy(); //ls
//        TPtrC8 mimeType = dataType.Des8();
//        
//        // attachment settings
//        MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
//        CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile); // ownership of attachmentInfo will be transferred
//        attachmentInfo->SetAttachmentNameL( shortFileName );        
//        attachmentInfo->SetMimeTypeL( mimeType );        
//
//        // save
//        CAsyncWaiter* waiter = CAsyncWaiter::NewLC();
//
//        TRAP(err, attachMan.AddAttachmentL(sourceFileName, attachmentInfo, waiter->iStatus ));
//        if (err == KErrNone)
//            waiter->StartAndWait();
//        else
//            LOGSTRING2("Creator: CCreatorMessages::CreateEmailEntryL failed to attach %S", &sourceFileName);
//
//        CleanupStack::PopAndDestroy(waiter);  
//        }

    // save the changes made to the message store
    store->CommitL();
    CleanupStack::PopAndDestroy(store); 
            
    // save the message      
    clientMtm->SaveMessageL();
    	
	// final fine tuning
	TMsvEmailEntry temailEntry = static_cast<TMsvEmailEntry>(messageEntry);
    temailEntry.SetMessageFolderType(EFolderTypeUnknown);
    temailEntry.SetDisconnectedOperation(ENoDisconnectedOperations);
    temailEntry.SetEncrypted(EFalse);
    temailEntry.SetSigned(EFalse);
    temailEntry.SetVCard(EFalse);
    temailEntry.SetVCalendar(EFalse);
    temailEntry.SetReceipt(EFalse);
    temailEntry.SetMHTMLEmail(EFalse);
    temailEntry.SetBodyTextComplete(ETrue);
    
	if (parameters.iAttachments->Count() > 0)
        temailEntry.SetAttachment(ETrue);
    else
        temailEntry.SetAttachment(EFalse);
	
    temailEntry.iDate.HomeTime();
    temailEntry.SetVisible(ETrue);
    temailEntry.SetInPreparation(EFalse);
    if (parameters.iCreateAsUnread)
        {
        temailEntry.SetUnread(ETrue);
        temailEntry.SetNew(ETrue);
        }
    else
        {
        temailEntry.SetUnread(EFalse);
        temailEntry.SetNew(EFalse);
        }
    temailEntry.SetComplete(ETrue);
    temailEntry.iServiceId = defaultServiceId;
    temailEntry.iRelatedId = 0;
    
    clientMtm->Entry().ChangeL(temailEntry);

    // reset email headers
    CImHeader* header = CImHeader::NewLC();
    CMsvStore* msvStore = msvEntry->EditStoreL();
    CleanupStack::PushL(msvStore);
    header->RestoreL(*msvStore);
    header->SetSubjectL( parameters.iMessageSubject->Des() );
    if( iSenderArray.Count() == 0 )
        {
        GetSendersL(iSenderArray, parameters, ETrue, KUndef );
        }
    
    if( iSenderArray.Count() > 0 )
        {
        TInt rndNum = iEngine->RandomNumber( iSenderArray.Count() );
        header->SetFromL(iSenderArray[rndNum]->Des());
        }
    else
        {
        header->SetFromL( parameters.iSenderAddress->Des() );
        }
    if( parameters.iRecipientAddress->Length() > 0 )
        {
        header->SetReceiptAddressL( parameters.iRecipientAddress->Des() );
        }
    else if( parameters.iRecipientAddressArray.Count() > 0 )
        {
        header->SetReceiptAddressL( parameters.iRecipientAddressArray[0]->Des());
        }
    
    header->StoreL( *msvStore );
    msvStore->CommitL();
    CleanupStack::PopAndDestroy(2); // msvStore, header
    
    // id has been generated, store it for being able to delete
    // only entries created with Creator
    iEntryIds.AppendL( messageEntry.Id() );
    
    CleanupStack::PopAndDestroy(3); //session, registry, clientMtm 

    return err;
    }

//----------------------------------------------------------------------------

TInt CCreatorMessages::CreateSmartMessageEntryL(const CMessagesParameters& parameters)
    {
    LOGSTRING("Creator: CCreatorMessages::CreateSmartMessageEntryL");

    TInt err = KErrNone;

    // init
    CMsvSession* session = CMsvSession::OpenSyncL(*this);
    CleanupStack::PushL(session);
    CClientMtmRegistry* registry = CClientMtmRegistry::NewL(*session);
    CleanupStack::PushL(registry);

    // get the client mtm and return if it isn't supported in the system        
    CSmsClientMtm* clientMtm = NULL;
    TRAP(err, clientMtm = static_cast<CSmsClientMtm*>(registry->NewMtmL(KUidMsgTypeSMS)));
    if (err || !clientMtm)
        {
        iEngine->GetEngineWrapper()->ShowErrorMessage(_L("BIO: Message type module not found"));
        CleanupStack::PopAndDestroy(2);
        User::Leave(KErrNotFound);
        }  
    CleanupStack::PushL(clientMtm);   

    // create a new object to access an existing entry
    CMsvEntry* msvEntry = CMsvEntry::NewL(*session, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
    CleanupStack::PushL(msvEntry);
  
    // get default service
    TMsvId defaultServiceId = 0;
    TRAP(err, defaultServiceId = clientMtm->DefaultServiceL());
    if (err)
        {
        iEngine->GetEngineWrapper()->ShowErrorMessage(_L("SMS: Define a SMS centre first"));
        CleanupStack::PopAndDestroy(2);
        User::Leave(KErrNotFound);
        } 
    
    // set folder type
    switch (parameters.iFolderType)
        {
        case EInbox:    { msvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);   break; }
        case EDrafts:   { msvEntry->SetEntryL(KMsvDraftEntryId);              break; }
        case EOutbox:   { msvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);  break; }
        case ESent:     { msvEntry->SetEntryL(KMsvSentEntryId);               break; }
        default:        { User::Panic(_L("Folder Type"), 871);                break; } 
        }
        
    // mtm takes ownership of entry context	
    CleanupStack::Pop(msvEntry);
    clientMtm->SetCurrentEntryL(msvEntry);    
    
    // create a new message
    clientMtm->CreateMessageL(defaultServiceId);

     // set the from field to sms header
    if (parameters.iFolderType == EInbox)
        {
        CSmsHeader* smsHeader = &clientMtm->SmsHeader();
        delete smsHeader;
        smsHeader = NULL;
        smsHeader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, clientMtm->Body());
        if( parameters.iSenderAddress )
            {
            smsHeader->SetFromAddressL( parameters.iSenderAddress->Des() );
            }
        else
            {
            smsHeader->SetFromAddressL(KEmpty);
            }
        }       

    // set body, the BIO message itself
    clientMtm->Body().Reset();
    clientMtm->Body().InsertL(0, _L("Business Card\nTester Mike\nThe Company Ltd.\nSoftware Engineer\ntel +358 66 1234567\n")); 

    // get the entry of the message
    TMsvEntry messageEntry = clientMtm->Entry().Entry();

    // TO-DO: Add support for all BIO messages, currently just insert a business card message ... :
    // set the message type
    // parameters.iBIOMessageType ...
    clientMtm->BioTypeChangedL(KUidBIOBusinessCardMsg);
    messageEntry.iBioType = KUidBIOBusinessCardMsg.iUid;

    // set the details field
    if (parameters.iFolderType == EInbox)
        {
        SetSenderToEntryDetailsL(messageEntry, parameters, EFalse);        
        }        
    else
        {
        SetRecipientToEntryDetailsL(messageEntry, parameters, EFalse);
        }

    // set the subject line
    messageEntry.iDescription.Set( parameters.iMessageSubject->Des().Left(KSmsDescriptionLength) );

    // set correct MTM type
    messageEntry.iMtm= KUidBIOMessageTypeMtm;

    // save the changes done above
    clientMtm->Entry().ChangeL(messageEntry);

    // save the message     
    clientMtm->SaveMessageL();
    	
	// final fine tuning
    messageEntry.SetAttachment(EFalse);
    messageEntry.iDate.HomeTime();
    messageEntry.SetVisible(ETrue);
    messageEntry.SetInPreparation(EFalse);
    if (parameters.iCreateAsUnread)
        {
        messageEntry.SetUnread(ETrue);
        messageEntry.SetNew(ETrue);
        }
    else
        {
        messageEntry.SetUnread(EFalse);
        messageEntry.SetNew(EFalse);
        }
    messageEntry.SetComplete(ETrue);
    messageEntry.iServiceId = defaultServiceId;
    messageEntry.iRelatedId = 0;
    clientMtm->Entry().ChangeL(messageEntry);
    
    // id has been generated, store it for being able to delete
    // only entries created with Creator
    iEntryIds.AppendL( messageEntry.Id() );
    
    CleanupStack::PopAndDestroy(3); //session, registry, clientMtm
        
    
    return err;
    }

//----------------------------------------------------------------------------

TInt CCreatorMessages::CreateObexEntryL(TUid aMtm, const CMessagesParameters& parameters)
    {
    LOGSTRING("Creator: CCreatorMessages::CreateObexEntryL");

    TInt err = KErrNone;

    // init
    CMsvSession* session = CMsvSession::OpenSyncL(*this);
    CleanupStack::PushL(session);
    CClientMtmRegistry* registry = CClientMtmRegistry::NewL(*session);
    CleanupStack::PushL(registry);

    // get the client mtm and return if it isn't supported in the system        
    CObexClientMtm* clientMtm = NULL;
    TRAP(err, clientMtm = static_cast<CObexClientMtm*>(registry->NewMtmL(aMtm)));
    if (err || !clientMtm)
        {
        iEngine->GetEngineWrapper()->ShowErrorMessage(_L("Obex: Message type module not found"));
        CleanupStack::PopAndDestroy(2);
        User::Leave(KErrNotFound);
        }  
    CleanupStack::PushL(clientMtm);   

    // create a new object to access an existing entry
    CMsvEntry* msvEntry = CMsvEntry::NewL(*session, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
    CleanupStack::PushL(msvEntry);
  
    // define default service
    TMsvId defaultServiceId = 0;
    
    // set folder type
    switch (parameters.iFolderType)
        {
        case EInbox:    { msvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);   break; }
        case EDrafts:   { msvEntry->SetEntryL(KMsvDraftEntryId);              break; }
        case EOutbox:   { msvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);  break; }
        case ESent:     { msvEntry->SetEntryL(KMsvSentEntryId);               break; }
        default:        { User::Panic(_L("Folder Type"), 871);                break; } 
        }
        
    // mtm takes ownership of entry context	
    CleanupStack::Pop(msvEntry);
    clientMtm->SetCurrentEntryL(msvEntry);    
    
    // create a new message
    clientMtm->CreateMessageL(defaultServiceId);

    // get the entry of the message
    TMsvEntry messageEntry = clientMtm->Entry().Entry();

    // set subject
    clientMtm->SetSubjectL( parameters.iMessageSubject->Des() );
    messageEntry.iDescription.Set( parameters.iMessageSubject->Des().Left(KSmsDescriptionLength) );

    // set body, must be empty for obex messages
    clientMtm->Body().Reset();

    // set the details field and
    if (parameters.iFolderType == EInbox)
        {
        SetSenderToEntryDetailsL(messageEntry, parameters, EFalse);
        }        
    else
        {
        SetRecipientToEntryDetailsL(messageEntry, parameters, EFalse);
        }
    
    // set mtm
    messageEntry.iMtm = aMtm;
    messageEntry.iType = KUidMsvMessageEntry;
    messageEntry.iServiceId = KMsvUnknownServiceIndexEntryId;

    // save the changes done above
    clientMtm->Entry().ChangeL(messageEntry);

    // save the message      
    clientMtm->SaveMessageL();
    	
	// final fine tuning
    messageEntry.iDate.HomeTime();
    messageEntry.SetVisible(ETrue);
    messageEntry.SetInPreparation(EFalse);
    if (parameters.iCreateAsUnread)
        {
        messageEntry.SetUnread(ETrue);
        messageEntry.SetNew(ETrue);
        }
    else
        {
        messageEntry.SetUnread(EFalse);
        messageEntry.SetNew(EFalse);
        }
    messageEntry.SetComplete(ETrue);
    clientMtm->Entry().ChangeL(messageEntry);
    
    // save the attachment
    if (parameters.iAttachments->Count() >= 1)
        {
        // create a new entry for the attachment
        TMsvEntry attachTEntry;
        attachTEntry.iType      = KUidMsvAttachmentEntry;
        attachTEntry.iServiceId = KMsvUnknownServiceIndexEntryId;   
        attachTEntry.iMtm       = KUidMsgTypeBt; //save as bt message
        
        msvEntry->CreateL(attachTEntry);
        
        CMsvEntry* attachEntry = session->GetEntryL(attachTEntry.Id());
        clientMtm->SetCurrentEntryL(attachEntry);
                
        // get source file
        TFileName sourceFileName = iEngine->TestDataPathL( (CCreatorEngine::TTestDataPath) parameters.iAttachments->At(0) );

        // get the mime type
        RApaLsSession ls;
        User::LeaveIfError(ls.Connect());
        CleanupClosePushL<RApaLsSession>(ls);
        TUid appUid;
        TDataType mimeType;
        ls.AppForDocument(sourceFileName, appUid, mimeType);
        CleanupStack::PopAndDestroy(); //ls
        
        CAsyncWaiter* waiter = CAsyncWaiter::NewLC();

        // add an attachment to the current message entry
        TRAP(err, clientMtm->AddAttachmentL( sourceFileName, mimeType.Des8(), 0, waiter->iStatus ));
        if (err == KErrNone)
            waiter->StartAndWait();
        else
            LOGSTRING2("Creator: CCreatorMessages::CreateObexEntryL failed to attach %S", &sourceFileName);
        
        CleanupStack::PopAndDestroy(waiter);
        }
    
    // id has been generated, store it for being able to delete
    // only entries created with Creator
    iEntryIds.AppendL( messageEntry.Id() );
    
    CleanupStack::PopAndDestroy(3); //session, registry, clientMtm 


    return err;
    }

//----------------------------------------------------------------------------

void CCreatorMessages::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/) // from MMsvSessionObserver
    {
    }

//----------------------------------------------------------------------------
void CCreatorMessages::SetSenderToEntryDetailsL(TMsvEntry& aMsgEntry, const CMessagesParameters& aParameters, TBool aUseEmailAddress)
    {        
    // Only one sender allowed:
    if( iSenderArray.Count() == 0 )
        {
        GetSendersL(iSenderArray, aParameters, aUseEmailAddress, KUndef );
        }
    if( iSenderArray.Count() > 0 )
        {        
        TInt rnd = iEngine->RandomNumber( iSenderArray.Count() );
        aMsgEntry.iDetails.Set( iSenderArray[rnd]->Des() );        
        }
    else
        {
        aMsgEntry.iDetails.Set(KEmpty);
        }        
    }

//----------------------------------------------------------------------------
void CCreatorMessages::SetRecipientToEntryDetailsL(TMsvEntry& aMsgEntry, const CMessagesParameters& aParameters, TBool aUseEmailAddress)
    {        
    // Only one sender allowed:
    GetAllRecipientsL(iRecipientArray, aParameters, aUseEmailAddress);
    if( iRecipientArray.Count() > 0 )
        {
        const TDesC& temp = iRecipientArray[0]->Des();
        aMsgEntry.iDetails.Set( temp );
        }
    else
        {
        aMsgEntry.iDetails.Set(KEmpty);
        }        
    }

//----------------------------------------------------------------------------
void CCreatorMessages::AddSenderToMtmAddresseeL(CBaseMtm& aMtm, const CMessagesParameters& aParameters, TBool aUseEmailAddress )
    {    
    // Only one sender allowed:
    if( iSenderArray.Count() == 0 )
        {
        GetSendersL(iSenderArray, aParameters, aUseEmailAddress, 1 );
        }
    
    for( TInt i = 0; i < iSenderArray.Count(); ++i )
        {
        aMtm.AddAddresseeL( iSenderArray[i]->Des() );        
        }    
    }

//----------------------------------------------------------------------------
void CCreatorMessages::AddMtmSenderL(CMmsClientMtm& aMtm, const CMessagesParameters& aParameters, TBool aUseEmailAddress )
    {    
    // Only one sender allowed:
    if( iSenderArray.Count() == 0 )
        {
        GetSendersL(iSenderArray, aParameters, aUseEmailAddress, 1 );
        }
    
    if( iSenderArray.Count() > 0 )
        {
        aMtm.SetSenderL( iSenderArray[0]->Des() );        
        }    
    }

//----------------------------------------------------------------------------
void CCreatorMessages::GetSendersL(RPointerArray<HBufC>& aSenderArray, const CMessagesParameters& aParameters, TBool aUseEmailAddress, TInt aMaxNum )
    {
    TInt counter = 0;
    
    if( counter < aMaxNum || aMaxNum == KUndef )
        {
        if( aParameters.iSenderAddress && aParameters.iSenderAddress->Length() > 0 )
            {
            HBufC* temp = HBufC::NewL(aParameters.iSenderAddress->Length());
            CleanupStack::PushL(temp);
            temp->Des().Copy(aParameters.iSenderAddress->Des());
            aSenderArray.AppendL( temp );
            CleanupStack::Pop(); // temp
            ++counter;
            }
        }
            
    if( counter < aMaxNum || aMaxNum == KUndef )
        {
        RPointerArray<HBufC> tempArray;
        CleanupResetAndDestroyPushL( tempArray );
        GetLinkedAddressesL(tempArray, aParameters.iSenderLinkIds, aUseEmailAddress, aParameters.iNumberOfExistingSenders);
        for( TInt i = 0; i < tempArray.Count() && (counter < aMaxNum || aMaxNum == KUndef); ++i )
            {
            HBufC* temp = tempArray[i]->AllocLC();
            aSenderArray.AppendL(temp);
            CleanupStack::Pop(); // temp
            ++counter;
            }
        CleanupStack::PopAndDestroy(); // tempArray
        }    
    }

//----------------------------------------------------------------------------
void CCreatorMessages::AddRecipientsL(CBaseMtm& aMtm, const CMessagesParameters& aParameters, TBool aUseEmailAddress )
    {
    GetAllRecipientsL(iRecipientArray, aParameters, aUseEmailAddress);
    for( TInt i = 0; i < iRecipientArray.Count(); ++i )
        {
        aMtm.AddAddresseeL( iRecipientArray[i]->Des() );        
        }    
    }

//----------------------------------------------------------------------------
void CCreatorMessages::GetAllRecipientsL(RPointerArray<HBufC>& aRecipientArray, const CMessagesParameters& aParameters, TBool aUseEmailAddress )
    {
    
    // If iRecipientAddress is given, add it to the array:
    if( aParameters.iRecipientAddress && aParameters.iRecipientAddress->Length() > 0 )
        {
        HBufC* temp = HBufC::NewL(aParameters.iRecipientAddress->Length());
        CleanupStack::PushL(temp);
        temp->Des().Copy(aParameters.iRecipientAddress->Des());
        aRecipientArray.AppendL( temp );
        CleanupStack::Pop(); // temp
        }
    
    // Add all recipients listed in iRecipientAddressArray:
    for( TInt i = 0; i < aParameters.iRecipientAddressArray.Count(); ++i )
        {
        HBufC* temp = HBufC::NewL(aParameters.iRecipientAddressArray[i]->Length());
        CleanupStack::PushL(temp);
        temp->Des().Copy(aParameters.iRecipientAddressArray[i]->Des());
        aRecipientArray.AppendL( temp );
        CleanupStack::Pop(); // temp        
        }
    
    // Get all linked addresses. 
    GetLinkedAddressesL(aRecipientArray, aParameters.iRecipientLinkIds, aUseEmailAddress, aParameters.iNumberOfExistingRecipients);
    }

/*
 * Get all linked addresses. This method gets addresses that are linked with contact-set id and 
 * existing addresses from the contact database, if required.
 */
void CCreatorMessages::GetLinkedAddressesL(
        RPointerArray<HBufC>& aAddressArray, 
        const RArray<TLinkIdParam>& aLinkIds, 
        TBool aUseEmailAddress,
        TInt /*aNumOfExistingAddresses*/ )
    {
    CCreatorPhonebook* phonebook = dynamic_cast<CCreatorPhonebook*>(iEngine->GetPhonebook());
    User::LeaveIfNull( phonebook );
    CCreatorPhonebookWrapper* phonebookWrapper = phonebook->GetPhonebookWrapper();
    
    for( TInt i = 0; i < aLinkIds.Count(); ++i )
        {
        const CCreatorContactSet& set = ContactLinkCache::Instance()->ContactSet(aLinkIds[i].iLinkId);
        const RArray<TUint32> links = set.ContactLinks();//ContactLinkCache::Instance()->ContactSets();//set.ContactLinks();
        TInt numberOfExplicitLinks = links.Count(); // Number of defined contacts in contact-set
        for( TInt j = 0; j < numberOfExplicitLinks; ++j )
            {
            TBuf<128> name;
            TBuf<128> email;
            TBuf<128> phoneNumber;
            phonebookWrapper->GetContactDetailsL( links[j], name, phoneNumber, email );
            HBufC* address = ( aUseEmailAddress ? email.AllocLC() : phoneNumber.AllocLC() );
            aAddressArray.AppendL( address );
            CleanupStack::Pop( address );
            }
        }
    }

//----------------------------------------------------------------------------
void CCreatorMessages::DeleteAllL()
    {
    LOGSTRING("Creator: CCreatorMessages::DeleteAllL");
    DeleteAllMessagesL( EFalse );
    }

//----------------------------------------------------------------------------
void CCreatorMessages::DeleteAllMessagesL( TBool aOnlyCreatedWithCreator )
    {
    LOGSTRING("Creator: CCreatorMessages::DeleteAllMessagesL");
    TInt err( KErrNone );
    
    // init
    CMsvSession* session = CMsvSession::OpenSyncL(*this);
    CleanupStack::PushL( session );
    CClientMtmRegistry* registry = CClientMtmRegistry::NewL(*session);
    CleanupStack::PushL( registry );

    // get the client mtm and return if it isn't supported in the system        
    CSmsClientMtm* clientMtm( NULL );
    TRAP( err, clientMtm = static_cast<CSmsClientMtm*>( registry->NewMtmL( KUidMsgTypeSMS ) ) );
    if ( err || !clientMtm )
        {
        iEngine->GetEngineWrapper()->ShowErrorMessage( _L( "SMS: Message type module not found" ) );
        CleanupStack::PopAndDestroy( 2 );
        User::Leave( KErrNotFound );
        }
    CleanupStack::PushL( clientMtm );
    
    TRAP_IGNORE( DeleteAllFromFolderL( KMsvGlobalInBoxIndexEntryId, session, clientMtm, aOnlyCreatedWithCreator ) );
    TRAP_IGNORE( DeleteAllFromFolderL( KMsvDraftEntryId, session, clientMtm, aOnlyCreatedWithCreator ) );
    TRAP_IGNORE( DeleteAllFromFolderL( KMsvGlobalOutBoxIndexEntryId, session, clientMtm, aOnlyCreatedWithCreator ) );
    TRAP_IGNORE( DeleteAllFromFolderL( KMsvSentEntryId, session, clientMtm, aOnlyCreatedWithCreator ) );
    
    // reset must be done here, because iEntryIds is stored in destructor
    iEntryIds.Reset();
    
    // all entries deleted, remove the Messages related registry
    iEngine->RemoveStoreL( KUidDictionaryUidMessages );

    CleanupStack::PopAndDestroy( clientMtm );
    CleanupStack::PopAndDestroy( registry );
    CleanupStack::PopAndDestroy( session );
    }

//----------------------------------------------------------------------------
void CCreatorMessages::DeleteAllFromFolderL( const TMsvId aContext, 
                                             CMsvSession* aSession,
                                             CSmsClientMtm* aClientMtm,
                                             TBool aOnlyCreatedWithCreator )
    {
    LOGSTRING("Creator: CCreatorMessages::DeleteAllFromFolderL");
    
    TMsvSelectionOrdering sort;
    sort.SetShowInvisibleEntries( ETrue );

    CMsvEntry* inboxContext = CMsvEntry::NewL( *aSession, aContext, sort );
    CleanupStack::PushL( inboxContext );

    CMsvEntrySelection* entries = inboxContext->ChildrenL();
    CleanupStack::PushL( entries );

    TInt msgCount = entries->Count();
    for ( TInt i = 0; i < entries->Count(); i++)
        {
        TMsvId entryID = entries->At(i);
        aClientMtm->SwitchCurrentEntryL( entryID );

        if ( !aOnlyCreatedWithCreator || iEntryIds.Find( entryID ) != KErrNotFound )
            {
            CMsvEntry* entry = aSession->GetEntryL( (*entries)[i] );
            CleanupStack::PushL( entry );

            entry->DeleteL( entries->At(i) );
            CleanupStack::PopAndDestroy( entry );            
            }
        }
    
    CleanupStack::PopAndDestroy( entries );
    CleanupStack::PopAndDestroy( inboxContext );
    }

//----------------------------------------------------------------------------
void CCreatorMessages::DeleteAllCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorMessages::DeleteAllCreatedByCreatorL");
    
    iEntryIds.Reset();
    
    // fetch ids of entries created by Creator
    iEngine->ReadEntryIdsFromStoreL( iEntryIds, KUidDictionaryUidMessages );
    
    // delete them
    DeleteAllMessagesL( ETrue );
    }

CRecipientInfo::CRecipientInfo() 
    {
    
    }

CRecipientInfo::~CRecipientInfo()
    {
    delete iPhoneNumber;
    delete iEmailAddress;
    }

void CRecipientInfo::SetPhoneNumber(HBufC* aPhone)
    {
    delete iPhoneNumber;    
    iPhoneNumber = aPhone;    
    }

void CRecipientInfo::SetEmailAddress(HBufC* aEmail)
    {
    delete iEmailAddress;    
    iEmailAddress = aEmail;    
    }

const HBufC* CRecipientInfo::PhoneNumber() const
    {
    return iPhoneNumber;
    }

const HBufC* CRecipientInfo::EmailAddress() const
    {
    return iEmailAddress;
    }
