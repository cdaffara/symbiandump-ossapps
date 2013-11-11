/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  common mailbox object
*
*/

#include "emailtrace.h"

// <qmail>
#include <nmcommonheaders.h>
// <//qmail>

#include <bamdesca.h>
#include "CFSMailPlugin.h"
#include "cmrcalendarinfoimpl.h"
#include "CFSMailBox.h"
#include "CFSMailRequestObserver.h"
#include "cmailboxstateext.h"
// <qmail>
#include "CFSMailFolder.h"
#include "CFSMailAddress.h"
// </qmail>
#include "emailmru.h"


// ================= MEMBER FUNCTIONS ==========================================
// -----------------------------------------------------------------------------
// CFSMailBox::NewLC
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBox* CFSMailBox::NewLC(TFSMailMsgId aMailBoxId)
{
    NM_FUNCTION;

    CFSMailBox* api = new (ELeave) CFSMailBox();
    CleanupStack:: PushL(api);
    api->ConstructL(aMailBoxId);
    return api;
}

// -----------------------------------------------------------------------------
// CFSMailBox::NewL
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBox* CFSMailBox::NewL(TFSMailMsgId aMailBoxId)
{
    NM_FUNCTION;

    CFSMailBox* api =  CFSMailBox::NewLC(aMailBoxId);
    CleanupStack:: Pop(api);
    return api;
}

// -----------------------------------------------------------------------------
// CFSMailBox::CFSMailBox
// -----------------------------------------------------------------------------
CFSMailBox::CFSMailBox()
{
    NM_FUNCTION;

    // get requesthandler pointer
    iRequestHandler = static_cast<CFSMailRequestHandler*>(Dll::Tls());
}

// -----------------------------------------------------------------------------
// CFSMailBox::~CFSMailBox
// -----------------------------------------------------------------------------
EXPORT_C CFSMailBox::~CFSMailBox()
{
    NM_FUNCTION;
    // <qmail> Not using KMailboxExtMrCalInfo </qmail>
    iFolders.ResetAndDestroy();
    delete iMru;
}

// -----------------------------------------------------------------------------
// CFSMailBox::ConstructL
// -----------------------------------------------------------------------------
void CFSMailBox::ConstructL(TFSMailMsgId aMailBoxId)
{
    NM_FUNCTION;

    QT_TRYCATCH_LEAVING({
    iMru = new EmailMRU();
    });

// <qmail>
    CFSMailBoxBase::ConstructL(aMailBoxId);
// </qmail>
}

// -----------------------------------------------------------------------------
// CFSMailBox::GoOnlineL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::GoOnlineL()
    {
    NM_FUNCTION;

    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->GoOnlineL( GetId() );
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBox::GoOfflineL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::GoOfflineL()
    {
    NM_FUNCTION;

    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->GoOfflineL( GetId() );
        }
    }
// -----------------------------------------------------------------------------
// CFSMailBox::RefreshNowL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::RefreshNowL(
    MFSMailRequestObserver& aOperationObserver )
    {
    NM_FUNCTION;

    TFSPendingRequest request;
    request.iRequestId = 0;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                        aOperationObserver );
        MFSMailRequestObserver* observer = request.iObserver;
        TRAPD(err,plugin->RefreshNowL( GetId(), *observer, request.iRequestId));
        if(err != KErrNone)
            {
            iRequestHandler->CompleteRequest(request.iRequestId);
            User::Leave(err);
            }
        }
    return request.iRequestId;
    }

// -----------------------------------------------------------------------------
// CFSMailBox::RefreshNowL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::RefreshNowL( )
    {
    NM_FUNCTION;

    TFSPendingRequest request;
    request.iRequestId = 0;
    MFSMailRequestObserver* observer = NULL;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(), *observer );

        observer = request.iObserver;
        TRAPD(err,plugin->RefreshNowL( GetId(), *observer, request.iRequestId));
        if(err != KErrNone)
            {
            iRequestHandler->CompleteRequest(request.iRequestId);
            User::Leave(err);
            }
        }
    return request.iRequestId;
    }

// -----------------------------------------------------------------------------
// CFSMailBox::CreateMessageToSend
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage* CFSMailBox::CreateMessageToSend( )
{
    NM_FUNCTION;

    CFSMailMessage* message(NULL);
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        TRAPD(err,message = plugin->CreateMessageToSendL( GetId() ));
        if(err != KErrNone)
            {
            message = NULL;
            }
        }
    return message;
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailBox::CreateMessageToSendL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::CreateMessageToSendL(
        MFSMailRequestObserver& aOperationObserver )
    {
    NM_FUNCTION;

    TFSPendingRequest request;

    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() );

    if ( plugin )
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                      aOperationObserver );

        MFSMailRequestObserver* observer = request.iObserver;

        TRAPD( err, plugin->CreateMessageToSendL(
            GetId(), *observer, request.iRequestId ) );

        if( err != KErrNone )
            {
            iRequestHandler->CompleteRequest( request.iRequestId );
            User::Leave( err );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    return request.iRequestId;
    }
