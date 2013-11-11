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
* Description: Message creator definition
*
*/
#ifndef BUFFEREDMESSAGECREATOR_H
#define BUFFEREDMESSAGECREATOR_H

#include <bautils.h>
#include "MessageStoreClientServer.h"
#include "ContainerStore.h"


class CContainerStoreContentManager;


/**
 * In this subdirectory uncommitted content files are being stored.
 */
_LIT( KUncommittedDirName, "uncommitted\\" );


/**
 * This class buffers all the container requests made during the creation of a
 * message store message. This usually involves the message container,
 * few parts (probably three for the message body plus one for each attachment)
 * and the parts' content files. When the message container is committed then
 * all of the writes to the message store are done in a single transactions.
 * 
 * Container ids are allocated before and during the buffering and if for some
 * reason the message is not committed the ids will be lost.
 * 
 * Public APIs often require a CContainerStore instance as this instance may get
 * deleted in certain cases so it cannot be cached internally.
 * 
 * Buffering is not supported when encryption is enabled. It will be
 * automatically turned off if the encryption is on. 
 */
NONSHARABLE_CLASS( CBufferedMessageCreator ) : public CBase
    {
    
    /**
     * Collects all of the information about a container.
     */
    class ContainerDescriptor
        {
    public:
        
        /**
         * @param aProperties - the ownership is transferred ("Assign"-ed) to
         * the current instance thus the caller should not "Close" the RBuf. 
         */
        ContainerDescriptor(
            TMsgStoreCreateContainerCmdParams& aParams,
            const RBuf8& aProperties );
        
        ~ContainerDescriptor();
        
    public:
        RBuf8 iProperties;
        TMsgStoreCreateContainerCmdParams iParams;
        };


public:

    static CBufferedMessageCreator* CBufferedMessageCreator::NewL(
        CContainerStore& aStore );
    
    virtual ~CBufferedMessageCreator();

    /**
     * Tries to buffer this request to create a container. A request to create
     * a message container starts the buffering and all of the related
     * containers will be buffered until the message container is committed. If
     * another message container is created while a different one is in progress
     * then the one in progress will be aborted. There is no technical reason
     * for that other than simplification.
     * 
     * @return ETrue if the request was buffered and thus the direct request
     * handling must be skipped.
     */
    TBool EnqueueL(
        CContainerStore& aStore,
        TMsgStoreCreateContainerCmdParams& aParams,
        const RBuf8& aProperties );    

    /**
     * Determines whether the specified id belongs to a container that has been
     * buffered.
     */
    TBool IsContainerBuffered( TMsgStoreId aId ) const;
    
    void CommitMessageL(
        CContainerStore& aStore,
        MContainerStoreObserver* aObserver );    
    
    void AbandonMessageL(
        CContainerStore& aStore,
        TMsgStoreId aId );

    void AppendContentL(
        CContainerStore& aStore,
        TMsgStoreId aId,
        RBuf8& aContentBuf );

    void PrependContentL(
        CContainerStore& aStore,
        TMsgStoreId aId,
        RBuf8& aContentBuf );
    
    void ReplaceContentL(
        CContainerStore& aStore,
        TMsgStoreId aId,
        RBuf8& aContentBuf );
    
    void ReplaceContentL(
        CContainerStore& aStore,
        TMsgStoreId aId,
        RFile& aFile );

    /**
     * Empties the current contents, cleans up content files and resets the
     * internal state so a new message creation can be buffered.
     */
    void ResetL( CContainerStore& aStore );
    
    void BeginBatchInsertL( CContainerStore& aStore );
    
    void FinishBatchInsertL(
        CContainerStore& aStore,
        MContainerStoreObserver* aObserver );
    
private:
    
    void CommitMessagesL(
        CContainerStore& aStore,
        MContainerStoreObserver* aObserver );    

    void CommitMessagesImplL(
        CContainerStore& aStore,
        MContainerStoreObserver* aObserver );    

    void AddRequestL(
        CContainerStore& aStore,
        TMsgStoreCreateContainerCmdParams& aParams,
        const RBuf8& aProperties );    

    TContainerId GetNextContainerIdL( CContainerStore& aStore );
    
    void GetUncommittedContentPath(
        CContainerStore& aStore,
        TMsgStoreId aId,
        TFileName& aFilePath );
    
    void ConstructL(
        CContainerStore& aStore );
    
    CBufferedMessageCreator();
    
    //disallow these.
    CBufferedMessageCreator( const CBufferedMessageCreator& );
    CBufferedMessageCreator& operator= ( const CBufferedMessageCreator& );        
    
private:
    TBool iInProgress;
    RPointerArray<CBufferedMessageCreator::ContainerDescriptor> iContainers;
    RArray<TContainerId> iIds;
    TBool iIsBatchInsert;
    };

	
#endif // BUFFEREDMESSAGECREATOR_H
