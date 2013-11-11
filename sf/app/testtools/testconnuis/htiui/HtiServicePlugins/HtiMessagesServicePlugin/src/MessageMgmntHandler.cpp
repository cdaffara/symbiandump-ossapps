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
* Description:  Functional implementation of HtiMessagesServicePlugin service
*
*/


// INCLUDE FILES
#include "HtiMessagesServicePlugin.h"
#include "MessageMgmntHandler.h"

#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>
#include <mtclreg.h>
#include <smscmds.h>
#include <smtcmtm.h>
#include <smuthdr.h>
#include <smsclnt.h>
#include <utf.h>
#include <mmsclient.h>
#include <mmsconst.h>
#include <miutset.h>
#include <irmsgtypeuid.h>
#include <btmsgtypeuid.h>
#include <biouids.h>
#include <apgcli.h>
#include <apmstd.h>
#include <bautils.h>
#include <obexclientmtm.h>
#include <cmsvmimeheaders.h>
#include <mmsvattachmentmanager.h>

// CONSTANTS
_LIT8( KErrorMissingCommand,        "Command was not given - message was empty" );
_LIT8( KErrorUnrecognizedCommand,   "Unrecognized command" );
_LIT8( KErrorInvalidParameters,     "Invalid command parameters");
_LIT8( KErrorTooLongSmsBody,        "Too long SMS body" );
_LIT8( KErrorInvalidId,             "Invalid SMS id parameter" );
_LIT8( KErrorInvalidFolder,         "Invalid folder parameter" );
_LIT8( KErrorItemNotFound,          "Item not found" );
_LIT8( KErrorFailedDelete,          "Failed to delete item" );
_LIT8( KErrorNotSupported,          "Not supported" );
_LIT8( KErrorSmsSettingNotDefined,  "SMS settings not defined" );
_LIT8( KErrorMmsSettingNotDefined,  "MMS settings not defined" );
_LIT8( KErrorMailboxNotDefined,     "Mailbox not defined" );
_LIT8( KErrorMsgTypeNotFound,       "Message type module not found" );
_LIT8( KErrorMsgStoreOpenFailed,    "Could not open message store" );
_LIT8( KErrorRfsConnectFailed,      "Could not connect to file server session" );
_LIT8( KErrorAttachmentNotFound,    "Attachment not found" );
_LIT8( KErrorInvalidFolderForSmartMsg, "Only inbox allowed for smart messages" );

const static TInt KAddSmsCmdMinLength        = 7;
const static TInt KAddMmsOrEmailCmdMinLength = 8;
const static TInt KAddObexMsgCmdMinLength    = 6;
const static TInt KAddSmartMsgCmdMinLength   = 11;
const static TInt KAddAudioCmdMinLength      = 10;

// ----------------------------------------------------------------------------
CMessageMgmntHandler* CMessageMgmntHandler::NewL()
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::NewL" );
    CMessageMgmntHandler* self = new (ELeave) CMessageMgmntHandler();
    CleanupStack::PushL ( self );
    self->ConstructL();
    CleanupStack::Pop();
    HTI_LOG_FUNC_OUT( "CMessageMgmntHandler::NewL: Done" );
    return self;
    }

// ----------------------------------------------------------------------------
CMessageMgmntHandler::CMessageMgmntHandler()
    {
    }

