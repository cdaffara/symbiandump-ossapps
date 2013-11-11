/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Multi-part support.
*
*/
#include "BasePlugin.h"
#include "baseplugincommonutils.h"
#include "baseplugindelayedopsprivate.h"


/**
 *
 */
EXPORT_C void CBasePlugin::ChildPartsL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aParentFolderId*/,
	const TFSMailMsgId& aMessageId,
	const TFSMailMsgId& aParentId,
	RPointerArray<CFSMailMessagePart>& aParts )

    {
    __LOG_ENTER( "ChildPartsL" )
    
    GetCachedMsgL( aMailBoxId.Id(), aMessageId.Id() );
    RPointerArray<CMsgStoreMessagePart>* parts;
    if ( aParentId.IsNullId() )
        {
        parts = &GetCachedMsgChildrenL();
        }
    else
        {
        __LOG_WRITE_INFO( "Looking for the child parts of a part." )
        GetCachedBodyL( aParentId.Id() );
        parts = &GetCachedBodyChildrenL();
        }

    __LOG_WRITE8_FORMAT1_INFO( "No of child parts : %d.", parts->Count() );
    
    TFSMailMsgId fsId( GetPluginId(), 0 );
    TInt count = parts->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CMsgStoreMessagePart& thePart = *((*parts)[i]); 
        fsId.SetId( thePart.Id() );

        CFSMailMessagePart* fsPart = CFSMailMessagePart::NewLC( aMessageId, fsId );
        TranslateMsgStorePropsL( aMailBoxId, thePart, *fsPart );
        fsPart->SetMailBoxId( aMailBoxId );

        aParts.AppendL( fsPart );
        CleanupStack::Pop( fsPart );
        }
    
    __LOG_EXIT
    }


/**
 * @param aInsertBefore msgstore does not support that.
 */
EXPORT_C CFSMailMessagePart* CBasePlugin::NewChildPartL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aParentFolderId,
	const TFSMailMsgId& aMessageId,
	const TFSMailMsgId& aParentPartId,
	const TFSMailMsgId& /*aInsertBefore*/,
	const TDesC& aContentType )
	
    {
    return NewChildPartFromFileL(
        aMailBoxId, aParentFolderId, aMessageId, aParentPartId,
        aContentType, KNullDesC );
    }


/**
 *
 */
EXPORT_C void CBasePlugin::RemoveChildPartL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aParentFolderId*/,
	const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& /*aParentPartId*/,
    const TFSMailMsgId& aPartId )

    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );
    
    CMsgStoreMessage* message = mailBox().FetchMessageL(
        aMessageId.Id(), KMsgStoreInvalidId );
    CleanupStack::PushL( message );
    
    message->RemoveChildPartL( aPartId.Id() );

    CleanupStack::PopAndDestroy( message );
    }

// <qmail>
/**
 *
 */
EXPORT_C void CBasePlugin::RemoveChildPartL(
    const TFSMailMsgId&  aMailBoxId ,
    const TFSMailMsgId&  aParentFolderId ,
    const TFSMailMsgId&  aMessageId,
    const TFSMailMsgId&  aParentPartId ,
    const TFSMailMsgId&  aPartId,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId )
    {
    //Asynchronous operation to remove child part.
    //CDelayedAddNewOrRemoveChildPartOp::ExecuteOpL will eventually delete childpart 
    CDelayedAddNewOrRemoveChildPartOp* op = CDelayedAddNewOrRemoveChildPartOp::NewLC(
            aMailBoxId, aParentFolderId , aMessageId, aParentPartId,  aPartId,
            aOperationObserver, aRequestId);
    iDelayedOpsManager->EnqueueOpL( op );
    CleanupStack::Pop( op );
    }
// </qmail>

/**
 *
 */
EXPORT_C void CBasePlugin::SetPartContentFromFileL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aParentFolderId*/,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aMessagePartId,
    const TDesC& aFilePath )

    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );
    
    CMsgStoreMessage* message = mailBox().FetchMessageL(
        aMessageId.Id(), KMsgStoreInvalidId );
    CleanupStack::PushL( message );
    
    CMsgStoreMessagePart* part = message->ChildPartL( aMessagePartId.Id(), ETrue );
    CleanupStack::PushL( part );
    
    part->ReplaceContentWithFileL( aFilePath );

    CleanupStack::PopAndDestroy( part );
    CleanupStack::PopAndDestroy( message );
    }


