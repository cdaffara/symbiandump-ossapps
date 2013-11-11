/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Video telephony engine command identifiers.
*
*/



#ifndef CVTENGCOMMANDS_H
#define CVTENGCOMMANDS_H

//  INCLUDES
#include <e32def.h>

//  DATA TYPES
typedef TInt TVtEngCommandId;

// CONSTANTS


const TInt KVtEngCommandNone = -1;

const TInt KVtEngCommandBase = 0;

/**
* Configuration commands
*/
const TInt KVtEngConfigureCommandBase = KVtEngCommandBase + 100;

// Allocates essential resources when application starts up
const TInt KVtEngInitializeEngine     = KVtEngConfigureCommandBase + 1;

// Resets engine. Use when all video calls are ended.
const TInt KVtEngResetEngine          = KVtEngConfigureCommandBase + 2;

/**
* Makes voice call to existing peer.
*/
const TInt KVtEngSwitchToVoice        = KVtEngConfigureCommandBase + 4;

// Allocates essential resources when application starts up - TEST MODE
const TInt KVtEngInitializeEngineDiag     = KVtEngConfigureCommandBase + 50;

// Allocates essential resources when application starts up - TEST MODE
const TInt KVtEngInitializeEngineTest     = KVtEngConfigureCommandBase + 51;
//

/**
* Media commands for controlling local display and sending
* of media (audio/video) to the peer.
*/
const TInt KVtEngMediaCommandBase   = KVtEngConfigureCommandBase + 100;      // 200

// Actual commands

/**
* Selects source,
* @see MVtEngMedia::TMediaSource for command parameters.
*/
const TInt KVtEngSetSource            = KVtEngMediaCommandBase + 1;

/**
* Async
* Initializes default or selected camera.
* Does not take parameters. See KVtEngSetSource command parameters.
*/
const TInt KVtEngPrepareCamera        = KVtEngMediaCommandBase + 2;

/**
* prepares engine to pass view finder frames to UI
* @see MVtEngMedia::TVtEngViewFinderConfig for command parameters.
*/
const TInt KVtEngPrepareViewFinder    = KVtEngMediaCommandBase + 3;

/**
* Sync
* prepares engine to render view finder frames using DSA
* @see MVtEngMedia::TRenderingOptionsDSA for command parameters.
*/
const TInt KVtEngPrepareViewFinderDSA = KVtEngMediaCommandBase + 4;

/**
* Async
* Starts view finder
* @see MVtEngMedia::TRenderingOptionsDSA for command parameters.
*/
const TInt KVtEngStartViewFinder      = KVtEngMediaCommandBase + 5;

/**
* Async
* View finder control commands, do not take parameters.
*/
const TInt KVtEngPauseViewFinder      = KVtEngMediaCommandBase + 6;
const TInt KVtEngStopViewFinder       = KVtEngMediaCommandBase + 7;

/**
* Prepares engine for rendering video (with DSA) from peer
* @see MVtEngMedia::TRenderingOptionsDSA for command parameters.
*/
const TInt KVtEngPrepareRemoteRenderDSA  = KVtEngMediaCommandBase + 8;

/**
* Prepares engine for rendering video from peer
* @see MVtEngMedia::TVtEngViewFinderConfig for command parameters.
*/
const TInt KVtEngPrepareRemoteRender  = KVtEngMediaCommandBase + 9;

/**
* Remote video rendering commands, do not take parameters.
*/
const TInt KVtEngStartRenderRemote    = KVtEngMediaCommandBase + 10;
const TInt KVtEngPauseRenderRemote    = KVtEngMediaCommandBase + 11;
const TInt KVtEngStopRenderRemote     = KVtEngMediaCommandBase + 12;

/**
* Media transport control commands, do not take parameters.
*/
// Audio
const TInt KVtEngMuteOutgoingAudio     = KVtEngMediaCommandBase + 13;
const TInt KVtEngUnmuteOutgoingAudio    = KVtEngMediaCommandBase + 14;
const TInt KVtEngPauseIncomigAudio      = KVtEngMediaCommandBase + 15;
const TInt KVtEngResumeIncomigAudio     = KVtEngMediaCommandBase + 16;

/**
* Sync.
* Sets video source provider zoom step.
* Parameter is TInt from zero to maximum zoom step.
* @see MVtEngMedia::GetMaxZoomStep.
*/
const TInt KVtEngSetZoomStep            = KVtEngMediaCommandBase + 17;

/**
* Sync
* Sets size of remote video, parameter is of type TSize
*/
const TInt KVtEngSetRemoteVideoSize       = KVtEngMediaCommandBase + 18;

/**
* Sets UI foreground state (ETrue or EFalse)
*/
const TInt KVtEngSetUIForeground            = KVtEngMediaCommandBase + 19;

