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
*     See class description.
*
*/

#ifndef __MPHONEUISTATEMACHINEFACTORY_H
#define __MPHONEUISTATEMACHINEFACTORY_H

//  INCLUDES
#include <e32std.h>


// FORWARD DECLARATIONS

class CPhoneUIController;
class MPhoneStateMachine;
class CPhoneResourceResolverBase;
class CPhoneErrorMessagesHandler;

class MPhoneViewCommandHandle;

/**
*  Class MPhoneUIStateMachineFactory : This abstract M-class defines the 
*  interface for PhoneUiStateMachine factory. The interface provides the 
*  abstraction of variant specific factory objects. The interface 
*  consists of methods that can be used for creating instances of variant 
*  specific classes.
*/
class MPhoneUIStateMachineFactory
    {
    public:
        /**
        * Destructor
        */
        virtual ~MPhoneUIStateMachineFactory(){};

        /**
        * Creates an instance of CPhoneStateMachine
        * derived class using the loaded factory DLL. The user of this function
        * is responsible of the cleanup of the created object.
        * @return an instance of the class upcasted to CPhoneStateMachine
        */
        virtual MPhoneStateMachine* CreatePhoneStateMachineL(
            MPhoneViewCommandHandle* aViewCommandHandle ) = 0;

        /**
        * Creates an instance of CPhoneResourceResolverBase
        * derived class. The user of this function is responsible of cleanup of the created object.
        * @param None.
        * @return an instance of class upcasted to CPhoneResourceResolverBase class
        */
        virtual CPhoneResourceResolverBase* CreatePhoneResourceResolverL() = 0;
        
        /**
        * Creates an instance of CPhoneErrorMessagesHandler
        * derived class. The user of this function is responsible of cleanup of the created object.
        * @param None.
        * @return an instance of class upcasted to CPhoneErrorMessagesHandler class
        */
        virtual CPhoneErrorMessagesHandler* CreatePhoneErrorMessagesHandlerL(
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneStateMachine* aStateMachine ) = 0;
    };

#endif      // __MPHONEUISTATEMACHINEFACTORY_H
    
// End of File
