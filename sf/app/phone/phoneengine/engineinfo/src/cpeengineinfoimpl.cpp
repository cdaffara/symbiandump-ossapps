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
* Description:  This module contains the implementation of CPEEngineInfoImpl class 
*
*/

// INCLUDE FILES
#include "cpecallinfo.h"
#include "cpeclientinformation.h"
#include "cpeengineinfoimpl.h"
#include <pepanic.pan>
#include <telinternalpskeys.h>
#include <cphcltdialdata.h>
#include <phclttypes.h>
#include <talogger.h>

// ================= MEMBER FUNCTIONS ==========================================

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CPEEngineInfoImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEEngineInfoImpl::CPEEngineInfoImpl()
    : iCurrentCalls( KPEMaximumNumberOfCalls )
    ,iContactId( KErrNotFound )
    {
    iCallCommandInfo.iPhoneNumber = KNullDesC;
    iCallCommandInfo.iCallId = 0;
    iCallCommandInfo.iKeyCode = 0;
    iCallCommandInfo.iAudioVolume = KPENumberInitValue;
    iCallCommandInfo.iAudioMute = EFalse;
    iCallCommandInfo.iDtmfString = KNullDesC;
    iCallCommandInfo.iCallType = EPECallTypeUninitialized;
    iCallCommandInfo.iServiceId = 0;
    iBasicInfo.iAudioOutputPreference = EPSAudioPrivate;
    iBasicInfo.iALSLine = CCCECallParameters::ECCELineTypePrimary;
    iBasicInfo.iALSLineSupport = EFalse;
    iBasicInfo.iPhoneAudioRouteParameters.iAudioOutput = EPENotActive;
    iBasicInfo.iAudioVolume = KPEDefaultAudioVolume;
    iBasicInfo.iKeypadVolume = KPENumberInitValue;
    iBasicInfo.iAudioMute = EFalse;
    iBasicInfo.iDtmfString = KNullDesC;
    iBasicInfo.iErrorInfo.iErrorCode = ECCPErrorNone;
    iBasicInfo.iErrorInfo.iCallId = KPECallIdNotUsed;
    iBasicInfo.iProfileId = ECCPErrorNotFound;
    iBasicInfo.iProfileName = KNullDesC;
    iBasicInfo.iLastCallDuration = KPENumberInitValue;
    iBasicInfo.iVMBXNumbers.iVMBXL1 = KNullDesC;
    iBasicInfo.iVMBXNumbers.iVMBXL2 = KNullDesC;
    iBasicInfo.iRingingType = EProfileRingingTypeRinging;
    iBasicInfo.iTextToSpeech = EFalse;
    iBasicInfo.iPhoneNumberIsServiceCode = EFalse;
    iBasicInfo.iTwoDigitSupportStatus = EFalse;

    iBasicInfo.iNetworkRegistrationStatus = ENWStatusRegistrationUnknown;

    iCallCommandInfo.iIdRestrict = RMobileCall::EIdRestrictDefault;
    iCallCommandInfo.iEmergencyNumber = KNullDesC;
    
    iCallCommandInfo.iTransferToAddress = KNullDesC;
    iCallCommandInfo.iForwardToAddressIndex = ECCPErrorNotFound;
    iBasicInfo.iActiveForward.iActiveType = RMobilePhone::ECFUnconditionalActive;
    iBasicInfo.iActiveForward.iServiceGroup =  RMobilePhone::EServiceUnspecified;
    iBasicInfo.iBarringInfo.iGroupCount = KPENumberInitValue;
    for( TInt i = 0; i < KPEMaxServiceGroup; i++ )
        {
        __ASSERT_DEBUG( 
            iBasicInfo.iBarringInfo.iBarredInfo.Count() > i, 
            Panic( EPEPanicIndexOutOfRange ) );
        iBasicInfo.iBarringInfo.iBarredInfo[i].iCondition = RMobilePhone::EBarUnspecified;
        iBasicInfo.iBarringInfo.iBarredInfo[i].iServiceGroup = 
            RMobilePhone::EServiceUnspecified;
        iBasicInfo.iBarringInfo.iBarredInfo[i].iStatus = 
            RMobilePhone::ECallBarringStatusUnknown;
        }
    iBasicInfo.iSSCommandInfo.action = EPESSActionUnspecified;
    iBasicInfo.iSSCommandInfo.type = EPESSTypeUnspecified;
    iBasicInfo.iSSCommandInfo.group = RMobilePhone::EServiceUnspecified;
    iBasicInfo.iSimState = EPESimStatusUninitialized;
    iBasicInfo.iSecureSpecified = ETrue; 
    iBasicInfo.iDataPortName = KNullDesC;    
    iConferenceCallInfo.iConferenceCallState = EPEStateConferenceIdle;
    iBasicInfo.iOutgoingBarringActivated = EFalse;
    //TODO remove after profile information is available
    iBasicInfo.iRingingVolume = 10;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEEngineInfoImpl* CPEEngineInfoImpl::NewL()
    {
    CPEEngineInfoImpl* self = new ( ELeave ) CPEEngineInfoImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ConstructL
// Initializes class member variables
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::ConstructL()
    {
    iCallParams = CCCECallParameters::NewL();

    iRemoteInfo = new ( ELeave ) RPointerArray<CPERemoteInfo>( KPEMaximumNumberOfCalls );

    for ( TInt i = 0; i < KPEMaximumNumberOfCalls; i++ )
        {
        TEFLOGSTRING( KTAOBJECT, "CPEEngineInfoImpl::ConstructL" );
        CPERemoteInfo* remoteInfo = CPERemoteInfo::NewL();
        CleanupStack::PushL( remoteInfo );
        // associate this remote info instance with a call id
        remoteInfo->SetCallId( i );
        iRemoteInfo->InsertL( remoteInfo, i );
        CleanupStack::Pop( remoteInfo );
        
        CPECallInfo* callInfo = CPECallInfo::NewL();
        CleanupStack::PushL( callInfo );
        iCurrentCalls.AppendL( callInfo );
        CleanupStack::Pop( callInfo );
        }
        
    for( TInt i = 0 ; i < KPEMaximumNumberOfCalls ; i++ ) 
        {
        ResetCallInfo( i );    // Resets every call.
        }
    }

// Destructor
CPEEngineInfoImpl::~CPEEngineInfoImpl()
    {
    if ( iRemoteInfo )
        {
        iRemoteInfo->ResetAndDestroy();
        iRemoteInfo->Close();
        delete iRemoteInfo;
        iRemoteInfo = NULL;
        }
    delete iCallParams;
    
    iCurrentCalls.ResetAndDestroy();
    iCurrentCalls.Close();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::AlertForGroup
// Gets group ids  from the TPEBasicInfo structure.
// -----------------------------------------------------------------------------
//
const TArray< TContactItemId > CPEEngineInfoImpl::AlertForGroup() const
    {
    return iAlertForGroup.Array();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ALSLine
// Gets active als line of the phone.
// -----------------------------------------------------------------------------
//
const CCCECallParameters::TCCELineType& CPEEngineInfoImpl::ALSLine() const
    {
    return iBasicInfo.iALSLine;
    }    


// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ALSLine
// Gets active als line of the phone.
// -----------------------------------------------------------------------------
//
const TBool& CPEEngineInfoImpl::ALSLineSupport() const
    {
    TEFLOGSTRING2( KTAMESIN,"CPEEngineInfoImpl::ALSLineSupport(),iALSLineSupport: %d"
        , iBasicInfo.iALSLineSupport );
    return iBasicInfo.iALSLineSupport;
    }  
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::AudioMute
// Gets audio mute value from the TPEBasicInfo structure.
// -----------------------------------------------------------------------------
//
const TBool& CPEEngineInfoImpl::AudioMute() const
    {
    return iBasicInfo.iAudioMute;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::AudioOutput
// Gets audio output value from the TPEBasicInfo structure.
// -----------------------------------------------------------------------------
//
const TPEAudioOutput& CPEEngineInfoImpl::AudioOutput() const
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::AudioOutput: iBasicInfo.iAudioOutput: %d", 
        iBasicInfo.iPhoneAudioRouteParameters.iAudioOutput );
    return iBasicInfo.iPhoneAudioRouteParameters.iAudioOutput;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::PhoneIdentityParameters
// Gets the phone identity parameters from the TPEBasicInfo structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPEPhoneAudioRouteParameters& CPEEngineInfoImpl::RouteParameters() const
    {
    return iBasicInfo.iPhoneAudioRouteParameters;
    }
 
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::AudioOutputAvailable
// Gets audio output path availability status from array
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::AudioOutputAvailable( 
        const TPEAudioOutput aOutput ) const 
    {
    __ASSERT_ALWAYS( ( EPENotActive <= aOutput && 
        aOutput <= EPETTY ), Panic( EPEPanicInvalidParameter ) );
        
    TBool available( EFalse );
    // find given output from the available outputs array
    for (TInt i = 0; i < iAvailableAudioOutputs.Count(); i++)
        {
        if ( iAvailableAudioOutputs[i] == aOutput )
            {
            available = ETrue;
            }
        }
    return available;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::AudioMuteCommand
// Gets audio mute command value from the TPECallCommandInfo structure.
// -----------------------------------------------------------------------------
//
const TBool& CPEEngineInfoImpl::AudioMuteCommand() const
    {
    return iCallCommandInfo.iAudioMute;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::AudioOutputCommand
// Gets audio output command value from the TPECallCommandInfo structure.
// -----------------------------------------------------------------------------
//
const TPEAudioOutput& CPEEngineInfoImpl::AudioOutputCommand() const
    {
    return iCallCommandInfo.iAudioOutput;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ShowNoteCommand
// Gets audio note command value from the TPECallCommandInfo structure.
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::ShowNoteCommand() const
    {
    return iCallCommandInfo.iShowNote;
    }
        
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::AudioOutputPreference
// Gets audio output preference value from the TPEBasicInfo structure.
// -----------------------------------------------------------------------------
//
const TInt& CPEEngineInfoImpl::AudioOutputPreference() const
    {
    return iBasicInfo.iAudioOutputPreference;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::AudioVolume
// Gets audio volume value from the TPEBasicInfo structure.
// -----------------------------------------------------------------------------
//
const TInt& CPEEngineInfoImpl::AudioVolume() const
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::AudioVolume, volume: %d", iBasicInfo.iAudioVolume );
    return iBasicInfo.iAudioVolume;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::AudioVolumeCommand
// Gets audio volume command value from the TPECallCommandInfo structure.
// -----------------------------------------------------------------------------
//
const TInt& CPEEngineInfoImpl::AudioVolumeCommand() const
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::AudioVolumeCommand, volume: %d", 
        iCallCommandInfo.iAudioVolume );
    return iCallCommandInfo.iAudioVolume;
    } 

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallClientInformation
// Gets phone client information from the TPECallInfo structure.
// -----------------------------------------------------------------------------
//
const MPEClientInformation& CPEEngineInfoImpl::CallClientInformation(
        const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->ClientInformation();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallDirection
// Gets the call direction.
// -----------------------------------------------------------------------------
//
RMobileCall::TMobileCallDirection CPEEngineInfoImpl::CallDirection( 
        const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->CallDirection();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallDuration
// Gets the duration of the last call to idle.
// -----------------------------------------------------------------------------
//
const TInt& CPEEngineInfoImpl::CallDuration() const
    {
    return iBasicInfo.iLastCallDuration;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallDuration
// Gets the duration of the call.
// -----------------------------------------------------------------------------
//
TTimeIntervalSeconds CPEEngineInfoImpl::CallDuration(
        const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->Duration();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallId
// Gets the identification number of the call from the TPECallCommandInfo structure. 
// -----------------------------------------------------------------------------
//
const TInt& CPEEngineInfoImpl::CallId() const
    {
    return iCallCommandInfo.iCallId;
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallALSLine
// Gets als line value from the TPECallInfo structure.
// -----------------------------------------------------------------------------
//
CCCECallParameters::TCCELineType CPEEngineInfoImpl::CallALSLine( 
        const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->AlsLine();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallStartTime
// Gets the start time of the call.
// -----------------------------------------------------------------------------
//
const TTime& CPEEngineInfoImpl::CallStartTime(
        const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->StartTime();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallState
// Gets the state of the call.
// -----------------------------------------------------------------------------
//
TPEState CPEEngineInfoImpl::CallState( 
        const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->CallState();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallType
// Gets the type of the call.
// -----------------------------------------------------------------------------
//
TPECallType CPEEngineInfoImpl::CallType(
        const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->CallType();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallTypeCommand
// Gets call type information for the attempted call
// -----------------------------------------------------------------------------
//
const TPECallType& CPEEngineInfoImpl::CallTypeCommand() const
    {
    return iCallCommandInfo.iCallType;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ConferenceCallState
// -----------------------------------------------------------------------------
//
TPEState CPEEngineInfoImpl::ConferenceCallState() const
    {
    return iConferenceCallInfo.iConferenceCallState;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::IsSecureCall
// Gets secured status from call info
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::IsSecureCall( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    TInt isSecured = EFalse; 
    
    // Conference call secure status
    if ( KPEConferenceCallID == aCallId )
        {
        TInt count = iCurrentCalls.Count();

        for ( TInt i = 0; i < count; i++ )
            {
            if ( iCurrentCalls[ i ]->ConferenceMemberId() != KPECallIdNotUsed )
            {
                if ( iCurrentCalls[ i ]->SecureStatus() )
                    {
                    isSecured = ETrue;
                    }
                else
                    {
                    isSecured = EFalse;
                    break;
                    }
                }
            }
        }
    else
        {
        isSecured = iCurrentCalls[ aCallId ]->SecureStatus();
        }
    
    return isSecured;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::DataPortName
// Gets the name of data port currently on-loan.
// -----------------------------------------------------------------------------
//
const TName& CPEEngineInfoImpl::DataPortName() const
    {
    return iBasicInfo.iDataPortName;
    }
        
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::DtmfPostFix
// Gets dtmf postfix value from the TPEBasicInfo structure.
// -----------------------------------------------------------------------------
//
const TPEDtmfString& CPEEngineInfoImpl::DtmfPostFix( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->DtmfPostFix();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::DtmfString
// Gets dtmf string value from the TPEBasicInfo structure.
// -----------------------------------------------------------------------------
//
const TPEDtmfString& CPEEngineInfoImpl::DtmfString() const
    {
    return iBasicInfo.iDtmfString;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::DtmfStringCommand
// Gets dtmf string command value from TPECallCommandInfo-structure
// -----------------------------------------------------------------------------
//
const TPEDtmfString& CPEEngineInfoImpl::DtmfStringCommand() const
    {
    return iCallCommandInfo.iDtmfString; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ErrorInfo
// Gets the error code value of the phone from the TPEBasicInfo structure.
// -----------------------------------------------------------------------------
//
const TPEErrorInfo& CPEEngineInfoImpl::ErrorInfo() const
    {
    return iBasicInfo.iErrorInfo;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::InbandTone
// Gets correct inband tone for specific call state transition or error
// -----------------------------------------------------------------------------
//
const TCCPTone& CPEEngineInfoImpl::InbandTone() const 
    {
    return iBasicInfo.iInbandTone; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::KeyCode
//  Gets key code value.
// -----------------------------------------------------------------------------
//
const TChar& CPEEngineInfoImpl::KeyCode() const
    {
    return iCallCommandInfo.iKeyCode;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::KeypadVolume
// Gets Keypad volume value from the TPEBasicInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TInt& CPEEngineInfoImpl::KeypadVolume() const
    {
    return iBasicInfo.iKeypadVolume;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::LoggingIndicator
// Get logging indicator from TPECallInfo.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::LoggingIndicator(
        const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    __ASSERT_ALWAYS( !CallIdCheck::IsConference( aCallId ), Panic( EPEPanicIllegalCommand ) );
    return iCurrentCalls[ aCallId ]->LoggingEnabled();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::MissedCall
// Get missed call indicator from TPECallInfo.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::MissedCall(
        const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    __ASSERT_ALWAYS( !CallIdCheck::IsConference( aCallId ), Panic( EPEPanicIllegalCommand ) );
    return iCurrentCalls[ aCallId ]->MissedCall();     
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::NetworkInformation
// Return network information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TNWNetworkRegistrationStatus& CPEEngineInfoImpl::NetworkRegistrationStatus() const
    {
    return iBasicInfo.iNetworkRegistrationStatus;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::PersonalToneStatus
// Gets personal tone status from the TPEBasicInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TBool& CPEEngineInfoImpl::PersonalToneStatus() const
    {
    return iBasicInfo.iPersonalToneStatus;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::PhoneNumber
// Return the phone number from the TPECallCommandInfo structure.
// -----------------------------------------------------------------------------
//
const TPEPhoneNumber& CPEEngineInfoImpl::PhoneNumber() const
    {
    return iCallCommandInfo.iPhoneNumber;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::PhoneNumberIsServiceCode
// Return the phone number parsing result
// -----------------------------------------------------------------------------
//
const TBool& CPEEngineInfoImpl::PhoneNumberIsServiceCode() const
    {
    return iBasicInfo.iPhoneNumberIsServiceCode;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallOriginCommand
// Gets call origin for next call attempt
// -----------------------------------------------------------------------------
//
const TPECallOrigin& CPEEngineInfoImpl::CallOriginCommand() const
    {
    return iCallCommandInfo.iCallOrigin;
    }
            
// CPEEngineInfoImpl::ProfileId
// Gets the profile id from the TPEBasicInfo structure.
// -----------------------------------------------------------------------------
//
const TInt& CPEEngineInfoImpl::ProfileId() const
    {
    return iBasicInfo.iProfileId;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ProfileName
// Gets the profile name from the TPEBasicInfo structure.
// -----------------------------------------------------------------------------
//
const TPEProfileName& CPEEngineInfoImpl::ProfileName() const
    {
    return iBasicInfo.iProfileName;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::RemoteIdentity
// Gets the remote identity from the TPEBasicInfo structure.
// -----------------------------------------------------------------------------
//
RMobileCall::TMobileCallRemoteIdentityStatus CPEEngineInfoImpl::RemoteIdentity(
        const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->RemoteIdentity();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::RingingTone
// Gets the ringing tone file name from the TPEBasicInfo structure.
// -----------------------------------------------------------------------------
// 
const TPEContactFileName& CPEEngineInfoImpl::RingingTone(
        const TInt& aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );

    if ( iCurrentCalls[aCallId]->AlsLine() == CCCECallParameters::ECCELineTypeAux )
        {
        return iBasicInfo.iRingingToneName2;
        }
    else if ( EPECallTypeVideo == iCurrentCalls[aCallId]->CallType() )
        {
        return iBasicInfo.iVideoCallRingingTone;
        }
    else
        {
        return iBasicInfo.iRingingToneName1;
        }
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::RingingType 
// Gets the ringingtype.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TProfileRingingType& CPEEngineInfoImpl::RingingType() const
    {
    return iBasicInfo.iRingingType;
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::RingingVolume
// Gets the ringing volume.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TInt& CPEEngineInfoImpl::RingingVolume() const
    {
    return iBasicInfo.iRingingVolume;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::TextToSpeech
// Return the status of the text-to-speech preference setting.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TBool& CPEEngineInfoImpl::TextToSpeech() const
    {
    return iBasicInfo.iTextToSpeech;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::UserToUserInformation
// Return User to user information
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
const TPEUserToUserInfo& CPEEngineInfoImpl::UserToUserInformation() const
    {
    return iCallCommandInfo.iUserToUserInfo;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetALSLine
// Sets active als line value to the TPEBasicInfo structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetALSLine( 
    const CCCECallParameters::TCCELineType& aALSLine ) 
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::SetALSLine: aALSLine: %d", aALSLine );
    __ASSERT_ALWAYS( ( CCCECallParameters::ECCELineTypePrimary <= aALSLine && aALSLine <= CCCECallParameters::ECCELineTypeAux ), 
        Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iALSLine = aALSLine;
    }


// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetALSLine
// Sets active als line value to the TPEBasicInfo structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetALSLineSupport( 
    const TBool& aALSLineSupport ) 
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::ALSLineSupport: aALSLineSupport: %d", aALSLineSupport );
    iBasicInfo.iALSLineSupport = aALSLineSupport;
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetAudioMuteCommand
// Sets audio mode of the phone to the TPEBasicInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetAudioMuteCommand( 
        const TBool& aAudioMute )
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::SetAudioMuteCommand, mute: %d", aAudioMute );
    iCallCommandInfo.iAudioMute = aAudioMute;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetAudioOutputCommand
// Sets audio output path to the TPECallCommandInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetAudioOutputCommand( 
        const TPEAudioOutput& aOutput,
        const TBool aShowNote
         )
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::SetAudioOutputCommand, output: %d", aOutput );
    __ASSERT_ALWAYS( ( EPENotActive <= aOutput && 
        aOutput <= EPETTY ), Panic( EPEPanicInvalidParameter ) );
    iCallCommandInfo.iAudioOutput = aOutput;
    iCallCommandInfo.iShowNote = aShowNote;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetAudioVolumeCommand
// Sets audio mode of the phone to the TPEBasicInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetAudioVolumeCommand( 
        const TInt& aAudioVolume )
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::SetAudioVolumeCommand, volume: %d", aAudioVolume );
    // No need to check that, is parameter value correct. Maximun value is 
    // platform depended. If the volume is not within this range, the volume 
    // is automatically set to minimum or maximum value based on the value 
    // that is being passed (by CMMFDevSound ).
    iCallCommandInfo.iAudioVolume = aAudioVolume;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallClientInformation
// Sets phone client information to the TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallClientInformation( 
        const MPEClientInformation& aClientInformation, 
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->ClientInformation().Set( aClientInformation );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallDirection
// Sets the call direction to the TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallDirection( 
        const RMobileCall::TMobileCallDirection& aCallDirection, 
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    __ASSERT_ALWAYS( ( RMobileCall::EDirectionUnknown <= aCallDirection && 
        aCallDirection <= RMobileCall::EMobileTerminated ), Panic( EPEPanicInvalidParameter ) );
    iCurrentCalls[ aCallId ]->SetCallDirection( aCallDirection );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallDuration
// Sets the duration of the call to the TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallDuration( 
        const TTimeIntervalSeconds& aCallDuration,  
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetDuration( aCallDuration );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallId
// Sets the identification number of the call to the TPECallCommandInfo structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallId( 
        const TInt& aCallId )
    {
    __ASSERT_ALWAYS( CallIdCheck::IsValid( aCallId ), 
        Panic( EPEPanicCallIndexOutOfRange ) );
    iCallCommandInfo.iCallId = aCallId;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallStartTime
// Sets the start time of the call to the TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallStartTime( 
        const TTime& aCallStartTime,
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetStartTime( aCallStartTime );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallState
// Sets the state of the call to the TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallState( 
        const TPEState& aCallState, 
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    __ASSERT_ALWAYS( !( aCallState > EPEStateMaximumState ), 
        Panic( EPEPanicInvalidParameter ) );
    iCurrentCalls[ aCallId ]->SetCallState( aCallState );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallType
// Sets the type of the call.
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallType( 
        const TPECallType& aCallType,   
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    __ASSERT_ALWAYS( ( EPECallTypeUninitialized <= aCallType &&
        aCallType <= EPECallTypeVoIP ), Panic( EPEPanicInvalidParameter ) );
    iCurrentCalls[ aCallId ]->SetCallType( aCallType );
    TEFLOGSTRING3( KTAINT, 
        "PE CPEEngineInfoImpl::SetCallType: call id: %d , aCallType: %d"
        ,aCallId ,aCallType );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallTypeCommand
// Sets call type information for following command message
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallTypeCommand( 
        const TPECallType& aCallType )
    {
    __ASSERT_ALWAYS( ( EPECallTypeUninitialized <= aCallType &&
        aCallType <= EPECallTypeVoIP ), Panic( EPEPanicInvalidParameter ) );
    iCallCommandInfo.iCallType = aCallType;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetConferenceCallState
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetConferenceCallState(
        const TPEState& aCallState )
    {
    __ASSERT_ALWAYS( !( aCallState > EPEStateMaximumState ), 
        Panic( EPEPanicInvalidParameter ) );
    iConferenceCallInfo.iConferenceCallState = aCallState;
    }



// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetDataPortName
// Sets the name of data port currently on-loan.
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetDataPortName(
        const TName& aDataPortName )
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::SetDataPortName: dataport name = %S", &aDataPortName );
    iBasicInfo.iDataPortName = aDataPortName;
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetDtmfStringCommand
// Sets the dtmf string value to the TPECommandInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetDtmfStringCommand( 
        const TPEDtmfString& aDtmfString )
    {
    __ASSERT_DEBUG( !( aDtmfString.Length() > KPEDtmfMaxLength ),
        Panic( EPEPanicInvalidParameter ) );
    iCallCommandInfo.iDtmfString = aDtmfString;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetKeyCode
// Sets the key code value to the TPECallCommandInfo structure.
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetKeyCode( 
        const TChar& aKeyCode )
    {
    iCallCommandInfo.iKeyCode = aKeyCode;
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetLoggingIndicator
// Set logging indicator to TPECallInfo.
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetLoggingIndicator(
        const TBool aLoggingIndicator, 
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    __ASSERT_ALWAYS( !CallIdCheck::IsConference( aCallId ), Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetLoggingEnabled( aLoggingIndicator );
    TEFLOGSTRING3( KTAINT, 
        "PE CPEEngineInfoImpl::SetLoggingIndicator: call id: %d , iLoggingEnabled: %d"
        ,aCallId ,aLoggingIndicator );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetMissedCall
// Set missed call indicator to TPECallInfo.
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetMissedCall(
        const TBool& aMissedCall,    
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    __ASSERT_ALWAYS( !CallIdCheck::IsConference( aCallId ), Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetMissedCall( aMissedCall );
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetPhoneNumber
// Sets the phone number to the TPECallCommandInfo structure.
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetPhoneNumber( 
        const TPEPhoneNumber& aPhoneNumber )
    {
    __ASSERT_DEBUG( !( aPhoneNumber.Length() > KPEPhoneNumberMaxLength ),
        Panic( EPEPanicExternalDataIdOutOfRange ) );
    iCallCommandInfo.iPhoneNumber = aPhoneNumber;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetRemoteIdentity
// Sets the remote party status to the TPECallInfo structure.
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetRemoteIdentity( 
    const RMobileCall::TMobileCallRemoteIdentityStatus& aIdentity, 
    const TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::SetRemoteIdentity: aIdentity = %d", aIdentity );
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    __ASSERT_ALWAYS( ( RMobileCall::ERemoteIdentityUnknown <= aIdentity &&
        aIdentity <= RMobileCall::ERemoteIdentityAvailableNoCliUnavailable ), 
        Panic( EPEPanicInvalidParameter ) );    
    iCurrentCalls[ aCallId ]->SetRemoteIdentity( aIdentity );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::TwoDigitSupportStatus
// Return the status of the two dial digit.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

const TBool& CPEEngineInfoImpl::TwoDigitSupportStatus() const
    {
    return iBasicInfo.iTwoDigitSupportStatus;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::VoiceMailBoxNumberLine1
// Gets the VoiceMailBoxNumber for line 1.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPEPhoneNumber& CPEEngineInfoImpl::VoiceMailBoxNumberLine1() const
    {
    return iBasicInfo.iVMBXNumbers.iVMBXL1;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::VoiceMailBoxNumberLine2
// Gets the VoiceMailBoxNumber for line 2.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPEPhoneNumber& CPEEngineInfoImpl::VoiceMailBoxNumberLine2() const
    {
    return iBasicInfo.iVMBXNumbers.iVMBXL2;
    }
    
// -----------------------------------------------------------------------------
// CPEEngienInfo::DTMFVoiceMailBoxNumberLine1
// Gets the DTMFVoiceMailBoxNumber for line 1.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPEPhoneNumber& CPEEngineInfoImpl::DTMFVoiceMailBoxNumberLine1() const
    {
    return iBasicInfo.iVMBXNumbers.iDTMFVMBXL1;
    }

// -----------------------------------------------------------------------------
// CPEEngienInfo::DTMFVoiceMailBoxNumberLine2
// Gets the DTMFVoiceMailBoxNumber for line 2.
// -----------------------------------------------------------------------------
//
const TPEPhoneNumber& CPEEngineInfoImpl::DTMFVoiceMailBoxNumberLine2() const
    {
    return iBasicInfo.iVMBXNumbers.iDTMFVMBXL2;
    }    

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetAlertForGroup
// Set the group id's which are active to play ring tone
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetAlertForGroup(
        const TArray< TContactItemId >& aAlertForGroup ) 
    {
    TInt errorCode( KErrNone );

    iAlertForGroup.Reset();
    for ( TInt i = 0; i < aAlertForGroup.Count(); i++ )
        {
        errorCode = iAlertForGroup.Append( aAlertForGroup[ i ] );
        TEFLOGSTRING3( 
            KTAINT, 
            "PE CPEEngineInfoImpl::SetAlertForGroup, index: %d, alert for: %d", 
            i, 
            aAlertForGroup[ i ] );
        }
    if ( errorCode != KErrNone )
        {
        TEFLOGSTRING2( KTAERROR,
            "CPEENGINEINFOIMPL::SETALERTFORGROUP ! RARRAY::APPEND FAILED WITH EXIT CODE: &d",
            errorCode );
        iAlertForGroup.Reset();
        }
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetAudioMute
// Sets audio mute state of the phone to TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetAudioMute(
        const TBool aAudioMute ) 
    {
    iBasicInfo.iAudioMute = aAudioMute; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetAudioOutput
// Sets hands free mode of the phone to TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetAudioOutput(
        const TPEAudioOutput aOutput,
        const TPEAudioOutput aPreviousOutput,
        const TBool aShowNote ) 
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::SetAudioOutput, output: %d", aOutput );
    __ASSERT_ALWAYS( ( EPENotActive <= aOutput && 
        aOutput <= EPETTY ), Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iPhoneAudioRouteParameters.iAudioOutput = aOutput;
    iBasicInfo.iPhoneAudioRouteParameters.iPreviousOutput = aPreviousOutput;
    iBasicInfo.iPhoneAudioRouteParameters.iShowNote = aShowNote;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetAvailableAudioOutputs
// Sets hands free mode of the phone to TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetAvailableAudioOutputs(
        const TArray<TPEAudioOutput>& aAvailableOutputs ) 
    {
    TEFLOGSTRING( KTAINT, "PE CPEEngineInfoImpl::SetAvailableAudioOutputs" );
    // reset the array
    iAvailableAudioOutputs.Reset();

    // set the available outputs
    for (TInt j = 0; j < aAvailableOutputs.Count(); j++)
        {
        iAvailableAudioOutputs[j] = aAvailableOutputs[j];
        TEFLOGSTRING3( 
            KTAINT, 
            "PE CPEEngineInfoImpl::SetAvailableAudioOutputs, index: %d, available: %d", 
            j, 
            aAvailableOutputs[j] );
        }
    TEFLOGSTRING( KTAINT, "PE CPEEngineInfoImpl::SetAvailableAudioOutputs:  Done" );    
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetAudioOutputPreference
// Sets audio output preference to TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetAudioOutputPreference(
        const TInt aOutputPreference )
    {    
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::SetAudioOutputPreference, value: %d", 
        aOutputPreference );    
    __ASSERT_ALWAYS( !( aOutputPreference != EPSAudioPrivate && 
        aOutputPreference != EPSAudioPublic ), Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iAudioOutputPreference = aOutputPreference;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetAudioVolume
// Sets audio volume of the phone to TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetAudioVolume(
        const TInt aAudioVolume )
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::SetAudioVolume, volume: %d", aAudioVolume );
    iBasicInfo.iAudioVolume = aAudioVolume;
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallALSLine
// Sets the alsline value to the TPECallInfo structure.
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallALSLine( 
        const CCCECallParameters::TCCELineType& aALSLine,
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    __ASSERT_ALWAYS( ( CCCECallParameters::ECCELineTypePrimary <= aALSLine && aALSLine <= CCCECallParameters::ECCELineTypeAux ),
        Panic( EPEPanicInvalidParameter ) );
    iCurrentCalls[ aCallId ]->SetAlsLine( aALSLine );
    TEFLOGSTRING3( KTAINT, 
        "PE CPEEngineInfoImpl::SetCallALSLine: line: %d, call id: %d", 
        iCurrentCalls[ aCallId ]->AlsLine(), 
        aCallId );
    }    

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallDuration
// Sets call duration of the last call to Idle to TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallDuration(
        const TInt& aCallDuration ) 
    {
    iBasicInfo.iLastCallDuration = aCallDuration; 
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetDtmfPostFix
// Sets dtmf postfix value to TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetDtmfPostFix( 
        const TPEDtmfString& aDtmfPostFix,
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    __ASSERT_DEBUG( !( aDtmfPostFix.Length() > KPEDtmfMaxLength ),
            Panic( EPEPanicInvalidParameter ) );
    iCurrentCalls[ aCallId ]->SetDtmfPostFix( aDtmfPostFix );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetDtmfString
// Sets dtmf string value to TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetDtmfString(
        const TPEDtmfString& aDtmfString ) 
    {
    __ASSERT_DEBUG( !( aDtmfString.Length() > KPEDtmfMaxLength ),
        Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iDtmfString = aDtmfString; 
    }
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetErrorCode
// Sets error code of the phone to TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetErrorCode(
        const TInt aErrorCode ) 
    {
    iBasicInfo.iErrorInfo.iErrorCode = aErrorCode; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetInbandTone
// Sets correct inband tone for specific call state transition or error
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetInbandTone( const TCCPTone aTone ) 
    {
    iBasicInfo.iInbandTone = aTone; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetKeypadVolume
// Sets keypad volume of the phone to TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetKeypadVolume(
        const TInt aKeypadVolume ) 
    {
    iBasicInfo.iKeypadVolume = aKeypadVolume; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetNetworkRegistrationStatus
// Sets network registration status to TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetNetworkRegistrationStatus(
        const TNWNetworkRegistrationStatus& aRegistrationStatus ) 
    {
    __ASSERT_ALWAYS( ( ENWStatusRegistrationUnknown <= aRegistrationStatus &&
        aRegistrationStatus <= ENWStatusRegisteredRoaming ), Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iNetworkRegistrationStatus = aRegistrationStatus;
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::SetNetworkRegistrationStatus, iNetworkRegistrationStatus: %d", 
        iBasicInfo.iNetworkRegistrationStatus );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetPersonalToneStatus
// Sets personal tone status of the phones profile settings to TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetPersonalToneStatus( 
        const TInt& aPersonalToneStatus ) 
    {
    iBasicInfo.iPersonalToneStatus = aPersonalToneStatus; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetPhoneNumberIsServiceCode
// Sets phone number parsing result
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetPhoneNumberIsServiceCode( 
    const TBool& aServiceCode )
    {
    iBasicInfo.iPhoneNumberIsServiceCode = aServiceCode;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallOriginCommand
// Sets call origin for the next call attempt
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallOriginCommand( const TPECallOrigin& aOrigin )
    {
    iCallCommandInfo.iCallOrigin = aOrigin;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetProfileId
// Sets profile id to TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetProfileId(
        const TInt aProfileId ) 
    {
    iBasicInfo.iProfileId = aProfileId; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetProfileName
// Sets profile name to TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetProfileName(
        const TPEProfileName& aProfileName ) 
    {
    __ASSERT_DEBUG( !( aProfileName.Length() > KPEProfileNameLength ), Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iProfileName = aProfileName; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetRingingTone1
// Sets ringing type to TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetRingingTone1(
        const TPEContactFileName& aRingingToneName )
    {
    __ASSERT_DEBUG( !( aRingingToneName.Length() > KMaxFileName ), 
        Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iRingingToneName1 = aRingingToneName; 
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetRingingTone2
// Sets ringing type to TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetRingingTone2( 
        const TPEContactFileName& aRingingToneName )
    {
    __ASSERT_DEBUG( !( aRingingToneName.Length() > KMaxFileName ), 
        Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iRingingToneName2 = aRingingToneName; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetVideoCallRingingTone
// Sets video call ringing tone name to TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetVideoCallRingingTone( 
        const TPEContactFileName& aRingingToneName )
    {
    __ASSERT_DEBUG( !( aRingingToneName.Length() > KMaxFileName ), 
        Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iVideoCallRingingTone = aRingingToneName; 
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetRingingType
// Sets ringing type to TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetRingingType(
        const TProfileRingingType& aRingingType )
    {
    __ASSERT_ALWAYS( ( EProfileRingingTypeRinging <= aRingingType && 
        aRingingType <= EProfileRingingTypeSilent ), Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iRingingType = aRingingType; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetRingingVolume
// Sets ringing volume to TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetRingingVolume(
        const TInt& aRingingVolume )
    {
    iBasicInfo.iRingingVolume = aRingingVolume; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetTextToSpeech
// Sets text-to-speech of the phone to TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetTextToSpeech( 
    const TBool aTextToSpeech )
    {
    iBasicInfo.iTextToSpeech = aTextToSpeech;
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetTwoDigitSupportStatus
// Sets two digit support status of the phone to TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetTwoDigitSupportStatus( 
    const TBool aTwoDigitSupportStatus )
    {
    iBasicInfo.iTwoDigitSupportStatus = aTwoDigitSupportStatus;
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallSecureStatus
// Sets Secure status to call info
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallSecureStatus( 
    const TBool aSecureStatus, 
    const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetSecureStatus( aSecureStatus );
    TEFLOGSTRING3( KTAINT, 
        "PE CPEEngineInfoImpl::SetCallSecureStatus, iSecureStatus: %d, call id: %d", 
        iCurrentCalls[ aCallId ]->SecureStatus(), 
        aCallId );
    }  

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetSecureSpecified
// Sets Secure status to call info
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetSecureSpecified( 
    const TBool aSecureSpecified )
    {
    iBasicInfo.iSecureSpecified = aSecureSpecified;
    }  

// -----------------------------------------------------------------------------
// CPEEngienInfo::SetDTMFVoiceMailBoxNumberLine1
// Sets DTMFVoiceMailBoxNumber for Line 1 information from TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetDTMFVoiceMailBoxNumberLine1( 
    const TDesC& aDTMFVoiceMailBoxNumberLine1 ) 
    { 
    __ASSERT_DEBUG( !( aDTMFVoiceMailBoxNumberLine1.Length() 
        > KPEPhoneNumberMaxLength ), Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iVMBXNumbers.iDTMFVMBXL1 = aDTMFVoiceMailBoxNumberLine1; 
    }

// -----------------------------------------------------------------------------
// CPEEngienInfo::SetDTMFVoiceMailBoxNumberLine2
// Sets DTMFVoiceMailBoxNumber for Line 2 information from 
// TPEBasicInfo-structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetDTMFVoiceMailBoxNumberLine2( 
    const TDesC& aDTMFVoiceMailBoxNumberLine2 )
    {
    __ASSERT_DEBUG( !( aDTMFVoiceMailBoxNumberLine2.Length() 
        > KPEPhoneNumberMaxLength ), Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iVMBXNumbers.iDTMFVMBXL2 = aDTMFVoiceMailBoxNumberLine2; 
    }
    
// -----------------------------------------------------------------------------
// CPEEngienInfo::SetVoiceMailBoxNumberLine1
// Gets VoiceMailBoxNumber for Line 1 information from TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetVoiceMailBoxNumberLine1( 
    const TDesC& aVoiceMailBoxNumberLine1 ) 
    {
    __ASSERT_DEBUG( !( aVoiceMailBoxNumberLine1.Length() 
        > KPEPhoneNumberMaxLength ), Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iVMBXNumbers.iVMBXL1 = aVoiceMailBoxNumberLine1; 
    }

// -----------------------------------------------------------------------------
// CPEEngienInfo::SetVoiceMailBoxNumberLine2
// Gets VoiceMailBoxNumber for Line 2 information from TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetVoiceMailBoxNumberLine2( 
    const TDesC& aVoiceMailBoxNumberLine2 )
    {
    __ASSERT_DEBUG( !( aVoiceMailBoxNumberLine2.Length() 
        > KPEPhoneNumberMaxLength ), Panic( EPEPanicInvalidParameter ) );
    iBasicInfo.iVMBXNumbers.iVMBXL2 = aVoiceMailBoxNumberLine2; 
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetLogForcedCopletion
// Set the log forced completion indicator.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetLogForcedCompletion(
    const TBool aForcedCompletion,  
    const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetLogForcedCompletion( aForcedCompletion );
    }    

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::GetLogForcedCompletion
// Gets the log forced completion indicator
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::LogForcedCompletion( 
    const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->LogForcedCompletion();
    }
   
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetPhoneNumber
// Sets the phone number to the TPECallCommandInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetWholeOutgoingPhoneNumber( 
        const TPEPhoneNumber& aPhoneNumber, 
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    __ASSERT_DEBUG( !( aPhoneNumber.Length() > KPEPhoneNumberMaxLength ),
        Panic( EPEPanicInvalidParameter ) );
    iCurrentCalls[ aCallId ]->SetWholeOutgoingPhoneNumber( aPhoneNumber );
    TEFLOGSTRING3( KTAINT, 
        "PE CPEEngineInfoImpl::SetWholeOutgoingPhoneNumber: phone number: '%S', call id: %d"
        , &iCurrentCalls[ aCallId ]->WholeOutgoingPhoneNumber(), aCallId );
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::PhoneNumber
// Return the phone number from the TPECallCommandInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPEPhoneNumber& CPEEngineInfoImpl::WholeOutgoingPhoneNumber( 
        const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->WholeOutgoingPhoneNumber();
    }
                
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ServiceIdCommand
// Return the Service Id from the TPECallCommandInfo structure.
// -----------------------------------------------------------------------------
//
TUint32 CPEEngineInfoImpl::ServiceIdCommand() const
    {
    return iCallCommandInfo.iServiceId; 
    } 

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetServiceIdCommand
// Set the Service Id to the TPECallCommandInfo structure.
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetServiceIdCommand( TUint32 aServiceId )
    {
    iCallCommandInfo.iServiceId = aServiceId; 
    }
                
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ServiceId
// Return the Service Id from the TPECallInfo structure.
// -----------------------------------------------------------------------------
//
TUint32 CPEEngineInfoImpl::ServiceId( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->CallServiceId(); 
    } 

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetServiceId
// Set the Service Id to the TPECallInfo structure.
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetServiceId( const TInt aCallId, TUint32 aServiceId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetCallServiceId( aServiceId ); 
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ActiveForwardingInfo
// Gets active forward info from the TPEBasicInfo structure. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPEActiveForwardInfo& CPEEngineInfoImpl::ActiveForwardingInfo() const
    {
    return iBasicInfo.iActiveForward;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::BarringInfo
// Gets the call barring information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPEBarringInfo& CPEEngineInfoImpl::BarringInfo() const
    {
    return iBasicInfo.iBarringInfo;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallConference
// Gets the call's conference value.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEEngineInfoImpl::CallConference( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->Conference();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallControlCaps
// Gets the call control caps
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPECallControlCaps CPEEngineInfoImpl::CallControlCaps( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->CallControlCaps();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallForwarded
// Gets the call forwarded value. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::CallForwarded( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->Forwarded();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallService
// Gets Service value.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RMobilePhone::TMobileService CPEEngineInfoImpl::CallService( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->Service();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ConferenceCallCaps
// Gets Conference call capabilities.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint32 CPEEngineInfoImpl::ConferenceCallCaps( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->ConferenceCaps();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ConferenceMemberName 
// Gets added or removed Conference members TName information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TName& CPEEngineInfoImpl::ConferenceMemberName( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->ConferenceMemberName();
    }
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::IsConferenceMemberId 
// Gets added or removed Conference members TName information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEEngineInfoImpl::IsConferenceMemberId( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->ConferenceMemberId();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::EmergencyNumber
// Gets emergency number for checking.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPEPhoneNumber& CPEEngineInfoImpl::EmergencyNumber() const
    {
    return iCallCommandInfo.iEmergencyNumber;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::NumberOfConferenceMembers
// Gets number of conference members involved in conference call.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEEngineInfoImpl::NumberOfConferenceMembers( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->NumberOfConferenceMembers();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SimState 
// Gets Sim state from the TPEBasicInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPESimState& CPEEngineInfoImpl::SimState() const
    {
    return iBasicInfo.iSimState;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SecureSpecified
// Gets secure specified status 
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::SecureSpecified( ) const
    {
    return iBasicInfo.iSecureSpecified;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SSCommandInfo
// Gets the last suplementary services string command information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPESSCommandInfo& CPEEngineInfoImpl::SSCommandInfo() const
    {
    return iBasicInfo.iSSCommandInfo;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ResetCallInfo
// Resets TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::ResetCallInfo( const TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfoImpl::ResetCallInfo CallId %d", aCallId );
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );

    iCurrentCalls[aCallId]->Reset( CallIdCheck::IsConference( aCallId ) );
    
    if ( iRemoteInfo->Count() &&       // Check that array is not empty
         ( *iRemoteInfo )[aCallId ]     // Check that there is content in this index
       )
        {
        ( *iRemoteInfo )[aCallId ]->ResetRemoteInfo();
        }
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallConference
// Sets the conference value to the TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallConference( const TInt aConference, const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicIllegalCommand ) );
    iCurrentCalls[ aCallId ]->SetConference( aConference );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallControlCaps
// Sets the call control caps to the TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallControlCaps( const TPECallControlCaps& aCallControlCaps,
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicIllegalCommand ) );
    iCurrentCalls[ aCallId ]->SetCallControlCaps( aCallControlCaps );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallForwarded
// Sets the forwarded value to the TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallForwarded( const TBool aCallForwarded, const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetForwarded( aCallForwarded );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallService
// Sets the call service to the TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallService( const RMobilePhone::TMobileService& aCallService,
        const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetService( aCallService );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetConferenceCallCaps
// Sets the conference call capabilities to the TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetConferenceCallCaps( const TUint32 aCaps, const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetConferenceCaps( aCaps );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetConferenceMemberInfo
// Sets the added or removed conference members TName information to the TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetConferenceMemberInfo( const TName& aConferenceMemberName, 
        const TInt aConferenceMemberCallId, const TInt aCallId  )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetConferenceMemberName( aConferenceMemberName );
    iCurrentCalls[ aCallId ]->SetConferenceMemberId( aConferenceMemberCallId ); 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetEmergencyNumber
// Sets the emergency numbers of the phone to the TPEBasicInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetEmergencyNumber( const TPEPhoneNumber& aEmergencyNumber )
    {
    iCallCommandInfo.iEmergencyNumber = aEmergencyNumber;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::RemotePartyName
// Remote party's network given name. (CNAP, skype id).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPEContactName& CPEEngineInfoImpl::RemotePartyName( 
    const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->RemotePartyName();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ErrorCode
// Error code from basic call info.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPEEngineInfoImpl::ErrorCode() const
    {
    return iBasicInfo.iErrorInfo.iErrorCode;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetNumberOfConferenceMembers
// Sets the number of conference members involved in Conference
// call to the TPECallInfo structure.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetNumberOfConferenceMembers( const TInt aNumberOfMembers, const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetNumberOfConferenceMembers( aNumberOfMembers );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ResumeHeldCall
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::ResumeHeldCall( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->ResumeHeldCall(); 
    }

// -----------------------------------------------------------------------------
// From base class MPEDataStore
// -----------------------------------------------------------------------------
//
TInt CPEEngineInfoImpl::CallIndex( TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->CallIndex(); 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetBarringInfo
// Sets Barring information to TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetBarringInfo( const TPEBarringInfo& aBarringInfo ) 
    {
    iBasicInfo.iBarringInfo = aBarringInfo;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetDeflectInfo
// Sets deflect information to the TPEBasicInfo structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetDeflectInfo( const TPEActiveForwardInfo& aActiveForward )
    {
    iBasicInfo.iActiveForward  = aActiveForward;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetResumeHeldCall
// Set iHeldByUser value to TPECallInfo structure
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetResumeHeldCall( const TBool aResumeHeldCall, 
    const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetResumeHeldCall( aResumeHeldCall ); 
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetSimState
// Sets Sim state to TPEBasicInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetSimState( const TPESimState& aSimState )
    {
    iBasicInfo.iSimState = aSimState;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetSSCommandInfo
// Sets Suplementary Services command information to TPESSCommandInfo-structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetSSCommandInfo( const TPESSCommandInfo& aSSCommandInfo ) 
    {
    iBasicInfo.iSSCommandInfo = aSSCommandInfo; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetUserToUserInfo
// Set the user to user info.
// -----------------------------------------------------------------------------
//    
void CPEEngineInfoImpl::SetUserToUserInformation( const TPEUserToUserInfo& aUUI )
    {
    iCallCommandInfo.iUserToUserInfo = aUUI;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetUserToUserInfo
// Set the user to user info.
// -----------------------------------------------------------------------------
//   
void CPEEngineInfoImpl::SetRemotePartyName( 
    const TPEContactName& aName, 
    const TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetRemotePartyName( aName ); 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallParameters
// Get the call parameters.
// -----------------------------------------------------------------------------
// 
CCCECallParameters& CPEEngineInfoImpl::CallParameters()
    {
    return *iCallParams;
    }


// -----------------------------------------------------------------------------
// From base class MPEDataStore
// Sets call index
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallIndex( TInt aCallIndex, TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetCallIndex( aCallIndex );
    }

// -----------------------------------------------------------------------------
// From base class MPEDataStore
// Sets call origin
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallOrigin( TPECallOrigin aOrigin, TInt aCallId )
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->SetCallOrigin( aOrigin ); 
    }


// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::UnattendedTransferTarget
// -----------------------------------------------------------------------------
//
const TPEPhoneNumber& CPEEngineInfoImpl::UnattendedTransferTarget( 
        TInt aCallId ) const
    {
    __ASSERT_DEBUG( aCallId < iCurrentCalls.Count(), 
        Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->UnattendedTransferTarget(); 
    }


// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetUnattendedTransferTarget
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetUnattendedTransferTarget( const TPEPhoneNumber& aTarget,
        TInt aCallId )
    {
    __ASSERT_DEBUG( aCallId < iCurrentCalls.Count(),
        Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetUnattendedTransferTarget( aTarget );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ForwardAddressChoices
// -----------------------------------------------------------------------------
//
const CDesC8Array* CPEEngineInfoImpl::ForwardAddressChoices( 
        TInt aCallId ) const
    {
    __ASSERT_DEBUG( aCallId < iCurrentCalls.Count(), 
        Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->ForwardAddressChoices(); 
    }


// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetForwardAddressChoices
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetForwardAddressChoices( const CDesC8Array* aArray,
        TInt aCallId )
    {
    __ASSERT_DEBUG( aCallId < iCurrentCalls.Count(),
        Panic( EPEPanicCallIndexOutOfRange ) );
    iCurrentCalls[ aCallId ]->SetForwardAddressChoices( aArray );
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetForwardAddressIndex
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetForwardAddressIndex( TInt aIndex )
    {
    iCallCommandInfo.iForwardToAddressIndex = aIndex;
    }
    
// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ForwardAddressIndex
// -----------------------------------------------------------------------------
//
TInt CPEEngineInfoImpl::ForwardAddressIndex()
    {
    return iCallCommandInfo.iForwardToAddressIndex;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetTransferTargetCommand
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetTransferTargetCommand( const TPEPhoneNumber& aTransferTarget )
    {
    iCallCommandInfo.iTransferToAddress = aTransferTarget; 
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::TransferTarget
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TPEPhoneNumber& CPEEngineInfoImpl::TransferTargetCommand() const
    {
    return iCallCommandInfo.iTransferToAddress;
    }


// -----------------------------------------------------------------------------
// CPEEngineInfo::CallOrigin
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPECallOrigin CPEEngineInfoImpl::CallOrigin( const TInt aCallId ) const
    {
    __ASSERT_DEBUG( iCurrentCalls.Count() > aCallId, Panic( EPEPanicCallIndexOutOfRange ) );
    return iCurrentCalls[ aCallId ]->CallOrigin();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallerImage
// -----------------------------------------------------------------------------
//
const TPtrC CPEEngineInfoImpl::CallerImage( const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->CallerImage();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallerText
// -----------------------------------------------------------------------------
//
const TPtrC CPEEngineInfoImpl::CallerText( const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->CallerText();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CallerThumbnail
// -----------------------------------------------------------------------------
//
CFbsBitmap* CPEEngineInfoImpl::CallerThumbnail( const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->CallerThumbnail();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::HasCallerThumbnail
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::HasCallerThumbnail( const TInt aCallId ) const
   {
    return ( *iRemoteInfo )[ aCallId ]->HasCallerThumbnail();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ContactId
// -----------------------------------------------------------------------------
//
const CPhCntContactId& CPEEngineInfoImpl::ContactId( const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->ContactId();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ContactGroups
// -----------------------------------------------------------------------------
//
const TArray< TContactItemId > CPEEngineInfoImpl::ContactGroups(
    const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->ContactGroups();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ContactLink
// -----------------------------------------------------------------------------
//
const TDesC8& CPEEngineInfoImpl::ContactLink( const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->ContactLink();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::PersonalRingingTone
// -----------------------------------------------------------------------------
//
const TPEContactFileName& CPEEngineInfoImpl::PersonalRingingTone(
    const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->PersonalRingingTone();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::RemoteCompanyName
// -----------------------------------------------------------------------------
//
const TPEContactCompany& CPEEngineInfoImpl::RemoteCompanyName(
    const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->CompanyName();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::RemoteName
// -----------------------------------------------------------------------------
//
const TPEContactName& CPEEngineInfoImpl::RemoteName( const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->Name();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::RemotePhoneNumber
// -----------------------------------------------------------------------------
//
const TPEPhoneNumber& CPEEngineInfoImpl::RemotePhoneNumber(
    const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->PhoneNumber();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::RemotePhoneNumberType
// -----------------------------------------------------------------------------
//
const TPEPhoneNumberIdType& CPEEngineInfoImpl::RemotePhoneNumberType(
    const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->PhoneNumberType();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::RemoteTextToSpeechText
// -----------------------------------------------------------------------------
//
const TPtrC CPEEngineInfoImpl::RemoteTextToSpeechText(
    const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->TextToSpeechText();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::RemotePredefinedDtmfStrings
// -----------------------------------------------------------------------------
//
const  CDesCArray& CPEEngineInfoImpl::RemotePredefinedDtmfStrings(
    const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->PredefinedDtmfStrings();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::RemoteColpNumber
// -----------------------------------------------------------------------------
//
const TPEPhoneNumber& CPEEngineInfoImpl::RemoteColpNumber(
    const TInt aCallId ) const
    {
    return ( *iRemoteInfo )[ aCallId ]->ColpNumber();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ResetRemoteInfo
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::ResetRemoteInfo( const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->ResetRemoteInfo();
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallerImage
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallerImage(
    const TPtrC aCallerImage, 
    const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetCallerImage( aCallerImage );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallerText
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallerText(
    const TPtrC aCallerText, 
    const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetCallerText( aCallerText );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetCallerThumbnail
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetCallerThumbnail(
    CFbsBitmap* aCallerThumbnail, 
    const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetCallerThumbnail( aCallerThumbnail );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetHasCallerThumbnail
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetHasCallerThumbnail(
    TBool aStatus, 
    const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetHasCallerThumbnail( aStatus );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetPredefinedDtmfStrings
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetPredefinedDtmfStrings( 
    const CDesCArray& aArray, 
    const TInt aCallId )            
    {
    return ( *iRemoteInfo )[ aCallId ]->SetPredefinedDtmfStrings( aArray );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetContactGroups
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetContactGroups(
    TArray< TContactItemId >& aGroupIdArray,
    const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetContactGroups( aGroupIdArray );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetContactId
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetContactId(
        CPhCntContactId* aContactId,
        const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetContactId( aContactId );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetPersonalRingingTone
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetPersonalRingingTone(
    const TPEContactFileName& aRingingTonepath,
    const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetPersonalRingingTone( aRingingTonepath );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetRemoteCompanyName
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetRemoteCompanyName(
    const TPEContactName& aCompanyName, 
    const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetCompanyName( aCompanyName );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetRemoteColpNumber
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetRemoteColpNumber(
    const TPEPhoneNumber& aColpNumber, 
    const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetColpNumber( aColpNumber );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetRemoteName
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetRemoteName(
    const TPEContactName& aContactName, 
    const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetName( aContactName );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetRemotePhoneNumber
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetRemotePhoneNumber(
    const TPEPhoneNumber& aPhoneNumber, 
    const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetPhoneNumber( aPhoneNumber );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetRemotePhoneNumberType
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetRemotePhoneNumberType(
    TPEPhoneNumberIdType aPhoneNumberType, 
    const TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetPhoneNumberType( aPhoneNumberType );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetRemoteTextToSpeechText
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetRemoteTextToSpeechText(
    HBufC* aText,
    TInt aCallId )
    {
    return ( *iRemoteInfo )[ aCallId ]->SetTextToSpeechText( aText );
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::CheckIfCallStateExists
// Checks if given state can be found.
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::CheckIfCallStateExists( const TPEState& aCallState )
    {
    TInt count = iCurrentCalls.Count();
    TBool stateExists(EFalse);
    for ( TInt i = 0; i < count; i++ )
      {
      if( aCallState == iCurrentCalls[ i ]->CallState() )
          {
          stateExists = ETrue;
          break;
          }
      }
    return stateExists;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetContactId
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetContactId2( const TInt aContactId )
{
    TEFLOGSTRING2( KTAINT, 
        "PE CPEEngineInfo::SetContactId: aContactId: %d", aContactId );
    iContactId = aContactId;    
}

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ContactId
// -----------------------------------------------------------------------------
//
TInt CPEEngineInfoImpl::ContactId2 () const
    {
    TEFLOGSTRING2( KTAINT, "ContactId: %d", iContactId );
    return iContactId;      
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetProtocolError
// Sets the protocol spesific error code
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetProtocolError( TInt aError, TInt aCallId )
    {
    if ( 0<=aCallId && aCallId<iCurrentCalls.Count() )
        {
        iCurrentCalls[ aCallId ]->SetProtocolError( aError );
        }
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::ProtocolError
// Returns the protocol spesific error code
// -----------------------------------------------------------------------------
//
TInt CPEEngineInfoImpl::ProtocolError( TInt aCallId )
    {
    TInt ret( KErrNotFound );
    
    if ( 0<=aCallId && aCallId<iCurrentCalls.Count() )
        {
        ret = iCurrentCalls[ aCallId ]->ProtocolError();
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::IsOutgoingCallBarringActivated
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::IsOutgoingCallBarringActivated()
    {
    return iBasicInfo.iOutgoingBarringActivated;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetOutgoingCallBarringActivated
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetOutgoingCallBarringActivated( 
        TBool aActivated )
    {
    iBasicInfo.iOutgoingBarringActivated = aActivated;
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::SetProtocolError
// Sets the protocol spesific error code
// -----------------------------------------------------------------------------
//
void CPEEngineInfoImpl::SetIncomingCallForwarded( TBool aForwarded, TInt aCallId )
    {
    if ( 0<=aCallId && aCallId<iCurrentCalls.Count() )
        {
        iCurrentCalls[ aCallId ]->SetIncomingCallForwarded( aForwarded );
        }
    }

// -----------------------------------------------------------------------------
// CPEEngineInfoImpl::IncomingCallForwarded
// -----------------------------------------------------------------------------
//
TBool CPEEngineInfoImpl::IncomingCallForwarded( TInt aCallId ) const
    {
    TBool ret( EFalse );
    
    if ( 0<=aCallId && aCallId<iCurrentCalls.Count() )
        {
        ret = iCurrentCalls[ aCallId ]->IncomingCallForwarded();
        }
    
    return ret;
    }

// End of File
