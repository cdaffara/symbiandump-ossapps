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
*     Listens Commands via Mediator component.
*
*/


#ifndef CPHONEMEDIATORCOMMANDLISTENER_H
#define CPHONEMEDIATORCOMMANDLISTENER_H

// INCLUDES
#include <e32base.h>
#include <coemain.h>
#include <MediatorCommandResponder.h>

// FORWARD DECLARATIONS
class MPhoneMenuAndCbaEvents;
class MPhoneEngineMessageSender;
class MPEEngineInfo;

// CLASS DECLARATION

class CPhoneMediatorCommandListener : public CCoeStatic, 
                                      public MMediatorCommandObserver
    {
    public:  

        /**
        * First call initializes the singleton object. Subsequent calls return
        * instance.
        * @param None.
        * @return Pointer to the one and only instance of Phone Mediator 
        *         Command Listener -object.
        */
        static CPhoneMediatorCommandListener* Instance();

        /**
        * Destructor.
        */
        virtual ~CPhoneMediatorCommandListener();
        
        /**
        * Initializes command listener singleton for use. Mandotory to call before first use.
        * @param aMenuAndCbaEventHandler - Interface to receive converted events.
        * @param aMessageSender - Interface for sending messages
        * @param aEngineInfo - Interface for setting Phone Engine specific information
        */
        void Initialize( 
            MPhoneMenuAndCbaEvents* aMenuAndCbaEventHandler,
            MPhoneEngineMessageSender* aMessageSender,
            MPEEngineInfo* aEngineInfo );
        
        /**
        * A Mediator Service command.
        *
        * @since Series 60 3.1     
        * @param aDomain    The identifier of the domain         
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.
        * @param aVersion   The version information of the command.
        * @param aData      The parameters of the command.
        * @return None.
        */
        void MediatorCommandL( TUid aDomain,
                               TUid aCategory, 
                               TInt aCommandId,
                               TVersion aVersion, 
                               const TDesC8& aData );

        /**
        * Cancel a Mediator Service command.
        *
        * @since Series 60 3.1        
        * @param aDomain    The identifier of the domain      
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.        
        * @return None.
        */
        void CancelMediatorCommand( TUid aDomain,
                                    TUid aCategory, 
                                    TInt aCommandId );


    private:

        /**    
         * Protected constructor because of Singleton Pattern
         */    
        CPhoneMediatorCommandListener();  

        /**
        * Instantiates this class and returns a pointer to us
        */
        static CPhoneMediatorCommandListener* NewL();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();
        
        /**
        * Registers all Mediator Commands that this class listens to.
        */
        void RegisterMediatorCommandsL();
        
        /**
        * Registers Audio Mediator Commands.
        */
        void RegisterAudioMediatorCommands();

        /**
        * Registers Generic Mediator Commands.
        */
        void RegisterGenericMediatorCommands();
        
        /**
        * Registers Video Telephony specific Mediator Commands.
        */
        void RegisterVideoTelephonyMediatorCommands();
        
        /**
        * Sends Mediator Response for the received Command.
        * @param aDomain - The identifier of the domain.
        * @param aCategory - The identifier of the category.
        * @param aCommandId - The identifier of the command.
        */
        void SendResponse( TUid aDomain, TUid aCategory, TInt aCommandId );
        
        /**
        * Handles Video Telephony specific Mediator Commands.
        * @param aCommandId - The identifier of the command
        * @param aVersion - Used interface version
        */
        void VideoTelephonyCommandL( TInt aCommandId, TVersion aVersion );
        
        /**
        * Handles Audio specific Mediator Commands.
        * @param aCommandId - The identifier of the command
        * @param aVersion - Used interface version
        */
        void AudioCommandL( TInt aCommandId, TVersion aVersion );

        /**
        * Handles generic Mediator Commands.
        * @param aCommandId - The identifier of the command
        * @param aVersion - Used interface version
        */
        void GenericCommandL( TInt aCommandId, TVersion aVersion );
                                
        
    private:
    
        // For mapping commands to CBA and Menu selections
        MPhoneMenuAndCbaEvents* iMenuAndCbaHandler;
        
        // For mapping commands to Phone Engine messages
        MPhoneEngineMessageSender* iMessageSender;

        // For setting needed Phone Engine information
        MPEEngineInfo* iEngineInfo;
        
        CMediatorCommandResponder* iCommandResponder;
        MediatorService::RCommandList iVideoTelCommands;
        MediatorService::RCommandList iAudioCommands;
        MediatorService::RCommandList iGenericCommands;
    };
    
#endif // CPHONEMEDIATORCOMMANDLISTENER_H

// End of File
