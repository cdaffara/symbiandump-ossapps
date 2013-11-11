/*
 * Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: This module contains the implementation of AudioData class
 *              member functions
 *
 */

// INCLUDE FILES
#include "cpeaudiodata.h"

#include <AudioPreference.h>
#include <mpephonemodelinternal.h>
#include <mpedatastore.h>
#include <mpeexternaldatahandler.h>
#include <talogger.h>
#include <telmicmutestatuspskeys.h>
#include <PSVariables.h>
#include <tms.h>
#include <tmseffectobsrvr.h>
#include <tmsglobalvoleffect.h>
#include <telinformationpskeys.h>
#include "pepanic.pan"
#include "cpeaudiodtmftoneplayer.h"
#include "cpeaudioroutingmonitor.h"
#include "cpecallaudioroutinghandler.h"
#include "cpeaudiofactory.h"
#include "cpeaudioeffect.h"

#include <telinternalpskeys.h>

// CONSTANTS
//Mute Value for volume
const TInt KPEMaxVolume = 10;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEAudioData::CPEAudioData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEAudioData::CPEAudioData(MPEPhoneModelInternal& aPhoneModel) :
    iPhoneModel(aPhoneModel)
    {
    iRouteInitialized = EFalse;
    }

// -----------------------------------------------------------------------------
// CPEAudioData::ConstructL
// Symbian 2nd phase constructor can leave
// -----------------------------------------------------------------------------
//
void CPEAudioData::ConstructL(CPEAudioFactory& aAudioFactory)
    {
    TEFLOGSTRING( KTAOBJECT, "AUD CPEAudioData::ConstructL start" );
    iAudioRoutingMonitor = CPEAudioRoutingMonitor::NewL(*this);
    iAudioRouting = aAudioFactory.CreateTelephonyAudioRoutingL(
            *iAudioRoutingMonitor);
    iAudioRoutingHandler = CPECallAudioRoutingHandler::NewL(*this);
    iAudioEffect = CPEAudioEffect::NewL(*this);

    InitializeAudioVolume();

    TEFLOGSTRING( KTAINT, "AUD CPEAudioData::ConstructL  complete" );
    }

// Destructor
EXPORT_C CPEAudioData::~CPEAudioData()
    {
    TEFLOGSTRING( KTAOBJECT, "AUD CPEAudioData::~CPEAudioData" );

    delete iAudioRouting;
    delete iAudioRoutingMonitor;
    delete iAudioRoutingHandler;

    iPEavailableOutputs.Close();

    delete iAudioEffect;
    }

// -----------------------------------------------------------------------------
// CPEAudioData::InitializeAudioVolume
// Gets audio volume value
// -----------------------------------------------------------------------------
//
void CPEAudioData::InitializeAudioVolume()
    {
    TInt volume = iAudioEffect->Volume();
    iPhoneModel.DataStore()->SetAudioVolume(volume);

    TEFLOGSTRING2( KTAINT,
            "AUD CPEAudioData::InitializeAudioVolumes, volume = %d",
            volume );
    }

// -----------------------------------------------------------------------------
// CPEAudioData::GetAudioMuteSync
// Creates synchronous request to get Audio Mute value.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEAudioData::GetAudioMuteSync(TBool& aAudioMute) const
    {
    aAudioMute = iAudioEffect->MuteState();
    TEFLOGSTRING2(
            KTAREQIN,
            "AUD CPEAudioData::GetAudioMuteSync: value = %d",
            aAudioMute);

    }

// -----------------------------------------------------------------------------
// CPEAudioData::GetAudioVolumeSync
// Creates synchronous request to get audio volume.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEAudioData::GetAudioVolumeSync(TInt& aAudioVolume) const
    {
    TEFLOGSTRING( KTAINT, "AUD CPEAudioData::GetAudioVolumeSync" );
    aAudioVolume = iAudioEffect->Volume();
    }

