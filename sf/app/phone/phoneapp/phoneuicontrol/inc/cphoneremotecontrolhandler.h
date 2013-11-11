/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Class for handling messages from Engine and from all inputs to the
*     states.
*
*/


#ifndef __CPHONEREMOTECONTROLHANDLER_H
#define __CPHONEREMOTECONTROLHANDLER_H

// INCLUDES
#include <remconcoreapitargetobserver.h>
#include <e32base.h>
#include "mphonestatemachine.h"


// FORWARD DECLARATIONS
class CRemConInterfaceSelector;  
class CRemConCoreApiTarget;
class MPhoneStateMachine;
class CPhoneHandler;

// CLASS DECLARATION

/**
* Class for handling messages from Engine and from all inputs to the states
*/
class CPhoneRemoteControlHandler : 
    public CBase,
    public MRemConCoreApiTargetObserver
    {
    public: 

        /**
        * Two-phased constructor
        * @param aStateMachine: reference to Phone UI state machine
        * @return an instance of class CPhoneRemoteControlHandler
        */
        static CPhoneRemoteControlHandler* NewL(
            MPhoneStateMachine* aStateMachine );
        /*
        * Destructor.
        */
        virtual ~CPhoneRemoteControlHandler();
        
    protected:

        /**
        * @see MRemConCoreApiTargetObserver.
        *
        * A command has been received. 
        * @param aOperationId The operation ID of the command.
        * @param aButtonAct The button action associated with the command.
        */
        void MrccatoCommand( 
            TRemConCoreApiOperationId aOperationId, 
            TRemConCoreApiButtonAction aButtonAct);
        
    private:

        /**
        * By default EPOC constructor is private.
        */
        CPhoneRemoteControlHandler( MPhoneStateMachine* aStateMachine );
        
        /**
        * ConstructL()
        */
        virtual void ConstructL();

        /**
        * Catch button timer event. Current remote operation is repeated
        * @param aAny
        */
        static TInt DoHandleButtonRepeat( TAny* aAny );
        
    private: // Data  
        
        /** Remote Controller */
        CRemConInterfaceSelector* iInterfaceSelector;  
        
        /** Remote Controller */
        CRemConCoreApiTarget* iCoreTarget;

        /** Current Remote Controller operation */
        TRemConCoreApiOperationId iOperationId;
        
        /** Current Remote Controller button act */
        TRemConCoreApiButtonAction iButtonAct;

        /** Button repeat timer */
        CPeriodic* iButtonRepeatTimer;
        
        /**
        * Handle to State machine
        */
        MPhoneStateMachine* iStateMachine;
        
        /** Phone Handler */
        CPhoneHandler*  iPhoneHandler;

    };

#endif      // __CPhoneRemoteControlHandler_H
            
// End of File
