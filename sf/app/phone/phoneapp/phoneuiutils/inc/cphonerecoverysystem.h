/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Manager to recover important operations.
*
*/


#ifndef CPHONERECOVERYSYSTEM_H
#define CPHONERECOVERYSYSTEM_H

// INCLUDES
#include    <coemain.h>
#include    <e32base.h>
#include    "ctelerecoverysystem.h"

// CONSTANTS
const TInt KNullId = -1;
const TInt KIdContainerSize = 10;

// MACROS     


// DATA TYPES
typedef TInt TPhoneRecoveryId;
typedef TInt8 TPhoneRecoveryPriority;

typedef TPhoneRecoveryId TRecoveryId;
typedef TPhoneRecoveryPriority TRecoveryPriority;



// FORWARD DECLARATIONS
class CPhoneTimer;
//class CTeleRecoverySystem;

// CLASS DECLARATION

/**
*  If an operation fails, phone application may be leaved to incorrect states. 
*  By using recovery system, failed operation will be tried again some time 
*  later. This may enhance the robustness of phone application.
* 
*  Return value from callback function
*  KErrNone: EStateIdle
*  KErrNoMemory, KErrInUse, KErrServerTerminated, KErrServerBusy, KErrNotRead,
*  KErrAccessDenied, KErrLocked, KErrDiskFull, KErrTimedOut: EStateWaiting
*  Otherwise: EStatePending
*  If call back function leaves, then the error of leave is used as return 
*  value.
*  Note: In the recovery callback function, CPhoneRecoverySystem::AddL(...) or
*  CPhoneRecoverySystem::Add(...) should never be called. 
*  
*  @since 1.0
*/
class CPhoneRecoverySystem 
    : public CCoeStatic
    {
    public:  // Constructors and destructor

        /**
        * First call initializes the singleton object. Subsequent calls return
        * instance.
        * @return the created instance.
        */
        IMPORT_C static CPhoneRecoverySystem* Instance();

        /**
        * Destructor.
        */
        ~CPhoneRecoverySystem();
       
    public: // new function

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
        TRecoveryId Add( TCallBack aCallBack, 
            TRecoveryPriority aPriority, 
            CTeleRecoverySystem::TRecoveryState aState );

        /**
        * Add a new item to the recovery system. The function may leave if
        * OOM. 
        *
        * @param aCallBack callback.
        * @param aPriority priority.
        * @param aState start state.
        * @return identifier.
        */
        IMPORT_C TRecoveryId AddL( TCallBack aCallBack,
            TRecoveryPriority aPriority,
            CTeleRecoverySystem::TRecoveryState aState );
    
        /**
        * Removes item.
        *
        * @param aId identifier of the item to be removed.
        */
        IMPORT_C static void Remove( TRecoveryId aId );
    
        /**
        * Sets state. Note: The timer will not be started if the state is
        * set to be waiting.
        *
        * @param aId identifier.
        * @param aState new state.
        */
        void SetState( TRecoveryId aId, 
            CTeleRecoverySystem::TRecoveryState aState );
    
        /**
        * Resets pending recovery actions back to waiting.
        */
        void ResetPending();
    
        /**
        * Recovers one step or all steps synchronously. Even if it's solved
        * it should call the method.
        *
        * @param aId identifier.
        * @param aPriority priority.
        * @param aAllSteps ETrue if all steps need to be run.
        * @return error code.
        */
        IMPORT_C TInt RecoverNow( TRecoveryId aId, 
            TRecoveryPriority aPriority, 
            TBool aAllSteps = EFalse );

        /**
        * Informs that precondition has been satisfied.
        */
        IMPORT_C void EnablePreconditionL();

                        
       /**
        * Create tele recovery system
        */    
        CTeleRecoverySystem* CreateRecoverySystemL();

    private: // New functions

        /**
        * Removes recovery id item.
        *
        * @param aId identifier of the item to be removed.
        */
        void RemoveId( TRecoveryId aId );
    
    private: // private structures
        
        // Defines structure containing information
        // of a recovery item.
        class TRecoveryItem
            {
            public:
                // Identifier
                TRecoveryId iId;
                // Callback
                TCallBack iCallBack;
                // Priority
                TRecoveryPriority iPriority;
                // State
                CTeleRecoverySystem::TRecoveryState iState;
            };

    private: //private function

        /**
        * Two-phased constructor.
        * @return new instance.
        */
        static CPhoneRecoverySystem* NewL();

        /**
        * C++ constructor.
        */
        CPhoneRecoverySystem();
         
        /**
        * Adds id to container which holds all the ids this singleton
        * has given out via AddL method.
        * @param aId - Id to be added in container
        * @exception -1 (KErrNoSpace) is thrown if there isn't room in the container
        */
        void AddIdToContainer( TRecoveryId aId );
        
        /**
        * Removes given id from the container which holds all the ids this singleton
        * has given out via AddL method.
        * @param aId - Id to be removed from container
        */
        void RemoveFromContainer( TRecoveryId aId );
        
        /**
        * This is called from destructor. Method removes all the ids from recovery
        * system that are hold within container. This is done this way because
        * this singleton object is actually destructed by coeenv before some of
        * object's clients' destructor is called where it would try to remove
        * id from the recovery system. Now this way we do not leak any resources.
        */
        void RemoveAllIdsFromContainer();


    private:    // Data  
           
        // Timer for recovery
        CPhoneTimer* iTimer;

        // Timer for recovery all steps
        CPhoneTimer* iAllStepsTimer;

        // Id counter for TRecoveryId
        TInt iIdCounter;

        // Count the ticks
        TInt iTickCounter;

#ifdef _DEBUG
        // Debug: to prevent modification of recovery system in 
        // recovery callback.
        TBool iIsCallBack;
#endif // _DEBUG

        // ETrue if precondition ok
        TBool iPrecondOk;

        // ETrue if precondition SIM ok
        TBool iPrecondSimOk;

        // Keycount - recovery counter is checked only after some keypresses
        TInt iKeyCount;
        
        // Reference to Tele recovery system.
        CTeleRecoverySystem* iRecoverySystem;
        
        // Container for allocated recovery ids
        TFixedArray< TInt, KIdContainerSize > iIdContainer;

    };

#endif      // CPHONERECOVERYSYSTEM_H   
            
// End of File
