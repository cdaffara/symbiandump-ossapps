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
* Description: This file implements class CEmailMailbox.
*
*/

#include <e32cmn.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <viewclipartner.h>
#include <vwsdefpartner.h>
#else
#include <viewcli.h>
#include <vwsdef.h>
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

#include <mmailboxsyncobserver.h>
#include "emailmailbox.h"
#include "emailapiutils.h"
#include "emailaddress.h"
#include "emailfolder.h"
#include "emailmessage.h"
#include "emailmessagesearch.h"
#include "CFSMailClient.h"
#include "emailclientapiimpl.h"
#include "CFSMailPlugin.h"
#include "CFSMailBox.h"
#include "CFSMailFolder.h"
#include "emailclientapi.hrh"

#include <xqservicerequest.h>
#include "email_services_api.h"

// Constants

#include <QStringList>

_LIT( KNewLine, "\n" );

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CEmailMailbox* CEmailMailbox::NewL(
    CPluginData& aPluginData,
    const TMailboxId& aMailboxId )
    {
    CEmailMailbox* self = new ( ELeave ) CEmailMailbox( aPluginData, aMailboxId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CEmailMailbox::CEmailMailbox(
    CPluginData& aPluginData,
    const TMailboxId& aMailboxId )
    : iPluginData( aPluginData ),
    iMailboxId( aMailboxId.iId )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::ConstructL()
    {
    iPlugin = iPluginData.ClaimInstanceL();
    iEventMapper = new ( ELeave ) TObserverEventMapper(
        iPlugin, iPluginData.Uid(), iMailboxId );
    iFsMailbox = iPlugin->GetMailBoxByUidL( FsMailboxId() );
    iSyncObserver = new ( ELeave ) CEmailMailbox::CEmailRequestObserver();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CEmailMailbox::~CEmailMailbox()
    {
    delete iFsMailbox;
    delete iEventMapper;
    delete iAddress;
    iPluginData.ReleaseInstance();
    delete iSyncObserver;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TEmailTypeId CEmailMailbox::InterfaceId() const
    {
    return KEmailIFUidMailbox;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::Release()
    {
    delete this;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TMailboxId CEmailMailbox::MailboxId() const
    {
    return iMailboxId;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MEmailAddress* CEmailMailbox::AddressL() const
    {
    if ( !iAddress )
        {
        iAddress = CEmailAddress::NewL(
            MEmailAddress::ESender, EAPIOwns );
        iAddress->SetDisplayNameL( iFsMailbox->GetName() );
        iAddress->SetAddressL( iFsMailbox->OwnMailAddress().GetEmailAddress() );
        }
    return iAddress;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TPtrC CEmailMailbox::MailboxName() const
    {
    return iFsMailbox->GetName();
    }

// -----------------------------------------------------------------------------
// Constructs and returns type mailbox id of internal data type TFSMailMsgId
// -----------------------------------------------------------------------------
TFSMailMsgId CEmailMailbox::FsMailboxId() const
    {
    return FsMsgId( iPluginData, iMailboxId );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TInt CEmailMailbox::GetFoldersL( RFolderArray& aFolders ) const
    {
    TFSMailMsgId rootId = TFSMailMsgId();
    RPointerArray<CFSMailFolder> folders;
    CleanupResetAndDestroyPushL( folders );
    iPlugin->ListFoldersL( FsMailboxId(), rootId, folders );
    TInt res( folders.Count() );
    for ( TInt i = 0; i < res; i++ )
        {
        const CFSMailFolder* fsfolder = folders[i];
        const TEntryId id = fsfolder->GetFolderId().Id();
        const TFolderId folderId( id, iMailboxId );
        CEmailFolder* folder = CEmailFolder::NewLC( iPluginData, folderId, folders[i] );
        aFolders.AppendL( folder );
        CleanupStack::Pop( folder );
        }
    CleanupStack::Pop( &folders );
    folders.Close(); // close but don't delete folders because they are
                     // owned by CEmailFolder
    return res;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MEmailFolder* CEmailMailbox::FolderL( const TFolderId& aFolderId ) const
    {
    CFSMailFolder* fsFolder = iPlugin->GetFolderByUidL(
            FsMsgId( iPluginData, iMailboxId ),
            FsMsgId( iPluginData, aFolderId ) );
    CleanupStack::PushL( fsFolder );

    CEmailFolder* folder = CEmailFolder::NewL( iPluginData,
                aFolderId, fsFolder );
    CleanupStack::Pop( fsFolder );

    return folder;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MEmailFolder* CEmailMailbox::FolderByTypeL(
    const TFolderType aFolderType ) const
    {
    TFSMailMsgId fsFolderId;

    switch ( aFolderType )
        {
        case EInbox:
            fsFolderId = iPlugin->GetStandardFolderIdL( FsMailboxId(), EFSInbox );
            break;
        case EOutbox:
            fsFolderId = iPlugin->GetStandardFolderIdL( FsMailboxId(), EFSOutbox );
            break;
        case EDrafts:
            fsFolderId = iPlugin->GetStandardFolderIdL( FsMailboxId(), EFSDraftsFolder );
            break;
        case EDeleted:
            fsFolderId = iPlugin->GetStandardFolderIdL( FsMailboxId(), EFSDeleted );
            break;
        case ESent:
            fsFolderId = iPlugin->GetStandardFolderIdL( FsMailboxId(), EFSSentFolder );
            break;
        case EOther:
        default:
            User::Leave( KErrNotFound );
            break;
        }

    const TFolderId folderId( fsFolderId.Id(), iMailboxId );

    CFSMailFolder* fsFolder = iPlugin->GetFolderByUidL(
            FsMsgId( iPluginData, iMailboxId ),
            fsFolderId );
    CleanupStack::PushL( fsFolder );

    CEmailFolder* folder = CEmailFolder::NewL( iPluginData,
            folderId,
            fsFolder );

    CleanupStack::Pop( fsFolder );

    return folder;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MEmailMessage* CEmailMailbox::MessageL( const TMessageId& aMessageId )
    {
    CFSMailMessage *fsMessage = iPlugin->GetMessageByUidL(
            FsMsgId( iPluginData, aMessageId.iFolderId.iMailboxId ),
            FsMsgId( iPluginData, aMessageId.iFolderId ),
            FsMsgId( iPluginData, aMessageId ),
            EFSMsgDataEnvelope );

    CleanupStack::PushL( fsMessage );
    CEmailMessage* message = CEmailMessage::NewL( iPluginData, fsMessage, EClientOwns );
    CleanupStack::Pop( fsMessage );

    return message;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MEmailMessage* CEmailMailbox::CreateDraftMessageL() const
    {
    CFSMailMessage* fsMessage = iFsMailbox->CreateMessageToSend();
    User::LeaveIfNull( fsMessage );
    CleanupStack::PushL( fsMessage );
    MEmailMessage* message = CEmailMessage::NewL( iPluginData, fsMessage, EClientOwns );
    CleanupStack::Pop( fsMessage );

    return message;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MEmailMessage* CEmailMailbox::CreateReplyMessageL( 
        const TMessageId& aMessageId, const TBool aReplyToAll ) const
    {
    CFSMailMessage* fsMessage = iFsMailbox->CreateReplyMessage( 
        FsMsgId( iPluginData, aMessageId ), aReplyToAll );
    User::LeaveIfNull( fsMessage );
    CleanupStack::PushL( fsMessage );
    MEmailMessage* message = CEmailMessage::NewL( iPluginData, fsMessage, EClientOwns );
    CleanupStack::Pop( fsMessage );

    return message;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MEmailMessage* CEmailMailbox::CreateForwardMessageL( const TMessageId& aMessageId ) const
    {
    CFSMailMessage* fsMessage = iFsMailbox->CreateForwardMessage(
        FsMsgId( iPluginData, aMessageId ), KNewLine() );
    User::LeaveIfNull( fsMessage );
    CleanupStack::PushL( fsMessage );
    MEmailMessage* message = CEmailMessage::NewL( iPluginData, fsMessage, EClientOwns );
    CleanupStack::Pop( fsMessage );

    return message;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::SynchroniseL( MMailboxSyncObserver& aObserver )
    {
    iSyncObserver->SetObserverL( &aObserver );
    iRequestId = iFsMailbox->RefreshNowL( *iSyncObserver );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::CancelSynchronise()
    {
    iFsMailbox->CancelSearch();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::RegisterObserverL( MMailboxContentObserver& aObserver )
    {
    iEventMapper->AddObserverL( aObserver );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::UnregisterObserver( MMailboxContentObserver& aObserver )
    {
    iEventMapper->RemoveObserver( aObserver );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
MEmailMessageSearchAsync* CEmailMailbox::MessageSearchL()
    {
    MEmailMessageSearchAsync* searchAPI = 
        CEmailMessageSearchAsync::NewL( iPluginData, iMailboxId );
    return searchAPI;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::ShowInboxL()
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::EditNewMessageL()
    {
    bool syncronous;

    XQServiceRequest request( XQI_EMAIL_MESSAGE_SEND,
                              XQOP_EMAIL_MESSAGE_SEND,
                              syncronous );

    QVariant returnValue;

    if ( !request.send( returnValue ) )
        {
        User::Leave( KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CEmailMailbox::TObserverEventMapper::TObserverEventMapper(
    CFSMailPlugin* aPlugin,
    TUid aPluginUid, const TMailboxId& aMailboxId )
     : iPlugin( aPlugin ),
       iFsMailboxId( aPluginUid.iUid, aMailboxId.iId )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CEmailMailbox::TObserverEventMapper::~TObserverEventMapper()
    {
    iClientObservers.Close();
    }

// -----------------------------------------------------------------------------
// Adds mailbox observer and subscribes to plugin mailbox events if there
// were no previous observers.
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::AddObserverL( 
        MMailboxContentObserver& aObserver )
    {
    TIdentityRelation<MMailboxContentObserver> relation( 
        &CEmailMailbox::TObserverEventMapper::Equals );
    if ( iClientObservers.Find( &aObserver, relation ) == KErrNotFound )
        {
        const TInt count( iClientObservers.Count() );
        if  ( !count )
            {
            iPlugin->SubscribeMailboxEventsL( iFsMailboxId, *this );
            }
        iClientObservers.AppendL( &aObserver );
        }
    }

// -----------------------------------------------------------------------------
// Removes an observer and removes event subscription if the observer was
// last in observer array.
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::RemoveObserver( 
        MMailboxContentObserver& aObserver )
    {
    TIdentityRelation<MMailboxContentObserver> relation( 
        &CEmailMailbox::TObserverEventMapper::Equals );
    const TInt index( iClientObservers.Find( &aObserver, relation ) );
    if ( index != KErrNotFound )
        {
        iClientObservers.Remove( index );
        const TInt count( iClientObservers.Count() );
        if  ( !count )
            {
            iPlugin->UnsubscribeMailboxEvents( iFsMailboxId, *this );
            }
        }
    }

// -----------------------------------------------------------------------------
// Maps protocol a plugin event to client event
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::EventL(
    TFSMailEvent aEvent,
    TFSMailMsgId aMailbox,
    TAny* aParam1,
    TAny* aParam2,
    TAny* aParam3 )
    {
    const TEventMapFunc KMailboxEventHandlers[] = {
    &CEmailMailbox::TObserverEventMapper::IgnoreEventL,
    &CEmailMailbox::TObserverEventMapper::IgnoreEventL,
    &CEmailMailbox::TObserverEventMapper::IgnoreEventL,
    &CEmailMailbox::TObserverEventMapper::IgnoreEventL,
    &CEmailMailbox::TObserverEventMapper::IgnoreEventL,
    &CEmailMailbox::TObserverEventMapper::IgnoreEventL,
    &CEmailMailbox::TObserverEventMapper::IgnoreEventL,
    &CEmailMailbox::TObserverEventMapper::IgnoreEventL,
    &CEmailMailbox::TObserverEventMapper::IgnoreEventL,
    &CEmailMailbox::TObserverEventMapper::IgnoreEventL,
    &CEmailMailbox::TObserverEventMapper::IgnoreEventL,
    &CEmailMailbox::TObserverEventMapper::NewMessageL,
    &CEmailMailbox::TObserverEventMapper::MessageChangedL,
    &CEmailMailbox::TObserverEventMapper::MessageDeletedL,
    &CEmailMailbox::TObserverEventMapper::MessageMoved,
    &CEmailMailbox::TObserverEventMapper::MessageCopiedL,
    &CEmailMailbox::TObserverEventMapper::NewFolderL,
    &CEmailMailbox::TObserverEventMapper::FolderChangeL,
    &CEmailMailbox::TObserverEventMapper::FoldersDeletedL,
    &CEmailMailbox::TObserverEventMapper::FoldersMovedL,
    &CEmailMailbox::TObserverEventMapper::ExceptionL
    };

    TMailboxId id( aMailbox.Id() );
    // boundary check
    const TInt index( aEvent );
    if ( index < sizeof( KMailboxEventHandlers ) / sizeof( KMailboxEventHandlers[ index ] ) )
        {
        // call event handler function
        TEventMapFunc method = KMailboxEventHandlers[ index ];
        (this->*method)(id, aParam1,aParam2,aParam3 );
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::ConvertParamsL( 
        TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, 
        REmailMessageIdArray& aMessageIds, TFolderId& aFolderId )
    {
    RArray<TFSMailMsgId>* newEntries( static_cast< RArray<TFSMailMsgId>* >( aParam1 ) );
    CleanupClosePushL( *newEntries );
    TFSMailMsgId* parentFolder = static_cast<TFSMailMsgId*>( aParam2 );
    aFolderId = TFolderId( parentFolder->Id(), aMailbox );

    for ( TInt j = 0; j < newEntries->Count(); j++ ) 
        {
        TFSMailMsgId fsId(( *newEntries )[j] );
        TMessageId messageId( fsId.Id(), aFolderId.iId, aMailbox );
        aMessageIds.Append( messageId );
        }

    CleanupStack::PopAndDestroy( newEntries );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::IgnoreEventL(
    TMailboxId /*aMailbox*/, TAny* /*aParam1*/, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::NewMessageL(
    TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* /*aParam3*/ )
    {
    REmailMessageIdArray messageIds;
    TFolderId folderId;
    ConvertParamsL( aMailbox, aParam1, aParam2, messageIds, folderId );
    CleanupClosePushL( messageIds );

    for ( TInt i = 0; i < iClientObservers.Count(); i++ )
        {
        MMailboxContentObserver* observer = iClientObservers[i];
        if (observer)
            observer->NewMessageEventL( aMailbox, messageIds, folderId );
        }
    
    CleanupStack::PopAndDestroy( &messageIds );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::MessageChangedL(
    TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* /*aParam3*/ )
    {
    REmailMessageIdArray messageIds;
    TFolderId folderId;
    ConvertParamsL( aMailbox, aParam1, aParam2, messageIds, folderId );
    CleanupClosePushL( messageIds );

    for ( TInt i = 0; i < iClientObservers.Count(); i++ )
        {
        MMailboxContentObserver* observer = iClientObservers[i];
        if (observer)
            observer->MessageChangedEventL( aMailbox, messageIds, folderId );
        }
    CleanupStack::PopAndDestroy( &messageIds );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::MessageDeletedL(
    TMailboxId aMailbox, TAny* aParam1, TAny* aParam2, TAny* /*aParam3*/ )
    {
    REmailMessageIdArray messageIds;    
    TFolderId folderId;
    ConvertParamsL( aMailbox, aParam1, aParam2, messageIds, folderId );
    CleanupClosePushL( messageIds );

    for ( TInt i = 0; i < iClientObservers.Count(); i++ )
        {
        MMailboxContentObserver* observer = iClientObservers[i];
        if (observer)
            observer->MessageDeletedEventL( aMailbox, messageIds, folderId );
        }
    
    CleanupStack::PopAndDestroy( &messageIds );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::MessageMoved(
    TMailboxId /*aMailbox*/, TAny* /*aParam1*/, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::MessageCopiedL(
    TMailboxId /*aMailbox*/, TAny* /*aParam1*/, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::NewFolderL(
    TMailboxId /*aMailbox*/, TAny* /*aParam1*/, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::FolderChangeL(
    TMailboxId /*aMailbox*/, TAny* /*aParam1*/, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::FoldersDeletedL(
    TMailboxId /*aMailbox*/, TAny* /*aParam1*/, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::FoldersMovedL(
    TMailboxId /*aMailbox*/, TAny* /*aParam1*/, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::TObserverEventMapper::ExceptionL(
    TMailboxId /*aMailbox*/, TAny* /*aParam1*/, TAny* /*aParam2*/, TAny* /*aParam3*/ )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
TBool CEmailMailbox::TObserverEventMapper::Equals( 
        const MMailboxContentObserver& a1, const MMailboxContentObserver& a2 )
    {
    return ( &a1 == &a2 );
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
CEmailMailbox::CEmailRequestObserver::CEmailRequestObserver() : iObserver( NULL )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::CEmailRequestObserver::RequestResponseL( 
        TFSProgress aEvent, TInt /* aRequestId */ )
    {
    if ( iObserver && aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete )
        {
        iObserver->MailboxSynchronisedL(aEvent.iError);
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CEmailMailbox::CEmailRequestObserver::SetObserverL( MMailboxSyncObserver* aObserver )
    {
    iObserver = aObserver;
    }

// End of file

