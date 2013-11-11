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

#ifndef MPHONEVIDEOPLAYEROBSERVER_H
#define MPHONEVIDEOPLAYEROBSERVER_H

// Class declaration

/**
*  Video player observer interface.
*
*  @lib Phone.app
*  @since Series 60 3.1
*/
class MPhoneVideoPlayerObserver  
    {
    public:
        
        // Enumerates different failure events.
        //
        enum TPhoneVideoPlayerErrorEvent
            {
            EVideoPlayerInitializingFailure, // Failure during initialization
            EVideoPlayerPlayingFailure // Failure during playback
            };
        
        /**
        * This function is called for video player errors.
        * @param aEvent The error event happend in initializing or playing.
        * @param aError The error code return by MMF video player. 
        */
        virtual void HandleVideoPlayerError( 
            TPhoneVideoPlayerErrorEvent aEvent,
            TInt aError ) = 0;

        /**
        * This function is called when video player initialization complete
        */
        virtual void HandleVideoPlayerInitComplete() = 0;

        /**
        * This function is called after the playing completes successfully
        */
        virtual void HandleVideoPlayerPlayingComplete() = 0;
    };

#endif      // MPHONEVIDEOPLAYEROBSERVER_H   
            
// End of File
