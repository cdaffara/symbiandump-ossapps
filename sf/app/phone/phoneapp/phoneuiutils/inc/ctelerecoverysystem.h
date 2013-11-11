/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Recovery system class.
*
*/



#ifndef CTeleRecoverySystem_H
#define CTeleRecoverySystem_H

//  INCLUDES
#include <e32base.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class CPhoneTimer;
class CTeleWaitingIdle;

// DATA TYPES
typedef TInt TPhoneRecoveryId;
typedef TInt8 TPhoneRecoveryPriority;

// Template class for late construct
// User guide:
// In header file:
// class A
//     {
//     public:
//         static A* NewL( CTeleRecoverySystem& aRecoverySystem );
//         void ~A();
//         void EarlyDestruct();
//     private:
//         A( CTeleRecoverySystem& aRecoverySystem );
//         void ConstructL();
//         void LateConstructL();
//         static TInt LateConstructCallBackL( TAny* aAny );
//     private:
//         CTeleRecoverySystem& iRecoverySystem;
//         TInt iLateConstructId;
//     }
// 
// In cpp file
// A* A::NewL( CPhoneRecoverySstem& aRecoverySystem )
//     {
//     A* self = new ( ELeave ) A( aRecoverySystem );
//     CleanupStack::PushL( A );
//     A->ConstructL();
//     CleanupStack::Pop();
//     }
// void A::~A();
//    {
//    EarlyDestruct();
//    iRecoverySystem.Remove( iLateConstructId );
//    }
// void A::EarlyDestruct()
//    {
//    //do all the construct
//    }
// A::A( CTeleRecoverySystem& aRecoverySystem ) 
//    : iReoverySystem( aRecoverySystem )
//    {
//    }
// void A::ConstructL()
//    {
//    // Add everything related to recovery system. 
//    // If EStateWaiting is used, make sure in the callback 
//    // function, the system may not be constructed yet.
//    iRecoverySystem.AddL( TCallBack( LateConstructCallBackL, this ),
//        CTeleRecoverySystem::EPhonePriorityStandard,
//        CTeleRecoverySystem::EStateWaiting );
//    }
// void A::LateConstructL()
//    {
//    //Do the real construct
//    //Note! CTeleRecoverySystem::AddL() shouldn't be called in this function.
//    }
// TInt A::LateConstructCallBackL( TAny* aAny )
//    {
//    A* self = reinterpret_cast< A* >( aAny );
//    CleanupEarlyDestructPushL( *self );
//    self->LateConstructL();
//    CleanupStack::Pop();
//    }
// 
// Call Resume() from time to time. For example in OfferKeyEventL, every 16th
// key press calls Resume().
//
// TKeyResponse A::OfferKeyEventL(...)
//    {
//    iKeyCount++;
//    iKeyCount %= 16;
//    if ( !iKeyCount )
//      {
//      iRecoverySystem.Resume();
//      iKeyCount = 0;
//      }
//    return EKeyWasNotConsumed;
//    }
//      

/**
* Template class to handle late construction.
*/
template <class T>
class CleanupEarlyDestruct
    {
    public:
        
        /**
        * Pushes an item to cleanup stack. 
        * If leave occurs, then EarlyDestruct 
        * method will be called.
        *
        * @param aRef instance of which EarlyDestruct
        *             needs to be called upon leave.
        */
        inline static void PushL( T& aRef );
        
    private:
        
        /**
        * Cleanup action.
        */
        static void Cleanup( TAny* aPtr );
        
    };

/**
* Helper function.
*/ 
template <class T>
inline void CleanupEarlyDestructPushL( T& aRef );

// CLASS DECLARATION

