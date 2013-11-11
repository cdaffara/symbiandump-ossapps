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
* Description:  Class responsible for application exit.
*
*/


#ifndef C_VTUIAPPSHUTTER_H
#define C_VTUIAPPSHUTTER_H

#include <babitflags.h>

#include    <mphcltemergencycallobserver.h>
#include    <cphcltemergencycall.h>
#include    <rphcltserver.h>

class CAsyncCallBack;
class MLcVtShutterObserver;
class MLcVtStateContext;

/**
* Shutdown service for the application. This is singleton
* which deletes itself when shutdown is complete.
*
* @since S60 v3.2
*/                       
class CLcVtAppShutter : public CBase
    {
public:          

    /**
    * Returns the instance of application shutter.
    *
    * @param aComms for sending call ending related commands to Phone.
    * @param aObserver observer informed when shutdown is completed.
    * @return the application shutter
    */
    static CLcVtAppShutter* InstanceL( 
        MLcVtShutterObserver& aObserver );

    /**
    *  Starts shutdown.
    */
    void StartShutdown();

    /**
    * Starts shutdown because emergency call was requested by the user.
    * If supplied number is detected other than emergency number this method
    * leaves KErrArgument.
    * @param aStateContext for accessing emergency number and local
    * variation.
    */
    void ShutdownWithEmergencyCallL( MLcVtStateContext& aStateContext );
                               
    /**
    * Callback from emergency caller.
    */
    void EmergencyCallDoneL();
                                   
private:

    /**
    * Exits the application if no pending shutdown preparation
    * is ongoing. Deletes itself as last action.
    */
    void ShutdownIfReadyAndDestroy();

    
    /**
    * Enques async callback.
    */
    void EnqueCallbackL( TCallBack& aCallback );

    /**
    * Asynch callback handling response to emergency call creation.
    */
    static TInt EmergencyResponseCallback( TAny* aAny );

    /** C++ constructor */
    CLcVtAppShutter( MLcVtShutterObserver& aObserver );

    /** destructor */
    ~CLcVtAppShutter();

private: // for making shutdown with emergency call
    class CEmergencyCaller : public CBase, private MPhCltEmergencyCallObserver
        {
        public:         
            /**
            * Dials emergency call. Object destroys itself when
            * response is received.
            */
            static CEmergencyCaller* DialEmergencyL(
                CLcVtAppShutter& aObserver,
                MLcVtStateContext& aStateContext );
            
            // Destructor
            ~CEmergencyCaller();
            
        private: // C++ constructor
            
            CEmergencyCaller( 
                CLcVtAppShutter& aObserver, 
                MLcVtStateContext& aStateContext );
            
            /**
            * 2nd phase constructor
            */
            void ConstructL();
            
        public: // from MPhCltEmergencyCallObserver       
            /**
            * @see  MPhCltEmergencyCallObserver::HandleEmergencyDialL
            */
            virtual void HandleEmergencyDialL( const TInt aStatus );

        private: // Data

            // Emergency number
            TPhCltTelephoneNumber iNumber;             
            
            // Phone Server session
            RPhCltServer          iServer;
            
            // Emergency call handler
            CPhCltEmergencyCall*  iEmergency;
            
            // Observer for callback
            CLcVtAppShutter&      iObserver;

            // State context for gettin entered numbers and local variation
            MLcVtStateContext&    iStateContext;
        };
private: // Data

    /**
    * Guard flags for completing shutdown.
    * All must be cleared before shutdown
    * can be completed.
    */
    enum TShutterFlags 
        {                               
        /** Waiting for call on StartShutdown() */
        EShutterWaitingStart,
        /** Waiting for callback on emegency call request */
        EShutterWaitingEmergencyCallback,
        };
    
    // Asynch callback
    CAsyncCallBack* iCallback;
    
    // guard flags          
    TBitFlags8 iPendingStateFlags;
     
    // Observer to notify when application exit
    // is allowed.
    MLcVtShutterObserver& iObserver;

    // Handles emergency call dialling    
    CEmergencyCaller* iEmergencyCaller;

    };

#endif // C_VTUIAPPSHUTTER_H
