/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Telephony Microphone Mute Status Publish and Subscribe keys.
*
*/


#ifndef TELMICMUTESTATUSPSKEYS_H
#define TELMICMUTESTATUSPSKEYS_H

#include <e32std.h>

// CONSTANTS

/*****************************************************************************
* Telephony Microphone Mute Status API
*****************************************************************************/
const TUid KPSUidTelMicrophoneMuteStatus = {0x102029A9}; 
    
/**
* Microphone mute state.
* VoIP is notified through P&S.
* @type RProperty::EInt
*/
const TUint32 KTelMicrophoneMuteState = 0x00000001;
enum TPSTelMicMuteState
    {
    EPSTelMicMuteStateUninitialized = 0,
    EPSTelMicMuteOn,
    EPSTelMicMuteOff
    };

#endif      // TELMICMUTESTATUSPSKEYS_H

// End of file