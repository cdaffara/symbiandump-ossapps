/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides audio playback for Alarm UI.
*
*/



// INCLUDE FILES
#include "AlmAlarmPlayer.h"
#include "alarmutils.h"
#include "pim_trace.h"

#include <AudioPreference.h>

// CONSTANTS AND MACROS
const TInt KSoundInterval( 1000000 );  // 1 second

// set player volume
#define SET_VOLUME( t, v )  t->SetVolume( v );

// Copied from Phone's Toneplayer.
_LIT8( KPhoneBeepSequence, "\x00\x11\x06\x0A\x08\x73\x0A\x40\x28\x0A\xF7\x05\xFC\x40\x64\x0A\x08\x40\x32\x0A\xF7\x06\x0B" );
_LIT8( KPhoneNoSoundSequence, "\x00\x11\x06\x05\xFC\x0A\x08\x40\x32\x0A\xF7\x40\x64\x06\x0B" );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor.
// -----------------------------------------------------------------------------
//
CAlmAlarmPlayer::CAlmAlarmPlayer(CAlarmUtils* aAlarmUtils)
 : iAlarmUtils( aAlarmUtils )
{
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::ConstructL(void)
{
    TRACE_ENTRY_POINT;
    PlayAlarmL();
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAlmAlarmPlayer* CAlmAlarmPlayer::NewL(CAlarmUtils* aAlarmUtils)
{ // static
    TRACE_ENTRY_POINT;
    CAlmAlarmPlayer* self = new( ELeave )CAlmAlarmPlayer( aAlarmUtils );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_EXIT_POINT;
    return self;
}

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CAlmAlarmPlayer::~CAlmAlarmPlayer(void)
{
    TRACE_ENTRY_POINT;

    if( iSequencePlayer )
    {
        if( iSequencePlayer->State() == EMdaAudioToneUtilityPlaying )
        {
            iSequencePlayer->CancelPlay();
        }
        else
        {
            iSequencePlayer->CancelPrepare();
        }
        delete iSequencePlayer;
    }

    if( iSamplePlayer )
    {
        iSamplePlayer->Stop();
        iSamplePlayer->Close();
        delete iSamplePlayer;
    }

    if( iBeepPlayer )
    {
        if( iBeepPlayer->State() == EMdaAudioToneUtilityPlaying )
        {
            iBeepPlayer->CancelPlay();
        }
        else
        {
            iBeepPlayer->CancelPrepare();
        }
        delete iBeepPlayer;
    }
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Once tone initialization is complete play the tone.
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::MatoPrepareComplete(TInt aError)
{
    TRACE_ENTRY_POINT;
    if( aError != KErrNone )
    {
        PIM_TRAPD_ASSERT( HandlePlayErrorL( aError ); )
    }
    else if( iBeepPlayer )
    {
        iBeepPlayer->Play();
    }
    else
    {
        iSequencePlayer->Play();
    }
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Callback for completing tone player.
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::MatoPlayComplete(TInt aError)
{
    TRACE_ENTRY_POINT;
    if( aError != KErrNone )
    {
        PIM_TRAPD_ASSERT( HandlePlayErrorL( aError ); )
    }
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// When initialization is complete set the player settings and play the sound.
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
{
    TRACE_ENTRY_POINT;
    if( aError == KErrNone )
    {
        iSamplePlayer->SetVolumeRamp( TTimeIntervalMicroSeconds( iAlarmUtils->AlarmData().iVolumeRampTime ) );

        iSamplePlayer->SetRepeats( iAlarmUtils->AlarmData().iRepeatValue, 
                                   TTimeIntervalMicroSeconds( KSoundInterval ) );

        SET_VOLUME( iSamplePlayer, 
                    PlayerVolume( iSamplePlayer->MaxVolume(),
                                  iAlarmUtils->AlarmData().iVolume ) )

        iSamplePlayer->Play();
    }
    else
    {
        PIM_TRAPD_ASSERT( HandlePlayErrorL( aError ); )
    }
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Callback for completing audio player.
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::MapcPlayComplete(TInt aError)
{
    TRACE_ENTRY_POINT;
    if( aError != KErrNone )
    {
        PIM_TRAPD_ASSERT( HandlePlayErrorL( aError ); )
    }
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Play appropriate alarm type using profile ringing type setting.
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::PlayCalendarAlarmL(TBool aIsRngTone)
{
    TRACE_ENTRY_POINT;
    if( iAlarmUtils->AlarmData().iRingType == EProfileRingingTypeBeepOnce )
    {
        PlayCalendarBeepL();
    }
    else if( iAlarmUtils->AlarmData().iRingType == EProfileRingingTypeSilent )
    {
        PlaySilentCalendarAlarmL();
    }
    else
    {
        if( aIsRngTone )
        {
            PlayCalendarAlarmToneL();
        }
        else // normal sound file
        {
            if( iSamplePlayer )
            {
                iSamplePlayer->Close();
                delete iSamplePlayer;
                iSamplePlayer = NULL;
            }
            iSamplePlayer = CMdaAudioPlayerUtility::NewFilePlayerL(
                iAlarmUtils->AlarmData().iAlarmTone,
                *this, 
                KAudioPriorityAlarm, 
                static_cast<TMdaPriorityPreference>( KAudioPrefCalendarAlarm ) );
        }
    }
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Play calendar alarm tone. (KRngMimeType files only)
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::PlayCalendarAlarmToneL(void)
{
    TRACE_ENTRY_POINT;
    if( iSequencePlayer )
    {
        delete iSequencePlayer;
        iSequencePlayer = NULL;
    }
    iSequencePlayer = CMdaAudioToneUtility::NewL( *this );

    iSequencePlayer->SetPriority( KAudioPriorityAlarm, static_cast<TMdaPriorityPreference>( KAudioPrefCalendarAlarm ) );

    iSequencePlayer->SetRepeats( iAlarmUtils->AlarmData().iRepeatValue, 
                                 TTimeIntervalMicroSeconds( KSoundInterval ) );

    SET_VOLUME( iSequencePlayer, 
                PlayerVolume( iSequencePlayer->MaxVolume(),
                              iAlarmUtils->AlarmData().iVolume ) )

    iSequencePlayer->SetVolumeRamp( TTimeIntervalMicroSeconds( iAlarmUtils->AlarmData().iVolumeRampTime ) );

    iSequencePlayer->PrepareToPlayFileSequence( iAlarmUtils->AlarmData().iAlarmTone );
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Play silent calendar alarm. (needed to for vibra)
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::PlaySilentCalendarAlarmL(void)
{
    TRACE_ENTRY_POINT;
    iBeepPlayer = CMdaAudioToneUtility::NewL( *this );

    SET_VOLUME( iBeepPlayer, 
                PlayerVolume( iBeepPlayer->MaxVolume(),
                              iAlarmUtils->AlarmData().iVolume ) )

    iBeepPlayer->SetPriority( KAudioPriorityAlarm, static_cast<TMdaPriorityPreference>( KAudioPrefCalendarAlarm ) );

    iBeepPlayer->PrepareToPlayDesSequence( KPhoneNoSoundSequence );
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Play beep alarm for calendar.
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::PlayCalendarBeepL(void)
{
    TRACE_ENTRY_POINT;
    iBeepPlayer = CMdaAudioToneUtility::NewL( *this );

    SET_VOLUME( iBeepPlayer, 
                PlayerVolume( iBeepPlayer->MaxVolume(),
                              iAlarmUtils->AlarmData().iVolume ) )

    iBeepPlayer->SetPriority( KAudioPriorityAlarm, static_cast<TMdaPriorityPreference>( KAudioPrefCalendarAlarm ) );

    iBeepPlayer->PrepareToPlayDesSequence( KPhoneBeepSequence );
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Play beep alarm for clock.
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::PlayClockBeepL(void)
{
    /*TRACE_ENTRY_POINT;
    iBeepPlayer = CMdaAudioToneUtility::NewL( *this );

    SET_VOLUME( iBeepPlayer, 
                PlayerVolume( iBeepPlayer->MaxVolume(),
                              iAlarmUtils->AlarmData().iVolume ) )

    iBeepPlayer->SetPriority( KAudioPriorityAlarm, static_cast<TMdaPriorityPreference>( KAudioPrefAlarmClock ) );

    iBeepPlayer->PrepareToPlayDesSequence( KPhoneBeepSequence );*/
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Play normal clock alarm.
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::PlayClockAlarmL(void)
{
    TRACE_ENTRY_POINT;
    if( iSamplePlayer )
    {
        iSamplePlayer->Close();
        delete iSamplePlayer;
        iSamplePlayer = NULL;
    }
    iSamplePlayer = CMdaAudioPlayerUtility::NewFilePlayerL(
        iAlarmUtils->AlarmData().iAlarmTone, 
        *this, 
        KAudioPriorityAlarm, 
        static_cast<TMdaPriorityPreference>( KAudioPrefAlarmClock ) );
    TRACE_EXIT_POINT;

}

// -----------------------------------------------------------------------------
// Play calendar alarm tone. (KRngMimeType files only)
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::PlayClockAlarmToneL(void)
{
    TRACE_ENTRY_POINT;
    if( iSequencePlayer )
    {
        delete iSequencePlayer;
        iSequencePlayer = NULL;
    }
    iSequencePlayer = CMdaAudioToneUtility::NewL( *this );

    iSequencePlayer->SetPriority( KAudioPriorityAlarm, static_cast<TMdaPriorityPreference>( KAudioPrefAlarmClock ) );

    iSequencePlayer->SetRepeats( iAlarmUtils->AlarmData().iRepeatValue, 
                                 TTimeIntervalMicroSeconds( KSoundInterval ) );

    SET_VOLUME( iSequencePlayer, 
                PlayerVolume( iSequencePlayer->MaxVolume(),
                              iAlarmUtils->AlarmData().iVolume ) )

    iSequencePlayer->SetVolumeRamp( TTimeIntervalMicroSeconds( iAlarmUtils->AlarmData().iVolumeRampTime ) );

    iSequencePlayer->PrepareToPlayFileSequence( iAlarmUtils->AlarmData().iAlarmTone );
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Check the file and alarm type and try to play the alarm.
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::PlayAlarmL()
{
    TRACE_ENTRY_POINT;
    if( iAlarmUtils->AlarmData().iAlarmTone.Length() )
    {
        const TBool isRngTone( iAlarmUtils->IsRngTone() );

        if( iAlarmUtils->IsClockAlarm() )
        {
            if( !isRngTone )
            {
                PlayClockAlarmL();
            }
            else
            {
                PlayClockAlarmToneL();
            }
        }
        else
        {
            PlayCalendarAlarmL( isRngTone );
        }
    }
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Common fallback functionality for prepare/play errors.
// -----------------------------------------------------------------------------
//
void CAlmAlarmPlayer::HandlePlayErrorL(TInt aError)
{
    TRACE_ENTRY_POINT;
    /**
    * No fallback for KErrUnderflow - alarm got replaced with the default beep.
    * Also, nothing to do if iBeepPlayer was already used.
    **/
    if( aError != KErrUnderflow && !iBeepPlayer )
    {
        // fallback to beep tone on OOM or if the default tone was already in use
        if( aError == KErrNoMemory || 
            iAlarmUtils->AlarmData().iAlarmTone == iAlarmUtils->AlarmData().iDefaultAlarmTone )
        {
            delete iSequencePlayer;
            iSequencePlayer = NULL;

            if( iSamplePlayer )
            {
                iSamplePlayer->Close();
            }
            delete iSamplePlayer;
            iSamplePlayer = NULL;

            // try playing the beep tone
            if( iAlarmUtils->IsClockAlarm() )
            {
                PlayClockBeepL();
            }
            else
            {
                PlayCalendarBeepL();
            }
        }
        else // retry using the default tone
        {
            iAlarmUtils->AlarmData().iAlarmTone = iAlarmUtils->AlarmData().iDefaultAlarmTone;
            PlayAlarmL();
        }
    }
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Set volume between zero and player's maximum volume
// -----------------------------------------------------------------------------
//
TInt CAlmAlarmPlayer::PlayerVolume(TInt aPlayerMaxVol, TInt aVol)
{
    TRACE_ENTRY_POINT;
    const TInt profileMaxVol( iAlarmUtils->MaxProfileVolume() );
    TInt vol( 0 ); // leave as 0 if using off tone
    if( !iAlarmUtils->IsOffTone() )
    {
        // calculate value between 0 and aPlayerMaxVolume
        vol = Max( 0, Min( aPlayerMaxVol / profileMaxVol * aVol, aPlayerMaxVol ) );
    }
    TRACE_EXIT_POINT;
    return vol;
}


// End of File
