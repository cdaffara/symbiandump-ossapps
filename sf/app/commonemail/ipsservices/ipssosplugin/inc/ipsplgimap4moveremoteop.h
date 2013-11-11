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
* Description:  IMAP4 remote move message operation
*
*/

#ifndef IPSPLGIMAP4MOVEREMOTEOP_H
#define IPSPLGIMAP4MOVEREMOTEOP_H

#include "ipsplgonlineoperation.h"
// <qmail> mfsmailrequestobserver include removed

/**
* Move a selection of messages that may or may not be complete.
* Always fetches entire messages.
*/
NONSHARABLE_CLASS ( CIpsPlgImap4MoveRemoteOp ) :
    public CIpsPlgOnlineOperation
    {
public:
    /**
    * NewL
    * @param aMsvSession client/server session to MsvServer
    * @param aObserverRequestStatus client's status
    * @param aService serviceId of the mailbox
    * @param aActivityTimer mailbox specific activity timer
    * @param aDestinationFolder destination folder id to where the messages are to be moved
    * @param aSelection selection of messages
    * @param aFSMailBoxId identifies mailbox
    * @param aFSOperationObserver observer of this operation
    * @param aFSRequestId client assigned request identifier
    * @return class instance
    */
    // <qmail> parameters changed: aFunctionId removed, TImImap4GetMailInfo& -> TMsvId&
	// <qmail> MFSMailRequestObserver* changed to pointer
    static CIpsPlgImap4MoveRemoteOp* NewL(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aService,
        CIpsPlgTimerOperation& aActivityTimer,
        const TMsvId& aDestinationFolder,
        const CMsvEntrySelection& aSelection,
        TFSMailMsgId aFSMailBoxId,
        MFSMailRequestObserver* aFSOperationObserver,
        TInt aFSRequestId );

    virtual ~CIpsPlgImap4MoveRemoteOp();
    
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
    
    // <qmail> new func to this op
    /**
     * Returns operation type
     */
    TIpsOpType IpsOpType() const;
    
private:
    // <qmail> parameters changed
    /**
    * for explanation of parameters, see NewL
    */
    CIpsPlgImap4MoveRemoteOp(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aService,
        CIpsPlgTimerOperation& aActivityTimer,
        const TMsvId& aDestinationFolder,
        TFSMailMsgId aFSMailBoxId,
        MFSMailRequestObserver* aFSOperationObserver,
        TInt aFSRequestId );

    void ConstructL( const CMsvEntrySelection& aSelection );
    
    void RunL();
    
    /**
     * actual implementation of RunL's functionality
     * RunL wraps and traps this function
     */
    void DoRunL();
    
    void DoConnectL();
    
    /**
    * Sort messages into complete and incomplete.
    */
    void SortMessageSelectionL(const CMsvEntrySelection& aSel);
    
    /**
     * Completes client's status
     */
    void Complete();

    /**
     * handles remote moving
     */
    void DoMoveRemoteL();
    
    // <qmail> removed TInt GetEngineProgress( const TDesC8& aProgress );

private:
    // internal state of this operation
    enum TState 
        { 
        EIdle, 
        EConnecting, 
        ELocalMsgs, 
        ERemoteMsgs 
        };
    TState                              iState;

    // <qmail> iFunctionId removed, TImImap4GetMailInfo -> TMsvId
    // used in error situations
    TDesC8*                             iMoveErrorProgress;
    // <qmail> using destination folder Id instead of mailInfo struct
    // specifies folder where to move
    TMsvId                              iDestinationFolderId;
    // <qmail> removed iSelection;
    CMsvEntrySelection*                 iRemoteSel;     // Incomplete messages to be fetched.
    TPckgBuf<TImap4CompoundProgress>    iProgressBuf;
    TPckgBuf<TImap4SyncProgress>        iSyncProgress;
    };


NONSHARABLE_CLASS( CIpsPlgImap4MoveRemoteOpObserver ) : public CBase,
    public MFSMailRequestObserver
    {
public:

    static CIpsPlgImap4MoveRemoteOpObserver* NewL( CMsvSession& aSession,
        CIpsPlgEventHandler& aEventHandler, const TFSMailMsgId& aSourceFolder,
        const RArray<TFSMailMsgId>& aMessageIds );
    ~CIpsPlgImap4MoveRemoteOpObserver();

    // From base class MFSMailRequestObserver
    void RequestResponseL( TFSProgress aEvent, TInt aRequestId );

private:

    CIpsPlgImap4MoveRemoteOpObserver( CMsvSession& aSession,
        CIpsPlgEventHandler& aEventHandler,
        TMsvId aSourceFolderId );
    void ConstructL( const RArray<TFSMailMsgId>& aMessageIds );

private:

    // data
    CMsvSession& iSession;
    CIpsPlgEventHandler& iEventHandler;
    TMsvId iSourceFolderId;
    CMsvEntrySelection* iSelection;
    };

#endif // IPSPLGIMAP4MOVEREMOTEOP_H

// End of File
