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
* Description:  IMAP4 attachment fetch operation
*
*/

#ifndef IPSPLGIMAP4FETCHATTACHMENTOP_H
#define IPSPLGIMAP4FETCHATTACHMENTOP_H

class CIpsPlgImap4FetchAttachmentOp;

/**
* CIpsFetchProgReport
* helper class for the operation
* implements a timer which initiates periodical progress updating
*/
NONSHARABLE_CLASS ( CIpsFetchProgReport ) : public CActive
    {
public:
    /**
    * @param aAttaOp (callback) reference to fetch operation that created us
    */
    static CIpsFetchProgReport* NewL( CIpsPlgImap4FetchAttachmentOp& aAttaOp );

	// <qmail> destructor made virtual
    virtual ~CIpsFetchProgReport();

protected: // From CActive

    virtual void DoCancel();
	
    virtual void RunL();

private:
    CIpsFetchProgReport( CIpsPlgImap4FetchAttachmentOp& aAttaOp );
    void ConstructL();
    void AdjustTimer();

private: // members
    RTimer iTimer;
    // reference to our "parent" op
    CIpsPlgImap4FetchAttachmentOp& iAttaOp; 
    };

/**
* class CIpsPlgImap4FetchAttachmentOp
* fetches messageparts
*/
NONSHARABLE_CLASS ( CIpsPlgImap4FetchAttachmentOp ) :
    public CIpsPlgOnlineOperation
    {
public:

    /**
    * NewL()
    * @param CMsvSession& client/server session to MsvServer
    * @param aObserverRequestStatus operation's observer's status
    * @param aService serviceId of this mailbox
    * @param aActivityTimer mailbox specific activity timer
    * @param aSelection selection of message part ids, ownership moves
    * @param aFSMailBoxId specifies mailbox
    * @param aFSOperationObserver observes the progress of this operation
    * @param aFSRequestId identifier for this instance of operation, assigned by the client
    * @return CIpsPlgImap4FetchAttachmentOp* self pointer
    */
	// <qmail> CMsvEntrySelection& changed to pointer, aFunctionId removed
	// <qmail> MFSMailRequestObserver& changed to pointer
    static CIpsPlgImap4FetchAttachmentOp* NewL(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aService,
        CIpsPlgTimerOperation& aActivityTimer,
        const CMsvEntrySelection* aSelection,
        TFSMailMsgId aFSMailBoxId,
        MFSMailRequestObserver* aFSOperationObserver,
        TInt aFSRequestId );

    /**
    * ~CIpsPlgImap4FetchAttachmentOp()
    * destructor
    */
    virtual ~CIpsPlgImap4FetchAttachmentOp();

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
    
private:
	// <qmail> CMsvEntrySelection& changed to pointer, aFunctionId removed
	// <qmail> MFSMailRequestObserver& changed to pointer
    CIpsPlgImap4FetchAttachmentOp(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aService,
        CIpsPlgTimerOperation& aActivityTimer,
        const CMsvEntrySelection* aSelection,
        TFSMailMsgId aFSMailBoxId,
        MFSMailRequestObserver* aFSOperationObserver,
        TInt aFSRequestId );

    void ConstructL();

    void RunL();

    void DoRunL();

    //virtual void DoCancel();

    void Complete();
    
    void DoConnectL();
    
    /**
     * Called periodically by CIpsFetchProgReport during active fetching
     * Leads to sending progress report to plugin's client
     */
    void ReportProgressL();  
    
    // <qmail> new func to this op
    /**
     * Returns operation type
     */
    TIpsOpType IpsOpType() const;
    
protected:
    
	// <qmail> GetEngineProgress removed
    void DoFetchAttachmentL( );
        
// <qmail>
private: //Data
    friend class CIpsFetchProgReport;
// <qmail>

    // internal state of this statemachine
    enum TFetchState {
        EStateIdle,
        EStateConnecting,
        EStateFetching,
		EStateWaiting,
        EStateDisconnecting };
    TFetchState iState;
        
    const CMsvEntrySelection*               iSelection;
    TDesC8*                                 iFetchErrorProgress;
	// <qmail> iGetMailInfo, iFunctionId removed
    TPckgBuf<TImap4CompoundProgress>        iProgress;
	// <qmail> iService, iPopulated removed
    CIpsFetchProgReport*                    iProgReport;
    // Temporary fix for handling KErrServerBusy errors from the
    // messaging service.  Remove these once the appropriate observer
    // mechanism has been implemented in the messaging service.
    TInt                                    iRetryCount;
    RTimer                                  iRetryTimer;
    };

#endif // IPSPLGIMAP4FETCHATTACHMENTOP_H
