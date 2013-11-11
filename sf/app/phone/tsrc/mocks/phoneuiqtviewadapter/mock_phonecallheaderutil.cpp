/** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#include <QDebug>
#include <smcmockclassincludes.h>
#include "phonecallheaderutil.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::PhoneCallHeaderUtil
// -----------------------------------------------------------------------------
//
PhoneCallHeaderUtil::PhoneCallHeaderUtil( 
        MPEEngineInfo & engineInfo )
    : m_engineInfo(engineInfo)
    {
    
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::~PhoneCallHeaderUtil
// -----------------------------------------------------------------------------
//
PhoneCallHeaderUtil::~PhoneCallHeaderUtil(  )
    {
    
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::LabelText
// -----------------------------------------------------------------------------
//
const TDesC & PhoneCallHeaderUtil::LabelText( 
        TInt aCallId )
    {
    SMC_MOCK_METHOD1( const TDesC &, TInt, aCallId )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::GetCliTexts
// -----------------------------------------------------------------------------
//
void PhoneCallHeaderUtil::GetCliTexts( 
        TInt aCallId,
        TDes & aCliText,
        ClippingDirection & aCliClip,
        TDes & aSecondaryCliText,
        ClippingDirection & aSecondaryCliClip )
    {
    SMC_MOCK_METHOD5( void, TInt, aCallId, 
        TDes &, aCliText, 
        ClippingDirection &, aCliClip, 
        TDes &, aSecondaryCliText, 
        ClippingDirection &, aSecondaryCliClip )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::GetCli
// -----------------------------------------------------------------------------
//
void PhoneCallHeaderUtil::GetCli( 
        TInt aCallId,
        TDes & aCliText,
        ClippingDirection & aClipping )
    {
    SMC_MOCK_METHOD3( void, TInt, aCallId, 
        TDes &, aCliText, 
        ClippingDirection &, aClipping )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::GetSecondaryCli
// -----------------------------------------------------------------------------
//
void PhoneCallHeaderUtil::GetSecondaryCli( 
        TInt aCallId,
        TDes & aSecondaryCliText,
        ClippingDirection & aClipping )
    {
    SMC_MOCK_METHOD3( void, TInt, aCallId, 
        TDes &, aSecondaryCliText, 
        ClippingDirection &, aClipping )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::RemotePhoneNumber
// -----------------------------------------------------------------------------
//
const TDesC & PhoneCallHeaderUtil::RemotePhoneNumber( 
        TInt aCallId ) const
    {
    SMC_MOCK_METHOD1( const TDesC &, TInt, aCallId )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::CallType
// -----------------------------------------------------------------------------
//
TInt PhoneCallHeaderUtil::CallType( 
        const TInt aCallId )
    {
    SMC_MOCK_METHOD1( TInt, const TInt, aCallId )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::ServiceId
// -----------------------------------------------------------------------------
//
TInt PhoneCallHeaderUtil::ServiceId( 
        const TInt aCallId )
    {
    SMC_MOCK_METHOD1( TInt, const TInt, aCallId )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::IsCallForwarded
// -----------------------------------------------------------------------------
//
TBool PhoneCallHeaderUtil::IsCallForwarded( 
        TInt aCallId )
    {
    SMC_MOCK_METHOD1( TBool, TInt, aCallId )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::SecureSpecified
// -----------------------------------------------------------------------------
//
TBool PhoneCallHeaderUtil::SecureSpecified(  )
    {
    SMC_MOCK_METHOD0( TBool )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::Ciphering
// -----------------------------------------------------------------------------
//
TBool PhoneCallHeaderUtil::Ciphering( 
        TInt aCallId )
    {
    SMC_MOCK_METHOD1( TBool, TInt, aCallId )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::CallState
// -----------------------------------------------------------------------------
//
TInt PhoneCallHeaderUtil::CallState( 
        TInt aCallId ) const
    {
    SMC_MOCK_METHOD1( TInt, TInt, aCallId )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::CallerImage
// -----------------------------------------------------------------------------
//
const TDesC & PhoneCallHeaderUtil::CallerImage( 
        const TInt aCallId )
    {
    SMC_MOCK_METHOD1( const TDesC &, const TInt, aCallId )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::EmergencyHeaderText
// -----------------------------------------------------------------------------
//
const TDesC & PhoneCallHeaderUtil::EmergencyHeaderText(  )
    {
    SMC_MOCK_METHOD0( const TDesC & )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::AttemptingEmergencyText
// -----------------------------------------------------------------------------
//
const TDesC & PhoneCallHeaderUtil::AttemptingEmergencyText(  )
    {
    SMC_MOCK_METHOD0( const TDesC & )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::IsVoiceCall
// -----------------------------------------------------------------------------
//
TBool PhoneCallHeaderUtil::IsVoiceCall( 
        TInt aCallId ) const
    {
    SMC_MOCK_METHOD1( TBool, TInt, aCallId )
    }


// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::IsVideoCall
// -----------------------------------------------------------------------------
//
TBool PhoneCallHeaderUtil::IsVideoCall( 
        int aCallId ) const
    {
    SMC_MOCK_METHOD1( TBool, int, aCallId )
    }


