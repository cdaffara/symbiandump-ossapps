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
* Description:  IMAP4 populate messages operation
*
*/

#ifndef IPSPLGIMAP4POPULATEOP_H
#define IPSPLGIMAP4POPULATEOP_H

#include "ipsplgonlineoperation.h"

class CIpsPlgEventHandler;

/**
* class CIpsPlgImap4PopulateOp
* loads message's actual content/ message parts
*/
NONSHARABLE_CLASS ( CIpsPlgImap4PopulateOp ) :
    public CIpsPlgOnlineOperation
    {
public:

    /**
    * NewL()
    * Basic factory function - creates dialog with standard title resource
    * @param aMsvSession client/server session to msvServer 
    * @param aObserverRequestStatus caller's status
    * @param aService serviceId of the mailbox
    * @param aActivityTimer mailbox specific activity timer
    * @param aPartialMailInfo parameter struct for the service request to msvServer
    * @param aSel selection of messages to populate (first entry contains service entry)
    * @param aFSMailBoxId mailbox identifier
    * @param aFSOperationObserver observer for this operation
    * @param aFSRequestId client assigned request Id
    * @param aEventHandler 
    * @param aDoFilterSelection specifies whether a full populate is requested (i.e. refetch already fecthed or not)
    * @return CIpsPlgImap4PopulateOperation* self pointer
    */
    // <qmail> priority parameter has been removed
	// <qmail> MFSMailRequestObserver& changed to pointer
    IMPORT_C static CIpsPlgImap4PopulateOp* NewL(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aService,
        CIpsPlgTimerOperation& aActivityTimer,
        const TImImap4GetPartialMailInfo& aPartialMailInfo,
        const CMsvEntrySelection& aSel,
        TFSMailMsgId aFSMailBoxId,
        MFSMailRequestObserver* aFSOperationObserver,
        TInt aFSRequestId,
        CIpsPlgEventHandler* aEventHandler,  
        TBool aDoFilterSelection = ETrue );

    virtual ~CIpsPlgImap4PopulateOp();

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
     
    // <qmail> change ret val type
    /**
     * Returns operation type
     */
    TIpsOpType IpsOpType() const;
    
private:
    // <qmail> priority parameter has been removed
    CIpsPlgImap4PopulateOp(
        CMsvSession& aMsvSession,
        TRequestStatus& aObserverRequestStatus,
        TMsvId aService,
        CIpsPlgTimerOperation& aActivityTimer,
        const TImImap4GetPartialMailInfo& aGetMailInfo,
        TFSMailMsgId aFSMailBoxId,
        MFSMailRequestObserver* aFSOperationObserver,
        TInt aFSRequestId,
        CIpsPlgEventHandler* aEventHandler );

    void ConstructL( const CMsvEntrySelection& aSel, TBool aDoFilterSelection );

    void RunL();

    void DoRunL();
    
    void DoCancel();

    /**
     * completes the status of the observer
     */
    void Complete();
    
    /**
    * handle connecting state
    */
    void DoConnectL();
    
    /**
    * Filters given selection of entries
    * The result is set to internal iSelection member
    * (filtering is done on basis of what is the current populate status of the particular message)
    * @param aSelection list of entries to fetch
    */
    void FilterSelectionL( const CMsvEntrySelection& aSelection );
    
    /**
    * helper for selection filtering
    */
    TBool IsPartialPopulate();

protected:
    
    /**
    * starts the actual populate command to msvserver
    */
    void DoPopulateL();
    
    // <qmail> remove GetEngineProgress( const TDesC8& aProgress );
    
private: //Data
    // <qmail> members changed
    // internal state of the operation
    enum TFetchState 
        {
        EStateIdle,
        EStateConnecting,
        EStateFetching,
        EStateInfoEntryChange
        };
    TFetchState                       iState;
    
    // parameter struct for the populate command to msvserver
    TImImap4GetPartialMailInfo        iPartialMailInfo;
    // contains full list of messages to fetch
    CMsvEntrySelection*               iSelection;
    // contains serviceId and one message entry id from iSelection list
    // i.e. populate op loops through iSelection with this iTempSelection
    CMsvEntrySelection*               iTempSelection;
    // not owned
    CIpsPlgEventHandler*              iEventHandler;
    TDesC8*                           iFetchErrorProgress;
    TPckgBuf<TImap4SyncProgress>      iSyncProgress;
    };

#endif // IPSPLGIMAP4POPULATEOP_H
