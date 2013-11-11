/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for controlling S60 device lights.
*
*/


// INCLUDE FILES
#include <HtiDispatcherInterface.h>
#include <HtiLogging.h>
#include "HtiLightsController.h"

// CONSTANTS
const static TUid KSysInfoServiceUid = { 0x10210CC7 };

const static TInt KLightStatusCmdLength = 2;
const static TInt KLightOnCmdLength     = 6;
const static TInt KLightOffCmdLength    = 5;
const static TInt KLightBlinkCmdLength  = 9;

enum TSysInfoLightControlCommand
    {
    ELightStatus = 0x30,
    ELightOn =     0x31,
    ELightOff =    0x32,
    ELightBlink =  0x33
    };

_LIT8( KErrDescrArgument,     "Invalid argument" );
_LIT8( KErrDescrLightOn,      "LightOn failed" );
_LIT8( KErrDescrLightOff,     "LightOff failed" );
_LIT8( KErrDescrLightBlink,   "LightBlink failed" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHtiLightsController::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHtiLightsController* CHtiLightsController::NewL( MHtiDispatcher* aDispatcher )
    {
    CHtiLightsController* self = new (ELeave) CHtiLightsController(
                                                  aDispatcher );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// -----------------------------------------------------------------------------
// CHtiLightsController::CHtiLightsController
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CHtiLightsController::CHtiLightsController(
        MHtiDispatcher* aDispatcher ):iDispatcher( aDispatcher ),
                                      iCommand( 0 ),
                                      iTarget( 0 ),
                                      iDuration( 0 ),
                                      iOnDuration( 0 ),
                                      iOffDuration( 0 ),
                                      iIntensity( 0 ),
                                      iFade( EFalse )
    {
    }


// -----------------------------------------------------------------------------
// CHtiLightsController::~CHtiLightsController()
// Destructor
// -----------------------------------------------------------------------------
//
CHtiLightsController::~CHtiLightsController()
    {
    HTI_LOG_TEXT("CHtiLightsController destroy");
    delete iLight;
    }

// Second phase construction
void CHtiLightsController::ConstructL()
    {
    HTI_LOG_TEXT("CHtiLightsController::ConstructL");
    iLight = CHWRMLight::NewL( this );
    }


// -----------------------------------------------------------------------------
// CHtiLightsController::ProcessMessageL
// Called by the plugin when there is a message to be processed by
// the lights controller.
// -----------------------------------------------------------------------------
//
void CHtiLightsController::ProcessMessageL( const TDesC8& aMessage,
                                            TDes8& aReply )
    {
    HTI_LOG_FUNC_IN( "CHtiLightsController::ProcessMessageL" );

    iCommand = aMessage[0];
    iReply.Zero();

    switch ( iCommand )
        {
        case ELightStatus:
            {
            HTI_LOG_TEXT( "ELightStatus" );
            HandleLightStatusL( aMessage );
            break;
            }

        case ELightOn:
            {
            HTI_LOG_TEXT( "ELightOn" );
            HandleLightOnL( aMessage );
            break;
            }

        case ELightOff:
            {
            HTI_LOG_TEXT( "ELightOff" );
            HandleLightOffL( aMessage );
            break;
            }

        case ELightBlink:
            {
            HTI_LOG_TEXT( "ELightBlink" );
            HandleLightBlinkL( aMessage );
            break;
            }

        default:
            {
            // If comes here it's an error from caller.
            User::Leave( KErrArgument );
            }
        }

    aReply.Copy( iReply );

    HTI_LOG_FUNC_OUT("CHtiLightsController::ProcessMessageL");
    }


// -----------------------------------------------------------------------------
// CHtiLightsController::HandleLightStatusL
// Gets the status of the given light target.
// Returns "Not supported" for S60 2.x
// -----------------------------------------------------------------------------
//
void CHtiLightsController::HandleLightStatusL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiLightsController::HandleLightStatusL" );

    if ( aMessage.Length() != KLightStatusCmdLength )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument, KErrDescrArgument, KSysInfoServiceUid );
        }

    else
        {
        iTarget = aMessage[1];
        iReply.Append( iLight->LightStatus( iTarget ) );
        }
    HTI_LOG_FUNC_OUT( "CHtiLightsController::HandleLightStatusL" );
    }


// -----------------------------------------------------------------------------
// CHtiLightsController::HandleLightOnL
// Turns on light with specified parameters.
// For S60 2.x just turns on lights forever, parameters are ignored.
// -----------------------------------------------------------------------------
//
void CHtiLightsController::HandleLightOnL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiLightsController::HandleLightOnL" );

    if ( aMessage.Length() != KLightOnCmdLength )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument, KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

    // parse values from message
    iTarget    = aMessage[1];
    iDuration  = aMessage[2] + ( aMessage[3] << 8 );
    iIntensity = aMessage[4];
    iFade      = (TBool)aMessage[5];

    TInt err = KErrNone;

    // normalize possibly abnormal values
    if ( iIntensity < KHWRMLightMinIntensity )
        iIntensity = KHWRMDefaultIntensity;

    if ( iIntensity > KHWRMLightMaxIntensity )
        iIntensity = KHWRMLightMaxIntensity;

    if ( iDuration < 1 ) iDuration = KHWRMInfiniteDuration;

    // shoot
    TRAP( err, iLight->LightOnL( iTarget, iDuration, iIntensity, iFade ) );

    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err, KErrDescrLightOn, KSysInfoServiceUid );
        }

    else
        {
        iReply.Append( 0 );
        }

    HTI_LOG_FUNC_OUT( "CHtiLightsController::HandleLightOnL ");
    }


