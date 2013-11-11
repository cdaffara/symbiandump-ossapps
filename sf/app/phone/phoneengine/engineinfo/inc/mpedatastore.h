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
* Description:  Interface class for DataStore of the PhoneEngine
*
*/


#ifndef MPEDATASTORE_H
#define MPEDATASTORE_H

//  INCLUDES
#include <mpeengineinfo.h>
#include <cphcntcontactid.h>
#include <ccpdefs.h>
#include <mccecall.h>

class MPEClientInformation;


// CLASS DECLARATION

/**
*  Offers data store interface to phone engine
*  
*
*  @lib phoneengine.lib
*  @since Series 60 5.0
*/
NONSHARABLE_CLASS( MPEDataStore )
    : public MPEEngineInfo
    {
    public: // New functions
    
        /**
        * Gets audio mute command value
        * @return audio mute value for PhoneApplication commands
        */
        virtual const TBool& AudioMuteCommand() const = 0;
        
        /**
        * Gets audio output preference
        * @return audio output preference
        */
        virtual const TInt& AudioOutputPreference() const = 0;

        /**
        * Gets audio output command value
        * @return audio output value for PhoneApplication commands
        */
        virtual const TPEAudioOutput& AudioOutputCommand() const = 0;
        
        /**
        * Gets audio output command value
        * @return audio output value for PhoneApplication commands
        */
        virtual TBool ShowNoteCommand() const = 0;
          
        /**
        * Gets audio volume command value
        * @return audio volume value for Phone Application commands
        */
        virtual const TInt& AudioVolumeCommand() const = 0;      

        /**
        * Get remote contact link or id
        * @param aCallId is the call identification number.
        * @return contact link or id
        */
        virtual const CPhCntContactId& ContactId( const TInt aCallId ) const = 0;

        /**
        * Gets dtmf postfix
        * @param aCallId is the call identification number.
        * @return dtmf postfix
        */
        virtual const TPEDtmfString& DtmfPostFix( const TInt aCallId  ) const = 0;     

        /**
        * Gets dtmf string command value
        * @return dtmf string value for Phone Application commands
        */
        virtual const TPEDtmfString& DtmfStringCommand() const = 0;
        
        /**
        * Gets DTMFVoiceMailBoxNumber information for line 1
        * @return line 1 DTMFVoiceMailBoxNumber information
        */
        virtual const TPEPhoneNumber& DTMFVoiceMailBoxNumberLine1() const = 0;

        /**
        * Gets DTMFVoiceMailBoxNumber information for line 2
        * @return line 2 DTMF VoiceMailBoxNumber information
        */
        virtual const TPEPhoneNumber& DTMFVoiceMailBoxNumberLine2() const = 0;
        
        /**
        * Gets error code from TPEBasicInfo-structure
        * @return Error code
        */
        virtual TInt ErrorCode() const = 0;

        /**
        * Gets correct inband tone for specific call state transition or error
        * @return the inband tone to be played
        */
        virtual const TCCPTone& InbandTone() const = 0;
        
        /**
        * Gets call origin for next call attempt
        * @return value indicates if the call is phone, client or SAT originated
        */
        virtual const TPECallOrigin& CallOriginCommand() const = 0;

        /**
         * Reset remote information
        * @param aCallId is the Call identification number.
         */
        virtual void ResetRemoteInfo( const TInt aCallId ) = 0;

        
        /**
         * Gets user to user information. If this is set before dial, the
         * data is sent to network.
         *
         * @return User to user info.
         */
        virtual const TPEUserToUserInfo& UserToUserInformation() const = 0;

        /**
        * Set the group id's which are active to play ring tone.
        * @param aAlertForGroup group id .
        */
        virtual void SetAlertForGroup( const TArray< TContactItemId >& aAlertForGroup ) = 0;  
        
        /**
        * Sets active als line value to the TPEGsmBasicInfo structure.
        * @param aALSLine is the active als line value.
        */
        virtual void SetALSLine( const CCCECallParameters::TCCELineType& aALSLine ) = 0;
             
        /**
        * Sets ALS support value to the TPEGsmBasicInfo structure.
        * @param aALSLineSupport is the ALS support value.
        */
        virtual void SetALSLineSupport( const TBool& aALSLineSupport ) = 0;
   
        /**
        * Sets audio mute state of the phone to TPEBasicInfo-structure
        * @param aAudioMute is audio mute state of the phone (ETrue = mute on)
        */
        virtual void SetAudioMute( const TBool aAudioMute ) = 0;
        
        /**
        * Sets audio output path
        * @param aOutput is the audio previous output path.
        * @param aPreviousOutput is the audio previous output path.
        * @param aShowNote is audio note value
        */
        virtual void SetAudioOutput( const TPEAudioOutput aOutput,
                                     const TPEAudioOutput aPreviousOutput,
                                     const TBool aShowNote ) = 0;

        /**
        * Sets hands free mode of the phone to TPEBasicInfo-structure
        * @param aHandsfreeMode is handsfree mode of the phone(on,off).
        */
        virtual void SetAvailableAudioOutputs( 
            const TArray< TPEAudioOutput >& aAvailableOutputs ) = 0;
        
        // TODO: change TInt param to enum type when it is available
        /**
        * Sets audio output preference to TPEBasicInfo-structure
        * @param aOutputPreference is audio output preference (public/private)
        */
        virtual void SetAudioOutputPreference( const TInt aOutputPreference ) = 0;
        
        /**
        * Sets audio volume of the phone to TPEBasicInfo-structure
        * @param aAudioVolume is audio volume of the phone.
        */
        virtual void SetAudioVolume( const TInt aAudioVolume ) = 0;

        /**
        * Sets alsline value to TPECallInfo-structure
        * @param aALSLine informs which line is selected active
        * @param aCallId is the Call identification number.
        */
        virtual void SetCallALSLine( const CCCECallParameters::TCCELineType& aALSLine, const TInt aCallId ) = 0;
        
        /**
        * Sets phone client information to TPECallInfo-structure
        * @param aClientInformation includes phone client information.
        * @param aCallId is the Call identification number.
        */
        virtual void SetCallClientInformation( 
            const MPEClientInformation& aClientInformation, 
            const TInt aCallId ) = 0;
                       
        /**
        * Sets Call direction to TPECallInfo-structure
        * @param aCallDirection is the Call direction value (0=Unknown/1=MO/2=MT)
        * @param aCallId is the Call identification number.
        */
        virtual void SetCallDirection( 
            const RMobileCall::TMobileCallDirection& aCallDirection, 
            const TInt aCallId ) = 0;
    
        /**
        * Sets call duration of the last call to Idle to TPEBasicInfo-structure
        * @param aDiagnostics is call terminated diagnostics value
        */
        virtual void SetCallDuration( const TInt& aCallDuration ) = 0;       

        /**
        * Sets duration of the call to TPECallInfo-structure
        * @param aDuration is duration of the call. 
        * @param aCallId is the Call identification number.
        */
        virtual void SetCallDuration( 
            const TTimeIntervalSeconds& aCallDuration, 
            const TInt aCallId ) = 0;

        /**
        * Set caller image
        * @param aCallerImage caller image
        * @param aCallId is the call identification number
        */
        virtual void SetCallerImage(
            const TPtrC aCallerImage,
            const TInt aCallId ) = 0;

        /**
        * Set caller text
        * @param aCallerText caller text
        * @param aCallId is the call identification number
        */
        virtual void SetCallerText( const TPtrC aCallerText, const TInt aCallId ) = 0;

        /**
        * Set caller thumbnail
        * @param aCallerThumbnail caller thumbnail
        * @param aCallId is the call identification number
        */
        virtual void SetCallerThumbnail(
            CFbsBitmap* aCallerThumbnail, 
            const TInt aCallId ) = 0;
            
        /**
        * Set status of caller thumbnail 
        * @param aStatus caller thumbnail status
        * @param aCallId is the call identification number
        */
        virtual void SetHasCallerThumbnail( TBool aStatus, const TInt aCallId  ) = 0;
        
        /**
        * Sets start time of the call to TPECallInfo-structure
        * @param aStartTime is the start time of the call.
        * @param aCallId is the Call identification number.
        */
        virtual void SetCallStartTime( const TTime& aCallStartTime, const TInt aCallId ) = 0;
        
        /**
        * Sets Call State value to TPECallInfo-structure
        * @param aCallState is call state value.
        * @param aCallId is the Call identification number.
        */
        virtual void SetCallState( const TPEState& aCallState, const TInt aCallId ) = 0;

        /**
        * Sets call type information
        * @param aCallType is the call type.
        * @param aCallId is the Call identification number.
        */
        virtual void SetCallType( const TPECallType& aCallType, const TInt aCallId ) =  0;

        /**
        * Sets Secure status to call info
        * @param SecureStatus is the secure status (on/off)
        * @param aCallId is the Call identification number.
        */
        virtual void SetCallSecureStatus( const TBool aSecureStatus, const TInt aCallId ) = 0;

        /**
        * Sets call state of a conference call
        * @param aCallState is call state value.
        */
        virtual void SetConferenceCallState( const TPEState& aCallState ) = 0;
        
        /**
        * Set user group ids this contact is assigned to
        * Array is reset if the operation fails.
        * @param aGroupIdArray array of group ids
        * @param aCallId is the Call identification number
        */
        virtual void SetContactGroups(
            TArray< TContactItemId >& aGroupIdArray,
            const TInt aCallId ) = 0;

        /**
        * Set contact link or id
        * @param aContactId pointer to contact id
        * @param aCallId is the Call identification number
        */
        virtual void SetContactId(
            CPhCntContactId* aContactId,
            const TInt aCallId ) = 0;

        /**
        * Sets the name of data port currently on-loan.
        * Data port loaning is handled by CSPlugin and happens when a data
        * call goes into connected state. The dataport is released when VT components
        * indicate it can be released, i.e. video call hangup.
        * @param aName is Data port name
        */
        virtual void SetDataPortName( const TName& aDataPortName ) = 0;
        
        /**
        * Sets dtmf postfix value to TPEBasicInfo-structure
        * @param aDtmfPostFix is persed dtmf postfix. 
        * @param aCallId is the call identification number.
        */
        virtual void SetDtmfPostFix( const TPEDtmfString& aDtmfPostFix, const TInt aCallId ) = 0;
  
        /**
           
        /**
        * Sets dtmf string value to TPEBasicInfo-structure
        * @param aDtmfString is the dtmf string to be sent. 
        */
        virtual void SetDtmfString( const TPEDtmfString& aDtmfString ) = 0;
        
        /**
        * Sets DTMFVoiceMailBoxNumber for Line 1 information from TPEGsmBasicInfo-structure
        * @param aDTMFVoiceMailBoxNumberLine1 includes line 1 DTMFVoiceMailBoxNumber information.  
        */
        virtual void SetDTMFVoiceMailBoxNumberLine1( 
            const TDesC& aDTMFVoiceMailBoxNumberLine1 ) = 0;

        /**
        * Sets DTMFVoiceMailBoxNumber for Line 2 information from TPEGsmBasicInfo-structure
        * @param aDTMFVoiceMailBoxNumberLine2 includes line 2 DTMFVoiceMailBoxNumber information.  
        */
        virtual void SetDTMFVoiceMailBoxNumberLine2( 
            const TDesC& aDTMFVoiceMailBoxNumberLine2 ) = 0;
        
        /**
        * Sets error code of the phone to TPEBasicInfo-structure
        * @param aErrorCode is error code value
        */
        virtual void SetErrorCode( const TInt aErrorCode ) = 0;

        /**
        * Sets correct inband tone for specific call state transition or error
        * @param aTone is the inband tone to be played
        */
        virtual void SetInbandTone( const TCCPTone aTone ) = 0;
            
        /**
        * Sets Keypad volume of the phone to TPEBasicInfo-structure
        * @param aKeypadVolume is Keypad volume of the phone.
        */
        virtual void SetKeypadVolume( const TInt aKeypadVolume ) = 0;
        
        /**
        * Sets a logging indicator to TPECallInfo-tructure
        * @param aLoggingIndicator, Continuous logging indicator.
        * @param aCallId, the call identification number.
        */
        virtual void SetLoggingIndicator( const TBool aLoggingIndicator, const TInt aCallId ) = 0;
       
        /**
        * Set missed call indicator. 
        * @param TBool& aMissedCall, missed call indicator.
        * @param TInt aCallId, call identification.
        */
        virtual void SetMissedCall( const TBool& aMissedCall, const TInt aCallId ) = 0; 
          
        /**
        * Sets network information to TPEBasicInfo-structure
        * @param aNetworkInfo is the current network information.
        */
        virtual void SetNetworkRegistrationStatus( 
            const TNWNetworkRegistrationStatus& aRegistrationStatus ) = 0;

        /**
        * Set personal ringing tone
        * Used to set a ringing tone defined for one of the user groups
        * if personal tone is not found
        * @param aRingingTonepath, personal ringing tone
        * @param aCallId, call identification number
        */
        virtual void SetPersonalRingingTone(
            const TPEContactFileName& aRingingTonepath,
            const TInt aCallId ) = 0;

        /**
        * Sets personal tone status value to TPEBasicInfo-structure.
        * @param aPersonalTone is personal tone status (EFalse/ETrue).
        */
        virtual void SetPersonalToneStatus( const TInt& aPersonalToneStatus ) = 0;
        
        /**
        * Set phone number parsing result
        * @param TBool& aServiceCode, service code indicator
        */
        virtual void SetPhoneNumberIsServiceCode( const TBool& aServiceCode ) = 0;
 
        /**
        * Sets call origin for the next call attempt
        * @param aOrigin indicates if the call is phone, client or SAT originated
        */
        virtual void SetCallOriginCommand( const TPECallOrigin& aOrigin ) = 0;
         
        /**
        * Sets profile id to TPEBasicInfo-structure
        * @param aProfileId is the identification number of the profile
        */
        virtual void SetProfileId( const TInt aProfileId ) = 0;   
        
        /**
        * Sets profile name to TPEBasicInfo-structure
        * @param aProfileName is the name of the profile
        */
        virtual void SetProfileName( const TPEProfileName& aProfileName ) = 0;

        /**
        * Set contact company name
        * @param aCompanyName company name
        * @param aCallId, is the call identification number
        */
        virtual void SetRemoteCompanyName(
            const TPEContactName& aCompanyName, 
            const TInt aCallId ) = 0;

        /**
        * Sets remote identity status
        * @param aIdentity is the status of identity 
        */
        virtual void SetRemoteIdentity( 
            const RMobileCall::TMobileCallRemoteIdentityStatus& aIdentity, 
            const TInt aCallId ) = 0;

        /**
        * Set contact name
        * Depending on user selected language may be in order:
        * first + last name OR last + first name
        * @param aContactName contact name
        * @param aCallId, is the call identification number
        */
        virtual void SetRemoteName(
            const TPEContactName& aContactName, 
            const TInt aCallId ) = 0;

        /**
        * Set remote end phone number
        * @param aPhoneNumber phone number
        * @param aCallId, is the call identification number
        */
        virtual void SetRemotePhoneNumber(
            const TPEPhoneNumber& aPhoneNumber, 
            const TInt aCallId ) = 0;

        /**
        * Set remote end phone number type
        * @param aPhoneNumberType phone number id
        * @param aCallId, is the call identification number
        */
        virtual void SetRemotePhoneNumberType(
            TPEPhoneNumberIdType aPhoneNumberType, 
            const TInt aCallId ) = 0;

        /**
        * Set remote end Text To Speech Text
        * @param aText Text To Speech Text
        * @param aCallId, is the call identification number
        */
        virtual void SetRemoteTextToSpeechText(
            HBufC* aText,
            TInt aCallId ) = 0;

        /**
        * Sets ringing type to TPEBasicInfo-structure
        * @param aRingingToneName is the current ringintone name for line 1. 
        */
        virtual void SetRingingTone1(
            const TPEContactFileName& aRingingToneName ) = 0;
        
        /**
        * Sets ringing type to TPEGsmBasicInfo-structure
        * @param aRingingToneName is the current ringintone name for line 2.
        */
        virtual void SetRingingTone2(
            const TPEContactFileName& aRingingToneName ) = 0;

        /**
        * Sets video call ringing tone name to TPEGsmBasicInfo-structure
        * @param aRingingToneName is the current ringintone name for video call.
        */
        virtual void SetVideoCallRingingTone( const TPEContactFileName& aRingingToneName ) = 0;
                
        /**
        * Sets ringing type to TPEBasicInfo-structure
        * @param aRinginType is the ringintype
        */
        virtual void SetRingingType(
            const TProfileRingingType& aRingingType ) = 0;
        
        /**
        * Sets ringing volume to TPEBasicInfo-structure
        * @param aRingingVolume is the current ringingtone name. 
        */
        virtual void SetRingingVolume( const TInt& aRingingVolume ) = 0;

        /**
        * Sets text-to-speech to TPEBasicInfo-structure
        * @param aTextToSpeech is the current text-to-speech preference setting. 
        */
        virtual void SetTextToSpeech( const TBool aTextToSpeech ) = 0;

        /**
        * Sets two digit support status of the phone to TPEBasicInfo-structure
        * @param aTwoDigitSupportStatus is support status (EFalse/ETrue).                                                      
        */
        virtual void SetTwoDigitSupportStatus(
            const TBool aTwoDigitSupportStatus ) = 0;
   
        /**
        * Gets VoiceMailBoxNumber for Line 1 information from TPEGsmBasicInfo-structure
        * @param aVMBXL1 includes line 1 VoiceMailBoxNumber information.  
        */
        virtual void SetVoiceMailBoxNumberLine1(
            const TDesC& aVoiceMailBoxNumberLine1 ) = 0;

        /**
        * Gets VoiceMailBoxNumber for Line 2 information from TPEGsmBasicInfo-structure
        * @param aVMBXL2 includes line 2 VoiceMailBoxNumber information.  
        */
        virtual void SetVoiceMailBoxNumberLine2(
            const TDesC& aVoiceMailBoxNumberLine2 ) = 0;

        /**
        * Set log forced completion indicator. 
        * @since Series60_4.0
        * @param TBool aForcedCOmpletion, log forced completion indicator.
        * @param TInt aCallId, call identification.
        */
        virtual void SetLogForcedCompletion(  
            const TBool aForcedCompletion, 
            const TInt aCallId ) = 0;

        /**
        * Get log forced completion indicator. 
        * @since Series60_4.0
        * @param TInt aCallId, call identification.
        * @return TBool&, log forced completion indicator.
        */
        virtual TBool LogForcedCompletion(
            const TInt aCallId ) const = 0;
        
        /**
        * Sets whole outgoing phone number 
        * @since Series60_4.0
        * @param aPhoneNumber is the phone number
        * @param TInt aCallId, call identification.
        */
        virtual void SetWholeOutgoingPhoneNumber( 
            const TPEPhoneNumber& aPhoneNumber, 
            const TInt aCallId ) = 0;

        /**
        * Gets whole outgoing phone number
        * @since Series60_4.0
        * @param TInt aCallId, call identification.
        * @return TPEPhoneNumber&, remote party number including DTMF string.
        */
        virtual const TPEPhoneNumber& WholeOutgoingPhoneNumber(
            const TInt aCallId ) const = 0;
       
        /**
        * Returns the service-id used for the command.
        * @since S60 3.2
        * @param none
        * @return TUint32 Service-id
        */
        virtual TUint32 ServiceIdCommand() const = 0;
        
        /**
        * Set the service-id for the call.
        * @since S60 3.2
        * @param TUint32 Service-id
        * @return none
        */
        virtual void SetServiceId( const TInt aCallId, TUint32 aServiceId ) = 0;
        
        /**
        * Sets predefined dtmf strings
        * @param aArray, is reference to string array
        * @param aCallId, is the call identification number
        */
        virtual void SetPredefinedDtmfStrings(
            const CDesCArray& aArray, 
            const TInt aCallId ) = 0;

         /**
         * ResumeHeldCall
         * @param aCallId is the call identification number.
         * @return TBool.
         */
         virtual TBool ResumeHeldCall( const TInt aCallId ) const = 0;
        
         /**
         * Resets TPECallInfo parameters.
         * @param aCallId is the Call identification number.
         * @return None.
         */
         virtual void ResetCallInfo( const TInt aCallId ) = 0;

         /**
         * Sets Barring information to TPEGsmBasicInfo-structure
         * @param aBarringInfo includes call Barring information
         * @return
         */
         virtual void SetBarringInfo( const TPEBarringInfo& aBarringInfo ) = 0;
         
         /**
         * Sets Confrence value to TPECallInfo-structure
         * @param aCallConference informs is the call a member of conference call.
         * @param aCallId is the Call identification number.
         * @return None.
         */
         virtual void SetCallConference( const TInt aCallConference, const TInt aCallId ) = 0;
         
         /**
         * Sets Call control caps to TPECallInfo-structure
         * @param aCallControlCaps the value of the call control caps
         * @param aCallId is the Call identification number.
         * @return None.
         */
         virtual void SetCallControlCaps( 
             const TPECallControlCaps& aCallControlCaps, 
             const TInt aCallId ) = 0;        
         
         /**
         * Sets forwarded value to TPECallInfo-structure
         * @param aForwarded Indicates if incoming call was forwarded
         * @param aCallId is the Call identification number.
         * @return None.
         */
         virtual void SetCallForwarded( const TBool aCallForwarded, const TInt aCallId ) = 0;

         /**
         * Sets Call service to TPECallInfo-structure
         * @param aCallService is the Call service value.
         * @param aCallId is the Call identification number.
         * @return None.
         */
         virtual void SetCallService( 
             const RMobilePhone::TMobileService& aCallService, 
             const TInt aCallId ) = 0;
         
      
         /**
         * Sets Conference capabilities to TPECallInfo structure.
         * @param aCaps, conference capabilities
         * @param aCallId is the Call identification number.
         * @return None.
         */
         virtual void SetConferenceCallCaps( const TUint32 aCaps, const TInt aCallId ) = 0;        
         
         /**
         * Sets added or removed members TName information to TPECallInfo structure
         * @param aName is the TName information of added or removed member.
         * @param aCallId is the Call identification number.
         * @return None.
         */
         virtual void SetConferenceMemberInfo( 
             const TName& aConferenceMemberName, 
             const TInt aConferenceMemberCallId, 
             const TInt aCallId ) = 0;

         /**
         * Sets deflect information to the TPEGsmBasicInfo structure.
         * @param aActiveForward is the active forward information.
         * @return None.
         */
         virtual void SetDeflectInfo( const TPEActiveForwardInfo& aActiveForward ) = 0;    

         /**
         * Set iResumeHeldCall value to TPEGsmCallInfo structure
         * @param aResumeHeldCall
         * @param aCallId is the Call identification number.
         * @return None.
         */
         virtual void SetResumeHeldCall( 
             const TBool aResumeHeldCall, 
             const TInt aCallId ) = 0;
     
         /**
         * Sets the number of conference members to TPEGsmCallInfo structure
         * @param aNumberOfMembers, number of conference members
         * @param aCallId is the Call identification number.
         * @return None.
         */
         virtual void SetNumberOfConferenceMembers( 
             const TInt aNumberOfMembers, 
             const TInt aCallId ) = 0;

         /**
         * Set remote end connected phone number
         * @param aColpNumber phone number
         * @param aCallId, is the call identification number.
         */
         virtual void SetRemoteColpNumber(
             const TPEPhoneNumber& aColpNumber, 
             const TInt aCallId ) = 0;

         /**
         * Sets Sim state to TPEGsmBasicInfo-structure
         * @param aSimState is the Sim state
         * @return None.
         */
         virtual void SetSimState( const TPESimState& aSimState ) = 0;

         /**
         * Sets Suplementary Services command information to TPESSCommandInfo-structure
         * @param aSSCommandInfo includes Suplementary Services command information.  
         * @return None.
         */
         virtual void SetSSCommandInfo( const TPESSCommandInfo& aSSCommandInfo ) = 0;

        /**
         * Sets user to user information.
         * @since S60 5.0
         * @param aUUI User to user info.
         */
        virtual void SetUserToUserInformation( const TPEUserToUserInfo& aUUI ) = 0;
                
        /**
         * Sets remote party name given by network to TPECallInfo structure.
         * @param aName Remote party name.
         */
        virtual void SetRemotePartyName( 
            const TPEContactName& aName, 
            const TInt aCallId ) = 0;

        /**
         * Sets secure specified status.
         * @param aSecureSpecified Secure specified status.
         */
        virtual void SetSecureSpecified( const TBool aSecureSpecified ) = 0;
        
        /**
        * Gets call parameters
        * @return None.
        */
        virtual CCCECallParameters& CallParameters() = 0;
                
        /**
         * Sets call index of a call.
         * @param aCallIndex Call index
         * @param aCallId call identification
         */
        virtual void SetCallIndex( TInt aCallIndex, TInt aCallId ) = 0;
        
        /**
         * Gets call index of a call.
         * @param aCallId Call identification
         * @return Call index of a call.
         */
        virtual TInt CallIndex( TInt aCallId ) const = 0;
        
        /**
         * Sets call origin for the call
         * @param aOrigin indicates if the call is phone, client or SAT originated
         */
        virtual void SetCallOrigin( TPECallOrigin aOrigin, TInt aCallId ) = 0;

    };

#endif      //MPEDATASTORE_H

