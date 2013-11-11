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
*     All possible event types from Phone Engine.
*
*/


#ifndef MPHONEENGINEEVENTS_H
#define MPHONEENGINEEVENTS_H

//  INCLUDES
#include <e32def.h>
#include <pevirtualengine.h>

// CLASS DECLARATION

/**
*  Handle all possible messages from Phone Engine
*/
class MPhoneEngineEvents
    {
    public:
        
        /**
        * Destructor.
        */
        virtual ~MPhoneEngineEvents() {};

        /**
        * A message handling function for Phone Engine messages
        * @param aCallId: the call id of the call
        */
        virtual void HandlePhoneEngineMessageL(
            const TInt aMessage, 
            TInt aCallId ) = 0; 
           
        /**
        * Handles errors codes sent by Phone Engine.
        * @param aErrorInfo 
        */
        virtual void HandleErrorL( 
            const TPEErrorInfo& aErrorInfo ) = 0;
    };

#endif      // MPHONEENGINEEVENTS_H   
            
// End of File
