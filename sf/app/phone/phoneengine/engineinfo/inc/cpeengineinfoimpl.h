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
* Description:  This file contains the header file of the CPEEngineInfoImpl object.
*
*/

#ifndef CPEENGINEINFOIMPL_H
#define CPEENGINEINFOIMPL_H

//INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <ccpdefs.h>
#include <pevirtualengine.h>
#include "cperemoteinfo.h"
#include "mpedatastore.h"
#include "cpeengineinfo.h"

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
NONSHARABLE_STRUCT( TPECallCommandInfo )                  // For Mobile Originated Call Commands
    {
    TChar iKeyCode;                         // pressed key code
    TInt iAudioMute;                        // state of microphone ( 0 = false, 1 = true )
    TInt iAudioVolume;                      // Audio volume
    TPEAudioOutput iAudioOutput;            // audio output path
    TBool iShowNote;                        // Status of call audio route info note.
    TInt iCallId;                           // Calls ID
    TPEDtmfString iDtmfString;              // Dtmf string to be sent.
    TPEPhoneNumber iPhoneNumber;            // Dialed Phonenumber, set by client
    TPECallType iCallType;                  // Type of the attempted call
    TUint32 iServiceId;                     // ServiceId    
    TPECallOrigin iCallOrigin;              // Origin of the attempted call (Phone, client or SAT)
    RMobileCall::TMobileCallIdRestriction iIdRestrict; // Call Id restriction setting to be used for this call
    TPEPhoneNumber iEmergencyNumber;        // For checking is this number emergency number.
    TPEUserToUserInfo iUserToUserInfo;      // User to user information.    
    TPEPhoneNumber iTransferToAddress;      // Unattended transfer target address   
    TInt iForwardToAddressIndex;            // Array index for a address to be forwarded
    };

NONSHARABLE_STRUCT( TPEBasicInfo )                         // Contains information of the phone related variables 
    {
    TBool iAudioMute;                       // state of microphone.                 
    TBool iTwoDigitSupportStatus;           // Two Digit support status
    TBool iALSLineSupport;                  // ALS line support.                
    TInt iAudioOutputPreference;            // Audio output preference (private/public) - TODO: change type to enum when available
    TInt iAudioVolume;                      // Audio volume                 
    TInt iKeypadVolume;                     // Keypad volume                    
    TInt iLastCallDuration;                 // Call duration (seconds)                  
    TInt iPersonalToneStatus;               // Personal tone status                 
    TInt iProfileId;                        // the id number of the profile                 
    TInt iRingingVolume;                    // Current ringing volume                   
    TBool iTextToSpeech;                    // Current text-to-speech setting
    TBool iPhoneNumberIsServiceCode;        // flag indicating if the phone number is a service code
    TNWNetworkRegistrationStatus iNetworkRegistrationStatus;                    
    CCCECallParameters::TCCELineType iALSLine;        // includes active als line.                  
    TPEContactFileName iRingingToneName1;   // Current ringingtone name                 
    TPEContactFileName iRingingToneName2;
    TPEContactFileName iVideoCallRingingTone; // Video call ringingtone name                                    
    TPEDtmfString iDtmfString;              // Dtmf string.                 
    TPEErrorInfo iErrorInfo;                // possible error code                  
    TPEProfileName iProfileName;            // the name of the profile                  
    TProfileRingingType iRingingType;       // Includes current ringing type                    
    TPEVoiceMailBoxNumberInfo iVMBXNumbers; // Voice mail box numbers
    TPEPhoneAudioRouteParameters iPhoneAudioRouteParameters; // the phone audio route parameters
    TCCPTone iInbandTone;                   // Inband tone needed to play in a call state transition or error
    TPEActiveForwardInfo iActiveForward;    // active forward information.
    TPEBarringInfo iBarringInfo;            // includes call barring information.
    TPESimState iSimState;
    TPESSCommandInfo iSSCommandInfo;        // Contains information of the Suplementary Services command.
    TBool iSecureSpecified;                 // Secure specified status
    TName iDataPortName;                    // the name of data port currently on-loan    
    TBool iOutgoingBarringActivated;        // Outgoing barring activated 
    };

NONSHARABLE_STRUCT( TPEConferenceCallInfo ) // Contains conference call related variables 
    {
    TPEState iConferenceCallState;
    };

class CPECallInfo;

// CLASS DECLARATION