// -----------------------------------------------------------------------------
// CHtiLightsController::HandleLightOffL
// Turns off light with specified parameters.
// Returns "Not supported" for S60 2.x
// -----------------------------------------------------------------------------
//
void CHtiLightsController::HandleLightOffL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiLightsController::HandleLightOffL" );

    if ( aMessage.Length() != KLightOffCmdLength )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument, KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

    // parse values from message
    iTarget   = aMessage[1];
    iDuration = aMessage[2] + ( aMessage[3] << 8 );
    iFade     = (TBool)aMessage[4];

    // normalize possibly abnormal values
    if ( iDuration < 1 ) iDuration = KHWRMInfiniteDuration;

    // shoot
    TRAPD( err, iLight->LightOffL( iTarget, iDuration, iFade ) );

    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err, KErrDescrLightOff, KSysInfoServiceUid );
        }

    else
        {
        iReply.Append( 0 );
        }
    HTI_LOG_FUNC_OUT( "CHtiLightsController::HandleLightOffL" );
    }


// -----------------------------------------------------------------------------
// CHtiLightsController::HandleLightBlinkL
// Blinks light with specified parameters.
// Returns "Not supported" for S60 2.x
// -----------------------------------------------------------------------------
//
void CHtiLightsController::HandleLightBlinkL( const TDesC8& aMessage )
    {
    HTI_LOG_FUNC_IN( "CHtiLightsController::HandleLightBlinkL" );

    if ( aMessage.Length() != KLightBlinkCmdLength )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            KErrArgument, KErrDescrArgument, KSysInfoServiceUid );
        return;
        }

    // parse values from message
    iTarget      = aMessage[1];
    iDuration    = aMessage[2] + ( aMessage[3] << 8 );
    iOnDuration  = aMessage[4] + ( aMessage[5] << 8 );
    iOffDuration = aMessage[6] + ( aMessage[7] << 8 );
    iIntensity   = aMessage[8];

    // normalize possibly abnormal values
    if ( iIntensity < KHWRMLightMinIntensity )
        iIntensity = KHWRMDefaultIntensity;

    if ( iIntensity > KHWRMLightMaxIntensity )
        iIntensity = KHWRMLightMaxIntensity;

    if ( iDuration < 1 ) iDuration = KHWRMInfiniteDuration;

    if ( iOnDuration < 1 || iOffDuration < 1 )
        {
        iOnDuration = KHWRMDefaultCycleTime;
        iOffDuration = KHWRMDefaultCycleTime;
        }

    // shoot
    TRAPD( err, iLight->LightBlinkL(
            iTarget, iDuration, iOnDuration, iOffDuration, iIntensity ) );

    if ( err )
        {
        iDispatcher->DispatchOutgoingErrorMessage(
            err, KErrDescrLightBlink, KSysInfoServiceUid );
        }

    else
        {
        iReply.Append( 0 );
        }
    HTI_LOG_FUNC_OUT( "CHtiLightsController::HandleLightBlinkL" );
    }


// -----------------------------------------------------------------------------
// CHtiLightsController::LightStatusChanged
// Called when status of any light target changes.
// If infinite duration is requested, restores the state back to what was
// last requested.
// This method does not exist for S60 2.x
// -----------------------------------------------------------------------------
//
void CHtiLightsController::LightStatusChanged( TInt aTarget,
                                     CHWRMLight::TLightStatus aStatus )
    {
    HTI_LOG_FORMAT( "Light status changed for target %d", aTarget );
    HTI_LOG_FORMAT( "New status = %d", aStatus );
    HTI_LOG_FORMAT( "Current target = %d", iTarget );

    TInt target = aTarget & iTarget;
    if ( !target )
        {
        HTI_LOG_TEXT( "Not interested about the target" );
        return;
        }

    HTI_LOG_TEXT( "Matches current target" );

    if ( iDuration != KHWRMInfiniteDuration )
        {
        return;
        }

    if ( ( aStatus == CHWRMLight::ELightOn && iCommand == ELightOn ) ||
         ( aStatus == CHWRMLight::ELightOff && iCommand == ELightOff ) ||
         ( aStatus == CHWRMLight::ELightBlink && iCommand == ELightBlink ) )
        {
        HTI_LOG_TEXT( "Status already OK" );
        return;
        }

    HTI_LOG_TEXT( "Infinite duration wanted - restore light status" );
    switch ( iCommand )
        {
        case ELightOn:
            {
            // Ignore error
            TRAPD( err, iLight->LightOnL(
                    target, iDuration, iIntensity, iFade ) );
            HTI_LOG_FORMAT( "LightOnL return code %d", err );
            err = err; // to get rid of compiler warning for non-logging
            break;
            }
        case ELightOff:
            {
            // Ignore error
            TRAPD( err, iLight->LightOffL( target, iDuration, iFade ) );
            HTI_LOG_FORMAT( "LightOffL return code %d", err );
            err = err; // to get rid of compiler warning for non-logging
            break;
            }
        case ELightBlink:
            {
            // Ignore error
            TRAPD( err, iLight->LightBlinkL(
                    target, iDuration, iOnDuration,
                    iOffDuration, iIntensity ) );
            HTI_LOG_FORMAT( "LightBlinkL return code %d", err );
            err = err; // to get rid of compiler warning for non-logging
            break;
            }
        default:
            break;
        }
    }

// End of file
