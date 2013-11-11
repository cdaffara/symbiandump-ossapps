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
* Description:  Email interface implementation.
*
*/


//MsgStore.
//<cmail>
#include "MsgStore.h"
#include "MsgStoreAccount.h"
#include "MsgStoreMailBox.h"
#include "MsgStoreMessage.h"
#include "MsgStoreFolder.h"
#include "MsgStorePropertyContainer.h"
#include "MsgStorePropertyKeys.h"
#include "MsgStoreSortCriteria.h"
#include "MsgStoreSortResultIterator.h"
//Freestyle.
#include "CFSMailCommon.h"
#include "CFSMailMessage.h"
#include "cmrcalendarinfo.h" 
//</cmail>
//Base plugin.
#include "BasePlugin.h"
#include "BasePluginPanic.h"
#include "baseplugincommonutils.h"
#include "MailIterator.h"
#include "baseplugindelayedopsprivate.h"

//<qmail>
#include "CFSMailFolder.h"
#include "baseplugindef.h"
//</qmail>

// Other
#include <e32base.h>
#include <utf.h>
#include <calsession.h>

//<qmail>
//<cmail>
//#include "freestyleemailuiconstants.h" // removed from 10.1
//</cmail>
//</qmail>
//size of the read buffer when reading body content from the quoted
//message when replying/forwarding.
const TInt KQuotedReadBufferSize = 1024*64;


/**
 *
 */
CBasePlugin* CBasePlugin::NewLC()
    {
    CBasePlugin* api = new (ELeave) CBasePlugin();
    CleanupStack::PushL( api );
    api->ConstructL();
    return api;
    } 

/**
 *
 */
CBasePlugin* CBasePlugin::NewL()
   {
   CBasePlugin* api =  CBasePlugin::NewLC();
   CleanupStack:: Pop(api);
   return api;
   }


/**
 *
 */
 void CBasePlugin::ConstructL()
    {
    __LOG_CONSTRUCT( "baseplugin", "CBasePlugin" )

    __LOG_ENTER( "ConstructL" )
    iMsgStore = CMsgStore::NewL( EFalse );
    
    iDelayedOpsManager = CDelayedOpsManager::NewL( *this );

    __LOG_EXIT
    }


/**
 *
 */
 CBasePlugin::CBasePlugin()
: iMailboxes( RMap<TInt, CMailboxInfo>::CompareInt )
   {
   }


/**
 *
 */
CBasePlugin::~CBasePlugin()
    {
    //needs to be first thing to do as it might force execution of operations
    //that depend on the plugin's current state.
    delete iDelayedOpsManager;
    
    iMailboxes.ResetAndDestroy();
    iMailboxes.Close();

//<qmail>
    if ( iObservers.Count() > 0 && iMsgStore )
//</qmail>
        {
        TRAP_IGNORE( iMsgStore->RemoveObserverL( this ) );
        }
    iObservers.Close();

    delete iMsgStore;

    iReqs.ResetAndDestroy();
    iReqs.Close();
//<qmail>
    // Note that all the ops in iDelayedOpReqs array (if any) have already been
    // deleted in the destructor of CDelayedOpsManager class.
    iDelayedOpReqs.Reset();
    iDelayedOpReqs.Close();
//</qmail>
    ResetCache();

    __LOG_DESTRUCT
    }
    

#pragma mark --- "MFSMAILPLUGIN - MAILBOX RELATED" ---


/**
 * Note that as the msgstore performs the delete immediately the observer will get
 * called from within this method.
 */
 void CBasePlugin::DeleteMailBoxByUidL(
    const TFSMailMsgId& aMailBoxId,
    MFSMailRequestObserver& aOperationObserver,
	const TInt aRequestId )
    {
    __LOG_ENTER( "DeleteMailBoxByUidL" )

    RPointerArray< CMsgStoreAccount > accounts;
    iMsgStore->AccountsL( accounts );
    CleanupResetAndDestroyClosePushL( accounts );

    TFSProgress progress = { TFSProgress::EFSStatus_RequestCancelled, 0, 0, 0 };
    progress.iError = KErrNotFound;

    TInt count = accounts.Count();
    for( TInt i = 0; i < count; i++ )
        {
        CMsgStoreMailBox* msbox = iMsgStore->OpenAccountL( *accounts[i] );
        CleanupStack::PushL( msbox );
      
        if( aMailBoxId.Id() == msbox->Id() )
            {
            __LOG_WRITE_INFO( "Found, about to start the delete.." )

            //remove from the cache.
            if ( KErrNotFound != iMailboxes.Find( msbox->Id() ) )
                {
                iMailboxes.RemoveL( msbox->Id() );
                }
            
            iMsgStore->DeleteAccountL( *accounts[i] );
            CleanupStack::PopAndDestroy( msbox );
            
            progress.iError = KErrNone;
            progress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
            
            __LOG_WRITE_INFO( "Delete complete." )
            break;
            }
      
        CleanupStack::PopAndDestroy( msbox );
        }

    CleanupStack::PopAndDestroy( &accounts );

    aOperationObserver.RequestResponseL( progress, aRequestId );
    __LOG_EXIT
    }


/**
 *
 */
 void CBasePlugin::ListMailBoxesL( RArray<TFSMailMsgId>& aMailboxes )
    {
    __LOG_ENTER( "ListMailBoxesL" )
    
    RPointerArray< CMsgStoreAccount > accounts;
    iMsgStore->AccountsL( accounts );
    CleanupResetAndDestroyClosePushL( accounts );

    TFSMailMsgId fsid;
    fsid.SetPluginId( TUid::Uid( GetPluginId() ) );
    
    __LOG_WRITE_FORMAT1_INFO( "Plugin id: 0x%X,", GetPluginId() )
    
    TInt count = accounts.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        __LOG_WRITE_FORMAT1_INFO( "Owner id: 0x%X,", accounts[i]->Owner() )
        
        if ( accounts[i]->Owner() == GetPluginId() )
            {
            CMsgStoreMailBox* msbox = iMsgStore->OpenAccountL( *accounts[i] );
            CleanupStack::PushL( msbox );

            fsid.SetId( msbox->Id() );
            aMailboxes.AppendL( fsid );
        
            __LOG_WRITE_FORMAT1_INFO( "Found mailbox with id of 0x%X,", msbox->Id() )
            __LOG_WRITE_FORMAT1_INFO( "and account name of %S.", &accounts[i]->Name() )

            CleanupStack::PopAndDestroy( msbox );
            }
        }

    CleanupStack::PopAndDestroy( &accounts );
    __LOG_EXIT
    }


