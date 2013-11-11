/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Bluetooth audio disconnection handler.
*
*/


#ifndef CPHONEBTAADISCONNECTHANDLER_H
#define CPHONEBTAADISCONNECTHANDLER_H

// INCLUDES
#include <coemain.h>
#include "mphonetimer.h"
 
// CLASS DECLARATION
class CPhoneTimer;
class MPhoneStateMachine;
class MPhoneViewCommandHandle;
 
/**
*  It waits for bluetooth audio disconnection and
*  shows query when it happens.
*
*/
class CPhoneBtaaDisconnectHandler : public CCoeStatic, protected MPhoneTimer
    {
    public:
        
        /**
         * Returns the bluetooth accessory disconnect handler singleton, constructing it if needed.
         * 
         * @return  Bluetooth accessory disconnect handler instance.
         */
        static CPhoneBtaaDisconnectHandler* InstanceL();
        
        /**
        * Handles connection lost situation.
        */
        void HandleConnectionLostL();
 
        /**
        * Handles query dismissal.
        * @param aCommand command selected.
        */
        void HandleQueryDismissL( TInt aCommand );

        /**
        * Cancels current operation.
        */
        void Cancel();
        
        /**
        * Return query status
        */
        TBool IsQuery();
        
    protected: //from base class

        /**
        * @see CPhoneTimer, handles timeout.
        */
        virtual void HandleTimeOutL();
        
    private:

        /**
        * C++ constructor.
        */
        CPhoneBtaaDisconnectHandler();

        /**
        * Destructor.
        */
        ~CPhoneBtaaDisconnectHandler();
        
        /**
        * Lauches query.
        */
        void LaunchQueryL();

        /**
        * Deletes all temporary instances.
        */
        void DeleteAll();

        /**
        * Terminate all calls.
        */
        void TerminateAllCallsL(); 
      
        /**
        * Show query
        * @param aResourceId resource id to be resolved
        */      
        void ShowQueryL( TInt aResourceId );
        
        /**
        * Set mute
        * @param aStatus ETrue if muted else unmute
        */
        void SetMuteL( TBool aStatus );

    protected:  // Data        
        /**
        * Phone UI View command handle
        */
        MPhoneViewCommandHandle* iViewCommandHandle;
        
        // Interface for setting and getting active state
        MPhoneStateMachine* iStateMachine;

    private:    // Data  
       
        /**
        * Enumerates states.
        *
        * EIdle - idle, waiting for disconnection.
        * ELaunchQuery - query to be launched.
        */
        enum TState
            {
            EIdle,
            ELaunchQuery,
            };

        // Current state.
        TState iState;

        // Timer.
        CPhoneTimer* iTimer;
     
        // Old mute state.
        TBool iOldMuted;

        // Indicates if call is supposed to be continued in 
        // handset or loudspeaker. ETrue corresponds to loudspeaker,
        // while EFalse corresponds to handset.
        TBool iVideoCallStatus;

    };

#endif      // CPHONEBTAADISCONNECTHANDLER_H   
            
// End of File
