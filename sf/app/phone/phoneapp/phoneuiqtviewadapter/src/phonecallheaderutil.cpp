/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A parameter class for setting call header information
*
*/


#include <StringLoader.h>
#include <bautils.h>

#include "phonecallheaderutil.h"
#include "cphonemainresourceresolver.h"
#include "phonerssbase.h"
#include "cphonecenrepproxy.h"
#include "telephonyvariant.hrh"
#include "phoneviewcommanddefinitions.h"
#include "phonelogger.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::PhoneCallHeaderUtil
// -----------------------------------------------------------------------------
//  
PhoneCallHeaderUtil::PhoneCallHeaderUtil(
        MPEEngineInfo& engineInfo ) 
        : m_engineInfo( engineInfo),
          iLabelText( NULL ),
          iCliText( NULL ),
          iSecondaryCliText( NULL ),
          iCallerImage( NULL ),
          iEmergencyHeaderText( NULL ),
          iAttemptingEmergencyText( NULL )
    {
    }

// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::~PhoneCallHeaderUtil
// -----------------------------------------------------------------------------
//  
PhoneCallHeaderUtil::~PhoneCallHeaderUtil()
    {
    delete iLabelText;
    delete iCliText;
    delete iSecondaryCliText;
    delete iCallerImage;
    delete iEmergencyHeaderText;
    delete iAttemptingEmergencyText;
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::LabelText
// -----------------------------------------------------------
//
const TDesC& PhoneCallHeaderUtil::LabelText( TInt aCallId )
    {

    if (iLabelText)
        {
        delete iLabelText;
        iLabelText = NULL;
        }
    
    switch (m_engineInfo.CallState(aCallId))
        {
        case EPEStateDisconnecting:
            {
            iLabelText = LoadResource(EPhoneInCallDisconnected);
            break;
            }
        case EPEStateHeld:
        case EPEStateHeldConference:
            {
            iLabelText = LoadResource(EPhoneCallOnHold);
            break;
            }
        case EPEStateConnectedConference:
            {
            iLabelText = LoadResource(EPhoneCLIConferenceCall);
            break;
            }
        case EPEStateRinging:
            {
            SetCallHeaderLabelTextForRingingCall(aCallId);
            break;
            }
        case EPEStateDialing:
            {
            if ( IsVideoCall(aCallId) )
                {
                iLabelText = LoadResource(EPhoneOutgoingVideoCallLabel);
                }
            else
                {
                iLabelText = LoadResource(EPhoneOutgoingCallLabel);
                }
            break;
            }
        default:
            break;
        }
    
    return iLabelText ? *iLabelText : KNullDesC();
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::GetCliTexts
// -----------------------------------------------------------
//
void PhoneCallHeaderUtil::GetCliTexts( 
        TInt aCallId,
        TDes& aCliText,
        ClippingDirection &aCliClip,
        TDes& aSecondaryCliText,
        ClippingDirection &aSecondaryCliClip )
    {
    if ( aCallId == KEmergencyCallId )
        {
        // Set "Emergency call" as cli in emergency call.
        aCliText = EmergencyHeaderText();
        aCliClip = PhoneCallHeaderUtil::ERight;
        }
    else if ( aCallId == KConferenceCallId )
        {
        // No need for secondary cli in conference call.
        GetCli( aCallId, aCliText, aCliClip );
        }
    else
        {   
        GetCli( aCallId, aCliText, aCliClip );
        GetSecondaryCli( aCallId, aSecondaryCliText, aSecondaryCliClip );
        }

    }
    
// -----------------------------------------------------------
// PhoneCallHeaderUtil::GetCli
// -----------------------------------------------------------
//
void PhoneCallHeaderUtil::GetCli( TInt aCallId,
        TDes& aCliText,
        ClippingDirection &aClipping )
    {
    if (iCliText)
        {
        delete iCliText;
        iCliText = NULL;
        }
    
    if ( aCallId == KEmergencyCallId )
        {
        aCliText = EmergencyHeaderText();
        aClipping = PhoneCallHeaderUtil::ERight;
        return;
        }
    else if ( aCallId == KConferenceCallId )
        {
        iCliText = LoadResource(EPhoneCLIConferenceCall);
        }
    else if ( ( m_engineInfo.RemotePhoneNumber( aCallId ).Length() ) && 
         ( !RemoteNameAvailable( aCallId ) ) )
        {   
        iCliText = m_engineInfo.RemotePhoneNumber( aCallId ).Alloc();  
        aClipping = PhoneCallHeaderUtil::ELeft;
        }
    else
        {
        TBuf<KCntMaxTextFieldLength> remoteInfoText( KNullDesC ); 
        GetRemoteInfoData( aCallId, remoteInfoText );
        
        iCliText = remoteInfoText.Alloc();
        aClipping = PhoneCallHeaderUtil::ERight;
        }
    
    aCliText = iCliText ? *iCliText : KNullDesC();
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::GetSecondaryCli
// -----------------------------------------------------------
//
void PhoneCallHeaderUtil::GetSecondaryCli( TInt aCallId,
        TDes& aSecondaryCliText,
        ClippingDirection &aClipping )
    {
    if (iSecondaryCliText)
        {
        delete iSecondaryCliText;
        iSecondaryCliText = NULL;
        }
    
    if ( IsSecondaryCliAllowed( aCallId ) )
        {
        if (CPhoneCenRepProxy::Instance()->
                IsTelephonyFeatureSupported( KTelephonyLVFlagUUS ) &&
            m_engineInfo.RemotePartyName( aCallId ).Length() )
            {
            iSecondaryCliText = m_engineInfo.RemotePartyName( aCallId ).Alloc();
            aClipping = PhoneCallHeaderUtil::ERight;
            }
        else
            {
            iSecondaryCliText = m_engineInfo.RemotePhoneNumber( aCallId ).Alloc();
            aClipping = PhoneCallHeaderUtil::ELeft;
            }
        }
    
    aSecondaryCliText = iSecondaryCliText ? *iSecondaryCliText : KNullDesC();
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::RemotePhoneNumber
// -----------------------------------------------------------
//
const TDesC& PhoneCallHeaderUtil::RemotePhoneNumber( TInt aCallId ) const
    {
    return m_engineInfo.RemotePhoneNumber( aCallId );
    }

// ---------------------------------------------------------------------------
//  PhoneCallHeaderUtil::CallType
// ---------------------------------------------------------------------------
//
TInt PhoneCallHeaderUtil::CallType( 
        const TInt aCallId )
    {
    return  (TInt)m_engineInfo.CallType( aCallId );
    }

// ---------------------------------------------------------------------------
//  PhoneCallHeaderUtil::ServiceId
// ---------------------------------------------------------------------------
//
TInt PhoneCallHeaderUtil::ServiceId( 
        const TInt aCallId )
    {
    return m_engineInfo.ServiceId( aCallId );
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::IsCallForwarded
// -----------------------------------------------------------
//
TBool PhoneCallHeaderUtil::IsCallForwarded( TInt aCallId )
    {
    return m_engineInfo.IncomingCallForwarded(aCallId);
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::SecureSpecified
// -----------------------------------------------------------
//
TBool PhoneCallHeaderUtil::SecureSpecified()
    {
    return m_engineInfo.SecureSpecified();
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::Ciphering
// -----------------------------------------------------------
//
TBool PhoneCallHeaderUtil::Ciphering( TInt aCallId )
    {
    return m_engineInfo.IsSecureCall( aCallId );
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::CallState
// -----------------------------------------------------------
//
TInt PhoneCallHeaderUtil::CallState( TInt aCallId ) const
    {
    return m_engineInfo.CallState(aCallId);
    }

// -----------------------------------------------------------------------------
// PhoneCallHeaderUtil::CallerImage
// -----------------------------------------------------------------------------
//     
const TDesC& PhoneCallHeaderUtil::CallerImage( const TInt aCallId )
    {
    if (iCallerImage)
        {
        delete iCallerImage;
        iCallerImage = NULL;
        }
    
    // Set the call header picture data if it is available
    if ( ( m_engineInfo.CallerImage( aCallId ).Length() > 0 )  && 
         ( BaflUtils::FileExists( CCoeEnv::Static()->FsSession(), 
                 m_engineInfo.CallerImage( aCallId ) ) ) )
        {
        iCallerImage = m_engineInfo.CallerImage( aCallId ).Alloc();
        }

    return iCallerImage ? *iCallerImage : KNullDesC();
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::EmergencyHeaderText
// -----------------------------------------------------------
//
const TDesC& PhoneCallHeaderUtil::EmergencyHeaderText()
    {
    if (!iEmergencyHeaderText)
        {
        iEmergencyHeaderText = LoadResource(EPhoneEmergencyCallHeader);
        }
     
    return iEmergencyHeaderText ? *iEmergencyHeaderText : KNullDesC();
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::AttemptingEmergencyText
// -----------------------------------------------------------
//
const TDesC& PhoneCallHeaderUtil::AttemptingEmergencyText()
    {
    if (!iAttemptingEmergencyText)
        {
        iAttemptingEmergencyText = LoadResource(EPhoneAttemptingEmergencyText);
        }
     
    return iAttemptingEmergencyText ? *iAttemptingEmergencyText : KNullDesC();
    }

// ---------------------------------------------------------------------------
//  PhoneCallHeaderUtil::IsVoiceCall
// ---------------------------------------------------------------------------
//
TBool PhoneCallHeaderUtil::IsVoiceCall(TInt aCallId) const
    {
    if( aCallId < 0 )
        {
        return ( m_engineInfo.CallTypeCommand()
                    == EPECallTypeCSVoice ||
                m_engineInfo.CallTypeCommand()
                    == EPECallTypeVoIP );  
        }
    
    return ( m_engineInfo.CallType( aCallId )
                == EPECallTypeCSVoice ||
             m_engineInfo.CallType( aCallId )
                == EPECallTypeVoIP );
    }

// ---------------------------------------------------------------------------
//  PhoneCallHeaderUtil::IsVideoCall
// ---------------------------------------------------------------------------
//
TBool PhoneCallHeaderUtil::IsVideoCall(int aCallId ) const
    {
    if( aCallId < 0 )
        {
        return ( m_engineInfo.CallTypeCommand()
            == EPECallTypeVideo );  
        }
    return ( m_engineInfo.CallType( aCallId )
            == EPECallTypeVideo );
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::SetCallHeaderLabelTextForRingingCall
// -----------------------------------------------------------
//
void PhoneCallHeaderUtil::SetCallHeaderLabelTextForRingingCall( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "PhoneCallHeaderUtil::CallHeaderLabelTextForRingingCall( ) ");
    
    TInt labelId(0);
    
    // Fetch engine info parameters.
    const TBool auxLine( m_engineInfo.CallALSLine( aCallId ) == CCCECallParameters::ECCELineTypeAux );
    const TBool cli( m_engineInfo.RemotePhoneNumber( aCallId ).Length());
    const TBool cnap( m_engineInfo.RemotePartyName( aCallId ).Length());
    const TBool videoCall( IsVideoCall( aCallId ) );
    const TBool waitingCall( IsWaitingCall( aCallId ) );
    const TInt numberType( m_engineInfo.RemotePhoneNumberType( aCallId ));

    if ( !cli && !cnap && numberType != EPEPrivateNumber && numberType != EPEUnknownNumber )
        {
        if ( auxLine )
            {
            if ( waitingCall )
                {
                labelId = EPhoneIncomingLine2WaitingText; // waiting, line 2
                }
            else
                {
                labelId = EPhoneIncomingLine2Text; // on line 2
                }
            }
        // If CLIR, but also network limitation(e.g. EPEUnknownNumber), then second line 
        // should be empty in call bubble.
        else
            {
            labelId = KPhoneRssCommonFirst; // No second line in call bubble
            }
        }
    else  // Voice or video call with CLI or with CNAP.
        {
        if ( waitingCall )
            {
            if ( auxLine  )
                {
                labelId = EPhoneIncomingLine2WaitingText; // waiting, line 2
                }
            else
                {
                labelId = EPhoneCallWaitingLabel; // waiting
                }
            }
        else // Mo other calls
            {
            if ( auxLine )
                {
                labelId = EPhoneIncomingLine2CallingText; // calling, line 2
                }
            else
                {
                // If CLIR, but not network limitation, then second line 
                // (calling or video call) should be shown in call bubble.
                if ( videoCall )
                    {
                    labelId = EPhoneVideoCallIncoming; // video call
                    }
                else
                    {
                    labelId = EPhoneIncomingCallLabel; // calling
                    }
                }
            }
        }
    __PHONELOG1( EBasic, EPhoneControl, "PhoneCallHeaderUtil::SetCallHeaderTexts - labelId(%d)", labelId );
    iLabelText = LoadResource(labelId) ;
    }

// ---------------------------------------------------------------------------
//  PhoneCallHeaderUtil::RemoteNameAvailable
// ---------------------------------------------------------------------------
//
TBool PhoneCallHeaderUtil::RemoteNameAvailable( const TInt aCallId ) const
    {
    __LOGMETHODSTARTEND(EPhoneControl, "PhoneCallHeaderUtil::ContactInfoAvailable( ) ");
    
    TBool ret( m_engineInfo.RemoteName( aCallId ).Length() || 
               m_engineInfo.RemoteCompanyName( aCallId ).Length() );
    
    if ( EFalse == ret && m_engineInfo.RemotePartyName( aCallId ).Length() )
        {
        ret = !CPhoneCenRepProxy::Instance()->
                IsTelephonyFeatureSupported( KTelephonyLVFlagUUS );
        }
               
    return ret;
    }

// ---------------------------------------------------------------------------
//  PhoneCallHeaderUtil::GetRemoteInfoDataL
// ---------------------------------------------------------------------------
//
void PhoneCallHeaderUtil::GetRemoteInfoData( 
        const TInt aCallId, 
        TDes& aData ) const 
    {
    __LOGMETHODSTARTEND( EPhoneControl, "PhoneCallHeaderUtil::GetRemoteInfoDataL() ");
    __PHONELOG1( EBasic, EPhoneControl, "PhoneCallHeaderUtil::GetRemoteInfoDataL() - call id =%d ", aCallId);

    if ( aCallId == KEmergencyCallId )
        {
        // Set emergency label text
        LoadResource( aData, EPhoneEmergencyCallHeader );
        }
    else
        {
        // Note next if-statements are in priority order so be careful if you change order
        // or add new if-statements.
        if ( m_engineInfo.RemoteName( aCallId ).Length() )
            {
            // Display the contact name if it is available
            aData.Copy( m_engineInfo.RemoteName( aCallId ) );
            }
        else if ( m_engineInfo.RemotePartyName( aCallId ).Length() )
            {
            // Display the CNAP or UUS info if it is available.
            aData.Copy( m_engineInfo.RemotePartyName( aCallId ) );
            }
        else if ( m_engineInfo.RemoteCompanyName( aCallId ).Length() )
            {
            // Display the company name if it is available
            aData.Copy( m_engineInfo.RemoteCompanyName( aCallId ) );
            }
        else if ( m_engineInfo.CallDirection( aCallId ) == RMobileCall::EMobileTerminated )
            {
            const RMobileCall::TMobileCallRemoteIdentityStatus identity = 
                    m_engineInfo.RemoteIdentity( aCallId );
            
            if ( EPEPrivateNumber == m_engineInfo.RemotePhoneNumberType( aCallId ) )
                {
                if ( EPECallTypeVoIP == m_engineInfo.CallType( aCallId ) )
                    {
                    // TODO voip LoadResource( aData, iManagerUtility.Customization()->CustomizeCallHeaderText() );
                    }
                else
                    {
                    // private number
                    LoadResource( aData, EPhoneCLIWithheld );
                    }
                }
            else if ( identity == RMobileCall::ERemoteIdentityUnavailableNoCliCoinOrPayphone || 
                    identity == RMobileCall::ERemoteIdentityAvailableNoCliCoinOrPayphone )
                {
                // Display "Payphone".
                LoadResource( aData, EPhoneCLIPayphone );
                }
            else if ( identity == RMobileCall::ERemoteIdentityUnknown )
                {
                __PHONELOG( EBasic, EPhoneControl, "GetRemoteInfoDataL : unknown number" );
                // Display "Unknown Number".
                LoadResource( aData, EPhoneCallCLIUnknown );
                }
            else if ( m_engineInfo.RemotePhoneNumber( aCallId ).Length() )
                {
                // Display the number if it is available
                aData.Copy( m_engineInfo.RemotePhoneNumber( aCallId ) );
                }            
            }
        }
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::IsSecondaryCliAllowed
// -----------------------------------------------------------
//
TBool PhoneCallHeaderUtil::IsSecondaryCliAllowed( TInt aCallId )
    {
    TBool ret( ( aCallId != KEmergencyCallId && 
               ( m_engineInfo.RemoteName( aCallId ).Length() ||
                 m_engineInfo.RemotePartyName( aCallId ).Length() || 
                 m_engineInfo.RemoteCompanyName( aCallId ).Length() ) &&
                 m_engineInfo.RemotePhoneNumber( aCallId ).Length() ) );
    
    if ( ret && RMobileCall::EMobileTerminated == 
            m_engineInfo.CallDirection( aCallId ) )
        {
        const RMobileCall::TMobileCallRemoteIdentityStatus identity = 
                m_engineInfo.RemoteIdentity( aCallId );
        
        if ( m_engineInfo.RemotePhoneNumberType( aCallId ) ==  EPEPrivateNumber || 
            identity == RMobileCall::ERemoteIdentityUnavailableNoCliCoinOrPayphone ||
            identity == RMobileCall::ERemoteIdentityAvailableNoCliCoinOrPayphone ||
            identity == RMobileCall::ERemoteIdentityUnknown )
            {
            ret = EFalse;
            }
        }
    
    return ret;
    }

// -----------------------------------------------------------
// PhoneCallHeaderUtil::IsWaitingCall
// -----------------------------------------------------------
//
TBool PhoneCallHeaderUtil::IsWaitingCall(int callId) const
    {
    bool waiting( EFalse );
    if ( callId >= 0 && 
         m_engineInfo.CallState( callId ) == EPEStateRinging )
        {
        if ( m_engineInfo.CheckIfCallStateExists(EPEStateConnected) ||
             m_engineInfo.CheckIfCallStateExists(EPEStateConnectedConference) ||
             m_engineInfo.CheckIfCallStateExists(EPEStateDisconnecting) ||
             m_engineInfo.CheckIfCallStateExists(EPEStateHeld) ||
             m_engineInfo.CheckIfCallStateExists(EPEStateHeldConference) )
            {
            waiting = ETrue;
            }
        }
    return waiting;
    }

// ---------------------------------------------------------------------------
// PhoneCallHeaderUtil::LoadResource
// ---------------------------------------------------------------------------
//
HBufC* PhoneCallHeaderUtil::LoadResource( const TInt aResource ) const
    {
    HBufC* ret = NULL;
    
    TRAP_IGNORE( ret = StringLoader::LoadL(  
            CPhoneMainResourceResolver::Instance()->ResolveResourceID( aResource ), 
            CCoeEnv::Static() ) );
    
    return ret;
    }

// ---------------------------------------------------------------------------
// PhoneCallHeaderUtil::LoadResource
// ---------------------------------------------------------------------------
//
void PhoneCallHeaderUtil::LoadResource( TDes& aData, const TInt aResource ) const
    {
    StringLoader::Load(  
            aData, CPhoneMainResourceResolver::Instance()->ResolveResourceID( aResource ), 
            CCoeEnv::Static() );
    }