/**
 *
 * @param aMailBoxId if not found leaves with KErrNotFound.
 */
 CFSMailBox* CBasePlugin::GetMailBoxByUidL( const TFSMailMsgId& aMailBox )
    {
    __LOG_ENTER( "GetMailBoxByUidL" )

    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBox.Id() );
    
    CFSMailBox* result = CFSMailBox::NewL( aMailBox );
    CleanupStack::PushL( result ); 

    CFSMailAddress* ownMailBox = CFSMailAddress::NewLC();
    ownMailBox->SetEmailAddress( *mailBox.iMailBoxName );
    
    if ( !( mailBox.iMailBoxDisplayName->Length() ) )
        {
        ownMailBox->SetDisplayName( KNullDesC );
        result->SetName( *mailBox.iMailBoxName );
        }
    else
        {
        ownMailBox->SetDisplayName( *(mailBox.iMailBoxDisplayName) );
        result->SetName( *(mailBox.iMailBoxDisplayName) );
        }
        
    result->SetOwnMailAddressL( ownMailBox );
    
    CleanupStack::Pop( ownMailBox );

    MMRInfoProcessor* infoProcessor = CBaseMrInfoProcessor::NewL( *this );
    result->SetMRInfoProcessorL( infoProcessor );
    
    //JOJA-83VJ4L Plugins need to set the correct Calendar Database ID for MRUI to work
    //get the calendar file name from the derived class
    const TDesC& calFileName = CalendarFileName();
    if ( calFileName != KNullDesC )
        {
        //the derived class does use its own calendar file, check if that file exists
        //the file may not exist because cal sync was disabled, or the file was deleted from the calendar UI
        CCalSession* calSession = CCalSession::NewL();
        CleanupStack::PushL( calSession );
        
        TRAPD( error, calSession->OpenL( calFileName ) );
        if ( error == KErrNone )
            {
            //calendar file exists, get its file id, and set it to cMail Celendar Info via the extention
            TCalFileId fileId = KNullFileId; 
            calSession->FileIdL( fileId );
            
            //get the extention api for the MR info, set the cal db id, and release the extention
            CEmailExtension* extension = NULL;
            TRAP( error, extension = result->ExtensionL( KMailboxExtMrCalInfo ) );
            if ( error == KErrNone )
                {
                CMRCalendarInfo* calInfo = reinterpret_cast<CMRCalendarInfo*>( extension );
                calInfo->SetCalendarDatabaseIdL( fileId ); // cannot actually leave
                result->ReleaseExtension( calInfo );
                }
            }
        
        CleanupStack::PopAndDestroy( calSession );
        }
    
    CleanupStack::Pop( result );
    
    __LOG_EXIT
    return result;
    }


#pragma mark --- "MFSMAILPLUGIN - FOLDER RELATED" ---

/**
 *
 * @param aMailBoxId if not found leaves with KErrNotFound.
 */
 TFSMailMsgId CBasePlugin::GetStandardFolderIdL(
    const TFSMailMsgId& aMailBoxId,
    const TFSFolderType aFolderType )
    
    {
    __LOG_ENTER( "GetStandardFolderIdL" )
    
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );
    
    TFSMailMsgId result;
    result.SetPluginId( TUid::Uid( GetPluginId() ) );
    
    if( EFSInbox <= aFolderType && aFolderType <= EFSDeleted )
        {
        result.SetId( mailBox.iRootFolders.iFolders[ aFolderType ] );
        }

    __LOG_EXIT
    return result;
    }


/**
 * Note that the unseen count is not set by the plugin as this information is not
 * readily available.
 *
 * @param aMailBoxId if not found leaves with KErrNotFound.
 */
 CFSMailFolder* CBasePlugin::GetFolderByUidL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aFolderId )

    {
    __LOG_ENTER( "GetFolderByUidL" )
    
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );

    TUint totalCount, unreadCount;
    mailBox().MessageCountsL( aFolderId.Id(), totalCount, unreadCount );

    //prepare the fsfw folder.
    CFSMailFolder* result = CFSMailFolder::NewLC( aFolderId );

    result->SetMailBoxId( aMailBoxId );
    result->SetMessageCount( totalCount );
    result->SetUnreadCount( unreadCount );

    //get the folder name from the msgstore folder object.
    __LOG_WRITE_INFO( "About to get the folder's name." )
    CMsgStoreFolder* msgStoreFolder = mailBox().FolderL( aFolderId.Id() );
    CleanupStack::PushL( msgStoreFolder );

    TUint idx;
    if ( msgStoreFolder->FindProperty( KMsgStorePropertyFolderDisplayName, idx ) )
        {
        const TDesC& name = msgStoreFolder->PropertyValueDesL( idx );
        result->SetFolderName( const_cast<TDesC&>( name ) );
        }
    else
        {
        //the root folders will get names from the UI. the base plugin should not
        //deal with any localization.
        result->SetFolderName( KNullDesC() );
        }

    TFSFolderType folderType = EFSOther;
    TUint i = 0;
    if ( msgStoreFolder->FindProperty( KMsgStorePropertyFolderType, i ) )
        {
        __LOG_WRITE_INFO( "Getting the folder's type." )
        folderType = (TFSFolderType)( msgStoreFolder->PropertyValueUint32L( i ) );
        result->SetFolderType( folderType );
        }

    TFSMailMsgId parentId( GetPluginId(), msgStoreFolder->ParentId() );
    result->SetParentFolderId( parentId );
    CleanupStack::PopAndDestroy( msgStoreFolder );

    RPointerArray<CMsgStoreFolder> folders;
    CleanupResetAndDestroyClosePushL( folders );
    
    mailBox().FoldersL( aFolderId.Id(), folders );
    result->SetSubFolderCount( folders.Count() );
    
    BlockCopyMoveFromFoldersL( aFolderId, folderType, *result );

    CleanupStack::PopAndDestroy( &folders );
    CleanupStack::Pop( result );

    __LOG_EXIT
    return result;
    } //GetFolderByUidL.


/**
 * @param aFolderId parent folder id.
 * @param aMailBoxId if not found leaves with KErrNotFound.
 * @return if the folder already exists returns the existing one.
 */
 CFSMailFolder* CBasePlugin::CreateFolderL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aParentFolderId,
    const TDesC& aFolderName,
    const TBool aSync )

    {
    __LOG_ENTER( "CreateFolderL" )
    
    CMailboxInfo& mailboxInfo = GetMailboxInfoL( aMailBoxId.Id() );

    CMsgStorePropertyContainer* props = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( props );

    props->AddPropertyL( KMsgStorePropertyName, aFolderName );
    props->AddPropertyL( KMsgStorePropertyFolderType, EMsgStoreOther );

    TMsgStoreId parent = aParentFolderId.Id();
    TMsgStoreId folderId =
        mailboxInfo().CreateFolderL( parent, *props, aSync );
    CleanupStack::PopAndDestroy( props );

    //return the FS folder.
    TFSMailMsgId fid( GetPluginId(), folderId );

    CFSMailFolder* result = CFSMailFolder::NewLC( fid );

    result->SetParentFolderId( aParentFolderId );
    result->SetFolderName( aFolderName );

    result->SetMailBoxId( aMailBoxId );
    result->SetMessageCount( 0 );
    result->SetUnreadCount( 0 );

    BlockCopyMoveFromFoldersL( fid, EFSOther, *result );
    
    CleanupStack::Pop( result );
    __LOG_EXIT
    return result;
    }


/**
 *
 * @param aMailBoxId if not found leaves with KErrNotFound.
 */
 void CBasePlugin::DeleteFolderByUidL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aFolder )
    
    {
    __LOG_ENTER( "DeleteFolderByUidL" )

    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );
    mailBox().DeleteFolderL( aFolder.Id() );
    
    __LOG_EXIT
    }

