/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <cxesoundplayersymbian.h>
#include <AudioPreference.h>
#include <ProfileEngineSDKCRKeys.h>
#include "cxutils.h"
#include "cxecameradevicecontrolsymbian.h"
#include "cxestate.h"
#include "cxesettings.h"


const TUint KCxeAudioPriority = KAudioPriorityCameraTone;

_LIT(KCxeCaptureSound,    "z:\\system\\sounds\\digital\\capture.wav");
_LIT(KCxeVideoStartSound, "z:\\system\\sounds\\digital\\videoStart.wav");
_LIT(KCxeVideoStopSound,  "z:\\system\\sounds\\digital\\videoStop.wav");
_LIT(KCxeAutoFocusSound,  "z:\\system\\sounds\\digital\\autoFocus.wav");


CxeSoundPlayerSymbian::CxeSoundPlayerSymbian(CaptureSound soundId,
    CxeSettings &settings) :
    CxeStateMachine("CxeSoundPlayerSymbian"),
    mAudioPlayer(NULL),
    mSoundId(soundId),
    mUseSound(true),
    mCaptureSoundForced(false),
    mSettings(settings)
{
    CX_DEBUG_ENTER_FUNCTION();

    checkCaptureSoundSettings();

    // connect to setting changed signal in order to monitor
    // profile warning tone changes
    connect(&mSettings,
            SIGNAL(settingValueChanged(long int, unsigned long int, QVariant)),
            this, SLOT(enableSound(long int, unsigned long int, QVariant)));

    qRegisterMetaType<CxeSoundPlayerSymbian::State>();
    initializeStates();

    doOpen();
    CX_DEBUG_EXIT_FUNCTION();
}

CxeSoundPlayerSymbian::~CxeSoundPlayerSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mAudioPlayer;

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Plays the current sound file if soundplayer is in correct state. Once
 * playing is finished or there is an error playComplete signal is emitted.
 * \sa playComplete()
 */
void CxeSoundPlayerSymbian::play()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeSoundPlayerSymbian::play mSoundId: %d, state: %d", mSoundId, state()));

    // Only play the capture sound if CMdaAudioPlayerUtility is initialised
    if (state() == Ready) {

        if (mUseSound) {
            //! @todo: Define & implement what to do, when sound is already playing.
            // Important for sequence mode.
            setState(Playing);
            mAudioPlayer->Play();
        } else {
            CX_DEBUG(("CxeSoundPlayerSymbian::play capture sounds turned off"));

            // sounds are off and we don't play the sound,
            // act as the sound has been played
            emit playComplete(KErrNone);
        }

    } else {
        // Here sound loading has failed or has not finished yet.
        // Emit signal with error code.
        emit playComplete(KErrNotReady);
    }

    CX_DEBUG_EXIT_FUNCTION();
}


void CxeSoundPlayerSymbian::MapcInitComplete(TInt aStatus, const TTimeIntervalMicroSeconds &/*aDuration*/)
{
    CX_DEBUG_IN_FUNCTION();
    CX_DEBUG(("MapcInitComplete aStatus: %d", aStatus));

    if (aStatus) {
        setState(NotReady);
    } else {
        setState(Ready);
    }
}

void CxeSoundPlayerSymbian::MapcPlayComplete(TInt aStatus)
{
    CX_DEBUG_IN_FUNCTION();
    CX_DEBUG(("MapcPlayComplete aStatus: %d", aStatus));

    if (aStatus != KErrNone && aStatus != KErrInUse) {
        // An error occurred. Close and reopen sound player to be sure.
        mAudioPlayer->Close();
        setState(NotReady);
        doOpen();
    } else {
        setState(Ready);
    }

    emit playComplete(aStatus);
}

