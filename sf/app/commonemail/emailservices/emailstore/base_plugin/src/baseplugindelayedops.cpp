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
* Description:  Email interface implementation.
*
*/

//<qmail>
#include <utf.h>
//<//qmail>

#include "MsgStoreWritablePropertyContainer.h"

#include "baseplugindelayedops.h"
#include "baseplugindelayedopsprivate.h"

///////////////////////////////////////////////////
// CDelayedOp                                    //
///////////////////////////////////////////////////

/**
 * 
 */
/*public virtual*/  CDelayedOp::~CDelayedOp()
    {
    Cancel();
    __LOG_DESTRUCT
    }

/**
 * 
 */
/*public*/  void CDelayedOp::SetContext(
    CBasePlugin& aPlugin, MDelayedOpsManager& aManager )
    {
    iPlugin = &aPlugin;
    iManager = &aManager;
    } 

/**
 * 
 */
/*public*/  void CDelayedOp::StartOp()
    {
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* pStatus = &iStatus;
    User::RequestComplete( pStatus, KErrNone );
    } 
/*protected*/  CDelayedOp::CDelayedOp()
    : CActive( CActive::EPriorityIdle )
    {    
    CActiveScheduler::Add( this );
    }
    
/**
 * 
 */
/*private virtual*/  void CDelayedOp::RunL()
    {
    __LOG_ENTER_SUPPRESS( "Run" );
    TBool again( EFalse );
    
    TRAPD( err, again = ExecuteOpL() );
    
    if ( KErrNone != err )
        {
        __LOG_WRITE8_FORMAT1_INFO(
            "Error while executing delayed operation: %d.", err );
        }
    
    if ( again )
        {
        StartOp();
        }
    else
        {
        //self-destroy.
        iManager->DequeueOp( *this );
//<qmail>		
		    //Remove this from iDelayedOpReqs as well.
	    for ( TInt i = 0; i < iPlugin->iDelayedOpReqs.Count(); i++ )
	        {
	        if ( iPlugin->iDelayedOpReqs[i] == this )
	            {
	            iPlugin->iDelayedOpReqs.Remove( i );
	            break;
	            }
	        }
//</qmail>			
        delete this;
        }
    }
    
/**
 * 
 */
/*protected*/  CBasePlugin& CDelayedOp::GetPlugin()
    {
    return *iPlugin;
    }

/**
 * 
 */
/*private virtual*/  void CDelayedOp::DoCancel()
    {
    }


///////////////////////////////////////////////////
// CDelayedOpsManager                            //
///////////////////////////////////////////////////

/**
 * 
 */
/*static public*/
CDelayedOpsManager* CDelayedOpsManager::NewL( CBasePlugin& aPlugin )
    {
    CDelayedOpsManager* self = new (ELeave) CDelayedOpsManager( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
/**
 * 
 */
/*public virtual*/ CDelayedOpsManager::~CDelayedOpsManager()
    {
    ExecutePendingOps();
    
    iDelayedOps.ResetAndDestroy();
    iDelayedOps.Close();
    
    __LOG_DESTRUCT
    }
    
/**
 * 
 */
/*public virtual*/ void CDelayedOpsManager::EnqueueOpL( CDelayedOp* aOp )
    {
    iDelayedOps.AppendL( aOp );
    aOp->SetContext( iPlugin, *this );        
    aOp->StartOp();
    }
    
/**
 * 
 */
/*public virtual*/ void CDelayedOpsManager::DequeueOp( const CDelayedOp& aOp )
    {
    TInt count = iDelayedOps.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        CDelayedOp* op = iDelayedOps[i];
        if ( &aOp == op )
            {
            iDelayedOps.Remove( i );
            break;
            }
        }
    }
    
/**
 * 
 */
/*public virtual*/ TInt CDelayedOpsManager::Extension1(
    TUint /*aExtensionId*/, TAny*& /*a0*/, TAny* /*a1*/ )
    {
    return KErrNotSupported;
    }

/**
 * 
 */
/*private*/ CDelayedOpsManager::CDelayedOpsManager( CBasePlugin& aPlugin )
    : iPlugin ( aPlugin )
    {
    }

/**
 * 
 */
/*private*/ void CDelayedOpsManager::ConstructL()
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedOpsManager" );
    }

