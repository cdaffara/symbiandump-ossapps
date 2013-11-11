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
* Description:  Video Telephony mediator plugin header
*
*/


#ifndef C_VTMEDIATORPLUGIN_H
#define C_VTMEDIATORPLUGIN_H

// INCLUDES
#include <MediatorPluginBase.h>
#include <MediatorNotifications.h>
#include <MediatorEventConsumer.h>
#include <MediatorEventProvider.h>
#include <MediatorCommandResponder.h>
#include <vtinternalmediatorapi.h>
#include <mediatoreventsfromtelephonyapi.h>
#include <MediatorCommandInitiator.h>
#include <w32std.h>



using namespace TelephonyMediator;
using namespace MediatorService;

class TelephonyMediator::TTelephonyCallDataParam;
class CMediatorCommandInitiator;
/**
* Mediator plugin for CS video telephony. Fullfills responsibilities:
*   - launches Video Telephone application
*   - caches CLI and Dataport information for VT app for time between
*     the data is broadcasted to time VT is ready to receive it.                                                                 
* @since S60 5.0
*/

class CVtMediatorPlugin : public CMediatorPluginBase, 
                            public MMediatorNotifications,
                            public MMediatorEventObserver,
                            public MMediatorCommandObserver,
                            public MMediatorCommandResponseObserver
	{
public:          
       
     static CMediatorPluginBase* NewL();
     
    /**
     * Destructor.
     */
    ~CVtMediatorPlugin();
    
public: // From Base class
    
    virtual void StartL();
    
public: // From MMediatorNotifications

    /**
    * Raises VT specific CLI/dataport event when event is registered by VT app,
    * or when the information is available
    *
    * @since Series 60 3.1
    * @param aDomain    The identifier of the domain.
    * @param aCategory  The identifier of the category.
    * @param aEvents    The list of events that have been registered.
    * @return None.
    */
    virtual void MediatorEventsAddedL( TUid aDomain, 
                                       TUid aCategory, 
                                       const REventList& aEvents );

    /**
    * New commands have been registered. The commands can now be subscribed.
    * The list can contain 1-n new commands into the category.
    *
    * @since Series 60 3.1
    * @param aDomain    The identifier of the domain.
    * @param aCategory  The identifier of the category.
    * @param aCommands  List of commands that have been registered.
    * @return None.
    */
    virtual void MediatorCommandsAddedL( TUid aDomain, 
                                         TUid aCategory,
                                         const RCommandList& aCommands );
     
    /**
    * A category has been removed. All subscriptions are cancelled.
    *
    * @since Series 60 3.1
    * @param aDomain    The identifier of the domain.
    * @param aCategory  The identifier of the category.        
    * @return None.
    */
    virtual void MediatorCategoryRemovedL( TUid aDomain, TUid aCategory );

    /**
    * List of events has been removed. All subscriptions are cancelled.
    * List can contain 1-n events from the specified category.
    *
    * @since Series 60 3.1
    * @param aDomain    The identifier of the domain.
    * @param aCategory  The identifier of the category.
    * @param aEvents    The list of events removed.
    * @return None.
    */
    virtual void MediatorEventsRemovedL( TUid aDomain, 
                                         TUid aCategory,
                                         const REventList& aEvents );
                                         
    /**
    * List of commands have been removed. All subscriptions are cancelled.
    * List can contain 1-n commands from the specified category.
    *
    * @since Series 60 3.1
    * @param aDomain    The identifier of the domain.
    * @param aCategory  The identifier of the category.
    * @param aCommands  The list of commands removed
    * @return None.
    */
    virtual void MediatorCommandsRemovedL( TUid aDomain, 
                                           TUid aCategory,
                                           const MediatorService::RCommandList& aCommands ); 
         
public: // from MMediatorCommandObserver
    
    /**
    * @see MMediatorCommandObserver::MediatorCommandL       
    */
    virtual void MediatorCommandL( TUid aDomain,
                                 TUid aCategory, 
                                 TInt aCommandId,
                                 TVersion aVersion, 
                                 const TDesC8& aData );
    
    /**
    * @see MMediatorCommandObserver::CancelMediatorCommand       
    */
    virtual void CancelMediatorCommand( TUid aDomain,
                                      TUid aCategory, 
                                      TInt aCommandId );
    
public:// from MMediatorCommandResponseObserver
    /**
    * @see MMediatorCommandResponseObserver::CommandResponseL
    */
     virtual void CommandResponseL( TUid aDomain,
                                   TUid aCategory, 
                                   TInt aCommandId,
                                   TInt aStatus, 
                                   const TDesC8& aData );  
                                               
public: // from MMediatorEventObserver
                                                   
        /**
        * A Mediator Service event.
        *
        * @since Series 60 3.1        
        * @param aDomain    The identifier of the domain.
        * @param aCategory  The identifier of the category.
        * @param aEventId   The identifier of the event.
        * @param aData      The parameters of the event.
        * @return None.
        */
        virtual void MediatorEventL( TUid aDomain,
                                     TUid aCategory, 
                                     TInt aEventId, 
                                     const TDesC8& aData );                                               

public: // new
    static TInt EventRaiserCallback( TAny* aAny );
    
private: // constructors
    
    CVtMediatorPlugin();      void ConstructL();

    enum TInternalState {
        /** 
         * Waiting for video call state to reach value that is suitable
         * for launching Video Telephone application. In this state
         * video call is not ongoing.
         */
        EWaitingAppLaunch,
        
        /** 
         * Waiting for VT application's readiness to receive video call info
         * events. In this state video call is ongoing and VT application
         * is starting up.
         */
        EWaitingEventRegistration,
        
        /** 
         * VT application is ready to receive video call info events.
         */
        EReady
    };
    
private: // new functions    
    void ChangeCallDataEventSubscriptionL(
        const REventList& aEvents,
        const TBool aEventRegistered );
    
    void HandleVtCommandUnregistrationL();
    
    void HandleVtCommandRegistrationL();
    
    void LaunchVtAppL();
    
    void SaveCallData( const TelephonyMediator::TTelephonyCallDataParam& aData, TVtVideoTelephonyCallInformation& iVtCallInfo);
                         
    void HandleDataportCommandL( const TDesC8& aData );
                             
    void ClearData(TVtVideoTelephonyCallInformation& aVtCallInfo, TCallState& aCallState);
    
    void RegisterVtInternalEventL();
    //test
    void RegisterVtInternalEvent_2L();
    
    void RegisterDataportCommandL();
    
    void RaiseVtEventL();    
                                    
    /**
     * Starts VT application if video call state transition is suitable.
     */                                    
    void HandleCallStateChangeL( const TelephonyMediator::TTelephonyCallDataParam& aData ); 
    
    TBool IsEnoughMemory();
    
    void FreeRam();
    
    void StopDeathActiveL();
    
private: 
    // for detecting subscription to video call information from VT app.
    CMediatorNotifications* iMediatorNotifications;

    // for sending video call information events
    CMediatorEventProvider* iEventProvider;
                                                            
    // for receiving CLI from Telephony
    CMediatorEventConsumer* iEventConsumer;

    // for responding to dataport/releasedataport command from Telephony
    CMediatorCommandResponder* iCommandResponder;
    
    // for issue command to vt app
    CMediatorCommandInitiator* iCommandInitiator;
    
    // supported mediator commands
    MediatorService::RCommandList iCommandList;
    
    // plugin's internal state 
    TInternalState iState;
    
    // cached active video call state (from last call data event from Telephony)                    
    TCallState iActiveVideoCallState;
        
    TVtVideoTelephonyCallInformation iActiveVideoCallInfo;
    
    // cached waiting video call state (from last call data event from Telephony)                    
    TCallState iWaitingVideoCallState;
           
    TVtVideoTelephonyCallInformation iWaitingVideoCallInfo;
    
    // for raising event to VT asynchronously when it starts up
    CAsyncCallBack* iCallBack;
    
    class CAppDeathActive;
    // Owned application death waiter.
    CAppDeathActive* iAppDeathActive;
    
    // Owned application thread.
    RThread iAppThread;    
    
    // Application thread id.
    TThreadId iAppThreadId;
    
    /**
        * Active object for waiting thread death.
        *
        * @since 2.6
        * @lib videotelcomms.lib
        */
    NONSHARABLE_CLASS( CAppDeathActive ) : public CActive
         {
         public: // Constructors
              
             /**
             * Constructor.
             */
             CAppDeathActive( CVtMediatorPlugin& aMediatorPlugin, 
                     RThread& aAppThread );
              
             /**
             * Destructor.
             */
             ~CAppDeathActive();

         public: // New functions

             /**
             * Starts active object.
             */
             void Start();

         public: // From base classes

             /**
             * @see CActive::RunL.
             */
             virtual void RunL();
                
             /**
             * @see CActive::DoCancel.
             */
             virtual void DoCancel();
                
             /**
             * @see CActive::RunError.
             */
             virtual TInt RunError( TInt aError );

         private:

             // Reference to starter.
             CVtMediatorPlugin& iMediatorPlugin;

             // Reference to thread.
             RThread& iAppThread;

         };
    
    };

#endif // C_VTMEDIATORPLUGIN_H

