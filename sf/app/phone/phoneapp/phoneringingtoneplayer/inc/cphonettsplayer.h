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

#ifndef CPHONETTSPLAYER_H_
#define CPHONETTSPLAYER_H_

// System includes
#include <e32base.h>
#include <Profile.hrh>

// User includes
#include "cphonetimer.h"
#include "cphoneaudioplayer.h"
#include "mphoneaudioplayerobserver.h"

// Forward declarations
class CPhoneRingingTone;
class CPhoneRingingtonePlayer;

NONSHARABLE_CLASS(CPhoneTTSPlayer): public CBase,
                        public MPhoneAudioPlayerObserver
                       
    {
public:
    CPhoneTTSPlayer(CPhoneRingingtonePlayer* aRingingtonePlayer);
    virtual ~CPhoneTTSPlayer();
    
    /**
    * Two-phased constructor.
    * @return new instance.
    */
     static CPhoneTTSPlayer* NewL(CPhoneRingingtonePlayer* aRingingtonePlayer);
    
     
public:
    
    /**
    * Play TTS tone simultaneously with Personal or Default tones.
    * This method does not command phone to play TTS immediately. Instead 
    * it registers a request to play TTS, when Personal or default tones 
    * are played.
    *
    * @since Series 60 3.0
    * @param aTextToSay Text, that should be pronounced by the 
    * Text-To-Speech engine. This is normal descriptor, no UTF-8 
    * and no "(tts)" prefix
    */
    void PlayTtsTone(
        const TDesC& aTextToSay,
        CPhoneRingingTone* aAudioVideoRingingTone );
    
    void StopPlaying();
   
    
    /**
    * Checks if TTS component should be played for the current ringtone 
    * playback and initializes the TTS playback. This method is to be 
    * called from within PlayXXXXTone methods
    * @since Series 60 3.0
    * @param aRingingType ringing type
    */
    void AddTtsPlaybackIfNeeded();
    
    
private: // from MPhoneAudioPlayerObserver
    /**
    * Handle the error of audio file player.
    */
    virtual void HandleAudioPlayerError( 
        TPhoneAudioPlayerErrorEvent aEvent, 
        TInt aError, 
        TInt aId = KPhoneAudioPlayerNotSpecified );

    /**
    * Handle audio player initialize complete.
    */
    virtual void HandleAudioPlayerInitComplete(
        TInt aId = KPhoneAudioPlayerNotSpecified );

    /**
    * Handle auido file playing complete successfully.
    */
    virtual void HandlePlayingComplete( 
        TInt aId = KPhoneAudioPlayerNotSpecified );  
    
private:
    
    /**
    * Enumerates how many times TTs is played
    * ESaidOnce -once
    * ESaidTwice twice 
    */    
    enum TTtsStatus
        {
        ESaidOnce,
        ESaidTwice
        };
    
    /**
    * Wrapper of DoHandleTtsDelayTimeout, that can be used as TCallback. 
    * Immediately passes control to DoHandleTtsDelayTimeout method.
    * @since Series 60 3.0
    * @param object Object to call DoHandleTtsDelayTimeout() on, . 
    *        instance of CPhoneRingingTonePlayer.
    * @return KErrNone.
    */
    static TInt HandleTtsDelayTimeout( TAny* object );
    
    /**
    * It is called after TTS delay elapsed. Starts playing TTS.
    */
    void DoHandleTtsDelayTimeout();
         
    /**
    * Returns currently active player which is used with TTS player.
    * @return pointer to CPhoneAudioPlayer.
    */
    CPhoneAudioPlayer* GetCurrentlyActiveAudioPlayerWithTTs();   
     
    /**
    * TTs cases only. Calculates new volume and ramptimes
    * for Ascending TTS ringing case and normal TTS ringing case.
    * @param aStatus -status of TTs player.
    */         
    void SolveNewVolumeAndRamptime( TTtsStatus aStatus );   
    
    /**
    * Retarts currently active ringingtone player.
    */
    void ReStartRingingTonePlayer();
    
    /**
    * Convert ringing type from PhoneAppEnging to the ringing type in 
    * CPhoneAudioPlayer.
    * @param aRingingType the ringing type form PhoneAppEngine.
    * @return ringing type of CPhoneAudioPlayer.
    */
    CPhoneAudioPlayer::TRingingType ConvertRingingType(
        TProfileRingingType aRingingType );    
        
private:
    
    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL();
    
private: 
    
    // Tts player.
    CPhoneAudioPlayer* iTtsPlayer;
    
    // True if Tts is currently playing. Is different from 
    // iTonePlayingStatus, because TTS is played in
    // parallel with the other tone types
    TBool iTtsTonePlaying;
    
    // Index of the TTS delay that is "executed" currently or is to be 
    // "executed" after the current playing of the TTS tone. Is zero based
    TInt iTtsDelayIndex;
    
    // True if TTS tone should be played when the next playing of the 
    // default or personal tone is fired.
    TBool iTtsToneToBePlayed;
    
    // Timer, that tracks delays before playing TTS
    CPhoneTimer* iTtsDelayTimer;

    // Number of times TTS tone should be played for the CURRENT ringtone
    // If ringing tone is to be played just once, iTtsDelayCount == 1,
    // otherwise iTtsDelayCount == KTtsDelayCount
    TInt iTtsDelaysCount;
    
    // Counter for TTs timeouts
    TInt iTTsTimeOutCounter;
    
    // RingingType for TtsPlayer only.
    TProfileRingingType iTtsRingingType;
    
    // Volume of the text pronounced by the TTS engine
    TInt iTtsVolume;
    
    CPhoneRingingtonePlayer* iRingingtonePlayer;
    };

#endif /* CPHONETTSPLAYER_H_ */