// ----------------------------------------------------------------------------
CMessageMgmntHandler::~CMessageMgmntHandler()
    {
    delete iMtmReg;
    delete iSession;
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::ConstructL()
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::ConstructL" );
    iSession = CMsvSession::OpenSyncL( *this );
    iMtmReg = CClientMtmRegistry::NewL( *iSession );
    HTI_LOG_FUNC_OUT( "CMessageMgmntHandler::ConstructL: Done" );
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::SetDispatcher( MHtiDispatcher* aDispatcher )
    {
    iDispatcher = aDispatcher;
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::ProcessMessageL( const TDesC8& aMessage,
                                            THtiMessagePriority /*aPriority*/ )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::ProcessMessageL" );
    HTI_LOG_FORMAT( "Msg length: %d", aMessage.Length() );

    if ( aMessage.Length() == 0 )
        {
        SendErrorMessageL( KErrArgument, KErrorMissingCommand );
        return;
        }

    switch ( aMessage[0] )
        {
        case CHtiMessagesServicePlugin::EAddSms:
            HTI_LOG_TEXT( "Add SMS" );
            HandleCreateSmsL( aMessage.Right( aMessage.Length() - 1 ) );
            break;

        case CHtiMessagesServicePlugin::EAddMms:
        case CHtiMessagesServicePlugin::EAddAudioMsg: // special MMS sub type
            HTI_LOG_TEXT( "Add MMS" );
            HandleCreateMmsL( aMessage );
            break;

        case CHtiMessagesServicePlugin::EAddEmail:
            HTI_LOG_TEXT( "Add Email" );
            HandleCreateEmailL( aMessage );
            break;

        case CHtiMessagesServicePlugin::EAddIrMsg:
            HTI_LOG_TEXT( "Add IR msg" );
            HandleCreateObexMsgL( aMessage.Right( aMessage.Length() - 1 ),
                                  TUid::Uid( KUidMsgTypeIrTInt32 ),
                                  KUidMsgTypeIrUID );
            break;

        case CHtiMessagesServicePlugin::EAddBtMsg:
            HTI_LOG_TEXT( "Add BT msg" );
            HandleCreateObexMsgL( aMessage.Right( aMessage.Length() - 1 ),
                                  TUid::Uid( KUidMsgTypeBtTInt32 ),
                                  KUidMsgTypeBt );
            break;

        case CHtiMessagesServicePlugin::EAddSmartMsg:
            HTI_LOG_TEXT( "Add smart msg" );
            HandleCreateSmartMsgL( aMessage.Right( aMessage.Length() - 1 ) );
            break;

        case CHtiMessagesServicePlugin::EDeleteMessage:
            HTI_LOG_TEXT( "Delete message" );
            HandleDeleteMessageL( aMessage.Right( aMessage.Length() - 1 ) );
            break;

        case CHtiMessagesServicePlugin::EDeleteFolderContent:
            HTI_LOG_TEXT( "Delete messages" );
            HandleDeleteMessagesL( aMessage.Right( aMessage.Length() - 1 ) );
            break;

        default:
            HTI_LOG_TEXT( "Unknown command" );
            SendErrorMessageL( KErrUnknown, KErrorUnrecognizedCommand );
            break;
        }

    HTI_LOG_FUNC_OUT( "CMessageMgmntHandler::ProcessMessageL: Done" );
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::HandleCreateSmsL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::HandleSmsImportFuncL" );

    if ( ValidateAddSmsCommand( aData ) )
        {
        TInt position( 0 );
        HBufC16* fromTo = ExtractDesLC( aData, position, 1 );
        HBufC16* description = ExtractDesLC( aData, position, 1 );
        HBufC16* body = ExtractDesLC( aData, position, 2 );
        TBool isNew = (TBool)aData[position];
        TBool isUnread = (TBool)aData[position+1];
        TFolder folder = (TFolder)aData[position+2];

        CSmsClientMtm* smsMtm = NULL;
        TRAPD( err, smsMtm = ( CSmsClientMtm* )iMtmReg->NewMtmL(
                KUidMsgTypeSMS ) );
        if ( err || !smsMtm )
            {
            HTI_LOG_TEXT( "SMS message type module not found" );
            SendErrorMessageL( KErrNotFound, KErrorMsgTypeNotFound );
            CleanupStack::PopAndDestroy( body );
            CleanupStack::PopAndDestroy( description );
            CleanupStack::PopAndDestroy( fromTo );
            return;
            }
        CleanupStack::PushL( smsMtm );

        CMsvEntry* entry = CMsvEntry::NewL( *iSession,
                                            KMsvGlobalInBoxIndexEntryId,
                                            TMsvSelectionOrdering() );
        CleanupStack::PushL( entry );

        // get the default service
        TMsvId defaultServiceId = 0;
        TRAP( err, defaultServiceId = smsMtm->DefaultServiceL() );
        if ( err )
            {
            HTI_LOG_FORMAT( "Could not get default service, err: %d", err );
            SendErrorMessageL( err, KErrorSmsSettingNotDefined );
            CleanupStack::PopAndDestroy( entry );
            CleanupStack::PopAndDestroy( smsMtm );
            CleanupStack::PopAndDestroy( body );
            CleanupStack::PopAndDestroy( description );
            CleanupStack::PopAndDestroy( fromTo );
            return;
            }

        // map the folder parameter to folder id
        TMsvId folderId = KMsvGlobalInBoxIndexEntryId;
        TRAP( err, folderId = MapFolderToIdL( folder ) );
        if ( err )
            {
            HTI_LOG_FORMAT( "Invalid folder: %d", folder );
            SendErrorMessageL( err, KErrorInvalidFolder );
            CleanupStack::PopAndDestroy( entry );
            CleanupStack::PopAndDestroy( smsMtm );
            CleanupStack::PopAndDestroy( body );
            CleanupStack::PopAndDestroy( description );
            CleanupStack::PopAndDestroy( fromTo );
            return;
            }
        entry->SetEntryL( folderId );

        // mtm takes ownership of entry context
        smsMtm->SetCurrentEntryL( entry );
        CleanupStack::Pop( entry );

        // create a new message
        smsMtm->CreateMessageL( defaultServiceId );

        if ( folder == EInbox )
            {
            CSmsHeader* smsHeader = &( smsMtm->SmsHeader() );
            delete smsHeader;
            smsHeader = NULL;
            smsHeader = CSmsHeader::NewL( CSmsPDU::ESmsDeliver, smsMtm->Body() );
            smsHeader->SetFromAddressL( fromTo->Des() );
            }
        else
            {
            smsMtm->AddAddresseeL( fromTo->Des() );

            // set delivery settings
            CSmsSettings* sendOptions = CSmsSettings::NewL();
            CleanupStack::PushL( sendOptions );
            sendOptions->CopyL( smsMtm->ServiceSettings() );
            sendOptions->SetDelivery( ESmsDeliveryImmediately );

            CSmsHeader* smsHeader = &( smsMtm->SmsHeader() );
            smsHeader->SetSmsSettingsL( *sendOptions );
            CleanupStack::PopAndDestroy( sendOptions );
            }



        // set body
        smsMtm->Body().Reset();
        smsMtm->Body().InsertL( 0, *body );

        // save the message
        smsMtm->SaveMessageL();

        // get the entry of the message
        TMsvEntry tentry = smsMtm->Entry().Entry();

        // set the details field
        tentry.iDetails.Set( fromTo->Des() );

        // set the description field if it is given.
        // (with no description the beginning of the message body
        //  is used as a description)
        if ( description->Length() > 0 )
            {
            tentry.iDescription.Set( description->Des() );
            }

        // final fine tuning
        tentry.SetAttachment( EFalse );
        tentry.iDate.UniversalTime();
        tentry.SetVisible( ETrue );
        tentry.SetInPreparation( EFalse );
        tentry.SetUnread( isUnread );
        tentry.SetNew( isNew );
        tentry.SetComplete( ETrue );
        tentry.SetSendingState( KMsvSendStateWaiting );
        tentry.iServiceId = defaultServiceId;
        tentry.iRelatedId = 0;
        if ( folder == EInbox )
            {
            tentry.SetReadOnly( ETrue );
            }

        smsMtm->Entry().ChangeL( tentry );

        // send the message, if it is in outbox
        if ( folder == EOutbox )
            {
            CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
            CleanupStack::PushL( selection );
            selection->AppendL( tentry.Id() );

            TBuf8<1> dummyParameter;
            CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
            CMsvOperation* op = smsMtm->InvokeAsyncFunctionL(
                    ESmsMtmCommandScheduleCopy, *selection,
                    dummyParameter, waiter->iStatus );
            CleanupStack::PushL( op );
            waiter->Start();
            CActiveScheduler::Start();
            CleanupStack::PopAndDestroy( op );
            CleanupStack::PopAndDestroy( waiter );
            CleanupStack::PopAndDestroy( selection );
            }

        CleanupStack::PopAndDestroy( smsMtm );
        CleanupStack::PopAndDestroy( body );
        CleanupStack::PopAndDestroy( description );
        CleanupStack::PopAndDestroy( fromTo );

        TInt32 id = tentry.Id();
        TBuf8<8> idStr;
        idStr.Copy( ( TUint8* )( &id ), sizeof( id ) );
        SendOkMsgL( idStr );
        }

    HTI_LOG_FUNC_OUT("CMessageMgmntHandler::HandleSmsImportFuncL: Done");
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::HandleCreateMmsL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::HandleCreateMmsL" );

    if ( !ValidateAddMmsOrAddEmailCommand( aData ) )
        {
        // Error message has been sent from validation method.
        return;
        }

    // parse the parameters
    TInt position( 0 );
    TInt cmdCode = aData[position];
    position++;
    HBufC16* fromTo = ExtractDesLC( aData, position, 1 );
    HBufC16* description = ExtractDesLC( aData, position, 1 );
    HBufC8* body = NULL;
    if ( cmdCode == CHtiMessagesServicePlugin::EAddMms )
        {
        body = ExtractDes8LC( aData, position, 2 );
        }
    else // Audio msg does not have body text
        {
        body = HBufC8::NewLC( 0 );
        }
    HBufC16* attPath = ExtractDesLC( aData, position, 1 );
    TBool isNew = (TBool)aData[position];
    TBool isUnread = (TBool)aData[position+1];
    TFolder folder = (TFolder)aData[position+2];
    TInt extraAttNum;
    position += 3;
    TInt len = aData.Length();
    if( aData.Length() > position )
        {
        extraAttNum = (TInt)aData[position];
        }
    else
        {
        extraAttNum = 0;
        }
    position++;

    HTI_LOG_TEXT( "Creating MMS Client MTM" );
    CMmsClientMtm* mmsMtm = NULL;
    TRAPD( err , mmsMtm = ( CMmsClientMtm* )iMtmReg->NewMtmL(
            KUidMsgTypeMultimedia ) );
    if ( err || !mmsMtm )
        {
        HTI_LOG_TEXT( "MMS message type module not found" );
        SendErrorMessageL( KErrNotFound, KErrorMsgTypeNotFound );
        CleanupStack::PopAndDestroy( attPath );
        CleanupStack::PopAndDestroy( body );
        CleanupStack::PopAndDestroy( description );
        CleanupStack::PopAndDestroy( fromTo );
        return;
        }
    CleanupStack::PushL( mmsMtm );

    HTI_LOG_TEXT( "Creating MMS Client MTM" );
    CMsvEntry* entry = CMsvEntry::NewL( *iSession,
                                        KMsvGlobalInBoxIndexEntryId,
                                        TMsvSelectionOrdering() );
    CleanupStack::PushL( entry );

    // get the default service
    TMsvId defaultServiceId = 0;
    TRAP( err, defaultServiceId = mmsMtm->DefaultServiceL() );
    if ( err )
        {
        HTI_LOG_FORMAT( "Could not get default service, err: %d", err );
        SendErrorMessageL( err, KErrorMmsSettingNotDefined );
        CleanupStack::PopAndDestroy( entry );
        CleanupStack::PopAndDestroy( mmsMtm );
        CleanupStack::PopAndDestroy( attPath );
        CleanupStack::PopAndDestroy( body );
        CleanupStack::PopAndDestroy( description );
        CleanupStack::PopAndDestroy( fromTo );
        return;
        }

    // map the folder parameter to folder id
    TMsvId folderId = KMsvGlobalInBoxIndexEntryId;
    TRAP( err, folderId = MapFolderToIdL( folder ) );
    if ( err )
        {
        HTI_LOG_FORMAT( "Invalid folder: %d", folder );
        SendErrorMessageL( err, KErrorInvalidFolder );
        CleanupStack::PopAndDestroy( entry );
        CleanupStack::PopAndDestroy( mmsMtm );
        CleanupStack::PopAndDestroy( attPath );
        CleanupStack::PopAndDestroy( body );
        CleanupStack::PopAndDestroy( description );
        CleanupStack::PopAndDestroy( fromTo );
        return;
        }
    entry->SetEntryL( folderId );

    // mtm takes ownership of entry context
    mmsMtm->SetCurrentEntryL( entry );
    CleanupStack::Pop( entry );

    HTI_LOG_TEXT( "Creating MMS..." );
    mmsMtm->CreateMessageL( defaultServiceId );
    mmsMtm->SetMessageClass( EMmsClassPersonal );
    mmsMtm->SetExpiryInterval( 86400 );
    mmsMtm->SetDeliveryTimeInterval( 0 );
    mmsMtm->SetMessagePriority( EMmsPriorityNormal );
    mmsMtm->SetSenderVisibility( EMmsMaximumSenderVisibility );
    mmsMtm->SetDeliveryReport( EMmsDeliveryReportNo );
    mmsMtm->SetReadReply( EMmsReadReplyYes );

    if ( description->Length() > 0 )
        {
        mmsMtm->SetSubjectL( description->Des() );
        }

    if ( folder == EInbox )
        {
        mmsMtm->SetSenderL( fromTo->Des() );
        }
    else
        {
        mmsMtm->AddAddresseeL( fromTo->Des() );
        }

    // get an access to the message store
    HTI_LOG_TEXT( "Getting message store..." );
    CMsvStore* store = NULL;
    TRAP( err, store = entry->EditStoreL() );
    if ( err )
        {
        HTI_LOG_FORMAT( "Could not get access to message store, err: %d", err );
        SendErrorMessageL( err, KErrorMsgStoreOpenFailed );
        CleanupStack::PopAndDestroy( mmsMtm );
        CleanupStack::PopAndDestroy( attPath );
        CleanupStack::PopAndDestroy( body );
        CleanupStack::PopAndDestroy( description );
        CleanupStack::PopAndDestroy( fromTo );
        return;
        }
    CleanupStack::PushL( store );

    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    // set body attachment only for normal MMS - audio message doesn't have body
    if ( cmdCode == CHtiMessagesServicePlugin::EAddMms )
        {
        // Set the message body as attachment
        // Use UTF-8 as charset because MMS created with MMS editor seems to
        // save text attachments also as UTF-8.
        HTI_LOG_TEXT( "Setting body..." );
        CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
        CleanupStack::PushL( mimeHeaders );
        mimeHeaders->SetContentTypeL( _L8( "text" ) );
        mimeHeaders->SetContentSubTypeL( _L8( "plain" ) );
        mimeHeaders->SetMimeCharset( KMmsUtf8 );
        mimeHeaders->SetSuggestedFilenameL( _L( "body.txt" ) );

        // ownership of bodyAttachment will be transferred
        CMsvAttachment* bodyAttachment = CMsvAttachment::NewL(
                CMsvAttachment::EMsvFile );
        CleanupStack::PushL( bodyAttachment );
        bodyAttachment->SetAttachmentNameL( _L( "body.txt" ) );
        bodyAttachment->SetMimeTypeL( _L8( "text/plain" ) );
        mimeHeaders->StoreL( *bodyAttachment );

        RFile textFile;
        CleanupClosePushL( textFile );
        CWaiter* waiter = CWaiter::NewLC();
        attachMan.CreateAttachmentL( _L( "body.txt" ), textFile,
                bodyAttachment, waiter->iStatus );
        waiter->StartAndWait();
        CleanupStack::PopAndDestroy( waiter );

        // write the UTF-8 body data to attachment file
        textFile.Write( *body );
        CleanupStack::PopAndDestroy(); // textFile
        CleanupStack::Pop( bodyAttachment ); // ownership transfered
        CleanupStack::PopAndDestroy( mimeHeaders );
        }

    // get the entry of the message
    TMsvEntry tentry = mmsMtm->Entry().Entry();

    // set the details field
    tentry.iDetails.Set( *fromTo );

    // set the description field
    if ( description->Length() > 0 )
        {
        tentry.iDescription.Set( description->Left( KMmsMaxDescription ) );
        }
    else
        {
        TBuf<KMmsMaxDescription> descr;
        CnvUtfConverter::ConvertToUnicodeFromUtf8( descr, *body );
        tentry.iDescription.Set( descr );
        }

    // if this is audio message, set the bio type uid
    if ( cmdCode == CHtiMessagesServicePlugin::EAddAudioMsg )
        {
        tentry.iBioType = KUidMsgSubTypeMmsAudioMsg.iUid;
        }

    // handle attachment
    TBool attachmentsExist = EFalse;
	if ( attPath->Length() > 0 )
		{
		HTI_LOG_TEXT( "Handling attachment..." );
		// check that attachment exists
		RFs fsSession;
		if ( fsSession.Connect() != KErrNone )
			{
			HTI_LOG_FORMAT( "Error in connecting to file server session: %d", err );
			SendErrorMessageL( KErrCouldNotConnect, KErrorRfsConnectFailed );
			CleanupStack::PopAndDestroy( store );
			CleanupStack::PopAndDestroy( mmsMtm );
			CleanupStack::PopAndDestroy( attPath );
			CleanupStack::PopAndDestroy( body );
			CleanupStack::PopAndDestroy( description );
			CleanupStack::PopAndDestroy( fromTo );
			return;
			}

		TBool fileExists = BaflUtils::FileExists( fsSession, attPath->Des() );
		fsSession.Close();
		if ( !fileExists )
			{
			HTI_LOG_TEXT( "Attachment file not found" );
			SendErrorMessageL( KErrPathNotFound, KErrorAttachmentNotFound );
			store->RevertL();
			CleanupStack::PopAndDestroy( store );
			CleanupStack::PopAndDestroy( mmsMtm );
			CleanupStack::PopAndDestroy( attPath );
			CleanupStack::PopAndDestroy( body );
			CleanupStack::PopAndDestroy( description );
			CleanupStack::PopAndDestroy( fromTo );
			return;
			}
		else
			{
			// save the attachment
			TParse parser;
			parser.Set( *attPath, NULL, NULL);
			TFileName shortFileName = parser.NameAndExt();

			// get the mime type
			RApaLsSession ls;
			User::LeaveIfError( ls.Connect() );
			CleanupClosePushL( ls );
			TUid appUid;
			TDataType dataType;
			ls.AppForDocument( *attPath, appUid, dataType );
			CleanupStack::PopAndDestroy(); // ls
			TPtrC8 mimeType = dataType.Des8();

			// attachment settings
			// ownership of attachment will be transferred
			CMsvAttachment* attachment = CMsvAttachment::NewL(
					CMsvAttachment::EMsvFile );
			attachment->SetAttachmentNameL( shortFileName );
			attachment->SetMimeTypeL( mimeType );

			// save
			CWaiter* waiter = CWaiter::NewLC();
			attachMan.AddAttachmentL( *attPath, attachment, waiter->iStatus );
			waiter->StartAndWait();
			CleanupStack::PopAndDestroy( waiter );
			attachmentsExist = ETrue;
			}
		}

    do
		{
		HBufC16* attPath2;
       if(extraAttNum-- > 0)
            {
            attPath2 = ExtractDesLC( aData, position, 1 );
            }
        else
            {
            break;
            }

		if ( attPath2->Length() > 0 )
			{
			HTI_LOG_TEXT( "Handling attachment..." );
			// check that attachment exists
			RFs fsSession;
			if ( fsSession.Connect() != KErrNone )
				{
				HTI_LOG_FORMAT( "Error in connecting to file server session: %d", err );
				SendErrorMessageL( KErrCouldNotConnect, KErrorRfsConnectFailed );
				CleanupStack::PopAndDestroy( store );
				CleanupStack::PopAndDestroy( mmsMtm );
				CleanupStack::PopAndDestroy( attPath );
				CleanupStack::PopAndDestroy( body );
				CleanupStack::PopAndDestroy( description );
				CleanupStack::PopAndDestroy( fromTo );
				CleanupStack::PopAndDestroy( attPath2 );
				return;
				}
	
			TBool fileExists = BaflUtils::FileExists( fsSession, attPath2->Des() );
			fsSession.Close();
			if ( !fileExists )
				{
				HTI_LOG_TEXT( "Attachment file not found" );
				SendErrorMessageL( KErrPathNotFound, KErrorAttachmentNotFound );
				store->RevertL();
				CleanupStack::PopAndDestroy( store );
				CleanupStack::PopAndDestroy( mmsMtm );
				CleanupStack::PopAndDestroy( attPath );
				CleanupStack::PopAndDestroy( body );
				CleanupStack::PopAndDestroy( description );
				CleanupStack::PopAndDestroy( fromTo );
				CleanupStack::PopAndDestroy( attPath2 );
				return;
				}
			else
				{
				// save the attachment
				TParse parser;
				parser.Set( *attPath2, NULL, NULL);
				TFileName shortFileName = parser.NameAndExt();
	
				// get the mime type
				RApaLsSession ls;
				User::LeaveIfError( ls.Connect() );
				CleanupClosePushL( ls );
				TUid appUid;
				TDataType dataType;
				ls.AppForDocument( *attPath2, appUid, dataType );
				CleanupStack::PopAndDestroy(); // ls
				TPtrC8 mimeType = dataType.Des8();
	
				// attachment settings
				// ownership of attachment will be transferred
				CMsvAttachment* attachment = CMsvAttachment::NewL(
						CMsvAttachment::EMsvFile );
				attachment->SetAttachmentNameL( shortFileName );
				attachment->SetMimeTypeL( mimeType );
	
				// save
				CWaiter* waiter = CWaiter::NewLC();
				attachMan.AddAttachmentL( *attPath2, attachment, waiter->iStatus );
				waiter->StartAndWait();
				CleanupStack::PopAndDestroy( waiter );
				attachmentsExist = ETrue;
				}

			CleanupStack::PopAndDestroy( attPath2 );
			}
		} while(ETrue);
    // save the changes made to the message store
    store->CommitL();
    CleanupStack::PopAndDestroy( store );

    // save the message
    mmsMtm->SaveMessageL();

    // final fine tuning
    tentry.SetAttachment( attachmentsExist );
    tentry.iDate.UniversalTime();
    tentry.SetVisible( ETrue );
    tentry.SetInPreparation( EFalse );
    if ( folder == EDrafts )
        {
        tentry.SetReadOnly( EFalse );
        }
    else
        {
        tentry.SetReadOnly( ETrue );
        }
    tentry.SetUnread( isUnread );
    tentry.SetNew( isNew );
    tentry.SetComplete( ETrue );
    tentry.SetSendingState( KMsvSendStateWaiting );
    tentry.iServiceId = defaultServiceId;
    tentry.iRelatedId = 0;
    tentry.iMtmData1 = KMmsMessageMRetrieveConf | KMmsMessageMobileTerminated;

    mmsMtm->Entry().ChangeL( tentry );

    HTI_LOG_TEXT( "MMS created and ready" );

    // send the message, if it is in outbox
    if ( folder == EOutbox )
        {
        HTI_LOG_TEXT( "MMS is in Outbox, sending it..." );

        CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
        CleanupStack::PushL( selection );
        selection->AppendL( tentry.Id() );

        CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
        CMsvOperation* op = mmsMtm->SendL( *selection,
                                           waiter->iStatus,
                                           tentry.iDate );
        CleanupStack::PushL( op );
        waiter->Start();
        CActiveScheduler::Start();
        CleanupStack::PopAndDestroy( op );
        CleanupStack::PopAndDestroy( waiter );
        CleanupStack::PopAndDestroy( selection );
        }

    HTI_LOG_TEXT( "Cleaning up" );
    CleanupStack::PopAndDestroy( mmsMtm );
    CleanupStack::PopAndDestroy( attPath );
    CleanupStack::PopAndDestroy( body );
    CleanupStack::PopAndDestroy( description );
    CleanupStack::PopAndDestroy( fromTo );

    // send the message id back
    TInt32 id = tentry.Id();
    TBuf8<8> idStr;
    idStr.Copy( ( TUint8* )( &id ), sizeof( id ) );
    SendOkMsgL( idStr );

    HTI_LOG_FUNC_OUT("CMessageMgmntHandler::HandleCreateMmsL: Done");
    }


// ----------------------------------------------------------------------------
void CMessageMgmntHandler::HandleCreateEmailL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::HandleCreateEmailL" );

    if ( ValidateAddMmsOrAddEmailCommand( aData ) )
        {
        // parse the parameters
        TInt position( 1 ); // position 0 is command code
        HBufC16* fromTo = ExtractDesLC( aData, position, 1 );
        HBufC16* description = ExtractDesLC( aData, position, 1 );
        HBufC16* body = ExtractDesLC( aData, position, 2 );
        HBufC16* attPath = ExtractDesLC( aData, position, 1 );
        TBool isNew = (TBool)aData[position];
        TBool isUnread = (TBool)aData[position+1];
        TFolder folder = (TFolder)aData[position+2];
	    TInt extraAttNum;
	    position += 3;
	    TInt len = aData.Length();
	    if( aData.Length() > position )
	        {
	        extraAttNum = (TInt)aData[position];
	        }
	    else
	        {
	        extraAttNum = 0;
	        }
	    position++;

        HTI_LOG_TEXT( "Creating SMTP Client MTM" );
        CSmtpClientMtm* smtpMtm = NULL;
        TRAPD( err, smtpMtm = ( CSmtpClientMtm* )iMtmReg->NewMtmL(
                KUidMsgTypeSMTP ) );
        if ( err || !smtpMtm )
            {
            HTI_LOG_TEXT( "SMTP message type module not found" );
            SendErrorMessageL( KErrNotFound, KErrorMsgTypeNotFound );
            CleanupStack::PopAndDestroy( attPath );
            CleanupStack::PopAndDestroy( body );
            CleanupStack::PopAndDestroy( description );
            CleanupStack::PopAndDestroy( fromTo );
            return;
            }
        CleanupStack::PushL( smtpMtm );

        HTI_LOG_TEXT( "Creating a new CMsvEntry" );
        CMsvEntry* entry = CMsvEntry::NewL( *iSession,
                                            KMsvGlobalInBoxIndexEntryId,
                                            TMsvSelectionOrdering() );
        CleanupStack::PushL( entry );

        // get the default service
        HTI_LOG_TEXT( "Getting the default service" );
        TMsvId defaultServiceId = 0;
        TRAP( err, defaultServiceId = smtpMtm->DefaultServiceL() );
        if ( err )
            {
            HTI_LOG_FORMAT( "Could not get default service, err: %d", err );
            SendErrorMessageL( err, KErrorMailboxNotDefined );
            CleanupStack::PopAndDestroy( entry );
            CleanupStack::PopAndDestroy( smtpMtm );
            CleanupStack::PopAndDestroy( attPath );
            CleanupStack::PopAndDestroy( body );
            CleanupStack::PopAndDestroy( description );
            CleanupStack::PopAndDestroy( fromTo );
            return;
            }

        // map the folder parameter to folder id
        HTI_LOG_TEXT( "Mapping the folder parameter to folder id" );
        TMsvId folderId = KMsvGlobalInBoxIndexEntryId;
        TRAP( err, folderId = MapFolderToIdL( folder ) );
        if ( err )
            {
            HTI_LOG_FORMAT( "Invalid folder: %d", folder );
            SendErrorMessageL( err, KErrorInvalidFolder );
            CleanupStack::PopAndDestroy( entry );
            CleanupStack::PopAndDestroy( smtpMtm );
            CleanupStack::PopAndDestroy( attPath );
            CleanupStack::PopAndDestroy( body );
            CleanupStack::PopAndDestroy( description );
            CleanupStack::PopAndDestroy( fromTo );
            return;
            }
        entry->SetEntryL( folderId );

        // mtm takes ownership of entry context
        smtpMtm->SetCurrentEntryL( entry );
        CleanupStack::Pop( entry );

        // create a message and set subject and body
        smtpMtm->CreateMessageL( defaultServiceId );
        smtpMtm->SetSubjectL( description->Des() );
        smtpMtm->Body().Reset();
        smtpMtm->Body().InsertL( 0, body->Des() );

        // get the entry of the message
        TMsvEntry tentry = smtpMtm->Entry().Entry();

        // add addressee
        smtpMtm->AddAddresseeL( fromTo->Des() );
        tentry.iDetails.Set( fromTo->Des() );

        // If creating to Inbox use other than KUidMsgTypeSMTP so that the
        // mail displays "from" field and not "to" field.
        if ( folder == EInbox )
            {
            tentry.iMtm = KUidMsgTypeIMAP4;
            }

        // set the description field same as the message subject
        tentry.iDescription.Set( description->Des() );

        // save the changes done above
        smtpMtm->Entry().ChangeL( tentry );

        // get an access to the message store
        CMsvStore* store = entry->EditStoreL();
        CleanupStack::PushL( store );
        CImHeader* header = CImHeader::NewLC();
        header->RestoreL( *store );
        TUint charset = header->Charset();
        CleanupStack::PopAndDestroy( header );
        CleanupStack::PopAndDestroy( store );

        // handle attachment
        TBool attachmentsExist = EFalse;
        if ( attPath->Length() > 0 )
            {
            // check that attachment exists
            RFs fsSession;
            if ( fsSession.Connect() != KErrNone )
                {
                HTI_LOG_FORMAT( "Error in connecting to file server session: %d", err );
                SendErrorMessageL( KErrCouldNotConnect, KErrorRfsConnectFailed );
                CleanupStack::PopAndDestroy( smtpMtm );
                CleanupStack::PopAndDestroy( attPath );
                CleanupStack::PopAndDestroy( body );
                CleanupStack::PopAndDestroy( description );
                CleanupStack::PopAndDestroy( fromTo );
                return;
                }
            CleanupClosePushL( fsSession );

            TBool fileExists = BaflUtils::FileExists( fsSession, attPath->Des() );
            if ( !fileExists )
                {
                HTI_LOG_TEXT( "Attachment file not found" );
                SendErrorMessageL( KErrPathNotFound, KErrorAttachmentNotFound );
                CleanupStack::PopAndDestroy(); // fsSession
                CleanupStack::PopAndDestroy( smtpMtm );
                CleanupStack::PopAndDestroy( attPath );
                CleanupStack::PopAndDestroy( body );
                CleanupStack::PopAndDestroy( description );
                CleanupStack::PopAndDestroy( fromTo );
                return;
                }
            else
                {
                // get the mime type
                HTI_LOG_TEXT( "Getting the attachment's mime type" );
                RApaLsSession ls;
                User::LeaveIfError( ls.Connect() );
                TUid appUid;
                TDataType dataType;
                ls.AppForDocument( *attPath, appUid, dataType );
                TPtrC8 mimeType = dataType.Des8();

                HTI_LOG_TEXT( "Adding the attachment" );
                CWaiter* waiter = CWaiter::NewLC();
                smtpMtm->AddAttachmentL( attPath->Des(), mimeType, charset,
                        waiter->iStatus );
                waiter->StartAndWait();
                CleanupStack::PopAndDestroy( waiter );
                HTI_LOG_TEXT( "Attachment added succesfully" );
                ls.Close();
                attachmentsExist = ETrue;
                }

            CleanupStack::PopAndDestroy(); // fsSession
            }

		do
			{
			HBufC16* attPath2;
			if(extraAttNum-- > 0)
	            {
	            attPath2 = ExtractDesLC( aData, position, 1 );
	            }
	        else
	            {
	            break;
	            }
            // check that attachment exists
            RFs fsSession;
            if ( fsSession.Connect() != KErrNone )
                {
                HTI_LOG_FORMAT( "Error in connecting to file server session: %d", err );
                SendErrorMessageL( KErrCouldNotConnect, KErrorRfsConnectFailed );
                CleanupStack::PopAndDestroy( smtpMtm );
                CleanupStack::PopAndDestroy( attPath );
                CleanupStack::PopAndDestroy( body );
                CleanupStack::PopAndDestroy( description );
                CleanupStack::PopAndDestroy( fromTo );
                CleanupStack::PopAndDestroy( attPath2 );
                return;
                }
            CleanupClosePushL( fsSession );

            TBool fileExists = BaflUtils::FileExists( fsSession, attPath2->Des() );
            if ( !fileExists )
                {
                HTI_LOG_TEXT( "Attachment file not found" );
                SendErrorMessageL( KErrPathNotFound, KErrorAttachmentNotFound );
                CleanupStack::PopAndDestroy(); // fsSession
                CleanupStack::PopAndDestroy( smtpMtm );
                CleanupStack::PopAndDestroy( attPath );
                CleanupStack::PopAndDestroy( body );
                CleanupStack::PopAndDestroy( description );
                CleanupStack::PopAndDestroy( fromTo );
                CleanupStack::PopAndDestroy( attPath2 );
                return;
                }
            else
                {
                // get the mime type
                HTI_LOG_TEXT( "Getting the attachment's mime type" );
                RApaLsSession ls;
                User::LeaveIfError( ls.Connect() );
                TUid appUid;
                TDataType dataType;
                ls.AppForDocument( *attPath2, appUid, dataType );
                TPtrC8 mimeType = dataType.Des8();

                HTI_LOG_TEXT( "Adding the attachment" );
                CWaiter* waiter = CWaiter::NewLC();
                smtpMtm->AddAttachmentL( attPath2->Des(), mimeType, charset,
                        waiter->iStatus );
                waiter->StartAndWait();
                CleanupStack::PopAndDestroy( waiter );
                HTI_LOG_TEXT( "Attachment added succesfully" );
                ls.Close();
                attachmentsExist = ETrue;
                }

            CleanupStack::PopAndDestroy(); // fsSession
			CleanupStack::PopAndDestroy( attPath2 );
		} while(ETrue);

        // save the message
        smtpMtm->SaveMessageL();

        // final fine tuning
        TMsvEmailEntry temailEntry = static_cast<TMsvEmailEntry>( tentry );
        temailEntry.SetMessageFolderType( EFolderTypeUnknown );
        temailEntry.SetDisconnectedOperation( ENoDisconnectedOperations );
        temailEntry.SetEncrypted( EFalse );
        temailEntry.SetSigned( EFalse );
        temailEntry.SetVCard( EFalse );
        temailEntry.SetVCalendar( EFalse );
        temailEntry.SetReceipt( EFalse );
        temailEntry.SetMHTMLEmail( EFalse );
        temailEntry.SetBodyTextComplete( ETrue );
        temailEntry.SetAttachment( attachmentsExist );
        temailEntry.iDate.UniversalTime();
        temailEntry.SetVisible( ETrue );
        temailEntry.SetInPreparation( EFalse );
        temailEntry.SetSendingState( KMsvSendStateWaiting );
        temailEntry.SetUnread( isUnread );
        temailEntry.SetNew( isNew );
        temailEntry.SetComplete( ETrue );
        temailEntry.iServiceId = defaultServiceId;
        temailEntry.iRelatedId = 0;

        smtpMtm->Entry().ChangeL( temailEntry );

        // get an access to the message store
        store = entry->EditStoreL();
        CleanupStack::PushL( store );

        // set email header info
        header = CImHeader::NewLC();
        header->RestoreL( *store );
        header->SetSubjectL( description->Des() );
        header->SetFromL( fromTo->Des() );
        header->SetReceiptAddressL( fromTo->Des() );
        header->StoreL( *store );
        store->CommitL();
        CleanupStack::PopAndDestroy( header );
        CleanupStack::PopAndDestroy( store );

        // send the message, if it is in outbox
        if ( folder == EOutbox )
            {
            HTI_LOG_TEXT( "E-Mail was created in outbox, marking it to be sent on next connection" );

            CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
            CleanupStack::PushL( selection );
            selection->AppendL( temailEntry.Id() );

            TBuf8<1> dummyParameter;
            CMsvOperationActiveSchedulerWait* waiter =
                    CMsvOperationActiveSchedulerWait::NewLC();
            CMsvOperation* op = smtpMtm->InvokeAsyncFunctionL(
                    KSMTPMTMSendOnNextConnection, *selection,
                    dummyParameter, waiter->iStatus );
            CleanupStack::PushL( op );
            waiter->Start();
            CleanupStack::PopAndDestroy( op );
            CleanupStack::PopAndDestroy( waiter );
            CleanupStack::PopAndDestroy( selection );
            }

        HTI_LOG_TEXT( "Cleaning up" );
        CleanupStack::PopAndDestroy( smtpMtm );
        CleanupStack::PopAndDestroy( attPath );
        CleanupStack::PopAndDestroy( body );
        CleanupStack::PopAndDestroy( description );
        CleanupStack::PopAndDestroy( fromTo );

        // send the message id back
        TInt32 id = tentry.Id();
        TBuf8<8> idStr;
        idStr.Copy( ( TUint8* )( &id ), sizeof( id ) );
        SendOkMsgL( idStr );
        }

    HTI_LOG_FUNC_OUT("CMessageMgmntHandler::HandleCreateEmailL: Done");
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::HandleCreateObexMsgL( const TDesC8& aData,
                                                 TUid aMtmUid,
                                                 TUid aMsgTypeUid )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::HandleCreateObexMsgL" );

    if ( ValidateAddObexMsgCommand( aData ) )
        {
        // parse the parameters
        TInt position( 0 );
        HBufC16* fromTo = ExtractDesLC( aData, position, 1 );
        HBufC16* description = ExtractDesLC( aData, position, 1 );
        HBufC16* attPath = ExtractDesLC( aData, position, 1 );
        TBool isNew = (TBool)aData[position];
        TBool isUnread = (TBool)aData[position+1];
        TFolder folder = (TFolder)aData[position+2];

        // Adding Obex messages to the outbox is not allowed
        if ( folder == EOutbox )
            {
            HTI_LOG_TEXT( "Outbox not supported with Obex messages" );
            SendErrorMessageL( KErrNotSupported, KErrorNotSupported );
            CleanupStack::PopAndDestroy( attPath );
            CleanupStack::PopAndDestroy( description );
            CleanupStack::PopAndDestroy( fromTo );
            return;
            }

        CObexClientMtm* obexMtm = NULL;
        TRAPD( err, obexMtm = ( CObexClientMtm* )iMtmReg->NewMtmL( aMtmUid ) );
        if ( err || !obexMtm )
            {
            HTI_LOG_TEXT( "Obex message type module not found" );
            SendErrorMessageL( KErrNotFound, KErrorMsgTypeNotFound );
            CleanupStack::PopAndDestroy( attPath );
            CleanupStack::PopAndDestroy( description );
            CleanupStack::PopAndDestroy( fromTo );
            return;
            }
        CleanupStack::PushL( obexMtm );

        CMsvEntry* entry = CMsvEntry::NewL( *iSession,
                                            KMsvGlobalInBoxIndexEntryId,
                                            TMsvSelectionOrdering() );
        CleanupStack::PushL( entry );

        TMsvId defaultServiceId = 0;

        // map the folder parameter to folder id
        TMsvId folderId = KMsvGlobalInBoxIndexEntryId;
        TRAP( err, folderId = MapFolderToIdL( folder ) );
        if ( err )
            {
            HTI_LOG_FORMAT( "Invalid folder: %d", folder );
            SendErrorMessageL( err, KErrorInvalidFolder );
            CleanupStack::PopAndDestroy( entry );
            CleanupStack::PopAndDestroy( obexMtm );
            CleanupStack::PopAndDestroy( attPath );
            CleanupStack::PopAndDestroy( description );
            CleanupStack::PopAndDestroy( fromTo );
            return;
            }
        entry->SetEntryL( folderId );

        // mtm takes ownership of entry context
        obexMtm->SetCurrentEntryL( entry );
        CleanupStack::Pop( entry );

        // create a new message
        obexMtm->CreateMessageL( defaultServiceId );

        // get the entry of the message
        TMsvEntry tentry = obexMtm->Entry().Entry();

        // set subject
        obexMtm->SetSubjectL( description->Des() );
        tentry.iDescription.Set( description->Des() );

        // set body, must be empty for obex messages
        obexMtm->Body().Reset();

        // set the details field and
        tentry.iDetails.Set( fromTo->Des() );

        // set mtm
        tentry.iMtm = aMtmUid;
        tentry.iType = KUidMsvMessageEntry;
        tentry.iServiceId = KMsvUnknownServiceIndexEntryId;

        // save the changes done above
        obexMtm->Entry().ChangeL( tentry );

        // save the message
        obexMtm->SaveMessageL();

        // final fine tuning
        tentry.iDate.HomeTime();
        tentry.SetVisible( ETrue );
        tentry.SetInPreparation( EFalse );
        tentry.SetUnread( isUnread );
        tentry.SetNew( isNew );
        tentry.SetComplete( ETrue );
        obexMtm->Entry().ChangeL( tentry );

        // handle attachment
        if ( attPath->Length() > 0 )
            {
            // check that attachment exists
            RFs fsSession;
            if ( fsSession.Connect() != KErrNone )
                {
                HTI_LOG_FORMAT( "Error in connecting to file server session: %d", err );
                SendErrorMessageL( KErrCouldNotConnect, KErrorRfsConnectFailed );
                CleanupStack::PopAndDestroy( obexMtm );
                CleanupStack::PopAndDestroy( attPath );
                CleanupStack::PopAndDestroy( description );
                CleanupStack::PopAndDestroy( fromTo );
                return;
                }

            TBool fileExists = BaflUtils::FileExists( fsSession, attPath->Des() );
            fsSession.Close();
            if ( !fileExists )
                {
                HTI_LOG_TEXT( "Attachment file not found" );
                SendErrorMessageL( KErrPathNotFound, KErrorAttachmentNotFound );
                CleanupStack::PopAndDestroy( obexMtm );
                CleanupStack::PopAndDestroy( attPath );
                CleanupStack::PopAndDestroy( description );
                CleanupStack::PopAndDestroy( fromTo );
                return;
                }
            else
                {
                // create a new entry for the attachment
                TMsvEntry attachTEntry;
                attachTEntry.iType = KUidMsvAttachmentEntry;
                attachTEntry.iServiceId = KMsvUnknownServiceIndexEntryId;
                attachTEntry.iMtm = aMsgTypeUid; //save as bt message

                entry->CreateL( attachTEntry );

                CMsvEntry* attachEntry = iSession->GetEntryL( attachTEntry.Id() );
                obexMtm->SetCurrentEntryL( attachEntry );

                // get source file
                TFileName sourceFileName = attPath->Des();

                // get the mime type
                RApaLsSession ls;
                User::LeaveIfError( ls.Connect() );
                CleanupClosePushL<RApaLsSession>(ls);
                TUid appUid;
                TDataType mimeType;
                ls.AppForDocument( sourceFileName, appUid, mimeType );
                CleanupStack::PopAndDestroy(); //ls

                CWaiter* waiter = CWaiter::NewLC();

                // add an attachment to the current message entry
                obexMtm->AddAttachmentL( sourceFileName, mimeType.Des8(), 0,
                        waiter->iStatus );
                waiter->StartAndWait();
                CleanupStack::PopAndDestroy( waiter );
                }
            }

        CleanupStack::PopAndDestroy( obexMtm );
        CleanupStack::PopAndDestroy( attPath );
        CleanupStack::PopAndDestroy( description );
        CleanupStack::PopAndDestroy( fromTo );

        // send the message id back
        TInt32 id = tentry.Id();
        TBuf8<8> idStr;
        idStr.Copy( ( TUint8* )( &id ), sizeof( id ) );
        SendOkMsgL( idStr );
        }

    HTI_LOG_FUNC_OUT("CMessageMgmntHandler::HandleCreateObexMsgL: Done");
    }