/**
*  Handles telephony related information structures.
*  
*
*  @lib engineinfo.lib
*  @since Series60_4.0
*/
NONSHARABLE_CLASS( CPEEngineInfoImpl )
        : public CPEEngineInfo
{
    public:     //Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        static CPEEngineInfoImpl* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CPEEngineInfoImpl();
        
    public:     //New functions

        // Functions from MPEEngineInfo

        /**
        * Gets active als line of the phone from TPEBasicInfo-structure
        * @return active als line of the phone
        */
        const CCCECallParameters::TCCELineType& ALSLine() const;
        
        /**
        * Gets ALS support of the phone from TPEBasicInfo-structure
        * @return ETrue if als is supported.
        */
        const TBool& ALSLineSupport() const;

        /**
        * Gets audio mute value
        * @return audio mute value
        */
        const TBool& AudioMute() const;

        /**
        * Gets current audio output path
        * @return audio output path
        */
        const TPEAudioOutput& AudioOutput() const;
        
        /**
        * Gets phone audio route parameters
        * @return phone audio route parameters. 
        */
        const TPEPhoneAudioRouteParameters& RouteParameters() const;
                
        /**
        * Gets audio output path availability
        * @return audio output path availability status
        */
        TBool AudioOutputAvailable( 
            const TPEAudioOutput aOutput ) const;

        /**
        * Gets audio output preference
        * @return audio output preference
        */
        const TInt& AudioOutputPreference() const;

        /**
        * Gets audio volume value
        * @return audio volume value
        */
        const TInt& AudioVolume() const;

        /**
        * Gets call direction of the specified call
        * @param aCallId is the call identification number
        * @return call direction of the specified call
        */
        RMobileCall::TMobileCallDirection CallDirection( 
            const TInt aCallId ) const;

        /**
        * Gets duration of the last call to go idle
        * @return call duration of the last active call
        */
        const TInt& CallDuration() const;

        /**
        * Gets call duration of the ongoing or last active call with specified id
        * @param aCallId is the call identification number
        * @return call duration of the ongoing or last active call
        */
        TTimeIntervalSeconds CallDuration( const TInt aCallId ) const;
        
        /**
        * Gets line information for the specified call
        * @param aCallId is the call identification number
        * @return line information for the specified call
        */
        CCCECallParameters::TCCELineType CallALSLine( const TInt aCallId ) const;

        /**
        * Gets call origin for next call attempt
        * @return value indicates if the call is phone, client or SAT originated
        */
        const TPECallOrigin& CallOriginCommand() const;

        /**
        * Gets start time for the specified call
        * @param aCallId is the call identification number
        * @return time of the call
        */
        const TTime& CallStartTime( const TInt aCallId ) const;

        /**
        * Gets state of the specified call
        * @param aCallId is the call identification number.
        * @return call state of a specified call
        */
        TPEState CallState( const TInt aCallId ) const;

        /**
        * Gets type of the specified call
        * @param aCallId is the call identification number.
        * @return call state of a specified call
        */
        TPECallType CallType( const TInt aCallId ) const;

        /**
        * Gets call state of a conference call
        * @return call state
        */
        TPEState ConferenceCallState() const;
        
        /**
        * Gets the name of data port currently on-loan.
        * Data port loaning is handled by CSPlugin and happens when a data
        * call goes into connected state. The dataport is released when VT components
        * indicate it can be released, i.e. video call hangup.
        * @return data port name
        */
        const TName& DataPortName() const;

        /**
        * Gets secure status from call info
        * @param aCallId is the call identification number.
        * @return TBool the secure status (on/off)
        */
        TBool IsSecureCall( const TInt aCallId ) const;
        
        /**
        * Gets dtmf postfix
        * @param aCallId is the call identification number.
        * @return dtmf postfix
        */
        const TPEDtmfString& DtmfPostFix( const TInt aCallId ) const;
        
        /**
        * Gets dtmf string
        * @return dtmf string
        */
        const TPEDtmfString& DtmfString() const;

        /**
        * Gets error code and cause values for the latest error
        * @return error code and cause values for the latest error
        */
        const TPEErrorInfo& ErrorInfo() const;

        /**
        * Gets correct inband tone for specific call state transition or error
        * @return the inband tone to be played
        */
        const TCCPTone& InbandTone() const;

        /**
        * Gets network registration status
        * @return TNWNetworkRegistrationStatus Network registration status
        */
        const TNWNetworkRegistrationStatus& NetworkRegistrationStatus() const;

        /**
        * Gets phone number
        * @return Reference to TPEPhoneNumber, containing the phone number of the remote party.
        */
        const TPEPhoneNumber& PhoneNumber() const;

        /**
        * Gets phone number parsing result
        * @return ETrue if phone number is a service code
        */
        const TBool& PhoneNumberIsServiceCode() const;
        
        /**
        * Gets profile id
        * @return profile id
        */
        const TInt& ProfileId() const;

        /**
        * Gets remote party identity 
        * @param aCallId is the Call identification number
        * @return remote party identity
        */
        RMobileCall::TMobileCallRemoteIdentityStatus RemoteIdentity( 
            const TInt aCallId ) const;

        /**
        * Gets ringing type
        * @return ringing type
        */
        const TProfileRingingType& RingingType() const;
        
        /**
        * Sets active als line value to the TPEGsmBasicInfo structure.
        * @param aALSLine is the active als line value.
        */
        void SetALSLine( const CCCECallParameters::TCCELineType& aALSLine );
        
        /**
        * Sets ALS support value to the TPEGsmBasicInfo structure.
        * @param aALSLineSupport is the ALS support value.
        */
        void SetALSLineSupport( const TBool& aALSLineSupport );

        /**
        * Sets audio mute command to TPEBasicInfo-structure
        * @param aAudioMute is audio mute
        */
        void SetAudioMuteCommand( const TInt& aAudioMute );

        /**
        * Sets audio output command to TPEBasicInfo-structure
        * @param aOutput is audio output path
        * @param aShowNote is audio note value 
        * @return None.
        */
        void SetAudioOutputCommand( const TPEAudioOutput& aOutput,
                                             const TBool aShowNote  );

        /**
        * Sets audio volume of the phone to TPEBasicInfo-structure
        * @param aAudioVolume is audio volume of the phone
        */
        void SetAudioVolumeCommand( const TInt& aAudioVolume );

        /**
        * Sets CallId value to TPECallCommandInfo-structure
        * @param aPhoneMumber is the phone number
        */ 
        void SetCallId( const TInt& aCallId );
        
        /**
        * Sets alsline value to TPEGsmCallInfo-structure
        * @param aALSLine informs which line is selected active
        * @param aCallId is the Call identification number.
        */
        void SetCallALSLine( const CCCECallParameters::TCCELineType& aALSLine, const TInt aCallId );

        /**
        * Sets call type information for the following command message
        * @param aCallType is call type value.
        * @param aCallId is the Call identification number.
        */
        void SetCallTypeCommand( const TPECallType& aCallType );

        /**
        * Sets the name of data port currently on-loan.
        * Data port loaning is handled by CSPlugin and happens when a data
        * call goes into connected state. The dataport is released when VT components
        * indicate it can be released, i.e. video call hangup.
        * @param aName is Data port name
        */
        void SetDataPortName( const TName& aDataPortName );

        /**
        * Sets dtmf string value to TPECommandInfo-structure
        * @param aDtmfString is the dtmf string to be sent. 
        */
        void SetDtmfStringCommand( const TPEDtmfString& aDtmfString );

        /**
        * Sets correct inband tone for specific call state transition or error
        * @param aTone is the inband tone to be played
        */
        void SetInbandTone( const TCCPTone aTone ) ;

        /**
        * Sets key code value to TPECallCommandInfo-structure
        * @param aKeyCode is the ascii coded key code.
        */
        void SetKeyCode( const TChar& aKeyCode );

        /**
        * Sets phone number to TPECallCommandInfo-structure
        * @param aPhoneNumber is the phone number
        */
        void SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber );

        /**
        * Gets active forwarding information from TPEBasicInfo-structure
        * @return deflect information
        */
        const TPEActiveForwardInfo& ActiveForwardingInfo() const;

        /**
        * Gets barring information from TPEBasicInfo-structure
        * @return barring information
        */
        const TPEBarringInfo& BarringInfo() const;

        /**
        * Gets conference value from TPECallInfo-structure
        * @param aCallId is the call identification number
        * @return call conference value
        */
        TInt CallConference( const TInt aCallId ) const;

        /**
        * Gets call control caps from the TPECallInfo-structure
        * @param aCallId is the call identification number
        * @return call control capabilities
        */
        TPECallControlCaps CallControlCaps( const TInt aCallId ) const;

        /**
        * Gets forwarded value from TPECallInfo-structure
        * @param aCallId is the call identification number
        * @return is call forwarded or not
        */
        TBool CallForwarded( const TInt aCallId ) const;

        /**
        * Gets Service information from TPECallInfo-structure
        * @param aCallId is the call identification number.
        * @return mobile call service type
        */
        RMobilePhone::TMobileService CallService( const TInt aCallId ) const;

        /**
        * Gets Conference call capabilities
        * @param aCallId is the call identification number
        * @return conference call capabilities
        */
        TUint32 ConferenceCallCaps( const TInt aCallId ) const;

        /**
        * Gets Conference call capabilities
        * @param aCallId is the call identification number
        * @return conference call member name
        */
        const TName& ConferenceMemberName( const TInt aCallId ) const;

        /**
        * Gets Conference call capabilities
        * @param aCallId is the call identification number
        * @return conference call member id
        */
        TInt IsConferenceMemberId( const TInt aCallId ) const;

        /**
        * Gets number of the conference members
        * @param aCallId is the call identification number
        * @return number of conference members
        */
        TInt NumberOfConferenceMembers( const TInt aCallId ) const;

        /**
        * Gets Sim state
        * @return current Sim state
        */
        const TPESimState& SimState() const;

        /**
        * Gets secure specified status from call info
        * @return TBool the secure specified status (on/off)
        */
        TBool SecureSpecified( ) const;
        
        /**
        * Gets the last suplementary services string command information 
        * from TPESSCommandInfo-structure
        * @since Series90_1.1
        * @return Suplementary Services command information. 
        */
        const TPESSCommandInfo& SSCommandInfo() const;
    
        /**
        * Sets emergency numbers of the phone to TPEGsmBasicInfo-structure
        * @since Series90_1.1
        * @param aEmergencyNumber for emergency number checking.
        * @return None.
        */
        void SetEmergencyNumber( const TPEPhoneNumber& aEmergencyNumbers );
        
        /**
         * Gets remote party name. Contains possible CNAP name or received 
         * skype identification
         * @param aCallId Call identification number.
         * @return Name of the remote party detemined by network.
         */
        const TPEContactName& RemotePartyName( 
            const TInt aCallId ) const;
        
        /**
         * Gets error code from TPEBasicInfo-structure
         * @return Error code
         */
         TInt ErrorCode() const;
         
                  
         /**
          * Returns transfer target from the last received unattended transfer 
          * request or KNullDesC if transfer target is not available.
          * 
          * @return  Transfer target.
          */
         const TPEPhoneNumber& UnattendedTransferTarget( TInt aCallId ) const;
         
         /**
          * Sets transfer target from the last received unattended transfer request.
          *
          * @param   aTarget     Transfer target to set.
          * @param   aCallId     Call identification number.
          */
         void SetUnattendedTransferTarget( const TPEPhoneNumber& aTarget, 
             TInt aCallId );

        /**
         * Returns address choices from received call forward request 
         * or NULL if addresses are not available.
         * 
         * @return  Address array.
         */
        const CDesC8Array* ForwardAddressChoices( TInt aCallId ) const;
        
        /**
         * Sets address choices from received call forward request.
         *
         * @param   aArray      Address array.
         * @param   aCallId     Call identification number.
         */
        void SetForwardAddressChoices( const CDesC8Array* aArray,
            TInt aCallId );
         
        /**
         * Sets array index for user selected address.
         *
         * @param   aIndex     Array index for selected address.
         */
        void SetForwardAddressIndex( TInt aIndex );
        
        /**
         * Returns array index for user selected address.
         *
         * @return Array index for selected address.
         */
        TInt ForwardAddressIndex();
        
         /**
         * Sets unattended transfer target address
         * @param aTransferTarget Transfer target address
         * @return None
         */
         void SetTransferTargetCommand( const TPEPhoneNumber& aTransferTarget );
         
         /**
         * Returns transfer target address
         * @return Transfer target address
         * address.
         */
         const TPEPhoneNumber& TransferTargetCommand() const;
                 
         
         /**
          * From base class MPEEngineInfo
          * @see MPEEngineInfo::CallOrigin          
          */
         TPECallOrigin CallOrigin( const TInt aCallId ) const;
         
         /**
         * Checks if given state can be found.
         * @param aCallState state to be checked.
         */
          TBool CheckIfCallStateExists( const TPEState& aCallState );
          
          /**
          * Sets the protocol spesific error code
          * @param aError is the error code from protocol.
          * @return None.
          */
           void SetProtocolError( TInt aError ); 

    // Functions from MPEDataStore (reading values)
    public:
        /**
        * Gets group ids which are active to play ring tone
        * @return contact group id
        */
        const TArray< TContactItemId > AlertForGroup() const;

        /**
        * Gets audio mute command value
        * @return audio mute value for PhoneApplication commands
        */
        const TBool& AudioMuteCommand() const;

        /**
        * Gets audio output command value
        * @return audio output value for PhoneApplication commands
        */
        const TPEAudioOutput& AudioOutputCommand() const;
        
        /**
        * Gets audio note command value
        * @return audio output value for PhoneApplication commands
        */
        TBool ShowNoteCommand() const;
        
        /**
        * Gets audio volume command value
        * @return audio volume value for Phone Application commands
        */
        const TInt& AudioVolumeCommand() const;      

        /**
        * Gets phone client information of the specified call
        * @since Series60_2.7
        * @param aCallId is the call identification number
        * @return client information of the specified call
        */
        const MPEClientInformation& CallClientInformation( const TInt aCallId ) const;

        /**
        * Get caller image
        * @param aCallId is the call identification number.
        * @return caller image
        */
        const TPtrC CallerImage( const TInt aCallId ) const;

        /**
        * Get caller text
        * @param aCallId is the call identification number.
        * @return caller text
        */
        const TPtrC CallerText( const TInt aCallId ) const;

        /**
        * Get caller thumbnail
        * @param aCallId is the call identification number.
        * @return pointer to thumbnail data
        */
        CFbsBitmap* CallerThumbnail( const TInt aCallId ) const;
        
        /**
        * Get caller thumbnail status
        * @return ETrue if has thumbnail, otherwise EFalse.
        */
        TBool HasCallerThumbnail( const TInt aCallId ) const;
      
        /**
        * Gets identification number of a call
        *
        * This value is used for determining the target for a command message
        * that requires such information
        * @return identification number of a call
        */
        const TInt& CallId() const;

        /**
        * Gets call type information for the attempted call
        * @return call state of a specified call
        */
        const TPECallType& CallTypeCommand( ) const;

        /**
        * Get remote contact link or id
        * @param aCallId is the call identification number.
        * @return contact link or id
        */
        const CPhCntContactId& ContactId( const TInt aCallId ) const;

        /**
        * Get user group ids this contact is assigned to
        * @param aCallId is the call identification number.
        * @return array for group ids
        */
        const TArray< TContactItemId > ContactGroups(
            const TInt aCallId ) const;

        /**
         * Get contact link packet. See virtual phonebook API.
         * @param aCallId is the call identification number.
         * @return Virtual phonebook contact link. 
         */
        const TDesC8& ContactLink( const TInt aCallId ) const;
        
        /**
        * Gets dtmf string command value
        * @return dtmf string value for Phone Application commands
        */
        const TPEDtmfString& DtmfStringCommand() const;
        
        /**
        * Gets DTMFVoiceMailBoxNumber information for line 1
        * @return line 1 DTMFVoiceMailBoxNumber information
        * @return None.
        */
        const TPEPhoneNumber& DTMFVoiceMailBoxNumberLine1() const;

        /**
        * Gets DTMFVoiceMailBoxNumber for Line 2 information from TPEGsmBasicInfo-structure
        * @return line 2 DTMF VoiceMailBoxNumber information
        */
        const TPEPhoneNumber& DTMFVoiceMailBoxNumberLine2() const;

        /**
        * Gets key code of the last key press
        * @return key code of the last key press
        */
        const TChar& KeyCode() const;
        
        /**
        * Gets keypad volume value
        * @return keypad volume value
        */
        const TInt& KeypadVolume() const;

        /**
        * Gets a logging indicator
        * @param aCallId, call identification number     
        * @return continuous logging indicator
        */
        TBool LoggingIndicator( const TInt aCallId ) const;
        
        /**
        * Get missed call indicator
        * @param aCallId, call identification number
        * @return missed call indicator
        */
        TBool MissedCall( const TInt aCallId ) const;

        /**
        * Get ringing tone
        * Returns a ringing tone defined for one of the user groups
        * if personal tone is not found
        * @param aCallId, call identification number
        * @return ringing tone file path
        */
        const TPEContactFileName& PersonalRingingTone(
            const TInt aCallId ) const;
        
        /**
        * Gets personal tone status
        * @return personal tone status
        */
        const TBool& PersonalToneStatus() const;

        /**
        * Gets profile name
        * @return profile name.  
        */
        const TPEProfileName& ProfileName() const;

        /**
        * Get contact company name
        * @param aCallId, call identification number
        * @return company name
        */
        const TPEContactCompany& RemoteCompanyName(
            const TInt aCallId ) const;

        /**
        * Get contact name
        * Depending on user selected language may be in order:
        * first + last name OR last + first name
        * @param aCallId, call identification number
        * @return contact name
        */
        const TPEContactName& RemoteName( const TInt aCallId ) const;

        /**
        * Get remote end phone number
        * @param aCallId, call identification number
        * @return phone number
        */
        const TPEPhoneNumber& RemotePhoneNumber(
            const TInt aCallId ) const;

        /**
        * Get remote end phone number type
        * @param aCallId, call identification number
        * @return phone number type
        */
        const TPEPhoneNumberIdType& RemotePhoneNumberType(
            const TInt aCallId ) const;

        /**
        * Gets text to speech text for this contact. Used in
        * text to speech ringing tone.
        * @param aCallId, call identification number
        * @return descriptor, ownership passed.
        */
        const TPtrC RemoteTextToSpeechText(
            const TInt aCallId ) const;

        /**
        * Gets remote predefined dtmf strings
        * @param aCallId, call identification number
        * @return reference to string array
        */
        const  CDesCArray& RemotePredefinedDtmfStrings(
            const TInt aCallId ) const;
  
        /**
        * Get remote end connected phone number
        * @param aCallId, call identification number
        * @return phone number
        */
        const TPEPhoneNumber& RemoteColpNumber(
            const TInt aCallId ) const;

         /**
        * Gets ringing tone name
        * @param aCallId, call identification number 
        * @return name of the current ringing tone
        */
        virtual const TPEContactFileName& RingingTone( const TInt& aCallId ) const;
        
         /**
        * Gets current ringing volume
        * @return the current ringing volume
        */
        const TInt& RingingVolume() const;       

        /**
        * Gets text-to-speech preference setting value
        * @return text-to-speech status (EFalse/ETrue)
        */
        const TBool& TextToSpeech() const; 
        
        /**
         * @see MPEDataStore::UserToUserInformation.
         */
        const TPEUserToUserInfo& UserToUserInformation() const;

        /**
        * Gets two digit support status value
        * @return two digit support status (EFalse/ETrue)
        */
        const TBool& TwoDigitSupportStatus() const; 
  
        /**
        * Gets VoiceMailBoxNumber for Line 1 information from TPEBasicInfo-structure
        * @return line 1 VoiceMailBoxNumber information
        */
        const TPEPhoneNumber& VoiceMailBoxNumberLine1() const;

        /**
        * Gets VoiceMailBoxNumber for Line 2 information from TPEBasicInfo-structure
        * @return line 2 VoiceMailBoxNumber information
        */
        const TPEPhoneNumber& VoiceMailBoxNumberLine2() const;   
        
        /**
        * Get log forced completion indicator. 
        * @param TInt aCallId, call identification.
        * @return TBool&, log forced completion indicator.
        */
        TBool LogForcedCompletion( const TInt aCallId ) const;

        /**
        * Gets whole outgoing phone number
        * @param TInt aCallId, call identification.
        * @return TPEPhoneNumber&, remote party number including DTMF string.
        */
        const TPEPhoneNumber& WholeOutgoingPhoneNumber( const TInt aCallId ) const;
                             
        /**
        * Gets emergency numbers of the phone from TPEGsmBasicInfo-structure
        * @return emergency number to be checked for validity
        */
        const TPEPhoneNumber& EmergencyNumber() const;

        /**
        * ResumeHeldCall
        * @param aCallId is the call identification number.
        * @return TBool.
        */
        TBool ResumeHeldCall( const TInt aCallId ) const;
        
        /**
         * From base class MPEDataStore
         * @see MPEDataStore::CallIndex
         */
        TInt CallIndex( TInt aCallId ) const;

    // Functions from MPEDataStore (setting values)
    public:
        /**
        * Resets TPECallInfo parameters.
        * @param aCallId is the Call identification number.
        * @return None.
        */
        void ResetCallInfo( const TInt aCallId );              

        /**
        * Reset remote information
        * @param aCallId is the Call identification number.
        */
        void ResetRemoteInfo( const TInt aCallId );

        /**
        * Set the group id's which are active to play ring tone.
        * @param aAlertForGroup group id .
        */
        void SetAlertForGroup( const TArray< TContactItemId >& aAlertForGroup );

        /**
        * Sets audio mute state of the phone to TPEBasicInfo-structure
        * @param aAudioMute is audio mute state of the phone (ETrue = mute on)
        */
        void SetAudioMute( const TBool aAudioMute );

        /**
        * Sets audio output path
        * @param aOutput is the audio output path.
        * @param aPreviousOutput is the audio previous output path.
        * @param aShowNote is audio note value
        */
        void SetAudioOutput( const TPEAudioOutput aOutput,
                                      const TPEAudioOutput aPreviousOutput,
                                      const TBool aShowNote );

        /**
        * Sets hands free mode of the phone to TPEBasicInfo-structure
        * @param aHandsfreeMode is handsfree mode of the phone(on,off).
        */
        void SetAvailableAudioOutputs( 
            const TArray< TPEAudioOutput >& aAvailableOutputs );

        // TODO: change TInt param to enum type when it is available
        /**
        * Sets audio output preference to TPEBasicInfo-structure
        * @param aOutputPreference is audio output preference (public/private)
        */
        void SetAudioOutputPreference( const TInt aOutputPreference );
        
        /**
        * Sets audio volume of the phone to TPEBasicInfo-structure
        * @param aAudioVolume is audio volume of the phone.
        */
        void SetAudioVolume( const TInt aAudioVolume );

        /**
        * Sets phone client information to TPECallInfo-structure
        * @param aClientInformation includes phone client information.
        * @param aCallId is the Call identification number.
        */
        void SetCallClientInformation( 
            const MPEClientInformation& aClientInformation, 
            const TInt aCallId );
                       
        /**
        * Sets Call direction to TPECallInfo-structure
        * @param aCallDirection is the Call direction value (0=Unknown/1=MO/2=MT)
        * @param aCallId is the Call identification number.
        */
        void SetCallDirection( 
            const RMobileCall::TMobileCallDirection& aCallDirection, 
            const TInt aCallId );

        /**
        * Sets call duration of the last call to Idle to TPEBasicInfo-structure
        * @param aCallDuration is the duration of the call.
        */
        void SetCallDuration( const TInt& aCallDuration ); 

        /**
        * Sets duration of the call to TPECallInfo-structure
        * @param aDuration is duration of the call. 
        * @param aCallId is the Call identification number.
        */
        void SetCallDuration( 
            const TTimeIntervalSeconds& aCallDuration, 
            const TInt aCallId );

        /**
        * Set caller image
        * @param aCallerImage caller image
        * @param aCallId is the call identification number
        */
        void SetCallerImage(
            const TPtrC aCallerImage, 
            const TInt aCallId );

        /**
        * Set caller text
        * @param aCallerText caller text
        * @param aCallId is the call identification number
        */
        void SetCallerText(
            const TPtrC aCallerText, 
            const TInt aCallId );

        /**
        * Set caller thumbnail
        * @param aCallerThumbnail caller thumbnail
        * @param aCallId is the call identification number
        */
        void SetCallerThumbnail(
            CFbsBitmap* aCallerThumbnail, 
            const TInt aCallId );
        
        /**
        * Set caller thumbnail status
        * @param aStatus caller thumbnail status
        * @param aCallId is the call identification number
        */
        void SetHasCallerThumbnail(
            TBool aStatus, 
            const TInt aCallId );

        /**
        * Sets call origin for the next call attempt
        * @param aOrigin indicates if the call is phone, client or SAT originated
        */
        void SetCallOriginCommand( const TPECallOrigin& aOrigin );

        /**
        * Sets predefined dtmf strings
        * @param aArray, is reference to string array
        * @param aCallId, is the call identification number
        */
        void SetPredefinedDtmfStrings( 
            const CDesCArray& aArray, 
            const TInt aCallId );
 
        /**
        * Sets start time of the call to TPECallInfo-structure
        * @param aStartTime is the start time of the call.
        * @param aCallId is the Call identification number.
        */
        void SetCallStartTime( const TTime& aCallStartTime, const TInt aCallId );

        /**
        * Sets Call State value to TPECallInfo-structure
        * @param aCallState is call state value.
        * @param aCallId is the Call identification number.
        */
        void SetCallState( const TPEState& aCallState, const TInt aCallId );

        /**
        * Sets call type information
        * @param aCallType is the call type.
        * @param aCallId is the Call identification number.
        */
        void SetCallType( const TPECallType& aCallType, const TInt aCallId );

        /**
        * Sets Secure status to call info
        * @since Series60_4.0
        * @param SecureStatus is the secure status (on/off)
        * @param aCallId is the Call identification number.
        */
        void SetCallSecureStatus( const TBool aSecureStatus, const TInt aCallId );

        /**
        * Sets call state of a conference call
        * @param aCallState is call state value.
        */
        void SetConferenceCallState( const TPEState& aCallState );
        
        /**
        * Sets Secure specified status to call info
        * @since Series60_5.0
        * @param SecureSpecifiedStatus is the secure status (on/off)
        */
        void SetSecureSpecified( const TBool aSecureSpecifiedStatus );        
        
        /**
        * Set user group ids this contact is assigned to
        * Array is reset if the operation fails.
        * @param aGroupIdArray array of group ids
        * @param aCallId is the Call identification number
        */
        void SetContactGroups(
            TArray< TContactItemId >& aGroupIdArray,
            const TInt aCallId );

        /**
        * Set contact link or id
        * @param aContactId pointer to contact id
        * @param aCallId is the Call identification number
        * @return 
        */
        void SetContactId(
                CPhCntContactId* aContactId,
                const TInt aCallId );

        /**
        * Sets dtmf postfix value to TPEBasicInfo-structure
        * @param aDtmfPostFix is persed dtmf postfix. 
        * @param aCallId is the call identification number.
        */
        void SetDtmfPostFix( const TPEDtmfString& aDtmfPostFix, const TInt aCallId );
                
        /**
        * Sets dtmf string value to TPEBasicInfo-structure
        * @param aDtmfString is the dtmf string to be sent. 
        */
        void SetDtmfString( const TPEDtmfString& aDtmfString );
        
        /**
        * Sets DTMFVoiceMailBoxNumber for Line 1 information from TPEGsmBasicInfo-structure
        * @param aDTMFVoiceMailBoxNumberLine1 includes line 1 DTMFVoiceMailBoxNumber information.  
        */
        void SetDTMFVoiceMailBoxNumberLine1( const  TDesC& aDTMFVoiceMailBoxNumberLine1 );

        /**
        * Sets DTMFVoiceMailBoxNumber for Line 2 information from TPEGsmBasicInfo-structure
        * @param aDTMFVoiceMailBoxNumberLine2 includes line 2 DTMFVoiceMailBoxNumber information.  
        */
        void SetDTMFVoiceMailBoxNumberLine2( const  TDesC& aDTMFVoiceMailBoxNumberLine2 );
 
        /**
        * Sets error code of the phone to TPEBasicInfo-structure
        * @param aErrorCode is error code value
        */
        void SetErrorCode( const TInt aErrorCode );

        /**
        * Sets Keypad volume of the phone to TPEBasicInfo-structure
        * @param aKeypadVolume is Keypad volume of the phone.
        */
        void SetKeypadVolume( const TInt aKeypadVolume );

        /**
        * Sets a logging indicator to TPECallInfo-tructure
        * @param aLoggingIndicator, Continuous logging indicator.
        * @param aCallId, the call identification number.
        */
        void SetLoggingIndicator( const TBool aLoggingIndicator, const TInt aCallId );

        /**
        * Set missed call indicator. 
        * @param TBool& aMissedCall, missed call indicator.
        * @param TInt aCallId, call identification.
        */
        void SetMissedCall( const TBool& aMissedCall, const TInt aCallId );

        /**
        * Sets network information to TPEBasicInfo-structure
        * @param aNetworkInfo is the current network information.
        */
        void SetNetworkRegistrationStatus( 
            const TNWNetworkRegistrationStatus& aRegistrationStatus );

        /**
        * Set personal ringing tone
        * Used to set a ringing tone defined for one of the user groups
        * if personal tone is not found
        * @param aRingingTonepath, personal ringing tone
        * @param aCallId, call identification number
        */
        void SetPersonalRingingTone(
            const TPEContactFileName& aRingingTonepath,
            const TInt aCallId );

        /**
        * Sets personal tone status value to TPEBasicInfo-structure.
        * @param aPersonalTone is personal tone status (EFalse/ETrue).
        */
        void SetPersonalToneStatus( const TInt& aPersonalToneStatus );
        
        /**
        * Sets phone number parsing result
        * @param aServiceCode indicates if phone number a service code
        */
        void SetPhoneNumberIsServiceCode( const TBool& aServiceCode );

        /**
        * Sets profile id to TPEBasicInfo-structure
        * @param aProfileId is the identification number of the profile
        */
        void SetProfileId( const TInt aProfileId );

        /**
        * Sets profile name to TPEBasicInfo-structure
        * @param aProfileName is the name of the profile
        */
        void SetProfileName( const TPEProfileName& aProfileName );

        /**
        * Sets remote identity status
        * @param aIdentity is the status of identity 
        */
        void SetRemoteIdentity( 
            const RMobileCall::TMobileCallRemoteIdentityStatus& aIdentity, 
            const TInt aCallId );

        /**
        * Set contact company name
        * @param aCompanyName company name
        * @param aCallId, is the call identification number
        */
        void SetRemoteCompanyName(
            const TPEContactName& aCompanyName, 
            const TInt aCallId );

        /**
        * Set remote end connected phone number
        * @param aColpNumber phone number
        * @param aCallId, is the call identification number.
        */
        void SetRemoteColpNumber(
            const TPEPhoneNumber& aColpNumber, 
            const TInt aCallId );

        /**
        * Set contact name
        * Depending on user selected language may be in order:
        * first + last name OR last + first name
        * @param aContactName contact name
        * @param aCallId, is the call identification number
        */
        void SetRemoteName(
            const TPEContactName& aContactName, 
            const TInt aCallId );

        /**
        * Set remote end phone number
        * @param aPhoneNumber phone number
        * @param aCallId, is the call identification number
        */
        void SetRemotePhoneNumber(
            const TPEPhoneNumber& aPhoneNumber, 
            const TInt aCallId );

        /**
        * Set remote end phone number type
        * @param aPhoneNumberType phone number id
        * @param aCallId, is the call identification number
        */
        void SetRemotePhoneNumberType(
            TPEPhoneNumberIdType aPhoneNumberType, 
            const TInt aCallId );

        /**
        * Set remote end Text To Speech Text
        * @param aText Text To Speech Text
        * @param aCallId, is the call identification number
        */
        void SetRemoteTextToSpeechText(
            HBufC* aText,
            TInt aCallId );

        /**
        * Sets ringing type to TPEBasicInfo-structure
        * @param aRingingToneName is the current ringintone name for line 1. 
        */
        void SetRingingTone1( const TPEContactFileName& aRingingToneName );
        
        /**
        * Sets ringing type to TPEBasicInfo-structure
        * @param aRingingToneName is the current ringintone name for line 2.
        */
        void SetRingingTone2( const TPEContactFileName& aRingingToneName );

        /**
        * Sets video call ringing tone name to TPEBasicInfo-structure
        * @param aRingingToneName is the current ringintone name for video call.
        */
        void SetVideoCallRingingTone( 
            const TPEContactFileName& aRingingToneName );
        
        /**
        * Sets ringing type to TPEBasicInfo-structure
        * @param aRinginType is the ringintype
        */
        void SetRingingType( const TProfileRingingType& aRingingType );
                
        /**
        * Sets ringing volume to TPEBasicInfo-structure
        * @param aRingingVolume is the current ringingtone name. 
        */
        void SetRingingVolume( const TInt& aRingingVolume );

        /**
        * Sets text-to-speech to TPEBasicInfo-structure
        * @param aTextToSpeech is the current text-to-speech preference setting. 
        */
        void SetTextToSpeech( const TBool aTextToSpeech );

         /**
        * Sets two digit support status of the phone to TPEBasicInfo-structure
        * @param aTwoDigitSupportStatus is support status (EFalse/ETrue).
        */
        void SetTwoDigitSupportStatus( const TBool aTwoDigitSupportStatus );

        /**
        * Gets VoiceMailBoxNumber for Line 1 information from TPEGsmBasicInfo-structure
        * @param aVMBXL1 includes line 1 VoiceMailBoxNumber information.  
        */
        void SetVoiceMailBoxNumberLine1( const  TDesC& aVoiceMailBoxNumberLine1 );

        /**
        * Gets VoiceMailBoxNumber for Line 2 information from TPEGsmBasicInfo-structure
        * @param aVMBXL2 includes line 2 VoiceMailBoxNumber information.  
        */
        void SetVoiceMailBoxNumberLine2( const  TDesC& aVoiceMailBoxNumberLine2 );

        /**
        * Set log forced completion indicator. 
        * @param TBool aForcedCOmpletion, log forced completion indicator.
        * @param TInt aCallId, call identification.
        */
        void SetLogForcedCompletion(  const TBool aForcedCompletion, const TInt aCallId );
       
        /**
        * Sets whole outgoing phone number 
        * @param aPhoneNumber is the phone number
        * @param TInt aCallId, call identification.
        */
        void SetWholeOutgoingPhoneNumber( 
            const TPEPhoneNumber& aPhoneNumber, 
            const TInt aCallId );
        
        /**
        * Returns the service-id used for the command.
        * @since S60 3.2
        * @param none
        * @return TUint32 Service-id
        */
        TUint32 ServiceIdCommand() const;
        
        /**
        * Set the service-id for the command.
        * @since S60 3.2
        * @param TUint32 Service-id
        * @return none
        */
        void SetServiceIdCommand( TUint32 aServiceId );
        
        /**
        * Returns the service-id used for the call.
        * @since S60 3.2
        * @param none
        * @return TUint32 Service-id
        */
        TUint32 ServiceId( const TInt aCallId ) const;
        
        /**
        * Set the service-id for the call.
        * @since S60 3.2
        * @param TUint32 Service-id
        * @return none
        */
        void SetServiceId( const TInt aCallId, TUint32 aServiceId );
        
                
        /**
        * Sets Barring information to TPEGsmBasicInfo-structure
        * @param aBarringInfo includes call Barring information
        * @return
        */
        void SetBarringInfo( const TPEBarringInfo& aBarringInfo );

        /**
        * Sets Confrence value to TPEGsmCallInfo-structure
        * @param aCallConference informs is the call a member of conference call. 
        * -2 = normal call, -1 = conference master, >= 0 master id
        * @param aCallId is the Call identification number.
        * @return None.
        */
        void SetCallConference( const TInt aCallConference, const TInt aCallId );

        /**
        * Sets Call control caps to TPEGsmCallInfo-structure
        * @param aCallControlCaps the value of the call control caps
        * @param aCallId is the Call identification number.
        * @return None.
        */
        void SetCallControlCaps( const TPECallControlCaps& aCallControlCaps, const TInt aCallId );

        /**
        * Sets forwarded value to TPEGsmCallInfo-structure
        * @param aForwarded Indicates if incoming call was forwarded
        * @param aCallId is the Call identification number.
        * @return None.
        */
        void SetCallForwarded( const TBool aCallForwarded, const TInt aCallId );

        /**
        * Sets Call service to TPEGsmCallInfo-structure
        * @param aCallService is the Call service value
        * @param aCallId is the Call identification number.
        * @return None.
        */
        void SetCallService( 
            const RMobilePhone::TMobileService& aCallService, 
            const TInt aCallId );

       /**
        * Sets Conference capabilities to TPEGsmCallInfo structure.
        * @param aCaps, conference capabilities
        * @param aCallId is the Call identification number.
        * @return None.
        */
        void SetConferenceCallCaps( const TUint32 aCaps, const TInt aCallId );

        /**
        * Sets added or removed members TName information to TPEGsmCallInfo structure
        * @param aName is the TName information of added or removed member.
        * @param aCallId is the Call identification number.
        * @return None.
        */
        void SetConferenceMemberInfo( 
            const TName& aConferenceMemberName, 
            const TInt aConferenceMemberCallId, 
            const TInt aCallId );

        /**
        * Sets deflect information to the TPEGsmBasicInfo structure.
        * @param aActiveForward is the active forward information.
        * @return None.
        */
        void SetDeflectInfo( const TPEActiveForwardInfo& aActiveForward );           

        /**
        * Set iResumeHeldCall value to TPEGsmCallInfo structure
        * @param aResumeHeldCall
        * @param aCallId is the Call identification number.
        * @return None.
        */
        void SetResumeHeldCall( const TBool aResumeHeldCall, const TInt aCallId );

        /**
        * Sets the number of conference members to TPEGsmCallInfo structure
        * @param aNumberOfMembers, number of conference members
        * @param aCallId is the Call identification number.
        * @return None.
        */
        void SetNumberOfConferenceMembers( const TInt aNumberOfMembers, const TInt aCallId );

        /**
        * Sets Sim state to TPEGsmBasicInfo-structure
        * @param aSimState is the Sim state
        * @return None.
        */
        void SetSimState( const TPESimState& aSimState );

        /**
        * Sets Suplementary Services command information to TPESSCommandInfo-structure
        * @param aSSCommandInfo includes Suplementary Services command information.  
        * @return None.
        */
        void SetSSCommandInfo( const TPESSCommandInfo& aSSCommandInfo );

        /**
         * @see MPEDataStore::UserToUserInformation
         * @since S60 5.0
         */
        void SetUserToUserInformation( const TPEUserToUserInfo& aUUI );
        
        /**
         * Sets remote party name given by network to TPECallInfo structure.
         * @param aName Remote party name.
         * @param aCallId Call identification number.
         */
        void SetRemotePartyName(    
            const TPEContactName& aName, 
            const TInt aCallId );

        CCCECallParameters& CallParameters();       
        
        /**
         * From base class MPEDataStore
         * @see MPEDataStore::SetCallIndex
         */
        void SetCallIndex( TInt aCallIndex, TInt aCallId );
        
        /**
         * From base class MPEDataStore
         * @see MPEDataStore::SetCallOrigin          
         */
        void SetCallOrigin( TPECallOrigin aOrigin, TInt aCallId );
      
        /**
        * Sets call Phonebook contact identifier.
        * @param aContactId is the contact identifier for a call
        */
        void SetContactId2( const TInt aContactId );

        /**
         * Returns Phonebook contact identifier.
         */
        TInt ContactId2() const;
        
        /**
        * Sets the protocol spesific error code
        * @param aError is the error code from protocol.
        * @return None.
        */
        void SetProtocolError( TInt aError, TInt aCallId ); 
 
        /**
        * Returns the protocol spesific error code
        * @return Error code.
        */
        TInt ProtocolError( TInt aCallId ); 
        
        /**
        * Returns flag if the outgoing barring is activated.
        * @return ETrue if barring activated.
        */
        TBool IsOutgoingCallBarringActivated();
        
        /**
        * Sets the flag if the outgoing barring is activated.
        * @return ETrue if barring activated.
        */
        void SetOutgoingCallBarringActivated( 
                TBool aActivated );
        
        /**
        * Sets incoming call forwarded flag.
        * @param aForwarded is call forwarded
        */
        void SetIncomingCallForwarded( TBool aForwarded, TInt aCallId );

        /**
         * Returns incoming call forwarded flag.
         */
        TBool IncomingCallForwarded( TInt aCallId ) const;
        
    private:
          /**
        * C++ default constructor.
        */
        CPEEngineInfoImpl();

        /**
        * By default EPOC constructor is private.
        */
        void ConstructL();                      

    protected: // Data
        // Contains information of the calls
        RPointerArray<CPECallInfo> iCurrentCalls;

        // Contains information of the conference call
        TPEConferenceCallInfo iConferenceCallInfo;
        
        // For Mobile Originated Call Commands information
        TPECallCommandInfo iCallCommandInfo;

        // Contains information of the phone related variables 
        TPEBasicInfo iBasicInfo;

        // Remote data array
        RPointerArray< CPERemoteInfo >* iRemoteInfo;

        // Available audio outputs array
        TFixedArray<TBool, KPEMaxAudioOutputs> iAvailableAudioOutputs;

        // Contains the group id's which are active for ringtone
        RArray< TContactItemId > iAlertForGroup;

        /**
         * CCE call dial parameters
         */
        CCCECallParameters *iCallParams;

        /**
         * Phonebook contact identifier.
         */
        TInt iContactId;
    };


#endif // CPEENGINEINFOIMPL_H   
            
// End of File