/**
 *
 */
 void CBasePlugin::ListFoldersL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aFolderId,
    RPointerArray<CFSMailFolder>& aFolderList )
    
    {
    __LOG_ENTER( "ListFoldersL, aFolderId" )
    
    TMsgStoreId folderId;
    if ( 0 == aFolderId.Id() )
    	{
    	folderId = aMailBoxId.Id();
    	}
    else
    	{
    	folderId = aFolderId.Id();
    	}
    
	DoListFoldersL( aMailBoxId, folderId, EFalse, aFolderList );
    __LOG_EXIT 
    }

/**
 *
 */
 void CBasePlugin::ListFoldersL(
    const TFSMailMsgId& aMailBoxId,
    RPointerArray<CFSMailFolder>& aFolderList )
    
    {
    __LOG_ENTER( "ListFoldersL" )
    DoListFoldersL( aMailBoxId, KMsgStoreInvalidId, ETrue, aFolderList );
    __LOG_EXIT
    }


#pragma mark --- "MFSMAILPLUGIN - MESSAGE RELATED" ---


/**
 *
 * @param aMailBoxId if not found leaves with KErrNotFound.
 */
 MFSMailIterator* CBasePlugin::ListMessagesL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aFolderId,
    const TFSMailDetails aDetails,
    const RArray<TFSMailSortCriteria>& aSorting )
   
    {
    __LOG_ENTER( "ListMessagesL" )
    
    CMailboxInfo& mailbox = GetMailboxInfoL( aMailBoxId.Id() );
    
    RMsgStoreSortCriteria criteria;
    CleanupClosePushL( criteria );
 
    criteria.iFolderId = aFolderId.Id();
    if( aSorting.Count() > 0 )
        {
        criteria.iSortBy = static_cast<TMsgStoreSortByField>( aSorting[0].iField - 1);
        criteria.iSortOrder = static_cast<TMsgStoreSortOrder>( aSorting[0].iOrder );
        criteria.iSecondarySortOrder = EMsgStoreSortDescending;
        }
    else
        {
        __LOG_WRITE_INFO( "No sorting criteria supplied, falling back to the default." )
        criteria.iSortBy = EMsgStoreSortByReceivedDate;
        criteria.iSortOrder = EMsgStoreSortDescending;
        }
    
    //determine the details to request from the msgstore.
    if ( EFSMsgDataStructure & aDetails || EFSMsgDataEnvelope & aDetails
        || EFSMsgDataSubject & aDetails )
        {
        criteria.AddResultPropertyL( KMsgStorePropertySubject );
        }

    if ( EFSMsgDataStructure & aDetails || EFSMsgDataEnvelope & aDetails
        || EFSMsgDataDate & aDetails )
        {
        criteria.AddResultPropertyL( KMsgStorePropertyReceivedAt );
        criteria.AddResultPropertyL( KMsgStorePropertySent );
        }

    if ( EFSMsgDataStructure & aDetails || EFSMsgDataEnvelope & aDetails
        || EFSMsgDataSender & aDetails )
        {
        criteria.AddResultPropertyL( KMsgStorePropertyFrom );
        }
    
    if ( EFSMsgDataStructure & aDetails || EFSMsgDataEnvelope & aDetails
        || EFSMsgDataIdOnly & aDetails )
        {
        /**@ no result properties added means the msgstore leaves with -6; ask
        about it because EFSMsgDataIdOnly means id-only.*/
        criteria.AddResultPropertyL( KMsgStorePropertyTo );
        }

    if ( EFSMsgDataStructure & aDetails || EFSMsgDataEnvelope & aDetails )
        {
        criteria.AddResultPropertyL( KMsgStorePropertyFlags );
        criteria.AddResultPropertyL( KMsgStorePropertySize );
        criteria.AddResultPropertyL( KMsgStorePropertyCc );
        criteria.AddResultPropertyL( KMsgStorePropertyBcc );
        }

    CMsgStoreSortResultIterator* msIter = mailbox().SortL( criteria );
    CleanupStack::PopAndDestroy( &criteria );

    CleanupStack::PushL( msIter );
    HMailIterator* result = new (ELeave) HMailIterator( *this, msIter, aMailBoxId );
    CleanupStack::Pop( msIter );
    
    __LOG_EXIT
    return result;
    }

/**
 *
 * @param aMailBoxId if not found leaves with KErrNotFound.
 */
 CFSMailMessage* CBasePlugin::GetMessageByUidL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aFolderId*/,
    const TFSMailMsgId& aMessageId,
    const TFSMailDetails aDetails )
    
    {
    __LOG_ENTER( "GetMessageByUidL" )
    
    CMsgStoreMessage* message = GetCachedMsgL( aMailBoxId.Id(), aMessageId.Id() );

    CFSMailMessage* result = CFSMailMessage::NewLC( aMessageId );
    TranslateMsgStorePropsL( aMailBoxId, *message, *result, aDetails );
    result->SetMailBoxId( aMailBoxId );
    CleanupStack::Pop( result );

    __LOG_EXIT
    return result;
    }


/**
 *
 * @param aMailBoxId if not found leaves with KErrNotFound.
 */
 void CBasePlugin::DeleteMessagesByUidL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aFolderId,
    const RArray<TFSMailMsgId>& aMessages )
    
    {
    __LOG_ENTER( "DeleteMessagesByUidL" )

    CDelayedDeleteMessagesOp* delayedOp = CDelayedDeleteMessagesOp::NewLC(
        aMailBoxId.Id(), aFolderId.Id(), aMessages );
    iDelayedOpsManager->EnqueueOpL( delayedOp );
    CleanupStack::Pop( delayedOp );

    __LOG_EXIT
    }