/**
 * 
 */
/*private*/ void CDelayedOpsManager::ExecutePendingOps()
    {
    //check the count on every iteration to avoid missing operations being
    //enqueued by another operations.
    for ( TInt i = 0; i < iDelayedOps.Count(); ++i )
        {
        CDelayedOp* op = iDelayedOps[i];
        
        op->Cancel();
        TRAP_IGNORE( op->ExecuteOpL() );
        }
    }    


///////////////////////////////////////////////////
// CDelayedDeleteMessagesOp                      //
///////////////////////////////////////////////////

/**
 * 
 */
/*public static */ CDelayedDeleteMessagesOp* CDelayedDeleteMessagesOp::NewLC(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aFolderId,
    const RArray<TFSMailMsgId>& aMessages )
    {
    CDelayedDeleteMessagesOp* self = new (ELeave) CDelayedDeleteMessagesOp(
        aMailBoxId, aFolderId );
    CleanupStack::PushL( self );
    self->ConstructL( aMessages );
    return self;
    }

/**
 * 
 */
/*public static */ CDelayedDeleteMessagesOp* CDelayedDeleteMessagesOp::NewLC(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aFolderId,
    TMsgStoreId aMsgId )
    {
    CDelayedDeleteMessagesOp* self = new (ELeave) CDelayedDeleteMessagesOp(
        aMailBoxId, aFolderId );
    CleanupStack::PushL( self );
    self->ConstructL( aMsgId );
    return self;
    }

/**
 * 
 */
/*public virtual*/ CDelayedDeleteMessagesOp::~CDelayedDeleteMessagesOp()
    {
    iMessages.Close();
    __LOG_DESTRUCT
    }

/**
 * 
 */
/*private*/
void CDelayedDeleteMessagesOp::ConstructL(
    const RArray<TFSMailMsgId>& aMessages )
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedDeleteMessagesOp" );
            
    TInt count = aMessages.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iMessages.AppendL( aMessages[i].Id() );
        }
    }

/**
 * 
 */
/*private*/
void CDelayedDeleteMessagesOp::ConstructL(
    TMsgStoreId aMsgId )
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedDeleteMessagesOp" );
    iImmediateDelete = ETrue;
    iMessages.AppendL( aMsgId );
    }

/**
 * 
 */
/*private*/ CDelayedDeleteMessagesOp::CDelayedDeleteMessagesOp(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aFolderId )
    :
    iMailBoxId( aMailBoxId ), iFolderId( aFolderId )
    {
    }

/**
 * 
 */
