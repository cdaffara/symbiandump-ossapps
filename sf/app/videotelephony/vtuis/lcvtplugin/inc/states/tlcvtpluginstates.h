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
* Description:  Concrete VT plugin state classes
*
*/


#ifndef T_LCVTPLUGINSTATES_H
#define T_LCVTPLUGINSTATES_H

#include "tlcvtstatebase.h"
#include <babitflags.h>
#include <vtengcommands.h>


/**
*  Initial application state. Responsible for startup phase 1 and
*  startup phase 2 (see EVtUiAppUiStartup in CVtUiAppUi.cpp).
*  Certain other state changes are required for phase 2 and history
*  of their reception is handled with precondition flags.
*
*  @since S60 v3.2
*/
class TLcVtStateStarting : public TLcVtStateBase
    {
public:
    /**
    * returns singleton instance of TLcVtStateStarting
    * @param aCtx state context
    * @param aUiStates UI states
    * @return singleton
    */
    static TLcVtStateStarting* InstanceL(
        MLcVtStateContext& aCtx,
        TLcVtStates& aUiStates );

public: // from TLcVtStateBase

    /** @see TLcVtStateBase */
    virtual void LcVtStartupPhase1DoneL();

    /** @see TLcVtStateBase */
    virtual void LcVtStartupPhase2DoneL();

    /** @see TLcVtStateBase */
    virtual TEventResponse HandleVtEventL( TInt aEvent );

    /** @see TLcVtStateBase */
    virtual TEventResponse HandleCommandL( const TInt aCommand );

    /** @see TLcVtStateBase */
    virtual TBool HandleForegroundChangedL( TBool aIsForeground );

    /** @see TLcVtStateBase */
    virtual TBool PreHandleForegroundChangedL( TBool aIsForeground );

private: // from TLcVtStateBase

    /** @see TLcVtStateBase */
    virtual void OpenL();

    /** @see TLcVtStateBase */
    virtual void Close();

private: // new methods

    void CheckSessionStateL();
    
    static TInt AsynchOpen( TAny* aAny );
    
    void DoOpenL();
    
    /** Preconditions that must be passed in order to proceed
     * to Phase 2 in startup.
     */
    enum TPreconditions
        {
        /** waiting for CLI from engine*/
        EWaitingCLI,
        /** waiting for phase1 to complete */
        EWaitingPhase1ToComplete,
        /** waiting for engine to get proper state */
        EWaitingSessionState
    };

    /**
    * Clears precondition passed as argument and checks if any
    * preconditions hold and if not proceeds to phase 2 of startup.
    */
    void CheckPhase2StartupL( const TPreconditions aClearCondition );
    
    void HandleCLIReceivedL();

private: // constructors and destructors

    // c++ constructor
    TLcVtStateStarting(
        MLcVtStateContext& aCtx,
        TLcVtStates& aUiStates );

    // Destructor
    virtual ~TLcVtStateStarting();

private: // data members

    // Precondition for state change to running
    TBitFlags8 iPreconditions;

    // Singleton
    static TLcVtStateStarting* iInstance;
    
    // Starts phase1 startup in state state context. Needed because
    // initial state is opened from within state context construction.
    CAsyncCallBack*     iStartCallback;
    };

/**
*  Application state that is active in normal operation.
*
*  @since S60 v3.2
*/
class TLcVtStateRunning : public TLcVtStateBase
    {
public:
    /**
    * returns singleton instance of TLcVtStateRunning
    * @param aCtx state context
    * @param aUiStates UI states
    * @return singleton
    */
    static TLcVtStateRunning* InstanceL(
        MLcVtStateContext& aCtx,
        TLcVtStates& aUiStates );

public: // from TLcVtStateBase

    /** @see TLcVtStateBase */
    virtual TEventResponse HandleVtEventL( TInt aEvent );

    /** @see TLcVtStateBase */
    virtual TEventResponse HandleCommandL( const TInt aCommand );

    /** @see TLcVtStateBase */
    virtual TBool HandleForegroundChangedL( TBool aIsForeground );

    /** @see TLcVtStateBase */
    virtual TBool PreHandleForegroundChangedL( TBool aIsForeground );
    /** @see TLcVtStateBase */
    //virtual void HandleLayoutChangedL();

    /** @see TLcVtStateBase */
    //virtual void StartDtmfTone( const TChar& aTone );

    /** @see TLcVtStateBase */
    //virtual void StopDtmfTone();

    /** @see TLcVtStateBase */
    virtual void OpenL();

private: // from TLcVtStateBase
    /** @see TLcVtStateBase */
    virtual void Close();

private: // constructors and destructors

    // c++ constructor
    TLcVtStateRunning(
        MLcVtStateContext& aCtx,
        TLcVtStates& aUiStates );

    // Destructor
    virtual ~TLcVtStateRunning();

private: // new functions

    /** Requests call handling subsystem to initiate a voice call.
     * Starts shutdown of the application.
     */
    void CreateVoiceCallL();

    /** Dials emergency call and starts shutdown */
    void DialEmergencyCallL();
    
    void ShutdownWithEngineCommandL( const TInt aCommand );

private: // data members
    // singleton
    static TLcVtStateRunning* iInstance;
    
    };

/**
*  Application state that is active during shutdown.
*
*  @since S60 v3.2
*/
class TLcVtStateResetting : public TLcVtStateBase
    {
public:
    /**
    * returns singleton instance of TLcVtStateResetting
    * @param aCtx state context
    * @param aUiStates UI states
    * @param aCommand engine command to send when resetting state activates
    * @return singleton
    */
    static TLcVtStateResetting* InstanceL(
        MLcVtStateContext& aCtx,
        TLcVtStates& aUiStates,
        const TVtEngCommandId aCommand = KVtEngCommandNone );

public: // from TLcVtStateBase

    /** @see TLcVtStateBase */
    virtual void LcVtStartupFailedL();

    /** @see TLcVtStateBase */
    virtual void ShutdownDoneL();

    /** @see TLcVtStateBase */
    virtual TEventResponse HandleVTCommandPerformedL(
        TVtEngCommandId aCommand,
        const TInt aError );

    /** @see TLcVtStateBase */
    virtual TEventResponse HandleVtEventL( TInt aEvent );

    /** @see TLcVtStateBase */
    virtual TEventResponse HandleCommandL( const TInt aCommand );

    /** @see TLcVtStateBase */
    virtual TBool HandleForegroundChangedL( TBool aIsForeground );

    /** @see TLcVtStateBase */
    virtual TBool PreHandleForegroundChangedL( TBool aIsForeground );

    /** @see TLcVtStateBase */
    //virtual void HandleLayoutChangedL();

private: // from TLcVtStateBase

    /** @see TLcVtStateBase */
    virtual void OpenL();

    /** @see TLcVtStateBase */
    virtual void Close();

private: // constructors and destructors

    // c++ constructor
    TLcVtStateResetting(
        MLcVtStateContext& aCtx,
        TLcVtStates& aUiStates,
        const TVtEngCommandId aCommand = KVtEngCommandNone );

    // Destructor
    virtual ~TLcVtStateResetting();

private: // data members

    // Singleton
    static TLcVtStateResetting* iInstance;
    
    // Used in user originated shutdown (end active call/switch to voice)
    TVtEngCommandId iCommand;
    };

#endif // T_LCVTPLUGINSTATES_H
