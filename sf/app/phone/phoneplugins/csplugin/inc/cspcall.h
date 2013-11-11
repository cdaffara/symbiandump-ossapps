/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSPCall for CS Call Plug-in
*
*/



#ifndef CSPCALL_H
#define CSPCALL_H

#include <mccpcscall.h>
#include <mccpcallobserver.h>
#include <mccptransferobserver.h>
#include <mccpforwardobserver.h>
#include <etel.h>
#include <etelmm.h>
#include <cccecallparameters.h>

#include "cspconsts.h"
#include "cspetelcallrequester.h" // For the requester and TRequestType
#include "mcspcallobserver.h"
#include "mcspcallerrorobserver.h"
#include "mcspuusmessageobserver.h"
#include "mcspcallcommandhandling.h"
#include "mcspaudiohandlerobserver.h"

class MCCPForwardProvider;
class CSPForwardProvider;
class CSPTransferProvider;
class RMobileCall;
class CSPEtelCallStatusMonitor;
class CSPEtelCallEventMonitor;
class CSPCallInfoMonitor;
class CSPEtelCallCapsMonitor;
class CSPTransferProvider;
class CSPEtelCallCapsMonitor;
class CSPAudioHandlerBase;
class MCSPCommonInfo;
class CSPUUIMonitor;
class CSPUUIMessageSender;