/*private*/ TBool CDelayedDeleteMessagesOp::ExecuteOpL()
    {
    __LOG_ENTER( "ExecuteOpL" );

    //Maximum number of messages deleted in one go
    const TInt KNumOfDeletesBeforeYield = 30;

    //<qmail>
    TInt result(KErrNone);

    TBool runAgain = ETrue;
    TInt endIndex = iIndex + KNumOfDeletesBeforeYield;

    if (endIndex >= iMessages.Count())
        {
        endIndex = iMessages.Count();
        runAgain = EFalse; // last time, no need to run again.
        }

    CMailboxInfo& mailBoxInfo = GetPlugin().GetMailboxInfoL(iMailBoxId);
    CMsgStoreMailBox& mailBox = mailBoxInfo();

    for (; iIndex < endIndex; iIndex++)
        {
        TMsgStoreId msgId = iMessages[iIndex];

        if (EFalse == iImmediateDelete)
            {
            //try to find the message
            CMsgStoreMessage* theMessage = NULL;
            theMessage = mailBox.FetchMessageL(msgId, KMsgStoreInvalidId);
            //save parentId
            TMsgStoreId msgParentId;
            msgParentId = theMessage->ParentId();
            //check if message is in deleted or drafts folder or not.
            if (msgParentId != mailBoxInfo.iRootFolders.iFolders[EFSDeleted] &&
                msgParentId != mailBoxInfo.iRootFolders.iFolders[EFSDraftsFolder])
                {
                //if not in deleted items then move it there.
                __LOG_WRITE8_FORMAT1_INFO("Moving message 0x%X to the deleted items.", msgId );
                TRAP(result,mailBox.MoveMessageL(
                                msgId, KMsgStoreInvalidId,
                                mailBoxInfo.iRootFolders.iFolders[EFSDeleted] ));
                if (result == KErrNone)
                    {
                    GetPlugin().NotifyEventL(iMailBoxId, msgId,
                            KMsgStoreInvalidId, TFSEventMailMoved,
                            msgParentId);
                    }
                else
                    {
                    User::Leave(result);
                    }

                }
            else
                {
                //in deleted items, really delete it.
                __LOG_WRITE8_FORMAT1_INFO( "Deleting message 0x%X.", msgId );

                delete theMessage;
                TRAP(result,mailBox.DeleteMessageL( msgId, iFolderId ));
                if (result == KErrNone)
                    {
                    GetPlugin().NotifyEventL(iMailBoxId, msgId,
                            KMsgStoreInvalidId, TFSEventMailDeleted,
                            iFolderId);
                    }
                else
                    {
                    User::Leave(result);
                    }
                }
            }
        else
            {
            TRAP(result,mailBox.DeleteMessageL( msgId, iFolderId ));
            if (result == KErrNone)
                {
                GetPlugin().NotifyEventL(iMailBoxId, msgId,
                        KMsgStoreInvalidId, TFSEventMailDeleted, iFolderId);
                }
            else
                {
                User::Leave(result);
                }
            }
        //</qmail>
        } 
		__LOG_EXIT;
    return runAgain;
    }


///////////////////////////////////////////////////
// CDelayedStorePropertiesOp                     //
///////////////////////////////////////////////////

/**
 * 
 */
/*public static*/
CDelayedSetContentOp* CDelayedSetContentOp::NewLC(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aMessageId,
    TMsgStoreId aMessagePartId,
    const TDesC& aContent )
    {
    CDelayedSetContentOp* self = new (ELeave) CDelayedSetContentOp(
        aMailBoxId, aMessageId, aMessagePartId );
    CleanupStack::PushL( self );
    self->ConstructL( aContent );
    return self;
    }

/**
 * 
 */
/*public static*/
CDelayedSetContentOp* CDelayedSetContentOp::NewLC(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aMessageId,
    TMsgStoreId aMessagePartId,
    TInt aContentLength )
    {
    CDelayedSetContentOp* self = new (ELeave) CDelayedSetContentOp(
        aMailBoxId, aMessageId, aMessagePartId, aContentLength );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

/**
 * 
 */
/*public virtual*/ CDelayedSetContentOp::~CDelayedSetContentOp()
    {
    delete iContent;
    
    __LOG_DESTRUCT
    }

/**
 * 
 */
/*private*/ void CDelayedSetContentOp::ConstructL( const TDesC& aContent )
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedSetContentOp1" );
    
    iContent = aContent.AllocL();
    }

/**
 * 
 */
/*private*/ void CDelayedSetContentOp::ConstructL()
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedSetContentOp2" );
    }

/**
 * CDelayedOp::ExecuteOpL
 */
