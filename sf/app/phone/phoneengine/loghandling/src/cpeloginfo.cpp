/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPELogInfo class 
*                member functions.
*
*/

//  INCLUDE FILES
#include "cpeloginfo.h"
#include "talogger.h"


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CPELogInfo* CPELogInfo::NewL()
    {
    return new( ELeave ) CPELogInfo();
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CPELogInfo::~CPELogInfo()
    {
    delete iContactLink;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CPELogInfo::CPELogInfo() : iEventType( CPELogInfo::EPEUnknownEvent ),
    iCallDirection( RMobileCall::EDirectionUnknown ),
    iCallId( KPECallIdNotUsed ),
    iCallState( EPEStateUnknown ),
    iCurrentLine( CCCECallParameters::ECCELineTypePrimary ), 
    iPhoneNumberId( EPEUnknownNumber )
    {
    }


// ---------------------------------------------------------------------------
// Set basic event data, guarantees data existence
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetEventData( 
    TInt aCallId, 
    const MPEDataStore& aDataStore )
    {
    //CallId
    iCallId = aCallId;

    // Line
    iCurrentLine = aDataStore.CallALSLine( aCallId );

    // EventType
    switch ( aDataStore.CallType( aCallId ) )
        {
        case EPECallTypeCSVoice:

            iEventType = EPEVoiceEvent;
            break;

        case EPECallTypeVoIP:

            iEventType = EPEVoIPEvent;

            break;
        case EPECallTypeVideo:
            iEventType = EPEVideoCallEvent;
            break;
        case EPECallTypeUninitialized:
        default:
            iEventType = EPEUnknownEvent;
            break;
        }

    // CallDirection
    iCallDirection = aDataStore.CallDirection( aCallId );
   
    // Name
    if ( aDataStore.RemoteName( aCallId ).Length() > 0 )
        {
        iName = aDataStore.RemoteName( aCallId );
        }
    else if ( aDataStore.RemoteCompanyName( aCallId ).Length() > 0 ) 
        {
        iName = aDataStore.RemoteCompanyName( aCallId );
        }
    else
        {
        iName = KNullDesC;
        }
      
    if ( iCallDirection == RMobileCall::EMobileTerminated )
        {
        // adjust log info name in case of suppressed and unknown remote identity for MT calls only
        RMobileCall::TMobileCallRemoteIdentityStatus remoteIdentity;
        remoteIdentity = aDataStore.RemoteIdentity( aCallId );

        switch ( remoteIdentity )
            {
            case RMobileCall::ERemoteIdentityUnknown:
                iName = KPELogUnknownText;
                break;
            case RMobileCall::ERemoteIdentityAvailableNoCliCoinOrPayphone:
            case RMobileCall::ERemoteIdentityUnavailableNoCliCoinOrPayphone:
                iName = KLogsPayphoneText;
                break;
            case RMobileCall::ERemoteIdentitySuppressed:
                if ( aDataStore.RemotePhoneNumber( aCallId ).Length() == 0 )
                    {
                    iName = KLogsPrivateText;
                    }
                break;
            default:
                break;
            }
        }

    // CallState
    iCallState = aDataStore.CallState( aCallId );
    
    // CallDuration
    iDuration = aDataStore.CallDuration( aCallId );
   
    // PhoneNumberType
    TPEPhoneNumberIdType numberType = aDataStore.RemotePhoneNumberType( aCallId );
    iPhoneNumberId = numberType;

    //Start time
    iCallStartTime = aDataStore.CallStartTime( aCallId );
    
    //Missed call
    iMissedCall = aDataStore.MissedCall( aCallId );

    //LoggingEnabled
    iLoggingEnabled = aDataStore.LoggingIndicator( aCallId );
    
    // Get log forced completion indicator
    iForcedCompletion = aDataStore.LogForcedCompletion( aCallId );
     
    // Set log event type in case of emergency call
    if ( numberType == EPEEmergencyNumber )
        {
        TEFLOGSTRING( KTAINT, "LOG CPELogHandling::CreateLogInfo() Emergecy call" );
        iEventType = EPEEmergecyEvent;
        }

    iServiceId = aDataStore.ServiceId( aCallId );
 
    iMyAddress = KNullDesC();
    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::CopyL( const CPELogInfo& aLogInfo )
    {
    iEventType = aLogInfo.iEventType;
    iCallDirection = aLogInfo.iCallDirection;
    iCallId = aLogInfo.iCallId;
    iPhoneNumber = aLogInfo.iPhoneNumber;
    iName = aLogInfo.iName;
    iCallState = aLogInfo.iCallState;
    iDuration = aLogInfo.iDuration;
    iCurrentLine = aLogInfo.iCurrentLine;
    iPhoneNumberId = aLogInfo.iPhoneNumberId;
    iLoggingEnabled = aLogInfo.iLoggingEnabled;
    iCallStartTime = aLogInfo.iCallStartTime;
    iMissedCall = aLogInfo.iMissedCall;
    iForcedCompletion = aLogInfo.iForcedCompletion;
    iServiceId = aLogInfo.iServiceId;
    iVoipAddress = aLogInfo.iVoipAddress;
    iMyAddress = aLogInfo.iMyAddress;
    if ( iContactLink )
        {
        delete iContactLink;
        iContactLink = NULL;
        }
    if ( aLogInfo.iContactLink )
        {
        iContactLink = aLogInfo.iContactLink->AllocL();
        }
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::Reset()
    {
    iEventType = CPELogInfo::EPEUnknownEvent;
    iCallDirection = RMobileCall::EDirectionUnknown;
    iCallId = KPECallIdNotUsed;
    iPhoneNumber = KNullDesC;
    iName = KNullDesC;       
    iCallState = EPEStateUnknown;
    iDuration = 0; 
    iCurrentLine = CCCECallParameters::ECCELineTypePrimary; 
    iPhoneNumberId = EPEUnknownNumber;
    iLoggingEnabled = EFalse;
    iCallStartTime = 0;
    iMissedCall = EFalse;   
    iForcedCompletion = EFalse;
    iVoipAddress = KNullDesC;
    iMyAddress = KNullDesC;
    delete iContactLink;
    iContactLink = NULL;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetEventType( TPELogEventType aEventType )
    {
    iEventType = aEventType;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CPELogInfo::TPELogEventType CPELogInfo::EventType() const
    {
    return iEventType;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetCallDirection( RMobileCall::TMobileCallDirection aCallDirection )
    {
    iCallDirection = aCallDirection;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
RMobileCall::TMobileCallDirection CPELogInfo::CallDirection() const
    {
    return iCallDirection;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetCallId( TInt aCallId )
    {
    iCallId = aCallId;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CPELogInfo::CallId() const
    {
    return iCallId;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber )
    {
    iPhoneNumber = aPhoneNumber;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TPEPhoneNumber& CPELogInfo::PhoneNumber() const
    {
    return iPhoneNumber;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetName( const TPEContactName& aName )
    {
    iName = aName;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TPEContactName& CPELogInfo::Name() const
    {
    return iName;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetCallState( TPEState aCallState )
    {
    iCallState = aCallState;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPEState CPELogInfo::CallState() const
    {
    return iCallState;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetDuration( TTimeIntervalSeconds aDuration )
    {
    iDuration = aDuration;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TTimeIntervalSeconds CPELogInfo::Duration() const
    {
    return iDuration;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetCurrentLine( CCCECallParameters::TCCELineType aCurrentLine )
    {
    iCurrentLine = aCurrentLine;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCCECallParameters::TCCELineType CPELogInfo::CurrentLine() const
    {
    return iCurrentLine;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetPhoneNumberId( TPEPhoneNumberIdType aPhoneNumberId )
    {
    iPhoneNumberId = aPhoneNumberId;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPEPhoneNumberIdType CPELogInfo::PhoneNumberId() const
    {
    return iPhoneNumberId;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetLoggingEnabled( TBool aLoggingEnabled )
    {
    iLoggingEnabled = aLoggingEnabled;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CPELogInfo::LoggingEnabled() const
    {
    return iLoggingEnabled;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetCallStartTime( TTime aCallStartTime )
    {
    iCallStartTime = aCallStartTime;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TTime CPELogInfo::CallStartTime() const
    {
    return iCallStartTime;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetMissedCall( TBool aMissedCall )
    {
    iMissedCall = aMissedCall;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CPELogInfo::MissedCall() const
    {
    return iMissedCall;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetForcedCompletion( TBool aForcedCompletion )
    {
    iForcedCompletion = aForcedCompletion;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CPELogInfo::ForcedCompletion() const
    {
    return iForcedCompletion;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetServiceId( TUint32 aServiceId )
    {
    iServiceId = aServiceId;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint32 CPELogInfo::ServiceId() const
    {
    return iServiceId;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetVoipAddress( const TPEPhoneNumber& aVoipAddress )
    {
    iVoipAddress = aVoipAddress;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TPEPhoneNumber& CPELogInfo::VoipAddress() const
    {
    return iVoipAddress;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetMyAddress( const TPEPhoneNumber& aMyAddress )
    {
    iMyAddress = aMyAddress;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TPEPhoneNumber& CPELogInfo::MyAddress() const
    {
    return iMyAddress;
    }


// ---------------------------------------------------------------------------
// Save contact link. Ownership is transferred.
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetContactLink( const HBufC8* aContactLink )
    {
    delete iContactLink;
    iContactLink = aContactLink;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const HBufC8& CPELogInfo::ContactLink() const
    {
    return *iContactLink;
    }
