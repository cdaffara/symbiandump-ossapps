/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*       folder refresh operation
*
*/
#ifndef IPSPLGCONNECTANDREFRESHFOLDERLIST_H
#define IPSPLGCONNECTANDREFRESHFOLDERLIST_H

// <qmail> AknWaitNoteWrapper include has been removed
#include <imapset.h>

// Specific includes
#include "ipsplgonlineoperation.h"

/**
* Operation that 1) goes online, 2) updates mailbox's folder list, and 3) disconnects again
*/
// <qmail> MAknBackgroundProcess base class has been removed
NONSHARABLE_CLASS ( CIpsPlgConnectAndRefreshFolderList ) : public CIpsPlgOnlineOperation
    {
// <qmail> removed 3 functions from MAknBackgroundProcess
public:

    /**
    * Construction
    * @param aSession session to the MsvServer
    * @param aObserverRequestStatus caller's status
    * @param aService service entry in use (related to mailbox)
    * @param aMailboxId identifies the mailbox
    * @param aSelection specifies selection (array) of entries that this op operates on
    *        ownership is moved to the operation
    * @param aFSObserver operation's observer
    * @param aActivityTimer mailbox specific activity timer
    * @return operation instance
    */
    // <qmail> priority parameter has been removed
    // <qmail> rename selection parameter, and changed it to pointer
    // <qmail> removed EXPORT from this function
    static CIpsPlgConnectAndRefreshFolderList* NewL(
        CMsvSession& aSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aService, 
        TFSMailMsgId& aMailboxId, 
        CMsvEntrySelection* aSelection,
        MFSMailRequestObserver* aFSObserver, 
        CIpsPlgTimerOperation& aActivityTimer );

    /**
    * Destructor
    * ~CIpsPlgConnectAndRefreshFolderList()
    */
    virtual ~CIpsPlgConnectAndRefreshFolderList();
    
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
    const TDesC8& GetErrorProgressL(TInt aError);
    
    TFSProgress GetFSProgressL() const;

// <qmail> new func to this op
    /**
     * Returns operation type
     */
    TIpsOpType IpsOpType() const;
// </qmail>

protected: // From CActive

    virtual void DoRunL();
    virtual void DoCancel();
    
private:
    // internal states of this operation
    enum TIpsSetFolderRefreshStates
        {
        EIdle,
        EStartConnect,
        EConnecting,
        ERefreshing,
        EDisconnecting,
        ECompleted
        };

	/**
    * C++ constructor
    * CIpsPlgConnectAndRefreshFolderList()
    */
    // <qmail> rename selection parameter
	// <qmail> MFSMailRequestObserver& changed to pointer
	CIpsPlgConnectAndRefreshFolderList( 
        CMsvSession& aSession,
        TRequestStatus& aObserverRequestStatus, 
        TMsvId aService, 
        TFSMailMsgId& aMailboxId,
        CMsvEntrySelection* aSelection,
        MFSMailRequestObserver* aFSObserver, 
        CIpsPlgTimerOperation& aTimer );

    /**
    * ConstructL()
    */
	// <qmail> aMsvEntry parameter has been removed
    void ConstructL();

	// <qmail> DisplayLoginFailedDialogL function has been removed

    //data
private:
    // <qmail> changes in members: iTimer, iAsyncWaitNote removed, iMsvEntry -> iSelection
    TIpsSetFolderRefreshStates          iState;
    CMsvEntrySelection*					iSelection; // owned
    TPckgBuf<TImap4CompoundProgress>    iProgressBuf;
    // </qmail>
    };

#endif // IPSPLGCONNECTANDREFRESHFOLDERLIST_H
