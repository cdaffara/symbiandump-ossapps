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
* Description:  POP3 fetch message(s) operation
*
*/

#ifndef IPSPLGPOP3FETCHOPERATION_H
#define IPSPLGPOP3FETCHOPERATION_H

#include "ipsplgonlineoperation.h"

/**
* CIpsPlgPop3FetchOperation
* Fetch message(s) operation, using client MTM Get Mail API.
*/
NONSHARABLE_CLASS ( CIpsPlgPop3FetchOperation ) :
    public CIpsPlgOnlineOperation
    {
    public:

	    /**
	    * NewL
	    * @param aMsvSession client/server session to MsvServer
	    * @param aObserverRequestStatus client status
	    * @param aService serviceId of mailbox
	    * @param aActivityTimer mailbox specific activity timer
	    * @param aGetMailInfo parameters set for actual MsvServer command
	    * @param aSelection selection of entries to fetch
	    * @param aFSMailBoxId mailbox identifier
	    * @param aFSOperationObserver observer of this operation
	    * @param aFSRequestId client assigned identifier for this operation instance
	    * @param aEventHandler event handler for sending sync status events
	    * @return operation instance
	    */
		// <qmail> MFSMailRequestObserver& changed to pointer
	    static CIpsPlgPop3FetchOperation* NewL(
	        CMsvSession& aMsvSession,
	        TRequestStatus& aObserverRequestStatus,
	        TMsvId aService,
	        CIpsPlgTimerOperation& aActivityTimer,
	        const TImPop3GetMailInfo& aGetMailInfo,
	        CMsvEntrySelection* aSelection,
	        TFSMailMsgId aFSMailBoxId,
	        MFSMailRequestObserver* aFSOperationObserver,
	        TInt aFSRequestId,
	        CIpsPlgEventHandler* aEventHandler );

       
        /**
        *
        */
        virtual ~CIpsPlgPop3FetchOperation();

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
        
        /**
        *
        */
        virtual TFSProgress GetFSProgressL() const;
        
// <qmail> new func to this op
        /**
         * Returns operation type
         */
        TIpsOpType IpsOpType() const;
// </qmail>
        
    protected:

        /**
        *
        */
		// <qmail> MFSMailRequestObserver& changed to pointer
	    CIpsPlgPop3FetchOperation(
	        CMsvSession& aMsvSession,
	        TRequestStatus& aObserverRequestStatus,
	        TMsvId aService,
	        CIpsPlgTimerOperation& aActivityTimer,
	        const TImPop3GetMailInfo& aGetMailInfo,
	        CMsvEntrySelection* aSelection,
	        TFSMailMsgId aFSMailBoxId,
	        MFSMailRequestObserver* aFSOperationObserver,
	        TInt aFSRequestId,
	        CIpsPlgEventHandler* aEventHandler);

            
        /**
        *
        */
        void ConstructL();

        /**
        * From CActive
        */
        virtual void RunL();

        /**
        * From CActive
        */
        virtual void DoCancel();

        /**
        * From CActive
        */
        virtual void DoRunL();

        /**
        *
        */
        void DoConnectL();

        /**
        *
        */
        void DoFetchL();

        /**
        *
        */
        void DoDisconnectL();

	// <qmail> protected to private
	private:

        enum TFetchState {
            EStateIdle,
            EStateConnecting,
            EStateClearCompleteFlag,
            EStateFetching,
            EStateDisconnecting };
    	// internal state of the operation
        TFetchState iState;
        // <qmail> protected to private
        TDesC8* iFetchErrorProgress;
    	// <qmail> removed iFunctionId;
        TImPop3GetMailInfo iGetMailInfo;
        TPckgBuf<TPop3Progress> iProgressBuf;
        CMsvEntrySelection* iSelection;
        CIpsPlgEventHandler* iEventHandler;
        // For clear complete flag operation
        CMsvEntry* iEntry;
        TInt iEntryIndex;
    };


#endif //IPSPLGPOP3FETCHOPERATION_H

// End of File
