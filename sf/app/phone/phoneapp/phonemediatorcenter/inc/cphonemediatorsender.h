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
*     Sends Events and Commands via Mediator component.
*
*/


#ifndef CPHONEMEDIATORSENDER_H
#define CPHONEMEDIATORSENDER_H

// INCLUDES
#include <e32base.h>
#include <coemain.h>
#include <MediatorEventProvider.h>
#include <MediatorCommandInitiator.h>
#include <mediatoreventsfromtelephonyapi.h>
#include "phoneviewcommanddefinitions.h"
#include "tphonecommandparam.h"

// FORWARD DECLARATIONS
class CMediatorEventProvider;
class MPhoneEngineMessageSender;
class MPhoneShutdownCommand;

using namespace TelephonyMediator;

/**
* Struct for buffering mediator command.
* In a case when several commands are sent to Mediator
* we resend the last used command after receiving a
* Mediator response.
*/
struct TPhoneCommandBuffer
    {
    TInt iCommandId;
    TUid iDomainUid;
    TUid iCategoryUid;
    TVersion iVersion;        
    };

// CLASS DECLARATION

class CPhoneMediatorSender : public CCoeStatic, private MMediatorCommandResponseObserver
    {
    public:  

        /**
        * First call initializes the singleton object. Subsequent calls return
        * instance.
        * @param None.
        * @return Pointer to the one and only instance of Phone Mediator 
        *         Sender -object.
        */
        IMPORT_C static CPhoneMediatorSender* Instance();

        /**
         * Destructor.
         */
        virtual ~CPhoneMediatorSender();
                        
        /**
         * Sends event to the Mediator.
         * @param aCommandId - command to be sent as an event
         */
        IMPORT_C void SendEvent( const TPhoneViewCommandId aCommandId ) const;
        
        /**
         * Sends event to the Mediator.
         * @param aCommandId - command to be sent as an event
         * @param aCallId - command's call id
         */
        IMPORT_C void SendEvent( const TPhoneViewCommandId aCommandId, const TInt aCallId ) const;
        
        /**
         * Sends event to the Mediator.
         * @param aCommandId - command to be sent as an event
         * @param aCommandParam - Reference to command parameter
         */
        IMPORT_C void SendEvent( const TPhoneViewCommandId aCommandId, 
            TPhoneCommandParam& aCommandParam );

        /**
         * Sends event to the Mediator.
         * @param aCommandId - command to be sent as an event
         * @param aCallId - command's call id
         * @param aCommandParam - Reference to command parameter
         */
        IMPORT_C void SendEvent( const TPhoneViewCommandId aCommandId, 
            const TInt aCallId, 
            TPhoneCommandParam& aCommandParam ) const;

        /**
         * Sends event to the Mediator.
         * @param aCommandId - command to be sent as an event
         * @param aCallId - command's call id
         * @param aCommandParam - Pointer to TDesC16
         */
        IMPORT_C void SendEvent( const TPhoneViewCommandId aCommandId, 
            const TInt aCallId, 
            const TDesC* aMessage ) const;

        /**
        * Issue a Mediator Service command.
        * 
        * @param aDomain    The identifier of the domain     
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.
        * @param aVersion   The version information for the command.
        * @param aData      The parameters of the command.
        * @return TInt      an error code
        */
        IMPORT_C TInt IssueCommand( const TUid aDomain, 
                                    const TUid aCategory, 
                                    const TInt aCommandId,
                                    const TVersion aVersion, 
                                    const TDesC8& aData,
                                    MPhoneShutdownCommand* aShutdownCommand = NULL );

        /**
        * A response to a Mediator Service command.
        *
        * @param aDomain    The identifier of the domain          
        * @param aCategory  The identifier of the category.
        * @param aCommandId The identifier of the command.
        * @param aStatus    Standard Symbian error code indicating the
        *                   success of the command.
        * @param aData      The parameters of the response.
        */
        void CommandResponseL( TUid aDomain,
                               TUid aCategory, 
                               TInt aCommandId,
                               TInt aStatus, 
                               const TDesC8& aData );     

    private:

        /**
        * Instantiates this class and returns a pointer to us
        */
        static CPhoneMediatorSender* NewL();

        /**    
         * Protected constructor because of Singleton Pattern
         */    
        CPhoneMediatorSender();  
        
        /**
         * By default EPOC constructor is private.
         */
        void ConstructL();
               
        /**
        * Registers generic Mediator Events sent via this interface.
        */
        void RegisterGenericEvents();
        
        /**
        * Reacts to Video Telephony specific Mediator Command reponse.
        * @param aCommandId - Command to react for.
        */
        void VideoTelephonyCommandResponse( TInt aCommandId );
        
        /**
        * Maps Phone Engine call state to the ones defined in our Mediator API.
        * @param aCallState - Phone Engine's call state which to map
        * @return TCallState
        */
        TCallState MapCallState( const TInt aCallState ) const;

        /**
        * Maps Phone Engine call type to the ones defined in our Mediator API.
        * @param aCallTypr - Phone Engine's call type which to map
        * @return TCallType
        */
        TCallType MapCallType( const TInt aCallType ) const;
        
        /**
        * Resets the values of used TPhoneCommandBuffer.
        */
        void ResetCommandBuffer();
                
        
    private:
        
        /**
         * Interface for sending events to Mediator
         */
        CMediatorEventProvider* iEventSender;
        
        /**
         * List of events - used for registering events we provide
         */
        MediatorService::REventList iEvents;
        
        /**
        * List of generic events - used for registering events we provide
        */
        MediatorService::REventList iGenericEvents;

        /**
        * Interface for sending commands to Mediator
        */        
        CMediatorCommandInitiator* iCommandInitiator;
    
        /**
        * Buffer for re-sending command after received Mediator response
        */
        TPhoneCommandBuffer iCommandBuffer;

        /**
        * Pointer for executing command related to Video Telephony shutdown.
        */
        MPhoneShutdownCommand* iShutdownCommand;
    };
    
#endif // CPHONEMEDIATORSENDER_H

// End of File
