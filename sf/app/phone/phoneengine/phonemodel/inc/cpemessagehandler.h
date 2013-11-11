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
* Description:  Message handler
*
*/



#ifndef CPEMESSAGEHANDLER_H
#define CPEMESSAGEHANDLER_H

//  INCLUDES
#include <pevirtualengine.h>
#include <DosSvrServices.h>
#include <cphcltcommandhandler.h>
#include <cphcltdialdata.h>

#include "mpecallhandling.h"
#include "mpecallcontrolif.h" 

// CONSTANTS
const TInt KModeNormal = 0;  // Normal System mode  
const TInt KModeFligth = 1;  // Flight mode 
const TInt KModePDA    = 2;  // PDA mode 

// Valid characters in an dialing string
_LIT( KPEClientValidChars, "+0123456789*#pwPW" );
_LIT( KPEValidDTMFChars, "0123456789*#pwPW" );
_LIT( KPEValidDTMFStringStopChars, "+pPwW" );

// Invalid characters in an dialing string, these chars can be removed from dial string
_LIT( KPECharsThatCanBeDelete, "\"/ ().-" );

// MACROS
// None.

// DATA TYPES

// Phone number prefix text, for prefix change
typedef TBuf<10> TPEPrefixText;

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class MPEPhoneModelInternal;
class MPECallHandling;
class MEngineMonitor;
class CPEGsmAudioData;
class CPESimStateMonitor;
class CPEMMParserBase;
class CPETimer;
class MPEAudioData;
class MPELogHandling;
class MPEContactHandling;
class CPEMMHandlerContainer;
class RTASecuritySession;
class CPEClientInformation;
class CPEClientServices;
class CPEClientCallRequestMonitor;
class CPEExternalDataHandler;
class MPEDataStore;
class CPhoneGsmOptionContainerBase;
class CPEParserPhoneNumberHandler;
class CPEParserSSHandler;
class CPhoneGsmParserBase;
class CPhoneGsmHandlerContainer;
class CPhoneGsmParserResult;
class CPEParserEmergencyNumberHandler;
class CPEParserVoipNumberHandler;
class CPEManualCallControlHandler;
class MPEServiceHandling;

// CLASS DECLARATION

