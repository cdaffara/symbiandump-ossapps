/*
* Copyright (c) 2003, 2004, 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plays snap, self-timer and video start/stop sounds.
*
*/


// INCLUDES
#include <videotelui.rsg>

#include <avkon.rsg>

#include <AudioPreference.h>
#include <StringLoader.h>

#include <CVtLogger.h>

#include "cvtengr2ftoneplayer.h"

// CONSTANTS

// FORWARD DECLARATIONS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CVtEngR2FTonePlayer::CVtEngR2FTonePlayer
// C++ constructor
// ----------------------------------------------------------
//
CVtEngR2FTonePlayer::CVtEngR2FTonePlayer()
    {
    }


// ---------------------------------------------------------------------------
// CVtEngR2FTonePlayer::NewL
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CVtEngR2FTonePlayer* CVtEngR2FTonePlayer::NewL()
    {
    CVtEngR2FTonePlayer* self = new ( ELeave ) CVtEngR2FTonePlayer();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// ----------------------------------------------------------
// CVtEngR2FTonePlayer::ConstructL
// Symbian OS 2nd phase constructor
// ----------------------------------------------------------
//
void CVtEngR2FTonePlayer::ConstructL()
    {
    __VTPRINTENTER( "CVtEngR2FTonePlayer.ConstructL" )
    iAudioPlayer = CMdaAudioPlayerUtility::NewL( *this,
        KAudioPriorityVideoRecording, 
        TMdaPriorityPreference( KAudioPrefCamera ) );
	
    iVideoStartSound = StringLoader::LoadL( R_VIDEOTELUI_VIDEO_RECORD_START_SOUND_PATH );
    iVideoStopSound = StringLoader::LoadL( R_VIDEOTELUI_VIDEO_RECORD_STOP_SOUND_PATH );
    iSoundInProgress = EFalse;
    iOpenFileInProgress = EFalse;
    __VTPRINTEXIT( "CVtEngR2FTonePlayer.ConstructL" )
    }


// Destructor.
CVtEngR2FTonePlayer::~CVtEngR2FTonePlayer()
    {
    if ( iAudioPlayer )
        {
        iAudioPlayer->Close();
        delete iAudioPlayer;
        }

    delete iVideoStartSound;
    delete iVideoStopSound;
    }


// ---------------------------------------------------------------------------
// CVtEngR2FTonePlayer::PlaySound
// Play a sound with given id.
// ---------------------------------------------------------------------------
//
TInt CVtEngR2FTonePlayer::PlaySound( TInt aSound )
    {
    __VTPRINTENTER( "CVtEngR2FTonePlayer.PlaySound" )
	TInt err = KErrNone;
    iAudioPlayer->Stop();
    iAudioPlayer->Close();
   
    switch ( aSound )
        {
        case EVtR2FVideoStartSoundId:
        case EVtR2FVideoStopSoundId:
            {
            __VTPRINT( DEBUG_GEN,"CVtEngR2FTonePlayer.PlaySound() user sound" )
            
            // Set correct sound file
            TPtrC soundFile = *iVideoStartSound;
            if ( aSound == EVtR2FVideoStopSoundId )
                {
                soundFile.Set( *iVideoStopSound );
                }
                
            // Video recording start sound using MMF
            if ( !iOpenFileInProgress && !iSoundInProgress )
                {
                TRAP( err, iAudioPlayer->OpenFileL( soundFile ) );
                if ( !err )
                    {
                    __VTPRINT( DEBUG_GEN,"CVtEngR2FTonePlayer.PlaySound() open now in progress" )
                    iOpenFileInProgress = ETrue;
                    }
                }
            else
                {
                __VTPRINT( DEBUG_GEN, "CVtEngR2FTonePlayer.PlaySound() err, in use" )
                // The last OpenFileL call still hasnt completed.
                // Cannot start playing a new file.
                err = KErrInUse;
                }
            break;
            }

        default:
            {
            // Other sounds are not supported
			err = KErrArgument;
            break;
            }
        }
	__VTPRINTEXIT( "CVtEngR2FTonePlayer.PlaySound" )
	return err;
    }

// ---------------------------------------------------------------------------
// CVtEngR2FTonePlayer::Stop
// Stop any ongoing sound.
// ---------------------------------------------------------------------------
//
void CVtEngR2FTonePlayer::StopSound()
    {
    __VTPRINTENTER( "CVtEngR2FTonePlayer.StopSound" )
    
    iAudioPlayer->Stop();
    iAudioPlayer->Close();
    
    __VTPRINTEXIT( "CVtEngR2FTonePlayer.StopSound" )
    }

// ---------------------------------------------------------------------------
// CVtEngR2FTonePlayer::MapcInitComplete
// CMdaAudioPlayerUtility initialization complete
// ---------------------------------------------------------------------------
//
void CVtEngR2FTonePlayer::MapcInitComplete(
    TInt aError, const TTimeIntervalMicroSeconds& aDuration )
    {
    __VTPRINTENTER( "CVtEngR2FTonePlayer.MapcInitComplete" )
    __VTPRINT3( DEBUG_GEN, "CVtEngR2FTonePlayer.MapcInitComplete(%d, %d) ", 
    	aError, I64INT( aDuration.Int64()) )
    
    (void) aDuration; 

    iOpenFileInProgress = EFalse;
	
	// If tone can be played
    if ( !aError )
        {
        __VTPRINT( DEBUG_GEN, "CVtEngR2FTonePlayer.MapcInitComplete() play" )
        iSoundInProgress = ETrue;
        iAudioPlayer->Play();
        }
    // error occured during tone player init    
    else
        {
        __VTPRINT( DEBUG_GEN,"CVtEngR2FTonePlayer.MapcInitComplete() error" )
        }
    __VTPRINTEXIT( "CVtEngR2FTonePlayer.MapcInitComplete" )
    }


// ---------------------------------------------------------------------------
// CVtEngR2FTonePlayer::MapcPlayComplete
// Playback complete, notify observer
// ---------------------------------------------------------------------------
//
void CVtEngR2FTonePlayer::MapcPlayComplete( TInt aError )
    {
    __VTPRINTENTER( "CVtEngR2FTonePlayer.MapcPlayComplete" )
    __VTPRINT2( DEBUG_GEN, "CVtEngR2FTonePlayer.MapcPlayComplete(%d)", aError)
    
    (void) aError;

    iSoundInProgress = EFalse;
    __VTPRINTEXIT( "CVtEngR2FTonePlayer.MapcPlayComplete" )    
    }


// End of File
