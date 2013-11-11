/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hbglobal.h>
#include <textresolver.h> // from CommonEngine
#include <mtclreg.h>
#include <mmsnotificationclient.h>
#include <mmssettings.h>
#include <xqconversions.h> // from xqutils
#include <mmsconst.h>
#include <QDateTime>

#include "conversationmsgstorehandler.h"
#include "draftsmodel.h"
#include "MuiuOperationWait.h"
#include "msgbiouids.h"
#include "UniEditorGenUtils.h"

// SYSTEM INCLUDES
#include <StringLoader.h>
#include <ccsdefs.h> 
#include <msvids.h>
#include <mmserrors.h>
#include <msvuids.h>
#include <smut.h>
#include <SendUiConsts.h>
#include <featmgr.h>
#include <gsmerror.h> 

// CONSTANTS
const TUid KSmsMtmUid ={KSenduiMtmSmsUidValue};
const TUid KMmsMtmUid ={KSenduiMtmMmsUidValue};
_LIT(KUnixEpoch, "19700000:000000.000000");
#define BYTES_TO_KBYTES_FACTOR 1024

// LOCALIZATION
#define LOC_MESSAGE_SIZE hbTrId("txt_messaging_list_size")
#define LOC_CLASS_ADVERTISEMENT hbTrId("txt_messaging_list_advertisement")
#define LOC_CLASS_INFORMATIONAL hbTrId("txt_messaging_list_informational")
#define LOC_CLASS_PERSONAL hbTrId("txt_messaging_list_personal")
#define LOC_MMS_RETRIEVAL_FAILED hbTrId("txt_messaging_dialog_mms_retrieval_failed")
#define LOC_MMS_NOTIF_EXPIRED hbTrId("txt_messaging_list_message_expired")  
#define LOC_MMS_WAITING hbTrId("txt_wireframe_list_multimedia_message_waiting")
#define LOC_MMS_RETRIEVING hbTrId("txt_messaging_list_retrieving_message")   
#define LOC_MMS_EXPIRY_DATE hbTrId("txt_messaging_list_expiry_date")

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Default constructor.
// ---------------------------------------------------------
//
ConversationMsgStoreHandler::ConversationMsgStoreHandler():
    iMsvSession(NULL),iDraftEntry(NULL),iDraftMessages(NULL),mDraftsModel(NULL),
    iMtmReg(NULL),iMmsClient(NULL),iNotificationClient(NULL)
    {
    TRAP_IGNORE(InitL());
    }

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
//
ConversationMsgStoreHandler::~ConversationMsgStoreHandler()
    {
    if ( iDraftEntry )
        {
        delete iDraftEntry;
        iDraftEntry = NULL;
        }

    if ( iDraftMessages )
        {
        iDraftMessages->Reset();
        delete iDraftMessages;
        iDraftMessages = NULL;
    }

    if (iMmsClient)
    {
        delete iMmsClient;
        iMmsClient = NULL;
    }
    if (iNotificationClient)
    {
        delete iNotificationClient;
        iNotificationClient = NULL;
    }
    if (iMtmReg)
    {
        delete iMtmReg;
        iMtmReg = NULL;
    }
    if (iMsvSession)
    {
        delete iMsvSession;
        iMsvSession = NULL;
        }
    }

// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::InitL( )
    {
    TInt err = KErrNone;
    TRAP(err,iMsvSession = CMsvSession::OpenSyncL(*this));
    if(err != KErrNone)
        {
        iMsvSession = NULL;
        return;
        }
    
    TRAP(err,iMtmReg = CClientMtmRegistry::NewL(*iMsvSession));
    if(err != KErrNone)
        {
        iMtmReg = NULL;
        return;
        }
	
	 // Get if offline is supported
    FeatureManager::InitializeLibL();
    if (FeatureManager::FeatureSupported(KFeatureIdOfflineMode))
    {
        iOfflineSupported = ETrue;
    }
    else
    {
        iOfflineSupported = EFalse;
    }
    FeatureManager::UnInitializeLib();
		
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::HandleSessionEventL()
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::HandleSessionEventL(TMsvSessionEvent aEvent,
        TAny* aArg1, TAny* aArg2, TAny* aArg3)
    {   
    //args
    if(aArg1 == NULL || aArg2 == NULL)
        {   
        return;
        }

    //start, processing the event
    CMsvEntrySelection* selection=NULL;
    TMsvId target = 0;
    TMsvId source = 0;
    selection= (CMsvEntrySelection*)aArg1;
    target = *(TMsvId*)aArg2; // target (current) parent
    if(aArg3 != NULL)
        {
        source = *(TMsvId*)aArg3; // source parent
        }
    //process for draft messages only
    if ( target == KMsvDraftEntryIdValue)
        {
        switch( aEvent )
            {
            //case EMsvEntriesCreated:
            case EMsvEntriesChanged:
            case EMsvEntriesMoved:
                {
                TMsvEntry entry;
                TMsvId service;
                TInt error= KErrNone;

                for( TInt i=0 ; i < selection->Count() ; ++i )
                    {
                    error = iMsvSession->GetEntry(selection->At(i),service,entry);
                    if (KErrNone == error)
                        {
                        // process only visible entries
                        if (entry.Visible() && IsMtmSupported(entry.iMtm.iUid))
                            {
                                if(mDraftsModel && mDraftsModel->isReady())
                                {
                                    mDraftsModel->addRow(entry);
                                }
                            }
                        }
                    }
                }
                break;

            case EMsvEntriesDeleted:
                {
                for( TInt i=0 ; i < selection->Count() ; ++i )
                    {
                    TMsvId id = selection->At( i );
                    if(mDraftsModel && mDraftsModel->isReady())
                        {
                        mDraftsModel->deleteRow(id);
                        }
                    }
                }
                break;
            }//end switch 
        }
    else if(KMsvDraftEntryIdValue == source)
        {
        //if message is moved from drafts to other folder
        // it needs to be removed.
        if(aEvent == EMsvEntriesMoved)
            {
            for( TInt i=0 ; i < selection->Count() ; ++i )
                {
                TMsvId id = selection->At( i );
                if(mDraftsModel && mDraftsModel->isReady())
                    {
                    mDraftsModel->deleteRow(id);
                    }
                }
            }
        }
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::MsgStatus
// ---------------------------------------------------------
//
TCsMmsNotificationMsgState ConversationMsgStoreHandler::
MmsNotificationStatus( TInt aMsvId )
    {
    TMsvId dummy = 0x0;
    
    TMsvEntry entry;
    iMsvSession->GetEntry(aMsvId, dummy, entry);
    
    TCsMmsNotificationMsgState status = EMsgStatusNull;

    TTime currentTime;
    currentTime.HomeTime( );
    TTime expiryTime = iNotificationClient->ExpiryDate( );
    TLocale locale;
    expiryTime += locale.UniversalTimeOffset();
    if (locale.QueryHomeHasDaylightSavingOn())          
    {
        TTimeIntervalHours daylightSaving(1);          
        expiryTime += daylightSaving;
    }

    // operationMask includes operation type. It is not bitmap but ordinal number. 
    // It does not include operation status and result
    TInt operationMask = (entry.iMtmData2 & KMmsOperationIdentifier) ;

    // Note! Ongoing operation resets every bit of operation type, operation status
    // and result. E.g. If message has been forwarded and then fetching starts, 
    // information about forwarding is lost

    if( ( entry.iMtmData1 & KMmsMessageTypeMask ) == KMmsMessageMNotificationInd )
        {
        if(     operationMask == KMmsOperationFetch 
                &&  OperationOngoing( entry ) )
            { 
            // It's in retrieving state
            status = EMsgStatusRetrieving;
            }
        else if(    operationMask == KMmsOperationForward
                &&  OperationOngoing( entry ) )
            { 
            // It's in forwarding state
            status = EMsgStatusForwarding;
            }
        else if(    operationMask == KMmsOperationForward
                &&  OperationFinished( entry )
                &&  !( entry.iMtmData2 & KMmsOperationResult ) )
            { 
            // It's been forwarded succesfully
            status = EMsgStatusForwarded;
            }
        else if(    operationMask == KMmsOperationFetch 
                &&  OperationFinished( entry )
                &&   (  entry.iMtmData2 & KMmsOperationResult 
                ||  entry.iError ) )
            { 
            // Fetch has been failed
            status = EMsgStatusFailed;
            }
        else if(    operationMask == KMmsOperationDelete
                &&  OperationFinished( entry )
                &&  !( entry.iMtmData2 & KMmsOperationResult ) )
            { 
            // It's been deleted succesfully
            status = EMsgStatusDeleted;
            }
        else if( currentTime > expiryTime )
            {
            status = EMsgStatusExpired;
            }
        else 
            {   // Normal waiting state
            status = EMsgStatusReadyForFetching;
            }
        }

    return status;
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::OperationOngoing
// ---------------------------------------------------------
//
TBool ConversationMsgStoreHandler::OperationOngoing( const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationOngoing 
            &&  !( aEntry.iMtmData2 & KMmsOperationFinished ) );
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::OperationFinished
// ---------------------------------------------------------
//
TBool ConversationMsgStoreHandler::OperationFinished( 
    const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationFinished
            &&  !( aEntry.iMtmData2 & KMmsOperationOngoing ) );
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::MarkMessagesReadL
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::MarkMessagesReadL(RArray<TInt>& aIdArray)
    {    
    for ( int index = 0; index < aIdArray.Count(); index++ )
        {
        TMsvId id = aIdArray[index];
        CMsvEntry* cEntry = NULL;
        TRAPD(err, cEntry = iMsvSession->GetEntryL(id));
        if ( err == KErrNotFound )
            continue; // Entry is already deleted.
        TMsvEntry entry = cEntry->Entry();
        if ( entry.Unread() ) 
            {
            // Mark the entry as read
            entry.SetUnread( EFalse );
            cEntry->ChangeL( entry );
            }
        delete cEntry;
        }
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::DeleteMessages
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::DeleteMessages(RArray<TInt>& aIdArray)
{
    for(int index=0;index < aIdArray.Count() ;++index)
    {
        TMsvId id = aIdArray[index];
        iMsvSession->RemoveEntry(id);
    }   
}

// ---------------------------------------------------------
// ConversationMsgStoreHandler::DeleteAllDraftMessages
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::DeleteAllDraftMessagesL()
{
    // Cancel the processing of draft messages.
    iIdle->Cancel();

    CMsvEntry *draftsEntry = iMsvSession->GetEntryL(KMsvDraftEntryIdValue);
    CleanupStack::PushL(draftsEntry);
    CMsvEntrySelection *draftsSelection = draftsEntry->ChildrenL();
    CleanupStack::PushL(draftsSelection);
    if ( draftsSelection->Count() > 0 )
        {
        CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();
        CMsvOperation *operation = draftsEntry->DeleteL(*draftsSelection, wait->iStatus);
        wait->Start();
        CleanupStack::PopAndDestroy(wait);
        delete operation;
        }
    CleanupStack::PopAndDestroy(2, draftsEntry);
}

// ---------------------------------------------------------
// ConversationMsgStoreHandler::DeleteMessagesL
// ---------------------------------------------------------
//
CMsvSession& ConversationMsgStoreHandler::GetMsvSession()
{
    return *iMsvSession;
}

// ---------------------------------------------------------
// ConversationMsgStoreHandler::FetchDraftsMessages
// ---------------------------------------------------------
//
void ConversationMsgStoreHandler::FetchDraftMessages(DraftsModel* draftsModel)
    {
    mDraftsModel = draftsModel;
    iState = EReadDrafts;
    TCallBack callback = TCallBack(ProcessDraftMessages, (TAny*) this);
    TRAPD(err, iIdle = CIdle::NewL(CActive::EPriorityStandard));
    if(err == KErrNone)
        {
        iIdle->Start(callback);    
        }
    }

// -----------------------------------------------------------------------------
// ConversationMsgStoreHandler::ProcessDraftMessages
// CIdle callback 
// -----------------------------------------------------------------------------
//
TInt ConversationMsgStoreHandler::ProcessDraftMessages(TAny* aArg)
    {
    ConversationMsgStoreHandler* handler = (ConversationMsgStoreHandler*) aArg; 
    TInt ok = 0;
    TRAPD(err, ok = handler->ProcessDraftMessagesL());
    return ((err == KErrNone) && ok);
    }

// ---------------------------------------------------------
// ConversationMsgStoreHandler::ProcessDraftMessagesL
// ---------------------------------------------------------
//
TInt ConversationMsgStoreHandler::ProcessDraftMessagesL()
    {
    switch(iState)
        {
        case EReadDrafts:
            {
            iDraftEntry = iMsvSession->GetEntryL(KMsvDraftEntryIdValue);
           
            iDraftMessages = iDraftEntry->ChildrenL();
            iDraftMessageCount = iDraftEntry->Count();

            if ( iDraftMessageCount ) 
                {
                iState = EProcessDrafts;
                }
            else
                {
                iState = EComplete; // no draft messages
                mDraftsModel->setReady(); //ready to handle dynamic events
                CleanupL();
                return 0; //DONE
                }
            return 1;
            }
        case EProcessDrafts:
            {
            if ( iDraftMessageCount ) 
                {
                iDraftMessageCount--;
                TMsvEntry entry = iDraftEntry->ChildDataL(
                        iDraftMessages->At(iDraftMessageCount));
                
                // process only visible entries
                if (entry.Visible() && IsMtmSupported(entry.iMtm.iUid))
                {
                //add message to model
                mDraftsModel->addRow(entry);
                }
                //continue to process other draft messages
                iState = EProcessDrafts;
                }
            else
                {
                iState = EComplete;
                mDraftsModel->setReady(); //ready to handle dynamic events
                CleanupL();
                return 0; // DONE
                }
            return 1; 
            }
        }

    return 0;
    }

// -----------------------------------------------------------------------------
// ConversationMsgStoreHandler::CleanupL()
// Helper function for state machine cleanup
// -----------------------------------------------------------------------------
//
void ConversationMsgStoreHandler::CleanupL()
    {
    if ( iDraftEntry )
        {
        delete iDraftEntry;
        iDraftEntry = NULL;
        }

    if ( iDraftMessages )
        {
        iDraftMessages->Reset();
        delete iDraftMessages;
        iDraftMessages = NULL;
        }

    iDraftMessageCount = 0;
    }

// -----------------------------------------------------------------------------
// ConversationMsgStoreHandler::IsMtmSupported()
// 
// -----------------------------------------------------------------------------
//
TBool ConversationMsgStoreHandler::IsMtmSupported(long uid)	
    {
    if ( KSenduiMtmSmsUidValue == uid || KSenduiMtmMmsUidValue == uid )
        {
        return ETrue;
        }	
    return EFalse;
    }
	
//-----------------------------------------------------------------------------
// ConversationMsgStoreHandler::ResendMessage(TMsvId aId)
// 
// -----------------------------------------------------------------------------
//	
	
bool ConversationMsgStoreHandler::ResendMessageL(TMsvId aId)
{
    bool retval = true;
	TMsvId serviceId;
	TMsvEntry msgEntry;
	  
	if(iMsvSession == NULL)
	    {
        return false;
   		}
  TInt err = iMsvSession->GetEntry(aId, serviceId, msgEntry);
    
  if (KErrNone == err)
  {
       TUid mtmUid = msgEntry.iMtm;

       CMsvEntry* entry = iMsvSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId);
       CleanupStack::PushL(entry);

       CMuiuOperationWait* wait = CMuiuOperationWait::NewLC();

       if (mtmUid == KSmsMtmUid)
       {
           UniEditorGenUtils* genUtils = new UniEditorGenUtils();
           CleanupStack::PushL(genUtils);

           // if phone is in offline mode set sending state to suspended
           // so that mtm does not try to send it.
           if (iOfflineSupported && genUtils->IsPhoneOfflineL())
           {
               msgEntry.SetSendingState(KMsvSendStateSuspended);
               msgEntry.iError = KErrGsmOfflineOpNotAllowed;
           }
           else
           {
               msgEntry.SetSendingState(KMsvSendStateWaiting);
           }
           CleanupStack::PopAndDestroy(genUtils);

           // Update message entry with sending state.
           entry->SetEntryL(msgEntry.Id());
           entry->ChangeL(msgEntry);

           TMsvId firstId;
           TSmsUtilities::ServiceIdL(*iMsvSession, firstId);

           entry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
           //send the message
           entry->CopyL(aId, firstId, wait->iStatus);
           wait->Start();
       }
       else if (mtmUid == KMmsMtmUid)
       {
           //send the message
           entry->CopyL(aId, msgEntry.iServiceId, wait->iStatus);
           wait->Start();
       }
       CleanupStack::PopAndDestroy(2, entry);
   }
   else
   {
       User::LeaveIfError(err);
   }
   return retval;
}

//-----------------------------------------------------------------------------
// ConversationMsgStoreHandler::DownloadOperationSupported
// -----------------------------------------------------------------------------
bool ConversationMsgStoreHandler::DownloadOperationSupported(
        const TMsvId& aId)
{
    TMsvId serviceId;
    TMsvEntry msgEntry;
    TInt err = iMsvSession->GetEntry(aId,serviceId,msgEntry);
    
    TCsMmsNotificationMsgState msgstatus = 
            MmsNotificationStatus( aId );

    if ( msgEntry.iType == KUidMsvMessageEntry  )
        {

        if( msgEntry.iMtmData2 & KMmsNewOperationForbidden ) // New Forbidden
            {
            return false;
            }
        // If notification is succesfully routed to app 
        // aContext.iMtmData2 & KMmsMessageRoutedToApplication is ETrue and
        // if notification is NOT succesfully routed to app 
        // aContext.iError is KMmsErrorUnregisteredApplication.
        if(    ( msgEntry.iError == KMmsErrorUnregisteredApplication ) // To unregistered application
            || ( msgEntry.iMtmData2 & KMmsMessageRoutedToApplication ) )
            {
            return false;
            }

        if( msgstatus == EMsgStatusDeleted )
            { // Msg already been deleted from server
            return false;
            }

        if( msgstatus == EMsgStatusReadyForFetching 
            || msgstatus == EMsgStatusFailed )
            {   // Fetch is supported if the msg is waiting or something has been failed
            return true;
            }

        if (    msgstatus == EMsgStatusForwarded 
            &&  msgEntry.iMtmData2 & KMmsStoredInMMBox )
            { // Fetch is supported if it's forwarded and multiple forward is supported
            return true;
            }
        
        if( msgstatus == EMsgStatusExpired )
            {
            return false;
            }
        }
    return false;
}

//---------------------------------------------------------------
// ConversationMsgStoreHandler::setNotificationMessageIdL
// @see header
//---------------------------------------------------------------
void ConversationMsgStoreHandler::setNotificationMessageIdL(int messageId)
{
    // get MMS Notification client mtm & set the content to current entry
    if(iNotificationClient)
    {
        delete iNotificationClient;
        iNotificationClient = NULL;
    }
    CClientMtmRegistry* mtmRegistry = 
            CClientMtmRegistry::NewL( *iMsvSession );
    iNotificationClient = static_cast<CMmsNotificationClientMtm*>( 
                    mtmRegistry->NewMtmL( KUidMsgMMSNotification ));
    iNotificationClient->SwitchCurrentEntryL(messageId);
    iNotificationClient->LoadMessageL();
}

//---------------------------------------------------------------
// ConversationMsgStoreHandler::NotificationMsgSizeL
// @see header
//---------------------------------------------------------------
QString ConversationMsgStoreHandler::NotificationMsgSize()
{
    // Size of message.
    TInt size = iNotificationClient->MessageTransferSize( );

    // read max receive size limit from settings
    TInt maxSize = 0;
    TRAP_IGNORE(maxSize = MaxReceiveSizeLimitL());

    // apply max size limit rule
    if( maxSize > 0 )
    {
        if( size > maxSize )
        {
            size = maxSize;
        }
    }

    TInt fileSize = size / BYTES_TO_KBYTES_FACTOR;
    if ( size % BYTES_TO_KBYTES_FACTOR )
    {
        fileSize++;
    }

    return LOC_MESSAGE_SIZE.arg(fileSize);
}

//---------------------------------------------------------------
// ConversationMsgStoreHandler::NotificationClass
// @see header
//---------------------------------------------------------------
QString ConversationMsgStoreHandler::NotificationClass()
{
    QString notificationClass;
    TInt msgClass = iNotificationClient->MessageClass( );
    switch( msgClass )
    {
        case EMmsClassPersonal:
        {
            notificationClass = LOC_CLASS_PERSONAL;
            break;
        }
        case EMmsClassAdvertisement:
        {
            notificationClass = LOC_CLASS_ADVERTISEMENT;
            break;
        }
        case EMmsClassInformational:
        {
            notificationClass = LOC_CLASS_INFORMATIONAL;
            break;
        }
        default:
        {   // In case no class is returned (0), don't add the field
            break;
        }
    }
    return notificationClass;
}

//---------------------------------------------------------------
// ConversationMsgStoreHandler::NotificationStatus
// @see header
//---------------------------------------------------------------
void ConversationMsgStoreHandler::NotificationStatus(
        int& status,
        QString& statusStr)
{
    // fetch mms notification status from store handler
    // and map as per our UI requirements
    TMsvEntry entry = iNotificationClient->Entry().Entry();
    status = MmsNotificationStatus(entry.Id());
    switch(status)
    {
        case ConvergedMessage::NotifFailed:
        {
            statusStr = LOC_MMS_RETRIEVAL_FAILED;
            break;
        }
        case ConvergedMessage::NotifExpired:
        {
            statusStr = LOC_MMS_NOTIF_EXPIRED;
            break;
        }
        case ConvergedMessage::NotifReadyForFetching:
        {
            statusStr = LOC_MMS_WAITING;
            break;
        }
        case ConvergedMessage::NotifWaiting:
        case ConvergedMessage::NotifRetrieving:
        {
            statusStr = LOC_MMS_RETRIEVING;
            break;
        }
        default:
        {
            // not handled, do nothing
            break;
        }
    }
}

//---------------------------------------------------------------
// ConversationMsgStoreHandler::NotificationExpiryDate
// @see header
//---------------------------------------------------------------
QString ConversationMsgStoreHandler::NotificationExpiryDate()
{
    TTime expiryTime = 0;
    QString expiryTimeStr;

    // get expiry time from entry
    expiryTime = iNotificationClient->ExpiryDate( );
    TLocale locale;
    expiryTime += locale.UniversalTimeOffset();
    if (locale.QueryHomeHasDaylightSavingOn())          
    {
        TTimeIntervalHours daylightSaving(1);          
        expiryTime += daylightSaving;
    }
    
    // create formatted string for the expiry time
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    expiryTime.SecondsFrom(unixEpoch, seconds);
    return LOC_MMS_EXPIRY_DATE.arg(seconds.Int());
}

//-----------------------------------------------------------------------------
// ConversationMsgStoreHandler::DownloadMessageL
// 
// -----------------------------------------------------------------------------
//  
    
TInt ConversationMsgStoreHandler::DownloadMessageL(TMsvId aId)
{
    TMsvId serviceId;
    TMsvEntry msgEntry;

    TInt err = iMsvSession->GetEntry(aId,serviceId,msgEntry);

    if(err!= KErrNone || msgEntry.iMtm!= KUidMsgMMSNotification)
    {
        return KErrGeneral;
    }

    /*if(!iNotificationClient)
    {
        iNotificationClient = static_cast<CMmsNotificationClientMtm*> 
        (iMtmReg->NewMtmL(KUidMsgMMSNotification));
    }
    iNotificationClient->SwitchCurrentEntryL(aId);    */ 
    
    // set context to current entry
    setNotificationMessageIdL(aId);

    TTime currentTime;
    currentTime.HomeTime( );
    TTime expiryTime = iNotificationClient->ExpiryDate( );
    TLocale locale;
    expiryTime += locale.UniversalTimeOffset();
    if (locale.QueryHomeHasDaylightSavingOn())          
    {
        TTimeIntervalHours daylightSaving(1);          
        expiryTime += daylightSaving;
    }

    if( currentTime > expiryTime )
    {   // Message is expired
        return KErrGeneral;
    }

    //Check if the mms client mtm object is already created or not
    if( iMmsClient )
    {
        // If mms client mtm object is already created restore the settings
        iMmsClient->RestoreSettingsL();
    }
    else
    {
        iMmsClient = static_cast<CMmsClientMtm*> (iMtmReg->NewMtmL(KMmsMtmUid));
    }        

    //TODO chk if only AP check is sufficient
    TMsvId service = iMmsClient->DefaultServiceL();
    TBool valid( iMmsClient->ValidateService( service ) == KErrNone );
    if(!valid)
    {
        return KErrNotFound;
    }

    CMuiuOperationWait* wait =CMuiuOperationWait::NewLC();;

    CMsvEntrySelection* sel = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( sel );
    sel->AppendL( aId );    

    CMsvOperation* op = 
            iNotificationClient->FetchMessagesL( *sel, wait->iStatus );

    if( !op )
    {  
        CleanupStack::PopAndDestroy( sel ); // selection
        CleanupStack::PopAndDestroy( wait ); // wait
        return KErrGeneral; 
    }

    CleanupStack::PushL( op );
    wait->Start();
    // Lets ignore the return value of wait

    CleanupStack::PopAndDestroy( op ); // op 
    CleanupStack::PopAndDestroy( sel ); // selection
    CleanupStack::PopAndDestroy( wait ); // wait

    return KErrNone;
}

//----------------------------------------------------------------------------
// ConversationMsgStoreHandler::markAsReadAndGetType
// @see header
//----------------------------------------------------------------------------
void ConversationMsgStoreHandler::markAsReadAndGetType(int msgId,
                                                      int& msgType,
                                                      int& msgSubType)
    {
    msgType = ConvergedMessage::None;
    msgSubType = ConvergedMessage::None;
    
    CMsvEntry* cEntry = NULL;
    TRAPD(err, cEntry = iMsvSession->GetEntryL(msgId));
    if ( err == KErrNone)
        {
        TMsvEntry entry = cEntry->Entry();
        if ( entry.Unread() ) 
            {
            // Mark the entry as read
            entry.SetUnread( EFalse );
            TRAP_IGNORE(cEntry->ChangeL( entry ));
            }
        // extract message type
        extractMsgType(entry,msgType,msgSubType);
        }
    
    delete cEntry;
    }

//----------------------------------------------------------------------------
// ConversationMsgStoreHandler::extractMsgType
// @see header
//----------------------------------------------------------------------------
void ConversationMsgStoreHandler::extractMsgType(const TMsvEntry& entry,
                                    int& msgType,
                                    int& msgSubType)
    {
    msgType = ConvergedMessage::None;
    msgSubType = ConvergedMessage::None;

    switch(entry.iMtm.iUid)   
        {
        case KSenduiMtmSmsUidValue:            
            msgType = ConvergedMessage::Sms;
            if (entry.iBioType == KMsgBioNokiaServiceSentMessage.iUid) {
                msgSubType = ConvergedMessage::NokiaService;
            }
            break;
        case KSenduiMtmBtUidValue:
            msgType = ConvergedMessage::BT;
            break;
        case KSenduiMtmMmsUidValue:        
            msgType = ConvergedMessage::Mms;
            break;
        case KSenduiMMSNotificationUidValue:            
            msgType = ConvergedMessage::MmsNotification;
            break;
        case KSenduiMtmBioUidValue:
            { 
 			if (entry.iMtmData1 == KSenduiMtmBtUidValue) 
				{
            	msgType = ConvergedMessage::BT;

            	if (entry.iBioType == KMsgBioUidVCard.iUid) 
					{	
               		 msgSubType = ConvergedMessage::VCard;
            		}
            	else if (entry.iBioType == KMsgBioUidVCalendar.iUid) 
					{
			
               		 msgSubType = ConvergedMessage::VCal;
            		}

           	 break;
       		 }
            msgType = ConvergedMessage::BioMsg; 

            // based on the biotype uid set message type
            if(entry.iBioType == KMsgBioUidRingingTone.iUid)
                {
                msgSubType = ConvergedMessage::RingingTone;
                }
            else if(entry.iBioType == KMsgBioProvisioningMessage.iUid)
                {
                msgSubType = ConvergedMessage::Provisioning;
                }     
            else if (entry.iBioType == KMsgBioUidVCard.iUid)
                {
                msgSubType = ConvergedMessage::VCard;
                }
            else if (entry.iBioType == KMsgBioUidVCalendar.iUid)
                {
                msgSubType = ConvergedMessage::VCal;
                }
            else if (entry.iBioType == KMsgBioNokiaServiceSentMessage.iUid) {
                msgSubType = ConvergedMessage::NokiaService;
                }
            }
        break;
    default:
        msgType = ConvergedMessage::None;
        break;
    }
}

//----------------------------------------------------------------------------
// ConversationMsgStoreHandler::getMsgSubType
// @see header
//----------------------------------------------------------------------------
int ConversationMsgStoreHandler::getMsgSubType(int msgId)
{
    int msgType = ConvergedMessage::None;
    int msgSubType = ConvergedMessage::None;
    CMsvEntry* cEntry = NULL;
    TRAPD(err, cEntry = iMsvSession->GetEntryL(msgId));
    if (err == KErrNone) {
        TMsvEntry entry = cEntry->Entry();
        extractMsgType(entry, msgType, msgSubType);
    }
    return msgSubType;
}

//----------------------------------------------------------------------------
// ConversationMsgStoreHandler::MaxReceiveSizeLimitL
// @see header
//----------------------------------------------------------------------------
TInt ConversationMsgStoreHandler::MaxReceiveSizeLimitL()
{
    CMmsSettings* settings = CMmsSettings::NewL();
    CleanupStack::PushL( settings );
    iNotificationClient->RestoreSettingsL();
    settings->CopyL( iNotificationClient->MmsSettings() );
    TInt maxSize = static_cast<TInt>(settings->MaximumReceiveSize() );
    CleanupStack::PopAndDestroy( settings );
    return maxSize;
}

// End of file