/**
*  If an operation fails, user classes may be leaved to incorrect states. 
*  By using recovery system, failed operation will be tried again some time 
*  later. This may enhance the robustness of user class.
*/
NONSHARABLE_CLASS( CTeleRecoverySystem ): public CActive
    {   
    public:
    // Enumerates priorities:
    // EPriorityDontCare - don't care
    // EPriorityLow - low
    // EPriorityStandard - standard
    // EPriorityHigh - high
    // EPriorityCritical - very critical.
    enum TPhonePriority
        {
        EPhonePriorityDontCare = -10,
        EPhonePriorityLow = -5,
        EPhonePriorityStandard = 0,
        EPhonePriorityHigh = 5,
        EPhonePriorityCritical = 100
        };
        
    // Enumerates states for a recovery item:
    // EStateIdle - solved, everything is fine
    // EStateWaiting - not solved, waiting
    // EStatePending - semipermanently not solved
    // EStateStarting - recovery for all steps started
    // EStateLockedPending - locked to pending state. it's not allowed 
    //                       to be moved back to waiting
    // EStatePrecond - precondition: light idle-phone is reached, 
    //                 but before PIN code query)must be satisfied 
    //                 before this can be run
    // EStatePrecondSim - precondition: SIM card is ready. This happens 
    //                 after PIN code is accepted.
    enum TRecoveryState
        {
        EPhoneStateIdle, 
        EPhoneStateWaiting,  
        EPhoneStatePending,  
        EPhoneStateStarting, 
        EPhoneStateLockedPending, 
        EPhoneStatePrecond, 
        EPhoneStatePrecondSim  
        };

    // Defines null id.
    enum 
        {
        KNullId = -1
        }; 
    
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CTeleRecoverySystem* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CTeleRecoverySystem();

    public: // New functions
               
        /**
        * Adds a new item with given priority & state. Returns Null id if it
        * failed. Same item must not be added twice. Callback's return value
        * indicates how many steps there are to be done, 0 - no more steps. 
        * Error code or leave - failure.
        * Priority indicates importantance of the action.
        *
        * @param aCallBack callback.
        * @param aPriority priority.
        * @param aState start state.
        * @return identifier.
        */
        virtual TPhoneRecoveryId Add( TCallBack aCallBack, 
            TPhoneRecoveryPriority aPriority, 
            TRecoveryState aState );

        /**
        * Add a new item to the recovery system.
        * The function may leave if OOM. 
        *
        * @param aCallBack callback.
        * @param aPriority priority.
        * @param aState start state.
        * @return identifier.
        */
        virtual TPhoneRecoveryId AddL( TCallBack aCallBack,
            TPhoneRecoveryPriority aPriority,
            TRecoveryState aState );
    
        /**
        * Removes item.
        *
        * @param aId identifier of the item to be removed.
        */
        virtual void Remove( TPhoneRecoveryId aId );
    
        /**
        * Sets state. Note: The timer will not be started if the state is
        * set to be waiting.
        *
        * @param aId identifier.
        * @param aState new state.
        */
        virtual void SetState( TPhoneRecoveryId aId, TRecoveryState aState );
    
        /**
        * Resets pending recovery actions back to waiting.
        */
        virtual void ResetPending();
    
        /**
        * Recovers one step or all steps synchronously. Even if it's solved
        * it should call the method.
        *
        * @param aId identifier.
        * @param aPriority priority.
        * @param aAllSteps ETrue if all steps need to be run.
        * @return error code.
        */
        virtual TInt RecoverNow( TPhoneRecoveryId aId, 
            TPhoneRecoveryPriority aPriority, 
            TBool aAllSteps = EFalse );

        /**
        * Informs that precondition has been satisfied.
        */
        virtual void EnablePrecondition();
        
        /**
        * If recovery has been paused due to internal logic, requests
        * recovery to continue. 
        * User class should call this from time to time to improve robustness.
        * For example, user class could call this based on key events it receives.
        */
        virtual void Resume();
    
    public:
    
        /**
        * Recovers all steps synchronously.
        */
        void RecoverAllNow();
        
    public: // Functions from base classes

        /**
        * @see CActive::RunL.
        */
        void RunL();

        /**
        * @see CActive::DoCancel.
        */
        void DoCancel();
        
    private: // private structures
        
        // Defines structure containing information of a recovery item.
        struct TRecoveryItem
            {
            // Identifier
            TPhoneRecoveryId iId;
            // Callback
            TCallBack iCallBack;
            // Priority
            TPhoneRecoveryPriority iPriority;
            // State
            TRecoveryState iState;
            };

    private:

        /**
        * C++ default constructor.
        */
        CTeleRecoverySystem();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Enable precondition Sim OK.
        */
        void EnablePrecondSimOk();

        /**
        * Do item call back
        */
        TInt DoItemCallBack( TRecoveryItem& aItem );

        /**
        * Call back funciton for CPhoneTimer.
        */
        static TInt HandleTimer( TAny* aAny );

        /**
        * Really handle function to handle timer.
        */
        TInt DoHandleTimer();

        /**
        * Call back funciton for AllStepsTimer.
        */
        static TInt HandleAllStepsTimer( TAny* aAny );

        /**
        * Handle AllStesTimer timeout.
        */
        TInt DoHandleAllStepsTimer();

        /**
        * Checks and updates state if precondition has been satisfied.
        */
        void CheckStateForPrecondition( TRecoveryState& aState );

        /**
        * It starts a timer if required when event state changes.
        *
        * @param aNewState new state of the event.
        */
        void StartTimerIfRequired( TRecoveryState aNewState );

        /**
        * It start the timer, but iTickCounter will not be cleared.
        */
        void StartTimerIfIdle();

        /**
        * Start timer.
        */
        void StartTimer();

        /**
        * Starts all steps timer.
        */
        void StartAllStepsTimer();

        /**
        * Starts timer for the next recovery attempt.
        */
        void StartNext( TInt aFound );

        /**
        * Determines which events will be run.
        */
        TInt DetermineRun() const;

        /**
        * Calculates next tick.
        */
        inline TInt NextTick( TInt aTicks ) const;

        /**
        * Finds by recovery id.
        *
        * @param aId id.
        * @return recovery id.
        */
        TInt Find( TPhoneRecoveryId aId ) const;                    
        
        /**
        * Issues new active object request.
        */
        void IssueRequest();
        
        /**
        * Is SIM now ok?
        */
        TBool IsSIMOk();
        
        /**
        * Check for SIM availability. 
        */
        TInt CheckSIMAvailable();
        
        /**
        * Checks if Idle is started.
        */
        TBool IsIdle();
                        
    private:    // Data
    
        // Currently handled request.
        enum TPhoneRequestStatus
            {
            EPhoneNone,
            EPhoneSIMPresent,
            EPhoneSIMStatus
            };

        // Timer for recovery
        CPhoneTimer* iTimer;

        // Timer for recovery all steps
        CPhoneTimer* iAllStepsTimer;
        
        // Timer for idle.
        CTeleWaitingIdle* iWaitingIdle;

        // Recoverty time array
        CArrayFixFlat< TRecoveryItem >* iRecoveryItems;

        // Id counter for TPhoneRecoveryId
        TInt iIdCounter;

        // Count the ticks
        TInt iTickCounter;
        
        // Sim present status. 
        TInt iSimAvailable;

#ifdef _DEBUG
        // Debug: to prevent modification of recovery system in 
        // recovery callback.
        TBool iIsCallBack;
#endif // _DEBUG

        // ETrue if precondition ok
        TBool iPrecondOk;

        // ETrue if precondition SIM ok
        TBool iPrecondSimOk;
        
        // Indicates what request is currently handled.
        TPhoneRequestStatus iRequest;
     
        //  Owned Publish and Subscribe property for SIM status.
        RProperty iSimStatusProperty;
        
        // Owned Publish and Subscribe property for SIM present.
        RProperty iSimPresentProperty;
        
        // Owned Publish and Subscribe property for Idle active.
        RProperty iIdleActive;
    };

// Inline methods.
#include "ctelerecoverysystem.inl"

#endif      // CTELERECOVERYSYSTEM_H

// End of File
