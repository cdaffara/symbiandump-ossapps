/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Audio specific message services sent from 3rd party client to
*                Phone Application.
*
*/


#ifndef MEDIATORAUDIOCOMMANDSTOTELEPHONYAPI_H
#define MEDIATORAUDIOCOMMANDSTOTELEPHONYAPI_H

// INCLUDES
#include <e32base.h>

/**  The category UID for the messages in this header file. */
const TUid KCatAudioCommandsToTelephony = { 0x100058BF };

/**  Version number of this API */
const TInt KAudioCmdToTelephonyVersionMajor( 1 );
const TInt KAudioCmdToTelephonyVersionMinor( 0 );
const TInt KAudioCmdToTelephonyVersionBuild( 0 );

/**  Video Telephony related commands to Phone Application. */
enum TAudioCommandsToTelephony
    {    
    /**
     * Commands Telephony to unmute microphone.
     * @capability ECapability_None
     */
    EAudioCmdUnmute = 0,
    
    /**
     * Commands Telephony to mute microphone.
     * @capability ECapability_None
     */
    EAudioCmdMute

    };

#endif // MEDIATORAUDIOCOMMANDSTOTELEPHONYAPI_H

