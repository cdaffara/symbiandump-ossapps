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
* Description: This file implements class CIpsPlgPop3Plugin.
 *
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"

//<qmail> removed

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::CIpsPlgPop3Plugin
// ---------------------------------------------------------------------------
//
CIpsPlgPop3Plugin::CIpsPlgPop3Plugin( ) :
    CIpsPlgSosBasePlugin( KIpsPlgPop3PluginUidValue )
    {
    FUNC_LOG;
    // none
    }

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::ConstructL
// ---------------------------------------------------------------------------
//
void CIpsPlgPop3Plugin::ConstructL()
    {
    FUNC_LOG;
    BaseConstructL();
    }

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CIpsPlgPop3Plugin* CIpsPlgPop3Plugin::NewL()
    {
    FUNC_LOG;
    CIpsPlgPop3Plugin* self = CIpsPlgPop3Plugin::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CIpsPlgPop3Plugin* CIpsPlgPop3Plugin::NewLC()
    {
    FUNC_LOG;
    CIpsPlgPop3Plugin* self = new( ELeave ) CIpsPlgPop3Plugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::~CIpsPlgPop3Plugin
// ---------------------------------------------------------------------------
//
CIpsPlgPop3Plugin::~CIpsPlgPop3Plugin()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::MailboxHasCapabilityL
// ---------------------------------------------------------------------------
//
TBool CIpsPlgPop3Plugin::MailboxHasCapabilityL(
    TFSMailBoxCapabilities aCapability,
    TFSMailMsgId /* aMailBoxId */)
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
        case EFSMBoxCapaRemoveFromCalendar:    
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
        case EFSMBoxCapaMoveToFolder:
        case EFSMBoxCapaCopyToFolder:
        case EFSMBoxCapaSupportsSync:
            {
            result = EFalse;
            break;
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::RefreshNowL
// ---------------------------------------------------------------------------
//  
TInt CIpsPlgPop3Plugin::RefreshNowL( )
    {
    FUNC_LOG;
    //we don't need this
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIpsPlgPop3Plugin::RefreshNowL
// ---------------------------------------------------------------------------- 
void CIpsPlgPop3Plugin::RefreshNowL(
    const TFSMailMsgId& aMailBoxId,
    MFSMailRequestObserver& aOperationObserver,
    TInt aRequestId,
    const TBool /*aSilentConnection*/ )
    {
    FUNC_LOG;
    TMsvId service = aMailBoxId.Id();
    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL(*this);
    CleanupStack::PushL( watcher ); // << watcher
    
    TBuf8<1> dummyParams;
    dummyParams.Zero();
    CMsvEntrySelection* sel=new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL( sel ); // << sel
    
    sel->AppendL( service );

    // Get population limit in order to find out if only headers are to be loaded
    // (i.e. whether to use force-populate or not in connect op)
    CImPop3Settings* settings = new(ELeave) CImPop3Settings();
    CleanupStack::PushL( settings );    // << settings
    TPopAccount popAccountId;
    CEmailAccounts* accounts = CEmailAccounts::NewLC(); // << accounts
    accounts->GetPopAccountL( service , popAccountId );
    accounts->LoadPopSettingsL( popAccountId, *settings );
    TInt populationLimit( settings->PopulationLimit() );
    CleanupStack::PopAndDestroy( 2, settings );   // >>> settings, accounts
    TBool forcePopulate( EFalse );
// <qmail> back to use
    if( populationLimit != KIpsSetDataHeadersOnly )
        {
        forcePopulate = ETrue;
        }
// </qmail>
    
    CIpsPlgBaseOperation* op = CIpsPlgPop3ConnectOp::NewL( 
        *iSession, 
        watcher->iStatus, 
        service, 
        forcePopulate,
        ActivityTimerL( aMailBoxId ),
        aMailBoxId, 
        &aOperationObserver,
        aRequestId,
        iEventHandler );
    
    watcher->SetOperation( op );
    CleanupStack::PopAndDestroy( sel ); // >>> sel
    
    iOperations.AppendL( watcher );
    CleanupStack::Pop( watcher );   // >> watcher
    	
    // send part of refresh
    EmptyOutboxL( aMailBoxId );
    }

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::ListFoldersL
// ---------------------------------------------------------------------------
//  
void CIpsPlgPop3Plugin::ListFoldersL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aFolderId*/,
    RPointerArray<CFSMailFolder>& aFolderList)
	{
    FUNC_LOG;
	// Pop3 returns always the root level, so ignore folder id even
	// it is given.
	ListFoldersL( aMailBoxId, aFolderList );
	}

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::ListFoldersL
// ---------------------------------------------------------------------------
//  
void CIpsPlgPop3Plugin::ListFoldersL(
    const TFSMailMsgId& aMailBoxId,
    RPointerArray<CFSMailFolder>& aFolderList)
 	{
    FUNC_LOG;
 	//list service folder, drafts, inbox, outbox
 	
 	//inbox
 	CFSMailFolder* folder = 
 	   GetFolderByUidL( aMailBoxId, GetStandardFolderIdL( aMailBoxId, EFSInbox ) );
 	if( folder )
 	    {
 	    // in Pop inbox is same as symbian mailbox, correct type refer to FS Inbox
 	    folder->SetFolderType( EFSInbox );
 	    CleanupStack::PushL( folder );
        aFolderList.AppendL( folder );
        CleanupStack::Pop( folder );
 	    }

    //outbox 	    
    folder = GetFolderByUidL( aMailBoxId, GetStandardFolderIdL( aMailBoxId, EFSOutbox ) );
    if( folder )
 	    {
 	    CleanupStack::PushL( folder );
        aFolderList.AppendL( folder );
        CleanupStack::Pop( folder );
 	    }
    
    //drafts
    folder = GetFolderByUidL( aMailBoxId, GetStandardFolderIdL( aMailBoxId, EFSDraftsFolder ) );
    if( folder )
 	    {
 	    CleanupStack::PushL( folder );
        aFolderList.AppendL( folder );
        CleanupStack::Pop( folder );
 	    } 	    
 	    
    //sent
    folder = GetFolderByUidL( aMailBoxId, GetStandardFolderIdL( aMailBoxId, EFSSentFolder ) );
    if( folder )
 	    {
 	    CleanupStack::PushL( folder );
        aFolderList.AppendL( folder );
        CleanupStack::Pop( folder );
 	    } 	    
 	}

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::GetStandardFolderIdL
// ---------------------------------------------------------------------------
//       
TFSMailMsgId CIpsPlgPop3Plugin::GetStandardFolderIdL( 
    const TFSMailMsgId& aMailBoxId,
    const TFSFolderType aFolderType )
 	{
    FUNC_LOG;
 	TFSMailMsgId result;
 	
 	if( aFolderType==EFSInbox )
        {
        //<qmail> removed CMsvEntry conversion 
 	
        // In case of pop3, mailbox id == service id == inbox id,
        // so no need to create CMsvEntry from mailbox id to
        // dig if any children exists.
        result.SetId( aMailBoxId.Id() );
        //</qmail>
        }
    else if( aFolderType==EFSOutbox )
        {
        result.SetId( KMsvGlobalOutBoxIndexEntryId );
        }
    else if( aFolderType==EFSDraftsFolder )
        {
        result.SetId( KMsvDraftEntryId );
        }
    else if( aFolderType==EFSSentFolder )
        {
        result.SetId( KMsvSentEntryId );
        }
    else if( aFolderType==EFSDeleted )
        {
        }
    else if( aFolderType==EFSOther )
        {
        }
    if ( !result.IsNullId() )
        {
        result.SetPluginId( KIpsPlgPop3PluginUid );
        }
    
    return result;        
 	}

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::FetchMessagesL
// ---------------------------------------------------------------------------
//   
void CIpsPlgPop3Plugin::FetchMessagesL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /* aFolderId */,
    const RArray<TFSMailMsgId>& aMessageIds, 
    TFSMailDetails /* aDetails */,
    MFSMailRequestObserver& aObserver,
    TInt aRequestId )
 	{
    FUNC_LOG;
    CIpsPlgSingleOpWatcher* watcher = CIpsPlgSingleOpWatcher::NewL(*this);
    CleanupStack::PushL( watcher );
    
    TBuf8<1> dummyParams;
    dummyParams.Zero();
    CMsvEntrySelection* sel=new(ELeave) CMsvEntrySelection;
    CleanupStack::PushL(sel);
    
    TInt count = aMessageIds.Count();
    for ( TInt i=0; i<count; i++ )
        {
        sel->AppendL( aMessageIds[i].Id() );
        }
    
    TImPop3GetMailInfo info;
    info.iMaxEmailSize = KMaxTInt32;
    info.iDestinationFolder = aMailBoxId.Id();
    // <qmail> ownership of selection is moved to the operation
    CIpsPlgBaseOperation* op = CIpsPlgPop3FetchOperation::NewL( 
        *iSession, 
        watcher->iStatus,
        aMailBoxId.Id(), 
        ActivityTimerL( aMailBoxId ), 
        info, 
        sel, 
        aMailBoxId, 
        &aObserver, 
        aRequestId,
        iEventHandler );
    
    watcher->SetOperation( op );
	// <qmail> change PopAndDestroy to Pop
    CleanupStack::Pop( sel );
    CleanupStack::Pop( watcher );
    
    iOperations.AppendL( watcher );	
 	}

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::FetchMessagesPartsL
// ---------------------------------------------------------------------------
//  
void CIpsPlgPop3Plugin::FetchMessagesPartsL( 
    const RArray<TFSMailMsgId>& /* aMessagePartIds */,
    MFSMailRequestObserver& aOperationObserver,
	TInt aRequestId)
 	{
    FUNC_LOG;
 	// this not supported atm, framework never calls this function
 	TFSProgress prog = { TFSProgress::EFSStatus_RequestCancelled, 0, 0, KErrNone };
 	aOperationObserver.RequestResponseL( prog, aRequestId );
 	
 	}

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::FetchMessagePartsL
// ---------------------------------------------------------------------------
//
void CIpsPlgPop3Plugin::FetchMessagePartsL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aFolderId,
    const TFSMailMsgId& aMessageId,
    const RArray<TFSMailMsgId>& /* aMessagePartIds */,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId,
    const TUint /* aPreferredByteCount */)
    {
    FUNC_LOG;
    // with pop no sense to fetch parts separately, instead fetch 
    // whole message id is just set some value FetchMessageL not use it
    RArray<TFSMailMsgId> msgId(1);
    msgId.AppendL( aMessageId );
    FetchMessagesL(
        aMailBoxId,
        aFolderId,
        msgId, 
        EFSMsgDataIdOnly,
        aOperationObserver,
        aRequestId );
    msgId.Close();
    }

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::HandleOpCompletedL
// ---------------------------------------------------------------------------
//
void CIpsPlgPop3Plugin::HandleOpCompletedL(
    CIpsPlgSingleOpWatcher& /*aOpWatcher*/,
    TInt /*aCompletionCode*/ )
    {

    }

// ---------------------------------------------------------------------------
// CIpsPlgPop3Plugin::PopulateNewMailL
// ---------------------------------------------------------------------------
//
void CIpsPlgPop3Plugin::PopulateNewMailL( 
    const TFSMailMsgId& /* aMailboxId */, 
    TMsvId /* aNewId */, 
    TMsvId /* aParentId */ )
    {
    // do nothing
    }

// End of File