// </qmail>

//<qmail>
EXPORT_C int CFSMailBox::UpdateMessageFlagsL(
    const TFSMailMsgId aMailboxId,
    RPointerArray<CFSMailMessage> &messages,
    MFSMailRequestObserver& aOperationObserver)
{
    NM_FUNCTION;

    Q_UNUSED(aMailboxId);
    TFSPendingRequest request;

    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() );

    if ( plugin )
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                      aOperationObserver );

        MFSMailRequestObserver* observer = request.iObserver;

        TRAPD( err, plugin->StoreMessagesL(
                GetId(),
                messages,
                *observer, request.iRequestId ) );

        if( err != KErrNone )
            {
            iRequestHandler->CompleteRequest( request.iRequestId );
            User::Leave( err );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    return request.iRequestId;

}
//</qmail>

// -----------------------------------------------------------------------------
// CFSMailBox::CreateForwardMessage
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage* CFSMailBox::CreateForwardMessage(  TFSMailMsgId aOriginalMessageId,
                                                            const TDesC& aHeaderDescriptor )
{
    NM_FUNCTION;

    CFSMailMessage* message(NULL);
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        TRAPD(err,message = plugin->CreateForwardMessageL( GetId(),
                                                                aOriginalMessageId,
                                                                aHeaderDescriptor ));
        if(err != KErrNone)
            {
            message = NULL;
            }
        }
    return message;
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailBox::CreateForwardMessageL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::CreateForwardMessageL( const TFSMailMsgId aOriginalMessageId,
                        MFSMailRequestObserver& aOperationObserver,
                        const TDesC& aHeaderDescriptor )
    {
    NM_FUNCTION;

    TFSPendingRequest request;

    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() );

    if ( plugin )
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                      aOperationObserver );

        MFSMailRequestObserver* observer = request.iObserver;

        TRAPD( err, plugin->CreateForwardMessageL(
            GetId(), aOriginalMessageId, *observer, request.iRequestId, aHeaderDescriptor ) );

        if( err != KErrNone )
            {
            iRequestHandler->CompleteRequest( request.iRequestId );
            User::Leave( err );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    return request.iRequestId;
    }
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailBox::CreateReplyMessage
// -----------------------------------------------------------------------------
EXPORT_C CFSMailMessage* CFSMailBox::CreateReplyMessage( TFSMailMsgId aOriginalMessageId,
                                                         TBool aReplyToAll,
                                                         const TDesC& aHeaderDescriptor )
{
    NM_FUNCTION;

    CFSMailMessage* message = NULL;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        TRAPD(err,message = plugin->CreateReplyMessageL( GetId(),
                                                            aOriginalMessageId,
                                                            aReplyToAll,
                                                            aHeaderDescriptor ));
        if(err != KErrNone)
            {
            message = NULL;
            }
        }
    return message;
}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailBox::CreateReplyMessageL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::CreateReplyMessageL( const TFSMailMsgId aOriginalMessageId,
                                      const TBool aReplyToAll,
                                      MFSMailRequestObserver& aOperationObserver,
                                      const TDesC& aHeaderDescriptor )
{
    NM_FUNCTION;

    TFSPendingRequest request;

    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() );

    if ( plugin )
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                      aOperationObserver );

        MFSMailRequestObserver* observer = request.iObserver;

        TRAPD( err, plugin->CreateReplyMessageL(
            GetId(), aOriginalMessageId, aReplyToAll, *observer, request.iRequestId, aHeaderDescriptor ) );

        if( err != KErrNone )
            {
            iRequestHandler->CompleteRequest( request.iRequestId );
            User::Leave( err );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    return request.iRequestId;
}
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailBox::GetStandardFolderId
// -----------------------------------------------------------------------------
EXPORT_C TFSMailMsgId CFSMailBox::GetStandardFolderId(TFSFolderType aFolderType)
{
    NM_FUNCTION;

    TFSMailMsgId folderId;
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        TRAPD(err, folderId = plugin->GetStandardFolderIdL(GetId(), aFolderType ));
        if(err != KErrNone)
            {
            folderId.SetNullId();
            }
        }
    return folderId;
}

// -----------------------------------------------------------------------------
// CFSMailBox::SendMessageL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::SendMessageL( CFSMailMessage& aMessage )
    {
    NM_FUNCTION;

    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        UpdateMrus( aMessage.GetToRecipients() );
        UpdateMrus( aMessage.GetCCRecipients() );
        UpdateMrus( aMessage.GetBCCRecipients() );
        plugin->SendMessageL( aMessage );
        }
	}

