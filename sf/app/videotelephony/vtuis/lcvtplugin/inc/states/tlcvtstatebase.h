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
* Description:  Base class for application states.
*
*/


#ifndef T_LCVTSTATEBASE_H
#define T_LCVTSTATEBASE_H

#include <e32base.h>
#include <mvtengsessioninfo.h>
#include "mlcvtstatecontext.h"
#include "mlcvtshutterobserver.h"
#include "tlcvtstates.h"

/**
* Base class for application states related to application startup, normal
* operation and shutdown. AppUi delegates decision making by forwarding
* events/commands it receives to current state.
* Not all event handlers declared by the base class are applicable in
* all concrete states. All methods returning TEventResponse imply that it may
* consume the event completely and caller should not process it further.
*
* It should be kept in mind that as new functionality is added to application,
* state classes may need changes if e.g. handling of specific event should be
* allowed in appUi. By default event handler in specific state may consume all
* events. Note that in this context e.g. HandleCommandL is considered as an
* event handler.
*
* @since S60 v3.2
*/
class TLcVtStateBase : public MLcVtShutterObserver
    {
public: // new functions

    /**
    * Return value from state's event handlers.
    */
    enum TEventResponse
        {
        /** Event was handled by a state and should
         *  not be further processed by caller.
         */
        EEventHandled,
        /** Event can be handled by caller. */
        EEventNotHandled
    };

    /**
    * Performs state dependent actions when phase 1 of
    * startup is completed.
    */
    virtual void LcVtStartupPhase1DoneL();

    /**
    * Performs state dependent actions when startup fails.
    */
    virtual void LcVtStartupFailedL();

    /**
    * Performs state dependent actions when phase 2 of
    * startup is completed.
    */
    virtual void LcVtStartupPhase2DoneL();

    /**
    * Handles completion of shutdown.
    */
    virtual void ShutdownDoneL();

    /**
    * Handles completion of an VT engine command.
    * @param aCommand completed command
    * @param aError error
    * @return event response, is caller allowed to handle event
    */
    virtual TEventResponse HandleVTCommandPerformedL(
        const TVtEngCommandId aCommand,
        const TInt aError );

    /**
    * Handles event from the engine.
    * @param aEvent
    * @return event response, is caller allowed to handle event
    */
    virtual TEventResponse HandleVtEventL( const TInt aEvent );

    /**
    * Handles command from the user (or application framework).
    * @param aCommand
    * @return event response, is caller allowed to handle event
    */
    virtual TEventResponse HandleCommandL( const TInt aCommand );

    /**
    * Performs state dependent actions for foreground change event.
    * @param aIsForeground foreground status passed from UI framework
    * @return ETrue if application should behave as in foreground
    */
    virtual TBool HandleForegroundChangedL( const TBool aIsForeground ) = 0;

    /**
    * Performs state dependent previous actions for foreground change event.
    * @param aIsForeground foreground status passed from UI framework
    * @return ETrue if application should behave as in foreground
    */
    virtual TBool PreHandleForegroundChangedL( const TBool aIsForeground ) = 0;
    
    /**
    * Performs state dependent actions for layout change.
    */
    //virtual void HandleLayoutChangedL();    
    
    /**
    * Sends DTMF tone if allowed by current state.
    * @param aTone DTMF tone to send
    */
    //virtual void StartDtmfTone( const TChar& aTone );

    /**
    * Stops DTMF tone if allowed by current state.
    */
    //virtual void StopDtmfTone();

    /**
    * Sets initial application state.
    * @param aCtx state context providing services to states
    * @param aUiStates ui states
    */
    static void SetInitialStateL(
        MLcVtStateContext& aCtx,
        TLcVtStates& aUiStates);

public: // from MLcVtShutterObserver

    /**
    * Handles 'shutdown ready' event.
    */
    virtual void HandleShutdownReady();
    

protected: // for concrete state classes

    /**
    * Destructor, for cleanup.
    */
    virtual ~TLcVtStateBase();

    /**
    * Performs entry action for a state.
    */
    virtual void OpenL() = 0;

    /**
    * Performs exit action for a state and deletes that state.
    */
    virtual void Close() = 0;

    /**
    * Sets current state.
    */
    void ChangeStateL( TLcVtStateBase& aNewState );

    /**
    * Sets transition to resetting state as cleanup item.
    */
    void CleanupResetPushL();

    /**
    * Starts shutdown when leave occurred.
    */
    static void TransitionToReset( TAny* aAny );


    /**
    * Ends the call if necessary and starts shutdown.
    * @return ETrue if shutdown started.
    */
    TBool CheckEndActiveCallL();

    /**
    * Returns engine session state.
    * @param aUpdate is state forced to update
    * @return session state
    */
    MVtEngSessionInfo::TSessionState SessionState(
        const TBool aUpdate ) const;

    /**
    * Delegates engine command execution to the state context.
    */
    template < typename T >
    inline void ExecuteEngineCommand(
        TInt aCommand, T& aParam );

    /**
    * Sets execution state.
    */
    void SetExecState( const TLcVtStates::TLcVtExecState aState );

protected: // constructor

    // c++ constructor
    TLcVtStateBase( MLcVtStateContext& aCtx, TLcVtStates& aUiStates );


private: // new functions

    /**
    * Updates applications lock state and sends to background if
    * the device is locked.
    */
    void HandleDeviceLockEventL( const TBool aDeviceIsLocked );

protected: // data members

    // Context providing services to states
    MLcVtStateContext&  iCtx;

    // Various application specific state information
    TLcVtStates&        iUiStates;

    /** Prevents sending end call command to call handling if ETrue.
     *  It is not allowed when call clearing is network originated.
     */
    
    /**
     * shutdown request received
     */
    static TBool iShutdownRequested;
    
    };

#include "tlcvtstatebase.inl"

#endif // T_LCVTSTATEBASE_H
