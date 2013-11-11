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
* Description:  Video telephony engine events
*
*/



#ifndef VTENGEVENTS_H
#define VTENGEVENTS_H

//  INCLUDES
#include <e32def.h>

// CONSTANTS

const TInt KVtEngEventNone = -1;

const TInt KVtEngEventBase = 0;

/**
* Engine configuration events
*/
const TInt KVtEngConfigureEventBase     = KVtEngEventBase + 100; // 100
const TInt KVtEngDurationSettingChanged = KVtEngConfigureEventBase + 1;
const TInt KVtEngInitializationProblem  = KVtEngConfigureEventBase + 2;

/**
* Engine media events
*/
const TInt KVtEngMediaEventBase         = KVtEngConfigureEventBase + 100; // 200
const TInt KVtEngCameraChanged          = KVtEngMediaEventBase + 1;
const TInt KVtEngSourceChanged          = KVtEngMediaEventBase + 2;
const TInt KVtEngRemoteVideoPaused      = KVtEngMediaEventBase + 3;
const TInt KVtEngRemoteVideoResumed     = KVtEngMediaEventBase + 4;
const TInt KVtEngRemoteAudioPaused      = KVtEngMediaEventBase + 5;
const TInt KVtEngRemoteAudioResumed     = KVtEngMediaEventBase + 6;
const TInt KVtEngAudioMuted             = KVtEngMediaEventBase + 7;
const TInt KVtEngAudioUnmuted           = KVtEngMediaEventBase + 8;
const TInt KVtEngRemoteVideoStarted     = KVtEngMediaEventBase + 10;
const TInt KVtEngRemoteVideoStopped     = KVtEngMediaEventBase + 11;
const TInt KVtEngRemoteAudioStarted     = KVtEngMediaEventBase + 12;
const TInt KVtEngRemoteRenderingProblem = KVtEngMediaEventBase + 14;
const TInt KVtEngLocalRenderingProblem  = KVtEngMediaEventBase + 15;
const TInt KVtEngSourceCapsChanged      = KVtEngMediaEventBase + 16;
const TInt KVtEngRemoteRenderStopped    = KVtEngMediaEventBase + 17;
const TInt KVtEngViewFinderStopped      = KVtEngMediaEventBase + 18;
const TInt KVtEngResolutionToSQCIF      = KVtEngMediaEventBase + 19;
const TInt KVtEngResolutionToQCIF       = KVtEngMediaEventBase + 20;

// Problems in starting negotiation with remote end
const TInt KVtEngNegotiationProblem     = KVtEngMediaEventBase + 21;

// Initializing media component failed
const TInt KVtEngMediaInitFailure       = KVtEngMediaEventBase + 22;

// Begin initialization of image sharing (convert, scale, etc.)
const TInt KVtEngShareImageInitializeBegin = KVtEngMediaEventBase + 23;

// End initialization of image sharing (convert, scale, etc.)
const TInt KVtEngShareImageInitializeEnd   = KVtEngMediaEventBase + 24;

// Informs status change in outgoing video channel.
const TInt KVtEngMediaOutgoingVideoChannelStatusChanged =
    KVtEngMediaEventBase + 25;

// Informs status change in outgoing audio channel.
const TInt KVtEngMediaOutgoingAudioChannelStatusChanged =
    KVtEngMediaEventBase + 26;

// Informs UI that decoding error happened during image share
const TInt KVtEngShareImageDecodingError = KVtEngMediaEventBase + 27;

// Informs UI that volume is already maximum
const TInt KVtEngAudioVolumeMax = KVtEngMediaEventBase + 28;

// Informs UI that volume is already minimum
const TInt KVtEngAudioVolumeMin = KVtEngMediaEventBase + 29;

// Informs UI that Locvid layoutchange has been done
const TInt KVtEngLCHProviderSwitchDone =  KVtEngMediaEventBase + 30;

/**
* Engine audio events
*/
const TInt KVtEngAudioEventBase         = KVtEngMediaEventBase + 100; // 300
const TInt KVtEngAudioOutputVolumeChanged = KVtEngAudioEventBase + 1;
const TInt KVtEngAudioRoutingChanged    = KVtEngAudioEventBase + 2;
const TInt KVtEngAudioRoutingAvailabilityChanged = KVtEngAudioEventBase + 3;
const TInt KVtEngAudioRoutingIHFActivated = KVtEngAudioEventBase + 4;
const TInt KVtEngAudioRoutingIHFDeactivated = KVtEngAudioEventBase + 5;

/**
* Engine session events
*/
const TInt KVtEngSessionEventBase       = KVtEngAudioEventBase + 100; // 400
const TInt KVtEngSessionStateChanged    = KVtEngSessionEventBase + 1;
const TInt KVtEngSessionAnswerPossible  = KVtEngSessionEventBase + 2;
const TInt KVtEngSessionWaitingCallActive  = KVtEngSessionEventBase + 3;
const TInt KVtEngSessionWaitingCallInactive = KVtEngSessionEventBase + 4;
const TInt KVtEngCLIAvailable           = KVtEngSessionEventBase + 5;

/**
* General events
*/
const TInt KVtEngGeneralEventBase       = KVtEngSessionEventBase + 100; // 500
const TInt KVtEngResourceMemAllocFailure = KVtEngGeneralEventBase + 1;
const TInt KVtEngDtmfFailure            = KVtEngGeneralEventBase + 2;
/**
* Remote supports UII DTMF.
*/
const TInt KVtEngRemoteUIIDtmfSupport           = KVtEngGeneralEventBase + 13;
/**
* Remote supports UII basic string.
*/
const TInt KVtEngRemoteUIIBasicStringSupport = KVtEngGeneralEventBase + 14;
/**
* Remote supports UII IA5 string.
*/
const TInt KVtEngRemoteUIIIA5StringSupport   = KVtEngGeneralEventBase + 15;
/**
* Remote supports UII general string.
*/
const TInt KVtEngRemoteUIIGeneralStringSupport = KVtEngGeneralEventBase + 6;


// Device is locked or unlocked (autolock feature)
const TInt KVtEngDeviceLockOn           = KVtEngGeneralEventBase + 3;
const TInt KVtEngDeviceLockOff          = KVtEngGeneralEventBase + 4;

// Remote disconnected
const TInt KVtEngRemoteDisconnect          = KVtEngGeneralEventBase + 5;

#endif      // VTENGEVENTS_H

// End of File