/**
 * Goes through all the part props and stores them in the msg store.
 *
 * @param aMailBoxId
 * @param aParentFolderId - not needed for the msg store.
 * @param aMessageId
 * @param aMessagePart
 */
EXPORT_C void CBasePlugin::StoreMessagePartL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aParentFolderId*/,
 	const TFSMailMsgId& aMessageId,
 	CFSMailMessagePart& aMessagePart )

    {
    __LOG_ENTER( "StoreMessagePartL" )
    
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );
    
    CMsgStoreMessage* message = mailBox().FetchMessageL(
        aMessageId.Id(), KMsgStoreInvalidId );
    CleanupStack::PushL( message );
    
    CMsgStoreMessagePart* part = message->ChildPartL(
        aMessagePart.GetPartId().Id(), ETrue );
    CleanupStack::PushL( part );

    TranslateEmailFwMessageL( aMessagePart, *part, EFalse );
    part->StorePropertiesL();

    CleanupStack::PopAndDestroy( part );
    CleanupStack::PopAndDestroy( message );
    
    __LOG_EXIT
    }


/**
 *
 */
EXPORT_C CFSMailMessagePart* CBasePlugin::CopyMessageAsChildPartL(
    const TFSMailMsgId& /*aMailBoxId*/,
	const TFSMailMsgId& /*aMessageId*/,
	const TFSMailMsgId& /*aParentFolderId*/,
	const TFSMailMsgId& /*aParentPartId*/,
	const TFSMailMsgId& /*aInsertBefore*/,
	const CFSMailMessage& /*aMessage*/ )
    {
    /*
    Regarding

    1. Check the need of the function from UI, CopyMessageAsChildPartL(), that copies given
    message as new child part to this part.

    Currently UI will enable only adding attachment files from file system so implementing
    this in FW/plugins is not high priority and can be postponed.

    // Jouni
    */

    /**@ implement.*/
    return NULL;
    }

/**
 *
 */
EXPORT_C CFSMailMessagePart* CBasePlugin::MessagePartL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aParentFolderId*/,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aMessagePartId )
    
    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );

    CMsgStoreMessage* message = mailBox().FetchMessageL(
        aMessageId.Id(), KMsgStoreInvalidId );
    CleanupStack::PushL( message );

    CMsgStoreMessagePart* part = message->ChildPartL( aMessagePartId.Id(), ETrue );
    CleanupStack::PushL( part );

    TFSMailMsgId msgPartId( GetPluginId(), part->Id() );    
    CFSMailMessagePart* result = CFSMailMessagePart::NewLC( aMessageId, msgPartId );
    TranslateMsgStorePropsL( aMailBoxId, *part, *result );
    result->SetMailBoxId( aMailBoxId );

    CleanupStack::Pop( result );
    CleanupStack::PopAndDestroy( part );
    CleanupStack::PopAndDestroy( message );
    
    return result;
    }


/**
 * The method will leave with KErrNotSupported if the msgstore is encrypted or if trying to open the file for writing.
 */
EXPORT_C TInt CBasePlugin::GetMessagePartFileL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aParentFolderId*/,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aMessagePartId,
    RFile& aFileHandle,
    TBool aForWriting)

    {
    if (aForWriting)
        {
        return KErrNotSupported;
        }
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );
    
    CMsgStoreMessage* msg = mailBox().FetchMessageL( aMessageId.Id(), KMsgStoreInvalidId );
    CleanupStack::PushL( msg );
    
    CMsgStoreMessagePart* part = msg->ChildPartL( aMessagePartId.Id(), ETrue );
    CleanupStack::PushL( part );
    
    //will leave with KErrNotSupported if encrypted.
    part->OpenOriginalContentFileL( aFileHandle );

    CleanupStack::PopAndDestroy( part );
    CleanupStack::PopAndDestroy( msg );
    
    return KErrNone;
    }


/**
 * @param aParentFolderId not used
 */
EXPORT_C void CBasePlugin::CopyMessagePartFileL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aParentFolderId*/,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aMessagePartId,
    const TDesC& aFilePath )

    {
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );

    CMsgStoreMessage* msg = mailBox().FetchMessageL( aMessageId.Id(), KMsgStoreInvalidId );
    CleanupStack::PushL( msg );
    
    CMsgStoreMessagePart* part = msg->ChildPartL( aMessagePartId.Id(), ETrue );
    CleanupStack::PushL( part );
    
    TUint idx = 0;
    if ( part->FindProperty( KMsgStorePropertyName, idx ) )
        {
        const TDesC& name = part->PropertyValueDesL( idx );
        
        HBufC* buf = HBufC::NewLC( name.Length() + aFilePath.Length() );
        *buf = aFilePath;
        buf->Des().Append( name );
        
        part->FetchContentToFileL( *buf );
        
        CleanupStack::PopAndDestroy( buf );
        }

    CleanupStack::PopAndDestroy( part );
    CleanupStack::PopAndDestroy( msg );
    }


