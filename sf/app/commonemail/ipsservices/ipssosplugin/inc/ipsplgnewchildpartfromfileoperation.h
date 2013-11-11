/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  New child part from file operation
*
*/

// <qmail>
#ifndef IPSPLGNEWCHILDPARTFROMFILEOPERATION_H
#define IPSPLGNEWCHILDPARTFROMFILEOPERATION_H

class MFSMailRequestObserver;

NONSHARABLE_CLASS ( CIpsPlgNewChildPartFromFileOperation ): public CIpsPlgBaseOperation
{
public:
    /**
    * Constructor.
    */
    static CIpsPlgNewChildPartFromFileOperation* NewL(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aMessageId,
        const TDesC& aContentType,
        const TDesC& aFilePath,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId,
        CIpsPlgMsgMapper *aMsgMapper);

    /**
    * Destructor.
    */
    virtual ~CIpsPlgNewChildPartFromFileOperation();

public: // From CIpsPlgBaseOperation
    
    virtual const TDesC8& ProgressL();
    
    virtual const TDesC8& GetErrorProgressL(TInt aError);

    virtual TFSProgress GetFSProgressL() const;

    TIpsOpType IpsOpType() const;   
    
protected:
    /**
    * Constructor.
    */
    CIpsPlgNewChildPartFromFileOperation(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        const TFSMailMsgId& aMailBoxId,
        const TFSMailMsgId& aMessageId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId);
    
    /**
    * Constructor.
    */
    void ConstructL(CIpsPlgMsgMapper *aMsgMapper,
        const TDesC& aContentType,
        const TDesC& aFilePath);

    /**
    * From CActive
    */
    void RunL();

    /**
    * From CActive
    */
    void DoCancel();

    /**
    * From CActive
    */
    TInt RunError(TInt aError);
    
    void SignalFSObserver(TInt aStatus, CFSMailMessagePart* aMessagePart );

private:
    void GetMessageEntryL( TMsvId aId,
                           CMsvEntry*& aMessageEntry,
                           CImEmailMessage*& aImEmailMessage );
    
    void CleanCachedMessageEntries();

    /*
     * Four different operation steps are needed
     */
    enum TOperationStep
        {
        EInitAttachmentManager,
        EPrepareMsvEntry,
        EPrepareStore,
        EStoreMessagePart
        };
    
    /*
     * Initializes attachment manager asynchronously
     */
    void InitAttachmentManagerL();
    
    /*
     * Prepares Msv entry to be updated asynchronously
     */
    void PrepareMsvEntryL();
    
    /*
     * Prepares message store asynchronously if needed
     */
    void PrepareStoreL();
    
    /*
     * Stores message part and finishes the operation
     */
    void StoreMessagePartL();
    
protected:
    CMsvOperation* iOperation;   // owned
	// <qmail> iBlank, iMailBoxId removed
    TFSMailMsgId iMessageId;
    MFSMailRequestObserver& iOperationObserver;  // not owned
	// <qmail> iRequestId removed
    HBufC* iContentType;
    HBufC* iFilePath;
    CMsvEntry* iCachedEntry;
    CImEmailMessage* iCachedEmailMessage;
    CImEmailMessage* iMessage;
    CMsvEntry* iEntry;
    CIpsPlgMsgMapper* iMsgMapper;
    TMsvId iNewAttachmentId;
    TOperationStep iStep;
    TFSProgress iFSProgress;
};

#endif // IPSPLGNEWCHILDPARTFROMFILEOPERATION_H

// </qmail>
