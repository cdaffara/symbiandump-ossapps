/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPEVideoCallHandling object
*
*/


#ifndef CPEVIDEOCALLHANDLING_H
#define CPEVIDEOCALLHANDLING_H

//INCLUDES
#include <e32base.h>
#include <mpemessagesender.h>

// CONSTANTS
const TInt KPEDataCallIdBase( KPEMaximumNumberOfVoiceCalls +
         KPEMaximumNumberOfConferenceCalls );

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CPESingleCall;
class MPEPhoneModelInternal;
class MPEMessageSender;
class CCCE;
class MCCECall;
class CPECallArrayOwner;
class CPEGprsTermination;
class MPEMonitorCommandOwner;
class MPEDataStore;

// CLASS DECLARATION

/**
* Handles video call related events from phone engine
*
* @lib CallHandlingGsm.lib
* @since Series60_4.0
*/
class CPEVideoCallHandling : public CBase,
                             public MPEMessageSender
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor
        */
        static CPEVideoCallHandling* NewL( 
                MPEMessageSender& aOwner,
                MPEPhoneModelInternal& aModel, 
                CCCE& aConvergedCallEngine,
                CPECallArrayOwner& aCallArrayOwner );

        /**
        * Destructor
        */
        virtual ~CPEVideoCallHandling();

    public:    // New functions
        
        /**
        * Makes answer request.
        * @return Symbian OS error code
        */
        TInt AnswerCall();

        /**
        * Returns call terminated error
        * @param aCallId, call identification number
        * @return Return errorCode
        */        
        TInt CallTerminatedError( const TInt aCallId );

        /**
        * Checks if there are any connected calls
        */
        TBool ConnectedCalls();

        /**
        * Checks if the call has reserved serial communications data port for use
        * @param aCallId, call identification number
        * @return ETrue if the data port is reserved for use or EFalse if not
        */
        TBool DataPortInUse( TInt aCallId ) const;

        /**
        * Creates dialing request.
        * @param aNumber, the telephone number to be called
        * @return Symbian OS error code
        */
        TInt DialCall( const TPEPhoneNumber& aNumber, TInt& aCallId );
        
        /**
        * Makes HangUp request. 
        * @param aCallId, call identification number
        * @return Symbian OS error code
        */
        TInt HangUp( TInt aCallId );

        /**
        * Initializes a single video call
        * @param aCall handle to converged call engine call object
        */
        void InitCall( MCCECall& aCall );

        /**
        * Checks if the data call is a multimedia call
        * @param aCallId, call identification number
        * @return TBool, ETrue if this is a multimedia call, EFalse if not
        */
        TBool IsMultimedia( TInt aCallId ) const;

        /**
        * Notifies system for changes to the call state.
        * @param aMessage, message id
        * @param aCallId, call identification number
        */
        void NotifySystemCallStateChanged( 
            MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
            TInt aCallId );

        /**
        * Makes resume request to the Multimode ETel 
        * @param aCallId, call identification number
        * @return Symbian OS error code
        */
        TInt ResumeCall( TInt aCallId );

        /**
        * Makes terminate all connection request.
        * @return KErrNone if operation was successful
        * @return KerrNotFound if there is not a call 
        * @return KErrServerBusy if similar request is already being serviced 
        * @return KErrGeneral
        * @return KErrNotSupported if feature is not supported
        */
        TInt TerminateAllConnections();

        /**
        * Starts monitoring incoming data calls
        */
        void StartUp();

        /**
        * Swaps connected and held calls
        * @return Symbian OS error code
        */
        TInt SwapCalls();

        /**
        * Checks is phonenumber available
        * @param aCallId, call identification number
        */
        TBool IsPhoneNumberAvailable( const TInt aCallId );

        /**
        * Video Call By State
        */
        CPESingleCall* VideoCallByState( TPEState aState );
                                              
        /**
        * Video Call By call id
        */
        CPESingleCall* VideoCallById( TInt aCallId );
        
        /**
         * Replace active video call with a waiting video call
         * NOT SUPPORTED AS OF 5.1
         */
        TInt ReplaceActive( TInt aCallId  );

        /**
        * Returns the number of non-idle data calls
        * @return Number of calls. 
        */
        TInt GetNumberOfCalls ();
        
        /**
        * Get the call state
        * @paramaCallId, call identification number
        * @return the call state. 
        */
        TPEState GetCallState ( TInt aCallId );
        
        /**
        * Returns information of the data call
        * @param aCallInfo, call info.
        * @param aCallId, call identification number
        * @return Symbian OS error code
        */  
        TInt GetCallInfo( RMobileCall::TMobileCallInfoV3& aCallInfo, TInt aCallId );
        
        /**
        * Finds data call name given the call id
        * @param aCallId, call identification number
        * @return data call name
        */
        TName& VideoCallName( const TInt aCallId );
        
        /**
        * Returns call duration time
        * @param aDuration the duration value of the call
        * @return KErrNone or KErrNotFound.
        */    
        TInt GetCallDuration( TTimeIntervalSeconds& aDuration, TInt aCallId );
        
        /**
        * Makes reject request. 
        * @return Symbian OS error code
        */
        TInt RejectCall( );

        /**
        * Makes HangUp requests for all calls
        * @return Symbian OS error code (KErrNotFound if there are no calls to release)
        */
        TInt ReleaseAll();

    public:    // From MPEMessageSender

        /**
        * Reroutes error message to Phone Engine, usable by call objects
        * @param aCallId Identification number of the call.
        * @param aErrorCode, the error code stored after previous unsuccessful request
        */
        void SendErrorMessage( 
            const TInt aCallId, 
            const TInt aErrorCode );
        
        /**
        * Overrides CPEDataCallHandling::SendMessage to provide internal handling for gsm events
        * @param aMessage, message id
        */
        void SendMessage( 
            const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
            const TInt aCallId );
                  
        /**
        * Reroutes call messages
        * @param aMessage The message code of the message
        * @param aName Name of call fill be maped to call id
        */  
        void SendMessage( 
            const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
            const TName& aName );
       
        /**
        * Reroutes call messages.
        * @param aMessage specifies the event that has taken place
        */
        void HandleInternalMessage(
            TInt aMessage );
        
    private:
        /**
        * C++ default constructor
        */
        CPEVideoCallHandling( MPEMessageSender& aOwner,
                              MPEPhoneModelInternal& aModel, 
                              CCCE& aConvergedCallEngine,
                              CPECallArrayOwner& aCallArrayOwner );

        /**
        * By default Symbian 2nd phase constructor is private
        */
        void ConstructL();
          
        /**
        * SetCallParams.
        * @param None.
        * @return None.
        */
        void SetCallParams( TInt aCallId );
        
        /**
        * Create new CPESingleCall object to CPECallArrayOwner and open new MCCECall ojcte to CCE.
        * @param None.
        * @return returns pointer to CPESingleCall objecte.
        */
        CPESingleCall* OpenNewCallL( const TPEPhoneNumber& aNumber );
        
        /**
        * Returns data call object by state
        * @param aState, call state
        * @return pointer to data call object
        */
        CPESingleCall* VideoCallByState( const TPEState aState ) const;
        
        /**
        * Returns data call object by name
        * @param aName, call name
        * @return pointer to data call
        */
        CPESingleCall* VideoCallByName( const TName& aName );
        
        /**
        * Checks if there are any data calls in the specified state
        * @param aState, call state
        * @return TBool, ETrue if there is a data call in the specified state
        *                EFalse if there is no data call in the specified state
        */
        TBool VideoCallInState( const TPEState& aState ) const;
        
        /**
        * Delete's call object and reset's datastore.
        * @param aCallid.
        */
        TInt ReleaseCallObject( const TInt aCallId  ); 
        
    private: // Data
    
        // Reference to owner of this class
        MPEMessageSender& iOwner;
        // Reference to phone model
        MPEPhoneModelInternal& iModel;
        // Etel phone connection.
        CCCE& iConvergedCallEngine;
        // Data call name stored for Video Call Manager use
        TName iDataCallName;
        // Call array owner
        CPECallArrayOwner& iCallArrayOwner;
        // Reference to data store
        MPEDataStore& iDataStore;
        // Restore Cause Video Call Reject 
        TBool iRestoreCauseVTCallReject;
        
        // Owned GPRS termination instance.
        CPEGprsTermination* iGprsInterface;
        /*
         * Call parameters for dialing
         * Own.
         */
        CCCECallParameters* iCallOpenParams;

};

#endif // CPEVIDEOCALLHANDLING_H
            
// End of File
