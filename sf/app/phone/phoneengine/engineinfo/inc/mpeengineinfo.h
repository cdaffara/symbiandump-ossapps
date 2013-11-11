/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface class for MPEEngineInfo of the PhoneEngine
*
*/



#ifndef MPEENGINEINFO_H
#define MPEENGINEINFO_H

//  INCLUDES
#include <pevirtualengine.h>
#include "mpecallsettersif.h" 

// CONSTANTS
// None

// FORWARD DECLARATIONS
class MPEClientInformation;

// DATA TYPES
// None

// CLASS DECLARATION

/**
*  Offers engine info interface to phone application
*
*  @lib phoneengine.lib
*  @since Series 60 4.0
*/
NONSHARABLE_CLASS( MPEEngineInfo ) : public MPECallSettersIF
    {

    public:
        /**
        * Gets active als line of the phone from TPEBasicInfo-structure
        * @return active als line of the phone
        */
        virtual const CCCECallParameters::TCCELineType& ALSLine() const = 0;
            
        /**
        * Gets ALS support of the phone from TPEBasicInfo-structure
        * @return ETrue if als is supported.
        */
        virtual const TBool& ALSLineSupport() const = 0;

        /**
        * Gets group ids which are active to play ring tone
        * @return contact group id
        */
        virtual const TArray< TContactItemId > AlertForGroup() const = 0;
    
        /**
        * Gets audio mute value
        * @return audio mute value
        */
        virtual const TBool& AudioMute() const = 0;

        /**
        * Gets audio volume value
        * @return audio volume value
        */
        virtual const TInt& AudioVolume() const = 0;
        
        /**
        * Gets current audio output path
        * @return audio output path
        */
        virtual const TPEAudioOutput& AudioOutput() const = 0;
        
        /**
        * Gets phone audio route parameters
        * @return phone audio route parameters. 
        */
        virtual const TPEPhoneAudioRouteParameters& RouteParameters() const = 0;  
  
        /**
        * Gets audio output path availability
        * @return audio output path availability status
        */
        virtual TBool AudioOutputAvailable( 
            const TPEAudioOutput aOutput ) const = 0;

        /**
        * Gets line information for the specified call
        * @param aCallId is the call identification number
        * @return line information for the specified call
        */
        virtual CCCECallParameters::TCCELineType CallALSLine( const TInt aCallId ) const = 0;

        /**
        * Gets phone client information of the specified call
        * @param aCallId is the call identification number
        * @return client information of the specified call
        */
        virtual const MPEClientInformation& CallClientInformation( const TInt aCallId ) const = 0;

        /**
        * Gets call direction of the specified call
        * @param aCallId is the call identification number
        * @return call direction of the specified call
        */
        virtual RMobileCall::TMobileCallDirection CallDirection( 
            const TInt aCallId ) const = 0;

        /**
        * Gets duration of the last call to go idle
        * @return call duration of the last active call
        */
        virtual const TInt& CallDuration() const = 0;

        /**
        * Gets call duration of the ongoing or last active call with specified id
        * @param aCallId is the call identification number
        * @return call duration of the ongoing or last active call
        */
        virtual TTimeIntervalSeconds CallDuration( const TInt aCallId ) const = 0;

        /**
        * Get caller image
        * @param aCallId is the call identification number.
        * @return caller image
        */
        virtual const TPtrC CallerImage( const TInt aCallId ) const = 0;

        /**
        * Get caller text
        * @param aCallId is the call identification number.
        * @return caller text
        */
        virtual const TPtrC CallerText( const TInt aCallId ) const = 0;

        /**
        * Get caller thumbnail
        * @param aCallId is the call identification number.
        * @return pointer to thumbnail data
        */
        virtual CFbsBitmap* CallerThumbnail(
            const TInt aCallId ) const = 0;
        
        /**
        * Get caller thumbnail status
        * @return ETrue if has thumbnail, otherwise EFalse.
        */
        virtual TBool HasCallerThumbnail( const TInt aCallId  ) const = 0;

        /**
        * Gets identification number of a call
        *
        * This value is used for determining the target for a command message
        * that requires such information
        * @return identification number of a call
        */
        virtual const TInt& CallId() const = 0;

        /**
        * Gets start time of the call
        * @param aCallTime is the start time of the call
        * @param aCallId is the call identification number
        * @return None.
        */
        virtual const TTime& CallStartTime( const TInt aCallId ) const = 0;

        /**
        * Gets call state of a specified call
        * @param aCallId is the call identification number
        * @return call state of a specified call
        */
        virtual TPEState CallState( const TInt aCallId ) const = 0;

        /**
        * Gets type of the specified call
        * @param aCallId is the call identification number.
        * @return call type of a specified call
        */
        virtual TPECallType CallType( const TInt aCallId ) const = 0;

        /**
        * Gets last set call type information
        * @return call type information
        */
        virtual const TPECallType& CallTypeCommand() const = 0;

        /**
        * Gets secure status from call info
        * @return TBool the secure status (on/off)
        */
        virtual TBool IsSecureCall( const TInt aCallId ) const = 0;

        /**
        * Gets call state of a conference call
        * @return call state
        */
        virtual TPEState ConferenceCallState() const = 0;

        /**
        * Get user group ids this contact is assigned to
        * @param aCallId is the call identification number.
        * @return array for group ids
        */
        virtual const TArray< TContactItemId > ContactGroups(
            const TInt aCallId ) const = 0;

        /**
        * Get remote contact link packet. See virtual phonebook API.
        * @param aCallId is the call identification number.
        * @return Virtual phonebook contact link packet.
        */
        virtual const TDesC8& ContactLink( const TInt aCallId ) const = 0;
        
        /**
        * Gets the name of data port currently on-loan.
        * Data port loaning is handled by CSPlugin and happens when a data
        * call goes into connected state. The dataport is released when VT components
        * indicate it can be released, i.e. video call hangup.
        * @return data port name
        */
        virtual const TName& DataPortName() const = 0;

        /**
        * Gets dtmf string
        * @return dtmf string
        */
        virtual const TPEDtmfString& DtmfString() const = 0;

        /**
        * Gets error code and cause values for the latest error
        * @return error code and cause values for the latest error
        */
        virtual const TPEErrorInfo& ErrorInfo() const = 0;

        /**
        * Gets key code of the last key press
        * @return key code of the last key press
        */
        virtual const TChar& KeyCode() const = 0;

        /**
        * Gets keypad volume value
        * @return keypad volume value
        */
        virtual const TInt& KeypadVolume() const = 0;

        /**
        * Gets a logging indicator
        * @param aCallId, call identification number     
        * @return continuous logging indicator
        */
        virtual TBool LoggingIndicator( const TInt aCallId ) const = 0;

        /**
        * Get missed call indicator
        * @param aCallId, call identification number
        * @return missed call indicator
        */
        virtual TBool MissedCall( const TInt aCallId ) const = 0;

        /**
        * Gets network registration status
        * @return TNWNetworkRegistrationStatus Network registration status
        */
        virtual const TNWNetworkRegistrationStatus& NetworkRegistrationStatus() const = 0;

        /**
        * Get ringing tone
        * Returns a ringing tone defined for one of the user groups
        * if personal tone is not found
        * @param aCallId, call identification number
        * @return ringing tone file path
        */
        virtual const TPEContactFileName& PersonalRingingTone(
            const TInt aCallId ) const = 0;

        /**
        * Gets personal tone status
        * @return personal tone status
        */
        virtual const TBool& PersonalToneStatus() const = 0;

        /**
        * Gets phone number
        * @return Reference to TPEPhoneNumber, containing the phone number of the remote party.
        */
        virtual const TPEPhoneNumber& PhoneNumber() const = 0;

        /**
        * Gets phone number parsing result
        * @return ETrue if phone number is a service code
        */
        virtual const TBool& PhoneNumberIsServiceCode() const = 0;

        /**
        * Gets profile id
        * @return profile id
        */
        virtual const TInt& ProfileId() const = 0;

        /**
        * Gets profile name
        * @return profile name.  
        */
        virtual const TPEProfileName& ProfileName() const = 0;

        /**
        * Gets contact company name
        * @param aCallId, call identification number
        * @return company name
        */
        virtual const TPEContactCompany& RemoteCompanyName(
            const TInt aCallId ) const = 0;

        /**
        * Gets remote party identity 
        * @param aCallId is the Call identification number
        * @return remote party identity
        */
        virtual RMobileCall::TMobileCallRemoteIdentityStatus RemoteIdentity ( 
            const TInt aCallId ) const = 0;

        /**
        * Gets contact name
        * Depending on user selected language may be in order:
        * first + last name OR last + first name
        * @param aCallId, call identification number
        * @return contact name
        */
        virtual const TPEContactName& RemoteName( const TInt aCallId ) const = 0;

        /**
        * Gets remote end phone number
        * @param aCallId, call identification number
        * @return phone number
        */
        virtual const TPEPhoneNumber& RemotePhoneNumber(
            const TInt aCallId ) const = 0;

        /**
        * Gets remote end phone number type
        * @param aCallId, call identification number
        * @return phone number type
        */
        virtual const TPEPhoneNumberIdType& RemotePhoneNumberType(
            const TInt aCallId ) const = 0;

        /**
        * Gets text to speech text for this contact. Used in
        * text to speech ringing tone.
        * @param aCallId, call identification number
        * @return descriptor, ownership passed.
        */
        virtual const TPtrC RemoteTextToSpeechText(
            const TInt aCallId ) const = 0;

        /**
        * Gets remote predefined dtmf strings
        * @param aCallId, call identification number
        * @return reference to string array
        */
        virtual const CDesCArray&  RemotePredefinedDtmfStrings(
            const TInt aCallId ) const = 0;                 
 
        /**
        * Gets ringing type
        * @return ringing type
        */
        virtual const TProfileRingingType& RingingType() const = 0; 

        /**
        * Gets ringing tone name
        * @param aCallId, call identification number 
        * @return name of the current ringing tone
        */
        virtual const TPEContactFileName& RingingTone( const TInt& aCallId ) const = 0;
        
        /**
        * Gets current ringing volume
        * @return the current ringing volume
        */
        virtual const TInt& RingingVolume() const = 0;       

        /**
        * Gets text-to-speech preference setting value
        * @return text-to-speech status (EFalse/ETrue)
        */
        virtual const TBool& TextToSpeech() const = 0;

        /**
        * Gets VoiceMailBoxNumber for Line 1 information from TPEBasicInfo-structure
        * @return line 1 VoiceMailBoxNumber information
        */
        virtual const TPEPhoneNumber& VoiceMailBoxNumberLine1() const = 0;

        /**
        * Gets VoiceMailBoxNumber for Line 2 information from TPEBasicInfo-structure
        * @return line 2 VoiceMailBoxNumber information
        */
        virtual const TPEPhoneNumber& VoiceMailBoxNumberLine2() const = 0;

        /**
        * Sets audio mute command to TPEBasicInfo-structure
        * @param aAudioMute is audio mute
        * @return None.
        */
        virtual void SetAudioMuteCommand( const TInt& aAudioMute ) = 0;
        
        /**
        * Sets audio output command to TPEBasicInfo-structure
        * @param aOutput is audio output path
        * @param aShowNote is audio note value
        * @return None.
        */
        virtual void SetAudioOutputCommand( 
            const TPEAudioOutput& aOutput,
            const TBool aShowNote  ) = 0;

        /**
        * Sets audio volume command to TPEBasicInfo-structure
        * @param aAudioVolume is audio volume
        * @return None.
        */
        virtual void SetAudioVolumeCommand( const TInt& aAudioVolume ) = 0;
        
        /**
        * Sets CallId value to TPECallCommandInfo-structure
        * @param aCallId is the call id number.
        * @return None.
        */
        virtual void SetCallId( const TInt& aCallId ) = 0;

        /**
        * Sets call type information for following command message
        * Used for specifying the call type of the next attempted call
        * @param aCallType is the call type of the next attempted call
        */
        virtual void SetCallTypeCommand( const TPECallType& aCallType ) =  0;

        /**
        * Sets dtmf string value to TPEBasicInfo-structure
        * @param aDtmfString is the dtmf string to be sent. 
        * @return None.
        */
        virtual void SetDtmfStringCommand( const TPEDtmfString& aDtmfString ) = 0;

        /**
        * Sets key code value to TPEBasicInfo-structure
        * @param aKeyCode is the key code value.
        * @return None.
        */
        virtual void SetKeyCode( const TChar& aKeyCode ) = 0; 

        /**
        * Sets phone number to TPECallCommandInfo-structure
        * @param aPhoneNumber is the phone number
        * @return None.
        */
        virtual void SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber ) = 0;

        /**
        * Gets two digit support status value
        * @return two digit support status (EFalse/ETrue)
        */
        virtual const TBool& TwoDigitSupportStatus() const = 0;
     
        /**
        * Returns the service-id used for the command.
        * @return TUint32 ServiceId
        */
        virtual TUint32 ServiceIdCommand() const = 0;
        
        /**
        * SetServiceIdCommand
        * @return None
        */
        virtual void SetServiceIdCommand( TUint32 aServiceId ) = 0;
        
        /**
        * Gets ServiceId
        * @return ServiceId
        */
        virtual TUint32 ServiceId( const TInt aCallId ) const = 0;

        /**
        * Gets active forwarding information from TPEBasicInfo-structure
        * @return deflect information
        */
        virtual const TPEActiveForwardInfo& ActiveForwardingInfo() const = 0;

        /**
        * Gets barring information from TPEBasicInfo-structure
        * @return barring information
        */
        virtual const TPEBarringInfo& BarringInfo() const = 0;

        /**
        * Gets conference value from TPECallInfo-structure
        * @param aCallId is the call identification number
        * @return call conference value
        */
        virtual TInt CallConference( const TInt aCallId ) const = 0;

        /**
        * Gets call control caps from the TPECallInfo-structure
        * @param aCallId is the call identification number
        * @return call control capabilities
        */
        virtual TPECallControlCaps CallControlCaps( const TInt aCallId ) const = 0;

        /**
        * Gets forwarded value from TPECallInfo-structure
        * @param aCallId is the call identification number
        * @return is call forwarded or not
        */
        virtual TBool CallForwarded( const TInt aCallId ) const = 0;

        /**
        * Gets Service information from TPECallInfo-structure
        * @param aCallId is the call identification number.
        * @return mobile call service type
        */
        virtual RMobilePhone::TMobileService CallService( const TInt aCallId ) const = 0;

        /**
        * Gets Conference call capabilities
        * @param aCallId is the call identification number
        * @return conference call capabilities
        */
        virtual TUint32 ConferenceCallCaps( const TInt aCallId ) const = 0;

        /**
        * Gets Conference call capabilities
        * @param aCallId is the call identification number
        * @return conference call member name
        */
        virtual const TName& ConferenceMemberName( const TInt aCallId ) const = 0;

        /**
        * Gets Conference call capabilities
        * @param aCallId is the call identification number
        * @return conference call member id
        */
        virtual TInt IsConferenceMemberId( const TInt aCallId ) const = 0;

        /**
        * Gets emergency number to be checked for validity
        * @return emergency number to be checked for validity
        */
        virtual const TPEPhoneNumber& EmergencyNumber() const = 0;

        /**
        * Gets number of the conference members
        * @param aCallId is the call identification number
        * @return number of conference members
        */
        virtual TInt NumberOfConferenceMembers( const TInt aCallId ) const = 0;

        /**
        * Get remote end connected phone number
        * @param aCallId, call identification number
        * @return phone number
        */
        virtual const TPEPhoneNumber& RemoteColpNumber(
            const TInt aCallId ) const = 0;

        /**
        * Gets Sim state
        * @return current Sim state
        */
        virtual const TPESimState& SimState() const = 0;

        /**
         * Gets secure specified status
         * @return ETrue if secure specified
         */
        virtual TBool SecureSpecified( ) const = 0;        
        
        /**
        * Gets the last suplementary services string command information
        * @return Suplementary Services command information.  
        */
        virtual const TPESSCommandInfo& SSCommandInfo() const = 0;
    
        /**
        * Sets emergency numbers of the phone to TPEBasicInfo-structure
        * @param aEmergencyNumber for emergency number checking.
        * @return None.
        */
        virtual void SetEmergencyNumber( const TPEPhoneNumber& aEmergencyNumbers ) = 0;
        
        /**
         * Gets remote party name. Contains possible recieved CNAP name or 
         *  received skype identification
         * @param aCallId, Call identification number.
         * @return Name of the remote party detemined by network.
         */
        virtual const TPEContactName& RemotePartyName( const TInt aCallId ) const = 0;
        
        /**
         * Returns transfer target from the last received unattended transfer 
         * request or KNullDesC if transfer target is not available.
         * 
         * @return  Transfer target.
         */
        virtual const TPEPhoneNumber& UnattendedTransferTarget( 
            TInt aCallId ) const = 0;
        
        /**
         * Sets transfer target from the last received unattended transfer request.
         *
         * @param   aTarget     Transfer target to set.
         * @param   aCallId     Call identification number.
         */
        virtual void SetUnattendedTransferTarget( 
            const TPEPhoneNumber& aTarget, TInt aCallId ) = 0;

        /**
         * Returns address choices from received call forward request 
         * or NULL if addresses are not available.
         * 
         * @param   aCallId     Call identification number.
         * @return  Address array.
         */
        virtual const CDesC8Array* ForwardAddressChoices( TInt aCallId ) const = 0;
        
        /**
         * Sets address choices from received call forward request.
         *
         * @param   aArray      Address array.
         * @param   aCallId     Call identification number.
         */
        virtual void SetForwardAddressChoices( const CDesC8Array* aArray,
            TInt aCallId ) = 0;
            
        /**
         * Sets array index for user selected address.
         *
         * @param   aIndex     Array index for selected address.
         */
        virtual void SetForwardAddressIndex( TInt aIndex ) = 0;
        
        /**
         * Returns array index for user selected address.
         * Address is used for call forwarding.
         *
         * @return Array index for selected address.
         */
        virtual TInt ForwardAddressIndex() = 0;

        /**
         * Sets transfer target address for unattended transfer
         * @param aTransferTarget Target address for unattended transfer
         */
        virtual void SetTransferTargetCommand( const TPEPhoneNumber& aTransferTarget ) = 0;

        /**
        * Returns transfer target address
        * @return Transfer target address set with SetTransferTargetCommand
        */
        virtual const TPEPhoneNumber& TransferTargetCommand() const = 0;
                
        /**
        * Checks if given state can be found.
        * @param aCallState state to be checked.
        */
        virtual TBool CheckIfCallStateExists( const TPEState& aCallState )= 0;
        
        /**
         * Returns call origin for the call
         * @param aOrigin indicates if the call is phone, client or SAT originated
         */
        virtual TPECallOrigin CallOrigin( const TInt aCallId ) const = 0;
        
        /**
         * Returns Phonebook contact identifier.
         */
        virtual TInt ContactId2 () const = 0;
        
        /**
        * Sets the protocol spesific error code
        * @param aError is the error code from protocol.
        * @return None.
        */
        virtual void SetProtocolError( TInt aError, TInt aCallId ) = 0; 
 
        /**
        * Returns the protocol spesific error code
        * @return Error code.
        */
        virtual TInt ProtocolError( TInt aCallId ) = 0; 
        
        /**
        * Returns flag if the outgoing barring is activated.
        * @return ETrue if barring activated.
        */
        virtual TBool IsOutgoingCallBarringActivated() = 0;
        
        /**
        * Sets the flag if the outgoing barring is activated.
        * @return ETrue if barring activated.
        */
        virtual void SetOutgoingCallBarringActivated( 
                TBool aActivated ) = 0;
        
        /**
        * Sets incoming call forwarded flag.
        * @param aForwarded is call forwarded
        */
        virtual void SetIncomingCallForwarded( 
                TBool aForwarded, TInt aCallId ) = 0;

        /**
         * Returns incoming call forwarded flag.
         */
        virtual TBool IncomingCallForwarded( TInt aCallId ) const = 0;
        
    }; // MPEEngineInfo
    
#endif      //MPEENGINEINFO_H
    
// End of File
    
