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
* Description:   This class provides audio playback for Alarm UI.
*
*/



#ifndef ALMALARMPLAYER_H
#define ALMALARMPLAYER_H


//  INCLUDES
#include <mdaaudiotoneplayer.h>
#include <mdaaudiosampleplayer.h>


// FORWARD DECLARATIONS
class CAlarmUtils;


// CLASS DECLARATION

/**
*  Audio player for Alarm UI.
*  This class reads alarm tone settings (volume, ringing type, etc.) and
*  starts playing the alarm tone.
*
*  @lib AknAlarmService.lib
*  @since 1.0
*/
NONSHARABLE_CLASS( CAlmAlarmPlayer ) : public CBase, 
                                       public MMdaAudioToneObserver, 
                                       private MMdaAudioPlayerCallback
{
 public:  // Constructors and destructor
    // Symbian two-phased constructor
    static CAlmAlarmPlayer* NewL(CAlarmUtils* aAlarmUtils);

    // Destructor
    ~CAlmAlarmPlayer();

 private:
    // C++ constructor
    CAlmAlarmPlayer(CAlarmUtils* aAlarmUtils);

    // Symbian 2nd phase constructor.
    void ConstructL();

    /**
    * Play appropriate alarm type using profile ringing type setting.
    * @param aIsRNGTone ETrue, if alarm tone is a RNG file.
    * @since 3.0
    */
    void PlayCalendarAlarmL(TBool aIsRngTone);

    /**
    * Play silent calendar alarm. (needed to for vibra)
    * @since 2.1
    */
    void PlaySilentCalendarAlarmL();

    /**
    * Play normal clock alarm.
    * @since 2,0
    */
    void PlayClockAlarmL();

    /**
    * Play calendar alarm tone. (KRngMimeType files only)
    * @since 2.0
    */
    void PlayCalendarAlarmToneL();

    /**
    * Play calendar alarm tone. (KRngMimeType files only)
    * @since 2,0
    */
    void PlayClockAlarmToneL();

    /**
    * Play beep alarm for calendar.
    * @since 2,0
    */
    void PlayCalendarBeepL();

    /**
    * Play beep alarm for clock.
    * @since 2,0
    */
    void PlayClockBeepL();

    /**
    * Check the file and alarm type and try to play the alarm.
    * @since 3.0
    */
    void PlayAlarmL();

    /**
    * Common fallback functionality for prepare/play errors.
    * @since 3.0
    * @param aError Error id.
    */
    void HandlePlayErrorL(TInt aError);

    /**
    * Calculate player volume.
    * @since s60 3.2
    * @param aPlayerMaxVol Maximum volume for the player.
    * @param aVol Requested profile volume.
    * @return Adjusted player volume.
    */
    TInt PlayerVolume(TInt aPlayerMaxVol, TInt aVol);

 private: // from MMdaAudioToneObserver
    /**
    * From MMdaAudioToneObserver. Called after tone has been initialized.
    * @since 1.0
    * @param aError Error code
    */
    void MatoPrepareComplete(TInt aError);

    /**
    * From MMdaAudioToneObserver. Called after tone has been played.
    * @since 1.0
    * @param aError Error code
    */
    void MatoPlayComplete(TInt aError);

 private: // from MMdaAudioPlayerCallback
    /**
    * From MMdaAudioPlayerCallback. Called after sample initialization is complete
    * @since 2.0
    * @param aError Error code
    * @param aDuration Duration of the sample
    */
    void MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration);

    /**
    * From MMdaAudioPlayerCallback. Called after sample has been played.
    * @since 2.0
    * @param aError Error code
    */
    void MapcPlayComplete(TInt aError);

 private:
    // Audio tone player
    CMdaAudioToneUtility* iSequencePlayer;

    // Beep tone player
    CMdaAudioToneUtility* iBeepPlayer;

    // Audio sample player
    CMdaAudioPlayerUtility* iSamplePlayer;

    /**
    * Pointer to alarm utils class.
    * Now own.
    */
    CAlarmUtils* iAlarmUtils;

};

#endif  // ALMALARMPLAYER_H


// End of File
