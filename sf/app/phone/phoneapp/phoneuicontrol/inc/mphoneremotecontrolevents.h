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
*     Interface class for handling key events.
*
*/


#ifndef __MPHONEREMOTECONTROLEVENTS_H
#define __MPHONEREMOTECONTROLEVENTS_H

//  INCLUDES
#include    <remconcoreapitargetobserver.h>

// CLASS DECLARATION
/**
*  Interface class for executing UI commands
*/
class MPhoneRemoteControlEvents
    {
    public:
        /**
        * Destructor
        */
        virtual ~MPhoneRemoteControlEvents(){};

        /**
        * Handles commands from the Remote Control framework.
        * @param aOperationId The operation ID of the command.
        * @param aButtonAct The button action associated with the command.
        * @return ETrue if the event was handled, EFalse otherwise.
        */
        virtual TBool HandleRemConCommandL( 
            TRemConCoreApiOperationId aOperationId, 
            TRemConCoreApiButtonAction aButtonAct) = 0;
         
    };

#endif      // __MPHONEREMOTECONTROLEVENTS_H   
            
// End of File
