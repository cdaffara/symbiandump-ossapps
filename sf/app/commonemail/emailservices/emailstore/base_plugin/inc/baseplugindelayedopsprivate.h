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
* Description: Support for executing asynchronously certain plugin
* operations.
*/
#ifndef BASEPLUGINDELAYEDOPSPRIVATE_H
#define BASEPLUGINDELAYEDOPSPRIVATE_H


#include "baseplugindelayedops.h"

/**
 * Lets the user enqueue and dequeue asynchronous jobs for handling
 * plugin-related actions. 
 */
NONSHARABLE_CLASS ( CDelayedOpsManager ) :
    public CBase,
    public MDelayedOpsManager
    {

public:

    static CDelayedOpsManager* NewL( CBasePlugin& aPlugin );
    
    virtual ~CDelayedOpsManager();
    
    // MDelayedOpsManager::EnqueueOpL
    virtual void EnqueueOpL( CDelayedOp* aOp );
    
    // MDelayedOpsManager::DequeueOpL
    virtual void DequeueOp( const CDelayedOp& aOp );
    
    // MDelayedOpsManager::Extension1
    virtual TInt Extension1(
        TUint /*aExtensionId*/, TAny*& /*a0*/, TAny* /*a1*/ );


private:
    
    CDelayedOpsManager( CBasePlugin& aPlugin );

    void ConstructL();
    
    void ExecutePendingOps();
    
    CDelayedOpsManager( const CDelayedOpsManager& );
    CDelayedOpsManager& operator= ( const CDelayedOpsManager& );    
    

private:
    CBasePlugin& iPlugin;
    RPointerArray<CDelayedOp> iDelayedOps;
    
    __LOG_DECLARATION
    };


/**
 * 
 */
NONSHARABLE_CLASS( CDelayedDeleteMessagesOp ) : public CDelayedOp
    {

public:

    static CDelayedDeleteMessagesOp* NewLC(
        TMsgStoreId aMailBoxId,
        TMsgStoreId aFolderId,
        const RArray<TFSMailMsgId>& aMessages );
    
    static CDelayedDeleteMessagesOp* NewLC(
        TMsgStoreId aMailBoxId,
        TMsgStoreId aFolderId,
        TMsgStoreId aMsgId );

    virtual ~CDelayedDeleteMessagesOp();

    //CDelayedOp::ExecuteOpL
    virtual TBool ExecuteOpL();

private:

    void ConstructL( const RArray<TFSMailMsgId>& aMessages );
    void ConstructL( TMsgStoreId aMsgId );
    
    CDelayedDeleteMessagesOp(
        TMsgStoreId aMailBoxId,
        TMsgStoreId aFolderId );
    
    CDelayedDeleteMessagesOp( const CDelayedDeleteMessagesOp& );
    CDelayedDeleteMessagesOp& operator= ( const CDelayedDeleteMessagesOp& );    
    
private:
    
    TMsgStoreId iMailBoxId;
    TMsgStoreId iFolderId;
    RArray<TMsgStoreId> iMessages;
    TBool iImmediateDelete;
    TInt iIndex;
    __LOG_DECLARATION
    };


/**
 * This most likely needs "large-content" mode where the content buffer is not
 * copied in memory but stored on a fast drive and then do a
 * ReplaceContentWithFileL.
 */
NONSHARABLE_CLASS( CDelayedSetContentOp ) : public CDelayedOp
    {

public:

    static CDelayedSetContentOp* NewLC(
       TMsgStoreId aMailBoxId,
       TMsgStoreId aMessageId,
       TMsgStoreId aMessagePartId,
       const TDesC& aContent );

    static CDelayedSetContentOp* NewLC(
       TMsgStoreId aMailBoxId,
       TMsgStoreId aMessageId,
       TMsgStoreId aMessagePartId,
       TInt aContentLength );

    virtual ~CDelayedSetContentOp();

    //CDelayedOp::ExecuteOpL
    virtual TBool ExecuteOpL();
    
private:

    void ConstructL( const TDesC& aContent );
    void ConstructL();

    CDelayedSetContentOp(
        TMsgStoreId aMailBoxId,
        TMsgStoreId aMessageId,
        TMsgStoreId aMessagePartId );

    CDelayedSetContentOp(
        TMsgStoreId aMailBoxId,
        TMsgStoreId aMessageId,
        TMsgStoreId aMessagePartId,
        TInt aContentLength );
    
    CMsgStoreMessagePart* FetchMessagePartLC();    
    
    CDelayedSetContentOp( const CDelayedSetContentOp& );
    CDelayedSetContentOp& operator= ( const CDelayedSetContentOp& );    
    
private:
    
    TMsgStoreId iMailBoxId;
    TMsgStoreId iMessageId;
    TMsgStoreId iMessagePartId;
    HBufC* iContent;
    TInt iContentLength;
    TBool iStepOne;
    
    __LOG_DECLARATION
    };

//<qmail>
/**
 * 
 */
