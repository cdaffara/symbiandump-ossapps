/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Audio util class.
*
*/


#ifndef PHONEAUDIOUTIL_H
#define PHONEAUDIOUTIL_H

// INCLUDES
#include <e32base.h>
#include <pevirtualengine.h>
#include "mphoneviewcommandhandle.h"
#include "mphonestatemachine.h"

// CLASS DECLARATION
 
/**
*  Note util class.
*
*/
class PhoneAudioUtil
    {
    public:
        
        /**
         * Handles audio mute changes.
         */
        static void HandleAudioMuteChangedL();
    
        /**
         * Handles audio output changes.
         */
        static void HandleAudioOutputChangedL();
        
        /**
         * Decreases audio volume.
         */
        static void DecreaseAudioVolumeL();
        
        /**
         * Increases audio volume.
         */
        static void IncreaseAudioVolumeL();
        
        /**
         * Changes audio volume.
         */
        static void ChangeAudioVolumeL( TInt aLevel, 
                TBool aUpdateControl );
        
        /**
         * Gets current volume level.
         */
        static TInt GetVolumeLevel();
        
        /**
         * Handles audio output changes.
         */
        static void HandleAudioAvailableOutputChangedL();
        
        /**
         * Toggles mute.
         */
        static void ToggleMute();
        
        /**
         * Sets handsfree mode.
         */
        static void SetHandsfreeModeL( TBool aHandsfreeMode );
        
        /**
         * Sets bt handsfree mode.
         */
        static void SetBtHandsfreeModeL( TBool aHandsfreeMode );
        
    private:
        
        /**
         * Returns view command handler.
         */
        inline static MPhoneViewCommandHandle* ViewCommandHandle();
 
        /**
         * Returns phone state handler.
         */
        inline static MPhoneStateMachine* StateMachine();
    };

#endif      // PHONEAUDIOUTIL_H  
            
// End of File
