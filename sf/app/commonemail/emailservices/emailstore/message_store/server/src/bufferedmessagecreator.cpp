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
* Description: Message creator implementation
*
*/

#include "bufferedmessagecreator.h"

#include "ContainerStoreContentManager.h"
#include "ContainerStoreUtils.h"
#include "ContainerStoreEncryption.h"
#include "messagestoreutils.h"


/**
 * 
 */
/*public static*/
CBufferedMessageCreator* CBufferedMessageCreator::NewL(
    CContainerStore& aStore )
    {
    CBufferedMessageCreator* self = new (ELeave) CBufferedMessageCreator();
    
    CleanupStack::PushL( self );
    self->ConstructL( aStore );
    CleanupStack::Pop( self );
    
    return self;
    }


/**
 * 
 */
/*public*/ TBool CBufferedMessageCreator::EnqueueL(
    CContainerStore& aStore,
    TMsgStoreCreateContainerCmdParams& aParams,
    const RBuf8& aProperties )
    {
    TBool doEnqueue = EFalse;
    
    if ( ( aParams.iType & EMsgStoreContainerMask ) == EMsgStoreMessageBits
        && EFalse == aStore.IsEncryptionEnabled() )
        {
        if ( EFalse == iIsBatchInsert )
            {
            //if not in batch-mode: a message container; discard current
            //message; start buffering.
            ResetL( aStore );
            }
        
        //in batch mode containers will be queued until the batch operation is
        //marked as complete.
            
        aStore.AllocateIdsBlockL( iIds );
        doEnqueue = ETrue;        
        iInProgress = ETrue;
        }
    else if ( ( aParams.iType & EMsgStoreContainerMask ) == EMsgStorePartBits
        && IsContainerBuffered( aParams.iParentId ) )
        {
        //a part container that belongs to a buffered message container; keep
        //buffering.
        doEnqueue = ETrue;
        }
    
    if ( doEnqueue )
        {
        AddRequestL( aStore, aParams, aProperties );
        }

    return doEnqueue;
    }

/**
 * 
 */
/*private*/ void CBufferedMessageCreator::AddRequestL(
    CContainerStore& aStore,
    TMsgStoreCreateContainerCmdParams& aParams,
    const RBuf8& aProperties )
    {
    aParams.iId = GetNextContainerIdL( aStore );
    aParams.iId |= aParams.iType;

    ContainerDescriptor* part = new (ELeave) ContainerDescriptor(
        aParams, aProperties );
    iContainers.AppendL( part );
    }
    
/**
 * 
 */
/*public*/ TBool CBufferedMessageCreator::IsContainerBuffered(
    TMsgStoreId aId ) const
    {
    TBool belongsToCurrentMessage = EFalse;
    
    if ( iInProgress )
        {
        //the buffering has started, now check the buffered containers.
    
        TInt count = iContainers.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            ContainerDescriptor& container = *iContainers[i];
            
            if ( container.iParams.iId == aId )
                {
                //the parent container has been buffered thus the current
                //container must be buffered too.
                belongsToCurrentMessage = ETrue;
                break;
                }
            }
        }
        
    return belongsToCurrentMessage;
    }

/**
 * 
 */
/*public*/
void CBufferedMessageCreator::CommitMessageL(
    CContainerStore& aStore,
    MContainerStoreObserver* aObserver )
    {
    //in batch mode commits are postponed until the batch job is marked as
    //complete.
    if ( EFalse == iIsBatchInsert )
        {
        iInProgress = EFalse;        
        CommitMessagesL( aStore, aObserver );
        }
    }
    
/**
 *
 */
/*public*/
void CBufferedMessageCreator::AppendContentL(
    CContainerStore& aStore,
    TMsgStoreId aId,
    RBuf8& aContentBuf )
    {
    TFileName filePath;
    GetUncommittedContentPath( aStore, aId, filePath );

    RFs& rfs = aStore.StoreUtils().FileSystem();

    //prepare the file handle.
    RFile file;
    TInt err = file.Open( rfs, filePath, EFileShareAny | EFileWrite );
    if ( KErrNotFound == err )
        {
        User::LeaveIfError( file.Create(
            rfs, filePath, EFileShareAny | EFileWrite ) );
        }
    else if ( KErrNone != err )
        {
        User::Leave( err );
        }
    CleanupClosePushL( file );
    
    //now append the content.
    TInt pos = 0;
    User::LeaveIfError( file.Seek( ESeekEnd, pos ) );
    User::LeaveIfError( file.Write( aContentBuf ) );
    
    CleanupStack::PopAndDestroy( &file );
    }
    
/**
 *
 */