// ----------------------------------------------------------------------------
void CMessageMgmntHandler::HandleCreateSmartMsgL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::HandleCreateSmartMsgL" );

    if ( ValidateAddSmartMsgCommand( aData ) )
        {
        TInt position( 0 );
        HBufC16* fromTo = ExtractDesLC( aData, position, 1 );
        HBufC16* description = ExtractDesLC( aData, position, 1 );
        HBufC16* body = ExtractDesLC( aData, position, 2 );
        TBool isNew = (TBool)aData[position];
        TBool isUnread = (TBool)aData[position+1];
        TFolder folder = (TFolder)aData[position+2];
        TInt bioUidValue =   aData[position+3] +
                           ( aData[position+4] << 8 ) +
                           ( aData[position+5] << 16 ) +
                           ( aData[position+6] << 24 );


        // Smart messages can be created only to inbox.
        // For sending smart messages, create a normal SMS with smart message
        // content as a body and send it.
        if ( folder != EInbox )
            {
            HTI_LOG_TEXT( "Invalid folder specified for smart message" );
            SendErrorMessageL( KErrArgument, KErrorInvalidFolderForSmartMsg );
            CleanupStack::PopAndDestroy( body );
            CleanupStack::PopAndDestroy( description );
            CleanupStack::PopAndDestroy( fromTo );
            return;

            }

        CSmsClientMtm* smsMtm = NULL;
        TRAPD( err, smsMtm = ( CSmsClientMtm* )iMtmReg->NewMtmL( KUidMsgTypeSMS ) );
        if ( err || !smsMtm )
            {
            HTI_LOG_TEXT( "SMS message type module not found" );
            SendErrorMessageL( KErrNotFound, KErrorMsgTypeNotFound );
            CleanupStack::PopAndDestroy( body );
            CleanupStack::PopAndDestroy( description );
            CleanupStack::PopAndDestroy( fromTo );
            return;
            }
        CleanupStack::PushL( smsMtm );

        CMsvEntry* entry = CMsvEntry::NewL( *iSession,
                KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering() );
        CleanupStack::PushL( entry );

        // get the default service
        TMsvId defaultServiceId = 0;
        TRAP( err, defaultServiceId = smsMtm->DefaultServiceL() );
        if ( err )
            {
            HTI_LOG_FORMAT( "Could not get default service, err: %d", err );
            SendErrorMessageL( err, KErrorSmsSettingNotDefined );
            CleanupStack::PopAndDestroy( entry );
            CleanupStack::PopAndDestroy( smsMtm );
            CleanupStack::PopAndDestroy( body );
            CleanupStack::PopAndDestroy( description );
            CleanupStack::PopAndDestroy( fromTo );
            return;
            }

        // no need for folder mapping, since only inbox allowed for smart messages
        TMsvId folderId = KMsvGlobalInBoxIndexEntryId;
        entry->SetEntryL( folderId );

        // mtm takes ownership of entry context
        smsMtm->SetCurrentEntryL( entry );
        CleanupStack::Pop( entry );

        // create a new message
        smsMtm->CreateMessageL( defaultServiceId );

        // update the message header
        CSmsHeader* smsHeader = &( smsMtm->SmsHeader() );
        delete smsHeader;
        smsHeader = NULL;
        smsHeader = CSmsHeader::NewL( CSmsPDU::ESmsSubmit, smsMtm->Body() );
        smsHeader->SetFromAddressL( fromTo->Des() );

        // set body, the actual BIO message content
        smsMtm->Body().Reset();
        smsMtm->Body().InsertL( 0, body->Des() );

        // get the entry of the message
        TMsvEntry tentry = smsMtm->Entry().Entry();

        // set BIO message type specific data
        tentry.iBioType = bioUidValue;
        smsMtm->BioTypeChangedL( TUid::Uid( bioUidValue ) );

        // set details field
        tentry.iDetails.Set( fromTo->Des() );

        // set the description field
        tentry.iDescription.Set( description->Des() );

        // set correct MTM type
        tentry.iMtm= KUidBIOMessageTypeMtm;

        // final fine tuning
        tentry.SetAttachment( EFalse );
        tentry.iDate.UniversalTime();
        tentry.SetVisible( ETrue );
        tentry.SetInPreparation( EFalse );
        tentry.SetUnread( isUnread );
        tentry.SetNew( isNew );
        tentry.SetComplete( ETrue );
        tentry.SetSendingState( KMsvSendStateWaiting );
        tentry.iServiceId = defaultServiceId;
        tentry.iRelatedId = 0;

        // save the changes done above
        smsMtm->Entry().ChangeL( tentry );

        // save the message
        smsMtm->SaveMessageL();

        CleanupStack::PopAndDestroy( smsMtm );
        CleanupStack::PopAndDestroy( body );
        CleanupStack::PopAndDestroy( description );
        CleanupStack::PopAndDestroy( fromTo );

        TInt32 id = tentry.Id();
        TBuf8<8> idStr;
        idStr.Copy( ( TUint8* )( &id ), sizeof( id ) );
        SendOkMsgL( idStr );
        }

    HTI_LOG_FUNC_OUT("CMessageMgmntHandler::HandleCreateSmartMsgL: Done");
    }