/**
 * Drafts folder assumed.
 *
 * @param aMailBoxId if not found leaves with KErrNotFound.
 */
 CFSMailMessage* CBasePlugin::CreateMessageToSendL(
    const TFSMailMsgId& aMailBox )
    
    {
    __LOG_ENTER( "CreateMessageToSendL" )
    
    CMailboxInfo& mailboxInfo = GetMailboxInfoL( aMailBox.Id() );

    CMsgStorePropertyContainer* props = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( props );
    props->AddOrUpdatePropertyL( KMsgStorePropertyContentType, KFSMailContentTypeMultipartMixed );

    // Set reply-to address, if it exists
    HBufC* replyToAddressString = GetReplyToAddressL( aMailBox );
    
    if ( NULL != replyToAddressString )
        {
        CleanupStack::PushL( replyToAddressString );
        
        if ( replyToAddressString->Length() > 0 )
            {
            RMsgStoreAddress replyToAddress;
            CleanupClosePushL( replyToAddress );
            
            replyToAddress.iEmailAddress.Create( *replyToAddressString );
            replyToAddress.iDisplayName.Create( KNullDesC );
            
            props->AddOrUpdatePropertyL( KMsgStorePropertyReplyTo, replyToAddress );
            
            CleanupStack::PopAndDestroy( &replyToAddress );
            }        
        CleanupStack::PopAndDestroy( replyToAddressString );
        }
    
    CMsgStoreMessage* message = mailboxInfo().CreateMessageL(
        mailboxInfo.iRootFolders.iFolders[EFSDraftsFolder], *props );

    CleanupStack::PopAndDestroy( props );
    CleanupStack::PushL( message );
    
    //create the body part.
    props = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( props );
    
    props->AddOrUpdatePropertyL(
        KMsgStorePropertyContentType, KFSMailContentTypeMultipartAlternative );
    CMsgStoreMessagePart* bodyPart = message->AddChildPartL( *props );
    
    CleanupStack::PopAndDestroy( props );
    CleanupStack::PushL( bodyPart );
    
    //create the plain text part.
    props = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( props );

    props->AddPropertyL(
        KMsgStorePropertyContentType, KFSMailContentTypeTextPlain );

    _LIT(KMessageBodyCharset, "UTF-8");
    props->AddPropertyL( KMsgStorePropertyCharset, KMessageBodyCharset );

    _LIT(KMessageBodyDisposition, "inline");
    props->AddPropertyL(
        KMsgStorePropertyContentDisposition, KMessageBodyDisposition );

    HBufC* signature = GetSignatureL( aMailBox );
    if ( signature )
        {
        CleanupStack::PushL( signature );       

        props->AddPropertyL(
            KMsgStorePropertySize, static_cast<TUint32>( signature->Length() ) );
        props->AddPropertyL(
            KMsgStorePropertyRetrievedSize,
            static_cast<TUint32>( signature->Length() ) );
        }
    
    CMsgStoreMessagePart* textPlain = bodyPart->AddChildPartL( *props );
    if ( signature )
        {
        CleanupStack::Pop( signature );
        }
    CleanupStack::PopAndDestroy( props );           
    if ( signature )
        {
        CleanupStack::PushL( signature );
        }
    CleanupStack::PushL( textPlain );
    
    //add signature, if it exists
	if ( NULL != signature && signature->Length() )
        {
        TPtrC8 ptr8(
            reinterpret_cast<const TUint8*>( signature->Ptr() ),
            signature->Size() );
        
        textPlain->ReplaceContentL( ptr8 );
		}

	CleanupStack::PopAndDestroy( textPlain );
    if ( signature )
        {
        CleanupStack::PopAndDestroy( signature );
        }
    
//<qmail> 
    //create the text/html part.
    props = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( props );
    
	props->AddPropertyL(
        KMsgStorePropertyContentType, KFSMailContentTypeTextHtml );
	
    props->AddPropertyL( KMsgStorePropertyCharset, KMessageBodyCharset );

    props->AddPropertyL(
        KMsgStorePropertyContentDisposition, KMessageBodyDisposition );
    
    CMsgStoreMessagePart* htmlPlain = bodyPart->AddChildPartL( *props );
    CleanupStack::PopAndDestroy( props );         
    CleanupStack::PushL( htmlPlain );
    htmlPlain->AppendToContentL(KNullDesC8);
    CleanupStack::PopAndDestroy( htmlPlain );
//</qmail>
    
    //delete body part
    CleanupStack::PopAndDestroy( bodyPart );
	
	//done.
    message->CommitL();
    
    TFSMailMsgId folderId( GetPluginId(), message->ParentId() );
    TFSMailMsgId msgId( GetPluginId(), message->Id() );
    CleanupStack::PopAndDestroy( message );

    CFSMailMessage* result = CFSMailMessage::NewL( msgId );    
    
    result->SetMailBoxId( aMailBox );
    result->SetFolderId( folderId );
    result->SetContentType( KFSMailContentTypeMultipartMixed );
    
    __LOG_EXIT
    return result;
    }

 // <qmail>
 /**
  *
  */
 void CBasePlugin::CreateMessageToSendL(
    const TFSMailMsgId& aMailBoxId,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    {
     CDelayedMessageToSendOp* delayedOp = CDelayedMessageToSendOp::NewLC(
    *this,aMailBoxId,aOperationObserver,aRequestId);
    iDelayedOpsManager->EnqueueOpL( delayedOp );
    iDelayedOpReqs.AppendL(delayedOp);
    CleanupStack::Pop( delayedOp );      
    }
 // </qmail> 
/**
 *
 */
 CFSMailMessage* CBasePlugin::CreateForwardMessageL(
    const TFSMailMsgId& aMailBox,
    const TFSMailMsgId& aOriginal,
    const TDesC& aHeaderDescriptor )
    {
    return CreateForwardReplyMessageL( aMailBox, aOriginal, EFalse, aHeaderDescriptor, ETrue );
    }


/**
 * Relying on the UI for the subject and recipients.
 */
 CFSMailMessage* CBasePlugin::CreateReplyMessageL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aOriginalMessageId,
    const TBool aReplyToAll,
    const TDesC& aHeaderDescriptor )
    {
    return CreateForwardReplyMessageL( aMailBoxId, aOriginalMessageId, aReplyToAll, aHeaderDescriptor, EFalse );
    }


/**
 *
 */
 CFSMailMessage* CBasePlugin::CreateMrReplyMessageL(
    const TFSMailMsgId& aMailBoxId,
    MMRInfoObject& /*aMeetingRequest*/,
    const TFSMailMsgId& aOriginalMessageId )
    {
    return CreateReplyMessageL( aMailBoxId, aOriginalMessageId, EFalse, KNullDesC );
    }


/**
 *
 * @param aMailBoxId if not found leaves with KErrNotFound.
 */
 void CBasePlugin::StoreMessageL(
    const TFSMailMsgId& aMailBoxId,
    CFSMailMessage& aMessage )
    
    {
    __LOG_ENTER( "StoreMessageL" )
    
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );

    CMsgStoreMessage* message = mailBox().FetchMessageL(
        aMessage.GetMessageId().Id(), KMsgStoreInvalidId );
    CleanupStack::PushL( message );

    //determine whether the message is in the inbox folder or some of its
    //children; the translation is different for inbox and all the other folders.
    TBool inInbox = EFalse;
    TMsgStoreId parentId = message->ParentId();
    while ( KMsgStoreInvalidId != parentId
    	&& EMsgStoreFolderContainer == mailBox().ContainerTypeById( parentId ) )
    	{
    	if ( parentId == mailBox.iRootFolders.iFolders[EFSInbox] )
    		{
    		inInbox = ETrue;
    		break;
    		}
    	else
    		{
    		CMsgStoreFolder* parent = mailBox().FolderL( parentId );
    		parentId = parent->ParentId();
    		delete parent;
    		}
    	}
    
    TranslateEmailFwMessageL( aMessage, *message, inInbox );
    message->StorePropertiesL();
    
    CleanupStack::PopAndDestroy( message );
    
    ResetCache();
    __LOG_EXIT
    }

