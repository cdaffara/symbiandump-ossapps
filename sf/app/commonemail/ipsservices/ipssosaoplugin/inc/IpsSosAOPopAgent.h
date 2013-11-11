/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Contains pop specified sync logic
*
*/


#ifndef IPSSOSAOPOPAGENT_H
#define IPSSOSAOPOPAGENT_H

#include <pop3set.h>
#include <mtclreg.h>

#include "IpsSosAOBaseAgent.h"


class CPop3ClientMtm;

/**
* class CIpsSosAOPopAgent;
*
* Is created by IpsSosAOBaseAgent, contains pop spesific 
* implementations of base class pure virtual functions.
*
*/
//<Qmail>
NONSHARABLE_CLASS (CIpsSosAOPopAgent) : public CIpsSosAOBaseAgent
//</Qmail>
    {
public:
    
    /**
    * Two phased constructor
    * @return self pointer
    */
    static CIpsSosAOPopAgent* NewL( 
            CMsvSession& aSession,
            MIpsSosAOAgentOperationResponse& aOpResponse,
            TMsvId aServiceId );

    /**
    * Destructor
    */
    virtual ~CIpsSosAOPopAgent();
    
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
     * Loads CImPop3Settings object
     */
    virtual void LoadSettingsL();
    //</Qmail>
private:

    /**
    * Constructor
    */
    CIpsSosAOPopAgent(             
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
    TBool                               iSyncStartedSignaled;
    TAgentState                         iState;
    TBool                               iDoNotDisconnect;
	//<Qmail>
    CImPop3Settings*                    iPopSettings;//owned
    CMsvOperation*                      iOngoingOp;//owned

    TPckgBuf<TPop3Progress>             iProgressBuf;
    CMsvEntrySelection*                 iSelection;//owned
    TInt                                iError;
    CPop3ClientMtm*                     iPopClientMtm;//owned
    CClientMtmRegistry*                 iMtmReg;//owned
	//</Qmail>
    TInt                                iPopulateLimit;
    
    };


#endif /*IPSSOSAOPOPAGENT_H*/
