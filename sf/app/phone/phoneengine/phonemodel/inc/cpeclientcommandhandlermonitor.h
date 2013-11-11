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
* Description:  Monitors AT command requests.
*
*/



#ifndef CPECLIENTCOMMANDHANDLERMONITOR_H
#define CPECLIENTCOMMANDHANDLERMONITOR_H

//  INCLUDES
#include <e32base.h>
#include <cphcltcommandhandler.h>
#include <cphcltcallnotify.h>
#include <rphcltserver.h>
#include <pevirtualengine.h>
#include <mphcltcomhandrequestobserver.h>

// FORWARD DECLARATIONS
class MPEPhoneModelInternal;
class MPECallHandling;
class CPEMessageHandler;
class CPEManualCallControlHandler;


/**
*  Monitors AT command requests.
*/
NONSHARABLE_CLASS( CPEClientCommandHandlerMonitor ) : public CBase, 
    public MPhCltComHandRequestObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @return Pointer to created CPEClientCommandHandlerMonitor instance.
        */
        static CPEClientCommandHandlerMonitor* NewL(          
            MPECallHandling& aCallHandling,
            CPEMessageHandler& aMessageHandler,
            MPEPhoneModelInternal& aModel, 
            RPhCltServer& aPhoneServer,
            CPEManualCallControlHandler& aManualCallControlHandler );

        /**
        * Destructor.
        */
        virtual ~CPEClientCommandHandlerMonitor();

    public: // New functions
        
        /**
        * Start listening for AT command requests.
        */
        void Start();

        /**
        * Sends response to AT command request 
        * and starts listening again.
        * 
        * @param aErrorCode Symbian OS error code of the success of the command.
        */
        void Respond( const TInt aErrorCode );
        
        /**
        * Sends response to atd command.
        * 
        * @param aFlag ETrue if dialing started.
        */
        void DoCompleteCmdAtd( TBool aFlag );
        
    // from MPhCltComHandRequestObserver
        /**
        * Handles completed request.
        */ 
        void ComHandRequest();
        
        /**
        * Cancels pending request.
        */
        void Cancel();
        
        /**
        * Tells whether this monitor is active.
        */
        TBool IsActive();
        


    private: // Constructors

        /**
        * Constructor.
        */
        CPEClientCommandHandlerMonitor(           
            MPECallHandling& aCallHandling,
            CPEMessageHandler& aMessageHandler,
            MPEPhoneModelInternal& aModel,
            RPhCltServer& aPhoneServer,
            CPEManualCallControlHandler& aManualCallControlHandler);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        /**
        * Sends response to server.
        */
        void SendResponse( const TInt aResponse );

        /* Handles a leave occurring in the request completion.
        * @since Series60_2.7 
        * @param aError The leave code.
        * @return KErrNone because leave has been handled.
        */
        TInt RunError( TInt aError );


        

        /**
        * Handles various BT AT commands
        * 
        * @param aArgs command arguments, if any
        * @return TNone
        */
        void HandleCommandRequestL(const TPhCltComHandCommandParameters& aArgs );

        /**
        * Processing ATA command.
        */
        void HandleCmdAta();

        /**
        * Processing CHUP command.
        */
        void HandleCmdChup();

        /**
        * Processing VTS command
        * 
        * @param aDtmfTone dtfm tone to be played
        * @param aAction action to be done for a dtmf tone
        * @return none
        */
        void HandleCmdVtsL( const TPhCltDtmfTone aDtmfTone,
                            const TPhCltDtmfAction aAction  );

        /**
        * Handles ATD command.
        * @param aPhoneNumber phone number.
        */
        void HandleCmdAtdL( const TPhCltTelephoneNumber& aPhoneNumber );
         
        /**
        * Handles CHLD command.
        * @param aChldCommand call parameters.
        * @param aCallNo Call index in GSM.
        */
        void HandleCmdChldL( 
            const TPhCltChldCommand aChldCommand,
            const TUint aCallNo );
        
        /**
        * Handles mute ringing tone command.
        */
        void HandleCmdMuteRingingTone();
        
        /**
        * Handles mute microphone command.
        * @param aMute 0 -> activate mic, 1 -> mute mic
        */
        void HandleCmdMuteMic( const TBool aMute );
        
    private: // utility method
        
        /** 
        * Finds a conference call or a single call (in that order) in specified 
        * state and tries to hang it up. 
        *  
        * @param aState The call state 
        * @return KErrNone if succesfull, otherwise an error code. Especially 
        *         KErrNotFound if there are no calls in the specified state and 
        *         KErrGeneral or KErrAlreadyExists if a hangup request already 
        *         exists for a call in the specified state. 
        */ 
        TInt HangUp( TPEState aState );

    private:    // Data

        // PhoneServer session.
        RPhCltServer&                               iServer;
        // Command handler notifier.
        CPhCltCallNotify*                           iNotify;
        // Command arguments.
        TPckgBuf< TPhCltComHandCommandParameters >  iArgs;
        // Library loader.
        RLibrary                                    iLibrary;   
        // MPECallHandling handles call related commands to CallHandling subsystem.
        MPECallHandling&                            iCallHandling;
        // CPEMessageHandler object which handles messages
        CPEMessageHandler&                          iMessageHandler;    
        // MPEPhoneModelInternal owns this object.
        MPEPhoneModelInternal&                      iModel;
        // Flag to indicate is the response from Phone UI is outstanding
        TBool                                       iWaitingForResponse;
        // Manual call control handler
        CPEManualCallControlHandler&                iManualCallControlHandler;

    };

#endif      // CPECLIENTCOMMANDHANDLERMONITOR_H
            
// End of File
