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
 * Description:   Reads message information from message store.
 *
 */

#include <msvids.h>
#include <msvstd.h>
#include <mmsconst.h>
#include <SendUiConsts.h>
#include <mtclreg.h>    // for CClientMtmRegistry
#include <mmsnotificationclient.h>
#include <mmssettings.h>
#include <xqconversions.h> // from xqutils
#include <mmsconst.h>
#include <HbExtendedLocale>
#include <QDateTime>
#include <hbglobal.h>

#include "msgstorehandler.h"
#include "msgbiouids.h"
#include "convergedmessage.h"
#include "msgcontacthandler.h"

// CONSTANTS
_LIT(KUnixEpoch, "19700000:000000.000000");
#define BYTES_TO_KBYTES_FACTOR 1024

// @see hbi18ndef.h
static const char DATE_FORMAT[] = r_qtn_date_short_with_zero;
static const char TIME_FORMAT[] = r_qtn_time_usual_with_zero;

// LOCALIZATION
#define LOC_MESSAGE_SIZE hbTrId("txt_messages_dialog_size")
#define LOC_CLASS_ADVERTISEMENT hbTrId("txt_messages_dialog_class_advertisement")
#define LOC_CLASS_INFORMATIONAL hbTrId("txt_messages_dialog_class_informational")
#define LOC_CLASS_PERSONAL hbTrId("txt_messages_dialog_class_personal")
#define LOC_MMS_RETRIEVAL_FAILED hbTrId("txt_messaging_dialog_mms_retrieval_failed")
#define LOC_MMS_NOTIF_EXPIRED hbTrId("txt_messaging_list_message_expired")  
#define LOC_MMS_WAITING hbTrId("txt_messages_dialog_multimedia_message_waiting")
#define LOC_MMS_RETRIEVING hbTrId("txt_messaging_list_retrieving_message")   
#define LOC_MMS_EXPIRY_DATE hbTrId("txt_messages_dialog_expiry_date")

//----------------------------------------------------------------------------
// MsgStoreHandler::MsgStoreHandler
// @see header
//----------------------------------------------------------------------------
MsgStoreHandler::MsgStoreHandler():
iMsvSession(NULL), iNotificationClient(NULL), iMtmReg(NULL)
{
    InitL();
}

//----------------------------------------------------------------------------
// MsgStoreHandler::~MsgStoreHandler
// @see header
//----------------------------------------------------------------------------
MsgStoreHandler::~MsgStoreHandler()
{
    if(iNotificationClient)
    {
        delete iNotificationClient;
        iNotificationClient = NULL;
    }

    if(iMtmReg)
    {
        delete iMtmReg;
        iMtmReg = NULL;
    }

    if(iMsvSession)
    {
        delete iMsvSession;
        iMsvSession = NULL;
    }
}

//----------------------------------------------------------------------------
// MsgStoreHandler::HandleSessionEventL
// @see header
//----------------------------------------------------------------------------
void MsgStoreHandler::HandleSessionEventL(TMsvSessionEvent /*aEvent*/,
        TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
    // Nothing to do
}

//----------------------------------------------------------------------------
// MsgStoreHandler::InitL
// @see header
//----------------------------------------------------------------------------
void MsgStoreHandler::InitL()
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
}

//----------------------------------------------------------------------------
// MsgStoreHandler::MessageLocation
// @see header
//----------------------------------------------------------------------------
TMsvId MsgStoreHandler::MessageLocation( TMsvId aMsgId ) const
{
    TMsvId parent = NULL;
    CMsvEntry* cEntry = NULL;
    TRAPD(err, cEntry = iMsvSession->GetEntryL(aMsgId));
    if(err == KErrNone)
    {
        TMsvEntry msvEntry = cEntry->Entry();
        parent = msvEntry.Parent();
    }
    delete cEntry;
    return parent;
}

//----------------------------------------------------------------------------
// MsgStoreHandler::markAsRead
// @see header
//----------------------------------------------------------------------------
void MsgStoreHandler::markAsRead(int msgId)
{
    CMsvEntry* cEntry = NULL;
    TRAPD(err, cEntry = iMsvSession->GetEntryL(msgId));
    if(err == KErrNone)
    {
        TMsvEntry entry = cEntry->Entry();
        if ( entry.Unread() ) 
        {
            // Mark the entry as read
            entry.SetUnread( EFalse );
            cEntry->ChangeL( entry );
        }
    }
    delete cEntry;
}

