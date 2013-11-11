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
*     Base class for always online mail syncing and fetchin operations
*
*/


#ifndef IPSSOSAOBASEAGENT_H
#define IPSSOSAOBASEAGENT_H

#include <e32base.h> 
#include <msvapi.h>

class CIpsSetDataApi;
class CIpsSosAOImapSyncOp;
class CImImap4Settings;
class MIpsAOObserverNotifyInterface;
class TIpsPlgPropertyEvent;

const TInt KIpsSosAOTextBufferSize = 200;

/**
* class MIpsSosAOAgentOperationResponse;
* 
* Interface for operation completion call backs
*
*/
class MIpsSosAOAgentOperationResponse
    {
public:
    virtual void OperationCompletedL( TInt aError ) = 0;
    };


/**
* class CIpsSosAOBaseAgent
*
* Base class for always online mailbox syncing and fetching logic.
* Offer interface for ao mbox logic class to hide symbian spesific
* mail sync and fetch logic.
* Created by IpsSosAOMboxLogic only ones, is active when asyncronic operation
* is running. Instance of this class is mailbox spesific.
*
* @lib ipssosaoplugin.lib
*/
//<Qmail>
NONSHARABLE_CLASS (CIpsSosAOBaseAgent) : public CActive
//</Qmail>
    {
    
public: // enums
    
    enum TAgentState
        { 
        EStateConnectAndSync,
        EStateConnectAndSyncOnHold,
        EStateRefreshFolderArray,
        EStatePopulateAll,
        EStatePopulateOnHold,
        EStateFetchOngoing,
        EStateFetchOnHold,
        EStateDisconnect,
        EStateCompleted,
        EStateSuspended,
        EStateIdle };
    
public:
    
    /**
    * Creates CIpsSosAOImapAgent or CIpsSosAOPopAgent according to type
    * of service, return value is casted to CIpsSosAOBaseAgent type object
    * @param aSession valid msv session reference
    * @parem aOpResponse interface class to async operation response call backs
    * @param aServiceId id of mailbox
    * @return CIpsSisAOBaseAgent pointer
    */
    static CIpsSosAOBaseAgent* CreateAgentL( 
            CMsvSession& aSession,
            MIpsSosAOAgentOperationResponse& aOpResponse,
            TMsvId aServiceId );
    
    /*
     * Destructor
     */
    virtual ~CIpsSosAOBaseAgent();
    
    /**
     * Checks from mtm uid is this agent imap agent,
     * mtm uid is set in constructor
     * @return ETrue if uid is IMAP, EFalse in other case 
     */
    TBool IsTypeImap4() const;
    
    /**
     * Gets FS plugin id related to protocol
     * @return FS plugin id
     */
    TInt GetFSPluginId() const;
    
    /**
     * Signals mailbox sync to started via P&S (Publish & Subscribe) mechanism
     * @param aMailbox id of the mailbox which sync is started 
     */
    void SignalSyncStarted( TInt aMailbox );
    
    /**
     * Signals mailbox sync to completed via P&S mechanism
     * @param aMailbox id of the mailbox which sync is competed
     * @param aError sync completion error code
     */
    void SignalSyncCompleted( TInt aMailbox, TInt aError );
    
    /**
     * Gets incoming server address from (symbian side) mailbox settings
     * NOTE: LoadSettingsL should be called before this in some point or
     * parameter is unchanged
     * @param aIncomingServer modifiable buffer where address is written
     */
    virtual void GetServerAddress( 
            TBuf<KIpsSosAOTextBufferSize>& aIncomingServer ) const = 0;
    
    /**
     * Gets mail account username from (symbian side) mailbox settings
     * NOTE: LoadSettingsL should be called before this in some point or
     * parameter is unchanged
     * @param aUsername modifiable buffer where username is written
     */
    virtual void GetUsername( 
            TBuf8<KIpsSosAOTextBufferSize>& aUsername ) const = 0;
    
    /**
     * Gets mailbox connection status
     * @return ETrue if mailbox is connected
     */
    virtual TBool IsConnected() const = 0;
    
    /**
     * Starts mailbox sync asynchronically, sync completion is informed via 
     * MIpsSosAOAgentOperationResponse interface
     */
    virtual void StartSyncL() = 0;
    
    /**
     * Starts message fetch asynchronically, fetch completion is informed via 
     * MIpsSosAOAgentOperationResponse interface
     * NOTE: this is not used in FS EMail 2.0, this is for later usege, mainly
     * for case if AO have to support IMAP-IDLE
     * @param aFetchMsgArray array of message ids to be fethced
     */
    virtual void StartFetchMessagesL( const RArray<TMsvId>& aFetchMsgArray ) = 0;
    
    /**
     * Cancel all ongoing operations and disconnects the service if it's connected
     */
    virtual void CancelAllAndDisconnectL() = 0;
    
    /**
     * Cancel all ongoing operations but not disconnect service, is used when
     * another operation should started for ex. user starts message fetching from ui
     * during always online sync
     */
    virtual void CancelAllAndDoNotDisconnect() = 0;
    
    /**
     * Sets agent not to disconnect after operation is completed.
     */
    virtual void DoNotDisconnect() = 0;
    
    /*
     * Clears not to disconnect flag
     */
    virtual void ClearDoNotDisconnect() = 0;
    
    /*
     * Cancels ongoing operation and puts agent on hold state
     */
    virtual void HoldOperations() = 0;

    /*
     * Continues operation that was cancelled when holded operations,
     * Does nothing if there's agent is not on hold state
     */
    virtual void ContinueHoldOperations() = 0;
    
    /*
     * Gets current agent state
     */
    virtual TAgentState GetState() const = 0;
    
    /*
     * Loads settings
     */
    virtual void LoadSettingsL() = 0;
    
protected:
    
    /*
     * Constructor 
     */
    CIpsSosAOBaseAgent();
    
    /*
     * Clears flag that informs is some event signaled via P&S by this agent instance
     */
    void ClearSignaledFlags();
    
private:
    
    /*
     * Sets new P&S property 
     */
    void SetPropertyEvent( 
            TInt aEvent, 
            TInt aMailbox,
            TInt aError );
    
private:
    
    TBool                           iSyncStartedSignaled;
    TUid                            iMtmType;
    TPckgBuf<TIpsPlgPropertyEvent>  iPropertyBuf;
    
    };

#endif /*IPSSOSAOBASEAGENT_H*/
