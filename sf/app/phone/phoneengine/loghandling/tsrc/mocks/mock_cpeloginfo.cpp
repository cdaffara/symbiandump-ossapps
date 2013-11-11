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
#include <smcmockclassincludes.h>
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
    SMC_MOCK_METHOD2( void, TInt, aCallId, const MPEDataStore&, aDataStore )	    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::CopyL( const CPELogInfo& aLogInfo )
    {
    SMC_MOCK_METHOD1( void, const CPELogInfo&, aLogInfo )	
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::Reset()
    {
    SMC_MOCK_METHOD0( void )	
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetEventType( TPELogEventType aEventType )
    {
    SMC_MOCK_METHOD1( void, TPELogEventType, aEventType )	
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CPELogInfo::TPELogEventType CPELogInfo::EventType() const
    {
    SMC_MOCK_METHOD0( CPELogInfo::TPELogEventType )	
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetCallDirection( RMobileCall::TMobileCallDirection aCallDirection )
    {
    SMC_MOCK_METHOD1( void, RMobileCall::TMobileCallDirection, aCallDirection )	
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
RMobileCall::TMobileCallDirection CPELogInfo::CallDirection() const
    {
    SMC_MOCK_METHOD0( RMobileCall::TMobileCallDirection )	
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetCallId( TInt aCallId )
    {
    SMC_MOCK_METHOD1( void, TInt, aCallId )	
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CPELogInfo::CallId() const
    {
    SMC_MOCK_METHOD0( TInt )	
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber )
    {
    SMC_MOCK_METHOD1( void, const TPEPhoneNumber&, aPhoneNumber )	
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TPEPhoneNumber& CPELogInfo::PhoneNumber() const
    {
    SMC_MOCK_METHOD0( TPEPhoneNumber& ) 
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetName( const TPEContactName& aName )
    {
    SMC_MOCK_METHOD1( void, const TPEContactName&, aName )	
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TPEContactName& CPELogInfo::Name() const
    {
    SMC_MOCK_METHOD0( TPEContactName& )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetCallState( TPEState aCallState )
    {
    SMC_MOCK_METHOD1( void, TPEState, aCallState )	
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPEState CPELogInfo::CallState() const
    {
    SMC_MOCK_METHOD0( TPEState )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetDuration( TTimeIntervalSeconds aDuration )
    {
    SMC_MOCK_METHOD1( void, TTimeIntervalSeconds, aDuration )	
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TTimeIntervalSeconds CPELogInfo::Duration() const
    {
    SMC_MOCK_METHOD0( TTimeIntervalSeconds )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetCurrentLine( CCCECallParameters::TCCELineType aCurrentLine )
    {
    SMC_MOCK_METHOD1( void, CCCECallParameters::TCCELineType, aCurrentLine )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCCECallParameters::TCCELineType CPELogInfo::CurrentLine() const
    {
    SMC_MOCK_METHOD0( CCCECallParameters::TCCELineType )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetPhoneNumberId( TPEPhoneNumberIdType aPhoneNumberId )
    {
    SMC_MOCK_METHOD1( void, TPEPhoneNumberIdType, aPhoneNumberId )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TPEPhoneNumberIdType CPELogInfo::PhoneNumberId() const
    {
    SMC_MOCK_METHOD0( TPEPhoneNumberIdType )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetLoggingEnabled( TBool aLoggingEnabled )
    {
    SMC_MOCK_METHOD1( void, TBool, aLoggingEnabled )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CPELogInfo::LoggingEnabled() const
    {
    SMC_MOCK_METHOD0( TBool )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetCallStartTime( TTime aCallStartTime )
    {
    SMC_MOCK_METHOD1( void, TTime, aCallStartTime )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TTime CPELogInfo::CallStartTime() const
    {
    SMC_MOCK_METHOD0( TTime )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetMissedCall( TBool aMissedCall )
    {
    SMC_MOCK_METHOD1( void, TBool, aMissedCall )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CPELogInfo::MissedCall() const
    {
    SMC_MOCK_METHOD0( TBool )
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetForcedCompletion( TBool aForcedCompletion )
    {
    SMC_MOCK_METHOD1( void, TBool, aForcedCompletion )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CPELogInfo::ForcedCompletion() const
    {
    SMC_MOCK_METHOD0( TBool )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetServiceId( TUint32 aServiceId )
    {
    SMC_MOCK_METHOD1( void, TUint32, aServiceId )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint32 CPELogInfo::ServiceId() const
    {
    SMC_MOCK_METHOD0( TUint32 )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetVoipAddress( const TPEPhoneNumber& aVoipAddress )
    {
    SMC_MOCK_METHOD1( void, const TPEPhoneNumber&, aVoipAddress )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TPEPhoneNumber& CPELogInfo::VoipAddress() const
    {
    SMC_MOCK_METHOD0( TPEPhoneNumber& )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetMyAddress( const TPEPhoneNumber& aMyAddress )
    {
    SMC_MOCK_METHOD1( void, const TPEPhoneNumber&, aMyAddress )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const TPEPhoneNumber& CPELogInfo::MyAddress() const
    {
    SMC_MOCK_METHOD0( TPEPhoneNumber& )
    }


// ---------------------------------------------------------------------------
// Save contact link. Ownership is transferred.
// ---------------------------------------------------------------------------
//
void CPELogInfo::SetContactLink( const HBufC8* aContactLink )
    {
    SMC_MOCK_METHOD1( void, const HBufC8*, aContactLink )
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
const HBufC8& CPELogInfo::ContactLink() const
    {
    SMC_MOCK_METHOD0( const HBufC8& )
    }
