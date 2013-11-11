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
*     Contains mailbox specified always online logic
*
*/
//<Qmail>
#ifndef IPSSOSAOMBOXLOGIC_H
#define IPSSOSAOMBOXLOGIC_H
//</Qmail>
#include <e32base.h> 
#include <msvapi.h>


#include "IpsSosAOBaseAgent.h"
//<QMail>

//</QMail>
#include "IpsSosAOPluginTimer.h"

//<QMail>

//</QMail>
class CIpsSosAOBaseAgent;
//<QMail>

//</QMail>
class CIpsSosAOImapPopLogic;

/**
* class CIpsSosAOImapPopLogic;
*
*/
//<Qmail>
NONSHARABLE_CLASS (CIpsSosAOMBoxLogic) : 
//</Qmail>
    public CBase, 
    public MIpsSosAOPluginTimerCallBack,
    public MIpsSosAOAgentOperationResponse
    {

public:
    
    /**
    * Two phased constructor
    * @return self pointer
    */
    static CIpsSosAOMBoxLogic* NewL( 
            CMsvSession& aSession,
            TMsvId aMailboxId );

    /**
    * Destructor
    */
    virtual ~CIpsSosAOMBoxLogic();

public: 
    
    // from MIpsSosAOPluginTimerCallBack
    virtual void TimerFiredL();
    
    // from MIpsSosAOAgentOperationResponse
    virtual void OperationCompletedL( TInt aError );
    
public:
    
    enum TMboxLogicCommand
        {
        // starts timed sync, or set logic to idle 
        // state to wait other commands
        ECommandStart,
        // cancel all ongoing operation and switch logic
        // to error state
        ECommandStop,
        // suspends logic to certain period of time
        ECommandSuspend,
        // continue suspended logic, does nothing if not
        // suspended
        ECommandContinue,
        // starts sync if connection is allowed,
        // used if emn is received
        ECommandStartSync,
        // inform roaming state. If sync is not allowed
        // in roaming state, stops logic 
        ECommandStartedRoaming,
        // inform home network state. Starts logic if it
        // is stopped
        ECommandStoppedRoaming,
        // cancel all ongoing operations and disconnect if mailbox
        // is connected state
        ECommandCancelAndDisconnect, 
        // cancel all but not disconnect
        ECommandCancelDoNotDiconnect,
        // mailbox is not disconnected if box is connected
        // non of ongoing operations are cancelled
        ECommandDoNotDisconnect,
        // clear do not disconnect flag, it's also
        // cleared if mailbox is disconnected elsewhere
        ECommandClearDoNotDisconnect
        };
    
    enum TMBoxLogicState
        {
        // in this state timer is waiting to sync start
        EStateWaitSyncStart = 0,
        //
        EStateSyncOngoing,
        //
        EStateFetchOngoing,
        //
        EStateSuspended,
        //
        EStateIdleAndWaitCommands,
        // in error state not taking any commands exept start
        EStateError
        };
    
    /*
     * Used by imap pop logic object to send commands to this logic object
     * @param aCommand TMBoxLogicCommand 
     */
    void SendCommandL( TInt aCommand );
    
    /*
     * Starts fetching messages 
     * NOTE: this is not currently used or tested, function can be 
     * used if IMAP-IDLE supportis needed
     * @param aArray msv id array of messages to be fetched
     */
    void FetchMessagesL( const RArray<TMsvId> aArray );
    
    /*
     * Gets mailbox id of logic object
     * @return msv id of the service
     */
    TMsvId GetMailboxId();
    
    /*
     * Gets reference to agent object
     * not currently used
     * @return reference to agent object
     */
    CIpsSosAOBaseAgent& GetAgent() const;
    
    /*
     * Gets emn state of mailbox settings
     * @return ETrue if emn is on
     */
    TBool IsEmnOn() const;
    
    /*
     * Gets TMboxLogicState of this logic object
     * @return TMBoxLogicState
     */
    TInt GetCurrentState() const;
    
    /*
     * Gets email address from settings
     * @param aEmailAddress mailbox email address
     */
    void GetEmailAddress(  
        TBuf<KIpsSosAOTextBufferSize>& aEmailAddress ) const;

    /*
     * Sets new value to emn received but no synced flag in
     * mailbox settings
     * @param aNewValue new value of flag
     */
    void SetEmnReceivedFlagL( TBool aNewValue );
    
    /*
     * Gets true if mailbox is stopped because of roaming
     * @return ETrue if roaming and mailbox logic is stopped during roaming
     */
    TBool IsMailboxRoamingStoppedL();
    
	/**
     * Has mailbox ever received and OMA EMN
     */
    TBool FirstEMNReceived();
    
    /**
     * First OMA EMN has been received.
     * Sets a flag to indicate the reception of fist OMA EMN,
     * Changes settings to "after notification", which means
     * turning off alwaysonline and using OMA EMN only.
     */
    void SetFirstEMNReceived();
    
private:
    
    /*
     * internal events which are generated when timer tricks of whne
     * command is sent to logic
     */
    enum TMBoxLogicEvent 
        {
        EEventTimerFired,
        EEventStartSync,
        EEventFetchMessages,
        EEventOperationCompleted,
        EEventSuspendOperations,
        EEventContinueOperations,
        EEventStart,
        EEventStopAndRemoveOps,
        EEventNop
        };
    
private:
    
    /**
    * Constructor
    */
    CIpsSosAOMBoxLogic(             
            CMsvSession& aSession,
            TMsvId aMailboxId );

    /**
    * Second phase constructor
    */
    void ConstructL();
    
    /*
     * Switch state according to event, all state transion
     * are handled in this function according to event parameter.
     */
    void HandleEventAndSwitchStateL( TInt aEvent );
    
    /*
     * is called by timer object
     */
    TInt HandleTimerFiredL();
    
    /*
     * is called when agent have completed its async operation
     */
    TInt HandleOperationCompletionL();
    
    /*
     * checks is operation completion error fatal and should timed sync swithc off
     */
    TBool IsErrorFatalL( TInt aError );
    
    /*
     * Suspends ongoing sync (or fetch) and swiths logic to suspend state
     */
    void SuspendOperations();
    
    /*
     * Checks is it time to start sync or set timer back on
     * returns event to handle event and switch state loop
     */
    TMBoxLogicEvent CheckSchedulingAndSwitchStateL();
    
    /*
     * calculates next interval (clock) time
     */
    void CalculateToNextIntervalL();
    
    /*
     * calculates seconds to next sync
     */
    TTimeIntervalSeconds CalculateScheduledSyncTimeL();
    
    /*
     * destroys old setting object and loads it again from setttings
     */
    void LoadSettingsL();
    
    /*
     * saves successfull sync time to mailbox settings
     */
    void SaveSuccessfulSyncTimeL();
    
    /*
     * checks from settings is connect allowed in roaming state
     */
    TBool CanConnectIfRoamingL();

    
private:
    
    CMsvSession&                    iSession;
    TMsvId                          iMailboxId;
    TMBoxLogicState                 iState;
    TInt                            iErrorCounter;
    TInt                            iError;
	//<Qmail>
    CIpsSosAOPluginTimer*           iTimer;//owned
    // message fetch is not currently used
    RArray<TMsvId>                  iFetchMsgArray;//owned
	//<QMail>

	//</QMail>
    CIpsSosAOBaseAgent*             iAgent;//owned
	//</Qmail>
	//<QMail>

	//</QMail>
    TBool                           iIsRoaming;
    };

#endif /*IPSSOSAOMBOXLOGIC_H*/