// <qmail>
/**
 *
 * @param aMailBoxId Mailbox id.
 * @param aOperationObserver Operation observer.
 * @param aRequestId The request id.
 */
 void CBasePlugin::StoreMessagesL(
    const TFSMailMsgId& aMailBox,
    RPointerArray<CFSMailMessage> &messages,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId )
    
    {
    __LOG_ENTER( "StoreMessagesL" )
    
    CDelayedMessageStorerOp* delayedOp = CDelayedMessageStorerOp::NewLC(
    aMailBox,messages,aOperationObserver,aRequestId);
    iDelayedOpsManager->EnqueueOpL( delayedOp );
    CleanupStack::Pop( delayedOp );
   
     
    __LOG_EXIT
    }
// </qmail>


// <qmail>
/**
 * Asynchronous message part storing
 *
 * @param aMessagePart email parts data to be stored
 * @param aOperationObserver Observer for the operation 
 * @param aRequestId id of the operation
 */
 void CBasePlugin::StoreMessagePartsL(
    RPointerArray<CFSMailMessagePart>& aMessageParts,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
{
    __LOG_ENTER( "StoreMessagePartsL" )
            
    CDelayedMessageStorerOp* delayedOp = CDelayedMessageStorerOp::NewLC(
    aMessageParts,aOperationObserver,aRequestId);
    iDelayedOpsManager->EnqueueOpL( delayedOp );
    CleanupStack::Pop( delayedOp );
    
    __LOG_EXIT
}
// <//qmail>

/**
 * Async operation, starts fetching.
 * @param aRequestId
 */
 void CBasePlugin::FetchMessagesL(
    const TFSMailMsgId& /*aMailBox*/,
    const TFSMailMsgId& /*aFolder*/,
    const RArray<TFSMailMsgId>& /*aMessageIds*/,
    TFSMailDetails /*aDetails*/,
    MFSMailRequestObserver& /*aObserver*/,
    TInt /*aRequestId*/ )

    {
    /**@ is this really necessary for the base plugin ?*/
    }


/**
 *
 */
 void CBasePlugin::GetMessagesL(
    const TFSMailMsgId& /*aMailBoxId*/,
    const TFSMailMsgId& /*aParentFolderId*/,
    const RArray<TFSMailMsgId>& /*aMessageIds*/,
    RPointerArray<CFSMailMessage>& /*aMessageList*/,
    const TFSMailDetails /*aDetails*/ )
    
    {
    /**@ is this really necessary for the base plugin ?*/
    }


/**
 * CFSMailPlugin::SendMessageL
 */
 void CBasePlugin::SendMessageL( CFSMailMessage& aMessage )    
    {
    __LOG_ENTER( "SendMessageL1" )

    TMsgStoreId mailBoxId = aMessage.GetMailBoxId().Id();
    TMsgStoreId msgId = aMessage.GetMessageId().Id();
    
    CMsgStoreMessage* message = GetCachedMsgL( mailBoxId, msgId );
    __LOG_WRITE8_FORMAT1_INFO( "Fetched message : 0x%X.", msgId )

    TranslateEmailFwMessageL( aMessage, *message, EFalse );
    //the sendmessagel will store the message's properties!

    TTime sentTime;
    sentTime.UniversalTime();
    aMessage.SetDate( sentTime );

    CMailboxInfo& mailBox = GetMailboxInfoL( aMessage.GetMailBoxId().Id() );
    SendMessageL( mailBox(), *message, sentTime );

    __LOG_EXIT
    }


/**
 * Plugin could use this method to "send" a message if the CFSMailPlugin's is
 * not necessary. The method also stores the message properties before doing so.
 * This allows for optimizing away of unnecessary writes to the msgstore.
 * @param aSentTime the sent time you want set for the message.
 */
 void CBasePlugin::SendMessageL(
    CMsgStoreMailBox& aMailBox,
    CMsgStoreMessage& aMsg,
    const TTime& aSentTime )    
    {
    __LOG_ENTER( "SendMessageL2" )

    //set the sent stamp.
    aMsg.AddOrUpdatePropertyL( KMsgStorePropertySent, aSentTime );
    // Due to no Received Time in the sent messages, it caused sorting problem
    // from MsgStore
    aMsg.AddOrUpdatePropertyL( KMsgStorePropertyReceivedAt, aSentTime );
    aMsg.StorePropertiesL();

    // move the msg to the outbox.
    TFSMailMsgId id( GetPluginId(), aMailBox.Id() );
    TFSMailMsgId drafts = GetStandardFolderIdL( id, EFSDraftsFolder );
    TFSMailMsgId outbox = GetStandardFolderIdL( id, EFSOutbox );

    aMailBox.CopyMessageL( aMsg.Id(), drafts.Id(), outbox.Id() );
    
    CDelayedDeleteMessagesOp* delayedOp = CDelayedDeleteMessagesOp::NewLC(
        aMailBox.Id(), KMsgStoreInvalidId, aMsg.Id() );
    iDelayedOpsManager->EnqueueOpL( delayedOp );
    CleanupStack::Pop( delayedOp );

    __LOG_EXIT
    }

/**
 *
 */
 void CBasePlugin::MoveMessagesL(
    const TFSMailMsgId& aMailBoxId,
    const RArray<TFSMailMsgId>& aMessageIds,
    const TFSMailMsgId& aSourceFolderId,
    const TFSMailMsgId& aDestinationFolderId )

    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );

    TInt count = aMessageIds.Count();
    for ( TUint i = 0; i < count; i++ )
        {
        mailBox().MoveMessageL(
            aMessageIds[i].Id(),
            aSourceFolderId.Id(),
            aDestinationFolderId.Id() );
        }    
    }


/**
 *
 */
 void CBasePlugin::CopyMessagesL(
    const TFSMailMsgId& aMailBoxId,
    const RArray<TFSMailMsgId>& aMessageIds,
    RArray<TFSMailMsgId>& aNewMessages,
    const TFSMailMsgId& aSourceFolderId,
    const TFSMailMsgId& aDestinationFolderId )

    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );

    TFSMailMsgId newId( GetPluginId(), KMsgStoreInvalidId );
    
    TInt count = aMessageIds.Count();
    for ( TUint i = 0; i < count; i++ )
        {
        TMsgStoreId msgStoreId = mailBox().CopyMessageL(
            aMessageIds[i].Id(),
            aSourceFolderId.Id(),
            aDestinationFolderId.Id() );
        
        newId.SetId( msgStoreId );
        aNewMessages.AppendL( newId );
        }
    }


#pragma mark --- "Plugin Helpers" ---