// <qmail>
// -----------------------------------------------------------------------------
// CFSMailBox::SendMessageL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::SendMessageL(
        CFSMailMessage& aMessage,
        MFSMailRequestObserver& aOperationObserver )
    {
    NM_FUNCTION;

    TFSPendingRequest request;

    CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() );

    if ( plugin )
        {
        UpdateMrus( aMessage.GetToRecipients() );
        UpdateMrus( aMessage.GetCCRecipients() );
        UpdateMrus( aMessage.GetBCCRecipients() );

        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                      aOperationObserver );

        MFSMailRequestObserver* observer = request.iObserver;

        TRAPD( err, plugin->SendMessageL(
            aMessage, *observer, request.iRequestId ) );

        if( err != KErrNone )
            {
            iRequestHandler->CompleteRequest( request.iRequestId );
            User::Leave( err );
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    return request.iRequestId;
    }
// </qmail>

// -----------------------------------------------------------------------------
// CFSMailBox::ListFolders
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::ListFolders(  TFSMailMsgId aFolder,
                                        RPointerArray<CFSMailFolder>& aFolderList)
{
    NM_FUNCTION;

    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
    {
        TRAPD(err, plugin->ListFoldersL(GetId(),aFolder,aFolderList));
        if(err != KErrNone)
            {
            aFolderList.ResetAndDestroy();
            }
    }
}

// -----------------------------------------------------------------------------
// CFSMailBox::ListFolders
// -----------------------------------------------------------------------------
EXPORT_C RPointerArray<CFSMailFolder>& CFSMailBox::ListFolders( )
{
    NM_FUNCTION;

    iFolders.ResetAndDestroy();
    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
    {
        TRAPD(err,plugin->ListFoldersL(GetId(),iFolders));
        if(err != KErrNone)
            {
            iFolders.ResetAndDestroy();
            }
    }

    return iFolders;
}

// -----------------------------------------------------------------------------
// CFSMailBox::MoveMessagesL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::MoveMessagesL( const RArray<TFSMailMsgId>& aMessageIds,
                                         const TFSMailMsgId aSourceFolderId,
                                         const TFSMailMsgId aDestinationFolderId )
{
    NM_FUNCTION;

    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->MoveMessagesL(GetId(), aMessageIds, aSourceFolderId, aDestinationFolderId);
        }
}

// -----------------------------------------------------------------------------
// CFSMailBox::MoveMessagesL
// -----------------------------------------------------------------------------
EXPORT_C TInt CFSMailBox::MoveMessagesL( MFSMailRequestObserver& aOperationObserver,
                                         const RArray<TFSMailMsgId>& aMessageIds,
                                         const TFSMailMsgId aSourceFolderId,
                                         const TFSMailMsgId aDestinationFolderId )
{
    NM_FUNCTION;

    TFSPendingRequest request;
    request.iRequestId = 0;
    if( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() ) )
        {
        // init asynchronous request
        request = iRequestHandler->InitAsyncRequestL( GetId().PluginId(),
                                                        aOperationObserver );
        plugin->MoveMessagesL(
            GetId(),
            aMessageIds,
            aSourceFolderId,
            aDestinationFolderId,
            aOperationObserver,
            request.iRequestId );
        }
    return request.iRequestId;
}

// -----------------------------------------------------------------------------
// CFSMailBox::CopyMessagesL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::CopyMessagesL( const RArray<TFSMailMsgId>& aMessageIds,
                                         RArray<TFSMailMsgId>& aNewMessages,
                                         const TFSMailMsgId aSourceFolderId,
                                         const TFSMailMsgId aDestinationFolderId )
{
    NM_FUNCTION;

    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->CopyMessagesL(GetId(), aMessageIds, aNewMessages,
                                aSourceFolderId, aDestinationFolderId );
        }
}