/*public virtual*/ TBool CDelayedSetContentOp::ExecuteOpL()
    {
    __LOG_ENTER( "ExecuteOpL" )

    CMsgStoreMessagePart* part = FetchMessagePartLC();

    if ( iStepOne )
        {
        part->ReplaceContentL(
            TPtrC8( reinterpret_cast<const TUint8*>(
                iContent->Ptr() ), iContent->Size() ) );
        __LOG_WRITE8_FORMAT1_INFO(
            "Stored the content of part 0x%X.", part->Id() )
        }
    else
        {
        part->AddOrUpdatePropertyL(
            KMsgStorePropertySize, static_cast<TUint32>( iContentLength ) );
        part->AddOrUpdatePropertyL(
            KMsgStorePropertyRetrievedSize,
            static_cast<TUint32>( iContentLength ) );
        
        part->StorePropertiesL();
        __LOG_WRITE8_FORMAT1_INFO(
            "Updated the properties of part 0x%X.", part->Id() )
        }
    
    CleanupStack::PopAndDestroy( part );    
    __LOG_EXIT
    return EFalse;
    }

/**
 * 
 */
/*private*/ CMsgStoreMessagePart* CDelayedSetContentOp::FetchMessagePartLC()
    {
    __LOG_ENTER_SUPPRESS( "FetchMessagePartLC" )
    CMailboxInfo& mailBox = GetPlugin().GetMailboxInfoL( iMailBoxId );
    
    CMsgStoreMessage* msg = mailBox().FetchMessageL(
        iMessageId, KMsgStoreInvalidId );
    CleanupStack::PushL( msg );
    
    CMsgStoreMessagePart* part;
    if ( KMsgStoreInvalidId == iMessagePartId )
        {
        __LOG_WRITE_INFO( "Fetched the message itself." );
        //watch the cleanupstack, part will point to the same thing thus the
        //popanddestroy is ok.
        part = msg;
        }
    else
        {
        __LOG_WRITE_INFO( "Fetched a child part of the message." );
        part = msg->ChildPartL( iMessagePartId, ETrue );
        CleanupStack::PopAndDestroy( msg );
        CleanupStack::PushL( part );
        }
    
    return part;
    }

/**
 * 
 */
/*private*/ CDelayedSetContentOp::CDelayedSetContentOp(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aMessageId,
    TMsgStoreId aMessagePartId )
    : iMailBoxId( aMailBoxId ), iMessageId( aMessageId ),
      iMessagePartId( aMessagePartId ), iStepOne( ETrue )
    {
    }

/**
 * 
 */
/*private*/ CDelayedSetContentOp::CDelayedSetContentOp(
    TMsgStoreId aMailBoxId,
    TMsgStoreId aMessageId,
    TMsgStoreId aMessagePartId,
    TInt aContentLength )
    : iMailBoxId( aMailBoxId ), iMessageId( aMessageId ),
      iMessagePartId( aMessagePartId ), iContentLength( aContentLength ),
      iStepOne( EFalse )
    {
    }

//<qmail>
///////////////////////////////////////////////////
// CDelayedMessageStorerOp                      //
///////////////////////////////////////////////////

/**
 * 
 */
/*public static */ CDelayedMessageStorerOp* CDelayedMessageStorerOp::NewLC(
        const TFSMailMsgId& aMailBox,
        RPointerArray<CFSMailMessage> &messages,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId)
    {
    CDelayedMessageStorerOp* self = new (ELeave) CDelayedMessageStorerOp(
            aMailBox, messages, aOperationObserver,aRequestId);
    CleanupStack::PushL( self );
    self->ConstructL(  );
    return self;
    }

/**
 * 
 */
/*public static */ CDelayedMessageStorerOp* CDelayedMessageStorerOp::NewLC(
        RPointerArray<CFSMailMessagePart>& aMessageParts,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId)
    {
    CDelayedMessageStorerOp* self = new (ELeave) CDelayedMessageStorerOp(
            aMessageParts, aOperationObserver, aRequestId);
    CleanupStack::PushL( self );
    self->ConstructL(  );
    return self;
    }

/**
 * 
 */
