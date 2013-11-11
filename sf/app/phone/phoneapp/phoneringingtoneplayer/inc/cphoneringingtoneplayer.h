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

#ifndef CPHONERINGINGTONEPLAYER_H_
#define CPHONERINGINGTONEPLAYER_H_

// System includes
#include <e32base.h>
#include <Profile.hrh>

// User includes
#include "mphoneaudioplayerobserver.h"
#include "cphoneaudioplayer.h"
#include "mphonevideoplayer.h"
#include "mphonecenrepobserver.h"
#include "mphonetimer.h"

// Forward declarations
class CPhoneTimer;
class CPhoneRingingTone;


NONSHARABLE_CLASS(CPhoneRingingtonePlayer) : public CActive,
                                             public MPhoneAudioPlayerObserver,
                                             public MPhoneCenRepObserver,
                                             private MPhoneTimer
    {
public:
    CPhoneRingingtonePlayer();
    virtual ~CPhoneRingingtonePlayer();
    
    
    /**
    * Two-phased constructor.
    * @return new instance.
    */
     static CPhoneRingingtonePlayer* NewL();
    
     /**
      * Playing ringing tone based on profile.
      * @param aAudioVideoRingingTone
      */     
     void PlayProfileBasedTone( CPhoneRingingTone* aAudioVideoRingingTone );   
     
    /**
     * Playing the Beep once.
     * @param aVolume volume used to play the ringing tone.
     */
     void PlayBeepOnce( TInt aVolume );

     /**
     * Play silent tone. No_sound.wav will be played. If vibration alert
     * is ON, it will vibrate.
     */
     void PlaySilentTone();

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
     * Play default tone.
     * @param aVolume volume used for the playing.
     * @param aRingingType ringing type.
     */
     void PlayDefaultTone( 
         TInt aVolume, 
         TProfileRingingType aRingingType );

     /**
     * Play backup tone.
     * @param aVolume volume used for the playing.
     * @param aRingingType ringing type.
     */
     void PlayBackupTone( 
         TInt aVolume, 
         TProfileRingingType aRingingType );
     
     /**
     * Stops playing the ringing tone.
     * Method does not do anything if ringing tone is not playing.
     */
     void StopPlaying();
     
     /** 
     * Enumerates identifiers for each player:
     * EPlayerAudio - audio
     * EPlayerBeepOnce - beep once
     * EPlayerSilent - silent
     * EPlayerDefault - default
     * EPlayerBackup - backup
     *
     * EPlayerFirst and EPlayerLast are alias
     * to other identifiers.
     * EPlayerFirst - first player
     * EPlayerLast - last player
     */
     enum TPlayerId
         {
         EPlayerFirst,
         EPlayerAudio = EPlayerFirst,
         EPlayerBeepOnce,
         EPlayerSilent,
         EPlayerDefault,
         EPlayerBackup,
         EPlayerTts, 
         EPlayerLast = EPlayerTts,
         EPlayerCount
         };     
     
     CPhoneAudioPlayer* GetCurrentlyActiveAudioPlayer();
     
     void MuteActiveAudioPlayer();
   
public: // From MPhoneCenRepObserver

    /**
    * Handle the change of the setting from Central Repository
    * @param aUid identifing the central repository UID.
    * @param aId central repository ID.
    */
    virtual void HandleCenRepChangeL( 
        const TUid& aUid,
        const TUint aId );

   
     
private:
    
    /**
    * By default Symbian OS constructor is private.
    */
    void ConstructL();

private: // CActive
    
    /**
    * @see CActive::RunL.
    */
    void RunL();

    /**
    * @see CActive::DoCancel.
    */
    void DoCancel();
    
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
    * @see MPhoneTimer
    */
    virtual void HandleTimeOutL();
    
private:    
    

    // Indicate which tone is playing or need to be played:
    // EIdle - idle,
    // EAudioTonePlaying - audio playing,
    // EBeepOnce - beep once playing,
    // ESilentTonePlaying - silent tone playing,
    // EDefaultTonePlaying - default tone playing,
    // EBackupTonePlaying - backup tone playing,
    // EVideoTonePlaying - playing video ringing tone
    // ESilentVideoTonePlaying  - silent playing video ringing tone
    // EPersonalVideoTonePlaying - personal playing video ringing tone
    enum TTonePlayingStatus
        {
        EIdle,
        EAudioTonePlaying,
        EBeepOnce,
        ESilentTonePlaying,
        EDefaultTonePlaying,
        EBackupTonePlaying,
        EVideoTonePlaying,
        ESilentVideoTonePlaying,
        EPersonalVideoTonePlaying
        };
    
    // States for this active object
    enum TState
        {
        EIdleState,  
        EDeletingAudioPlayer,
        EDeletingVideoPlayer,
        EPlayingDefaultVideo
        };
    
    /**
    * Deletes player asynchronoysly.
    * @param aPlayer player to be deleted.
    */
    void DeletePlayerAsync(
        TPlayerId aPlayer );
    
    /**
    * Construct sequence players.
    * @param aPlayer player to be constructed.
    */
    void ConstructSequencePlayer(
        TPlayerId aPlayer );

    /**
    * Construct sequence players.
    * @param aPlayer player to be constructed.
    */
    void ConstructSequencePlayerL(
        TPlayerId aPlayer );     
    
    /**
    * Construct backup tone player.
    */
    TInt ConstructBackupPlayer();
    

    /**
    * Construct media server and backup tone player.
    */
    void ConstructBackupPlayerL();
    
    /**
    * Default ringingtone from profile engine.
    */
    void ConstructDefaultPlayerL();
      
    /**
    * Do construct tone.
    * @param aRingingTone Ringing tone.
    * @return The instance of the player. NULL is returned if it's failed.
    */
    CPhoneAudioPlayer* ConstructTonePlayer( 
        const CPhoneRingingTone& aRingingTone,
        TInt aId );
    
    /**
    * Convert ringing type from PhoneAppEnging to the ringing type in 
    * CPhoneAudioPlayer.
    * @param aRingingType the ringing type form PhoneAppEngine.
    * @return ringing type of CPhoneAudioPlayer.
    */
    CPhoneAudioPlayer::TRingingType ConvertRingingType(
        TProfileRingingType aRingingType );

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
    * Cleanup players.
    */
    void CleanupPlayers();   
    
    /**
    * Handle audio player error.
    * @param aDelete ETrue if deletion of the player allowed, 
    *                EFalse otherwise.
    * @param aSync ETrue if synchronous deletion of players is allowed,
    *              EFalse otherwise.
    */
    void DoHandlePlayerError(
        TBool aDelete,
        TBool aSync );
    

private:    // Data
    
    
    // State of this active object
    TState iState;
    
    // Player to be deleted asynchronously.
    RPointerArray<CPhoneAudioPlayer> iAsyncDeletePlayers;
    
    // Current playing status.
    TTonePlayingStatus iTonePlayingStatus;
    
    // Voice call ringing tone file size max value.
    TInt iToneFileSizeLimitKB;   
    
    // DRM extend security
    TBool iExtSecNeeded;
    
    // Volume for backup tone playing.
    TInt iVolume;
    
    // Ringing type for backup tone playing.
    TProfileRingingType iRingingType;
    
    // Audio ring tone player. Memory allocated during construction.
    // After constructed, no leave may be caused during audio playing
    CPhoneAudioPlayer* iAudioPlayer;

    // Audio player for beep once.
    CPhoneAudioPlayer* iBeepOncePlayer;

    // Audio player for silent tone.
    CPhoneAudioPlayer* iSilentPlayer;

    // Default player.
    CPhoneAudioPlayer* iDefaultPlayer;
    
    // Tts player.
    CPhoneAudioPlayer* iTtsPlayer;

    // Backup ringing tone. Incase all default player failed.
    CPhoneAudioPlayer* iBackupPlayer;
    
    // Default player
    CPhoneRingingTone* iDefaultRingingTone;     
    
    // Media server.
    CMdaServer* iMdaServer;
    
    // Timer.
    CPhoneTimer* iTimer;
    
    // Audio/Video tone
    CPhoneRingingTone* iAudioVideoRingingTone;
    };

#endif /* CPHONERINGINGTONEPLAYER_H_ */
