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
 * Description: Class responsible for storing message part content asynchronously.
 * It can store text/plain and text/html body parts.
 *
 */

#include <e32std.h>
#include <e32base.h>
#include <msvapi.h>

#ifndef IPSPLGMESSAGEPARTSTOREOPERATION_H_
#define IPSPLGMESSAGEPARTSTOREOPERATION_H_

class CFSMailPlugin;
class CFSMailMessage;
class CFSMailMessagePart;
class MFSMailRequestObserver;

NONSHARABLE_CLASS( CIpsPlgMessagePartStorerOperation ) : public CIpsPlgBaseOperation
    {
public:
    /**
     * Symbian 2nd phase constructor
     * @param aMsvSession
     * @param aObserverRequestStatus
     * @param aPlugin
     * @param aMessageParts - ownership is taken from caller
     * @param aRequestId
     * @return CIpsPlgMessagePartStorerOperation*
     */
    static CIpsPlgMessagePartStorerOperation* NewL(CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus, CFSMailPlugin& aPlugin,
            RPointerArray<CFSMailMessagePart> &aMessageParts,
            MFSMailRequestObserver& aFSOperationObserver,
            const TInt aRequestId);

    /**
     * Symbian 2nd phase constructor
     * @param aMsvSession
     * @param aObserverRequestStatus
     * @param aPlugin
     * @param aMessageParts - ownership is taken from caller
     * @param aRequestId
     * @return CIpsPlgMessagePartStorerOperation*
     */
    static CIpsPlgMessagePartStorerOperation* NewLC(CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus, CFSMailPlugin& aPlugin,
            RPointerArray<CFSMailMessagePart> &aMessageParts,
            MFSMailRequestObserver& aFSOperationObserver,
            const TInt aRequestId);

    /**
     * Class destructor
     */
    virtual ~CIpsPlgMessagePartStorerOperation();

    /**
     * Implements cancellation of an outstanding request
     */
    void DoCancel();

    /**
    * From CMsvoperation
    */
    virtual const TDesC8& ProgressL();

    // <qmail>
    /**
    * From CIpsPlgBaseOperation
    */
    virtual const TDesC8& GetErrorProgressL(TInt aError);

    /**
    * From CIpsPlgBaseOperation
    */
    virtual TFSProgress GetFSProgressL() const;

    /**
     * From CIpsPlgBaseOperation
     * Returns operation type
     */
    TIpsOpType IpsOpType() const;    
    // </qmail>

protected:

    /**
     * Default constructor
     * @param aMsvSession
     * @param aObserverRequestStatus
     * @param aPlugin
     * @param aMessageParts - ownership is taken from caller
     * @param aRequestId
     */
    CIpsPlgMessagePartStorerOperation(CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus, CFSMailPlugin& aPlugin,
            RPointerArray<CFSMailMessagePart> &aMessageParts,
            MFSMailRequestObserver& aFSOperationObserver,
            const TInt aRequestId);

private:

    /**
     * Initialization and construction of all leaving members
     */
    void ConstructL();

    /**
     * Handles an active object's request completion event
     */
    void RunL();

    /**
     * Handles a leave occurring in the request completion event handler
     */
    TInt RunError(TInt aError);
    
    /**
	 * Stores next part content in message store
	 * @return returns EFalse if there are none message parts
	 */
    TBool StoreNextPartL();
    
    /**
	 * Stores text plain content in message store
	 * @param CFSMailMessagePart* aPart to be stored
	 */
    void StoreTextPlainPartL(CFSMailMessagePart* aPart);
    
    /**
     * Stores text html content in message store
     * @param CFSMailMessagePart* aPart to be stored
     */
    void StoreTextHtmlPartL(CFSMailMessagePart* aPart);

private:
    //data
    CFSMailPlugin& iPlugin;
    
    // Array of message parts to be stored
    RPointerArray<CFSMailMessagePart> iMessageParts;

    MFSMailRequestObserver& iFSOperationObserver;
    
    // Execution index stores current progres
    TInt iExecutionIndex;
    
    // Data buffer for async operations
    HBufC8* iDataBuffer;

    // <qmail>
    TFSProgress iFSProgress;
    // </qmail>
    };

#endif /* IPSPLGMESSAGEPARTSTOREOPERATION_H_ */
