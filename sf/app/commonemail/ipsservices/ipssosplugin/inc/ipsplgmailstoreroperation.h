/*
* Copyright (c) 2007 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: This file defines classes CIpsPlgSearch, TSearchParams, TMsgContainer
*
*/
#include <e32std.h>
#include <e32base.h>
#include <msvapi.h>

#ifndef CIPSPLGMAILSTOREROPERATION_H
#define CIPSPLGMAILSTOREROPERATION_H

class CFSMailPlugin;
class CFSMailMessage;
class MFSMailRequestObserver;

NONSHARABLE_CLASS( CIpsPlgMailStorerOperation ) :
    public CIpsPlgBaseOperation
    {
public:
    /**
     * Symbian 2nd phase construcror
     *
     * @return None
     */
    static CIpsPlgMailStorerOperation* NewL(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        CFSMailPlugin& aPlugin,
        RPointerArray<CFSMailMessage> &messages,
        MFSMailRequestObserver& aFSOperationObserver,
        const TInt aRequestId);

    /**
     * Symbian 2nd phase construcror
     *
     * @return None
     */
    static CIpsPlgMailStorerOperation* NewLC(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        CFSMailPlugin& aPlugin,
        RPointerArray<CFSMailMessage> &messages,
        MFSMailRequestObserver& aFSOperationObserver,
        const TInt aRequestId );

    /**
     * Class destructor
     *
     * @return
     */
    virtual ~CIpsPlgMailStorerOperation();

    /**
     * Cancels operation
     *
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
     *
     *
     */
    CIpsPlgMailStorerOperation(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        CFSMailPlugin& aPlugin,
        RPointerArray<CFSMailMessage> &aMessages,
        MFSMailRequestObserver& aFSOperationObserver,
        const TInt aRequestId );

private:

    /**
     *
     */
    void ConstructL();

    /**
     *
     */
    void RunL();

    /**
     *
     */
    TInt RunError();

private://data
    TInt                        iError;
    TInt                        iExecutionIndex;
    CFSMailPlugin&              iPlugin;
    RPointerArray<CFSMailMessage>     &iMessages;
    MFSMailRequestObserver&     iFSOperationObserver;
    // <qmail>
    TFSProgress                 iFSProgress;
    // </qmail>
    };

#endif //CIpsPlgMailStorerOperation_H
