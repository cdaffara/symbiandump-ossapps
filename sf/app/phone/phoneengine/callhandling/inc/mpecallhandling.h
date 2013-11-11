/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the MPECallHandling class.
*
*/



#ifndef MPECALLHANDLING_H
#define MPECALLHANDLING_H

//INCLUDES
#include <mccecall.h>
#include <pevirtualengine.h>
#include <mpecallowner.h>

// CONSTANTS
enum TPEHangUpOptions 
    {
    ETPEHangUpDefault = 0,       // Normal hangup
    ETPEHangUpResumeHeldCall,    // 1 + Send
    ETPEHangUpNotResumeHeldCall, // 1x + Send
    };

// CLASS DECLARATION

/**
*  Virtual interface of call handling subsystem
*
*  @lib CallHandling.lib
*  @since Series60_4.0
*/
class MPECallHandling
    {
    public:
        /**
        * destructor
        */
        virtual ~MPECallHandling( ) 
        {
        // empty destructor    
        };
        
    public:
        /**
        * Starts monitoring incoming calls for voice and data call.
        */
        virtual void StartUp() = 0;

        /**
        * Makes answer request to the Multimode ETel. 
        * @since Series60_4.0
        * @param None.
        * @return Error code.
        */
        virtual TInt AnswerCall() = 0;

        /**
        * Creates dialing request to the Multimode ETel. 
        * @since Series60_4.0
        * @param aNumber The telephone number to be called.
        * @param aCallId Identification number of the call.
        * @return Error (KErrNone is success).
        */
        virtual TInt DialCall( const TPEPhoneNumber& aNumber,TInt& aCallId ) = 0;

        /**
        * Creates dialing request to the Multimode ETel. 
        * @since Series60_4.0
        * @param aNumber The telephone number to be called.
        * @param aCallId Identification number of the call.
        * @return Error (KErrNone is success).
        */
        virtual TInt DialMultimedia( const TPEPhoneNumber& aNumber,TInt& aCallId ) = 0;

        /**
        * Makes HangUp request to the CCE. 
        * @since Series60_4.0
        * @param aCallId Identification number of the call to be rejected.
        * @param aAutoResumeOption if options for autoresume.
        * @return Error (KErrNone is success).
        */
        virtual TInt HangUp( TInt aCallId, TPEHangUpOptions aAutoResumeOption ) = 0;

        /**
        * Put all the calls to idle and make an MMEtel request to 
        * end the connection 
        * @return KErrNotFound if there is not a call 
        *         KErrNone if there is one call at least
        */
        virtual TInt ReleaseAll() = 0;

        /**
        * Makes terminate all connection request to Custom Api. 
        * @since Series60_4.0
        * @param  None.
        * @return KErrNone.
        */
        virtual TInt TerminateAllConnections() = 0;
        
        /**
        * Determines if there is a call in given state.
        * @since Series60_4.0
        * @param aState Call state search key.
        * @return TBool, ETrue is there is a call; otherwise EFalse.
        */
        virtual TBool IsCallInState( TPEState aState ) const = 0;
        
        /**
        * Determines if there is a call in given state.
        * @since Series60_4.0
        * @param aState Call state search key.
        * @return callid is there is a call; otherwise KPECallIdNotUsed( -1 ).
        */
        virtual TInt GetCallIdByState( TPEState aState ) const = 0;

        /**
        * Returns the missed voice call indicator. 
        * @since Series60_4.0
        * @param  aMissedCall, missed call indicator.
        * @param aCallId, call idendificator.
        * @return KErrNone or KErrNotFound.
        */
        virtual TInt GetMissedCall( TBool& aMissedCall, TInt aCallId ) = 0;

        /**
        * Returns information of the call.
        * @since Series60_4.0
        * @param aCallInfo Current call information.
        * @param aCallId Identification number of the call.
        * @return Error code (KErrNone or KErrNotFound).
        */
        virtual TInt GetCallInfo( RMobileCall::TMobileCallInfoV3& aCallInfo, TInt aCallId ) = 0;

        /**
        * Get the call state
        * 
        * @param a call ID. 
        * @return the call state. 
        */
        virtual TPEState GetCallState ( TInt aCallId ) = 0;
 
        /**
        * Get the number of calls (voice + call waiting)
        * 
        * @return Number of calls. 
        */
        virtual TInt GetNumberOfCalls ( ) = 0;
               
        /**
        * Returns call duration time.
        * @since Series60_4.0
        * @param aDuration Call duration.
        * @param aCallId Identification number of the call.
        * @return Error code (KErrNone or KErrNotFound).
        */    
        virtual TInt GetCallDuration( TTimeIntervalSeconds& aDuration, TInt aCallId ) = 0;

        /**
        * Makes reject request to the Multimode ETel 
        * @since Series60_4.0
        * @return Return possible error code
        */
        virtual TInt RejectCall( ) = 0;

        /**
        * Reroutes error codes to the Phone Model.
        * @since Series60_4.0
        * @param aCallId Identification number of the call.
        * @param aErrorCode Error code.
        * @return None.
        */
        virtual void SendErrorMessage( TInt aCallId, TInt aErrorCode ) = 0;

        /**
        * Sets active line
        */
        virtual void SetActiveLine() = 0;

        /**
        * Makes transfer request to the Multimode ETel 
        * @since Series60_4.0
        * @param None.
        * @return Return possible error code
        */
        virtual TInt BuildConference() = 0;
        
        /**
        * Swaps connected and held calls
        * @since Series60_4.0
        * @param  None.
        * @return KErrNone or KErrNotFound.
        */
        virtual TInt SwapCalls() = 0;

        /**
        * Checks that can add conference member.
        * @return possible error code from the CallHandling subsystem.
        */
        virtual TInt AddConferenceMember() = 0;
        
        /**
        * Drops one member from the conference call
        * @since Series60_4.0
        * @param aCallid It is the Call Id of the call to be dropped
        * @return Return possible error code
        */
        virtual TInt DropMember( TInt aCallId ) = 0;

        /**
        * Splits one member from the conference call. 
        * @since Series60_4.0
        * @param aCallid It is the index number of the call to be splited. 
        * @return Return possible error code
        */
        virtual TInt GoOneToOne( TInt aCallId ) = 0;

        /**
        * Makes hold request for the connected call
        * @return KErrNone if request was sent successfully
        *         KErrGsmNotAllowed if no connected call was found
        */
        virtual TInt HoldCall( ) = 0;

        /**
        * Makes resume request for the held call
        * @return KErrNone if request was sent successfully
        *         KErrGsmNotAllowed if no held call was found
        */
        virtual TInt ResumeCall( ) = 0;

        /**
        * Makes transfer request to the Multimode ETel 
        * @since Series60_4.0
        * @param None. 
        * @return KErrNone or KErrNotFound.
        */
        virtual TInt TransferCalls() = 0;
        
        /**
        * Creates emergency call request to the Multimode ETel 
        * @since Series60_4.0
        * @param None. 
        * @return None.
        */
        virtual void DialEmergencyCall( const TPEPhoneNumber& aEmergencyNumber ) = 0;

        /**
        * Returns call terminated diagnostics of a call
        * Info is valid only for Idle call
        * @since Series60_4.0
        * @param aCallName, TName information of a call
        * @return diagnostics info of a call
        */
        virtual TInt GetCallTerminatedDiagnostics( 
            TName& aCallName ) const = 0;
        
        /**
        * Returns call terminated error
        * @since Series60_4.0
        * @return Return errorCode
        */        
        virtual TInt CallTerminatedError( const TInt aCallId ) = 0;
         
        /**
        * Replace active call with waiting call. 
        * @since Series60_5.0
        * @return Error (KErrNone is success).
        */
        virtual  TInt ReplaceActive() = 0;

        /**
         * Accepts unattended transfer request. 
         * @since    Series 60 v5.1
         * @return   System wide error code.
         */
        virtual TInt AcceptUnattendedTransfer() = 0;
        
        /**
         * Rejects unattended transfer request. 
         * @since    Series 60 v5.1
         * @return   System wide error code.
         */
        virtual TInt RejectUnattendedTransfer() = 0;

        /**
        * Does unattended transfer to the connected VoIP call 
        * @since Series60_5.1
        * @return Error (KErrNone is success).
        */
        virtual TInt DoUnattendedTransfer( const TDesC& aTransferTarget ) = 0;

        /**
        * Starts call forwarding to selected address. 
        * @since Series60_5.1
        * @param aIndex A array index for a selected address.
        * @return Error (KErrNone is success).
        */ 
        virtual TInt ForwardCallToAddress( TInt aIndex ) = 0;
        
    public: // DTMF related
        /**
        * Sends dtmf string.
        * @since Series60_4.0
        * @param aDtmfString TPEDtmfString to be sent.
        * @return KErrNone.
        */
        virtual TInt SendDtmf( const TPEDtmfString& aDtmfString ) = 0;

        /**
        * Allows a client to continue dtmf string sending when 
        * it was stopped by use of 'w' char in the string.
        * @since Series60_4.0
        * @param  None.
        * @return None.
        */
        virtual void ContinueDtmfSending() = 0;
        
        /**
        * Sends dtmf tone
        * @since Series60_4.0
        * @param aTone Tone to be sent.
        * @return Error (KErrNone or KErrNotFound).
        */
        virtual TInt StartDtmfTone( const TChar& aTone ) = 0;

        /**
        * Allows a client to cancel the sending of a dtmf string when 
        * it was stopped by use of 'w' char in the string.
        * @since Series60_4.0
        * @param None.
        * @return None.
        */
        virtual void StopDtmfSending() = 0;

        /**
        * Stops sending dtmf tone.
        * @since Series60_4.0
        * @param None.
        * @return Error (KErrNone or KErrNotFound).
        */
        virtual TInt StopDtmfTone() = 0;

        /**
        * Cancels Dtmf string sending.
        * @since Series60_4.0
        * @param  None.
        * @return None.
        */
        virtual void CancelDtmfPlay() = 0;
        
        /**
        * This method gets the reference to MPECallOwner object.
        * @return None.
        */
        virtual MPECallOwner& CallOwner() const = 0;
    };

#endif // MPECALLHANDLING_H

//End of file