/**
 *
 */
EXPORT_C void CBasePlugin::GetContentToBufferL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aParentFolderId*/,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aMessagePartId,
    TDes& aBuffer,
    const TUint aStartOffset )
    
    {
    __LOG_ENTER( "GetContentToBufferL" )
    
    CMsgStoreMessage* msg = GetCachedMsgL( aMailBoxId.Id(), aMessageId.Id() );
    
    //could the cache be extended to cache the child parts in this case ?
    CMsgStoreMessagePart* part = msg->ChildPartL( aMessagePartId.Id(), ETrue );
    CleanupStack::PushL( part );

    TUint outSize = aBuffer.MaxSize();

    /**@ look for ways to avoid the copying.*/
    __LOG_WRITE8_FORMAT1_INFO( "Output buffer size: %d bytes.", outSize );

    HBufC8* buf = HBufC8::NewLC( outSize );
    TPtr8 ptr = buf->Des();
    part->FetchContentToBufferL( ptr, aStartOffset );
    __LOG_WRITE8_FORMAT1_INFO(
        "Msgstore content buffer size: %d bytes.", buf->Size() );
    
	TUint msgStoreSize = ptr.Size();
	TUint theSize = outSize > msgStoreSize ? msgStoreSize : outSize;
	
	TPtrC16 convert( reinterpret_cast<const TUint16*>( buf->Ptr() ), theSize/2 );
	aBuffer = convert;

    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PopAndDestroy( part );
    
    __LOG_EXIT
    } //GetContentToBufferL.


/**
 *
 */
EXPORT_C void CBasePlugin::SetContentL(
    const TDesC& aBuffer,
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aParentFolderId*/,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aMessagePartId )

    {
    __LOG_ENTER( "SetContentL" )
        
    TMsgStoreId partId = aMessagePartId.IsNullId()
        ? KMsgStoreInvalidId : aMessagePartId.Id();
    
    //part one, sets the content.
    CDelayedSetContentOp* op = CDelayedSetContentOp::NewLC(
        aMailBoxId.Id(), aMessageId.Id(), partId, aBuffer );
    iDelayedOpsManager->EnqueueOpL( op );
    CleanupStack::Pop( op );

    //part two, then updates the part's properties.
    op = CDelayedSetContentOp::NewLC(
        aMailBoxId.Id(), aMessageId.Id(), partId, aBuffer.Length() );
    iDelayedOpsManager->EnqueueOpL( op );
    CleanupStack::Pop( op );

    __LOG_EXIT
    } //SetContentL.


/**
 * @param aPartIds ids of the parts to be removed from the message with id aMessageId.
 */
EXPORT_C void CBasePlugin::RemovePartContentL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aParentFolderId*/,
    const TFSMailMsgId& aMessageId,
    const RArray<TFSMailMsgId>& aPartIds )

    {
    __LOG_ENTER( "RemovePartContentL" )

    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );

    CMsgStoreMessage* message = mailBox().FetchMessageL(
        aMessageId.Id(), KMsgStoreInvalidId );
    CleanupStack::PushL( message );

    for ( TInt i = 0; i < aPartIds.Count(); i++ )
        {
        CMsgStoreMessagePart* part = message->ChildPartL( aPartIds[i].Id() );
        CleanupStack::PushL( part );

        HandleRemovePartContentL( *message, *part );
        
        CleanupStack::PopAndDestroy( part );
        }

    CleanupStack::PopAndDestroy( message );
    __LOG_EXIT
    }


/**
 *
 */
EXPORT_C CFSMailMessagePart* CBasePlugin::NewChildPartFromFileL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aParentFolderId,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aParentPartId,
	const TDesC& aContentType,
    const TDesC& aFilePath )
    {
    RFile dummyFile;
    return NewChildPartFromFilePathOrHandleL( 
    		aMailBoxId,
    		aParentFolderId,
    		aMessageId,
    		aParentPartId,
    		aContentType,
    		aFilePath,
    		EFalse,
    		dummyFile
    		);
    } //NewChildPartFromFileL.


// <qmail>
/**
 *
 */
