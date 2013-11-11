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
* Description:  Disconnection operation class
*
*/

#ifndef IPSPLGDISCONNECTOP_H
#define IPSPLGDISCONNECTOP_H

#include "ipsplgonlineoperation.h"

/**
* Disconnect operation
* Handles both POP and IMAP cases
*/
NONSHARABLE_CLASS ( CIpsPlgDisconnectOp ) :
    public CIpsPlgOnlineOperation
    {
public:
    /**
    * Construction
    * @param aMsvSession client/server session to MsvServer
    * @param aObserverRequestStatus caller's status
    * @param aService serviceId of the mailbox
    * @param aActivityTimer mailbox specific activity timer
    * @param aFSMailBoxId specifies mailbox
    * @param aFSOperationObserver observer of the operation
    * @param aFSRequestId identifier for this request instance
    * @return instance of this operation class
    */
    // <qmail> aDoRemoveAfterDisconnect parameter removed
	// <qmail> MFSMailRequestObserver& changed to pointer
    static CIpsPlgDisconnectOp* NewL(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aService,
        CIpsPlgTimerOperation& aActivityTimer,
        TFSMailMsgId aFSMailBoxId,
        MFSMailRequestObserver* aFSOperationObserver,
        TInt aFSRequestId );

    virtual ~CIpsPlgDisconnectOp();

    /**
    * From MsvOperation
    * Gets information on the progress of the operation
    * (see MsvOperation header)
    */
    const TDesC8& ProgressL();

    /**
    * From CIpsPlgBaseOperation
    * For reporting if DoRunL leaves
    */
    const TDesC8& GetErrorProgressL( TInt aError );
    
   TFSProgress GetFSProgressL() const;

   // <qmail> Connected() used from baseclass
   
   // <qmail> new function in this op
   TIpsOpType IpsOpType() const;
   // </qmail>
   
protected: // From CActive
    void DoRunL();
    
private:

	// <qmail> aDoRemoveAfterDisconnect parameter removed
	// <qmail> MFSMailRequestObserver& changed to pointer
    CIpsPlgDisconnectOp(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aServiceId,
        CIpsPlgTimerOperation& aActivityTimer,
        TFSMailMsgId aFSMailBoxId,
        MFSMailRequestObserver* aFSOperationObserver,
        TInt aFSRequestId );

    void ConstructL();

    /**
    * Sends the disconnect command to MsvServer
    */
    void DoDisconnectL();

private: // Data
    TPckgBuf<TPop3Progress>          iPopProgress;
    TPckgBuf<TImap4CompoundProgress> iImapProgress;
    TMsvEntry                        iTEntry;
    // <qmail> iDisconnected; removed; using baseclass's Connected() instead
    // <qmail> iDoRemoveAfterDisconnect removed; not a task for disconnect op
    };

#endif //IPSPLGDISCONNECTOP_H