//----------------------------------------------------------------------------
// MsgStoreHandler::getMsgTypeInfo
// @see header
//----------------------------------------------------------------------------
void MsgStoreHandler::getMsgTypeInfo(int msgId,
                                     int& msgType,
                                     int& msgSubType)
{
    msgType = ConvergedMessage::None;
    msgSubType = ConvergedMessage::None;

    CMsvEntry* cEntry = NULL;
    TRAPD(err, cEntry = iMsvSession->GetEntryL(msgId));
    if(err != KErrNone)
    {
        // error in getting the entry, return back
        delete cEntry;
        return;
    }

    TMsvEntry entry = cEntry->Entry();
    switch(entry.iMtm.iUid)   
    {
        case KSenduiMtmSmsUidValue:
        {
            msgType = ConvergedMessage::Sms;
            if (entry.iBioType == KMsgBioUidVCard.iUid)
            {
                msgSubType = ConvergedMessage::VCard;
            }
            break;
        }
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
            break;
        }
        default:
            msgType = ConvergedMessage::None;       
            break;
    }
    delete cEntry;
}

//----------------------------------------------------------------------------
// MsgStoreHandler::deleteMessage
// @see header
//----------------------------------------------------------------------------
void MsgStoreHandler::deleteMessage(int msgId)
{
    iMsvSession->RemoveEntry(msgId);
}

//----------------------------------------------------------------------------
// MsgStoreHandler::isDraftMessage
// @see header
//----------------------------------------------------------------------------
bool MsgStoreHandler::isDraftMessage(int msgId)
{
    return (KMsvDraftEntryIdValue == MessageLocation(msgId));
}

//----------------------------------------------------------------------------
// MsgStoreHandler::locked
// @see header
//----------------------------------------------------------------------------
bool MsgStoreHandler::locked(int msgId)
{
    bool msglocked = false;
    CMsvEntry* cEntry = NULL;
    TRAPD(err, cEntry = iMsvSession->GetEntryL(msgId));
    if(err != KErrNone)
    {
        // error in getting the entry e.g. already deleted message
        msglocked = true;
    }
    else
    {
        // Outbox messages are locked and hence cann't be viewed.
        // However, make an exception for failed messages.
        TMsvEntry entry = cEntry->Entry();
        if( (KMsvGlobalOutBoxIndexEntryIdValue == entry.Parent()) && 
            (KMsvSendStateFailed != entry.SendingState()) )
        {
            msglocked = true;
        }
    }
    delete cEntry;
    return msglocked;
}

//----------------------------------------------------------------------------
// MsgStoreHandler::exists
// @see header
//----------------------------------------------------------------------------
bool MsgStoreHandler::exists(int msgId)
{
    bool msgexists = true;
    CMsvEntry* cEntry = NULL;
    TRAPD(err, cEntry = iMsvSession->GetEntryL(msgId));
    if(err != KErrNone)
    {
        // error in getting the entry e.g. already deleted message
        msgexists = false;
    }
    delete cEntry;
    return msgexists;
}

//----------------------------------------------------------------------------
// MsgStoreHandler::setNotificationMessageId
// @see header
//----------------------------------------------------------------------------
int MsgStoreHandler::setNotificationMessageId(int msgId)
{
    TRAPD(err, SetNotificationMessageIdL(msgId));
    return err;
}

//----------------------------------------------------------------------------
// MsgStoreHandler::SetNotificationMessageIdL
// @see header
//----------------------------------------------------------------------------
void MsgStoreHandler::SetNotificationMessageIdL( TMsvId aMsgId )
{
    // get MMS Notification client mtm & set the content to current entry
    if(iNotificationClient)
    {
        delete iNotificationClient;
        iNotificationClient = NULL;
    }
    iNotificationClient = static_cast<CMmsNotificationClientMtm*>( 
                    iMtmReg->NewMtmL( KUidMsgMMSNotification ));
    iNotificationClient->SwitchCurrentEntryL(aMsgId);
    iNotificationClient->LoadMessageL();
}

//----------------------------------------------------------------------------
// MsgStoreHandler::MaxReceiveSizeLimitL
// @see header
//----------------------------------------------------------------------------
TInt MsgStoreHandler::MaxReceiveSizeLimitL()
{
    CMmsSettings* settings = CMmsSettings::NewL();
    CleanupStack::PushL( settings );
    iNotificationClient->RestoreSettingsL();
    settings->CopyL( iNotificationClient->MmsSettings() );
    TInt maxSize = static_cast<TInt>(settings->MaximumReceiveSize() );
    CleanupStack::PopAndDestroy( settings );
    return maxSize;
}