/*public*/ void CBufferedMessageCreator::PrependContentL(
    CContainerStore& aStore,
    TMsgStoreId aId,
    RBuf8& aContentBuf )
    {
    if ( aContentBuf.Length() )
        {
        TFileName sourcePath;
        GetUncommittedContentPath( aStore, aId, sourcePath );
        
        if ( BaflUtils::FileExists(
            aStore.StoreUtils().FileSystem(), sourcePath ) )
            {
            TFileName targetPath( sourcePath );
            _LIT( KTmpExtension, ".tmp" );
            targetPath.Append( KTmpExtension );
        
            MessageStoreUtils::PrependBufferAndMoveFileL(
                aStore.StoreUtils().FileSystem(), aStore.StoreUtils(),
                sourcePath, targetPath, aContentBuf );
            }
        else
            {
            AppendContentL( aStore, aId, aContentBuf );
            }
        }
    }

/**
 * 
 */
/*public*/ void CBufferedMessageCreator::ReplaceContentL(
    CContainerStore& aStore,
    TMsgStoreId aId,
    RBuf8& aContentBuf )
    {
    TFileName filePath;
    GetUncommittedContentPath( aStore, aId, filePath );

    RFile file;
    User::LeaveIfError( file.Replace(
        aStore.StoreUtils().FileSystem(), filePath,
        EFileShareAny | EFileWrite ) );
    CleanupClosePushL( file );
    
    User::LeaveIfError( file.Write( aContentBuf ) );
    
    CleanupStack::PopAndDestroy( &file );
    }

/**
 * 
 */
/*public*/ void CBufferedMessageCreator::ReplaceContentL(
    CContainerStore& aStore,
    TMsgStoreId aId,
    RFile& aFile )
    {
    TFileName filePath;
    GetUncommittedContentPath( aStore, aId, filePath );

    MessageStoreUtils::ReplaceFileWithFileL(
        aStore.StoreUtils().FileSystem(), aStore.StoreUtils(), aFile, filePath );
    }

/**
 * 
 */
/*private*/
TContainerId CBufferedMessageCreator::GetNextContainerIdL(
    CContainerStore& aStore )
    {
    if ( iIds.Count() == 0 )
        {
        aStore.AllocateIdsBlockL( iIds, 1 );
        }
    
    TContainerId id = iIds[0];
    iIds.Remove( 0 );
    
    return id;
    }
    
/**
 * 
 */
/*private*/ void CBufferedMessageCreator::GetUncommittedContentPath(
    CContainerStore& aStore,
    TMsgStoreId aId,
    TFileName& aFilePath )
    {
    aFilePath = aStore.PrivatePath();
    aFilePath.Append( KUncommittedDirName );
    
    if ( KMsgStoreInvalidId != aId )
        {
        aFilePath.AppendNum( aId );
        }        
    }
    
/**
 * 
 */    
/*private*/ void CBufferedMessageCreator::ConstructL( CContainerStore& aStore )
    {
    TFileName filePath;
    GetUncommittedContentPath( aStore, KMsgStoreInvalidId, filePath );

    RFs& rfs = aStore.StoreUtils().FileSystem();
    TInt err = rfs.MkDirAll( filePath );
    
    if ( KErrAlreadyExists != err && KErrNone != err )
        {
        User::Leave( err );
        }
    }

/**
 * 
 */
/*private*/ CBufferedMessageCreator::CBufferedMessageCreator()
    {    
    }

/**
 * 
 */
/*public virtual*/
CBufferedMessageCreator::~CBufferedMessageCreator()
    {
    iContainers.ResetAndDestroy();
    iIds.Close();
    }

/**
 * 
 */
/*public*/ void CBufferedMessageCreator::ResetL( CContainerStore& aStore )
    {
    iInProgress = EFalse;
    iIsBatchInsert = EFalse;
    
    iContainers.ResetAndDestroy();
    iIds.Close();
    
    //then delete all of the uncommitted content files.
    TFileName filePath;
    GetUncommittedContentPath( aStore, KMsgStoreInvalidId, filePath );
    
    CDir* dirList;
    RFs& rfs = aStore.StoreUtils().FileSystem();
    
    User::LeaveIfError(
        rfs.GetDir( filePath,  KEntryAttMaskSupported, ESortByName, dirList ) );
    CleanupStack::PushL( dirList );

    TPath fullName;
    TInt count = dirList->Count();
    for ( TInt i = 0; i < count; i++ )
        {
        fullName = filePath;
        fullName.Append( (*dirList)[i].iName );
    
        //error code intentionally ignored, there's nothing to do with it.
        rfs.Delete( fullName );
        }    

    CleanupStack::PopAndDestroy( dirList );
    }

/**
 * 
 */
