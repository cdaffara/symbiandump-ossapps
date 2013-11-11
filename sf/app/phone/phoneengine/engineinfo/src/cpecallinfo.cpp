/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Information container for a single call
*
*/

#include "cpecallinfo.h"
#include "cpeclientinformation.h"

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPECallInfo* CPECallInfo::NewL()
    {
    CPECallInfo* self = new ( ELeave ) CPECallInfo;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CPECallInfo::CPECallInfo()
    {
    Reset( EFalse );
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CPECallInfo::ConstructL()
    {
    iClientInformation = CPEClientInformation::NewL();
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CPECallInfo::~CPECallInfo()
    {
    delete iClientInformation;
    }

// ---------------------------------------------------------------------------
// Resets all call information to their initial values.
// ---------------------------------------------------------------------------
//
void CPECallInfo::Reset( TBool aIsConference )
    {
    if ( iClientInformation )
        {
        iClientInformation->Reset();
        }
    
    if ( aIsConference )
        {
        iCallState = EPEStateConferenceIdle;
        iConference = KPEConferenceMaster;
        }
    else
        {
        iCallState = EPEStateIdle;
        iConference = KPENormalVoiceCall;
        }
    
    _LIT( dateTimeInit, "0,0,0,0,0,0,0" );
    iStartTime.Set( dateTimeInit );

    iCallType = EPECallTypeUninitialized;
    iCallDirection = RMobileCall::EDirectionUnknown;
    iRemoteIdentity = RMobileCall::ERemoteIdentityUnknown;
    iLoggingEnabled = ETrue;
    iMissedCall = EFalse;
    iALSLine = CCCECallParameters::ECCELineTypePrimary;
    iLogForcedCompletion = EFalse;
    iWholeOutgoingPhoneNumber = KNullDesC;
    iCallServiceId = 0;
    iSecureStatus = EFalse;
    iResumeHeldCall = ETrue;
    iForwarded = EFalse;
    iService = RMobilePhone::EServiceUnspecified;
    iConferenceCaps = 0;
    iConferenceMemberName = KNullDesC;
    iConferenceMemberId = KPECallIdNotUsed;
    iNumberOfConferenceMembers = 0;
    iRemotePartyName = KNullDesC;
    iSecureSpecified = ETrue; 
    iTransferTarget = KNullDesC;
    iDtmfPostFix = KNullDesC;
    iForwardAddressChoices = NULL;
    iCallIndex = KPEInvalidCallIndex;
    iProtocolError = KErrNotFound;
    iIncomingCallForwarded = EFalse;
    }

// ---------------------------------------------------------------------------
// Returns the client information of the call.
// ---------------------------------------------------------------------------
//
CPEClientInformation& CPECallInfo::ClientInformation()
    {
    return *iClientInformation;
    }

// ---------------------------------------------------------------------------
// Sets the state of the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetCallState( TPEState aState )
    {
    iCallState = aState;
    }

// ---------------------------------------------------------------------------
// Returns the state of the call.
// ---------------------------------------------------------------------------
//
TPEState CPECallInfo::CallState() const
    {
    return iCallState;
    }

// ---------------------------------------------------------------------------
// Sets the type of the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetCallType( TPECallType aType )
    {
    iCallType = aType;
    }

// ---------------------------------------------------------------------------
// Returns the type of the call.
// ---------------------------------------------------------------------------
//
TPECallType CPECallInfo::CallType() const
    {
    return iCallType;
    }

// ---------------------------------------------------------------------------
// Sets the call control capabilites.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetCallControlCaps( TPECallControlCaps aCaps )
    {
    iCallControlCaps = aCaps;
    }

// ---------------------------------------------------------------------------
// Returns the call control capabilities.
// ---------------------------------------------------------------------------
//
TPECallControlCaps CPECallInfo::CallControlCaps() const
    {
    return iCallControlCaps;
    }

// ---------------------------------------------------------------------------
// Sets the remote party name of the call as given by the network.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetRemotePartyName( const TPEContactName& aName )
    {
    iRemotePartyName = aName;
    }

// ---------------------------------------------------------------------------
// Returns the remote party name of the call as given by the network.
// ---------------------------------------------------------------------------
//
const TPEContactName& CPECallInfo::RemotePartyName() const
    {
    return iRemotePartyName;
    }

// ---------------------------------------------------------------------------
// Sets the whole outgoing phone number (invalid characters removed).
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetWholeOutgoingPhoneNumber( const TPEPhoneNumber& aNumber )
    {
    iWholeOutgoingPhoneNumber = aNumber;
    }

// ---------------------------------------------------------------------------
// Returns the whole outgoing phone number (invalid characters removed).
// ---------------------------------------------------------------------------
//
const TPEPhoneNumber& CPECallInfo::WholeOutgoingPhoneNumber() const
    {
    return iWholeOutgoingPhoneNumber;
    }

// ---------------------------------------------------------------------------
// Sets the ALS line type of the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetAlsLine( CCCECallParameters::TCCELineType aAlsLine )
    {
    iALSLine = aAlsLine;
    }

// ---------------------------------------------------------------------------
// Returns the ALS line type of the call.
// ---------------------------------------------------------------------------
//
CCCECallParameters::TCCELineType CPECallInfo::AlsLine() const
    {
    return iALSLine;
    }

// ---------------------------------------------------------------------------
// Sets the remote identity status of the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetRemoteIdentity( RMobileCall::TMobileCallRemoteIdentityStatus aIdentity )
    {
    iRemoteIdentity = aIdentity;
    }

// ---------------------------------------------------------------------------
// Returns the remote identity status of the call.
// ---------------------------------------------------------------------------
//
RMobileCall::TMobileCallRemoteIdentityStatus CPECallInfo::RemoteIdentity() const
    {
    return iRemoteIdentity;
    }

// ---------------------------------------------------------------------------
// Sets the direction of the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetCallDirection( RMobileCall::TMobileCallDirection aDirection )
    {
    iCallDirection = aDirection;
    }

// ---------------------------------------------------------------------------
// Returns the direction of the call.
// ---------------------------------------------------------------------------
//
RMobileCall::TMobileCallDirection CPECallInfo::CallDirection() const
    {
    return iCallDirection;
    }

// ---------------------------------------------------------------------------
// Sets the mobile call service type of the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetService( RMobilePhone::TMobileService aService )
    {
    iService = aService;
    }

// ---------------------------------------------------------------------------
// Returns the mobile call service type of the call.
// ---------------------------------------------------------------------------
//
RMobilePhone::TMobileService CPECallInfo::Service() const
    {
    return iService;
    }

// ---------------------------------------------------------------------------
// Sets the conference member name of the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetConferenceMemberName( const TName& aName )
    {
    iConferenceMemberName = aName;
    }

// ---------------------------------------------------------------------------
// Returns the conference member name of the call.
// ---------------------------------------------------------------------------
//
const TName& CPECallInfo::ConferenceMemberName() const
    {
    return iConferenceMemberName;
    }

// ---------------------------------------------------------------------------
// Sets the call start time.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetStartTime( const TTime& aTime )
    {
    iStartTime = aTime;
    }

// ---------------------------------------------------------------------------
// Returns the start time of the call.
// ---------------------------------------------------------------------------
//
const TTime& CPECallInfo::StartTime() const
    {
    return iStartTime;
    }

// ---------------------------------------------------------------------------
// Sets the duration of the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetDuration( TTimeIntervalSeconds aDuration )
    {
    iDuration = aDuration;
    }

// ---------------------------------------------------------------------------
// Returns the duration of the call.
// ---------------------------------------------------------------------------
//
TTimeIntervalSeconds CPECallInfo::Duration() const
    {
    return iDuration;
    }

// ---------------------------------------------------------------------------
// Sets the service identifier of the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetCallServiceId( TUint32 aId )
    {
    iCallServiceId = aId;
    }

// ---------------------------------------------------------------------------
// Returns the service identifier of the call.
// ---------------------------------------------------------------------------
//
TUint32 CPECallInfo::CallServiceId() const
    {
    return iCallServiceId;
    }

// ---------------------------------------------------------------------------
// Sets conference capabilities of the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetConferenceCaps( TUint32 aCaps )
    {
    iConferenceCaps = aCaps;
    }

// ---------------------------------------------------------------------------
// Returns the conference capabilities of the call.
// ---------------------------------------------------------------------------
//
TUint32 CPECallInfo::ConferenceCaps() const
    {
    return iConferenceCaps;
    }

// ---------------------------------------------------------------------------
// Sets the identification number of the conference master.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetConference( TInt aConference )
    {
    iConference = aConference;
    }

// ---------------------------------------------------------------------------
// Returns the identification number of the conference master.
// ---------------------------------------------------------------------------
//
TInt CPECallInfo::Conference() const
    {
    return iConference;
    }

// ---------------------------------------------------------------------------
// Sets the identifier of the conference member call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetConferenceMemberId( TInt aId )
    {
    iConferenceMemberId = aId;
    }

// ---------------------------------------------------------------------------
// Returns the identifier of the conference member call.
// ---------------------------------------------------------------------------
//
TInt CPECallInfo::ConferenceMemberId()
    {
    return iConferenceMemberId;
    }

// ---------------------------------------------------------------------------
// Sets the number of conference members.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetNumberOfConferenceMembers( TInt aNumberOfConferenceMembers )
    {
    iNumberOfConferenceMembers = aNumberOfConferenceMembers;
    }

// ---------------------------------------------------------------------------
// Returns the number of conference members.
// ---------------------------------------------------------------------------
//
TInt CPECallInfo::NumberOfConferenceMembers() const
    {
    return iNumberOfConferenceMembers;
    }

// ---------------------------------------------------------------------------
// Sets the forced completion indicator for logging.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetLogForcedCompletion( TBool aLog )
    {
    iLogForcedCompletion = aLog;
    }

// ---------------------------------------------------------------------------
// Returns the flag indicating if forced completion indicator for logging is used.
// ---------------------------------------------------------------------------
//
TBool CPECallInfo::LogForcedCompletion()
    {
    return iLogForcedCompletion;
    }

// ---------------------------------------------------------------------------
// Enables or disables logging for the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetLoggingEnabled( TBool aEnabled )
    {
    iLoggingEnabled = aEnabled;
    }

// ---------------------------------------------------------------------------
// Returns if logging is enabled for the call.
// ---------------------------------------------------------------------------
//
TBool CPECallInfo::LoggingEnabled() const
    {
    return iLoggingEnabled;
    }

// ---------------------------------------------------------------------------
// Sets the missed call status for the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetMissedCall( TBool aMissedCall )
    {
    iMissedCall = aMissedCall;
    }

// ---------------------------------------------------------------------------
// Returns the missed call status for the call.
// ---------------------------------------------------------------------------
//
TBool CPECallInfo::MissedCall() const
    {
    return iMissedCall;
    }

// ---------------------------------------------------------------------------
// Sets the secure status for the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetSecureStatus( TBool aSecureStatus )
    {
    iSecureStatus = aSecureStatus;
    }

// ---------------------------------------------------------------------------
// Returns the secure status of the call.
// ---------------------------------------------------------------------------
//
TBool CPECallInfo::SecureStatus() const
    {
    return iSecureStatus;
    }

// ---------------------------------------------------------------------------
// Sets the forwarded status for the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetForwarded( TBool aForwarded )
    {
    iForwarded = aForwarded;
    }

// ---------------------------------------------------------------------------
// Returns the forwarded status of the call.
// ---------------------------------------------------------------------------
//
TBool CPECallInfo::Forwarded() const
    {
    return iForwarded;
    }

// ---------------------------------------------------------------------------
// Sets the resume held call status for the call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetResumeHeldCall( TBool aResume )
    {
    iResumeHeldCall = aResume;
    }

// ---------------------------------------------------------------------------
// Returns the resume held call status of the call.
// ---------------------------------------------------------------------------
//
TBool CPECallInfo::ResumeHeldCall() const
    {
    return iResumeHeldCall;
    }

// ---------------------------------------------------------------------------
// Sets the secure specified status 
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetSecureSpecified( TBool aSecureSpecified )
    {
    iSecureSpecified = aSecureSpecified; 
    }

// ---------------------------------------------------------------------------
// Returns the secure specified status 
// ---------------------------------------------------------------------------
//
TBool CPECallInfo::SecureSpecified() const
    {
    return iSecureSpecified;
    }

// ---------------------------------------------------------------------------
// Returns unattended transfer target.
// ---------------------------------------------------------------------------
//
const TPEPhoneNumber& CPECallInfo::UnattendedTransferTarget() const
    {
    return iTransferTarget;
    }


// ---------------------------------------------------------------------------
// Sets unattended transfer target. 
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetUnattendedTransferTarget( const TPEPhoneNumber& aTarget )
    {
    iTransferTarget = aTarget;
    }

// ---------------------------------------------------------------------------
// Returns address array for call forward.
// ---------------------------------------------------------------------------
//
const CDesC8Array* CPECallInfo::ForwardAddressChoices() const
    {
    return iForwardAddressChoices;
    }


// ---------------------------------------------------------------------------
// Sets call forward address array. 
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetForwardAddressChoices( const CDesC8Array* aArray )
    {
    iForwardAddressChoices = aArray;
    }
    
// ---------------------------------------------------------------------------
// Sets array index. 
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetForwardAddressIndex( TInt aIndex )
    {
    iForwardAddressArrayIndex = aIndex;
    }

// ---------------------------------------------------------------------------
// Return dtmf postfix. 
// ---------------------------------------------------------------------------
//
const TPEDtmfString& CPECallInfo::DtmfPostFix() const
    {
    return iDtmfPostFix;
    }

// ---------------------------------------------------------------------------
// Sets dtmf postfix. 
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetDtmfPostFix( const TPEDtmfString& aDtmfPostFix )
    {
    iDtmfPostFix = aDtmfPostFix;
    }

// ---------------------------------------------------------------------------
// Sets call index
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetCallIndex( TInt aCallIndex )
    {
    iCallIndex = aCallIndex;
    }

// ---------------------------------------------------------------------------
// Gets call index
// ---------------------------------------------------------------------------
//
TInt CPECallInfo::CallIndex() const
    {
    return iCallIndex;
    }

// ---------------------------------------------------------------------------
// Sets the origin of a call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetCallOrigin( TPECallOrigin aOrigin )
    {
    iCallOrigin = aOrigin;
    }

// ---------------------------------------------------------------------------
// Gets the origin of a call.
// ---------------------------------------------------------------------------
//
TPECallOrigin CPECallInfo::CallOrigin() const
    {
    return iCallOrigin;
    }
    
// ---------------------------------------------------------------------------
// Sets the protocol error of a call.
// ---------------------------------------------------------------------------
//
void CPECallInfo::SetProtocolError( TInt aError )
    {
    iProtocolError = aError;
    }

// ---------------------------------------------------------------------------
// Returns protocol error of a call.
// ---------------------------------------------------------------------------
//
TInt CPECallInfo::ProtocolError() const
    {
    return iProtocolError;
    }

// -----------------------------------------------------------------------------
// CPECallInfo::SetIncomingCallForwarded
// -----------------------------------------------------------------------------
//
void CPECallInfo::SetIncomingCallForwarded( TBool aForwarded )
    {
    iIncomingCallForwarded = aForwarded;
    }

// -----------------------------------------------------------------------------
// CPECallInfo::IncomingCallForwarded
// -----------------------------------------------------------------------------
//
TBool CPECallInfo::IncomingCallForwarded() const
    {
    return iIncomingCallForwarded;
    }

// End of File