// ----------------------------------------------------------------------------
void CMessageMgmntHandler::HandleDeleteMessageL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::HandleDeleteMessageL" );

    if ( aData.Length() != 4 )
        {
        HTI_LOG_TEXT( "CMessageMgmntHandler: Error: wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidId );
        return;
        }

    TMsvId entryId =   aData[0] +
                     ( aData[1] << 8 ) +
                     ( aData[2] << 16 ) +
                     ( aData[3] << 24 );
    HTI_LOG_FORMAT( "CMessageMgmntHandler: Deleting one message, id: %d", entryId );
    TMsvEntry entry;
    TMsvId service;
    User::LeaveIfError( iSession->GetEntry( entryId, service, entry ) );

    CMsvEntry* parentCEntry = iSession->GetEntryL( entry.Parent() );
    CleanupStack::PushL( parentCEntry );
    TRAPD( err, parentCEntry->DeleteL( entry.Id() ) );
    CleanupStack::PopAndDestroy( parentCEntry );

    if ( err == KErrNone )
        {
        SendOkMsgL( KNullDesC8 );
        }
    else if ( err == KErrNotFound )
        {
        SendErrorMessageL( err, KErrorItemNotFound );
        }
    else
        {
        SendErrorMessageL( err, KErrorFailedDelete );
        }

    HTI_LOG_FUNC_OUT("CMessageMgmntHandler::HandleDeleteMessageL: Done");
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::HandleDeleteMessagesL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::HandleDeleteMessagesFuncL" );

    if ( aData.Length() != 2 )
        {
        HTI_LOG_TEXT( "CMessageMgmntHandler: Error: wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidFolder );
        return;
        }

    if ( aData[0] == EAllFolders )
        {
        HandleDeleteFromAllFoldersL( (TMessageType)aData[1] );
        }
    else if ( aData[1] == EAllMessageTypes )
        {
        HandleDeleteAllMessageTypesL( (TFolder)aData[0] );
        }
    else
        {
        HandleDeleteFromFolderByTypeL( (TFolder)aData[0],
                                       (TMessageType)aData[1] );
        }

    SendOkMsgL( KNullDesC8 );
    HTI_LOG_FUNC_OUT("CMessageMgmntHandler::HandleDeleteMessagesFuncL: Done");
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::HandleDeleteFromAllFoldersL( TMessageType aType )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::HandleDeleteFromAllFoldersL" );

    if ( aType == EAllMessageTypes )
        {
        for ( TInt i = 1; i < ENumberOfFolders; i++ )
            {
            HandleDeleteAllMessageTypesL( (TFolder)i );
            }
        }
    else
        {
        for ( TInt i = 1; i < ENumberOfFolders; i++ )
            {
            HandleDeleteFromFolderByTypeL( (TFolder)i, aType );
            }
        }

    HTI_LOG_FUNC_OUT("CMessageMgmntHandler::HandleDeleteFromAllFoldersL: Done");
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::HandleDeleteAllMessageTypesL( TFolder aFolder )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::HandleDeleteAllMessageTypesL" );

    if ( aFolder == EAllFolders )
        {
        for ( TInt i = 1; i < ENumberOfMessageTypes; i++ )
            {
            HandleDeleteFromAllFoldersL( (TMessageType)i );
            }
        }
    else
        {
        for ( TInt i = 1; i < ENumberOfMessageTypes; i++ )
            {
            HandleDeleteFromFolderByTypeL( aFolder, (TMessageType)i );
            }
        }

    HTI_LOG_FUNC_OUT("CMessageMgmntHandler::HandleDeleteAllMessageTypesL: Done");
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::HandleDeleteFromFolderByTypeL( TFolder aFolder,
                                                          TMessageType aType )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::HandleDeleteFromFolderByTypeL" );

    TMsvId folderId = MapFolderToIdL( aFolder );
    TUid msgTypeUid = MapMessageTypeToUidL( aType );

    HTI_LOG_TEXT( "Deleting messages..." );
    HTI_LOG_FORMAT( "Folder: %d", aFolder );
    HTI_LOG_FORMAT( "Message type: %d", aType );

    CMsvEntry* folder = CMsvEntry::NewL( *iSession,
                                         folderId,
                                         TMsvSelectionOrdering() );
    CleanupStack::PushL( folder );
    CMsvEntrySelection* sel = folder->ChildrenWithMtmL( msgTypeUid );

    CleanupStack::PushL( sel );
    HTI_LOG_FORMAT( "Found %d matching items", sel->Count() );

    for ( TInt i = 0; i < sel->Count(); i++ )
        {
        TMsvId entryId = sel->At( i );
        TMsvEntry entry;
        TMsvId service;
        User::LeaveIfError( iSession->GetEntry( entryId, service, entry ) );
        if ( ( aType == EAudioMessage && entry.iBioType != KUidMsgSubTypeMmsAudioMsg.iUid ) ||
                ( aType == EMMS && entry.iBioType == KUidMsgSubTypeMmsAudioMsg.iUid ) )
            {
            // do not delete audio messages when MMS deletion
            // requested and vice versa
            continue;
            }
        CMsvEntry* parentCEntry = iSession->GetEntryL( entry.Parent() );
        CleanupStack::PushL( parentCEntry );
        parentCEntry->DeleteL( entry.Id() );
        CleanupStack::PopAndDestroy( parentCEntry );
        }

    CleanupStack::PopAndDestroy( sel );
    CleanupStack::PopAndDestroy( folder );

    if ( aType == EEmail )
        {
        HandleDeleteFromFolderByTypeL( aFolder, EEmailPOP3 );
        HandleDeleteFromFolderByTypeL( aFolder, EEmailIMAP4 );
        }

    HTI_LOG_FUNC_OUT("CMessageMgmntHandler::HandleDeleteFromFolderByTypeL: Done");
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::SendOkMsgL( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN("CMessageMgmntHandler::SendOkMsgL: Starting");

    User::LeaveIfNull( iDispatcher );

    HBufC8* temp = HBufC8::NewL( aData.Length() + 1 );
    TPtr8 response = temp->Des();
    response.Append( (TChar) CHtiMessagesServicePlugin::EResultOk );
    response.Append( aData );
    User::LeaveIfError( iDispatcher->DispatchOutgoingMessage(
        temp, KHtiMessagesServiceUid ) );

    HTI_LOG_FUNC_OUT("CMessageMgmntHandler::SendOkMsgL: Done");
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::SendErrorMessageL( TInt aError,
                                              const TDesC8& aDescription )
    {
    HTI_LOG_FUNC_IN("CMessageMgmntHandler::SendErrorMessageL: Starting");
    User::LeaveIfNull( iDispatcher );
    User::LeaveIfError( iDispatcher->DispatchOutgoingErrorMessage(
        aError, aDescription, KHtiMessagesServiceUid ) );
    HTI_LOG_FUNC_OUT("CMessageMgmntHandler::SendErrorMessageL: Done");
    }

// ----------------------------------------------------------------------------
TBool CMessageMgmntHandler::ValidateAddSmsCommand( const TDesC8& aData )
    {
    if ( aData.Length() < KAddSmsCmdMinLength )
        {
        HTI_LOG_TEXT( "ValidateAddSmsCommand: Error: missing data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }

    TInt offset = 0;
    TInt fromLength = aData[offset];

    offset = 1 + fromLength;
    if ( offset > aData.Length() - 1 )
        {
        HTI_LOG_TEXT( "ValidateAddSmsCommand: Error: wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }
    TInt descrLength = aData[offset];

    offset = offset + 1 + descrLength;
    if ( offset > aData.Length() - 2 ) // body length in two bytes
        {
        HTI_LOG_TEXT( "ValidateAddSmsCommand: Error: wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }
    TInt bodyLength = aData[offset] + ( aData[offset+1] << 8 );

    TInt wholeLength = 1 + fromLength +
                       1 + descrLength +
                       2 + bodyLength +
                       1 + // is new
                       1 + // is unread
                       1;  // folder

    if ( wholeLength != aData.Length() )
        {
        HTI_LOG_TEXT( "ValidateAddSmsCommand: Error: wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }

    if ( bodyLength > 160 )
        {
        HTI_LOG_TEXT( "ValidateAddSmsCommand: Error: too long SMS body" );
        SendErrorMessageL( KErrOverflow, KErrorTooLongSmsBody );
        return EFalse;
        }

    return ETrue;
    }


// ----------------------------------------------------------------------------
TBool CMessageMgmntHandler::ValidateAddMmsOrAddEmailCommand( const TDesC8& aData )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::ValidateAddMmsOrAddEmailCommand" );
    if ( aData.Length() < KAddMmsOrEmailCmdMinLength + 1 ) // +1 = cmd code
        {
        HTI_LOG_TEXT( "Error: missing data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }

    if ( aData[0] == CHtiMessagesServicePlugin::EAddAudioMsg &&
            aData.Length() < KAddAudioCmdMinLength + 1 ) // +1 = cmd code
        {
        HTI_LOG_TEXT( "ValidateAddMmsOrAddEmailCommand: Error: missing data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }

    TInt offset = 0;
    TInt cmdCode = aData[offset];
    offset++;
    TInt fromToLength = aData[offset];
    fromToLength++; // the length byte

    offset = offset + fromToLength;
    if ( offset > aData.Length() - 1 )
        {
        HTI_LOG_TEXT( "Error: wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }
    TInt descrLength = aData[offset];
    descrLength++; // the length byte

    offset = offset + descrLength;
    TInt bodyLength = 0;
    if ( cmdCode != CHtiMessagesServicePlugin::EAddAudioMsg )
        {
        if ( offset > aData.Length() - 2 ) // body length in two bytes
            {
            HTI_LOG_TEXT( "Error: wrong length of data" );
            SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
            return EFalse;
            }
        bodyLength = aData[offset] + ( aData[offset+1] << 8 );
        bodyLength += 2; // the body length bytes
        }

    offset = offset + bodyLength;
    if ( offset > aData.Length() - 1 )
        {
        HTI_LOG_TEXT( ": wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }
    TInt attPathLength = aData[offset];
    if ( attPathLength == 0 && cmdCode == CHtiMessagesServicePlugin::EAddAudioMsg )
        {
        // attachment (the audio) is mandatory for audio message
        HTI_LOG_TEXT( "Error: missing attachment" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }
    attPathLength++; // the length byte

    TInt wholeLength = 1 + // command code
                       fromToLength + descrLength +  bodyLength + attPathLength +
                       1 + // is new
                       1 + // is unread
                       1;  // folder

    TInt extraAttPathLength = 0;
    TInt extraAttNum = 0;
    TInt extraNumLen = 0;
    if( wholeLength < aData.Length() )
    	{
    	offset = wholeLength;
        extraAttNum = aData[offset];
        offset ++;

        extraNumLen = 1;

	    while( offset < aData.Length() && extraAttNum > 0)
	    	{
	        extraAttPathLength += aData[offset];
	        extraAttPathLength ++;
	        extraAttNum --;
	        offset += 1 + aData[offset];
	    	}
	    }

	wholeLength += extraNumLen + extraAttPathLength;
	
    if ( wholeLength != aData.Length() )
        {
        HTI_LOG_TEXT( "Error: wrong length of data (wholeLength)" );
        HTI_LOG_FORMAT( "Expected: %d", wholeLength );
        HTI_LOG_FORMAT( "Was:      %d", aData.Length() );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }

    return ETrue;
    }


// ----------------------------------------------------------------------------
TBool CMessageMgmntHandler::ValidateAddObexMsgCommand( const TDesC8& aData )
    {
    if ( aData.Length() < KAddObexMsgCmdMinLength )
        {
        HTI_LOG_TEXT( "ValidateAddObexMsgCommand: Error: missing data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }

    TInt offset = 0;
    TInt fromToLength = aData[offset];

    offset = 1 + fromToLength;
    if ( offset > aData.Length() - 1 )
        {
        HTI_LOG_TEXT( "ValidateAddObexMsgCommand: Error: wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }
    TInt descrLength = aData[offset];

    offset = offset + 1 + descrLength;
    if ( offset > aData.Length() - 1 )
        {
        HTI_LOG_TEXT( "ValidateAddObexMsgCommand: Error: wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }
    TInt attPathLength = aData[offset];

    TInt wholeLength = 1 + fromToLength +
                       1 + descrLength +
                       1 + attPathLength +
                       1 + // is new
                       1 + // is unread
                       1;  // folder

    if ( wholeLength != aData.Length() )
        {
        HTI_LOG_TEXT( "ValidateAddObexMsgCommand: Error: wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
TBool CMessageMgmntHandler::ValidateAddSmartMsgCommand( const TDesC8& aData )
    {
    if ( aData.Length() < KAddSmartMsgCmdMinLength )
        {
        HTI_LOG_TEXT( "ValidateAddSmartMsgCommand: Error: missing data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }

    TInt offset = 0;
    TInt fromToLength = aData[offset];

    offset = 1 + fromToLength;
    if ( offset > aData.Length() - 1 )
        {
        HTI_LOG_TEXT( "ValidateAddSmartMsgCommand: Error: wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }
    TInt descrLength = aData[offset];

    offset = offset + 1 + descrLength;
    if ( offset > aData.Length() - 2 ) // body length in two bytes
        {
        HTI_LOG_TEXT( "ValidateAddSmartMsgCommand: Error: wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }
    TInt bodyLength = aData[offset] + ( aData[offset+1] << 8 );

    TInt wholeLength = 1 + fromToLength +
                       1 + descrLength +
                       2 + bodyLength +
                       1 + // is new
                       1 + // is unread
                       1 + // folder
                       4;  // biomessage uid

    if ( wholeLength != aData.Length() )
        {
        HTI_LOG_TEXT( "ValidateAddSmartMsgCommand: Error: wrong length of data" );
        SendErrorMessageL( KErrArgument, KErrorInvalidParameters );
        return EFalse;
        }

    return ETrue;
    }


// ----------------------------------------------------------------------------
// Extracts UTF-8 data, converts it to Unicode and returns as 16-bit descriptor.
// Within aData, read descriptor from aPosition:
//  - first bytes tell the size of data for UTF8 formatted data
//  - next bytes are the data as indicated by the size
//  - position is finally set to the end of UTF8 data area
// ----------------------------------------------------------------------------
HBufC16* CMessageMgmntHandler::ExtractDesLC( const TDesC8& aUtf8Data,
                                             TInt& aPosition,
                                             TInt aSizeBytes )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::ExtractDesLC" );
    TInt length = 0;
    for ( TInt i = 0; i < aSizeBytes; i++ )
        {
        length += ( aUtf8Data[aPosition+i] << ( i * 8 ) );
        }

    if ( length < 0 ||
         length > aUtf8Data.Mid( aPosition ).Length() )
        {
        User::Leave( KErrBadDescriptor );
        }

    HBufC16* result = NULL;

    if ( length > 0 )
        {
        result = CnvUtfConverter::ConvertToUnicodeFromUtf8L(
            aUtf8Data.Mid( aPosition + aSizeBytes, length ) );
        HTI_LOG_TEXT( "ExtractDesLC: Conversion to Unicode done" );
        CleanupStack::PushL( result );
        }

    else
        {
        result = HBufC16::NewLC( 0 );
        }

    aPosition += ( aSizeBytes + length );

    HTI_LOG_FUNC_OUT( "CMessageMgmntHandler::ExtractDesLC" );
    return result;
    }


// ----------------------------------------------------------------------------
// Extracts UTF-8 data to 8-bit descriptor without doing any conversions.
// ----------------------------------------------------------------------------
HBufC8* CMessageMgmntHandler::ExtractDes8LC( const TDesC8& aUtf8Data,
                                             TInt& aPosition,
                                             TInt aSizeBytes )
    {
    HTI_LOG_FUNC_IN( "CMessageMgmntHandler::ExtractDes8LC" );
    TInt length = 0;
    for ( TInt i = 0; i < aSizeBytes; i++ )
        {
        length += ( aUtf8Data[aPosition+i] << ( i * 8 ) );
        }

    if ( length < 0 ||
         length > aUtf8Data.Mid( aPosition ).Length() )
        {
        User::Leave( KErrBadDescriptor );
        }

    HBufC8* result = HBufC8::NewLC( length );

    if ( length > 0 )
        {
        result->Des().Copy( aUtf8Data.Mid( aPosition + aSizeBytes, length ) );
        }

    aPosition += ( aSizeBytes + length );

    HTI_LOG_FUNC_OUT( "CMessageMgmntHandler::ExtractDes8LC" );
    return result;
    }


// ----------------------------------------------------------------------------
TMsvId CMessageMgmntHandler::MapFolderToIdL( TFolder aFolder )
    {
    TMsvId id = 0;

    switch ( aFolder )
        {
        case EInbox:  { id = KMsvGlobalInBoxIndexEntryId;   break; }
        case EDrafts: { id = KMsvDraftEntryId;              break; }
        case ESent:   { id = KMsvSentEntryId;               break; }
        case EOutbox: { id = KMsvGlobalOutBoxIndexEntryId;  break; }
        default:      { User::Leave( KErrArgument );        break; }
        }

    return id;
    }

// ----------------------------------------------------------------------------
TUid CMessageMgmntHandler::MapMessageTypeToUidL( TMessageType aType )
    {
    TUid uid = { 0 };

    switch ( aType )
        {
        case ESMS:          { uid = KUidMsgTypeSMS;            break; }
        case EAudioMessage: // fall through - audio msg is MMS sub type
        case EMMS:          { uid = KUidMsgTypeMultimedia;     break; }
        case ESmartMessage: { uid = KUidBIOMessageTypeMtm;     break; }
        case EEmail:        { uid = KUidMsgTypeSMTP;           break; }
        case EEmailPOP3:    { uid = KUidMsgTypePOP3;           break; }
        case EEmailIMAP4:   { uid = KUidMsgTypeIMAP4;          break; }
        case EIrMessage:    { uid = KUidMsgTypeIrUID;          break; }
        case EBtMessage:    { uid = KUidMsgTypeBt;             break; }
        default:            { User::Leave( KErrArgument );     break; }
        }

    return uid;
    }

// ----------------------------------------------------------------------------
void CMessageMgmntHandler::HandleSessionEventL( TMsvSessionEvent /*aEvent*/,
                                                TAny* /*aArg1*/,
                                                TAny* /*aArg2*/,
                                                TAny* /*aArg3*/ )
    {
    }



// ----------------------------------------------------------------------------
CWaiter* CWaiter::NewL( TInt aPriority )
    {
    CWaiter* self = new(ELeave) CWaiter( aPriority );
    return self;
    }

// ----------------------------------------------------------------------------
CWaiter* CWaiter::NewLC( TInt aPriority )
    {
    CWaiter* self = new(ELeave) CWaiter( aPriority );
    CleanupStack::PushL( self );
    return self;
    }

// ----------------------------------------------------------------------------
CWaiter::CWaiter( TInt aPriority ) : CActive( aPriority )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
CWaiter::~CWaiter()
    {
    Cancel();
    }

// ----------------------------------------------------------------------------
void CWaiter::StartAndWait()
    {
    iStatus = KRequestPending;
    SetActive();
    iWait.Start();
    }

// ----------------------------------------------------------------------------
TInt CWaiter::Result() const
    {
    return iResult;
    }

// ----------------------------------------------------------------------------
void CWaiter::RunL()
    {
    iResult = iStatus.Int();
    iWait.AsyncStop();
    }

// ----------------------------------------------------------------------------
void CWaiter::DoCancel()
    {
    iResult = KErrCancel;
    if ( iStatus == KRequestPending )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrCancel );
        }

    iWait.AsyncStop();
    }


// End of file
