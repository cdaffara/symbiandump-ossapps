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
* Description:  Plguin command
*
*/

#ifndef LCVTPLUGINCOMMAND_H_
#define LCVTPLUGINCOMMAND_H_

enum
    {
    // End active call
    EPluginCmdEndActiveCall = 5000,
    // Switch to voice call
    EPluginCmdCreateVoice,
    // Create a emergency call
    EPluginCmdDialEmergency,    
    // Enable video
    EPluginCmdEnableVideo,
    // Enable audio
    EPluginCmdEnableAudio,
    // Disable video
    EPluginCmdDisableVideo,
    // Disable audio
    EPluginCmdDisableAudio,
    // Use primary camera
    EPluginCmdUsePrimaryCamera,
    // Use secondary camera
    EPluginCmdUseSecondaryCamera,
    // Active Loudspeaker
    EPluginCmdActivateLoudspeaker,
    // Deactive Loudspeaker
    EPluginCmdDeactivateLoudspeaker
    };


#endif /* LCVTPLUGINCOMMAND_H_ */
