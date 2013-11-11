/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the MPEMessageSender class.
*
*/



#ifndef MPEMESSAGESENDER_H
#define MPEMESSAGESENDER_H

//INCLUDES
#include <pevirtualengine.h>

// CLASS DECLARATION

/**
*  Abstract base class for message sender.
*
*  @lib CallHandling.lib
*/
class MPEMessageSender
    {
    public:

        /**
        * Reroutes error message
        * @param aErrorCode, the error code stored after previous unsuccessful request
        * @param aCallId
        */
        virtual void SendErrorMessage( 
            const TInt aCallId, 
            const TInt aErrorCode ) = 0;

        /**
        * Reroutes call messages
        * @param aMessage The message code of the message
        */    
        virtual void SendMessage( 
            const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
            const TInt aCallId ) = 0;
            
        /**
        * Reroutes call messages
        * @param aMessage The message code of the message
        * @param aName Name of call fill be maped to call id
        */  
        virtual void SendMessage( 
            const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
            const TName& aName ) = 0;
        
        /**
         * Reroutes call messages.
         * @param aMessage specifies the event that has taken place
         */
        virtual void HandleInternalMessage(
            TInt aMessage ) = 0;
    };

#endif // MPEMESSAGESENDER_H

//End of file
