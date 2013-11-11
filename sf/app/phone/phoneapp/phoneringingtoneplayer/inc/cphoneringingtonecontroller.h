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

#ifndef CPHONERINGINGTONECONTROLLER_H
#define CPHONERINGINGTONECONTROLLER_H

// INCLUDES
#include <e32base.h>
#include <Profile.hrh>
#include "mphoneaudioplayerobserver.h"
#include "mphonevideoplayerobserver.h"
#include "mphonecoveruiobserver.h"
#include "cphoneaudioplayer.h"
#include "mphonetimer.h"
#include "mphonevideoplayer.h"
#include "cphonemediatorsender.h"

// FORWARD DECLARATIONS
//class MPhoneRingingToneObserver;
class CPhoneAudioPlayer;
class CPhoneRingingtonePlayer;
class CPhoneTTSPlayer;
class MPhoneDisplayProvider;
class CPhoneRingingTone;
class TPhoneCommandParam;

// CLASS DECLARATION

/**
* It implements ringing tone functionality. 
* Note that when issuing Play-command to audio side, ALWAYS set the tone type
* before the Play() is called. This is to ensure that correct tone is tried to
* be played. 
*
* @since 1.0
*/
NONSHARABLE_CLASS(CPhoneRingingToneController) :
    public CBase,
    private MPhoneVideoPlayerObserver,
    private MPhoneCoverUiObserver
    {
    public:
        // States for this active object
        enum TState
            {
            EIdleState,  
            EDeletingAudioPlayer,
            EDeletingVideoPlayer,
            EPlayingDefaultVideo
            };

    public:
        
        /**
        * Two-phased constructor.
        * @return new instance.
        */
        IMPORT_C static CPhoneRingingToneController* NewL();
        
        /**
        * Destructor.
        */
        ~CPhoneRingingToneController();

        /**
        * Play ring tone
        * @param aCommandParam a command param
        */
        IMPORT_C void PlayRingToneL( TPhoneCommandParam *aCommandParam );

        /**
        * Stops playing the ringing tone.
        * Method does not do anything if ringing tone is not playing.
        */
        IMPORT_C void StopPlaying();

        /**
        * Continues video playback ( and ringing tone ) with volume 0
        * Stream not closed ( If music player was ongoing ) 
        */
        IMPORT_C void MuteRingingToneOnAnswer();
          
        /**
        * Continues video playback with muted audio.
        * Ringing tone is stopped.
        */
        IMPORT_C void MuteRingingTone();
        
          
        /**
        * Sets video player.
        * @since Series 60 v3.2
        * @param aVideoPlayer Video player reference.
        */
        void SetVideoPlayer( MPhoneVideoPlayer* aVideoPlayer );

        /**
        * Play video ring tone.
        * @since Series 60 3.1
        * @param aRingingTone Ringing tone to be played.
        * @param aVolume Volume used to play the ringing tone.
        * @param aRingingType Ringing type.
        */
        void PlayVideoRingingTone( 
            const CPhoneRingingTone& aRingingTone, 
            TInt aVolume, 
            TProfileRingingType aRingingType,
            TBool aPersonalTone = EFalse );

        /**
        * Checks if extended security required.
        * @since Series 60 3.1
        * @return ETrue if extended security is required.
        */
        TBool ExtendedSecurity() const;
   
   
        /**
        * Command when necessary Cover UI to play video ringing tone.
        * return ETrue if Cover ui handle video ringing tone playing.
        * return EFalse if Phone must handle video ringing tone playing.
        */
        TBool CoverUIPlayVideoRingingToneL( 
            const CPhoneRingingTone& aRingingTone );

        /**
        * This function is called when Mediator receives response to the sent
        * ShowMultiMediaRingingTone command.
        * @param aStatus - Standard Symbian error code indicating the
        *                  success of the command.
        */
        void ShowMultimediaRingingToneResponseL( TInt aStatus );

    private: // from MPhoneVideoPlayerObserver

        /**
        * HandleVideoPlayerError
        */
        virtual void HandleVideoPlayerError( 
            TPhoneVideoPlayerErrorEvent aEvent,
            TInt aError );

        /**
        * HandleVideoPlayerInitComplete
        */
        virtual void HandleVideoPlayerInitComplete();

        /**
        * HandleVideoPlayerPlayingComplete
        */
        virtual void HandleVideoPlayerPlayingComplete();
   
    private: 
        
        /**
        * C++ default constructor.
        */
        CPhoneRingingToneController();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();


        /**
        * Convert ringing type from PhoneAppEnging to the ringing type in 
        * MPhoneVideoPlayer.
        * @since Series 60 3.1
        * @param aRingingType the ringing type form PhoneAppEngine.
        * @return ringing type of MPhoneVideoPlayer.
        */
        MPhoneVideoPlayer::TPlayMode ConvertVideoRingingType(
            TProfileRingingType aRingingType );


       /**
        * Plays default L1 or L2 video ring tone asynchronously.
        * @since Series 60 3.1
        */
        void PlayDefaultVideoAsync();

        /**
        * Deletes video player and plays default L1 or L2
        * tone asynchronously.
        * @since Series 60 3.1
        */
        void DeleteVideoPlayerAndPlayDefaultAsync();
        
        /**
        * Mutes the ringing tone.
        */
        void DoMuteRingingTone();
        
        /**
        * Play audio ring tone 
        */
        void PlayAudioRingTone( 
            TInt aVolume, 
            TProfileRingingType aRingingType );
        

    private:    // Data

        // Audio/Video tone
        CPhoneRingingTone* iAudioVideoRingingTone;

        // Volume for backup tone playing.
        TInt iVolume;

        // Ringing type for backup tone playing.
        TProfileRingingType iRingingType;

        // Video ringing tone player
        MPhoneVideoPlayer* iVideoPlayer;

        // Pointer to Mediator Sender
        CPhoneMediatorSender* iMediatorSender;

        // Active Cover UI command EPhoneCoverUiShowMultimediaRingingTone
        TBool iActiveCoverUICommand;
        
        // Flag. Indicates video scaling capability of the device.
        TBool iArbitraryVideoScaling;
        
        CPhoneRingingtonePlayer* iRingingtonePlayer;
        
        CPhoneTTSPlayer* iTTSPlayer; 

    };

#endif      // CPHONERINGINGTONECONTROLLER_H
            
// End of File