// -----------------------------------------------------------------------------
// CPEAudioData::SendErrorMessage
// Saves errorcode to member variable and sends error notification to PhoneEngine.
// -----------------------------------------------------------------------------
//
void CPEAudioData::SendErrorMessage(const TInt aErrorCode)
    {
    iPhoneModel.DataStore()->SetErrorCode(aErrorCode);
    TEFLOGSTRING(
            KTAREQOUT,
            "AUD CPEAudioData::SendErrorMessage: iPhoneModel.SendMessage( MEngineMonitor::EPEMessageAudioHandlingError )" );
    iPhoneModel.SendMessage(MEngineMonitor::EPEMessageAudioHandlingError);
    }

// -----------------------------------------------------------------------------
// CPEAudioData::SendMessage
// Forwards message to PhoneEngine.
// -----------------------------------------------------------------------------
//
void CPEAudioData::SendMessage(
        const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage)
    {
    TEFLOGSTRING2( KTAREQIN,
            "AUD CPEAudioData::SendMessage: aMessage = %d", aMessage );

    if (aMessage == MEngineMonitor::EPEMessageAudioVolumeChanged)
        {
        // update volume
        TInt volume = iPhoneModel.DataStore()->AudioVolume();
        TEFLOGSTRING2( KTAINT,
                "AUD CPEAudioData::SendMessage: volume = %d",
                volume );
        }

    iPhoneModel.SendMessage(aMessage);
    }

// -----------------------------------------------------------------------------
// CPEAudioData::SendMessage
// Forwards message to PhoneEngine. Output mode is saved.
// -----------------------------------------------------------------------------
//
void CPEAudioData::SendMessage(
        const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
        const CTelephonyAudioRouting::TAudioOutput aOutput)
    {
    TEFLOGSTRING3(
            KTAREQIN,
            "AUD CPEAudioData::SendMessage: aMessage = %d, aOutput = %d",
            aMessage, aOutput );

    // BT long key press requires manual re-route to handset or loudspeaker
    if (aOutput == CTelephonyAudioRouting::ENone
            && iAudioRouting->PreviousOutput()
                    == CTelephonyAudioRouting::EBTAudioAccessory)
        {
        SetRoutePreference(ETrue);
        }
    else
        {
        // update the audio values in engineinfo
        TBool status;
        iAudioRouting->GetShowNote(status);
        iPhoneModel.DataStore()->SetAudioOutput(ConvertToPE(aOutput),
                ConvertToPE(PreviousOutput()), status);
        iPhoneModel.SendMessage(aMessage);
        }
    }

// -----------------------------------------------------------------------------
// CPEAudioData::SendMessage
// Forwards message to PhoneEngine.
// -----------------------------------------------------------------------------
//
void CPEAudioData::SendMessage(
        const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
        CTelephonyAudioRouting& aTelephonyAudioRouting)
    {
    TEFLOGSTRING2(
            KTAREQIN,
            "AUD CPEAudioData::SendMessage: aMessage = %d, available outputs changed ", aMessage );
    iPhoneModel.DataStore()->SetAvailableAudioOutputs(GetAvailableOutputs(
            aTelephonyAudioRouting));

    if (iRouteInitialized && aMessage
            == MEngineMonitor::EPEMessageAvailableAudioOutputsChanged)
        {
        iAudioRoutingHandler->HandleAudioRoutingAvailableChanged();
        }

    iPhoneModel.SendMessage(aMessage);
    }

// -----------------------------------------------------------------------------
// CPEAudioData::SetAudioMuteSync
// Creates synchronous request to set audio mute.
// EPEMessageAudioMuteChanged message is sent when gain is changed
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEAudioData::SetAudioMuteSync(const TBool aAudioMute) // Mute value to be set
    {
    TEFLOGSTRING( KTAINT, "AUD CPEAudioData::SetAudioMuteSync" );

    if (aAudioMute)
        {
        iPhoneModel.DataStore()->SetAudioMute(aAudioMute);
        SendMessage(MEngineMonitor::EPEMessageAudioMuteChanged);
        TInt err = iAudioEffect->SetMuteState(ETrue);
        TEFLOGSTRING2(
                KTAREQOUT,
                "AUD CPEAudioData::SetAudioMuteSync: error = %d",
                err );
        }
    else
        {
        iPhoneModel.DataStore()->SetAudioMute(aAudioMute);
        SendMessage(MEngineMonitor::EPEMessageAudioMuteChanged);
        TInt err = iAudioEffect->SetMuteState(EFalse);
        TEFLOGSTRING2(
                KTAREQOUT,
                "AUD CPEAudioData::SetAudioMuteSync: error = %d",
                err );
        }
    }

