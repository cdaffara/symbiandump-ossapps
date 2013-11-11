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
* Description:  Interface for state objects to request application services.
*
*/


#ifndef M_MLCVTSTATECONTEXT_H
#define M_MLCVTSTATECONTEXT_H

#include <e32def.h> 
#include <vtengcommands.h>
#include "mlcvtshutterobserver.h"

class TLcVtStateBase;
class CVtEngModel;

/**
* Provides service interface for vt plugin state objects.
*
* @since S60 v3.2
*/
class MLcVtStateContext : public MLcVtShutterObserver
    {
    public:
                
        /**
        * Starts first phase of startup when prepare is received.
        */
        virtual void StartupPhase1L() = 0;
        
        /**
        * Starts application shutdown.
        */
        virtual void ShutdownL() = 0;
        
        /**
        * Starts second phase of startup.
        */
        virtual void StartupPhase2L() = 0;
        
        /**
        * Sets application to foreground and background.            
        * @param aForeground ETrue => foreground, EFalse => background
        */
        virtual void ChangeApplicationFocus( const TBool aForeground ) = 0;
        
        /**
        * Changes current application state.
        * @param aState new state
        */
        virtual void ChangeState( TLcVtStateBase* aState ) = 0;
        
        /**
        * Starts DTMF tone.
        * @param aTone DTMF tone.
        */
        //virtual void StartDtmfTone( const TChar& aTone ) = 0;
        
        /**
        * Stops tarts DTMF tone.
        */
        //virtual void StopDtmfTone() = 0;
        
        /**
        * Returns model.
        * @return model
        */
        virtual CVtEngModel& Model() = 0;
                        
        /**
        * Returns source containing numbers typed by the user.
        * @return number soure
        */
        virtual TDesC* NumberSource() const = 0;
        
        /**
        * Executes engine command.
        * @param aCommand command id
        * @param aParams command params
        */
        virtual void DoExecuteCmdL( const TVtEngCommandId aCommand,
                TDesC8* aParams ) = 0;
        
        /**
        * Refreshes application UI.
        * @param aRefreshFlags identifies what to refresh
        */
        virtual void RefreshL( const TInt aRefreshFlags ) = 0;          
        
        /**
        * Updates engine states.
        */
        virtual void  RefreshStatesL() = 0;

        /**
        * Handles layout change.
        */
        //virtual void DoHandleLayoutChangedL() = 0;

        /**
        * Sets call id.
        */
        virtual void SetCallIdL( const TInt aCallId ) = 0;
        
        /**
        * Sets display text of remote end.
        */
        virtual TBool SetCallNameL( const TDesC& aName ) = 0;
        
        /**
        * Sets call number text of remote end.
        */
        virtual void SetCallNumberL( const TDesC& aNumber ) = 0;
        
        /**
         * Start shutdown process
         */
        virtual void StartShutdown() = 0;

        
        /**
        * Refresh flags
        */        
        enum TRefreshFlags
            {
            /** Navipane is refreshed */
            ENaviPane       = 0x01,
            /** Softkeys are refreshed */
            ESoftkeys       = 0x02,
            /** Refreshes blind (video availability) status */
            EBlind          = 0x04,
            /** Refreshes state sync with call handling */
            ESynchronise    = 0x08,
            /** Refreshes internal states (media etc) */
            EUiStates       = 0x10,
            /** Refreshes everything */
            EAll            = ENaviPane | 
                              ESoftkeys | 
                              EBlind | 
                              ESynchronise | 
                              EUiStates
            };
    };

    
#endif // M_MLCVTSTATECONTEXT_H
