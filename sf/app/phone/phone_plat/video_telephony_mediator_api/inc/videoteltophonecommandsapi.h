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
* Description:  Video Telephony specific message services sent from Video
*                Telephony to Phone Application.
*
*/


#ifndef VIDEOTELTOPHONECOMMANDSAPI_H
#define VIDEOTELTOPHONECOMMANDSAPI_H

// INCLUDES
#include <e32base.h>

/**  The category UID for the messages in this header file. */
const TUid KCatVideoTelToPhoneCommands = { 0x100058B6 };

/**  Version number of this API */
const TInt KVideoTelToPhoneCmdVersionMajor( 1 );
const TInt KVideoTelToPhoneCmdVersionMinor( 0 );
const TInt KVideoTelToPhoneCmdVersionBuild( 0 );

/**  Video Telephony related commands to Phone Application. */
enum TVideoTelToPhoneCommands
    {
    /**
     * Commands Phone to do fallback.
     * @capability ECapabilityNetworkControl
     */
    EVtCmdFallback = 0,
    
    /**
     * Commands Phone to switch back to voice call.
     * @capability ECapabilityNetworkControl
     */
    EVtCmdSwitchToVoice,
    
    /**
     * Commands phone that VT has not enough memory.
     * @capability ECapabilityNetworkControl
     */
    EVtCmdLowMemory

    };

#endif // VIDEOTELTOPHONECOMMANDSAPI_H