/*public*/ void CBufferedMessageCreator::AbandonMessageL(
    CContainerStore& aStore,
    TMsgStoreId aId )
    {
    if ( iIsBatchInsert )
        {
        //unless there is a good need to support this (not aware of one at the
        //moment) in case the batch insert mode is being used do not support
        //abandoning individual messages; while in batch insert mode abandoning
        //of all of the buffered messages can be achieved by starting a new
        //batch job (BeginBatchInsertL) and immediately finishing it
        //(FinishBatchInsertL).
        User::Leave( KErrNotSupported );
        }
    
    if ( iInProgress )
        {
        if ( iContainers.Count() )
            {
            //the message container is the first in the buffer.
            ContainerDescriptor& container = *iContainers[0];
            
            if ( aId == container.iParams.iId )
                {
                ResetL( aStore );
                }        
            }
        }
    }

/**
 * 
 */
/*public*/
void CBufferedMessageCreator::BeginBatchInsertL( CContainerStore& aStore )
    {
    ResetL( aStore );
    iIsBatchInsert = ETrue;    
    }

/**
 * 
 */
/*public*/
void CBufferedMessageCreator::FinishBatchInsertL(
    CContainerStore& aStore,
    MContainerStoreObserver* aObserver )
    {
    iInProgress = EFalse;
    iIsBatchInsert = EFalse;
    
    CommitMessagesL( aStore, aObserver );
    }


/**
 * 
 */
/*private*/
void CBufferedMessageCreator::CommitMessagesL(
    CContainerStore& aStore,
    MContainerStoreObserver* aObserver )
    {
    TRAPD( err, CommitMessagesImplL( aStore, aObserver ) );
    ResetL( aStore );
    User::LeaveIfError( err );
    }
    
/**
 * 
 */
/*private*/
void CBufferedMessageCreator::CommitMessagesImplL(
    CContainerStore& aStore,
    MContainerStoreObserver* aObserver )
    {
    aStore.BeginDatabaseTransactionLC();
   
    TInt msgIdx = -1;
    
    TInt count = iContainers.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        ContainerDescriptor& part = *iContainers[i];
        
        if ( EMsgStoreMessageBits
            == ( part.iParams.iType & EMsgStoreContainerMask ) )
            {
            //current part is a message.
        
            if ( -1 == msgIdx )
                {
                //starting now, just remember the message index for later.
                msgIdx = i;
                }
            else
                {
                //got to the next message, it is time to commit the current
                //one.
                ContainerDescriptor& part = *iContainers[msgIdx];
                aStore.CommitContainerL(
                    part.iParams.iId, part.iParams.iParentId,
                    part.iParams.iMailBoxId, aObserver );
            
                msgIdx = i;
                }
            }
            
        aStore.CreateContainerL(
            part.iParams.iType, 
            part.iParams.iParentId, 
            part.iParams.iGrandparentId, 
            part.iProperties,
            part.iParams.iId );

        //commit the message last so the event notifications are the same as
        //before the buffering (adding child parts to uncommitted message
        //does not trigger notifications).
        if ( msgIdx != i )
            {
            aStore.CommitContainerL(
                part.iParams.iId, part.iParams.iParentId,
                part.iParams.iMailBoxId, aObserver );
            }
        }

    //now commit the last message.
    if ( -1 != msgIdx )
        {
        ContainerDescriptor& part = *iContainers[msgIdx];
        aStore.CommitContainerL(
            part.iParams.iId, part.iParams.iParentId,
            part.iParams.iMailBoxId, aObserver );
        }
    
    //pops the rollback item.
    aStore.CommitDatabaseTransactionL();

    //now that the containers are committed the content files can be
    //"committed" too.
    RFs& rfs = aStore.StoreUtils().FileSystem();
    
    for ( TInt i = 0; i < count; i++ )
        {
        ContainerDescriptor& part = *iContainers[i];    
        
        TFileName filePath;
        GetUncommittedContentPath( aStore, part.iParams.iId, filePath );
        
        if ( BaflUtils::FileExists( rfs, filePath ) )
            {
            aStore.ContentManager().TransferContentFileL(
                part.iParams.iId, filePath );
            }
        }
    }


////////////////////////////////////////////////////////////
//                     PartDescriptor                     //                     
////////////////////////////////////////////////////////////

/**
 * 
 */
CBufferedMessageCreator::ContainerDescriptor::ContainerDescriptor(
    TMsgStoreCreateContainerCmdParams& aParams,
    const RBuf8& aProperties )
    : iParams( aParams )
    {
    iProperties.Assign( aProperties );
    }
    
/**
 * 
 */
CBufferedMessageCreator::ContainerDescriptor::~ContainerDescriptor()
    {
    iProperties.Close();
    }
