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
* Description:  IMAP4 connect operation
*
*/

#ifndef IPSPLGIMAP4CONNECTOP_H
#define IPSPLGIMAP4CONNECTOP_H

#include "ipsplgonlineoperation.h"
#include "imapconnectionobserver.h"

class CIpsPlgEventHandler;

// <qmail> KIpsAutoPopulateByteLimit not used

/**
* class CIpsPlgImap4ConnectionOp
* 
*/
NONSHARABLE_CLASS ( CIpsPlgImap4ConnectOp ) :
    public CIpsPlgOnlineOperation,
    // <qmail> public MMsvImapConnectionObserver, not used any more
    // <qmail> public MIpsPlgConnectOpCallback not used any more
    public MFSMailRequestObserver
    {
// <qmail> MIpsPlgConnectOpCallback not used any more
public:
            
    /**
    * NewL()
    * @param aMsvSession reference to client/server session to MsvServer
    * @param aObserverRequestStatus client's status
    * @param aService service (mailbox) id
    * @param aActivityTimer mailbox specific activity timer
    * @param aFSMailBoxId specifies mailbox
    * @param aFSOperationObserver observer of this operation
    * @param aFSRequestId request identifier of this op (assigned by caller, and used when calling 
    * back aFSOperationObserver
    * @param aEventHandler used for signalling synchronisation statuses
    * @param aDoPlainConnect whether just connect to mailbox, or synch messages also
    * @return CIpsPlgImap4ConnectionOp* self pointer
    */
    // <qmail> priority parameter has been removed
	// <qmail> MFSMailRequestObserver& changed it to pointer
	// <qmail> aSignallingAllowed parameter has been removed
    static CIpsPlgImap4ConnectOp* NewL(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aService,
        CIpsPlgTimerOperation& aActivityTimer,
        TFSMailMsgId aFSMailBoxId,
        MFSMailRequestObserver* aFSOperationObserver,
        TInt aFSRequestId,
        CIpsPlgEventHandler* aEventHandler,
        TBool aDoPlainConnect=EFalse );
        
    // <qmail> make destructor virtual
    virtual ~CIpsPlgImap4ConnectOp();

public: // from CIpsPlgBaseOperation

    /**
    * From MsvOperation
    * Gets information on the progress of the operation
    * (see MsvOperation header)
    */
    virtual const TDesC8& ProgressL();

    /**
    * From CIpsPlgBaseOperation
    * For reporting if DoRunL leaves
    */
    virtual const TDesC8& GetErrorProgressL( TInt aError );
    
    virtual TFSProgress GetFSProgressL() const;
    
    // <qmail> change ret val type
    /**
     * Returns operation type
     */
    TIpsOpType IpsOpType() const;
    
    //<qmail> moved from private to public and changed to static
    /**
     * 
     * @param aInfo is filled correctly in this function
     * @param aImap4Settings info is filled partly based on settings
     */
    static void ConstructImapPartialFetchInfo(
        TImImap4GetPartialMailInfo& aInfo, 
        const CImImap4Settings& aImap4Settings );
    //</qmail>
        
    // <qmail> HandleImapConnectionEvent() not used any more

public: // from MFSMailRequestObserver
    
    void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
        
// <qmail> Connected() used from baseclass

protected: // From CActive
    void DoRunL();
    void DoCancel();
    
private:
    enum TImapConnectionState 
        { 
        EStateStartConnect,
        // <qmail> removing EStateQueryingDetails, EStateQueryingDetailsBusy
        EStateConnectAndSync,
        EStatePopulateAllCompleted,
        EStateCompleted,
        EStateIdle 
        };
    
	// <qmail> priority parameter has been removed
	// <qmail> MFSMailRequestObserver& changed it to pointer
	// <qmail> aSignallingAllowed parameter has been removed
    CIpsPlgImap4ConnectOp(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aService,
        CIpsPlgTimerOperation& aActivityTimer,
        TFSMailMsgId aFSMailBoxId,
        MFSMailRequestObserver* aFSOperationObserver,
        TInt aFSRequestId,
        TBool aDoPlainConnect,
        CIpsPlgEventHandler* aEventHandler );

    void ConstructL();
    
    /**
    * Handles one internal state 
    */        
    void DoConnectOpL();
    
    // <qmail> remove void StartL();
    
    /**
    * Handles one internal state 
    */       
    void DoPopulateAllL();
    
    // <qmail> removed QueryUserPwdL();
    
    /**
     * initiates sync state changed event
     */
    void SignalSyncStarted();
    
    /**
     * initiates sync state changed event
     * @param aError specifies completion status
     */
    void SignalSyncCompleted( TInt aError );
    
    // <qmail> moved to public

    /**
     * GetImapSettingsLC()
     * constructs imap settings object with this account's settings (based on iService member)
     * @return CImImap4Settings object as a pointer. Object is placed into CleanupStack.
     */
    CImImap4Settings* GetImapSettingsLC();
    
    /**
     * Updates iSelection member to contain the message entries from inbox folder
     * NOTE: all prior contents of iSelection will be deleted
     * NOTE: takes care of only direct Inbox folder
     */
    void CreateInboxMessagesSelectionL();
    
    // </qmail>
    
private: //Data
    // internal state of this operation
    TImapConnectionState                iState;
    TPckgBuf<TImap4CompoundProgress>    iProgressBuf;
    CMsvEntrySelection* 				iSelection;
    // whether caller instantiated this op to do only connect or not
    TBool                               iDoPlainConnect;
    // pointer to event handler in order to signal sync status to client
    CIpsPlgEventHandler*                iEventHandler; // not owned
    // prevents signalling sync started for more than once
    TBool                               iIsSyncStartedSignaled;
	
	// <qmail> iAlreadyConnected removed
    };

#endif // IPSPLGIMAP4CONNECT_H
