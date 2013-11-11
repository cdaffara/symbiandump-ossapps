/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Audio handler implementation.
*
*/


// INCLUDE FILES
#include    "CVtEngAudioHandler.h"
#include    "CVtEngSettings.h"
#include    "CVtEngEventManager.h"
#include    "VtEngUtils.h"
#include    <cvtlogger.h>
#include    <featmgr.h>
#include    <mmf/server/sounddevice.h>
#include    <AudioPreference.h>
#include    "VtEngUtils.h"
#include    "CVtEngStateManager.h"
#include    "MVtEngSessionInfo.h"
#include    "CVtEngHandlerContainer.h"

// CONSTANTS

// For DevSound initialization
//const TInt KVtEngUidControlHwDevice = 0x10206593;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::CVtEngAudioHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngAudioHandler::CVtEngAudioHandler()
    {
    __VTPRINT( DEBUG_CONSTRUCT | DEBUG_AUDIO, "AH.c++" )
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngAudioHandler::ConstructL()
    {
    __VTPRINT( DEBUG_CONSTRUCT | DEBUG_AUDIO, "AH.ConstructL<" )

    iAccHandler = &CVtEngUtility::AccessoryHandler();
    iAccHandler->RegisterObserverL( this );
    iTelephonyAudioRouting = CTelephonyAudioRouting::NewL( *this );
    iCurrentAudioOutput = iTelephonyAudioRouting->Output();

    // Fetch the mode.
    User::LeaveIfError( iAccHandler->AccessoryMode( iAccMode ) );
    __VTPRINT2( DEBUG_AUDIO, "AH.iAccMode %d", ( TInt )iAccMode.iAccessoryMode )

    UpdateCurrentVolume();
    __VTPRINT( DEBUG_CONSTRUCT | DEBUG_AUDIO, "AH.ConstructL>")
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngAudioHandler* CVtEngAudioHandler::NewL()
    {
    CVtEngAudioHandler* self = new( ELeave ) CVtEngAudioHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::~CVtEngAudioHandler
// Destructor.
// -----------------------------------------------------------------------------
//
CVtEngAudioHandler::~CVtEngAudioHandler()
    {
    __VTPRINT( DEBUG_AUDIO, "AH.~<" )
    delete iRoutingEnabler;

    delete iTelephonyAudioRouting;
    if ( iAccHandler )
    	{
    	iAccHandler->RemoveObserver( this );
    	}
    __VTPRINT( DEBUG_AUDIO, "AH.~>" )
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::HandleL
// -----------------------------------------------------------------------------
//
void CVtEngAudioHandler::HandleL( CVtEngOperation& aOperation )
    {
    __VTPRINT2( DEBUG_AUDIO, "AH.HandleL op=%d", ( TInt )iOperation )
    if ( iOperation )
        {
        // While operation is ongoing, another call should not be made.
        User::Leave( KErrArgument );
        }

    MVtEngAudio::TVtEngRoutingSetting routeSetting;
    TVtEngOpParamUtil<MVtEngAudio::TVtEngRoutingSetting>::Set(
        routeSetting, aOperation );

    switch( routeSetting )
        {
        case EActivateHandsfree:
            SetLoudspeakerL( ETrue, ETrue );
            break;

        case EDeactivateHansfree:
            SetLoudspeakerL( EFalse, ETrue );
            break;

        case EActivateBT:
            SetBluetoothL( ETrue ); // asynchronous
            break;

        case EDeactivateBT:
            SetBluetoothL( EFalse );
            break;
        }

    iOperation = &aOperation;
    __VTPRINT( DEBUG_AUDIO | DEBUG_RETURN, "AH.HandleL" )
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::SetRoutingEnablerL
// -----------------------------------------------------------------------------
//
void CVtEngAudioHandler::SetRoutingEnablerL( const TBool aEnable )
    {
    __VTPRINTENTER( "AHRout.SetRoutingEnablerL" )

    if ( aEnable && !iRoutingEnabler )
        {
        // Before connected state audio routing can be changed by the user
        // only in MO calls. Create temporary devsound only if call is such.
        const CVtEngHandlerContainer& handlers =
            CVtEngUtility::StateManager()->Handlers();
        const MVtEngSessionInfo& session = handlers.Session();
        MVtEngSessionInfo::TDirection direction =
            MVtEngSessionInfo::EDirectionNone; // init to MO, if
        const TInt err( session.GetDirection( direction ) );
        if ( err == KErrNone && direction == MVtEngSessionInfo::EDirectionMO )
            {
            iRoutingEnabler = CRoutingEnabler::NewL();
            }
        }
    else if ( !aEnable )
        {
        delete iRoutingEnabler;
        iRoutingEnabler = NULL;
        }
    __VTPRINTEXIT( "AHRout.ReleaseRoutingEnablerL" )
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::IncreaseVolume
// -----------------------------------------------------------------------------
//
TBool CVtEngAudioHandler::IncreaseVolume()
    {
    __VTPRINTENTER( "AH.IncreaseVolume" )
    TBool adjustOk( AdjustVolume( 1 ) );
    if ( !adjustOk )
        {
        // only reason for AdjustVolume to fail is that volume is max already
        CVtEngEventManager::NotifyEvent( KVtEngAudioVolumeMax );
        }
    __VTPRINTEXITR( "AH.IncreaseVolume %d", adjustOk )
    return adjustOk;
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::DecreaseVolume
// -----------------------------------------------------------------------------
//
TBool CVtEngAudioHandler::DecreaseVolume()
    {
    __VTPRINTENTER( "AH.DecreaseVolume" )
    TBool adjustOk( AdjustVolume( -1 ) );
    if ( !adjustOk )
        {
        // only reason for AdjustVolume to fail is that volume is min already
        CVtEngEventManager::NotifyEvent( KVtEngAudioVolumeMin );
        }
    __VTPRINTEXITR( "AH.DecreaseVolume %d", adjustOk )
    return adjustOk;
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::AdjustVolume
// -----------------------------------------------------------------------------
//
TBool CVtEngAudioHandler::AdjustVolume( const TInt aDelta )
    {
    __VTPRINTENTER( "AH.AdjustVolume" )
    __VTPRINT2( DEBUG_AUDIO, "AH.AdjustVolume aDelta = %d", aDelta )
    TBool result( EFalse );
    CVtEngSettings& settings( CVtEngUtility::Settings() );
    TVtEngOutputVolume volume;
    settings.GetVolume( volume.iHandsetVolume, EFalse, EFalse );
    settings.GetVolume( volume.iHandsfreeVolume, ETrue, EFalse );
    TAudioRoutingState routingState;
    GetRoutingState( routingState );
    TInt& value( ( routingState == EAudioLoudspeaker ) ?
        volume.iHandsfreeVolume : volume.iHandsetVolume );
    value += aDelta;
    TInt validValue( settings.ValidVolume( value ) );
    if ( validValue == value )
        {
        settings.SetVolume(
            volume.iHandsetVolume,
            volume.iHandsfreeVolume,
            ETrue );
        settings.SetVolume(
            volume.iHandsetVolume,
            volume.iHandsfreeVolume,
            EFalse );
        result = ETrue;
        }
    __VTPRINTEXITR( "AH.AdjustVolume %d", result )
    return result;
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::GetRoutingState
// -----------------------------------------------------------------------------
//
TInt CVtEngAudioHandler::GetRoutingState(
        TAudioRoutingState& aAudioRoutingState )
    {
    switch ( iCurrentAudioOutput )
        {
        // Wired headset has the highest priority.
        case CTelephonyAudioRouting::EWiredAudioAccessory:
            __VTPRINT( DEBUG_CONSTRUCT | DEBUG_AUDIO, "AH.GRS.wired" )
            aAudioRoutingState = EAudioWiredHeadset;
            break;

        // Then bluetooth handsfree.
        case CTelephonyAudioRouting::EBTAudioAccessory:
            __VTPRINT( DEBUG_CONSTRUCT | DEBUG_AUDIO, "AH.GRS.bt" )
            aAudioRoutingState = EAudioBT;
            break;

        // Loudspeaker.
        case CTelephonyAudioRouting::ELoudspeaker:
            __VTPRINT( DEBUG_CONSTRUCT | DEBUG_AUDIO, "AH.GRS.IHF" )
            aAudioRoutingState = EAudioLoudspeaker;
            break;

        // Handset as fallback.
        default:
            __VTPRINT( DEBUG_CONSTRUCT | DEBUG_AUDIO, "AH.GRS.Handset" )
            aAudioRoutingState = EAudioHandset;
            break;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::GetRoutingAvailability
// -----------------------------------------------------------------------------
//
TInt CVtEngAudioHandler::GetRoutingAvailability(
        const TAudioRoutingState aAudioRoutingState,
        TBool& aAvailable )
    {
    TAudioRoutingState currentRouting;
    GetRoutingState( currentRouting ); // always succeeds

    if ( currentRouting == aAudioRoutingState )
        {
        // It is possible to route to itself, although it does not make
        // much sense.
        aAvailable = ETrue;
        }
    else if ( currentRouting == EAudioWiredHeadset )
        {
        // For wired headset, audio routings are not possible.
        aAvailable = EFalse;
        }
    else
        {
        switch ( aAudioRoutingState )
            {
            case EAudioHandset:
            case EAudioLoudspeaker:
                // Handset & loudspeaker are always available.
                aAvailable = ETrue;
                break;

            case EAudioBT:
                aAvailable = IsOutputAvailable( CTelephonyAudioRouting::EBTAudioAccessory );
                break;

            case EAudioWiredHeadset:
                // Wired headset can not be routed to. When headset is
                // attached, routing happens automatically.
                aAvailable = EFalse;
                break;
            }
        }

    __VTPRINT3( DEBUG_AUDIO, "AH.GRA state %d availability=%d",
        aAudioRoutingState, aAvailable )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::GetHeadsetType
// -----------------------------------------------------------------------------
//
TInt CVtEngAudioHandler::GetHeadsetType(
        TWiredHeadsetType& aHeadsetType )
    {
    switch ( iAccMode.iAccessoryMode )
        {
        case EAccModeWiredHeadset:
        case EAccModeWiredCarKit:
        case EAccModeMusicStand:
            aHeadsetType = EWiredHeadset;
            break;

        case EAccModeLoopset:
            aHeadsetType = EWiredLoopset;
            break;

        case EAccModeTextDevice:
            aHeadsetType = EWiredTty;
            break;

        case EAccModeHandPortable:
        case EAccModeWirelessHeadset:
        case EAccModeWirelessCarKit:
        default:
            // Not wired headset.
            aHeadsetType = EWiredNone;
            break;
        }

    __VTPRINT2( DEBUG_AUDIO, "AH.HeadsetType=%d", aHeadsetType )
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::OutputVolume
// -----------------------------------------------------------------------------
//
TInt CVtEngAudioHandler::OutputVolume(
        const TBool aHandsetVolume ) const
    {
    __VTPRINT2( DEBUG_AUDIO, "AH.OutputVolume isHandset=%d", aHandsetVolume )
    TInt vol = 0;
    CVtEngSettings& setting = CVtEngUtility::Settings();
    TInt err( setting.GetVolume( vol, !aHandsetVolume, EFalse ) );
    if ( err != KErrNone )
        {
        vol = err;
        }
    __VTPRINT2( DEBUG_AUDIO, "AH.OutputVolume vol=%d",vol )
    return vol;
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::SetLoudspeakerL
// -----------------------------------------------------------------------------
//
void CVtEngAudioHandler::SetLoudspeakerL( TBool aTurnOn, TBool /*aShowNote*/ )
    {
    __VTPRINTENTER(	"AH.SetLoudspeakerL" )
    __VTPRINT2( DEBUG_AUDIO, "AH.SetLoudspeakerL %d", aTurnOn )
    TBool available( EFalse );
    GetRoutingAvailability( EAudioLoudspeaker, available );
    if ( !available )
        {
        User::Leave( KErrNotSupported );
        }
    // Set shownote flag up in telephony audiorouting, before calling SetOutputL.
    User::LeaveIfError( iTelephonyAudioRouting->SetShowNote( ETrue ) );

    if( aTurnOn )
        {
        iTelephonyAudioRouting->SetOutputL( CTelephonyAudioRouting::ELoudspeaker );
        }
    else
        {
        iTelephonyAudioRouting->SetOutputL( CTelephonyAudioRouting::EHandset );
        }
    __VTPRINTEXIT( "AH.SetLoudspeakerL" )
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::SetBluetoothL
// -----------------------------------------------------------------------------
//
void CVtEngAudioHandler::SetBluetoothL( TBool aTurnOn )
    {
    __VTPRINTENTER( "AH.SetBluetoothL" )
    TBool available( EFalse );
    GetRoutingAvailability( EAudioBT, available );
    if ( !available )
        {
        User::Leave( KErrNotSupported );
        }
    // Set shownote flag up in telephony audiorouting, before calling SetOutputL.
    User::LeaveIfError( iTelephonyAudioRouting->SetShowNote( ETrue ) );

    if( aTurnOn )
        {
        iTelephonyAudioRouting->SetOutputL( CTelephonyAudioRouting::EBTAudioAccessory );
        }
    else
        {
        iTelephonyAudioRouting->SetOutputL( CTelephonyAudioRouting::EHandset );
        }
    __VTPRINTEXIT( "AH.SetBluetoothL" )
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::AvailableOutputsChanged
// -----------------------------------------------------------------------------
//
void CVtEngAudioHandler::AvailableOutputsChanged(
    CTelephonyAudioRouting& /*aTelephonyAudioRouting*/ )
    {
    __VTPRINTENTER( "AH.AvailableOutputsChanged" )
    CVtEngEventManager::NotifyEvent( KVtEngAudioRoutingAvailabilityChanged );
    __VTPRINTEXIT( "AH.AvailableOutputsChanged" )
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::OutputChanged
// -----------------------------------------------------------------------------
//
void CVtEngAudioHandler::OutputChanged(
    CTelephonyAudioRouting& aTelephonyAudioRouting )
    {
    __VTPRINTENTER( "AH.OutputChanged" )
    CTelephonyAudioRouting::TAudioOutput
        previousAudioOutput( iCurrentAudioOutput );
    iCurrentAudioOutput = aTelephonyAudioRouting.Output();
    CVtEngEventManager::NotifyEvent( KVtEngAudioRoutingChanged );
    // HandSet -> IHF
    if( ( previousAudioOutput == CTelephonyAudioRouting::EHandset ) &&
        ( iCurrentAudioOutput == CTelephonyAudioRouting::ELoudspeaker ) )
        {
        __VTPRINT( DEBUG_AUDIO, "AH.OutputChanged signalling HandSet to IHF" )
        }
    // IHF -> HandSet
    else if( ( previousAudioOutput == CTelephonyAudioRouting::ELoudspeaker ) &&
        ( iCurrentAudioOutput == CTelephonyAudioRouting::EHandset ) )
        {
        __VTPRINT( DEBUG_AUDIO, "AH.OutputChanged signalling IHF to HandSet" )
        }
    UpdateCurrentVolume();
    __VTPRINTEXITR( "AH.OutputChanged %d", iCurrentAudioOutput )
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::SetOutputComplete
// -----------------------------------------------------------------------------
//
void CVtEngAudioHandler::SetOutputComplete(
    CTelephonyAudioRouting& aTelephonyAudioRouting,
    TInt aError )
    {
    __VTPRINTENTER( "AH.SetOutputComplete" )
    if ( aError == KErrNone )
        {
        OutputChanged( aTelephonyAudioRouting );
        }
    CompleteOperation( aError );
    __VTPRINTEXIT( "AH.SetOutputComplete" )
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::CompleteOperation
// -----------------------------------------------------------------------------
//
void CVtEngAudioHandler::CompleteOperation( TInt aResult )
    {
    __VTPRINT3( DEBUG_AUDIO, "AH.CompleteOperation op=%d,res=%d" ,
        ( TInt )iOperation, aResult )
    if ( iOperation )
        {
        iOperation->HandleOpComplete( aResult );
        iOperation = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::UpdateCurrentVolume
// -----------------------------------------------------------------------------
//
void CVtEngAudioHandler::UpdateCurrentVolume()
    {
    __VTPRINTENTER( "AH.UpdateCurrentVolume" )
    TAudioRoutingState currentRouting = EAudioHandset;
    GetRoutingState( currentRouting );

    const TBool currentVolume = ( currentRouting == EAudioLoudspeaker );
    CVtEngSettings& setting = CVtEngUtility::Settings();
    setting.SetCurrentVolume( currentVolume );

    __VTPRINTEXITR( "AH.UpdateCurrentVolume output is IHF=%d", currentVolume )
    }

// -----------------------------------------------------------------------------
// CVtEngAudioHandler::AccessoryModeChanged
// -----------------------------------------------------------------------------
//
void CVtEngAudioHandler::AccessoryModeChanged(
    const TAccMode& aMode, const TBool /* aActivated */ )
    {
    iAccMode.iAccessoryMode = aMode;
    }


// -----------------------------------------------------------------------------
// CVtEngAudioHandler::IsOutputAvailable
// -----------------------------------------------------------------------------
//
TBool CVtEngAudioHandler::IsOutputAvailable(
    const CTelephonyAudioRouting::TAudioOutput aOutput )
    {
    const TArray< CTelephonyAudioRouting::TAudioOutput > outputs =
        iTelephonyAudioRouting->AvailableOutputs();

    for ( TInt i = 0; i < outputs.Count(); i++ )
        {
        if ( outputs[ i ] == aOutput )
            {
            return ETrue;
            }
        }
    return EFalse;
    }
// -----------------------------------------------------------------------------
// CRoutingEnabler::NewL
// -----------------------------------------------------------------------------
//
CRoutingEnabler* CRoutingEnabler::NewL()
    {
    CRoutingEnabler* enabler =
        new ( ELeave ) CRoutingEnabler();
    CleanupStack::PushL( enabler );
    enabler->StartL();
    CleanupStack::Pop( enabler );
    return enabler;
    }

// Destructor
CRoutingEnabler::~CRoutingEnabler()
    {
    if ( iDevSound )
    	{
        // If initialization ahs been successfull, Devsound Play has started
        // and need to be stopped.
        if( !iDevsoundInitializeFailure )
            {
            __VTPRINT( DEBUG_MEDIA, "AHRout.~ Stopping audio" )
    	    iDevSound->Stop();
            }
        delete iDevSound;
    	iDevSound = NULL;
        }
    }

// c++ constructor
CRoutingEnabler::CRoutingEnabler()
    {
    }

// -----------------------------------------------------------------------------
// CRoutingEnabler::StartL
// -----------------------------------------------------------------------------
//
void CRoutingEnabler::StartL()
    {
    __VTPRINTENTER( "AHRout.StartL" )
    iDevSound = CMMFDevSound::NewL();

    iDevSound->InitializeL(
        *( this ),
        EMMFStatePlaying );
    __VTPRINTEXIT( "AHRout.StartL" )
    }

// -----------------------------------------------------------------------------
// CRoutingEnabler::InitializeComplete
// -----------------------------------------------------------------------------
//
void CRoutingEnabler::InitializeComplete( TInt aError )
    {
    __VTPRINTENTER( "AHRout.InitializeComplete" )
    if ( aError == KErrNone )
        {
        TMMFPrioritySettings prioritySettings;
        prioritySettings.iPriority = KAudioPriorityCSCallDownlink;
        prioritySettings.iPref =
            ( TMdaPriorityPreference ) KAudioPrefCSCallDownlink;
        prioritySettings.iState = EMMFStatePlaying;
        if ( iDevSound )
            {
            iDevSound->SetPrioritySettings( prioritySettings );

            TRAPD( err, iDevSound->PlayInitL() );
            if ( err != KErrNone )
            	{
            	__VTPRINT2( DEBUG_AUDIO, "AHRout.InitializeComplete PlayInitL err=%d", err )

                // Devsound PlayInitL leave error happened.
                iDevsoundInitializeFailure = ETrue;
            	}
            }
        }
    else
        {
        __VTPRINT2( DEBUG_AUDIO, "AHRout.InitializeComplete err=%d", aError )

        // Devsound initialization failed
        iDevsoundInitializeFailure = ETrue;
        }
    __VTPRINTEXIT( "AHRout.InitializeComplete" )
    }

// -----------------------------------------------------------------------------
// CRoutingEnabler::ToneFinished
// -----------------------------------------------------------------------------
//
void CRoutingEnabler::ToneFinished( TInt /*aError*/)
    {
    }

// -----------------------------------------------------------------------------
// CRoutingEnabler::BufferToBeFilled
// -----------------------------------------------------------------------------
//
void CRoutingEnabler::BufferToBeFilled(
    CMMFBuffer* /*aBuffer*/)
    {
    }

// -----------------------------------------------------------------------------
// CRoutingEnabler::PlayError
// -----------------------------------------------------------------------------
//
void CRoutingEnabler::PlayError( TInt /*aError*/)
    {
    }

// -----------------------------------------------------------------------------
// CRoutingEnabler::BufferToBeEmptied
// -----------------------------------------------------------------------------
//
void CRoutingEnabler::BufferToBeEmptied(
    CMMFBuffer* /*aBuffer*/ )
    {
    }

// -----------------------------------------------------------------------------
// CRoutingEnabler::RecordError
// -----------------------------------------------------------------------------
//
void CRoutingEnabler::RecordError(
    TInt /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// CRoutingEnabler::ConvertError
// -----------------------------------------------------------------------------
//
void CRoutingEnabler::ConvertError( TInt /*aError*/ )
    {
    }

// -----------------------------------------------------------------------------
// CRoutingEnabler::DeviceMessage
// -----------------------------------------------------------------------------
//
void CRoutingEnabler::DeviceMessage(
    TUid /*aMessageType*/, const TDesC8& /*aMsg*/ )
    {
    }

//  End of File
