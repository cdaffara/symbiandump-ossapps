/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Video Telephony specific message services sent from Phone
*                Application to Video Telephony.
*
*/


#ifndef VIDEOTELCONTROLMEDIATORAPI_H
#define VIDEOTELCONTROLMEDIATORAPI_H

// INCLUDES
#include <e32base.h>

/**  The category UID for the messages in this header file. */
const TUid KCatPhoneToVideotelCommands = { 0x200159A1 };

/**  Version number of this API */
const TInt KPhoneToVideotelCmdVersionMajor( 1 );
const TInt KPhoneToVideotelCmdVersionMinor( 0 );
const TInt KPhoneToVideotelCmdVersionBuild( 0 );

/** package for passing dataport name to Video Telephony */
typedef TPckgBuf<TName> TDataPortPackage;

/**  Video Telephony related commands. */
enum TPhoneToVideotelCommands
    {
    /**
     * Commands Video Telephony to enable microphone.
     */
    EVtCmdUnmute = 0,
    
    /**
     * Commands Video Telephony to disable microphone.
     */
    EVtCmdMute,
        
    /**
     * Commands Video Telephony to use dataport. Command paramemeter is
     * TDataPortPackage. This should be called when dataport is loaned.
     * When this is sent, dataport recovery is allowed only after
     * 1) later issued EVtCmdShutdown response is received.
     * 2) call is ended by network/peer (i.e. local phone doesn't do hangup)
     * 3) EPhoneCmdEndActiveCall command is received (see Mediator Commands To 
     *    Telephony API)
     *  Otherwise H.245 (VT) signalling may fail if call is released in two
     *  stage.
     */
    EVtCmdUseDataport,
    
    EVtCmdReleaseDataport
    };

#endif // VIDEOTELCONTROLMEDIATORAPI_H

