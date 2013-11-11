/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPECallHandling class.
*
*/


#ifndef CPECALLHANDLING_H
#define CPECALLHANDLING_H

//INCLUDES
#include <etel.h>
#include <etelmm.h>
#include <ccce.h>
#include <pevirtualengine.h>
#include <mpemessagesender.h>
#include <mccedtmfinterface.h>
#include "mpecallinitialiser.h"
#include <mpecallhandling.h>

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CPEVideoCallHandling;
class MPEPhoneModelInternal;
class CPELine;
class CPEDtmfHandling;
class CPESingleCall;
class CPEConferenceCall;
class CPESupplementaryServicesMonitor;
class CPECallArrayOwner;
class CPsetSAObserver;
class CCCECallParameters;
class CPECCEObserver;
class CPESystemCallState;
class CRepository;

// CLASS DECLARATION

/**
* CPECallHandling Handles voice call related events from phone engine.
*
*  @lib callhandling.lib
*  @since Series60_4.0
*/
class CPECallHandling 
        : public CBase, 
          public MPEMessageSender,
          public MPECallHandling,
          public MPECallInitialiser
    {
    public: //Constructors and descructor
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CPECallHandling* NewL( MPEPhoneModelInternal& aModel, 
            CCCE& aConvergedCallEngine,
            MCCEDtmfInterface& aDtmfInterface );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CPECallHandling();

    protected:  // Constructors and destructor
        /**
        * C++ default constructor.
        */
        CPECallHandling( MPEPhoneModelInternal& aModel,
            CCCE& aConvergedCallEngine,
            MCCEDtmfInterface& aDtmfInterface );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public: // From MPEMessageSender
    
        /**
        * Reroutes error message to Phone Engine, usable by call objects
        * @param aCallId Identification number of the call.
        * @param aErrorCode, the error code stored after previous unsuccessful request
        */
        void SendErrorMessage( 
            const TInt aCallId, 
            const TInt aErrorCode );
        
        /**
        * Reroutes messages to Phone Engine, usable by call objects
        * @param aMessage Message id.
        * @param aCallId Identification number of the call.
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

    public: // From MPEMonitorCommandOwner
        /**
        * Reroutes error messages to Phone Engine, usable by other than call objects
        * @param aErrorCode, the error code stored after previous unsuccessful request
        */
        void SendErrorMessage( 
            const TInt aErrorCode );

        /**
        * Reroutes messages to Phone Engine, usable by other than call objects
        * @param aMessage The message code of the message
        */    
        void SendMessage( 
            const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage );

    public: // From MPECallHandling
        /**
        * Starts monitoring incoming calls for voice and data call.
        */
        IMPORT_C void StartUp();

        /**
        * Makes answer request to the Multimode ETel. 
        * @since Series60_4.0
        * @param None.
        * @return Error code.
        */
        IMPORT_C TInt AnswerCall();

        /**
        * Creates dialing request to the Multimode ETel. 
        * @since Series60_4.0
        * @param aNumber The telephone number to be called.
        * @param aCallId Identification number of the call.
        * @return Error (KErrNone is success).
        */
        IMPORT_C TInt DialCall( const TPEPhoneNumber& aNumber,TInt& aCallId );

        /**
        * Creates dialing request to the Multimode ETel. 
        * @since Series60_4.0
        * @param aNumber The telephone number to be called.
        * @param aCallId Identification number of the call.
        * @return Error (KErrNone is success).
        */
        IMPORT_C TInt DialMultimedia( const TPEPhoneNumber& aNumber,TInt& aCallId );

        /**
        * Returns call duration time.
        * @since Series60_4.0
        * @param aDuration Call duration.
        * @param aCallId Identification number of the call.
        * @return Error code (KErrNone or KErrNotFound).
        */    
        IMPORT_C TInt GetCallDuration( TTimeIntervalSeconds& aDuration, TInt aCallId );

        /**
        * Returns information of the call.
        * @since Series60_4.0
        * @param aCallInfo Current call information.
        * @param aCallId Identification number of the call.
        * @return Error code (KErrNone or KErrNotFound).
        */
        IMPORT_C  TInt GetCallInfo( 
            RMobileCall::TMobileCallInfoV3& aCallInfo, 
            TInt aCallId );

        /**
        * Returns state of the call.
        * @since Series60_4.0
        * @param aCallid Identification number of the call.
        * @return Call state ( EPEStateUnknown is error).
        */
        IMPORT_C TPEState GetCallState( TInt aCallId );

        /**
        * Returns the missed voice call indicator. 
        * @since Series60_4.0
        * @param  aMissedCall, missed call indicator.
        * @param aCallId, call idendificator.
        * @return KErrNone or KErrNotFound.
        */
        IMPORT_C TInt GetMissedCall( TBool& aMissedCall, TInt aCallId );

        /**
        * Returns number of non-idle calls
        * @since Series60_4.0
        * @param  None.
        * @return Number of current voice calls.
        */
        IMPORT_C TInt GetNumberOfCalls();

        /**
        * Returns remote alerting tone status (is it played by network or not).
        * @since Series60_4.0
        * @param aToneStatus Status of remote alerting tone play.
        * @return None.
        */
       /* IMPORT_C void GetRemoteAlertingToneStatus( 
            RMmCustomAPI::TRemoteAlertingToneStatus& aToneStatus );*/
        
        /**
        * Makes HangUp request to the CCE. 
        * @since Series60_4.0
        * @param aCallId Identification number of the call to be rejected.
        * @param aAutoResumeOption if options for autoresume.
        * @return Error (KErrNone is success).
        */
        IMPORT_C TInt HangUp( TInt aCallId, TPEHangUpOptions aAutoResumeOption );

        /**
        * Determines if there is a call in given state.
        * @since Series60_4.0
        * @param aState Call state search key.
        * @return TBool, ETrue is there is a call; otherwise EFalse.
        */
        IMPORT_C TBool IsCallInState( TPEState aState ) const;
        
        /**
        * Determines if there is a call in given state.
        * @since Series60_4.0
        * @param aState Call state search key.
        * @return callid is there is a call; otherwise KPECallIdNotUsed( -1 ).
        */
        IMPORT_C TInt GetCallIdByState( TPEState aState ) const;

        /**
        * Makes reject request to the Multimode ETel 
        * @return Return possible error code
        */
        IMPORT_C TInt RejectCall( );

        /**
        * Makes HangUp requests for all call to the Multimode ETel. 
        * @return Error (KErrNotFound if there is not a call and KErrNone if there one call at least).
        */
        IMPORT_C TInt ReleaseAll();

        /**
        * Set Dynamic Hscsd parameters.
        * @param const TInt aRxSlots, Hscsd data call received timeslots amount
                 const TInt aCallId, data call id.  
        * @return KErrNone or KErrNotFound.
        */
        IMPORT_C TInt SetDynamicHscsdParams( TInt aRxSlots, TInt aCallId );

        /**
        * Terminate all connection request. 
        */
        IMPORT_C TInt TerminateAllConnections();
        
        /**
        * Sets active line
        */
        IMPORT_C void SetActiveLine();

        /**
        * Makes transfer request to the Multimode ETel 
        * @param None.
        * @return Return possible error code
        */
        IMPORT_C TInt BuildConference();
        
        /**
        * Swaps connected and held calls
        * @since Series60_4.0
        * @param  None.
        * @return KErrNone or KErrNotFound.
        */
        IMPORT_C TInt SwapCalls();
        
        /**
        * Add conference member.
        * @return possible error code from the CallHandling subsystem.
        */
        IMPORT_C TInt AddConferenceMember();

        /**
        * Drops one member from the conference call
        * @since Series60_4.0
        * @param aCallid It is the Call Id of the call to be dropped
        * @return Return possible error code
        */
        IMPORT_C TInt DropMember( TInt aCallId );

        /**
        * Splits one member from the conference call. 
        * @since Series60_4.0
        * @param aCallid It is the index number of the call to be splited. 
        * @return Return possible error code
        */
        IMPORT_C TInt GoOneToOne( TInt aCallId );

        /**
        * Makes hold request for the connected call
        * @return KErrNone if request was sent successfully
        *         KErrGsmNotAllowed if no connected call was found
        */
        IMPORT_C TInt HoldCall();

        /**
        * Makes resume request for the held call
        * @return KErrNone if request was sent successfully
        *         KErrGsmNotAllowed if no held call was found
        */
        IMPORT_C TInt ResumeCall();

        /**
        * Makes transfer request to the Multimode ETel 
        * @since Series60_4.0
        * @param None. 
        * @return KErrNone or KErrNotFound.
        */
        IMPORT_C TInt TransferCalls();

        /**
        * Creates emergency call request to the Multimode ETel 
        * @since Series60_4.0
        * @param None. 
        * @return None.
        */
        IMPORT_C void DialEmergencyCall( const TPEPhoneNumber& aEmergencyNumber );

        /**
        * Returns call terminated diagnostics of a call
        * Info is valid only for Idle call
        * @since Series60_4.0
        * @param aCallName, TName information of a call
        * @return Return diagnostics info
        */
        IMPORT_C TInt GetCallTerminatedDiagnostics( 
            TName& aCallName ) const;

        /**
        * Returns call terminated error
        * @since Series60_4.0
        * @return Return errorCode
        */        
        IMPORT_C TInt CallTerminatedError( const TInt aCallId );
        
        /**
        * Replace active call with waiting call. 
        * @since Series60_5.0
        * @return Error (KErrNone is success).
        */
        IMPORT_C TInt ReplaceActive();
        
        /**
        * This method gets the reference to MPECallOwner object.
        * @since Series60_4.0
        * @param  None.
        * @return None.
        */
        IMPORT_C MPECallOwner& CallOwner() const;

        /**
         * Accepts unattended transfer request. 
         * @since    Series 60 v5.1
         * @return   System wide error code.
         */
        IMPORT_C TInt AcceptUnattendedTransfer();
        
        /**
         * Rejects unattended transfer request. 
         * @since    Series 60 v5.1
         * @return   System wide error code.
         */
        IMPORT_C TInt RejectUnattendedTransfer();

        /**
        * Does unattended transfer to the connected VoIP call 
        * @since Series60_5.1
        * @param aTransferTarget Address of the transfer target
        * @return Error (KErrNone is success).
        */
        IMPORT_C TInt DoUnattendedTransfer( const TDesC& aTransferTarget );

        /**
        * Starts call forwarding. 
        * @since Series60_5.1
        * @param aIndex A array index for a selected address.
        * @return Error (KErrNone is success).
        */
        IMPORT_C TInt ForwardCallToAddress( TInt aIndex );

    public: // From MPECallHandling - DTMF related
        /**
        * Cancels Dtmf string sending.
        */
        IMPORT_C void CancelDtmfPlay();        

        /**
        * Allows a client to continue dtmf string sending when 
        * it was stopped by use of 'w' char in the string.
        */
        IMPORT_C void ContinueDtmfSending();

        /**
        * Sends dtmf string.
        * @param aDtmfString String to be sent.
        * @return Symbian OS error code
        */
        IMPORT_C TInt SendDtmf( const TPEDtmfString& aDtmfString );

        /**
        * Sends dtmf tone
        * @param aTone Tone to be sent.
        * @return Error (KErrNone or KErrNotFound).
        */
        IMPORT_C TInt StartDtmfTone( const TChar& aTone );

        /**
        * Allows a client to cancel the sending of a dtmf string when 
        * it was stopped by use of 'w' char in the string.
        */
        IMPORT_C void StopDtmfSending();

        /**
        * Stops sending dtmf tone.
        * @return Error (KErrNone or KErrNotFound).
        */
        IMPORT_C TInt StopDtmfTone();

    public: // From base class MPECallInitialiser

        /**
        * @see MPECallInitialiser::InitVoiceCall
        */
        void InitVoiceCall( MCCECall& aNewCall );
        
        /**
        * @see MPECallInitialiser::InitVideoCall
        */
        void InitVideoCall( MCCECall& aNewCall );

        /**
        * @see MPECallInitialiser::InitConferenceCall
        */
        void InitConferenceCall( MCCEConferenceCall& aConference );

    public:
        /**
        * Tells if there is any connected calls.
        * @return Indication of connected call(s) (EFalse is no calls, ETrue is connected call(s)).
        */
        TBool ConnectedCalls();

        /**
        * Returns voice call object by state.
        * @param aState Call state search key.
        * @param aIndex Index of the found call object (-1 is not found).
        * @param aGetCommandObject (if ETrue then find by command object state).
        * @return Call object (NULL is not found).
        */
        CPESingleCall* VoiceCallDataByState( 
            TPEState aState, 
            TInt& aIndex ) const;
        
        /**
        * SetCallParams.
        * @param None.
        * @return None.
        */
        void SetCallParams( TInt aCallId );

        /**
        * Creates CPECallData objects
        * @since Series60_4.0
        * @param None. 
        * @return None. 
        */
        void CreateCallDataArraysL();

        /**
        * Create conference call
        */
        void CreateConferenceCallL( CPESingleCall& aCall1, CPESingleCall& aCall2 );
        
        /**
        * Returns number of conference members
        * @since Series60_4.0
        * @param aCount It is variable, where number of participants is returned
        * @return Return possible error code
        */
        TInt GetNumberOfParticipants( TInt& aCount );

        /**
        * Returns added or removed members TName information and CallId
        * @since Series60_4.0
        * @param aCallId, Conference calls CallId
        * @param aCallName, TName information of dropped or added call
        * @param aMemberCallId, CallId of the added or dropped call
        * @return Return errorCode
        */
        TInt GetConferenceMemberNameAndId( TName& aCallName, TInt& aMemberCallId );

        /**
        * Get call control caps
        * @since Series60_4.0
        * @param aCallControlCaps, the call control caps.
        * @return KErrNone or KErrNotFound.
        */        
        TInt GetCallControlCaps( TPECallControlCaps& aCallControlCaps, TInt aCallId );
     
        /**
        * Returns Conference capabilities
        * @since Series60_4.0
        * @param aCallId, call identification number
        * @param aCaps, capabilities are returned in this variable
        * @return Return errorCode
        */
        TInt GetConferenceCallCaps( TUint32& aCaps );
        
        /**
        * Create new CPESingleCall object to CPECallArrayOwner and open new MCCECall ojcte to CCE.
        * @param None.
        * @return returns pointer to CPESingleCall objecte.
        */
        CPESingleCall* OpenNewCallL( const TPEPhoneNumber& aNumber );

        /**
        * Adds one member to the conference call ( TRAPPED METHOD )
        * @since Series60_4.0
        * @param aCallid It is the index number of the call to be added
        * @return Return possible error code
        */
        TInt AddMember( TInt aCallId );
    
        /**
        * Adds one member to the conference call
        * @since Series60_4.0
        * @param aCallid It is the index number of the call to be added
        */
        void AddMemberL( TInt aCallId );
        
        /**
        * Get call's secure status.
        * @param aCallid.
        * @return TBool ETrue if security is enabled, else EFalse.
        */
        TBool IsSecureCall( const TInt aCallId ) const;
        
        /** 
        * Update System Agent setting for call forward.
        * @since Series60_4.0
        */
        void UpdateSaSetting();
        
        /** 
        * Defines basic service code for divert notification.
        * @since Series60_4.0
        * @param aLineType used line type
        * @param call reference to call object
        * @return Returns the basuc service code. 
        */ 
        TInt DefineDivertBsc( CCCECallParameters::TCCELineType aLineType, 
                              CPESingleCall* call );

        /**
        * Check if dial allowed   
        * @param aMultimediaDial. Is new dial multimedia
        * @return ETrue if dial allowed
        */
        TBool IsDialAllowed( TBool aMultimediaDial );

        /**
        * Get call's secure specified status.
        * @param aCallid.
        * @return TBool ETrue if security is specified, else EFalse.
        */
        TBool SecureSpecified( const TInt aCallId ) const; 
        
        /**
        * Initialise conference call info.
        * @param call reference to call object
        * @param call reference to call object
        * @return None.
        */
        void InitialiseConferenceCallInfo( 
            MCCECall& aCall1, 
            MCCECall& aCall2 );
        
        /**
        * Handle auto resume.
        */
        void HandleAutoResume();
        
    private:
        
        /**
        * Delete's call object and reset's datastore.
        * @param aCallid.
        */
        TInt ReleaseCallObject( const TInt aCallId  ); 
        
        /**
         * Set call origin to datastore.
         * @param aCallid.
         * @param aCall reference to call object
         */
        void SetCallOrigin( const TInt aCallId, const MCCECall& aCall ) const;
        
        /**
         * Set COLP number to datastore if remote party number is 
         * different from dialled number.
         * @param aCallid.
         * @param aCall reference to call object
         * @return Updating Colp number was necessary.
         */
        TBool UpdateColpNumber( TInt aCallId, const MCCECall& aCall ) const;

    protected:
        // Phone model instance which owns this object.
        MPEPhoneModelInternal& iModel;
        // Handle to converged call engine (CCE) interface
        CCCE& iConvergedCallEngine;
        // Video call handling object handles video call operations.
        CPEVideoCallHandling* iVideoCallHandling;
        // CPEDtmfHandling object handles dtmf commands
        CPEDtmfHandling* iDtmfHandling;   
        // Member variable for CallHandling Errorcode
        TInt iErrorCode;
        // Call parameters for dialing
        CCCECallParameters* iCallOpenParams;
        
        
        //Create conference call CPEConferenceCall object
        CPEConferenceCall* iConferenceCall;
        // Supplementary Service Monitor
        CPESupplementaryServicesMonitor* iSupplementaryServicesMonitor;
        // Call array owner
        CPECallArrayOwner* iCallArrayOwner;
        // CCE dtmf interface
        MCCEDtmfInterface& iDtmfInterface;
        // Active line
        CCCECallParameters::TCCELineType iActiveLine;
        // Owned system agent observer. 
        CPsetSAObserver* iPsetSAObserver;
        // Emergency number
        TPEPhoneNumber iEmergencyNumber;
        // ReplaceActive       
        TBool iReplaceActive;
        // Status of dial, before dialing state
        TBool iDialRequestCallId;

        /**
         * CCE observer.
         */
        CPECCEObserver* iCCEObserver;
        // System Call State
        CPESystemCallState* iSystemCallState;
        
        CRepository* iRepository;
    };

#endif    // CPECALLHANDLING_H
            
// End of File