/**
* Call class for CS Call Plug-in. Implements the API defined by CCP in classes
* MCCPCall and MCCPCSCall
*
*  @lib csplugin.dll
*/
class CSPCall : public CBase,
                public MCCPCSCall,
                public MCSPCallObserver,
                public MCSPCallErrorObserver,
                public MCSPUUSMessageObserver,
                public MCCPCallCommandHandling,
                public MCSPAudioHandlerObserver
    {
    public:
        /*
        * C++ default destructor
        */
        virtual ~CSPCall( );

        /**
        * ETel call name accessor.
        * @return system wide error code
        */
        void CallName( TName& aCallName );

        /**
        * Sets the audio handler for the call.
        * @param aHandler audio handler
        */
        void SetAudioHandler( CSPAudioHandlerBase* aHandler );

        /**
         * Security setting change notification.
         * @param aValue new value
         */
        void SecuritySettingChanged( TInt aValue );

        /**
         * Remote alerting tone status changed notification.
         * @param aNewStatus New remote alerting tone status.
         */
        void RemoteAlertingToneStatusChanged(
            RMmCustomAPI::TRemoteAlertingToneStatus aNewStatus );

        /**
         * Set iDontReportTerm flag value to ETrue.
         */
        void DontReportTerminationError();

// from base class MCSPCallObserver

        /**
        * Updates the call event for observers.
        * @param aEvent the event to be notified
        */
        virtual void NotifyForwardEventOccurred(
                        MCCPForwardObserver::TCCPForwardEvent aEvent );

        /**
        * Updates the state change and forwards the state for observer.
        * @param aState new state (ETel) of the call
        */
        virtual void NotifyCallStateChangedETel(
            RMobileCall::TMobileCallStatus aState );

        /**
        * Updates the call event for observers.
        * @param aEvent the event to be notified
        */
        virtual void NotifyCallEventOccurred(
            MCCPCallObserver::TCCPCallEvent aEvent );

        /**
        * Updates the transfer event for observers.
        * @param aEvent the event to be notified
        */
        virtual void NotifyTransferCallEventOccurred(
                        MCCPTransferObserver::TCCPTransferEvent aEvent );

        /**
        * Notifies changed call capabilities.
        * @param aCapsFlags new capability flags
        */

        virtual void CallCapsChanged( const TUint32 aCapsFlags );

// from base class MCSPCallErrorObserver

        /**
        * From MCSPCallErrorObserver
        * Dial request failed notification.
        * @param aErrorCode request failing error code
        */
        void DialRequestFailed( TInt aErrorCode );

        /**
        * From MCSPCallErrorObserver
        * Emergency dial request failed notification.
        * @param aErrorCode request failing error code
        */
        void EmergencyDialRequestFailed( TInt aErrorCode );

        /**
        * From MCSPCallErrorObserver
        * Notifies about error situation for CCE.
        */
        virtual void NotifyErrorOccurred( TCCPError aError );

// from base class MCSPUUSMessageObserver

        /**
        * From MCSPUUSMessageObserver
        * UUS message received notification. 
        */
        void UUSMessageReceived( const TDesC& aMessage );        
        
// from base class MCCPCSCall

        /**
        * From MCCPCSCall
        * Returns call information
        * @param aCallInfo TMobileCallInfoV3
        * @return KErrNone if succesfull
        *         otherwise another system wide error code
        */
        virtual TInt GetMobileCallInfo( TDes8& aCallInfo ) const;

        /**
        * From MCCPCSCall
        * Gets the calls data call capabilities
        * @param aCaps TMobileCallDataCapsV1
        * @return KErrNone if the function member was successful,
        *        KErrNotSupported if call doesn't support circuit switched data
        *        KErrNotFound if this call is not a data call
        */
        virtual TInt GetMobileDataCallCaps( TDes8& aCaps ) const;

// from base class MCCPCall

        /**
        * From MCCPCall
        * Answers to an incoming call.
        * @return system wide error code
        */
        virtual TInt Answer();

        /**
        * From MCCPCall
        * Rejects an incoming call.
        * @return system wide error code
        */
        virtual TInt Reject();

        /**
        * From MCCPCall
        * Reguests plug-in to queue the call.
        * @return system wide error code
        */
        virtual TInt Queue();

        /**
        * From MCCPCall
        * Starts dialing to remote party
        * @return system wide error code
        */
        virtual TInt Dial();

        /**
        * From MCCPCSCall
        * Starts dialing to recipient
        * @param aCallParams Call parameters used
        *   by the TSY (TCallParamsPckg)
        * @return system wide error code
        */
        TInt Dial( const TDesC8& aCallParams );

        /**
        * From MCCPCall
        * Call is ringning
        * @return system wide error code
        */
        virtual TInt Ringing();

        /**
        * From MCCPCall
        * Ends an ongoing call.
        * @return system wide error code
        */
        virtual TInt HangUp();

        /**
        * From MCCPCall
        * Cancels the ongoing request
        * @return system wide error code
        */
        virtual TInt Cancel();

        /**
        * From MCCPCall
        * Puts the specified call on hold.
        * @return system wide error code
        */
        virtual TInt Hold();

        /**
        * From MCCPCall
        * Resumes the call.
        * @return system wide error code
        */
        virtual TInt Resume();

        /**
        * From MCCPCall
        * Swaps a connected call to its opposite state either active or on hold
        * @return system wide error code
        */
        virtual TInt Swap();

        /**
        * From MCCPCall
        * Returns the remote party of the call.
        * @return remote party address,
        */
        const TDesC& RemoteParty() const;

        /**
        * From MCCPCall
        * Returns the remote party name of the call.
        * @return remote party name
        */
        const TDesC& RemotePartyName();

        /**
        * From MCCPCall
        * Dialled party.
        * @return dialled party name
        */
        const TDesC& DialledParty() const;

        /**
        * From MCCPCall
        * Return boolean value if the incoming call is forwarded or not.
        * @return ETrue if call is forwarded, else EFalse
        */
        virtual TBool IsCallForwarded() const;

        /**
        * From MCCPCall
        * Returns boolean value if the call is mobile originated or not.
        * @return ETrue if call is mobile originated, otherwise EFalse
        */
        virtual TBool IsMobileOriginated() const;

        /**
        * From MCCPCall
        * Returns the state of the call.
        * @return TCCPCallState The current state of the call
        */
        virtual MCCPCallObserver::TCCPCallState State() const;

        /**
        * From MCCPCall
        * Returns TUid of the plug-in used for this call
        * @return TUid Implementation UID of the Plug-In
        */
        virtual TUid Uid() const;

        /**
        * From MCCPCall
        * Returns call control caps for this call
        * @return call capability information
        */
        virtual MCCPCallObserver::TCCPCallControlCaps Caps( ) const;

        /**
        * From MCCPCall
        * Get call's secure status.
        * @return TBool ETrue if security is enabled, else EFalse.
        */
        virtual TBool IsSecured() const;

        /**
        * From MCCPCall
        * Get call's secure specified status.
        * @return TBool ETrue if secure specified
        */
        virtual TBool SecureSpecified() const;

        /**
        * From MCCPCall
        * Set call parameters.
        * @param aNewParams New call paramater information.
        */
        virtual void SetParameters( const CCCPCallParameters& aNewParams );

        /**
        * From MCCPCall
        * Cet call parameters.
        * @return Current call paramater information.
        */
        virtual const CCCPCallParameters& Parameters() const;

        /**
        * From MCCPCall
        * Get Forward provider
        * @param aObserver - observer class for forward events
        * @return Pointer to MCCPForwardProvider if succesfull, NULL if not available
        * @leave on error case system error code
        */
        virtual MCCPForwardProvider* ForwardProviderL( const MCCPForwardObserver& aObserver );

        /**
        * From MCCPCall
        * Get Call transfer provider
        * @param aObserver - observer class for transfer events
        * @return Pointer to MCCPTransferProvider if succesfull, NULL if not available
        * @leave on error case system error code
        */
        virtual MCCPTransferProvider* TransferProviderL( const MCCPTransferObserver& aObserver );

        /**
        * From MCCPCall
        * Adds a new observer for MCCECall object. Called in MT call case. In MO
        * case observer will be given at call construction.
        * Plug-in dependent feature if duplicates or more than one observers
        * are allowed or not. Currently CCE will set only one observer.
        * @param aObserver Observer
        * @leave system error if observer adding fails
        */
        virtual void AddObserverL( const MCCPCallObserver& aObserver );

        /**
        * From MCCPCall
        * Remove an observer.
        * @param aObserver Observer
        * @return KErrNone if removed succesfully. KErrNotFound if observer was not found.
        * Any other system error depending on the error.
        */
        virtual TInt RemoveObserver( const MCCPCallObserver& aObserver );

        /**
        * From MCCPCall
        * Returns the inband tone associated to an error condition or a state transition of a call.
        * See defenition for TCCPTone.
        * @since S60 3.2
        * @param None.
        * @return TCCPTone Used tone.
        */
        TCCPTone Tone() const;

// from base class MCCPCSCall

        /**
        * From MCCPCSCall
        * Indicates that FDN check is not used for dial.
        */
        virtual void NoFDNCheck( );

        /**
        * From MCCPCSCall
        * Log dialed  number. SAT related, check if dialed number is to be logged or not.
        * @since S60 5.0
        * @param None
        * @return Log dialed number or not.
        */
        virtual TBool LogDialedNumber() const;

// from base class MCCPCallCommandHandling

        /**
        * Perform answer command.
        * @return system wide error code
        */
        TInt PerformAnswerRequest();

        /**
        * Perform dial command.
        * @return system wide error code
        */
        TInt PerformDialRequest();

// from base class MCSPAudioHandlerObserver

        /*
         * Callback received when call audio control streams fail to start
         * within timeout specified in ReportAudioFailureAfterTimeout().
         */
        void AudioStartingFailed();

    protected:

        /**
        * Starts dialing to recipient.
        * Bypasses FDN check according to aFdnCheck.
        * @param aFdnCheck True if FDN checking is used
        * @return system wide error code
        */
        virtual TInt DialFdnCond( TBool aFdnCheck ) = 0;

    protected:

        /**
        * C++ default constructor.
        *
        * @param aLine line of the call
        * @param aMobileOriginated is call mobile originated
        * @param aName contains call name (mobile terminated) or phone number (mobile originated)
        * @param aCommonInfo reference to common info interface
        * @param aIsEmergencyCall is emergency call
        */
        CSPCall(  RMobileLine& aLine,
                  TBool aMobileOriginated,
                  const TDesC& aName,
                  MCSPCommonInfo& aCommonInfo,
                  TBool aIsEmergencyCall );

        /*
        * Constructing 2nd phase.
        * @param aParams call parameters
        */
        virtual void ConstructL( const CCCECallParameters& aParams );

        /*
        * Notifies the ringing state to observers.
        */
        virtual void NotifyRingingState();

        /**
        * Mapping ETel state to CCP state.
        * @param aEtelState an ETel state
        * @return corresponding CCP state
        */
        MCCPCallObserver::TCCPCallState CCPStateFromETelState(
                RMobileCall::TMobileCallStatus aEtelState );
                
        /**
        * From MCSPCallObserver 
        * Updates the state change including inband tones 
        * played by network and forwards the state for observer.
        * @param aState new state of the call
        */
        virtual void NotifyCallStateChangedWithInband(
            MCCPCallObserver::TCCPCallState aState );

        /**
        * Updates the state change
        * and forwards the state for observer.
        * @param aState new state of the call
        */
        virtual void NotifyCallStateChanged(
            MCCPCallObserver::TCCPCallState aState );

        /**
        * From MCSPCallObserver
        * Notify that remote party info has changed.
        * @param aRemotePartyName new remote party name
        * @param aRemotePartyNumber new remote party number
        */
        virtual void NotifyRemotePartyInfoChanged(
            const TDesC& aRemotePartyName,
            const TDesC& aRemotePartyNumber );

        /**
        * Open call handle.
        * Opens a new call handle for MO call
        * and an existing handle for MT call.
        */
        virtual void OpenCallHandleL();

        /**
        * Update call info data including call name,
        * remote name and number data.
        */
        virtual void UpdateCallInfo();
        
        /**
         * Implementation for UpdateCallInfo().       
         */
        virtual void UpdateCallInfoImpl( const RMobileCall::TMobileCallInfoV7& aCallInfo );
        
        /**
         * Set call origin to CCCECallParameters.       
         */
        virtual void UpdateCallOrigin( const RMobileCall::TMobileCallInfoV7& aCallInfo );
        
        /**
        * Reads information from cenrep.
        * @param aUid CR Uid
        * @param aKey CR key
        * @param aVal value of key, output
        */
        void ReadRepositoryL( TUid aUid, TUint aKey, TInt& aVal);

        /**
        * Check and notify call termination error.
        */
        void CheckAndNotifyTerminationError();

        /**
        * Error from iExitCode.
        */
        TInt ExitCodeError() const;

        /**
        * Update call name number info.
        * @param aCallInfo RMobileCall::TMobileCallInfoV3
        * @param aCallCreatedByMonitor, set:
        *       EFalse for calls added by plugin owner
        *       ETrue for calls added by ETel status monitor
        */
        void UpdateCallNameNumberInfo(
                const RMobileCall::TMobileCallInfoV3& aCallInfo, 
                TBool aCallAddedByMonitor ); 

        /**
        * Open new call.
        * New call is opened when the plugin owner adds the new
        * call (compare to OpenExistingCall where call is added by
        * ETel monitor).
        */
        void OpenNewCall();

        /**
        * Open existing call.
        * Existing call is opened if one of the ETel
        * monitors (incoming call monitor or line status monitor)
        * has added the call and no new call needs to be
        * opened.
        * @param aName name of the existing call
        */
        void OpenExistingCallL( const TDesC& aName );

    private:

        /**
        * Create call handlers for call requests and for
        * monitoring call events.
        */
        void CreateCallHandlersL();

        /**
        * Update call state from ETel
        * @return system wide error code
        */
        int UpdateCallState();

        /**
         * Sends a remote party info changed notification
         * if the remote party number has changed.
         */
        void NotifyRemotePartyNumberChanged();

	private:

        enum TCSPCallAudioStatus {
            ECSPCallAudioStatusInactive,
            ECSPCallAudioStatusActive
            };

    protected:

          /**
          * CCP Call Observer array.
          */
          RPointerArray<MCCPCallObserver> iObservers;

          /**
          * Actual ETel call handle.
          */
          RMobileCall iCall;

          /**
          * ETel line
          */
          RMobileLine& iLine;

          /**
          * Determines if the call direction is either MO (ETrue) or MT (EFalse)
          */
          TBool iMobileOriginated;

          /**
          * Name information of the call.
          *
          * Name information is stored during CSPCall construction
          * and assigned to iCallName (MT or client call) or
          * iRemotePartyNumber (MO call) afterwards.
          */
          TBuf<KCSPPhoneNumberMaxLength> iName;

          /**
          * Remote party name of the call.
          */
          TBuf<KCSPPhoneNumberMaxLength> iRemotePartyNumber;

          /**
          * Remote party name of the call.
          */
          TBuf<KCSPCallingNameMaxLength> iRemotePartyName;

          /**
          * Dialled party of the call.
          */
          TBuf<KCSPPhoneNumberMaxLength> iDialledParty;

          /**
          * Call name for MT call.
          */
          TName iCallName;

          /**
          * Call parameters.
          */
          CCCECallParameters* iParams;

          /**
          * Call state in CCP format
          */
          MCCPCallObserver::TCCPCallState iCallState;

          /**
          * Capability flags.
          */
          TUint32 iCapsFlags;

          /**
          * Call status monitor
          */
          CSPEtelCallStatusMonitor* iCallStatusMonitor;

          /**
          * Call event monitor
          */
          CSPEtelCallEventMonitor* iCallEventMonitor;

          /**
          * Call info monitor
          */
          CSPCallInfoMonitor* iCallInfoMonitor;

          /**
          * Makes the asynchronic requests for various purposes.
          */
          CSPEtelCallRequester* iRequester;

          /**
          * Dial completion code.
          */
          TInt iDialCompletionCode;

          /**
           * Common information for calls.
           */
           MCSPCommonInfo& iCommonInfo;

          /**
          * Call termination error notification has been sent.
          */
          TBool iTerminationErrorNotified;

          /**
          * Emergency call.
          */
          TBool iIsEmergencyCall;

    private:

          /**
          * Transfer provider.
          * Own.
          */
          CSPTransferProvider* iTransferProvider;

          /**
          * Forward provider. No actual implementation.
          * Own.
          */
          CSPForwardProvider* iForwardProvider;

          /**
          * Audio dev sound handler.
          * Not own.
          */
          CSPAudioHandlerBase* iAudioHandler;

          /**
          * FDN check usage status. Default is on. (ETrue)
          */
          TBool iFdnCheck;

          /**
          * Monitors call capabilities from ETel.
          * Own.
          */
          CSPEtelCallCapsMonitor* iCallCapsMonitor;

          /*
          * UUS monitor
          */
          CSPUUIMonitor* iUUIMonitor;

          /*
          * UUS sender
          */
          CSPUUIMessageSender* iUUIMessageSender;

          /**
          * Data for parsed skype id.
          * Own.
          */
          HBufC* iSkypeId;

          /**
          * Audio stop requested status. The flag prevents double audio stop.
          * Stop is executed in Disconnecting state and Idle state
          * if previous state has been DisconnectingWithInband.
          * Audio start status.
          */
          TCSPCallAudioStatus iAudioStatus;

          /**
          * Defines whether to report KErrGsmCCNormalUnspecified
          * and KErrGsmCCCallRejected.
          */
          TBool iDontReportTerm;

          /**
           * User to user information. Sended to network before dial.
           * Own.
           */
          HBufC* iUserToUserInformation;
          
          /**
           * Allocate CallInfo into heap since it could be
           * too big fit to stack.
           */
          RMobileCall::TMobileCallInfoV7 iEtelCallInfo;
          
          friend class UT_CSPCall;
          friend class MT_CConvergedCallProvider;

    };

#endif // CSPCALL_H