/**
 * Get the MsgStore mailbox instance for the specified FSEF mailbox id.
 *
 * @param aId mailbox id, if none can be found leaves with KErrNotFound.
 */
 CMailboxInfo& CBasePlugin::GetMailboxInfoL(
    TMsgStoreId aId )
    {
    
    __ASSERT_DEBUG( NULL != iMsgStore, ::BasePluginPanic( ENoMsgStoreSessionAssert ) );
    
    //if not present in the cache, add it.
    if ( !iMailboxes.ContainsKey(aId) )
        {
        RPointerArray< CMsgStoreAccount > accounts;
        iMsgStore->AccountsL( accounts );
        CleanupResetAndDestroyClosePushL( accounts );

        TBool found = EFalse;

        TInt count = accounts.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            CMsgStoreMailBox* msbox = iMsgStore->OpenAccountL( *accounts[i] );
            CleanupStack::PushL( msbox );

            if ( aId == msbox->Id() )
                {
                CMailboxInfo* mailBox = new (ELeave) CMailboxInfo( this );
                CleanupStack::PushL( mailBox );
                mailBox->iMailBox = msbox;
                //always observe for mailbox changes to keep the cache
                //up-to-date.
                mailBox->iMailBox->AddObserverL( this );

                mailBox->iMailBoxName = accounts[i]->Name().AllocL();
                mailBox->iBrandingId = KNullDesC().AllocL();
                
                RBuf mailboxDisplayName;
                mailboxDisplayName.CleanupClosePushL();
                GetMailboxDisplayNameL( aId, mailboxDisplayName );
                mailBox->iMailBoxDisplayName = mailboxDisplayName.AllocL();
                CleanupStack::PopAndDestroy( &mailboxDisplayName );
                
                MsgStoreFolderUtils::GetRootFoldersL( *msbox, mailBox->iRootFolders );

                TInt64* key = new (ELeave) TInt64( msbox->Id() );
                CleanupDeletePushL( key );
                iMailboxes.InsertL( key, mailBox );   //the map owns the memory.

                CleanupStack::Pop( key );
                CleanupStack::Pop( mailBox );
                CleanupStack::Pop( msbox );
                
                found = ETrue;
                break;
                }

            CleanupStack::PopAndDestroy( msbox );
            }

        CleanupStack::PopAndDestroy( &accounts );
        
        if( !found )
            {
            User::Leave( KErrNotFound );
            }
        }
    
    return iMailboxes.GetValueL( aId );
    } //GetMailboxInfoL.


/**
 * Get the MsgStore mailbox instance for the specified FSEF mailbox id.
 *
 * @param aId mailbox id, if none can be found leaves with KErrNotFound.
 */
void CBasePlugin::GetMailboxDisplayNameL(
    TMsgStoreId aId,
    RBuf& aDisplayName )
    {
    
    __ASSERT_DEBUG( NULL != iMsgStore, ::BasePluginPanic( ENoMsgStoreSessionAssert ) );
    
    RPointerArray< CMsgStoreAccount > accounts;
    iMsgStore->AccountsL( accounts );
    CleanupResetAndDestroyClosePushL( accounts );

    TInt count = accounts.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CMsgStoreMailBox* msbox = iMsgStore->OpenAccountL( *accounts[i] );
        CleanupStack::PushL( msbox );

        if( aId == msbox->Id() )
            {
            TUint index = 0;
            if ( msbox->FindProperty( KMsgStorePropertyName, index ) )
                {
                    const TDesC& string = msbox->PropertyValueDesL( index );
                    if (string.Length() > 0)
                        {
                        aDisplayName.Close();
                        aDisplayName.CreateL( string );
                        }
                }
                CleanupStack::PopAndDestroy( msbox );
                break;
            }
        else
            {
	             CleanupStack::PopAndDestroy( msbox );
	        }
        }

        CleanupStack::PopAndDestroy( &accounts );
    } //GetMailboxDisplayNameL.

/**
 * @param aFolderId if set to KMsgStoreInvalidId makes a recursive list.
 */
void CBasePlugin::DoListFoldersL(
    TFSMailMsgId aMailBoxId,
    TMsgStoreId aFolderId,
    TBool aRecursive,
    RPointerArray<CFSMailFolder>& aFolderList )
    
    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );
    
    RPointerArray<CMsgStoreFolder> folders;
    CleanupResetAndDestroyClosePushL( folders );
    
    if ( aRecursive )
        {
        mailBox().FoldersL( folders );
        }
    else
        {
        mailBox().FoldersL( aFolderId, folders );
        }
    
    TFSMailMsgId folderId;
    folderId.SetPluginId( TUid::Uid( GetPluginId() ) );
    
    TInt count = folders.Count();
    for( TInt i = 0; i < count; i++ )
        {
        folderId.SetId( folders[i]->Id() );
        
        CFSMailFolder* folder = GetFolderByUidL( aMailBoxId, folderId );

        CleanupStack::PushL( folder );
        aFolderList.AppendL( folder );
        CleanupStack::Pop( folder );
        }
    
    CleanupStack::PopAndDestroy( &folders );
    }

/**
 * This methoed must be called before destruction of 
 * CBasePlugin derived class. To fix bug EIZU-7XVF2Z.
 */
void CBasePlugin::DeleteDelayedOpsManager()
    {
    delete iDelayedOpsManager;
    iDelayedOpsManager = NULL;
    }

/**
 *
 */
CFSMailAddress* CBasePlugin::FetchEmailAddressL(
    CMsgStorePropertyContainer& aMessage,
    TUint aIdx )

    {
    RMsgStoreAddress value;
    CleanupClosePushL( value );

    aMessage.PropertyValueAddressL( aIdx, value );

    CFSMailAddress* result = CFSMailAddress::NewL();
    result->SetEmailAddress( value.iEmailAddress );
    if ( KNullDesC() != value.iDisplayName )
        {
        result->SetDisplayName( value.iDisplayName );
        }
    else
        {
        /**@ the UI would crash if there was no display name.*/
        result->SetDisplayName( value.iEmailAddress );
        }

    CleanupStack::PopAndDestroy( &value );
    
    return result;
    }


/**
 *
 */
 CMsgStoreMessagePart* CBasePlugin::GetBodyPartL(
    CMsgStoreMessage& aMessage,
    const TDesC& aContentType )
    {
    CMsgStoreMessagePart* result = NULL;
    
    RPointerArray<CMsgStoreMessagePart> parts;
    CleanupResetAndDestroyClosePushL( parts );

    aMessage.ChildPartsL( parts );
    if ( 0 < parts.Count() )
        {
        RPointerArray<CMsgStoreMessagePart> childParts;
        CleanupResetAndDestroyClosePushL( childParts );
        parts[0]->ChildPartsL( childParts );
        
        TInt count = childParts.Count();
        if ( 0 < count )
            {
            for ( TInt i = 0; i < count; i++ )
                {
                TUint idx = 0;
                if ( childParts[i]->FindProperty( KMsgStorePropertyContentType, idx, idx ) )
                    {
                    const TDesC& contentType = childParts[i]->PropertyValueDesL( idx );
                    if ( contentType == aContentType )
                        {
                        result = childParts[i];
                        childParts.Remove( i );
                        break;
                        }
                    }
                }
            
            }
        else
            {
            TUint idx = 0;
            if ( parts[0]->FindProperty( KMsgStorePropertyContentType, idx, idx ) )
                {
                const TDesC& contentType = parts[0]->PropertyValueDesL( idx );
                if ( contentType == aContentType )
                    {
                    result = parts[0];
                    parts.Remove( 0 );
                    }
                }
            }
            
        CleanupStack::PopAndDestroy( &childParts );
        }
        
    CleanupStack::PopAndDestroy( &parts );

    return result;
    } //GetBodyPartL.