/**
*  Handles phone related messages from the MPEPhoneModelInternal object.
*
*  @lib phoneenginebase.dll
*  @since S60_5.0
*/
NONSHARABLE_CLASS( CPEMessageHandler ) 
    : 
        public CBase, 
        public MPECallControlIF
    {
    public:  // Destructor

        /**
        * Two-phased constructor.
        */
        static CPEMessageHandler* NewL( 
                MPEPhoneModelInternal& aModel,
                MPECallHandling& aCallHandling,
                MEngineMonitor& aEngineMonitor,
                CPEGsmAudioData& aAudioData,
                MPELogHandling& aLogHandling,
                MPEContactHandling& aContactHandling,
                CPESimStateMonitor& aSimStateMonitor,
                CPEExternalDataHandler& aGsmExternalDataHandler,
                                           MPEServiceHandling& aServiceHandling,
                RFs& aFsSession );
        /**
        * Destructor.
        */
        ~CPEMessageHandler();

    public: // New functions

        /**
        * Handles answer call message from the phone application.
        * @param aAutomaticAnswer, ETrue if triggered by automatic answer
        * @return possible error code from the CallHandling subsystem.
        */
        TInt HandleAnswerCall( const TBool aAutomaticAnswer );

        /**
        * Handles accessory mode changed message from AudioHandling subsystem.
        * @param aVideoCall, ETrue if call type video call
        * @return possible error code from the AudioHandling subsystem
        */
        TInt HandleAudioRouting( TBool aVideoCall, TInt aCallId );

        /**
        * Handles route preference changed message from AudioHandling subsystem.
        * @return possible error code from the AudioHandling subsystem
        */
        TInt HandleRoutePreferenceChanged();
        
        /**
        * Handles continue dtmf sending ('w' character in the dtmf string have defected stop in the dtmf sending)
        * @param aContinue, informs continue or not.
        */
        void HandleContinueDtmfSending( const TBool aContinue );

        /**
        * Handles release all message from phone application 
        * @return possible error code from the CallHandling subsystem.
        */
        TInt HandleReleaseAll( );

        /**
        * Handles release message from phone application 
        * @param aAutoResumeOption if options for autoresume.
        * @return Return possible error code.
        */
        TInt HandleReleaseCall( TPEHangUpOptions aAutoResumeOption = ETPEHangUpDefault );

        /**
        * Handles send dtmf message from phone application 
        * @return possible error code from the CallHandling subsystem.
        */
        TInt HandleSendDtmf();

        /**
        * Handles audio mute message from the phone application. 
        */
        void HandleSetAudioMute();
        
        /**
        * Handles audio output message from the phone application. 
        * @return possible error code from the AudioHandling subsystem.
        */
        TInt HandleSetAudioOutput();

        /**
        * Handles change volume message from the phone application.
        */
        void HandleSetAudioVolume();

        /**
        * Starts tone playing. 
        */
        void HandlePlayDTMFL();

        /**
        * Stops tone playing. 
        * @return error code.
        */
        TInt HandleEndDTMF();

        /**
        * Terminates all connected calls, waiting call, data connections.
        * @return errorCode.
        */        
        TInt HandleTerminateAllConnections(); 

        /**
        * Sets phone client information
        * @param aCallId, the dentification number of the call.
        * @param aMainPartOfPhoneNumber The phone number, 
        *        where extra chaters "/ ().-" and possible prefix are removed. 
        */        
        void SetClientInformation( const TInt aCallId, 
            const TDesC& aMainPartOfPhoneNumber );
            
         /**
        * Sets phone client information
        * @param aClientDialData Client dial data 
        */  
        void SetClientData( 
            const CPhCltDialData& aClientDialData ); 
        
        /**
        * Checks if the current char is removable
        * @param TChar, current character from inspected string.
        * @return TBool.
        */
        TBool IsItCharThatCanDelete( const TChar& aChar ) const;

        /**
        * Checks if the current char is valid
        * @param TChar, current character from inspected string.
        * @return TBool.
        */
        TBool IsValidChar( const TChar& aChar, const TDesC& aValidChars ) const;

        /**
        * Removes invalid char if it is removable
        * @param TDesc, current string from wich we are interested in.
        * @return TBool.
        */
        TBool RemoveInvalidChars( TDes& aString, 
                                           const TDesC& aValidChars, 
                                           const TBool aCheckForDelete ) const;

        /**
        * Handles EPEMessageDTMFSent message from call handling subsystem
        * @param ECCPErrorNone or KPEDontSendMessage
        */
        TInt HandleDtmfSent();
        
        /**
        * Stops DTMF sending and resets DTMF processing state
        */
        void StopDtmfSending();
        
        /**
        * Handles reject call message 
        * @return possible error code from the CallHandling subsystem.
        */
        TInt HandleRejectCall();
        
        /**
        * Called asyncronously from callback. Calls HandleSendDtmf()
        * function.
        * @param aAny Pointer to 'this' object.
        * @return error code.
        */
        static TInt CallBackHandleSendDtmf( TAny* aAny );
        
        /**
        * Called asyncronously from callback.
        * @param aAny Pointer to 'this' object.
        * @return error code.
        */
        static TInt CallbackSendMessageStoppedDTMF( TAny* aAny );
        
        /**
        * Handles build conference message from phone application
        * @return Return possible error code from the CallHandling subsystem.
        */
        TInt HandleBuildConferenceCall();

        /**
        * Handles Going private in Conference Call
        * @return possible error code from the CallHandling subsystem.
        */
        TInt HandleGoOneToOne();        

        /**
        * Gets basic service group value. 
        * @return basic service group.
        */
        inline RMobilePhone::TMobileService BasicServiceGroup();
        
        /**
        * Handles add conference member message from phone application
        * @return possible error code.
        */
        TInt HandleAddConferenceMember();

        /**
        * Handles dropped conference member message
        */
        void HandleDroppedConferenceMember();
        
        /**
        * Handles EPEMessageALSLineChanged message.
        */
        void HandleALSLineChanged();

        /**
        * Handles error message from the CallHandling subsystem.
        * @param aCallId is the identification number of the call.
        * @param aBusyCall informs that error cause was busy remote party.
        */
        void HandleCallHandlingError( const TInt aCallId, const TBool aBusyCall );
        
        /**
        * Handles cancel SS string command.
        * @return Possible error code from the Settings utility subsystem.
        *         Security module don't have return value from cancel command.
        */
        TInt HandleCancelSSstringCommand();

        /**
        * Handles conference idle state
        * @param aCallId is the identification number of the call.
        * @return Return possible error code.
        */
        TInt HandleConferenceIdleState( const TInt aCallId );

        /**
        * Handles connected message from the CallHandling subsystem.
        * @param aCallId, identification number of the call.
        * @return possible error code from the CallHandling subsystem.
        */
        TInt HandleConnectedState( const TInt aCallId );

        /**
        * Handles video call logging in Connected state
        * @param aCallId is the identification number of the call.
        * @return Return possible error code.
        */
        TInt HandleVideoCallConnected( const TInt aCallId );

        /**
        * Handles video call logging in Idle state
        * @param aCallId is the identification number of the call.
        * @return Return possible error code.
        */
        TInt HandleVideoCallIdle( const TInt aCallId );

        /**
        * Handles EPEMessageSetALSLine message.
        */
        void HandleSetALSLineL();

        /**
        * Handles dial message from phone application or phone client.
        * @param aClientCall, Informs is the current call client originated or not.
        * @return Return possible error code.
        */
        TInt HandleDialCall( const TBool aClientCall );        

        /**
        * Handles dial emergency message from phone application or phone client.
        * Phase one of emergency call initialization
        * @param aClientCall, Informs is the current call client originated or not.
        * @return Return possible error code.
        */
        TInt HandleDialEmergencyCall( const TBool aClientCall );        

        /**
        * Handles Sat dial request completed message from the phone application 
        */
        void HandleSatCallRequestCompleted( );

        /**
        * Phase two of emergency call initialization
        */
        void ContinueDialEmergency();

        /**
        * Handles dialing state transition for voice and video calls
        * @param aCallId is the identification number of the call.
        */
        void HandleDialingStateL( const TInt aCallId );

        /**
        * Handles incoming voice and video call
        * @param aCallId is the identification number of the call.
        */
        void HandleIncomingCallL( const TInt aCallId );

        /**
        * Handles disconnecting without inband tones.
        * @param aCallId is the identification number of the call.
        * @return possible error code.
        */
        TInt HandleDisconnecting( const TInt aCallId ); 

        /**
        * Handles disconnecting with inband tones.
        * @param aCallId is the identification number of the call.
        * @return possible error code.
        */
        TInt HandleDisconnectingWithInband( const TInt aCallId );

        /**
        * Handles drop conference member message from phone application
        * @return possible error code from the CallHandling subsystem.
        */
        TInt HandleDropConferenceMember();

        /**
        * Handles hold message from the phone application 
        * @return possible error code from the CallHandling subsystem.
        */
        TInt HandleHoldCall( );
        

        /**
        * Handles idle state message from the CallHandling subsystem.
        * @param aCallId is the identification number of the call.
        * @return Return possible error code from the CallHandling subsystem.
        */
        TInt HandleVoiceCallIdleState( const TInt aCallId );

        /**
        * Handles play DMTF message from phone application 
        */
        void ProcessPlayDTMFL( );

        /**
        * Handles end DTMF playing message from phone application 
        * @return possible error code from the CallHandling subsystem.
        */
        TInt ProcessEndDTMF( );

        /**
        * Handles resume message from phone application 
        * @return possible error code from the CallHandling subsystem.
        */
        TInt HandleResumeCall( );

        /**
        * Handles send USSD request from CPEGsmParserMiscHandler
        * @param aString, USSD string to be sent
        * @return Symbian OS error code
        */
        TInt HandleSendUssd( const TDesC& aString );

        /**
        * Handles EPEMessageSimStateChanged from Dos Server.
        * @return None.
        */
        void HandleSimStateChanged( );

        /**
        * Handles startup message from the phone application.
        * @return None.
        */
        void HandleStartUp();

        /**
        * Handles Phone Number Edited message from phone application 
        */
        void HandlePhoneNumberEditedL();

        /**
        * Handles service enabled message from service handling subsystem
        * @since Series60_5.0
        * @return possible error code from the ServiceHandling subsystem
        */
        TInt HandleServiceEnabled();
        
        /**
         * Handles remote party information changed   
         * @param aCallId is the identification number of the call.               
         * @since Series60_5.2         
         */
        void HandleRemotePartyInfoChanged( const TInt aCallId );

        /**
        * Handles swap message from the phone application 
        * @return possible error code from the CallHandling subsystem.
        */
        inline TInt HandleSwapCalls();

        /**
        * Handles transfer message from phone application.
        * @return possible error code from the CallHandling subsystem.
        */
        inline TInt HandleTransferCalls();
    
        /**
        * Sets basic service group for future using,
        * @param aServiceGroup, includes basic service group.
        * @return None.
        */
        inline void SetBasicServiceGroup( const RMobilePhone::TMobileService& aServiceGroup );
        
        /**
        * Sets gsm parser error code for future using.
        * @param aGsmParserErrorCode, includes possible error code from parser process.
        * @return None.
        */
        inline void SetGsmParserErrorCode( TInt aGsmParserErrorCode);

        /**
        * Check is given number emergency number
        * @return Epoc errorcodes.
        */
        TInt HandleEmergencyCheck( );

        /**
        * Stops playing a inband tone. 
        */
        void HandleStopInbandTonePlay();

        /**
        * Handles automatic answer off from customa api -> engineinfo.
        * @return TInt possible error code..
        */
        void HandleAutomaticAnswerOff( ) const;

        /**
        * Handles AT dialing started
        * @param aSucceed true if succeeded
        */
        void HandleATDialingStarted( const TBool aSucceed ) const;
        
        /**
        * Replace active call
        * @return Possible error code
        */
        TInt HandleReplaceActive();
        
        /**
        * Check if phone is locked, if locked leave with ECCPErrorAuthenticationFailed error.
        */
        void CheckIfPhoneIsLockedL();
        
        /**
         * Handles unattended transfer request response.
         * @param    aAcceptRequest     ETrue to accept, EFalse to reject request.
         */
        TInt HandleUnattendedTransferRequestResponse( TBool aAcceptRequest );

        /**
        * Handle unattended transfer
        * @return KErrNone if succesfull
        */
        TInt HandleUnattendedTransfer();

        /**
        * Handle call forward
        * @return KErrNone if succesfull
        */ 
        TInt ForwardCallToAddress();
 
        /**
        * Handle disable service
        */     
        void HandleDisableService();

        /**
        * Adds SIM rejected MO CS call to logs. 
        * @param aCallId is the identification number of the call.   
        * @return KErrNone if succesfull
        */
        TInt AddSIMRejectedMoCsCallToLog( const TInt aCallId );

        /**
        * Handle dial service call
        */ 
        TInt HandleDialServiceCall(
            const TBool aClientCall );
        
        /**
        * Returns a boolean to indicate whether network connection 
        * is allowed or not.  
        * @return Return a True or False. 
        */
        TBool IsNetworkConnectionAllowed() const;
    
    private: // New functions
        
        /**
        * C++ default constructor.
        */
        CPEMessageHandler( MPEPhoneModelInternal& aModel, 
                           MPECallHandling& aCallHandling, 
                           MEngineMonitor& aEngineMonitor, 
                           CPEGsmAudioData& aAudioData, 
                           MPELogHandling& aLogHandling, 
                           MPEContactHandling& aContactHandling,
                           CPESimStateMonitor& aSimStateMonitor,
                           CPEExternalDataHandler& aExternalDataHandler,
                              MPEServiceHandling& aServiceHandling,
                           RFs& aFsSession );
        void ConstructL();
        
        /**
        * Checks the status of phone number prefix change setting
        * and calls CPEMessageHandler::ChangePrefix for execution
        * If error occurs the phone number is left untouched
        * @param aPhoneNumber, phone number to be checked
        */
        void CheckPrefix();

        /**
        * Replaces phone number prefix with the one provided
        * @param aPrefixText, prefix to be used in replacement
        * @return possible Symbian OS error code
        */
        TInt ChangePrefix( const TPEPrefixText& aPrefixText );
        
        /**
        * Remove '+' begin of the string
        */
        void RemovePlusPrefix( TPEPhoneNumber& aPhoneNumber );

        /**
        * Checks if automatic answer is on for audio accessory
        * @param TInt aCallId, call identification.
        * @return TBool.
        */
        TBool AutomaticAnswer( const TInt aCallId ) const;

        /**
        * Handle Client Call Data.
        * @return Return possible error.
        */
        void HandleClientCallData();

        /**
        * HandleEngineInfo. Set information to EngineInfo.
        * @param TInt aCallId, call identification.
        * @return Return possible error.
        */
        TInt HandleEngineInfo( const TInt aCallId ); 
        
        /**
        * Update Client information
        * @param TInt aCallId, call identification.
        */        
        TInt UpdateClientInfo( const TInt aCallId );
        
        /**
        * Check is phoneNumber emergency number and 
        * should phonenumber hide for User
        * @param TInt aCallId, call identification.
        */
        void CheckAndHideIdentity( const TInt aCallId );

        /**
        * Finds callInfo form TSY.
        * @param TInt aCallId, call identification.
        * @return Return possible error.
        */
        TInt FindCallInfo( const TInt aCallId );

        /**
        * Set phonenumber for call logging
        * @param TInt aCallId, call identification.
        * @return None.
        */
        void SetPhoneNumberForCallLogging( const TInt aCallId );

        /**
        * Store call information for logging
        * @param TInt aCallId, call identification.
        * @param TPEState aCallState, call's state.
        * @return None.
        */
        void SetLoggingInfo( const TInt aCallId, TPEState aCallState );

        /**
        * Set name to the EngineInfo.
        * @param TInt aCallId, call identification
        * @return None.
        */
        void SetName( const TInt aCallId );

        /**
        * Set rest of info to the EngineInfo.
        * @param TPEPhoneNumberIdType aPhoneNumberId, phonenumber type
        * @param TInt aCallId, call identification
        * @return None.
        */
        void HideIdentification(
            TPEPhoneNumberIdType aPhoneNumberId,
            const TInt aCallId );

        /**
        * Checks is the current call the missed call.
        * @param TInt aCallId, call identification.
        * @param TPEState aCallState, call's state.
        * @return None.
        */
        void IsMissedCall( const TInt aCallId, TPEState aCallState );

        /**
        * Notifies DevSound that there is no active call
        * and audio should be routed accordingly.
        */
        void ClearCallAudio();
             
        /**
        * Remove pre and post fix from phone number.
        * @param TDes aString, phonenumber. 
        * @return None
        */
        void RemovePreAndPostFix( TDes& aString );
        
        /**
        * Remove pre fix from phone number.
        * @param TLex aLex, orginal phonenumber.
        * @return None
        */
        void RemovePrefix( TLex& aLex );
        
        /**
        * Handle number part.
        * @param TLex aLex, orginal phonenumber.
        * @param TDes aNumberPart, TDes for edited phone numeber.
        * @return None
        */
        void HandleNumberPart( TLex& aLex, TDes& aNumberPart );
        
        /**
        * EqualsLeft combare if aString contains aPrefix.
        * @param TDesC aString, orginal phonenumber.
        * @param TDes aPrefix, 
        * @return TBool ETrue if match.
        */
        TBool EqualsLeft( const TDesC& aString, const TDesC& aPrefix );

        /**
        * Handles dial message from phone application or phone client.
        * @param aClientCall, Informs is the current call client originated or not.
        * @return Return possible error code.
        */
        TInt HandleDialCallL( const TBool aClientCall );

        /**
        * Reset CCCECallParameters to prevent of use a previous call´s parameters
        */
        void ResetClientCallData();

        /**
        * Checks if there are any connected video calls
        * @return   EFalse: no active video call,
        *           ETrue: active video call
        */
        TBool IsActiveVideo();
        
        /**
        * Checks that string contains only valid dtmf characters 
        * @param aString string to be checked
        * @return ETrue if valid dtmf string 
        */
        TBool IsValidDtmfString( TDes& aString );
        
        /**
         * Updates remote party info to Mediator.
         */
        void UpdateRemotePartyInfo();

    private:  // Data
        
        // MPEPhoneModelInternal owns this object.
        MPEPhoneModelInternal& iModel;
        // It is container for parser options
        CPhoneGsmOptionContainerBase* iOptions;
        // Handles phone number from the parser.
        // Not owned
        CPEParserPhoneNumberHandler* iPhoneNumberHandler;
        // Handles supplementary service requests from the parser.
        CPEParserSSHandler* iSSHandler;
        // MPECallHandling handles call related commands to CallHandling subsystem.
        MPECallHandling& iCallHandling;
        // MEngineMonitor handles interface structures to phone application
        MEngineMonitor& iEngineMonitor;
        // CPEGsmAudioData handles audio commands
        CPEGsmAudioData& iGsmAudioData;
        // CPELogsData object which handles logs related commands
        MPELogHandling& iLogHandling;
        // Handle to contact handling
        MPEContactHandling& iContactHandling;
        // Handle to external data handler
        CPEExternalDataHandler& iExternalDataHandler; 
        // Mobile call info.
        RMobileCall::TMobileCallInfoV3* iCallInfo;
        // Sim state monitor
        CPESimStateMonitor& iSimStateMonitor;
        // Handle to a file server session
        RFs& iFsSession; 
        // Handles call request from the client/server interface.
        CPEClientServices* iClientServices;
        // Automatic answer timer.
        CPETimer* iAutomaticAnswerTimer;
        // Number parser object
        CPhoneGsmParserBase* iParser;
        // Container for parser handlers.
        CPhoneGsmHandlerContainer* iParserHandlerContainer;
        // Includes identification number of the waiting call.
        TInt iWaitingCallId;
        // Call start time
        TTime iTime;
        //Current emergency call state.
        TBool iEmergencyCallActive;
        //
        TBool iBtaaDisconnected;
        //Client Information, member variable because emergency call from phone client
        //is not allowed to allocate memory. 
        CPEClientInformation* iClientInformation;
        // Dial Data
        CPhCltDialData* iClientDialData;
        // Instances will contain the results of the parsing
        CPhoneGsmParserResult* iResult;
        // Handles emergency number from the parser.
        // Not owned
        CPEParserEmergencyNumberHandler* iEmergencyNumberHandler;
        // Handles voip number from the parser.
        // Not owned
        CPEParserVoipNumberHandler* iVoipNumberHandler;
        // Handles manual call control.
        // Owned
        CPEManualCallControlHandler* iManualCallControlHandler;
        MPEServiceHandling& iServiceHandling;
        // reference to data store
        MPEDataStore& iDataStore;
        // Asyncromous callback
        CAsyncCallBack* iAsyncCallBack;
        // Includes basic service value.
        RMobilePhone::TMobileService iBasicServiceGroup;
        // Includes possible Gsm Parser error code.
        TInt iGsmParserErrorCode;
        // Cheks if audio is registred and system is ready for startup
        TBool iPEReadyForStartUpMessageSent;
    };

#include "cpemessagehandler.inl"

#endif      // CPEMESSAGEHANDLER_H   
            
// End of File