// -----------------------------------------------------------------------------
// CFSMailBox::SearchL
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::SearchL( const RPointerArray<TDesC>& aSearchStrings,
                                   const TFSMailSortCriteria& aSortCriteria,
                                   MFSMailBoxSearchObserver& aSearchObserver )
    {
    NM_FUNCTION;

    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {

        // get mailbox folder list
        iFolders.ResetAndDestroy();
        plugin->ListFoldersL(GetId(),iFolders);

        TFSMailMsgId draftsFolderId = GetStandardFolderId( EFSDraftsFolder );
        TFSMailMsgId outboxId = GetStandardFolderId( EFSOutbox );

        // remove outbox, drafts folder from folder list
        RArray<TFSMailMsgId> folderIds;
        CleanupClosePushL( folderIds );
        for(TInt i=0;i<iFolders.Count();i++)
        {
            TFSMailMsgId id = iFolders[i]->GetFolderId();
            if( id != draftsFolderId && id != outboxId )
            {
                folderIds.AppendL(id);
            }
        }

        // start search
        plugin->SearchL( GetId(), folderIds, aSearchStrings, aSortCriteria, aSearchObserver );
        CleanupStack::PopAndDestroy( &folderIds );
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBox::CancelSearch
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::CancelSearch()
    {
    NM_FUNCTION;

    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->CancelSearch( GetId() );
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBox::ClearSearchResultCache
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::ClearSearchResultCache()
    {
    NM_FUNCTION;

    if(CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId()))
        {
        plugin->ClearSearchResultCache( GetId() );
        }
    }

// -----------------------------------------------------------------------------
// CFSMailBox::CurrentSyncState
// -----------------------------------------------------------------------------
EXPORT_C TSSMailSyncState CFSMailBox::CurrentSyncState() const
    {
    NM_FUNCTION;

    TSSMailSyncState syncState(Idle);
    if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() ) )
        {
        syncState = plugin->CurrentSyncState( GetId() );
        }
    return syncState;
    }

// -----------------------------------------------------------------------------
// CFSMailBox::GetMailBoxStatus
// -----------------------------------------------------------------------------
EXPORT_C TFSMailBoxStatus CFSMailBox::GetMailBoxStatus()
{
    NM_FUNCTION;

    TFSMailBoxStatus status(EFSMailBoxOffline);
    if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid(GetId() ) )
    {
        status = plugin->GetMailBoxStatus( GetId() );
    }
    return status;
}

// -----------------------------------------------------------------------------
// CFSMailBox::HasCapability
// -----------------------------------------------------------------------------
EXPORT_C TBool CFSMailBox::HasCapability( const TFSMailBoxCapabilities aCapability ) const
{
    NM_FUNCTION;
    
    TBool capability = EFalse;
    if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() ) )
        {
        TRAPD( err,capability = plugin->MailboxHasCapabilityL( aCapability,GetId() )) ;
        if ( err != KErrNone )
            {
            capability = EFalse;
            }
        }
    return capability;
}

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CFSMailBox::UpdateMrus
// -----------------------------------------------------------------------------
void CFSMailBox::UpdateMrus(
    const RPointerArray<CFSMailAddress>& aRecipients ) const
    {
    NM_FUNCTION;

    QT_TRYCATCH_LEAVING({

    TUint count( aRecipients.Count() );
        TUint indexer( 0 );
        while ( indexer < count )
            {
            TDesC& address(aRecipients[indexer]->GetEmailAddress() );
            TDesC& name(aRecipients[indexer]->GetDisplayName() );
            QString qaddress = QString::fromUtf16(address.Ptr(), address.Length());
            QString qname = QString::fromUtf16(name.Ptr(), name.Length());
            iMru->updateMRU(qname, qaddress);
            indexer++;
            }
    });

    }

// -----------------------------------------------------------------------------
// CFSMailBox::ReleaseExtension
// -----------------------------------------------------------------------------
EXPORT_C void CFSMailBox::ReleaseExtension( CEmailExtension* aExtension )
    {
    NM_FUNCTION;
	// <qmail> Not using KMailboxExtMrCalInfo

    // no specialized behaviour, call base class
    CExtendableEmail::ReleaseExtension( aExtension );
	// </qmail>
    }

// -----------------------------------------------------------------------------
// CFSMailBox::ExtensionL
// -----------------------------------------------------------------------------
EXPORT_C CEmailExtension* CFSMailBox::ExtensionL( const TUid& aInterfaceUid )
    {
    NM_FUNCTION;

    CEmailExtension* extension = NULL;
// <qmail>
    extension = CExtendableEmail::ExtensionL( aInterfaceUid );
    if ( aInterfaceUid == KMailboxExtMrCalInfo )
        {
// </qmail>
        if ( !extension )
            {
            extension = new ( ELeave ) CMRCalendarInfoImpl();
            CleanupStack::PushL( extension );
            iExtensions.AddL( extension );
            CleanupStack::Pop(); // calInfo
            }
        }
    else if ( aInterfaceUid == KEmailMailboxStateExtensionUid )
		// <qmail> Not using KEmailSettingExtensionUid or KEmailConnectionStatusQueryExtensionUid </qmail>
        {
        if ( !extension )
            {
            // check that plugin supports requested extension.
            if ( CFSMailPlugin* plugin = iRequestHandler->GetPluginByUid( GetId() ) )
                {
                // request extension from plugin, leaves if not supported
                extension = plugin->ExtensionL( aInterfaceUid );
                }

            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    return extension;
    }
    

