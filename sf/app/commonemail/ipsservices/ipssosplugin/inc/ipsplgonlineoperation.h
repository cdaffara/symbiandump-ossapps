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
* Description:  Base online operation class
*
*/

#ifndef IPSPLGONLINEOPERATION_H
#define IPSPLGONLINEOPERATION_H

#include "ipsplgbaseoperation.h"

class CMsvEntrySelection;
class TFSMailMsgId;
class MFSMailRequestObserver;
class CIpsPlgTimerOperation;
class CClientMtmRegistry;

// <qmail> removing class MIpsPlgConnectOpCallback

/**
* class CIpsPlgOnlineOperation
* Common base class for email online operations.
*/
NONSHARABLE_CLASS ( CIpsPlgOnlineOperation ) : public CIpsPlgBaseOperation
    {
public:
    virtual ~CIpsPlgOnlineOperation();

    // <qmail> removing virtual const TDesC8& ProgressL() = 0; as unnecessary (already in baseclass)
    // <qmail> removing virtual const TDesC8& GetErrorProgressL(TInt aError) = 0; as unnecessary (already in baseclass)
    
    /**
     * return progress information about ongoing operation
     */
    virtual TFSProgress GetFSProgressL() const = 0;
    
    // <qmail> removing IpsOpType() from here because this is abstract base class
    
protected:

    /**
     * constructor
     * @param aMsvSession reference to existing client-server session to MessageServer
     * @param aObserverRequestStatus client status
     * @param aActivityTimer reference to mailbox's online activity timer
     * @param aFSMailBoxId mailbox id
     * @param aFSOperationObserver callback interface to caller for reporting progress (completion) statuses
     * @param aFSRequestId id of the request
     * @param aSignallingAllowed for asynchronous request response message
     */
    // <qmail> priority parameter has been removed
	// <qmail> MFSMailRequestObserver& changed to pointer
	// <qmail> aSignallingAllowed parameter has been returned
    CIpsPlgOnlineOperation(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        CIpsPlgTimerOperation& aActivityTimer,
        TFSMailMsgId aFSMailBoxId,
        MFSMailRequestObserver* aFSOperationObserver,
        TInt aFSRequestId,
        TBool aSignallingAllowed=ETrue );

    /**
    * Base constructor
    */
    void BaseConstructL( TUid aMtmType );

protected: // From CActive
    virtual void DoCancel();
    virtual void RunL();

    /**
     * All concrete classes must implement this
     * It is called by OnlineOperation's RunL()
     */
    virtual void DoRunL() = 0;

    // <qmail> removing virtual TInt RunError( TInt aError ); Not needed because RunL uses TRAP harness and never leaves

    /**
    * Completes observer with status aStatus
    * @param aStatus: Status of the operation.
    * Override if needed to complete with other status than KErrNone.
    */
    void CompleteObserver( TInt aStatus = KErrNone );

    /**
    * Completes "ourselves" not observer; used for starting a new round in statemachine.
    */
    void CompleteThis();

    /**
    * Sends a command to MessageServer
    * @param aFunctionId specifies the command
    * @param aEntryId specifies the entry on which the command should be executed
    * (e.g. which message entry to send)
    * @param aParams specifies other parameters needed by particular command
    */
	// <qmail> aContextId parameter has been removed
    void InvokeClientMtmAsyncFunctionL(
        TInt aFunctionId,
        TMsvId aEntryId,
        const TDesC8& aParams=KNullDesC8);

    /**
    * Sends a command to MessageServer
    * @param aFunctionId specifies the command
    * @param aSel specifies a selection of entries on which the command should be executed
    * (e.g. which messages to send)
    * @param aParams specifies other parameters needed by particular command
    */
	// <qmail> aContextId parameter has been removed
    void InvokeClientMtmAsyncFunctionL(
        TInt aFunctionId,
        const CMsvEntrySelection& aSel,
        const TDesC8& aParams=KNullDesC8);

    // <qmail>
    /**
     * Finds out the current connection state of the mailbox
     * @return true/false
     */
    virtual TBool Connected() const;
    // </qmail>
    
    /**
    * Signals observer of the operation if such exists
    * @param aStatus completion status of the operation
    */
    void SignalFSObserver( TInt aStatus );
        
protected:
        // mailbox specific activity timer
        CIpsPlgTimerOperation*  iActivityTimer;
        CBaseMtm*               iBaseMtm;
        CClientMtmRegistry*     iMtmReg;
        // sub-operation that this operation is using currently
        CMsvOperation*          iSubOperation;
        // Status of current operation
        TInt                    iError;
        // Return this if iSubOperation==NULL.
        TBuf8<1>                iDummyProg;
        
        // <qmail> boolean returned
        TBool           iSignallingAllowed;
        // </qmail>
        MFSMailRequestObserver* iFSOperationObserver; //not owned
    };

#endif //IPSPLGONLINEOPERATION_H