// -----------------------------------------------------------------------------
// CPEAudioData::SetAudioVolumeSync
// Creates synchronous request to set audio volume.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEAudioData::SetAudioVolumeSync(const TInt aAudioVolume) // Volume to be set
    {
    TEFLOGSTRING2(
            KTAREQIN,
            "AUD CPEAudioData::SetAudioVolumeSync, aAudioVolume = %d",
            aAudioVolume );

    if (aAudioVolume <= KPEMaxVolume)
        {
        TEFLOGSTRING2( KTAINT,
                "AUD CPEAudioData::SetAudioVolumeSync, aAudioVolume = %d",
                aAudioVolume );

        iAudioEffect->SetVolume(aAudioVolume);
        }
    DoHandleVolumeChange(aAudioVolume, ETrue);
    }

// -----------------------------------------------------------------------------
// CPEAudioData::SetAudioOutput
// Makes request to Telephony Audio Routing to set audio output path
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEAudioData::SetAudioOutput(const TPEAudioOutput aOutput,
        TBool aShowNote)
    {
    TEFLOGSTRING2(
            KTAREQIN,
            "AUD CPEAudioData::SetAudioOutput: aOutput = %d",
            aOutput );

    TInt error(KErrNone);

    TEFLOGSTRING2( KTAERROR,
            "AUD CPEAudioData::SetAudioOutput: iRouteInitialized (%d)"
            , iRouteInitialized );

    if (iRouteInitialized && iPhoneModel.DataStore()->AudioOutputAvailable(
            aOutput))
        {
        SetTAROutput(ConvertToTAR(aOutput), aShowNote);
        }
    else
        {
        if (aOutput == EPEHandset)
            {
            iAudioRoutingHandler->SetAnswerToHandset(ETrue);
            }
        else
            {
            TEFLOGSTRING2( KTAERROR,
                    "AUD CPEAudioData::SetAudioOutput: Requested audio path not available (%d)"
                    , aOutput );
            error = KErrArgument;
            }
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CPEAudioData::SetTAROutput
// -----------------------------------------------------------------------------
//
void CPEAudioData::SetTAROutput(CTelephonyAudioRouting::TAudioOutput aOutput,
        TBool aShowNote)
    {
    TEFLOGSTRING2(
            KTAREQIN,
            "AUD CPEAudioData::SetTAROutput: aOutput = %d",
            aOutput );

#if defined(__WINSCW__ ) && !defined(UNIT_TESTING)
    iAudioRouting->SetShowNote(aShowNote);
    SendMessage(MEngineMonitor::EPEMessageAudioOutputChanged, aOutput);
#else
    CTelephonyAudioRouting::TAudioOutput output = iAudioRouting->Output();

    if ( output == aOutput )
        {
        // audio routing cannot changed
        TEFLOGSTRING2( KTAERROR,
                "AUD CPEAudioData::SetTAROutput: audio path already (%d)"
                , aOutput );
        return;
        }
    iAudioRouting->SetShowNote( aShowNote );
    TRAPD( err, iAudioRouting->SetOutputL( aOutput ) );

    if( err )
        {
        TEFLOGSTRING2( KTAERROR,
                "AUD CPEAudioData::SetTAROutput:Leave.1 (%d)", err );
        switch ( aOutput )
            {
            case CTelephonyAudioRouting::ELoudspeaker:
            case CTelephonyAudioRouting::EWiredAudioAccessory:
            case CTelephonyAudioRouting::EBTAudioAccessory:
                // if leave try handset
                TRAPD( err2, iAudioRouting->SetOutputL( CTelephonyAudioRouting::EHandset ) );
                if( err2 )
                    {
                    TEFLOGSTRING2( KTAERROR,
                            "AUD CPEAudioData::SetTAROutput:Leave.2 (%d)", err );
                    }
                break;
            default:
                // None
                break;
            }
        }
#endif
    }

// -----------------------------------------------------------------------------
// CPEAudioData::StartUp
// Gets initial values and inializes TAR
// StartUpL methods.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEAudioData::StartUp()
    {
    TEFLOGSTRING( KTAREQIN, "AUD CPEAudioData::StartUp" );
    // get available audio output paths and current audio output
    iPhoneModel.DataStore()->SetAvailableAudioOutputs(GetAvailableOutputs(
            *iAudioRouting));
    iPhoneModel.DataStore()->SetAudioOutput(ConvertToPE(
            iAudioRouting->Output()), ConvertToPE(
            iAudioRouting->PreviousOutput()), EFalse);
    }

// -----------------------------------------------------------------------------
// CPEAudioData::GetAvailableOutputs
// Gets Available Outputs from Audio routing framework
// Return list of available outputs
// -----------------------------------------------------------------------------
//
TArray<TPEAudioOutput> CPEAudioData::GetAvailableOutputs(
        CTelephonyAudioRouting& aTelephonyAudioRouting)
    {
    TEFLOGSTRING( KTAREQOUT,
            "AUD CPEAudioData::GetAvailableOutputs > CTelephonyAudioRouting::AvailableOutputs()" );
    TArray<CTelephonyAudioRouting::TAudioOutput> availableOutputs =
            aTelephonyAudioRouting.AvailableOutputs();

    iPEavailableOutputs.Reset();

    for (TInt j = 0; j < availableOutputs.Count(); j++)
        {
        TInt err = iPEavailableOutputs.Append(ConvertToPE(availableOutputs[j]));
        TEFLOGSTRING4( KTAINT,
                "AUD CPEAudioData::GetAvailableOutputs, index: %d, available: %d, err: %d",
                j,
                availableOutputs[j],
                err );
        if(err != KErrNone)
            {
            // Do nothing. Here for just to suppress the compile warning.
            }
        }

    return iPEavailableOutputs.Array();
    }

// -----------------------------------------------------------------------------
// CPEAudioData::HandleCallStarting
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEAudioData::HandleCallStarting(TBool aVideoCall)
    {
    TEFLOGSTRING( KTAMESINT, "AUD CPEAudioData::HandleCallStarting()" );
    TInt ret(KErrNone);

    if (!iRouteInitialized) // if route not already initialized
        { // Not new route init for multi calls
        iRouteInitialized = ETrue;
        ret = iAudioRoutingHandler->HandleAudioRoutingCallInit(aVideoCall);
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CPEAudioData::HandleEnergencyCallStarting
// Can be called, if new call is created before previous call is idle state
// One used situation is that emergency call is dialed during the video call or
// voice call. Audio routing have to initialized when emergency is dialing state
// even previous call(voice or video) haven't got idle yet.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEAudioData::HandleEnergencyCallStarting()
    {
    TEFLOGSTRING( KTAMESINT, "AUD CPEAudioData::HandleCallSwitching()" );
    TInt ret(KErrNone);

    // Always new route init
    ret = iAudioRoutingHandler->HandleAudioRoutingCallInit(EFalse);
    iRouteInitialized = ETrue;

    return ret;
    }

// -----------------------------------------------------------------------------
// CPEAudioData::HandleCallEnding()
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEAudioData::HandleCallEnding()
    {
    TEFLOGSTRING( KTAMESINT, "AUD CPEAudioData::HandleCallEnding()" );

    if (iRouteInitialized)
        {
        iRouteInitialized = EFalse;
        TEFLOGSTRING( KTAMESINT,
                "AUD CPEAudioData::HandleCallEnding() > CTelephonyAudioRouting::ENotActive" );
        SetTAROutput(CTelephonyAudioRouting::ENotActive, EFalse);
        }
    }

// -----------------------------------------------------------------------------
// CPEAudioData::CallAudioRoutePreferenceChanged()
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEAudioData::CallAudioRoutePreferenceChanged()
    {
    TEFLOGSTRING( KTAMESINT, "AUD CPEAudioData::CallAudioRoutePreferenceChanged()" );
    if (iRouteInitialized)
        {
        return iAudioRoutingHandler->HandleAudioRoutePreferenceChanged();
        }
    return KErrNotReady;
    }

// -----------------------------------------------------------------------------
// CPEAudioData::PreviousOutput
// -----------------------------------------------------------------------------
//
CTelephonyAudioRouting::TAudioOutput CPEAudioData::PreviousOutput()
    {
    return iAudioRouting->PreviousOutput();
    }

// -----------------------------------------------------------------------------
// CPEAudioData::Output
// -----------------------------------------------------------------------------
//
CTelephonyAudioRouting::TAudioOutput CPEAudioData::Output()
    {
    return iAudioRouting->Output();
    }

// -----------------------------------------------------------------------------
// CPEAudioData::SetRoutePreference
// -----------------------------------------------------------------------------
//
void CPEAudioData::SetRoutePreference(TBool aShowNote)
    {
    TEFLOGSTRING( KTAMESINT, "AUD CPEAudioData::SetRoutePreference()" );
    CTelephonyAudioRouting::TAudioOutput output =
            CTelephonyAudioRouting::EHandset;

    const TInt outputPreference =
            iPhoneModel.DataStore()->AudioOutputPreference();
    if (outputPreference == EPSAudioPublic)
        {
        output = CTelephonyAudioRouting::ELoudspeaker;
        }
    TInt error;
    iAudioRouting->SetShowNote(aShowNote);
    TRAP( error, iAudioRouting->SetOutputL( output ) );
    TEFLOGSTRING2( KTAMESINT, "AUD CPEAudioData::SetRoutePreference() err %d", error );
    }

// -----------------------------------------------------------------------------
// CPEAudioData::RoutePreference
// -----------------------------------------------------------------------------
//
CTelephonyAudioRouting::TAudioOutput CPEAudioData::RoutePreference()
    {
    CTelephonyAudioRouting::TAudioOutput output =
            CTelephonyAudioRouting::EHandset;

    const TInt outputPreference =
            iPhoneModel.DataStore()->AudioOutputPreference();
    if (outputPreference == EPSAudioPublic)
        {
        output = CTelephonyAudioRouting::ELoudspeaker;
        }
    else
        {
        TInt value; 
        const TInt err = RProperty::Get( KPSUidTelCarMode, KTelCarMode, value );
        if ( !err && value == EPSCarModeOn )
            {
            output = CTelephonyAudioRouting::ELoudspeaker;
            }
        }
        
    TEFLOGSTRING2( KTAMESINT, "AUD CPEAudioData::RoutePreference() output %d", output );
    return output;
    }
// -----------------------------------------------------------------------------
// CPEAudioData::IsWiredAvailable
// -----------------------------------------------------------------------------
//
TBool CPEAudioData::IsWiredAvailable()
    {
    return iPhoneModel.DataStore()->AudioOutputAvailable(
            EPEWiredAudioAccessory);
    }

// -----------------------------------------------------------------------------
// CPEAudioData::IsBTAvailable
// -----------------------------------------------------------------------------
//
TBool CPEAudioData::IsBTAvailable()
    {
    return iPhoneModel.DataStore()->AudioOutputAvailable(EPEBTAudioAccessory);
    }

// -----------------------------------------------------------------------------
// CPEAudioData::IsTTYAvailable
// -----------------------------------------------------------------------------
//
TBool CPEAudioData::IsTTYAvailable()
    {
    return iPhoneModel.DataStore()->AudioOutputAvailable(EPETTY);
    }

// -----------------------------------------------------------------------------
// CPEAudioData::ConvertToPE
// -----------------------------------------------------------------------------
//
TPEAudioOutput CPEAudioData::ConvertToPE(
        CTelephonyAudioRouting::TAudioOutput aOutput)
    {
    TEFLOGSTRING2( KTAMESINT, "AUD CPEAudioData::ConvertToPE, aOutput %d", aOutput );
    TPEAudioOutput newOutput(EPENotActive);

    switch (aOutput)
        {
        case CTelephonyAudioRouting::ENotActive:
            newOutput = EPENotActive;
            break;
        case CTelephonyAudioRouting::EHandset:
            newOutput = EPEHandset;
            break;
        case CTelephonyAudioRouting::ELoudspeaker:
            newOutput = EPELoudspeaker;
            break;
        case CTelephonyAudioRouting::EWiredAudioAccessory:
            newOutput = EPEWiredAudioAccessory;
            break;
        case CTelephonyAudioRouting::EBTAudioAccessory:
            newOutput = EPEBTAudioAccessory;
            break;
        case CTelephonyAudioRouting::ETTY:
            newOutput = EPETTY;
            break;
        default:
            break;
        }

    return newOutput;
    }

// -----------------------------------------------------------------------------
// CPEAudioData::ConvertToTAR
// -----------------------------------------------------------------------------
//
CTelephonyAudioRouting::TAudioOutput CPEAudioData::ConvertToTAR(
        TPEAudioOutput aOutput)
    {
    TEFLOGSTRING2( KTAMESINT, "AUD CPEAudioData::ConvertToTAR, aOutput %d", aOutput );

    CTelephonyAudioRouting::TAudioOutput newOutput(
            CTelephonyAudioRouting::ENotActive);

    switch (aOutput)
        {
        case EPENotActive:
            newOutput = CTelephonyAudioRouting::ENotActive;
            break;
        case EPEHandset:
            newOutput = CTelephonyAudioRouting::EHandset;
            break;
        case EPELoudspeaker:
            newOutput = CTelephonyAudioRouting::ELoudspeaker;
            break;
        case EPEWiredAudioAccessory:
            newOutput = CTelephonyAudioRouting::EWiredAudioAccessory;
            break;
        case EPEBTAudioAccessory:
            newOutput = CTelephonyAudioRouting::EBTAudioAccessory;
            break;
        case EPETTY:
            newOutput = CTelephonyAudioRouting::ETTY;
            break;
        default:
            break;
        }
    return newOutput;
    }

// -----------------------------------------------------------------------------
// CPEAudioData::DoHandleVolumeChange
// -----------------------------------------------------------------------------
//
void CPEAudioData::DoHandleVolumeChange(TInt aVolume, TBool aSendMsg)
    {
    iPhoneModel.DataStore()->SetAudioVolume(aVolume);

    if (aSendMsg)
        {
        SendMessage(MEngineMonitor::EPEMessageAudioVolumeChanged);
        }
    }

// -----------------------------------------------------------------------------
// CPEAudioData::EffectsEvent
// -----------------------------------------------------------------------------
//
void CPEAudioData::EffectsEvent(const TMSEffect& tmseffect,
        TMSSignalEvent event)
    {
    TMSEffectType effecttype;
    const_cast<TMSEffect&>(tmseffect).GetType(effecttype);

    if (effecttype == TMS_EFFECT_GLOBAL_VOL &&
            event.type == TMS_EVENT_EFFECT_VOL_CHANGED)
        {
        TMSVolumeEventChangeData* vd;
        vd = static_cast<TMSVolumeEventChangeData*>(event.event_data);

        // If volume change event has been triggered by routing change, we
        // do not want to send EPEMessageAudioVolumeChanged message.
        if (!vd->output_changed)
            {
            DoHandleVolumeChange(iAudioEffect->Volume(), ETrue);
            }
        else
            {
            DoHandleVolumeChange(iAudioEffect->Volume(), EFalse);
            }
        }
    }

// End of File