/**
* Freeze provider video.
*/
const TInt KVtEngFreeze                     = KVtEngMediaCommandBase + 20;

/**
* Unfreeze provider video.
*/
const TInt KVtEngUnfreeze                   = KVtEngMediaCommandBase + 21;

/**
* Adjusts video quality.
*/
const TInt KVtEngSetVideoQuality            = KVtEngMediaCommandBase + 22;

/**
* Initialize image sharing.
*/
const TInt KVtEngInitializeShareImage       = KVtEngMediaCommandBase + 23;

/**
* Start image sharing (must be initialized first).
*/
const TInt KVtEngStartShareImage            = KVtEngMediaCommandBase + 24;

/**
* Stop image sharing.
*/
const TInt KVtEngStopShareImage             = KVtEngMediaCommandBase + 25;

/**
* Sync
* prepares engine to render view finder frames using DP
* @see MVtEngMedia::TRenderingOptionsDP for command parameters.
*/
const TInt KVtEngPrepareViewFinderDP        = KVtEngMediaCommandBase + 26;

/**
* Prepares engine for rendering video (with DP) from peer
* @see MVtEngMedia::TRenderingOptionsDP for command parameters.
*/
const TInt KVtEngPrepareRemoteRenderDP      = KVtEngMediaCommandBase + 27;

/**
* Engine reintializes camera instance when layout change happens and active
* provider is onboard camera. If anyother provider is selected, then nothing
* will be done by the engine.
*/
const TInt KVtEngHandleLayoutChange         = KVtEngMediaCommandBase + 28;

/**
 * Sync
 * Request update last remote video frame through MVtEngFrameObserver::vtSetFrame 
 */
const TInt KVtEngRequestLastRemoteFrame  = KVtEngMediaCommandBase + 29;


const TInt KVtEngPrepareRemoteRenderNGA      = KVtEngMediaCommandBase + 30;

/**
* Audio control
*/
const TInt KVtEngAudioCommandBase       = KVtEngMediaCommandBase + 100; // 300

// Actual commands

/**
* Sets output volume level [1..10] for active audio device
*/
const TInt KVtEngSetAudioVolume         = KVtEngAudioCommandBase + 1;

/**
* Audiorouting command
*/
const TInt KVtEngSetAudioRouting        = KVtEngAudioCommandBase + 2;

/**
* Increase volume by one step.
*/
const TInt KVtEngIncreaseAudioVolume    = KVtEngAudioCommandBase + 3;

/**
* Decrease volume by one step.
*/
const TInt KVtEngDecreaseAudioVolume    = KVtEngAudioCommandBase + 4;

/**
* Session commands
*/
const TInt KVtEngSessionCommandBase     = KVtEngAudioCommandBase + 100;

// Actual commands

/**
* Terminates session and tears down connection
*/
const TInt KVtEngTerminateSession       = KVtEngSessionCommandBase + 1;

const TInt KVtEngSendDtmf               = KVtEngSessionCommandBase + 2;

/**
* Starts sending a DTMF tone. Takes TChar parameter. Sync
*/
const TInt KVtEngStartDtmfTone          = KVtEngSessionCommandBase + 3;

/**
* Stops previously started tone. Does not take parametners. Sync
*/
const TInt KVtEngStopDtmfTone           = KVtEngSessionCommandBase + 4;

/**
* Remote supports UII DTMF.
*/
const TInt KVtEngSetUIIDtmfSupport           = KVtEngSessionCommandBase + 5;

/**
* Remote supports UII basic string.
*/
const TInt KVtEngSetUIIBasicStringSupport = KVtEngSessionCommandBase + 6;

/**
* Remote supports UII IA5 string.
*/
const TInt KVtEngSetUIIIA5StringSupport   = KVtEngSessionCommandBase + 7;

/**
* Remote supports UII general string.
*/
const TInt KVtEngSetUIIGeneralStringSupport = KVtEngSessionCommandBase + 8;


/**
* Extension commands.
*/
const TInt KVtEngExtensionCommandBase   = KVtEngSessionCommandBase + 100; //500
const TInt KVtEngSetContrast            = KVtEngExtensionCommandBase + 1;
const TInt KVtEngSetBrightness          = KVtEngExtensionCommandBase + 2;
const TInt KVtEngSetWhiteBalance        = KVtEngExtensionCommandBase + 3;
const TInt KVtEngSetColorTone           = KVtEngExtensionCommandBase + 4;
#if defined ( RD_VT_RTF )
const TInt KVtEngStartRecord			= KVtEngExtensionCommandBase + 5;
const TInt KVtEngStopRecord				= KVtEngExtensionCommandBase + 6;
#endif // RD_VT_RTF

#endif      // CVTENGCOMMANDS_H

// End of File
