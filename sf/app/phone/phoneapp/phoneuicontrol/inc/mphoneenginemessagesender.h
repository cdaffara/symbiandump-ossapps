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
* Description: 
*     Describes the interface for sending messages to Phone Engine.
*
*/


#ifndef MPHONEENGINEMESSAGESENDER_H
#define MPHONEENGINEMESSAGESENDER_H

/**
*  Describes the interface for sending messages to Phone Engine
*/
class MPhoneEngineMessageSender
    {
    public:
        
        
        /**
        * Send message to Phone Engine
        * @param aMessage
        */
        virtual void SendPhoneEngineMessage( TInt aMessage ) = 0;
  
    };
    
#endif // MPHONEENGINEMESSAGESENDER_H