NONSHARABLE_CLASS( CDelayedMessageStorerOp ) : public CDelayedOp
    {

public:

    static CDelayedMessageStorerOp* NewLC(
            const TFSMailMsgId& aMailBox,
            RPointerArray<CFSMailMessage> &messages,
            MFSMailRequestObserver& aOperationObserver,
            const TInt aRequestId );
    
    static CDelayedMessageStorerOp* NewLC(
            RPointerArray<CFSMailMessagePart>& aMessageParts,
            MFSMailRequestObserver& aOperationObserver,
            const TInt aRequestId);

    virtual ~CDelayedMessageStorerOp();

    //CDelayedOp::ExecuteOpL
    virtual TBool ExecuteOpL();
    
private:

    void ConstructL();
       
    CDelayedMessageStorerOp(
            const TFSMailMsgId& aMailBox,
            RPointerArray<CFSMailMessage> &messages,
            MFSMailRequestObserver& aOperationObserver,
            const TInt aRequestId);
    
    CDelayedMessageStorerOp(
            RPointerArray<CFSMailMessagePart>& aMessageParts,
            MFSMailRequestObserver& aOperationObserver,
            const TInt aRequestId);
    
    CDelayedDeleteMessagesOp& operator= ( const CDelayedDeleteMessagesOp& );    
    
    void StorePartL(CFSMailMessagePart* aPart);
    
private:
    //enums
    enum TActionType 
        {
        EHeaders = 0,
        EParts
        };
    TFSMailMsgId iMailBox;
    RPointerArray<CFSMailMessage> iMessages;
    RPointerArray<CFSMailMessagePart> iMessageParts;
    TInt iType;
    // Data buffer for async operations
    HBufC8* iDataBuffer;
    
    __LOG_DECLARATION
    };
/**
 * 
 */
NONSHARABLE_CLASS( CDelayedMessageToSendOp ) : public CDelayedOp
    {

public:

    static CDelayedMessageToSendOp* NewLC(
            CBasePlugin& aPlugin,
            const TFSMailMsgId& aMailBox,
            MFSMailRequestObserver& aOperationObserver,
            const TInt aRequestId );
    
    virtual ~CDelayedMessageToSendOp();

    //CDelayedOp::ExecuteOpL
    virtual TBool ExecuteOpL();
    
private:

    void ConstructL();
       
    CDelayedMessageToSendOp(
            CBasePlugin& aPlugin,
            const TFSMailMsgId& aMailBox,
            MFSMailRequestObserver& aOperationObserver,
            const TInt aRequestId);
    
    CDelayedMessageToSendOp& operator= ( const CDelayedMessageToSendOp& );    
    
private:
    CBasePlugin& iBasePlugin;
    TFSMailMsgId iMailBox;

    __LOG_DECLARATION
    };
/**
 * 
 */
NONSHARABLE_CLASS( CDelayedAddNewOrRemoveChildPartOp ) : public CDelayedOp
    {

public:

    static CDelayedAddNewOrRemoveChildPartOp* NewLC( const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aParentFolderId ,
            const TFSMailMsgId& aMessageId,
            const TFSMailMsgId& aParentPartId ,
            const TDesC& aContentType,
            const TDesC& aFilePath,
            MFSMailRequestObserver& aOperationObserver,
            const TInt aRequestId);
    
    static CDelayedAddNewOrRemoveChildPartOp* NewLC( const TFSMailMsgId& aMailBoxId,
               const TFSMailMsgId& aParentFolderId ,
               const TFSMailMsgId& aMessageId,
               const TFSMailMsgId& aParentPartId ,
               const TFSMailMsgId& aPartId,
               MFSMailRequestObserver& aOperationObserver,
               const TInt aRequestId);

    virtual ~CDelayedAddNewOrRemoveChildPartOp();

    //CDelayedOp::ExecuteOpL
    virtual TBool ExecuteOpL();
    
private:

    void ConstructL( const TDesC& aContentType, const TDesC& aFilePath );
       
    CDelayedAddNewOrRemoveChildPartOp( const TFSMailMsgId& aMailBoxId,
            const TFSMailMsgId& aParentFolderId ,
            const TFSMailMsgId& aMessageId,
            const TFSMailMsgId& aParentPartId ,
            MFSMailRequestObserver& aOperationObserver,
            const TInt aRequestId);
    
    CDelayedAddNewOrRemoveChildPartOp( const TFSMailMsgId& aMailBoxId,
                const TFSMailMsgId& aParentFolderId ,
                const TFSMailMsgId& aMessageId,
                const TFSMailMsgId& aParentPartId ,
                const TFSMailMsgId& aPartId ,
                MFSMailRequestObserver& aOperationObserver,
                const TInt aRequestId);
        
    CDelayedAddNewOrRemoveChildPartOp& operator= ( const CDelayedAddNewOrRemoveChildPartOp& );    
    
private:
    enum TActionType 
          {
          AddNewChild = 0,
          RemoveChild
          };
    TFSMailMsgId iMailBoxId;
    TFSMailMsgId iParentFolderId;
    TFSMailMsgId iMessageId;
    TFSMailMsgId iParentPartId;
    TFSMailMsgId iPartId;
    HBufC* iContentType;
    HBufC* iFilePath;
    TInt iActionType;
    __LOG_DECLARATION
    };
//</qmail>

#endif // BASEPLUGINDELAYEDOPSPRIVATE_H
