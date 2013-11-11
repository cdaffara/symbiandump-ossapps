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
* Description:  Remove child part operation
*
*/

// <qmail>
#ifndef IPSPLGREMOVECHILDPARTOPERATION_H
#define IPSPLGREMOVECHILDPARTOPERATION_H

class MFSMailRequestObserver;

NONSHARABLE_CLASS ( CIpsPlgRemoveChildPartOperation ): public CIpsPlgBaseOperation
{
public:
    /**
    * Constructor.
    */
    static CIpsPlgRemoveChildPartOperation* NewL(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aPartId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId);

    /**
    * Destructor.
    */
    virtual ~CIpsPlgRemoveChildPartOperation();

public: // From CIpsPlgBaseOperation
    
    virtual const TDesC8& ProgressL();
    
    virtual const TDesC8& GetErrorProgressL(TInt aError);

    virtual TFSProgress GetFSProgressL() const;

    TIpsOpType IpsOpType() const;
    
protected:
    /**
    * Constructor.
    */
    CIpsPlgRemoveChildPartOperation(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        const TFSMailMsgId& aMessageId,
        const TFSMailMsgId& aPartId,
        MFSMailRequestObserver& aOperationObserver,
        const TInt aRequestId);
    
    /**
    * Constructor.
    */
    void ConstructL();

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
    
    void StartOperationL();
    
protected:
    TFSMailMsgId iMessageId;
    TFSMailMsgId iPartId;
    MFSMailRequestObserver& iOperationObserver;  // not owned
    CImEmailMessage* iMessage;
    CMsvEntry* iCachedEntry;
    CImEmailMessage* iCachedEmailMessage;
    TFSProgress iFSProgress;
};

#endif // IPSPLGREMOVECHILDPARTOPERATION_H

// </qmail>