// ---------------------------------------------------------
// MsgStoreHandler::MmsNotificationStatus
// @see header
// ---------------------------------------------------------
TCsMmsNotificationMsgState 
MsgStoreHandler::MmsNotificationStatus( TInt aMsvId )
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
// MsgStoreHandler::OperationOngoing
// @see header
// ---------------------------------------------------------
TBool MsgStoreHandler::OperationOngoing( 
        const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationOngoing 
            &&  !( aEntry.iMtmData2 & KMmsOperationFinished ) );
    }

// ---------------------------------------------------------
// MsgStoreHandler::OperationFinished
// @see header
// ---------------------------------------------------------
TBool MsgStoreHandler::OperationFinished( 
    const TMsvEntry& aEntry ) const
    {
    return (    aEntry.iMtmData2 & KMmsOperationFinished
            &&  !( aEntry.iMtmData2 & KMmsOperationOngoing ) );
    }

// ---------------------------------------------------------
// MsgStoreHandler::notificationSender
// @see header
// ---------------------------------------------------------
QString MsgStoreHandler::notificationSender()
{
    QString retVal;
    QString sender;
    if(iNotificationClient)
    {
        TPtrC sndr = iNotificationClient->Sender();
        sender = XQConversions::s60DescToQString(sndr);
    }
    
    QString displayName;
    int count;
    if(-1 != MsgContactHandler::resolveContactDisplayName(
            sender, displayName, count))
    {
        retVal = QString("%1(%2)").arg(displayName).arg(sender);
    }
    else
    {
        retVal = sender;
    }

    return retVal;
}

//---------------------------------------------------------------
// MsgStoreHandler::notificationTimeStamp
// @see header
//---------------------------------------------------------------
QString MsgStoreHandler::notificationTimeStamp()
{
    // Null Check
    if(!iNotificationClient)
    {
        return QString();
    }

    TMsvEntry entry = iNotificationClient->Entry().Entry();
    TTime unixEpoch(KUnixEpoch);
    TTimeIntervalSeconds seconds;
    TTime timeStamp(entry.iDate.Int64());
    timeStamp.SecondsFrom(unixEpoch, seconds);
    
    QDateTime dateTime;
    dateTime.setTime_t(seconds.Int());
    HbExtendedLocale locale = HbExtendedLocale::system();

    QString timeStampStr;
    if (dateTime.date() == QDateTime::currentDateTime().date()) {
        timeStampStr = locale.format(dateTime.time(), TIME_FORMAT);
    }
    else {
        timeStampStr = locale.format(dateTime.date(), DATE_FORMAT);
    }
    return timeStampStr;
}

//---------------------------------------------------------------
// MsgStoreHandler::notificationSubject
// @see header
//---------------------------------------------------------------
QString MsgStoreHandler::notificationSubject()
{
    QString subject;
    if(iNotificationClient)
    {
        TPtrC sub =  iNotificationClient->SubjectL();
        subject = XQConversions::s60DescToQString(sub);
    }
    return subject;
}
//---------------------------------------------------------------
// MsgStoreHandler::notificationMsgSize
// @see header
//---------------------------------------------------------------
QString MsgStoreHandler::notificationMsgSize()
{
    // Null check
    if(!iNotificationClient)
    {
        return LOC_MESSAGE_SIZE.arg(0);
    }

    // Size of message.
    TInt size = iNotificationClient->MessageTransferSize();

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
// MsgStoreHandler::notificationClass
// @see header
//---------------------------------------------------------------
QString MsgStoreHandler::notificationClass()
{
    QString notificationClass;
    // Null Check
    if(!iNotificationClient)
    {
        return notificationClass;
    }

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
// MsgStoreHandler::notificationStatus
// @see header
//---------------------------------------------------------------
void MsgStoreHandler::notificationStatus(
        int& status,
        QString& statusStr)
{
    // Null Check
    if(!iNotificationClient)
    {
        return;
    }

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
// MsgStoreHandler::notificationExpiryDate
// @see header
//---------------------------------------------------------------
QString MsgStoreHandler::notificationExpiryDate()
{
    // Null Check
    if(!iNotificationClient)
    {
        return QString();
    }

    // get expiry time from entry
    TTime expiryTime = iNotificationClient->ExpiryDate( );
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

// End of file
