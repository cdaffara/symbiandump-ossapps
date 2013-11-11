/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CIpsPlgImap4Plugin.
*
*/

#include "emailtrace.h"
#include "ipsplgheaders.h"

// <qmail> priority const has been removed

// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::CIpsPlgImap4Plugin()
// ---------------------------------------------------------------------------
//
CIpsPlgImap4Plugin::CIpsPlgImap4Plugin()
    : CIpsPlgSosBasePlugin( KIpsPlgImap4PluginUidValue )
    {
    FUNC_LOG;
    // none
    }

// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::ConstructL()
// ---------------------------------------------------------------------------
//
void CIpsPlgImap4Plugin::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    }

// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CIpsPlgImap4Plugin* CIpsPlgImap4Plugin::NewL()
    {
    FUNC_LOG;
    CIpsPlgImap4Plugin* self = CIpsPlgImap4Plugin::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CIpsPlgImap4Plugin* CIpsPlgImap4Plugin::NewLC()
    {
    FUNC_LOG;
    CIpsPlgImap4Plugin* self = new( ELeave ) CIpsPlgImap4Plugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::~CIpsPlgImap4Plugin()
// ---------------------------------------------------------------------------
//
CIpsPlgImap4Plugin::~CIpsPlgImap4Plugin()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::MailboxHasCapabilityL
// ---------------------------------------------------------------------------
//
TBool CIpsPlgImap4Plugin::MailboxHasCapabilityL(
    TFSMailBoxCapabilities aCapability,
    TFSMailMsgId /* aMailBoxId */ )
    {
    FUNC_LOG;
    TBool result( EFalse );
    
    switch ( aCapability )
        {
        // These should be clear    
        case EFSMBoxCapaCanBeDeleted:
        case EFSMBoxCapaSupportsSaveToDrafts:
        case EFSMBoxCapaMeetingRequestRespond:
        case EFSMboxCapaSupportsAttahmentsInMR:
        case EFSMBoxCapaMeetingRequestCreate:
        case EFSMBoxCapaCanUpdateMeetingRequest:
        case EFSMBoxCapaMoveToFolder:
        case EFSMBoxCapaCopyToFolder:
        case EFSMBoxCapaSupportsSync:
        case EFSMBoxCapaRemoveFromCalendar:
        case EFSMBoxCapaMoveMessageNoConnectNeeded:
            {
            result = ETrue;
            break;
            }
        // These are positively not supported 
        case EFSMBoxCapaCreateSubfolder:
        case EFSMBoxCapaManipulateUserFolders: 
        case EFSMBoxCapaMoveMessageToUserFolder:
        case EFSMBoxCapaSmartReply:
        case EFSMBoxCapaSmartForward:
        case EFSMBoxCapaSupportsMRU:
        case EFSMBoxCapaSymbianMsgIntegration:
        case EFSMBoxCapaNewEmailNotifications:
        default:
            {
            result = EFalse;
            break;
            }
        }
    
    return result;
    }

// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::RefreshNowL( )
// ---------------------------------------------------------------------------
//  
TInt CIpsPlgImap4Plugin::RefreshNowL( )
    {
    FUNC_LOG;
    return KErrNone;	
    }

// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::RefreshNowL()
// ---------------------------------------------------------------------------
//
void CIpsPlgImap4Plugin::RefreshNowL(
    const TFSMailMsgId& aMailBoxId,
    MFSMailRequestObserver& aOperationObserver,
    TInt aRequestId,
    const TBool /*aSilentConnection*/ )
    {
    FUNC_LOG;
    
    TMsvId service = aMailBoxId.Id();
    
    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( watcher );
    
    // <qmail> priority parameter has been removed
    CIpsPlgBaseOperation* op = CIpsPlgImap4ConnectOp::NewL(
    		*iSession,
    		watcher->iStatus, 
    		service,
    		ActivityTimerL( aMailBoxId ),
          	aMailBoxId, 
          	&aOperationObserver,
          	aRequestId,
          	iEventHandler );
    
    watcher->SetOperation( op );
    iOperations.AppendL( watcher );
    CleanupStack::Pop( watcher );
    
    // send part of refresh
    EmptyOutboxL( aMailBoxId );
    }

// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::ListFoldersL()
// ---------------------------------------------------------------------------
//  
void CIpsPlgImap4Plugin::ListFoldersL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aFolderId,
    RPointerArray<CFSMailFolder>& aFolderList)
	{
    FUNC_LOG;
	DoListFoldersL( aMailBoxId, aFolderId, aFolderList, ETrue );
	}

// ---------------------------------------------------------------------------
//  CIpsPlgImap4Plugin::ListFoldersL()
// ---------------------------------------------------------------------------
//  
void CIpsPlgImap4Plugin::ListFoldersL(
    const TFSMailMsgId& aMailBoxId,
    RPointerArray<CFSMailFolder>& aFolderList)
 	{
    FUNC_LOG;
 	DoListFoldersL( aMailBoxId, TFSMailMsgId(), aFolderList, EFalse );
 	}

// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::GetStandardFolderIdL()
// ---------------------------------------------------------------------------
//       
TFSMailMsgId CIpsPlgImap4Plugin::GetStandardFolderIdL( 
    const TFSMailMsgId& aMailBoxId, 
    const TFSFolderType aFolderType )
 	{
    FUNC_LOG;
    TFSMailMsgId ret;
    
    if( aFolderType == EFSInbox )
        {
        CMsvEntry* cEntry = iSession->GetEntryL( aMailBoxId.Id() );
        CleanupStack::PushL( cEntry );
        
        TInt i( 0 );
        TInt count( cEntry->Count() );
        while ( i < count && ret.IsNullId() )
            {
            if ( (*cEntry)[i].iType == KUidMsvFolderEntry &&
                 (*cEntry)[i].iDetails.CompareF( KIpsPlgInbox ) == 0 )
                {
                ret.SetId( (*cEntry)[i].Id() );
                }
            i++;
            } 
        CleanupStack::PopAndDestroy( cEntry );
        }
    else if( aFolderType == EFSOutbox )
        {
        ret.SetId( KMsvGlobalOutBoxIndexEntryId );
        }
    else if( aFolderType == EFSDraftsFolder )
        {
        ret.SetId( KMsvDraftEntryId );
        }
    else if( aFolderType == EFSSentFolder )
        {
        ret.SetId( KMsvSentEntryId );
        }
    else if( aFolderType == EFSDeleted )
        {
        ret.SetId( KMsvDeletedEntryFolderEntryId );
        }
    else if( aFolderType == EFSOther )
        {
        ret.SetId( KMsvUnknownServiceIndexEntryId );
        }
    if ( !ret.IsNullId() )
        {
        ret.SetPluginId( KIpsPlgImap4PluginUid );
        }
        
    return ret;
 	}
	 
// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::FetchMessagesL()
// ---------------------------------------------------------------------------
//   
void CIpsPlgImap4Plugin::FetchMessagesL(
    const TFSMailMsgId& aMailBoxId, 
    const TFSMailMsgId& aFolderId,
    const RArray<TFSMailMsgId>& aMessageIds,
    TFSMailDetails /*aDetails*/,
    MFSMailRequestObserver& aObserver,
    TInt aRequestId)
     {
    FUNC_LOG;
 	
    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL(*this);
    CleanupStack::PushL( watcher );

    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);
    sel->AppendL( aMailBoxId.Id() );
    
    TInt count = aMessageIds.Count();
    
    if ( count == 0 )
        {
        User::Leave( KErrArgument );
        }
    for ( TInt i(0); i < count; i++ )
        {
        sel->AppendL( aMessageIds[i].Id() );
        }
    
    
    // fetch body texts only (text/plain, text/html)
    TImImap4GetPartialMailInfo info;
    info.iMaxEmailSize = KMaxTInt32;
    info.iGetMailBodyParts = EGetImap4EmailBodyText;
    info.iPartialMailOptions = EBodyTextOnly;
    info.iTotalSizeLimit = KMaxTInt32;
    info.iBodyTextSizeLimit = KMaxTInt32;
    info.iAttachmentSizeLimit = 0;
    info.iDestinationFolder = aFolderId.Id();
    
    // <qmail> priority parameter has been removed
    CIpsPlgBaseOperation* op = CIpsPlgImap4PopulateOp::NewL( 
        *iSession, 
        watcher->iStatus,
        aMailBoxId.Id(),
        ActivityTimerL( aMailBoxId ),
        info,
        *sel,
        aMailBoxId, 
        &aObserver,
        aRequestId,
        iEventHandler, 
        EFalse ); // do not filter mail selection
    
    watcher->SetOperation( op );
    CleanupStack::PopAndDestroy( sel );
    iOperations.AppendL( watcher );
    CleanupStack::Pop( watcher );

 	}
 	
// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::MoveMessagesL( )
// ---------------------------------------------------------------------------  
// 
void CIpsPlgImap4Plugin::MoveMessagesL( 
    const TFSMailMsgId& aMailBoxId, 
    const RArray<TFSMailMsgId>& aMessageIds, 
    const TFSMailMsgId& aSourceFolderId, 
    const TFSMailMsgId& aDestinationFolderId )
    {
    FUNC_LOG;
    if( aDestinationFolderId.Id() == KMsvDraftEntryId  )
        {
        // Move to Drafts folder
        MoveMessagesToDraftL( aMailBoxId, 
                aMessageIds, 
                aSourceFolderId, 
                aDestinationFolderId );
        }
    else if( aDestinationFolderId.Id() != aSourceFolderId.Id() )
        {
        TMsvId service = aMailBoxId.Id();

        CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL(*this);
        CleanupStack::PushL( watcher );
        
        CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL(sel);
        sel->AppendL( service );

        TInt msgIdCount( aMessageIds.Count() );
        for ( TInt i(0); i< msgIdCount; i++ )
            {
            sel->AppendL( aMessageIds[i].Id() );
            }
    	
		// <qmail> TImImap4GetMailInfo options not needed

        CIpsPlgImap4MoveRemoteOpObserver* observer =
            CIpsPlgImap4MoveRemoteOpObserver::NewL( *iSession, *iEventHandler,
                aSourceFolderId, aMessageIds );
        watcher->SetRequestObserver( observer );

        // Synchronous operation
        // <qmail> following constructor's parameters have changed
    	CIpsPlgBaseOperation* op = CIpsPlgImap4MoveRemoteOp::NewL(
            *iSession, 
            watcher->iStatus,
            service,
            ActivityTimerL( aMailBoxId ),
            aDestinationFolderId.Id(),
            *sel,
            aMailBoxId,
            NULL, // no observer, async not supported
            0 ); // async not supported

        watcher->SetOperation( op );
        CleanupStack::PopAndDestroy( sel );
        iOperations.AppendL( watcher );
        CleanupStack::Pop( watcher );
        }
    else
        {
        //do nothing
        }
    }

// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::MoveMessagesL()
// asynchronic move message function
// ---------------------------------------------------------------------------
//
TInt CIpsPlgImap4Plugin::MoveMessagesL( 
    const TFSMailMsgId& aMailBoxId,
    const RArray<TFSMailMsgId>& aMessageIds, 
    const TFSMailMsgId& aSourceFolderId,
    const TFSMailMsgId& aDestinationFolderId,
    MFSMailRequestObserver& aOperationObserver,
    TInt aRequestId )
    {
    FUNC_LOG;
    TInt ret = KErrNone;
    if( aDestinationFolderId.Id() == KMsvDraftEntryId  )
        {
        // Move to Drafts folder
        MoveMessagesToDraftL( aMailBoxId, 
                aMessageIds, 
                aSourceFolderId, 
                aDestinationFolderId );
        TFSProgress progress = { 
                TFSProgress::EFSStatus_RequestComplete, 0, 0, ret };
        aOperationObserver.RequestResponseL( 
                progress, aRequestId );
        }
    else if( aDestinationFolderId.Id() != aSourceFolderId.Id() )
        {
        TMsvId service = aMailBoxId.Id();

        CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL(*this);
        CleanupStack::PushL( watcher );
        
        CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
        CleanupStack::PushL(sel);
        sel->AppendL( service );

        TInt msgIdCount( aMessageIds.Count() );
        for ( TInt i(0); i< msgIdCount; i++ )
            {
            sel->AppendL( aMessageIds[i].Id() );
            }

        TPckgBuf<TImImap4GetMailInfo> optionsBuf;
        TImImap4GetMailInfo& options = optionsBuf();
        options.iMaxEmailSize = KMaxTInt32;
        options.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
        options.iDestinationFolder = aDestinationFolderId.Id();

		// <qmail> following constructor's parameters have changed
    	CIpsPlgBaseOperation* op = CIpsPlgImap4MoveRemoteOp::NewL(
            *iSession, 
            watcher->iStatus,
            service,
            ActivityTimerL( aMailBoxId ),
            aDestinationFolderId.Id(),
            *sel,
            aMailBoxId,
            &aOperationObserver, // async not supported
            0 ); // async not supported

        watcher->SetOperation( op );
        CleanupStack::PopAndDestroy( sel );
        iOperations.AppendL( watcher );
        CleanupStack::Pop( watcher );
        }
    else
        {
        ret = KErrNotSupported;
        TFSProgress progress = { 
                TFSProgress::EFSStatus_RequestComplete, 0, 0, ret };
        aOperationObserver.RequestResponseL( 
                progress, aRequestId );
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::FetchMessagePartsL()
// ---------------------------------------------------------------------------
//
void CIpsPlgImap4Plugin::FetchMessagePartsL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aFolderId,
    const TFSMailMsgId& aMessageId,
    const RArray<TFSMailMsgId>& aMessagePartIds,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId,
    const TUint /*aPreferredByteCount*/)
    {
    FUNC_LOG;
    
    TInt count( aMessagePartIds.Count() );
    if ( 0 == count )
        {
        User::Leave( KErrArgument );
        } 
    
    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL( *this );
    CleanupStack::PushL( watcher );
    
    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( sel );
    
	TMsvEntry tEntry;
    TMsvId service;
	
    for ( TInt i(0); i < count; i++ )
        {
        sel->AppendL( aMessagePartIds[i].Id() );
        
        iSession->GetEntry( aMessagePartIds[i].Id(), service, tEntry );
        
        if ( tEntry.iType.iUid == KUidMsvEmailHtmlEntryValue ||
             tEntry.iType.iUid == KUidMsvEmailExternalBodyEntryValue ||
             tEntry.iType.iUid == KUidMsvEmailRtfEntryValue )
            {
            CleanupStack::PopAndDestroy(2);//sel, watcher
            
            //we need to fetch the whole message in these cases, because
            //symbian components ignore these types of parts currently
            RArray<TFSMailMsgId> messages(1);
            messages.Append( aMessageId );
            
            // not used in function
            TFSMailDetails dymmy = EFSMsgDataStructure;
            
            FetchMessagesL( aMailBoxId,
                            aFolderId,
                            messages,
                            dymmy,
                            aOperationObserver,
                            aRequestId );
            
            messages.Close();
                        
            return;
            }
        
        }
       
	// <qmail> TImImap4GetMailInfo options removed
    CIpsPlgBaseOperation* op = CIpsPlgImap4FetchAttachmentOp::NewL( 
        *iSession, 
        watcher->iStatus,
        aMailBoxId.Id(),
        ActivityTimerL( aMailBoxId ), 
        sel, // ownership is transferred
        aMailBoxId,
        &aOperationObserver,
        aRequestId );
    
    watcher->SetOperation( op );
    iOperations.AppendL( watcher );
// <qmail>
    CleanupStack::Pop( sel );
// </qmail>
    CleanupStack::Pop( watcher );
    }

// ----------------------------------------------------------------------------
// CIpsPlgImap4Plugin::PopulateNewMailL( )
// ----------------------------------------------------------------------------
//
void CIpsPlgImap4Plugin::PopulateNewMailL( 
    const TFSMailMsgId& aMailboxId, TMsvId aNewId, TMsvId /*aParentId*/ )
    {
    FUNC_LOG;
    if ( aMailboxId.PluginId().iUid != PluginId() )
        {
        return;
        }
    else if ( iSyncStateHandler->GetMailboxIpsState( aMailboxId.Id() )
            == KIpsSosEmailSyncStarted )
        {
        return;
        }
    
    
    TMsvEntry tEntry;
    TMsvId serv;
    iSession->GetEntry( aMailboxId.Id(), serv, tEntry );
    if ( !tEntry.Connected() )
        {
        return;
        }
    
    CImImap4Settings* settings = new ( ELeave ) CImImap4Settings();
    CleanupStack::PushL( settings );
    CEmailAccounts* accounts = CEmailAccounts::NewLC();
    TImapAccount imapAcc;
    accounts->GetImapAccountL( aMailboxId.Id(), imapAcc );
    accounts->LoadImapSettingsL( imapAcc, *settings );
    TImImap4GetPartialMailInfo info;
// <qmail> Get TImImap4GetPartialMailInfo based on settings
    CIpsPlgImap4ConnectOp::ConstructImapPartialFetchInfo( info, *settings );
    CleanupStack::PopAndDestroy( 2, settings );

    if ( info.iTotalSizeLimit == KIpsSetDataHeadersOnly )
        {
        return;
        }
// </qmail>
    
    TPckgBuf<TImImap4GetPartialMailInfo> package(info);
    
    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL(*this);
    CleanupStack::PushL( watcher );

    CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);
    sel->AppendL( aMailboxId.Id() );
    sel->AppendL( aNewId );
    
    // <qmail> priority parameter has been removed
    CIpsPlgBaseOperation* op = CIpsPlgImap4PopulateOp::NewL( 
        *iSession, 
        watcher->iStatus,
        aMailboxId.Id(),
        ActivityTimerL( aMailboxId ),
        info,
        *sel,
        aMailboxId, 
        NULL, // no operation observer
        0,    // no use for requestId
        iEventHandler,
        EFalse ); // do not block entry changed and created events
    
    watcher->SetOperation( op );
    CleanupStack::PopAndDestroy( sel );
    iOperations.AppendL( watcher );
    CleanupStack::Pop( watcher );
    
    iEventHandler->SetNewPropertyEvent( 
            aMailboxId.Id(), KIpsSosEmailSyncStarted, KErrNone );
    
    }


// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::HandleOpCompletedL()
// ---------------------------------------------------------------------------
//
void CIpsPlgImap4Plugin::HandleOpCompletedL( 
							CIpsPlgSingleOpWatcher& /*aOpWatcher*/, 
							TInt /*aCompletionCode*/ )
    {
    FUNC_LOG;

    }

// ---------------------------------------------------------------------------
//  CIpsPlgImap4Plugin::DoListFoldersL( )
// ---------------------------------------------------------------------------
//   	
void CIpsPlgImap4Plugin::DoListFoldersL( 
							TFSMailMsgId aMailBoxId,
    						TFSMailMsgId aFolderId,
    						RPointerArray<CFSMailFolder>& aFolderList,
    						TBool aOnlyDirect )
    
    {
    FUNC_LOG;
    CFSMailFolder* folder( NULL );
	CMsvEntrySelection* folders = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL( folders );
	
	if( aFolderId.IsNullId() )
	    {
        folder = GetFolderByUidL( aMailBoxId, 
                                  GetStandardFolderIdL( 
                                          aMailBoxId, EFSOutbox ) );
        if( folder )
     	    {
     	    CleanupStack::PushL( folder );
     	    aFolderList.AppendL( folder );
     	    CleanupStack::Pop( folder );
     	    }
        
        folder = GetFolderByUidL( aMailBoxId, 
                                  GetStandardFolderIdL( 
                                          aMailBoxId, EFSDraftsFolder ) );
        if( folder )
     	    {
     	    CleanupStack::PushL( folder );
     	    aFolderList.AppendL( folder );
     	    CleanupStack::Pop( folder );
     	    } 	    
     	    
        folder = GetFolderByUidL( aMailBoxId, 
                                  GetStandardFolderIdL( 
                                          aMailBoxId, EFSSentFolder ) );
        if( folder )
     	    {
     	    CleanupStack::PushL( folder );
     	    aFolderList.AppendL( folder );
     	    CleanupStack::Pop( folder );
     	    } 	   
        
        // List root level (aOnlyDirect==ETrue) or all subfolders of 
        // given mailbox (aOnlyDirect==EFalse)
        HandleFoldersL( aMailBoxId, *folders, aOnlyDirect );
	    }
    else
        {
	    // Folder id defined, list direct subfolders of given
	    // folder
        HandleFoldersL( aFolderId, *folders, aOnlyDirect );
        }
    TInt folderCount( folders->Count() );
    for( TInt i(0); i < folderCount; i++ )
        {
        aFolderId.SetId( (*folders).At(i) );
        aFolderId.SetPluginId( KIpsPlgImap4PluginUid );
        
        folder = GetFolderByUidL( aMailBoxId, aFolderId );

        CleanupStack::PushL( folder );
        aFolderList.AppendL( folder );
        CleanupStack::Pop( folder );
        }
    CleanupStack::PopAndDestroy( folders );
    }	
    
// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::HandleFoldersL( )
// ---------------------------------------------------------------------------	
//
void CIpsPlgImap4Plugin::HandleFoldersL( 
							TFSMailMsgId aId, 
							CMsvEntrySelection& aFolders,
							TBool aOnlyDirect )
	{
    FUNC_LOG;
	if( HasChildFoldersL( aId.Id() ) )
	    {
	    // GetChildrenL is recursive, so we set CMsvEntry once and pass it
	    // forward instead of creating it several times before the first one 
	    // is deleted.
	    CMsvEntry* cEntry = iSession->GetEntryL( aId.Id() );
	    CleanupStack::PushL( cEntry );	    
	    CMsvEntrySelection* folderEntries = 
	    	GetChildrenL( aId.Id(), *cEntry, aOnlyDirect );
	    CleanupStack::PopAndDestroy( cEntry );
	    CleanupStack::PushL( folderEntries );
		
	    TInt count = folderEntries->Count();
	    for( TInt i(0); i < count; i++ )
	        {
	        aFolders.AppendL( folderEntries->At(i) );
	        }

        CleanupStack::PopAndDestroy( folderEntries );
        }
    else
        {
        // Do nothing, only subfolders of given folder will be added to array
        }
	}

// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::GetChildrenL( )
// ---------------------------------------------------------------------------
//
CMsvEntrySelection* CIpsPlgImap4Plugin::GetChildrenL( 
											TMsvId aId, 
											CMsvEntry& aEntry,
											TBool aOnlyDirect ) const
    {
    FUNC_LOG;
    aEntry.SetEntryL( aId );
    CMsvEntrySelection* children = 
    	aEntry.ChildrenWithTypeL( KUidMsvFolderEntry );
    CleanupStack::PushL( children );

	if( !aOnlyDirect )
		{
		TInt count = children->Count();
	    for( TInt i = 0; i < count; i++ )
	        {
	        if( HasChildFoldersL( children->At(i) ) )
	            {
	            // Recursive
	            CMsvEntrySelection* grandChildren = 
	            	GetChildrenL( children->At(i), aEntry, aOnlyDirect );
	            CleanupStack::PushL( grandChildren );

		    TInt count = grandChildren->Count();
	            for( TInt k(0); k < count; k++ )
	                {
	                children->AppendL( grandChildren->At(k) );
	                }
	            CleanupStack::PopAndDestroy( grandChildren );
	            }
	        }
		}
    CleanupStack::Pop( children );
    return children;
    }
    	    
// ---------------------------------------------------------------------------
// CIpsPlgImap4Plugin::HasChildFoldersL( )
// ---------------------------------------------------------------------------
//
TBool CIpsPlgImap4Plugin::HasChildFoldersL( TMsvId aId ) const
	{
    FUNC_LOG;
	CMsvEntry* cEntry = iSession->GetEntryL( aId );
    CleanupStack::PushL( cEntry );
	TInt index = cEntry->Count()-1;
	while ( index >= 0 && (*cEntry)[index].iType != KUidMsvFolderEntry )
        {
		index--;
        }
    CleanupStack::PopAndDestroy( cEntry );

	return ( index != -1 );
	}
	        
// End of File

