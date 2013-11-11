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
*     Factory for creating different kind of Mediator related objects.
*     This factory itself is a Singleton.
*
*/


#ifndef CPHONEMEDIATORFACTORY_H
#define CPHONEMEDIATORFACTORY_H

// INCLUDES
#include <e32base.h>
#include <coemain.h>

// FORWARD DECLARATIONS
class CPhoneMediatorSender;
class CPhoneMediatorCommandListener;
class MPhoneMenuAndCbaEvents;
class MPhoneEngineMessageSender;
class MPEEngineInfo;
class MPhoneMediatorMessage;

// CLASS DECLARATION

class CPhoneMediatorFactory : public CCoeStatic
    {
    public:  

        /**
        * First call initializes the singleton object. Subsequent calls return
        * instance.
        * @param None.
        * @return Pointer to the one and only instance of Phone Mediator 
        *         Sender -object.
        */
        IMPORT_C static CPhoneMediatorFactory* Instance();

        /**
         * Destructor.
         */
        virtual ~CPhoneMediatorFactory();
        
        /**
        * Creates and returns instance of Mediator Sender for sending Mediator
        * Commands and Events.
        * @return pointer to CPhoneMediatorSender.
        */
        IMPORT_C CPhoneMediatorSender* Sender();
        
        /**
        * Returns instance of Mediator Command Listener.
        * When called for the very first time parameters are needed to construct
        * the Singleton object. After that providing parameters aren't needed.
        */
        IMPORT_C CPhoneMediatorCommandListener* CommandListener( 
            MPhoneMenuAndCbaEvents* aMenuAndCbaEventHandler = NULL,
            MPhoneEngineMessageSender* aMessageSender = NULL,
            MPEEngineInfo* aEngineInfo = NULL );

        /**
        * Returns instance of Mediator Message.         
        * @param aMessage - Message from Phone Engine (EPEMessage).
        * @param aCallId - Caller id
        * @return MPhoneMediatorMessage object.
        */
        IMPORT_C MPhoneMediatorMessage* MediatorMessage(
            const TInt aMessage, TInt aCallId );
        

    private:

        /**
        * Instantiates this class and returns a pointer to us
        */
        static CPhoneMediatorFactory* NewL();

        /**    
         * Protected constructor because of Singleton Pattern
         */    
        CPhoneMediatorFactory();  
        
        /**
         * By default EPOC constructor is private.
         */
        void ConstructL();
        
             
    private:
        
        // Not owned
        MPhoneMenuAndCbaEvents* iMenuAndCbaEvents;
        
        // Not owned
        MPhoneEngineMessageSender* iMessageSender;
        
        // Not owned
        MPEEngineInfo* iEngineInfo;

    };
    
#endif // CPHONEMEDIATORFACTORY_H

// End of File