/*public virtual*/ CDelayedMessageStorerOp::~CDelayedMessageStorerOp()
    {
    __LOG_DESTRUCT
    iMessages.Reset();
    iMessageParts.Reset();
    if (iDataBuffer)
        {
        delete iDataBuffer;
        iDataBuffer = NULL;
        }
    }

/**
 * 
 */
/*private*/
void CDelayedMessageStorerOp::ConstructL( )
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedMessageStorerOp" );
        
    }

/**
 * 
 */
/*private*/ CDelayedMessageStorerOp::CDelayedMessageStorerOp(
    const TFSMailMsgId& aMailBox,
    RPointerArray<CFSMailMessage> &messages,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    : iMailBox( aMailBox ), 
      iType(EHeaders)
    {
    
    CDelayedOp::iOperationObserver = &aOperationObserver ;
    CDelayedOp::iRequestId =  aRequestId ;
       
    for(TInt i=0; i < messages.Count(); i++)
        {
        iMessages.Append(messages[i]);
        }
    }

/**
 * 
 */
/*private*/ CDelayedMessageStorerOp::CDelayedMessageStorerOp(
    RPointerArray<CFSMailMessagePart>& aMessageParts,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    :iType(EParts)
    {
    
    CDelayedOp::iOperationObserver = &aOperationObserver ;
    CDelayedOp::iRequestId =  aRequestId ;
    
    for(TInt i=0; i < aMessageParts.Count(); i++)
        {
        iMessageParts.Append(aMessageParts[i]);
        }
    }

/**
 * 
 */
/*private*/ TBool CDelayedMessageStorerOp::ExecuteOpL()
    {
    __LOG_ENTER( "ExecuteOpL" );
   
    TFSProgress progress = { TFSProgress::EFSStatus_RequestCancelled, 0, 0, 0 };
    progress.iError = KErrNotFound;
    TInt err(KErrNone);
    
    switch( iType )
        {
        case EHeaders: 
            {
            for ( TInt i = 0; i < iMessages.Count(); i++ )
                {
                TRAP(err, GetPlugin().StoreMessageL(iMailBox, *iMessages[i] ));
                if(err!=KErrNone)
                    {
                    break;
                    }
                }
            break;
            }
            
        case EParts:
            {
                for ( TInt i = 0; i < iMessageParts.Count(); i++ )
                {
                CFSMailMessagePart& part= *iMessageParts[i];
                TFSMailMsgId messageId= part.GetMessageId();
                TFSMailMsgId folderId= part.GetFolderId();
                
                if (part.GetContentType().Compare(KFSMailContentTypeTextPlain) == 0 ||
                        part.GetContentType().Compare(KFSMailContentTypeTextHtml) == 0)
                    {
                    TRAP(err,StorePartL(&part));
                    if(err!=KErrNone)
                        {
                        break;
                        }
                    TRAP(err, GetPlugin().StoreMessagePartL( part.GetMailBoxId(), folderId, messageId, part ));
                    }
                else
                    {
                    TRAP(err, GetPlugin().StoreMessagePartL( part.GetMailBoxId(), folderId, messageId, part ));
                    }
               if(err!=KErrNone)
                    {
                    break;
                    }
                }
            }
        
        default:
        break;
        }
    
    if( err == KErrNone )
        {
        progress.iError = KErrNone;
        progress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        }
		
		if(iOperationObserver)
			{
				iOperationObserver->RequestResponseL( progress, iRequestId );
			}
    
    
    __LOG_EXIT;
	return EFalse;
    }


/**
 * 
 */
void CDelayedMessageStorerOp::StorePartL(CFSMailMessagePart* aPart)
    {
    User::LeaveIfNull(aPart);

    // Text buffer for html text content
    HBufC* data16 = aPart->GetLocalTextContentLC();

    // Convert from 16 to 8 bit data -
    HBufC8* dataBuffer = HBufC8::NewLC((data16->Length() * 2) + 1);
    TPtr8 ptr8(dataBuffer->Des());
    CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr8, *data16);

    //get msgstore part
    CMailboxInfo& mailBox = GetPlugin().GetMailboxInfoL(
            aPart->GetMailBoxId().Id());

    CMsgStoreMessage* msg = mailBox().FetchMessageL(
            aPart->GetMessageId().Id(), KMsgStoreInvalidId);
    CleanupStack::PushL(msg);

    CMsgStoreMessagePart* part = msg->ChildPartL(aPart->GetPartId().Id(),
            ETrue);

    CleanupStack::PopAndDestroy(msg);
    CleanupStack::PushL(part);

    //replace content
    part->ReplaceContentL(ptr8);

    CleanupStack::PopAndDestroy(part);
    CleanupStack::PopAndDestroy(dataBuffer);
    CleanupStack::PopAndDestroy(data16);

    }


///////////////////////////////////////////////////
// CDelayedMessageToSendOp                      //
///////////////////////////////////////////////////

/**
 * 
 */
/*public static */ CDelayedMessageToSendOp* CDelayedMessageToSendOp::NewLC(
        CBasePlugin& aPlugin,
        const TFSMailMsgId& aMailBox,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId)
    {
    CDelayedMessageToSendOp* self = new (ELeave) CDelayedMessageToSendOp(
            aPlugin,aMailBox, aOperationObserver,aRequestId);
    CleanupStack::PushL( self );
    self->ConstructL(  );
    return self;
    }


/**
 * 
 */
/*public virtual*/ CDelayedMessageToSendOp::~CDelayedMessageToSendOp()
    {
    __LOG_DESTRUCT
    }

/**
 * 
 */
/*private*/
void CDelayedMessageToSendOp::ConstructL( )
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedMessageToSendOp" );
        
    }