/**
 *
 */
 CFSMailMessage* CBasePlugin::CreateForwardReplyMessageL(
    const TFSMailMsgId& aMailBox,
    const TFSMailMsgId& aOriginal,
    const TBool aReplyToAll,
    const TDesC& aHeaderDescriptor,
    TBool aKeepAttachments,
    TBool aKeepMeetingRequestInfo )
    {
    __LOG_ENTER( "CreateForwardReplyMessageL" )
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBox.Id() );

    CMsgStoreMessage* message = mailBox().FetchMessageL(
        aOriginal.Id(), KMsgStoreInvalidId );
    CleanupStack::PushL( message );

    TMsgStoreId newMsgId = mailBox().CopyMessageL(
        aOriginal.Id(), message->ParentId(),
        mailBox.iRootFolders.iFolders[EFSDraftsFolder] );
    
    CMsgStoreMessage* newMsg = mailBox().FetchMessageL( newMsgId, KMsgStoreInvalidId );
    CleanupStack::PushL( newMsg );

    if ( !aReplyToAll )
        {
        //Clear all To, Cc addresses for Reply, Forward
        RemoveAllPropertiesL( *newMsg, KMsgStorePropertyTo );
        RemoveAllPropertiesL( *newMsg, KMsgStorePropertyCc );
        }
    else
        {
        //Remove the mailbox address from the reply to addresses
        HBufC* mailBoxAdd = GetMailBoxAddressL( aMailBox );
        CleanupStack::PushL( mailBoxAdd );
        
        if ( NULL != mailBoxAdd )
            {           
            TUint idx1 = 0;
            while (newMsg->FindProperty( KMsgStorePropertyTo, idx1, idx1 ) )
                {
                RMsgStoreAddress tempAdd;
                CleanupClosePushL( tempAdd );
                
                newMsg->PropertyValueAddressL( idx1, tempAdd );
                if ( tempAdd.iEmailAddress == *mailBoxAdd )
                    {
                    newMsg->RemovePropertyL( idx1 );
                    CleanupStack::PopAndDestroy( &tempAdd );
                    break;
                    }
                CleanupStack::PopAndDestroy( &tempAdd );
                ++idx1;
                }
        
            idx1 = 0;
            while (newMsg->FindProperty( KMsgStorePropertyCc, idx1, idx1 ) )
                {
                RMsgStoreAddress tempAdd;
                CleanupClosePushL( tempAdd );
                        
                newMsg->PropertyValueAddressL( idx1, tempAdd );
                if ( tempAdd.iEmailAddress == *mailBoxAdd )
                    {
                    newMsg->RemovePropertyL( idx1 );
                    CleanupStack::PopAndDestroy( &tempAdd );
                    break;
                    }
                CleanupStack::PopAndDestroy( &tempAdd );
                ++idx1;
                } 
            }
        
        CleanupStack::PopAndDestroy( mailBoxAdd );
        }
    
	SetReplyOrForwardToFieldL( *newMsg );

    //Clear the response flag.
	TUint idx = 0;
    if ( newMsg->FindProperty( KMsgStorePropertyFlags, idx ) )
        {
        TUint32 flags = newMsg->PropertyValueUint32L( idx );
        flags &= ~EFSMsgFlag_Answered;
        newMsg->AddOrUpdatePropertyL( KMsgStorePropertyFlags, flags );
        }

    //Clear the From field, ReplyTo field
    RemoveAllPropertiesL( *newMsg, KMsgStorePropertyFrom );
    RemoveAllPropertiesL( *newMsg, KMsgStorePropertyReplyTo );
    
    //Use reply-to address, if it exists
    HBufC* replyToAddressString = GetReplyToAddressL( aMailBox );
       
    if ( NULL != replyToAddressString )
        {
        CleanupStack::PushL( replyToAddressString );
           
        if ( replyToAddressString->Length() > 0 )
            {
            RMsgStoreAddress replyToAddress;
            CleanupClosePushL( replyToAddress );
               
            replyToAddress.iEmailAddress.Create( *replyToAddressString );
            replyToAddress.iDisplayName.Create( KNullDesC );
               
            newMsg->AddOrUpdatePropertyL( KMsgStorePropertyReplyTo, replyToAddress );
               
            CleanupStack::PopAndDestroy( &replyToAddress );
            }
        CleanupStack::PopAndDestroy( replyToAddressString );
       }
          
    newMsg->StorePropertiesL();

    //deal with the attachments accordingly.
    /**@ the case where the attachments haven't been downloaded but some smart
     * reply/fw tag is present in the msg body needs to be dealt with in the specific
     * plugins.*/
    if ( aKeepAttachments == EFalse )
        {
        RPointerArray<CMsgStoreMessagePart> parts;
        CleanupResetAndDestroyClosePushL( parts );

        newMsg->ChildPartsL( parts );
        TInt count = parts.Count();

        for ( TInt i = 1; i < count; i++ )
            {
            newMsg->RemoveChildPartL( parts[i]->Id() );
            }
            
        CleanupStack::PopAndDestroy( &parts );
        }

    if ( aKeepMeetingRequestInfo == EFalse )
    	{
    	idx = 0;
    	if ( newMsg->FindProperty( KMsgStorePropertyMeetingRequest, idx ) )
    		{
    		CBasePlugin::RemoveAllPropertiesL( *newMsg, KMsgStorePropertyMeetingRequest );
    		}
    	}
    
    CFSMailMessage* result = CFSMailMessage::NewL(
        TFSMailMsgId( GetPluginId(), newMsgId ) );
    CleanupStack::PushL( result );
    
    TranslateMsgStorePropsL( aMailBox, *newMsg, *result );
    result->SetMailBoxId( aMailBox );

    /**@ candidate for refactoring out to a separate body dealing method.*/
    CMsgStoreMessagePart* srcPart = GetBodyPartL( *message );
    if ( NULL != srcPart )
        {
        CleanupStack::PushL( srcPart );
        CMsgStoreMessagePart* dstPart = GetBodyPartL( *newMsg );

        if ( NULL != dstPart )
            {
            CleanupStack::PushL( dstPart );
            
            HBufC* signature = GetSignatureL( aMailBox );
            if ( NULL != signature )
                {
                CleanupStack::PushL( signature );
                TPtrC8 ptr8(
                    reinterpret_cast<const TUint8*>( signature->Ptr() ),
                    signature->Size() );
                dstPart->ReplaceContentL( ptr8 );
                CleanupStack::PopAndDestroy( signature );

                __LOG_WRITE_INFO( "Set the email signature." )
                }

            //the quoted message header.
            if ( aHeaderDescriptor != KNullDesC )
                {
                TPckgBuf<TReplyForwardParams> rfParsPckg;
                rfParsPckg.Copy( aHeaderDescriptor );
                
                TPtrC8 ptrHeader8(
                    reinterpret_cast<const TUint8*>( rfParsPckg().iHeader->Ptr() ),
                    rfParsPckg().iHeader->Size() );
                NULL != signature ? dstPart->AppendToContentL( ptrHeader8 )
                    : dstPart->ReplaceContentL( ptrHeader8 );

                __LOG_WRITE8_FORMAT1_INFO(
                    "Original body content length: %d.", srcPart->ContentLengthL() )

                //copy the body content from the original.
                TInt startOffset = 0;
                RBuf8 buf;
                buf.CreateL( KQuotedReadBufferSize );
                CleanupClosePushL( buf );

                do
                    {
                    srcPart->FetchContentToBufferL( buf, startOffset );
                    dstPart->AppendToContentL( buf );
                    
                    startOffset += buf.Size();
                    }
                while ( buf.Size() == KQuotedReadBufferSize );

                CleanupStack::PopAndDestroy( &buf );

                TInt newFetchedSize = dstPart->ContentLengthL();
                __LOG_WRITE8_FORMAT1_INFO(
                    "Reply/fw body content length: %d.", newFetchedSize )

                TInt idx = dstPart->AddOrUpdatePropertyL(
                    KMsgStorePropertySize, static_cast<TUint32>( newFetchedSize ) );
                dstPart->StorePropertyL( idx );
                idx = dstPart->AddOrUpdatePropertyL(
                    KMsgStorePropertyRetrievedSize, static_cast<TUint32>( newFetchedSize ) );
                dstPart->StorePropertyL( idx );
                
                result->SetContentSize( newFetchedSize );
                result->SetFetchedContentSize( newFetchedSize );

                //determine whether the original msg was truncated.
                TBool wasTruncated = EFalse;
                TUint srcIdx = 0;
                if ( srcPart->FindProperty( KMsgStorePropertySize, srcIdx ) )
                    {
                    TUint size = srcPart->PropertyValueUint32L( srcIdx );

                    srcIdx = 0;
                    if ( srcPart->FindProperty( KMsgStorePropertyRetrievedSize, srcIdx ) )
                        {
                        TUint retrievedSize = srcPart->PropertyValueUint32L( srcIdx );
                        
                        if ( retrievedSize < size )
                            {
                            wasTruncated = ETrue;
                            }
                        }
                    }
                    
                //if truncated append msg that the whole msg is available server-side.
                if ( wasTruncated )
                    {
                    _LIT( KTwoLineFeeds, "\n\n" );
                    TPtrC8 ptrTwoLineFeeds8(
                        reinterpret_cast<const TUint8*>( KTwoLineFeeds().Ptr() ),
                        KTwoLineFeeds().Size() );
                    dstPart->AppendToContentL( ptrTwoLineFeeds8 );

                    TPtrC8 ptrSmartTag8(
                        reinterpret_cast<const TUint8*>( rfParsPckg().iSmartTag->Ptr() ),
                        rfParsPckg().iSmartTag->Size() );
                    dstPart->AppendToContentL( ptrSmartTag8 );
                    }
                }

            /**@ incorrect but per activesync request - the base plugin doesn't know anything about how the unicode characters
            are being encoded by the specific implementations when they talk to their backends.*/
            _LIT(KMessageBodyCharset, "UTF-8");
            dstPart->AddPropertyL( KMsgStorePropertyCharset, KMessageBodyCharset );

            CleanupStack::PopAndDestroy( dstPart );
            }
        CleanupStack::PopAndDestroy( srcPart );
        }
    
    CleanupStack::Pop( result );
    CleanupStack::PopAndDestroy( newMsg );
    CleanupStack::PopAndDestroy( message );
    
    __LOG_EXIT
    return result;
    } //CreateForwardReplyMessageL.


