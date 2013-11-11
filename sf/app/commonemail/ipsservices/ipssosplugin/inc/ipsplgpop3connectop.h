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
* Description:  POP connect operation
*
*/

#ifndef IPSPLGPOP3CONNECTOP_H
#define IPSPLGPOP3CONNECTOP_H


#include "ipsplgonlineoperation.h"
class CIpsPlgEventHandler;

/**
* Connect operation.
* Encapsulates connection validation.
*/
NONSHARABLE_CLASS ( CIpsPlgPop3ConnectOp ) :
    public CIpsPlgOnlineOperation
    // public MIpsPlgConnectOpCallback <qmail> not used any more
    {
    // <qmail> MIpsPlgConnectOpCallback not used any more
public:

	    /**
	    * NewL
	    * @param aMsvSession client/server session to MsvServer
	    * @param aObserverRequestStatus client status
	    * @param aService serviceId of the mailbox
	    * @param aForcePopulate whether to populate fetched messages
	    * @param aActivityTimer mailbox specific activity timer
	    * @param aFSMailBoxId specifies mailbox
	    * @param aFSOperationObserver observer callback pointer
	    * @param aFSRequestId client assigned identifier for the request instance
	    * @param aEventHandler event handler for sending sync events
	    * @param aSignallingAllowed for asynchronous request response message
	    * @param aFetchWillFollow used when connection must be kept open
	    * @return new instance of the class
	    */
		// <qmail> MFSMailRequestObserver& changed to pointer
		// <qmail> aSignallingAllowed parameter added
        // <qmail> aFetchWillFollow parameter added
        static CIpsPlgPop3ConnectOp* NewL(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aService,
            TBool aForcePopulate,
            CIpsPlgTimerOperation& aActivityTimer,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver* aFSOperationObserver,
            TInt aFSRequestId,
            CIpsPlgEventHandler* aEventHandler,
            TBool aSignallingAllowed=ETrue,
            TBool aFetchWillFollow=EFalse );

        /**
        *
        */
        virtual ~CIpsPlgPop3ConnectOp();

        /**
	    * From CIpsPlgBaseOperation
	    * For reporting if DoRunL leaves
        */
        const TDesC8& GetErrorProgressL(TInt aError);
        
        /**
        *
        */
        TFSProgress GetFSProgressL() const;

	    /**
	    * From MsvOperation
	    * Gets information on the progress of the operation
	    * (see MsvOperation header)
	    */
	    // <qmail> moved to 'public:'as defined so in base class
	    const TDesC8& ProgressL();
// <qmail> Connected() used from baseclass
        
// <qmail> change ret val type
        /**
         * Returns operation type
         */
        TIpsOpType IpsOpType() const;
// </qmail>

        TInt GetOperationErrorCodeL( );
        
    protected:

        /**
         * 
         */
        void DoCancel();

        /**
        * From CActive
        */
        void DoRunL();
        
    private:

        /**
        *
        */
		// <qmail> MFSMailRequestObserver& changed to pointer
		// <qmail> aSignallingAllowed parameter added
        // <qmail> aFetchWillFollow parameter added
        CIpsPlgPop3ConnectOp(
            CMsvSession& aMsvSession,
            TRequestStatus& aObserverRequestStatus,
            TMsvId aServiceId,
            TBool aForcePopulate,
            CIpsPlgTimerOperation& aActivityTimer,
            TFSMailMsgId aFSMailBoxId,
            MFSMailRequestObserver* aFSOperationObserver,
            TInt aFSRequestId,
            CIpsPlgEventHandler* aEventHandler,
            TBool aSignallingAllowed,
            TBool aFetchWillFollow );

        /**
        *
        */
        void ConstructL();

        // <qmail>
        /**
         * Do.. functions handle certain state of this operation
         */
        void DoConnectL();
        void DoPopulateL();
	    // <qmail> removed TBool ValidateL() (did nothing)
	    // <qmail> removed void DoQueryPasswordL() not used any more
        // <qmail> DoDisconnect -> DoDisconnectL
        void DoDisconnectL();
		// </qmail>
        
        // <qmail> removed flag methods as they were not used or even defined anywhere
        
	    // <qmail> new function
	    /**
	     * Reads populate limit from account's settings and converts it to member variable
	     */
	    TInt GetPopulateLimitFromSettingsL();
    private: // Data
    
    	enum TOperationState 
            {
            EStartConnect,
            EConnected,
            EPopulate,
            // <qmail> removed EQueryingDetails/EQueryingDetailsBusy state
        	// <qmail> new EDisconnecting state
            EDisconnecting,
        	// <qmail> removed EErrInvalidDetails,
            EIdle
            };

	    TOperationState                                 iState;
	    // <qmail> removed iEntry;
	    TPckgBuf<TPop3Progress>                         iProgress;
	    // <qmail> removed iPopulateLimit;
	    TBool                                           iForcePopulate;
	    // <qmail> removed iSelection;
	    CIpsPlgEventHandler*                            iEventHandler; // not owned
	    // <qmail> iAlreadyConnected removed
        // <qmail>
        TBool                                           iFetchWillFollow;
        // </qmail>
    };

#endif
