/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Contains imap specified sync logic
*
*/


#ifndef IPSSOSAOIMAPAGENT_H_
#define IPSSOSAOIMAPAGENT_H_

#include <imapconnectionobserver.h>
#include <imapset.h>
#include <mtclreg.h>
#include "MFSMailRequestObserver.h"

#include "IpsSosAOBaseAgent.h"

class CImap4ClientMtm;
//<QMail>

//</QMail>
/**
* class CIpsSosAOImapAgent;
*
* Is created by IpsSosAOBaseAgent, contains imap spesific implementations of base class pure
* virtual functions.
*
*/
//<Qmail>
NONSHARABLE_CLASS (CIpsSosAOImapAgent) : public CIpsSosAOBaseAgent,
//</Qmail>
                           public MMsvImapConnectionObserver,
                           public MFSMailRequestObserver
    {
public:
    
    /**
    * Two phased constructor
    * @return self pointer
    */
    static CIpsSosAOImapAgent* NewL( 
            CMsvSession& aSession,
            MIpsSosAOAgentOperationResponse& aOpResponse,
            TMsvId aServiceId );

    /**
    * Destructor
    */
    virtual ~CIpsSosAOImapAgent();
    
public: // from CIpsSosAOBaseAgent
    //<Qmail>
    /**
     * GetServerAddress
     * @param a return parameter
     */
    virtual void GetServerAddress( 
            TBuf<KIpsSosAOTextBufferSize>& aIncomingServer ) const;
    
    /**
     * GetUsername
     * @param a return parameter
     */
    virtual void GetUsername( 
            TBuf8<KIpsSosAOTextBufferSize>& aUsername ) const;
    
    /**
     * checks is does mailbox have connection open
     * @return is connected or not
     */
    virtual TBool IsConnected() const;
    
    /**
     * Starts email sync
     */
    virtual void StartSyncL();
    
    /**
     * starts fetching messages
     */
    virtual void StartFetchMessagesL( const RArray<TMsvId>& aFetchMsgArray );
    
    /**
     * cancels all ongoing suboperations and disconnects
     */
    virtual void CancelAllAndDisconnectL();
    
    /**
     * cancels all suboperations
     */
    virtual void CancelAllAndDoNotDisconnect();
    
    /**
     * sets iDoNotDisconnect as ETrue
     */
    virtual void DoNotDisconnect();
    
    /**
     * sets iDoNotDisconnect as EFalse
     */
    virtual void ClearDoNotDisconnect();
    
    /**
     * pauses ongoing operations
     */
    virtual void HoldOperations();

    /**
     * resumes paused operations
     */
    virtual void ContinueHoldOperations();
    
    /**
     * @return current iState
     */
    virtual CIpsSosAOBaseAgent::TAgentState GetState() const;
    
    /**
     * Loads CImImap4Settings object
     */
    virtual void LoadSettingsL();
    //</Qmail>
public: // MMsvImapConnectionObserver  
    
    /**
    * @param TImapConnectionEvent, imap connection event
    */
    virtual void HandleImapConnectionEvent(
        TImapConnectionEvent aConnectionEvent);
    
public: // MFSMailRequestObserver
    
    /**
    * used as dummy observer, needed for population operation
    */
    virtual void RequestResponseL( TFSProgress aEvent, TInt aRequestId );
    
private:
    
    /**
    * Constructor
    */
    CIpsSosAOImapAgent(             
            CMsvSession& aSession,
            MIpsSosAOAgentOperationResponse& aOpResponse,
            TMsvId aServiceId );

    /**
    * Second phase constructor
    */
    void ConstructL();
    //<Qmail>
    /**
     * called when cancelled
     */
    virtual void DoCancel();
    
    /**
     * from CActive
     */
    virtual void RunL();
    
    /**
     * from CActive
     */
    virtual TInt RunError( TInt aError );

    /**
     * fetches the content of all synced messages
     */
    void PopulateAllL();
    
    /**
     * triggers async statemachine
     */
    inline void SetActiveAndCompleteThis();
	//</Qmail>
private:
    
    CMsvSession&                        iSession;
    MIpsSosAOAgentOperationResponse&    iOpResponse;
    TMsvId                              iServiceId;
    TAgentState                         iState;
    TBool                               iDoNotDisconnect;
	//<Qmail>
    CImImap4Settings*                   iImapSettings;//owned
    CMsvOperation*                      iOngoingOp;//owned

    TPckgBuf<TImap4CompoundProgress>    iProgressBuf;
    TInt                                iError;
    CImap4ClientMtm*                    iImapClientMtm;//owned
    CClientMtmRegistry*                 iMtmReg;//owned
    RArray<TMsvId>                      iFoldersArray;//owned
	//<Qmail>
	//<QMail>

    //</QMail>
    };

#endif /*IPSSOSAOIMAPAGENT_H_*/
