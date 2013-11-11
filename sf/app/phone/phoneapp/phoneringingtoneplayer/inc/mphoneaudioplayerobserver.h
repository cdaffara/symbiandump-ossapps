/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MPHONEAUDIOPLAYEROBSERVER_H
#define MPHONEAUDIOPLAYEROBSERVER_H

// User includes
#include "phoneconstants.h"

// Class declaration

/**
*  Observer interface for audio player.
*
* @since 1.0
*/
class MPhoneAudioPlayerObserver
    {
    public:  // Constructors and destructor
        
        // Enumerates different failures.
        //
        // EAudioPlayerInitializingFailure - failure in initialising
        // EAudioPlayerPlayingFailure - failure during playing
        enum TPhoneAudioPlayerErrorEvent
            {
            EAudioPlayerInitializingFailure,
            EAudioPlayerPlayingFailure
            };
        
        /**
        * Observe the error.
        * @param aEvent the error event happend in initializing or playing.
        * @param aError The error code return by audio server. Observer can 
        *               handle  these two error code for recoverable 
        *               and unrecoverable errors.
        * @param aId Indicate which audio player is outof working.
        */
        virtual void HandleAudioPlayerError( 
            TPhoneAudioPlayerErrorEvent aEvent,
            TInt aError, 
            TInt aId = KPhoneAudioPlayerNotSpecified ) = 0;

        /**
        * This function is called when audio player initialization complete.
        * @param aId The id of the audio player.
        */
        virtual void HandleAudioPlayerInitComplete(
            TInt aId = KPhoneAudioPlayerNotSpecified ) = 0;

        /**
        * This funciton is called after the playing complete successfully.
        * @param aId The id of the audio player.
        */
        virtual void HandlePlayingComplete( 
            TInt aId = KPhoneAudioPlayerNotSpecified ) = 0;
    };

#endif      // MPHONEAUDIOPLAYEROBSERVER_H   
            
// End of File
