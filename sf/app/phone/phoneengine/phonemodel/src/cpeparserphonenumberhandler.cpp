/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CPEParserPhoneNumberHandler class methods implementation
*
*/



// INCLUDE FILES
#include "cpeclientcallrequestmonitor.h"
#include "cpemessagehandler.h"
#include "cpeparserphonenumberhandler.h"
#include <featmgr.h>
#include <gsmerror.h>
#include <mpecallhandling.h>
#include <mpedatastore.h>
#include <talogger.h>

// CONSTANTS
_LIT( KClirSuppress, "*31#" );
_LIT( KClirInvoke, "#31#" );

// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CPEParserPhoneNumberHandler::CPEParserPhoneNumberHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEParserPhoneNumberHandler::CPEParserPhoneNumberHandler( 
        CPEMessageHandler& aOwner, 
        MPECallHandling& aCallHandling,
        MPEDataStore& aDataStore
        ) : iOwner( aOwner ), 
            iCallHandling( aCallHandling ),
            iDataStore( aDataStore )
    {
    TEFLOGSTRING( KTAOBJECT, "PE CPEParserPhoneNumberHandler::CPEParserPhoneNumberHandler" )
    }

// -----------------------------------------------------------------------------
// CPEParserPhoneNumberHandler::ProcessDialToNumberL
// Process dial to number.
// -----------------------------------------------------------------------------
//
void CPEParserPhoneNumberHandler::ProcessDialToNumberL( 
        const TDesC& aNumber,
        const TDesC& aDtmfPostfix,
        TClir aClir )
    {
    TInt errorCode( ECCPErrorNotFound );
    TPEPhoneNumber phoneNumber;
    TInt callId;
        
    TEFLOGSTRING3( KTAINT, 
        "PE CPEParserPhoneNumberHandler::ProcessDialToNumberL, number: %S, dtmf postfix: %S", 
        &aNumber, 
        &aDtmfPostfix );
    
    // Check if phone is locked
    iOwner.CheckIfPhoneIsLockedL();
    
    if ( aNumber.Length() == 0 )
        {
        User::Leave( ECCPErrorInvalidPhoneNumber );
        }

    if ( aClir == EClirSuppress )
        {
        phoneNumber.Copy( KClirSuppress );
        }
    else if ( aClir == EClirInvoke )
        {
        phoneNumber.Copy( KClirInvoke );
        }
    
    phoneNumber.Append( aNumber );
    
    TPECallType processType = iDataStore.CallTypeCommand();
    if( processType != EPECallTypeVideo )
        {
        // TSY not accept phone number that include + or w chartes.
        TPEPhoneNumber postfix = FilterPostfixL( aDtmfPostfix );
        if( postfix.Length() )
            {
            phoneNumber.Append( postfix );
            }
        }
    TEFLOGSTRING2( KTAINT, 
          "PE CPEParserPhoneNumberHandler::ProcessDialToNumberL, phoneNumber: %S"
            , &phoneNumber );
    //Check for Offline mode
    if ( IsPhoneOffline() )
        {
        //Offline mode is active or network registration failed,
        //only emergency calls allowed
        TEFLOGSTRING( KTAERROR, 
            "PE CPEGSMPARSERPHONENUMBERHANDLER::PROCESSDIALTONUMBERL: CURRENTLY IN OFFLINE MODE: EMERGENCY CALLS ONLY! " );
        errorCode = ECCPErrorNotAllowedInOfflineMode;
        }
    else
        {
        switch ( processType )
            { 
            case EPECallTypeUninitialized: // There is no information about call, assume voice call
            case EPECallTypeCSVoice: //Make Dial request
            case EPECallTypeVoIP:
                {

                TBool clientCall = iDataStore.CallOriginCommand() != EPECallOriginPhone;
                errorCode = iCallHandling.DialCall( phoneNumber, callId );
                //PhoneClient originated call
                if( clientCall && ( ECCPErrorNone == errorCode ) )
                    {
                    iOwner.SetClientInformation( callId, phoneNumber );
                    }
                break;
                }
            case EPECallTypeVideo:
                {

                TBool clientCall = iDataStore.CallOriginCommand() != EPECallOriginPhone;
                errorCode = iCallHandling.DialMultimedia( phoneNumber, callId );
                // Note that video call dialing does not return an error code if it fails
                // due to insufficient free memory, so we have to check the call identifier as well.
                if( clientCall
                        && callId != KPECallIdNotUsed
                        && ( ECCPErrorNone == errorCode ) )
                    {
                    iOwner.SetClientInformation( callId, phoneNumber );
                    }
                break;
                }
            default:
                {
                errorCode = ECCPErrorNotFound;
                break;
                }
            } 
        if ( !errorCode )
            {
            //Set Dtmf string to dataStore
            iDataStore.SetDtmfPostFix( aDtmfPostfix, callId );
            }
        }
    iOwner.SetGsmParserErrorCode( errorCode );

    // KErrInUse is returned when we are not allowed to make the call.
    // One of these cases would be where a MO call is alerting and the user
    // tries to dial another number via Dialer
    }

// -----------------------------------------------------------------------------
// CPEParserPhoneNumberHandler::FilterPostfix
// -----------------------------------------------------------------------------
// 
TPtrC CPEParserPhoneNumberHandler::FilterPostfixL( TPtrC aPostfix )
    {
    TLex input( aPostfix );
    TInt stripStart = KErrNotFound;
    TInt postfixLength = aPostfix.Length();
    for ( TInt i = 0; i != postfixLength; i ++ )
        {
        TChar ch( input.Peek() );
        if (ch == KDtmfPlus)
            {
            User::Leave ( ECCPErrorInvalidPhoneNumber );
            }
        else if ( ch == KDtmfWait )
            {
            if ( i < stripStart || stripStart == KErrNotFound )
                {
                stripStart = i;
                }
            }
        input.Inc();
        }
    if ( stripStart != KErrNotFound )
        {
        return aPostfix.Left( stripStart );
        }
    else
        {
        return aPostfix;
        }
    }

// -----------------------------------------------------------------------------
// CPEParserPhoneNumberHandler::IsPhoneOffline
// -----------------------------------------------------------------------------
// 
TBool CPEParserPhoneNumberHandler::IsPhoneOffline()
    {
    TBool isPhoneOffline( EFalse );

    if( FeatureManager::FeatureSupported( KFeatureIdOfflineMode )
        && ( EFalse == iOwner.IsNetworkConnectionAllowed() ) )
        {
        isPhoneOffline = ETrue;
        }
    
    TNWNetworkRegistrationStatus networkRegStatus
        = iDataStore.NetworkRegistrationStatus();
    
    if( ( networkRegStatus == ENWStatusRegistrationUnknown )
        || ( networkRegStatus == ENWStatusNotRegisteredNoService )
        || ( networkRegStatus == ENWStatusNotRegisteredEmergencyOnly )
        || ( networkRegStatus == ENWStatusRegistrationDenied ) )
        {
        isPhoneOffline = ETrue;
        }

    return isPhoneOffline;
    }
    
//  End of File