/**
 * 
 */
/*private*/ CDelayedMessageToSendOp::CDelayedMessageToSendOp(
    CBasePlugin& aPlugin,
    const TFSMailMsgId& aMailBox,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    : iBasePlugin(aPlugin), 
      iMailBox( aMailBox )
    {
    
    CDelayedOp::iOperationObserver = &aOperationObserver ;
    CDelayedOp::iRequestId =  aRequestId ;

    }


/**
 * 
 */
/*private*/ TBool CDelayedMessageToSendOp::ExecuteOpL()
    {
    __LOG_ENTER( "ExecuteOpL" );
   
    TFSProgress progress = { TFSProgress::EFSStatus_RequestCancelled, 0, 0, 0 };
    progress.iError = KErrNotFound;
    TInt err(KErrNone);

    TRAP(err, progress.iParam = iBasePlugin.CreateMessageToSendL( iMailBox ));
    
    if( err == KErrNone  )
        {
        progress.iError = KErrNone;
        progress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        }

		if(iOperationObserver)
			{
				iOperationObserver->RequestResponseL( progress, iRequestId );
			}
    
    __LOG_EXIT;
	return EFalse;
    }


///////////////////////////////////////////////////
// CDelayedAddNewOrRemoveChildPartOp             //
///////////////////////////////////////////////////

/**
 * 
 */
/*public static */ CDelayedAddNewOrRemoveChildPartOp* CDelayedAddNewOrRemoveChildPartOp::NewLC(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aParentFolderId ,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aParentPartId ,
    const TDesC& aContentType,
    const TDesC& aFilePath,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    {
    CDelayedAddNewOrRemoveChildPartOp* self = new (ELeave) CDelayedAddNewOrRemoveChildPartOp(
            aMailBoxId, aParentFolderId, aMessageId,aParentPartId,
            aOperationObserver,aRequestId);
    CleanupStack::PushL( self );
    self->ConstructL( aContentType, aFilePath );
    return self;
    }

/**
 * 
 */
/*public static */ CDelayedAddNewOrRemoveChildPartOp* CDelayedAddNewOrRemoveChildPartOp::NewLC(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aParentFolderId ,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aParentPartId ,
    const TFSMailMsgId& aPartId ,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    {
    CDelayedAddNewOrRemoveChildPartOp* self = new (ELeave) CDelayedAddNewOrRemoveChildPartOp(
            aMailBoxId, aParentFolderId, aMessageId,aParentPartId,
            aPartId,aOperationObserver, aRequestId);
    CleanupStack::PushL( self );
    return self;
    }

/**
 * 
 */
/*public virtual*/ CDelayedAddNewOrRemoveChildPartOp::CDelayedAddNewOrRemoveChildPartOp(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aParentFolderId ,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aParentPartId ,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId) :
    iMailBoxId( aMailBoxId ),
    iParentFolderId( aParentFolderId ),
    iMessageId( aMessageId ),
    iParentPartId( aParentPartId ),
    iActionType( AddNewChild)
    {
    CDelayedOp::iOperationObserver = &aOperationObserver ;
    CDelayedOp::iRequestId =  aRequestId ;
    }

/**
 * 
 */
/*public virtual*/ CDelayedAddNewOrRemoveChildPartOp::CDelayedAddNewOrRemoveChildPartOp(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aParentFolderId ,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aParentPartId ,
    const TFSMailMsgId& aPartId ,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId) :
    iMailBoxId( aMailBoxId ),
    iParentFolderId( aParentFolderId ),
    iMessageId( aMessageId ),
    iParentPartId( aParentPartId ),
    iPartId( aPartId ),
    iActionType( RemoveChild)
    {
    CDelayedOp::iOperationObserver = &aOperationObserver ;
    CDelayedOp::iRequestId =  aRequestId ;
    }

/**
 * 
 */
/*public virtual*/ CDelayedAddNewOrRemoveChildPartOp::~CDelayedAddNewOrRemoveChildPartOp()
    {
    __LOG_DESTRUCT
    if ( iContentType )
        {
        delete iContentType;
        }
    if ( iFilePath )
        {
        delete iFilePath;
        } 
    }
	
/**
 * 
 */
/*private*/
void CDelayedAddNewOrRemoveChildPartOp::ConstructL( const TDesC& aContentType, const TDesC& aFilePath )
    {
    __LOG_CONSTRUCT( "baseplugin", "CDelayedAddNewOrRemoveChildPartOp" );      
    iContentType = aContentType.AllocL();
    iFilePath = aFilePath.AllocL();                 
    }

/**
 * 
 */
/*private*/ TBool CDelayedAddNewOrRemoveChildPartOp::ExecuteOpL()
    {
    __LOG_ENTER( "ExecuteOpL" );
    
    TFSProgress progress = { TFSProgress::EFSStatus_RequestCancelled, 0, 0, 0 };
    progress.iError = KErrNotFound;
    TInt err(KErrNone);
    
    if( iActionType == AddNewChild)
        {
        CFSMailMessagePart* tmp = NULL;
        TRAP(err, tmp = GetPlugin().NewChildPartFromFileL(iMailBoxId,iParentFolderId,
                    iMessageId,iParentPartId,iContentType->Des(),iFilePath->Des()));
        if(err == KErrNone )
            {
            tmp->SetAttachmentNameL( iFilePath->Des() );
            tmp->SaveL();
            }
        //iOperationObserver will make a copy of pointer "tmp" and will own the memory.
        progress.iParam = tmp;
        }
    else
        {
        TRAP(err, GetPlugin().RemoveChildPartL(iMailBoxId,iParentFolderId,
                    iMessageId,iParentPartId,iPartId)); 
        }

    if( err == KErrNone )
        {
        //clear plugin cache so that childparts of message can be updated next time with proper count
        GetPlugin().ResetCache();
        progress.iError = KErrNone;
        progress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        }
		
		if(iOperationObserver)
			{
				iOperationObserver->RequestResponseL( progress, iRequestId );
			}
    
    __LOG_EXIT;
    return EFalse;
    }
//</qmail>