void CxeSoundPlayerSymbian::doOpen()
{
    CX_DEBUG(("Calling OpenFileL for sound %d", mSoundId));
    TInt error = KErrNone;
    const TDesC* filename = 0;
    switch (mSoundId) {
    case StillCapture:
        filename = &KCxeCaptureSound;
        break;
    case VideoCaptureStart:
        filename = &KCxeVideoStartSound;
        break;
    case VideoCaptureStop:
        filename = &KCxeVideoStopSound;
        break;
    case  AutoFocus:
        filename = &KCxeAutoFocusSound;
        break;
    default:
        // sound is not known
        mSoundId = Unknown;
        break;
    }

    if (filename) {
        if (mAudioPlayer) {
            delete mAudioPlayer;
            mAudioPlayer = 0;
        }
        TRAP(error, mAudioPlayer =
             CMdaAudioPlayerUtility::NewFilePlayerL(*filename,
                                                    *this, KCxeAudioPriority,
                                                    TMdaPriorityPreference(KAudioPrefCamera)));
        if (!error) {
            setState(Opening);
        } else {
            setState(NotReady);
        }
    } else {
        setState(NotReady);
    }
}


void CxeSoundPlayerSymbian::handleStateChanged(int newStateId, CxeError::Id error)
{
    Q_UNUSED(newStateId);
    Q_UNUSED(error);
    // No implementation needed, because state is not visible outside of this class
}

/*!
 * Returns the current sound player state.
 * \return Current state
 */
CxeSoundPlayerSymbian::State CxeSoundPlayerSymbian::state() const
{
    return static_cast<State>(stateId());
}

/*!
 * Initialize CxeStateMachine.
 */
void CxeSoundPlayerSymbian::initializeStates()
{
    // addState(id, name, allowed next states)
    addState(new CxeState(NotReady, "NotReady", Opening));
    addState(new CxeState(Opening, "Opening", NotReady | Ready));
    addState(new CxeState(Ready, "Ready", Playing | Opening | NotReady));
    addState(new CxeState(Playing, "Playing", Ready | Opening | NotReady));

    setInitialState(NotReady);
}

/*!
 * Enables or disables the capture sound. If capture sound is forced to
 * be always on, then the capture sound cannot be disabled.
 *
 * \param uid UID of the changed setting
 * \param key Key of the changed setting
 * \param value New setting value
 */
void CxeSoundPlayerSymbian::enableSound(long int uid, unsigned long int key, QVariant value)
{

    // selftimer is only interested in warning tones
    if (uid == KCRUidProfileEngine.iUid && key == KProEngActiveWarningTones) {
        CX_DEBUG_IN_FUNCTION();
        // possible values are:
        // 0 -> warning tones off
        // 1 -> warning tones on
        bool warningTonesEnabled = (value.toInt() == 1);

        // update sound
        mUseSound = mCaptureSoundForced || warningTonesEnabled;
    }

    CX_DEBUG(("CxeSoundPlayerSymbian::enableSound <> Use sound [%d]",
               mUseSound));
}

/*!
 * Checks the initial capture sound settings based on profile warning tones
 * and whether the capture sound is forced to be always on. Connects settings
 * signal for capture sounds changes to enableCaptureSound slot.
 *
 * \sa enableCaptureSound()
 */
void CxeSoundPlayerSymbian::checkCaptureSoundSettings()
{

    CX_DEBUG_ENTER_FUNCTION();

    QVariant value(0);

    // get current profile setting for using camerasound
    // camera sound follows warning tone setting
    unsigned long int key = KProEngActiveWarningTones;
    long int uid = KCRUidProfileEngine.iUid;
    mSettings.get(uid, key, Cxe::Repository, value);

    // possible values are:
    // 0 -> warning tones off
    // 1 -> warning tones on
    bool warningTonesEnabled = (value.toInt() == 1);
    CX_DEBUG(("Warning tones enabled [%d]", value.toInt()));

    // check whether capture sound is forced or not
    mCaptureSoundForced = mSettings.get<bool>(CxeSettingIds::CAPTURE_SOUND_ALWAYS_ON, false);
    CX_DEBUG(("Capture sound forced [%d]", mCaptureSoundForced));

    // use sound if forced on or warningtones are enabled
    mUseSound = mCaptureSoundForced || warningTonesEnabled;
    CX_DEBUG(("Use sound [%d]", mUseSound));

    CX_DEBUG_EXIT_FUNCTION();
}


// end of file