EXPORT_C void CBasePlugin::NewChildPartFromFileL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aParentFolderId ,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aParentPartId ,
    const TDesC& aContentType,
    const TDesC& aFilePath,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId )
    {
    //Asynchronous operation to add child part.
    //CDelayedAddNewOrRemoveChildPartOp::ExecuteOpL will eventually add childpart 
    CDelayedAddNewOrRemoveChildPartOp* op = CDelayedAddNewOrRemoveChildPartOp::NewLC(
            aMailBoxId, aParentFolderId , aMessageId, aParentPartId,  aContentType,
            aFilePath, aOperationObserver, aRequestId);
    iDelayedOpsManager->EnqueueOpL( op );
    CleanupStack::Pop( op );

    }

// </qmail> 
/**
 *
 */
EXPORT_C CFSMailMessagePart* CBasePlugin::NewChildPartFromFileL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aParentFolderId,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aParentPartId,
	const TDesC& aContentType,
	RFile& aFile )

    {
    RFile dummyFile;
    return NewChildPartFromFilePathOrHandleL( 
    		aMailBoxId,
    		aParentFolderId,
    		aMessageId,
    		aParentPartId,
    		aContentType,
    		KNullDesC,
    		ETrue,
    		aFile
    		);
    } //NewChildPartFromFileL.


/**
 * Helper method to avoid duplicated code
 */
CFSMailMessagePart* CBasePlugin::NewChildPartFromFilePathOrHandleL(
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& /*aParentFolderId*/,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aParentPartId,
	const TDesC& aContentType,
    const TDesC& aFilePath,
    TBool  aUseFileHandle, 
    RFile& aFile )
	{
    CMailboxInfo& mailBox = GetMailboxInfoL( aMailBoxId.Id() );
    
    CMsgStoreMessage* message = mailBox().FetchMessageL(
        aMessageId.Id(), KMsgStoreInvalidId );
    CleanupStack::PushL( message );

    //determine the parent part - could be the message or any of its parts.
    CMsgStoreMessagePart* parent;
    if ( aParentPartId.IsNullId() )
        {
        parent = message;
        CleanupStack::Pop( message );
        }
    else
        {
        parent = message->ChildPartL( aParentPartId.Id(), ETrue );
        CleanupStack::PopAndDestroy( message );
        }
    CleanupStack::PushL( parent );

    //msgstore's message content type.
    CMsgStorePropertyContainer* props = CMsgStorePropertyContainer::NewL();
    CleanupStack::PushL( props );
    
    if ( aContentType != KNullDesC )
        {
        props->AddOrUpdatePropertyL( KMsgStorePropertyContentType, aContentType );
        }
    
    CMsgStoreMessagePart* part = NULL;
    if ( aUseFileHandle )
    	{
    	part = parent->AddChildPartL( *props, aFile );
    	}
    else
    	{
    	part = parent->AddChildPartL( *props, aFilePath );
    	}
    CleanupStack::PopAndDestroy( props );
    CleanupStack::PushL( part );

    //update the size and the fetched size.
    TUint contentLength = part->ContentLengthL();
    if ( 0 != contentLength )
        {
        part->AddOrUpdatePropertyL(
            KMsgStorePropertySize, static_cast<TUint32>( contentLength ) );
        part->AddOrUpdatePropertyL(
            KMsgStorePropertyRetrievedSize, static_cast<TUint32>( contentLength ) );
        
        part->StorePropertiesL();
        }
    
    TFSMailMsgId fsId( GetPluginId(), part->Id() );
    CleanupStack::PopAndDestroy( part );

    //prepare the fw object.
    CFSMailMessagePart* result = CFSMailMessagePart::NewL( aMessageId, fsId );

    if ( aContentType != KNullDesC )
        {
        result->SetContentType( aContentType );
        }
    result->SetContentSize( contentLength );
    result->SetFetchedContentSize( contentLength );
    result->SetMailBoxId( aMailBoxId );

    CleanupStack::PopAndDestroy( parent );

    return result;
	} //NewChildPartFromFileNameOrHandleL


/**
 * 
 */
EXPORT_C /*virtual*/ void CBasePlugin::HandleRemovePartContentL(
	CMsgStoreMessage& /* aMsg */,
	CMsgStoreMessagePart& aPart )

	{
    TUint idx = 0;
    idx = aPart.AddOrUpdatePropertyL(
    	KMsgStorePropertyRetrievedSize, static_cast<TUint32>( 0 ) );
	aPart.StorePropertyL( idx );
    
    aPart.RemoveContentL();
	}