/**
 * If there is a reply to address, add it to the TO list
 * else add the FROM address to the TO address list   
 */
/*private*/ void CBasePlugin::SetReplyOrForwardToFieldL( CMsgStoreMessage& aMsg )
	{
    RMsgStoreAddress toAddr;
    CleanupClosePushL( toAddr );
    
    TBool usingReplyTo = EFalse;
    
    TUint idx = 0;
    if( aMsg.FindProperty( KMsgStorePropertyReplyTo, idx ) )
    	{
    	//if both address and name are null strings no memory will be allocated.
        aMsg.PropertyValueAddressL( idx, toAddr );
        
        if ( toAddr.iEmailAddress.Length() || toAddr.iDisplayName.Length() )
        	{
        	usingReplyTo = ETrue;
        	}
    	}

    if ( EFalse == usingReplyTo )
		{
        idx = 0;
        if ( aMsg.FindProperty( KMsgStorePropertyFrom, idx ) )
        	{
            aMsg.PropertyValueAddressL( idx, toAddr );
        	}
		}

    aMsg.AddPropertyL( KMsgStorePropertyTo, toAddr );
    CleanupStack::PopAndDestroy( &toAddr );
	}


/**
 * RefreshMailboxCacheL - This will refresh the mailbox instance cache maintained within the base plugin.  
 *
 */
 CMailboxInfo& CBasePlugin::RefreshMailboxCacheL( TMsgStoreId aMailBoxId )
    {
    //save the observers
    CMailboxInfo& oldMailBox = GetMailboxInfoL( aMailBoxId );
    
    RPointerArray<MFSMailEventObserver> observers;
    // the elements are not owned by the base plugin, so just close the array
    CleanupClosePushL( observers );

    TUint count = oldMailBox.iObservers.Count();
    for ( TUint i = 0; i < count; i++ )
    	{
    	observers.AppendL( oldMailBox.iObservers[i] );
    	}
    
    //remove from the cache
    if ( KErrNotFound != iMailboxes.Find( aMailBoxId ) )
        {
        iMailboxes.RemoveL( aMailBoxId );
        }

    //force the refresh and restore the observes
    CMailboxInfo& newMailBox = GetMailboxInfoL( aMailBoxId );
    
    for ( TUint i = 0; i < count; i++ )
    	{
    	newMailBox.iObservers.AppendL( observers[i] );
    	}
    
    CleanupStack::PopAndDestroy( &observers );
    return newMailBox;
    }


/**
 * RefreshCachedMailBoxDisplayNameL -   This will check to see if the mailbox display name has changed.  If so it will
 *                                      update the cached mailbox display name and set mailBoxNameHasChanged to ETrue 
 *
 */
void CBasePlugin::RefreshCachedMailBoxDisplayNameL( TBool& aMailBoxNameHasChanged, const TMsgStoreId& aMailBoxId )
    {
    aMailBoxNameHasChanged = EFalse;
    
    RBuf currentDisplayName;
    currentDisplayName.CleanupClosePushL();
    GetMailboxDisplayNameL( aMailBoxId, currentDisplayName );
    
    CMailboxInfo& mailboxInfo = GetMailboxInfoL( aMailBoxId );
    TDesC* cachedDisplayName( mailboxInfo.iMailBoxDisplayName );

    if ( ( cachedDisplayName && currentDisplayName != *cachedDisplayName ) ||
         ( !cachedDisplayName && currentDisplayName.Length() ) )
        {
        aMailBoxNameHasChanged = ETrue;
        delete mailboxInfo.iMailBoxDisplayName;
        mailboxInfo.iMailBoxDisplayName = NULL;
        mailboxInfo.iMailBoxDisplayName = currentDisplayName.AllocL();
        }
    
    CleanupStack::PopAndDestroy( &currentDisplayName );
    }
 // <qmail> 
/**
 * Id - Return plugin implementation id  
 *
 */ 
TUid CBasePlugin::Id( ) const
    {
    return TUid::Uid( GetPluginId() );
    }
// </qmail>


