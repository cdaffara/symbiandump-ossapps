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
* Description:  
*
*/


#include <pv_2way_symbian_interface.h>
#include <pv_audio_output_control_interface.h>
#include "mvtprotocolhandler.h"
#include "caudioconfighandler.h"

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// -----------------------------------------------------------------------------
// TAudioConfigHandler::TAudioConfigHandler
// Constructor.
// -----------------------------------------------------------------------------
//
TAudioConfigHandler::TAudioConfigHandler(MVtProtocolHandler* aProtocolHandler) : iProtocolHandler(aProtocolHandler)
    {
    __IF_DEBUG(Print(_L("VTProto: TAudioConfigHandler::TAudioConfigHandler<")));
    __IF_DEBUG(Print(_L("VTProto: TAudioConfigHandler::TAudioConfigHandler>")));	
    }

// -----------------------------------------------------------------------------
// TAudioConfigHandler::AudioOutputControlCommandComplete
// Signals completion of the audio output control command.
// -----------------------------------------------------------------------------
//
void TAudioConfigHandler::AudioOutputControlCommandComplete(TPVCommandId aId
                ,TPVAudioOutputControlCommand aCmd
                ,TAny *aContextData
                ,TInt aStatus)
    {
    __IF_DEBUG(Print(_L("VTProto: TAudioConfigHandler::AudioOutputControlCommandComplete<")));	
    iProtocolHandler->HandleAudioOutputControlCommandComplete(aId, (TVtAudioOutputControlCommand)aCmd, aContextData, aStatus);
    __IF_DEBUG(Print(_L("VTProto: TAudioConfigHandler::AudioOutputControlCommandComplete>")));	
    }
// -----------------------------------------------------------------------------
// CAudioConfigCommand::CAudioConfigCommand
// Constructor.
// -----------------------------------------------------------------------------
//
CAudioConfigCommand::CAudioConfigCommand(MPVAudioOutputControl* aAudioCtrl) : iAudioCtrl(aAudioCtrl)
    {
    __IF_DEBUG(Print(_L("VTProto: CAudioConfigCommand::CAudioConfigCommand<")));	
    __IF_DEBUG(Print(_L("VTProto: CAudioConfigCommand::CAudioConfigCommand>")));		
    }

// -----------------------------------------------------------------------------
// CAudioConfigCommand::SetObserverL
// This API allows the user to specify observers for the 324m interface.
// -----------------------------------------------------------------------------
//
void CAudioConfigCommand::SetObserverL(MVtProtocolHandler* aHandler)
    {
    __IF_DEBUG(Print(_L("VTProto: CAudioConfigCommand::SetObserverL<")));	
    iAudioConfigHandler = new (ELeave)TAudioConfigHandler(aHandler);
    iAudioCtrl->SetAudioOutputControlObserver(iAudioConfigHandler);
    __IF_DEBUG(Print(_L("VTProto: CAudioConfigCommand::SetObserverL>")));
    }

// -----------------------------------------------------------------------------
// CAudioConfigCommand::SetAudioVolumeL
// Method to set the playback volume to the specified value.
// -----------------------------------------------------------------------------
//
TInt CAudioConfigCommand::SetAudioVolumeL(TInt aVolume)
    {
    __IF_DEBUG(Print(_L("VTProto: CAudioConfigCommand::SetAudioVolumeL")));	
    __IF_DEBUG(Print(_L("VTProto: CAudioConfigCommand::SetAudioVolumeL aVolume %d"), aVolume));		
    return iAudioCtrl->SetVolumeL(aVolume);
    }

// -----------------------------------------------------------------------------
// CAudioConfigCommand::GetMaxAudioVolumeL
// Method to get the maximum valid value for the playback volume.
// -----------------------------------------------------------------------------
//
TInt CAudioConfigCommand::GetMaxAudioVolumeL(TInt& aMaxVolume)
    {
    __IF_DEBUG(Print(_L("VTProto: CAudioConfigCommand::GetMaxAudioVolumeL")));	
    __IF_DEBUG(Print(_L("VTProto: CAudioConfigCommand::GetMaxAudioVolumeL aMaxVolume %d"), aMaxVolume));		
    return iAudioCtrl->GetMaxVolumeL(aMaxVolume);
    }

// -----------------------------------------------------------------------------
// CAudioConfigCommand::~CAudioConfigCommand
// Destructor.
// -----------------------------------------------------------------------------
//
CAudioConfigCommand::~CAudioConfigCommand()
    {
    __IF_DEBUG(Print(_L("VTProto: CAudioConfigCommand::~CAudioConfigCommand>")));
    delete iAudioConfigHandler;
    __IF_DEBUG(Print(_L("VTProto: CAudioConfigCommand::~CAudioConfigCommand")));
    }
//  End of File
