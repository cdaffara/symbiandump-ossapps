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
* Description: This file implements class CEmailMessage.
*
*/

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <viewclipartner.h>
#include <vwsdefpartner.h>
#else
#include <viewcli.h>
#include <vwsdef.h>
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

#include "emailmessage.h"
#include "emailaddress.h"
#include "emailapiutils.h"
#include "emailclientapi.hrh"
#include "emailapiutils.h"
#include "emailinterfacefactoryimpl.h"
#include "emailcontent.h"
#include "emailtextcontent.h"
#include "emailmultipart.h"
#include "emailattachment.h"
#include "CFSMailPlugin.h"
#include "CFSMailClient.h"

#include <xqservicerequest.h>
#include "email_services_api.h"

const TInt KSendMessageRequestId = 100;

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailMessage* CEmailMessage::NewL( CPluginData& aPluginData,
                                    CFSMailMessage* aFsMessage,
                                    const TDataOwner aOwner )
    {
    CEmailMessage* message = new ( ELeave ) CEmailMessage( aPluginData, aFsMessage, aOwner );
    CleanupStack::PushL( message );
    message->ConstructL();
    CleanupStack::Pop( message );
    return message;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailMessage::CEmailMessage(
        CPluginData& aPluginData,
        CFSMailMessage *aFsMessage,
        const TDataOwner aOwner)
        : iPluginData( aPluginData ), iPluginMessage( aFsMessage ), iOwner( aOwner )
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::ConstructL()
    {
    User::LeaveIfNull( iPluginMessage );
    iPlugin = iPluginData.ClaimInstanceL();
    if ( iPluginMessage )
        {
        iMessageId = TMessageId( 
            iPluginMessage->GetMessageId().Id(),
            iPluginMessage->GetFolderId().Id(), 
            iPluginMessage->GetMailBoxId().Id() );

        // Copy the message flags
        InitializeFlagValues();
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
CEmailMessage::~CEmailMessage()
    {
    delete iPluginMessage;
    delete iSender;
    delete iReplyTo;
    delete iTextContent;
    delete iContent;
    iPluginData.ReleaseInstance();
    iAttachments.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TEmailTypeId CEmailMessage::InterfaceId() const
    {
    return KEmailIFUidMessage;
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::Release()
    {
    if ( iOwner == EClientOwns )
        {
        delete this;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
const TMessageId& CEmailMessage::MessageId() const
    {
    return iMessageId;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailAddress* CEmailMessage::SenderAddressL() const
    {
    User::LeaveIfNull( iPluginMessage );

    CFSMailAddress* fsAddress = iPluginMessage->GetSender();
    if ( fsAddress )
        {
        if ( !iSender )
            {
            iSender = CEmailAddress::NewL( MEmailAddress::ESender, EAPIOwns );
            }
        iSender->SetAddressL(fsAddress->GetEmailAddress());
        iSender->SetDisplayNameL(fsAddress->GetDisplayName());
        }
    else
        {
        // Sender address not defined. Delete client object as well
        delete iSender;
        iSender = NULL;
        }
    return iSender;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailAddress* CEmailMessage::ReplyToAddressL() const
    {
    User::LeaveIfNull( iPluginMessage );

    const CFSMailAddress& fsAddress = iPluginMessage->GetReplyToAddress();
    if ( &fsAddress )
        {
        if ( !iReplyTo )
            {
            iReplyTo = CEmailAddress::NewL( MEmailAddress::EReplyTo, EAPIOwns );
            }
        iReplyTo->SetAddressL( fsAddress.GetEmailAddress() );
        iReplyTo->SetDisplayNameL( fsAddress.GetDisplayName() );
        }
    else
        {
        delete iReplyTo;
        iReplyTo = NULL;
        }
    return iReplyTo;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SetReplyToAddressL( const MEmailAddress& aSender )
    {
    User::LeaveIfNull( iPluginMessage );
    
    CFSMailAddress *fsAddress = CFSMailAddress::NewLC();
    if ( !iReplyTo )
        {
        iReplyTo = CEmailAddress::NewL( MEmailAddress::EReplyTo, EAPIOwns );
        }
    fsAddress->SetDisplayName( aSender.DisplayName() );
    fsAddress->SetEmailAddress( aSender.Address() );
    iPluginMessage->SetReplyToAddress( fsAddress );
    iReplyTo->SetAddressL( fsAddress->GetEmailAddress() );
    iReplyTo->SetDisplayNameL( fsAddress->GetDisplayName() );
    CleanupStack::Pop();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailMessage::GetRecipientsL( const MEmailAddress::TRole aRole,
        REmailAddressArray& aRecipients ) const
    {
    if( aRole == MEmailAddress::EReplyTo ||
        aRole == MEmailAddress::ESender )
        {
        User::Leave( KErrArgument );
        }
    else 
        {
        User::LeaveIfNull( iPluginMessage );
        
        if( aRole == MEmailAddress::ETo || 
            aRole == MEmailAddress::EUndefined )
            {
            const RPointerArray<CFSMailAddress>& toRecipients =
                iPluginMessage->GetToRecipients();
            ConvertAddressArrayL( 
                    MEmailAddress::ETo, 
                    toRecipients, aRecipients );
            }
        if( aRole == MEmailAddress::ECc || 
            aRole == MEmailAddress::EUndefined )
            {
            const RPointerArray<CFSMailAddress>& ccRecipients =
                iPluginMessage->GetCCRecipients();
            ConvertAddressArrayL( 
                    MEmailAddress::ECc, 
                    ccRecipients, aRecipients );
            }
        if( aRole == MEmailAddress::EBcc || 
            aRole == MEmailAddress::EUndefined )
            {
            const RPointerArray<CFSMailAddress>& bccRecipients =
                iPluginMessage->GetBCCRecipients();
            ConvertAddressArrayL( 
                    MEmailAddress::EBcc, 
                    bccRecipients, aRecipients );
            }
        }
    return aRecipients.Count();
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SetRecipientsL( const MEmailAddress::TRole aRole,
        REmailAddressArray& aRecipients )
    {
    User::LeaveIfNull( iPluginMessage );
    
    TInt count( aRecipients.Count() );

    for( TInt i=0;i<count;i++ )
        {
        const MEmailAddress* address = aRecipients[i];
        CFSMailAddress* fsAddress = CFSMailAddress::NewL();
        fsAddress->SetEmailAddress( address->Address() );
        fsAddress->SetDisplayName( address->DisplayName() );
        
        if( aRole == MEmailAddress::ETo )
            {
            iPluginMessage->AppendToRecipient( fsAddress );
            }
        else if( aRole == MEmailAddress::ECc )
            {
            iPluginMessage->AppendCCRecipient( fsAddress );
            }
        else if( aRole == MEmailAddress::EBcc )
            {
            iPluginMessage->AppendBCCRecipient( fsAddress );
            }
        else
            {
            delete fsAddress;
            User::Leave( KErrArgument );
            }        
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::RemoveRecipientL( const MEmailAddress& /*aRecipient*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TPtrC CEmailMessage::Subject() const
    {
    if ( !iPluginMessage )
        return KNullDesC();
    return iPluginMessage->GetSubject();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void  CEmailMessage::SetSubjectL( const TPtrC& aSubject )
    {
    User::LeaveIfNull( iPluginMessage );
    iPluginMessage->SetSubject( aSubject );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TTime CEmailMessage::Date() const
    {
    TTime time;

    if ( iPluginMessage ) {
        time = iPluginMessage->GetDate();
    }

    return time;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailMessage::Flags() const
    {
    return iFlags;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SetFlag( const TUint aFlag )
    {
    iFlags |= aFlag;
    TUint flag = MapFlags( aFlag );
    if ( iPluginMessage )
        iPluginMessage->SetFlag( flag );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::ResetFlag( const TUint aFlag )
    {
    iFlags &= ~aFlag;
    TUint flag = MapFlags( aFlag );
    if ( iPluginMessage )
        iPluginMessage->ResetFlag( flag );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::InitializeFlagValues()
    {
    if ( !iPluginMessage )
        return;
    // 1st reset member value, then start copying different flags
    iFlags = 0;
    
    // EFlag_Read
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_Read ) )
        {
        iFlags |= EFlag_Read;
        }
    else
        {
        iFlags &= ~EFlag_Read;
        }
    //    EFlag_Read_Locally
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_Read_Locally ) )
        {
        iFlags |= EFlag_Read_Locally;
        }
    else
        {
        iFlags &= ~EFlag_Read_Locally;
        }    
    // EFlag_Low
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_Low ) )
        {
        iFlags |= EFlag_Low;
        }
    else
        {
        iFlags &= ~EFlag_Low;
        }
    // EFlag_Important
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_Important ) )
        {
        iFlags |= EFlag_Important;
        }
    else
        {
        iFlags &= ~EFlag_Important;
        }
    // EFlag_FollowUpComplete
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_FollowUpComplete ) )
        {
        iFlags |= EFlag_FollowUpComplete;
        }
    else
        {
        iFlags &= ~EFlag_FollowUpComplete;
        }
    // EFlag_FollowUp
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_FollowUp ) )
        {
        iFlags |= EFlag_FollowUp;
        }
    else
        {
        iFlags &= ~EFlag_FollowUp;
        }
    // EFlag_Attachments
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_Attachments ) )
        {
        iFlags |= EFlag_Attachments;
        }
    else
        {
        iFlags &= ~EFlag_Attachments;
        }
    // EFlag_Multiple
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_Multiple ) )
        {
        iFlags |= EFlag_Multiple;
        }
    else
        {
        iFlags &= ~EFlag_Multiple;
        }
    // EFlag_CalendarMsg
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_CalendarMsg ) )
        {
        iFlags |= EFlag_CalendarMsg;
        }
    else
        {
        iFlags &= ~EFlag_CalendarMsg;
        }
    // EFlag_Answered
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_Answered ) )
        {
        iFlags |= EFlag_Answered;
        }
    else
        {
        iFlags &= ~EFlag_Answered;
        }
    // EFlag_Forwarded
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_Forwarded ) )
        {
        iFlags |= EFlag_Forwarded;
        }
    else
        {
        iFlags &= ~EFlag_Forwarded;
        }
    // EFlag_OnlyToMe
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_OnlyToMe ) )
        {
        iFlags |= EFlag_OnlyToMe;
        }
    else
        {
        iFlags &= ~EFlag_OnlyToMe;
        }
    // EFlag_RemoteDeleted
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_RemoteDeleted ) )
        {
        iFlags |= EFlag_RemoteDeleted;
        }
    else
        {
        iFlags &= ~EFlag_RemoteDeleted;
        }
    // EFlag_HasMsgSender 
    if ( iPluginMessage->IsFlagSet( EFSMsgFlag_HasMsgSender ) )
        {
        iFlags |= EFlag_HasMsgSender;
        }
    else
        {
        iFlags &= ~EFlag_HasMsgSender;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailMessageContent* CEmailMessage::ContentL() const
    {
    if ( iTextContent )
        {
        return iTextContent;
        }
    if ( iContent )
        {
        return iContent;
        }

    User::LeaveIfNull( iPluginMessage );
    
    RPointerArray<CFSMailMessagePart> parts;
    CleanupResetAndDestroyPushL( parts );
    iPluginMessage->ChildPartsL( parts );
    TInt count( parts.Count() );
    if( count == 0 )
        {
        /* No content, return NULL */
        CleanupStack::PopAndDestroy( &parts ); // in case heap allocated but not used
        return NULL;
        }
    CFSMailMessagePart* part = parts[0];
    TContentType contentType( part->GetContentType() );
    TMessageContentId msgContentId = TMessageContentId(
                        part->GetPartId().Id(),
                        iMessageId.iId,
                        iMessageId.iFolderId.iId,
                        iMessageId.iFolderId.iMailboxId );

    if ( contentType.Equals( KFSMailContentTypeTextPlain ) ||
        contentType.Equals( KFSMailContentTypeTextHtml ) )
        {                                
        iTextContent = CEmailTextContent::NewL(iPluginData, msgContentId, part, EAPIOwns );
        parts[0] = NULL; // ownership of part transferred
        }
    else if ( contentType.Equals( KFSMailContentTypeMultipartMixed ) ||
              contentType.Equals( KFSMailContentTypeMultipartAlternative ) ||
              contentType.Equals( KFSMailContentTypeMultipartDigest ) ||
              contentType.Equals( KFSMailContentTypeMultipartRelated ) ||
              contentType.Equals( KFSMailContentTypeMultipartParallel ) )
        {
        iContent = CEmailMultipart::NewL(iPluginData, msgContentId, part, EAPIOwns);
        parts[0] = NULL; // ownership of part transferred
        }

    CleanupStack::PopAndDestroy( &parts ); // parts

    if (iTextContent)
        {
        return iTextContent;
        }
    return iContent;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SetContentL( const MEmailMessageContent*  aContent )
    {
    User::LeaveIfNull( aContent );
    MEmailTextContent* textContent = aContent->AsTextContentOrNull();
    if ( textContent )
        {
        if ( iTextContent )
            {
            delete iTextContent; // Destroy old content
            }
        iTextContent = dynamic_cast<CEmailTextContent*>( textContent );
        if ( iTextContent )
            {
            iTextContent->SetOwner( EAPIOwns );
            }
        return;
        }
    MEmailMultipart* mPart = aContent->AsMultipartOrNull();
    if ( mPart )
        {
        if ( iContent )
            {
            delete iContent;
            }
        iContent = dynamic_cast<CEmailMultipart*>( mPart );
        if ( iContent )
            {
            iContent->SetOwner( EAPIOwns );
            }
        }    
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SetPlainTextBodyL( const TDesC& aPlainText )
    {
    if ( iTextContent )
        {
        iTextContent->SetTextL( MEmailTextContent::EPlainText, aPlainText );
        return;
        }

    User::LeaveIfNull( iPluginMessage );

    CFSMailMessagePart* msgTextPart = iPluginMessage->PlainTextBodyPartL();
    
    if( !msgTextPart )
        {
        msgTextPart = iPluginMessage->NewChildPartL( TFSMailMsgId(), KFSMailContentTypeTextPlain );
        }
    CleanupStack::PushL( msgTextPart );

    TMessageContentId msgContentId = MessageContentId( msgTextPart->GetPartId().Id() );

    msgTextPart->SetContentType( KFSMailContentTypeTextPlain );
    iTextContent = CEmailTextContent::NewL( iPluginData, msgContentId, msgTextPart, EAPIOwns );
    if (iTextContent)
        {
        iTextContent->SetTextL( MEmailTextContent::EPlainText, aPlainText );
        }
    CleanupStack::Pop( msgTextPart );

    return;

    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailAttachment* CEmailMessage::AddAttachmentL( const TDesC& aFullPath )
    {
    User::LeaveIfNull( iPluginMessage );

    CFSMailMessagePart* part = iPluginMessage->AddNewAttachmentL( aFullPath, TFSMailMsgId() );
    CleanupStack::PushL( part );    
    CEmailAttachment* att = CEmailAttachment::NewLC( iPluginData, iMsgContentId, part, EAPIOwns );
    iAttachments.AppendL( att );
    CleanupStack::Pop( 2, part );

    return att;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
MEmailAttachment* CEmailMessage::AddAttachmentL( RFile& aFile )
    {
    User::LeaveIfNull( iPluginMessage );
    TBufC8 <1> mime;
    CFSMailMessagePart* part = iPluginMessage->AddNewAttachmentL( aFile, mime );
    CleanupStack::PushL( part );
    CEmailAttachment* att = CEmailAttachment::NewLC( iPluginData, iMsgContentId, part, EAPIOwns );
    iAttachments.AppendL( att );

    CleanupStack::Pop( 2, part );

    return att;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
TInt CEmailMessage::GetAttachmentsL( REmailAttachmentArray& aAttachments )
    {
    User::LeaveIfNull( iPluginMessage );

    RPointerArray<CFSMailMessagePart> attachments;
    CleanupResetAndDestroyPushL( attachments );
    iPluginMessage->AttachmentListL( attachments );
    const TInt count( attachments.Count() );
    for (TInt i = 0; i < count; i++)
        {
        TMessageContentId msgContentId = MessageContentId( attachments[i]->GetPartId().Id() );

        CEmailAttachment* att = CEmailAttachment::NewL( 
            iPluginData, msgContentId, attachments[i], EClientOwns );

        aAttachments.AppendL( att );
        }
    CleanupStack::Pop( &attachments );
    return count;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::RemoveAttachmentL( const MEmailAttachment& /*aAttachment*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
const TFolderId& CEmailMessage::ParentFolderId() const
    {
    return iMessageId.iFolderId;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SaveChangesL()
    {
    User::LeaveIfNull( iPluginMessage );
    
    TFSMailMsgId mailboxId( 
            FsMsgId( iPluginData, iMessageId.iFolderId.iMailboxId ) );
    
    iPlugin->StoreMessageL( mailboxId, *iPluginMessage );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::SendL()
    {
    User::LeaveIfNull( iPluginMessage );
    
    if ( iEventLoop.isRunning() )
        User::Leave( KErrInUse );
    
    SaveChangesL();
    iError = KErrNone;
    iPlugin->SendMessageL( *iPluginMessage, *this, KSendMessageRequestId );
    iEventLoop.exec();

    User::LeaveIfError( iError );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::RequestResponseL( TFSProgress aEvent, TInt aRequestId )
    {
    iError = aEvent.iError;

    if ( aRequestId == KSendMessageRequestId &&
            aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete )
        iEventLoop.quit();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
void CEmailMessage::ConvertAddressArrayL(
        const MEmailAddress::TRole aRole,
        const RPointerArray<CFSMailAddress>& aSrc,
        REmailAddressArray& aDst ) const
    {
    for ( TInt i=0; i<aSrc.Count(); i++ )
        {
        CEmailAddress* recipient = CreateAddressLC( aRole, *aSrc[i] );
        aDst.AppendL( recipient );
        CleanupStack::Pop( recipient );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CEmailAddress* CEmailMessage::CreateAddressLC(
        const MEmailAddress::TRole aRole, 
        CFSMailAddress& aFsAddress ) const
    {
    CEmailAddress* address = CEmailAddress::NewLC( aRole, EClientOwns );
    TDesC& temp1 = aFsAddress.GetEmailAddress();
    if ( &temp1 )
        {
        address->SetAddressL( temp1 );
        }
    TDesC& temp2 = aFsAddress.GetDisplayName();
    if ( &temp2 )
        {
        address->SetDisplayNameL( temp2 );
        }   
    return address;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TUint CEmailMessage::MapFlags( const TUint& aFlag )
    {
    TUint flag = 0;
    switch( aFlag )
        {
        case EFlag_Read:
            flag = EFSMsgFlag_Read;
            break;
        case EFlag_Read_Locally:
            flag = EFSMsgFlag_Read_Locally;
            break;
        case EFlag_Low:
            flag = EFSMsgFlag_Low;
            break;
        case EFlag_Important:
            flag = EFSMsgFlag_Important;
            break;
        case EFlag_FollowUpComplete:
            flag = EFSMsgFlag_FollowUpComplete;
            break;
        case EFlag_FollowUp:
            flag = EFSMsgFlag_FollowUp;
            break;
        case EFlag_Attachments:
            flag = EFSMsgFlag_Attachments;
            break;
        case EFlag_Multiple:
            flag = EFSMsgFlag_Multiple;
            break;
        case EFlag_CalendarMsg:
            flag = EFSMsgFlag_CalendarMsg;
            break;
        case EFlag_Answered:
            flag = EFSMsgFlag_Answered;
            break;
        case EFlag_Forwarded:
            flag = EFSMsgFlag_Forwarded;
            break;
        case EFlag_OnlyToMe:
            flag = EFSMsgFlag_OnlyToMe;
            break;
        case EFlag_RemoteDeleted:
            flag = EFSMsgFlag_RemoteDeleted;
            break;
        case EFlag_HasMsgSender:
            flag = EFSMsgFlag_HasMsgSender;
            break;
        default:
            break;
        }
    return flag;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMessage::ShowMessageViewerL()
    {   
    bool syncronous;

    XQServiceRequest request( XQI_EMAIL_MESSAGE_VIEW,
                              XQOP_EMAIL_MESSAGE_VIEW,
                              syncronous );

    TFSMailMsgId mailboxId = FsMsgId( iPluginData, iMessageId.iFolderId.iMailboxId );
    TFSMailMsgId folderId = FsMsgId( iPluginData, iMessageId.iFolderId );
    TFSMailMsgId messageId = FsMsgId( iPluginData, iMessageId );

    QList<QVariant> idList;
    idList.append( mailboxId.Id() );
    idList.append( folderId.Id() );
    idList.append( messageId.Id() );

    /*
        Uncomment the following when the viewer service API operation definition
        (XQOP_EMAIL_MESSAGE_VIEW) has been updated.
    
    QVariant idListAsVariant = QVariant::fromValue( idList );

    // Add the message list view into the view stack.
    quint64 flags( EmailBackReturnsToMessageList );

    QList<QVariant> argumentList;
    argumentList.append( idListAsVariant );
    argumentList.append( flags );

    request.setArguments( argumentList );    
    */
    
    // Remove the following line when the viewer service API operation
    // definition has been updated.
    request.setArguments( idList );

    QVariant returnValue;

    if ( !request.send( returnValue ) )
        {
        User::Leave( KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
// Launches Email application and new reply message in editor. 
// The method follows "fire and forget" pattern, returns immediately.
// -----------------------------------------------------------------------------
void CEmailMessage::ReplyToMessageL( const TBool /*aReplyToAll*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMessage::ForwardMessageL()
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TMessageContentId CEmailMessage::MessageContentId( TEntryId aContentId ) const
    {
    TMessageContentId msgContentId = TMessageContentId( 
                    aContentId,
                    iMessageId.iId,
                    iMessageId.iFolderId.iId,
                    iMessageId.iFolderId.iMailboxId );
    return msgContentId;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TContentType::TContentType( const TDesC& aContentType ) : iContentType( aContentType )
    {
    _LIT( KSeparator, ";" );
    TInt end = aContentType.Find( KSeparator );
    if ( end != KErrNotFound )
        {
        iContentType.Set( aContentType.Left( end ) );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TBool TContentType::Equals( const TDesC& aContentType )
    {
    TBool ret = iContentType.CompareF( aContentType );
    if ( ret == 0  )
        return ETrue;
    else
        return EFalse;
    }

// End of file
