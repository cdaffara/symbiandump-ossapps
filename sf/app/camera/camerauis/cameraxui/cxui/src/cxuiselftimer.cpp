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
#include <QGraphicsWidget>
#include <hblabel.h>
#include <hbpushbutton.h>

#ifdef Q_OS_SYMBIAN
#include <ProfileEngineSDKCRKeys.h>
#endif

#include "cxuiselftimer.h"
#include "cxutils.h"
#include "cxuienums.h"
#include "cxuidocumentloader.h"
#include "cxesettings.h"
#include "cxenamespace.h" // CxeSettingIds
#include "cxeerror.h"
#include "cxeexception.h"

using namespace CxUiLayout;

/*!
    \class CxuiSelfTimer
    \brief Class for handling selftimer countdown.

    CxuiSelfTimer class handles the selftimer countdown and updates widgets
    accordingly.
*/

// constants

const int CONTINUOUS_POSTCAPTURE    = -1;
const int UNKNOWN                   = -99;

const static QString SELFTIMER_SOUND = "z:\\system\\sounds\\digital\\selftimer.wav";

CxuiSelfTimer::CxuiSelfTimer(CxeSettings &settings)
:  mDelay(-1),
   mCounter(0),
   mTimer(this),
   mOldPostCaptureTimeOut(UNKNOWN),
   mWidgetContainer(NULL),
   mTimerLabel(NULL),
   mCancelButton(NULL),
   mStartButton(NULL),
   mSettings(settings),
   mSound(SELFTIMER_SOUND),
   mUseSound(true)
{
    CX_DEBUG_ENTER_FUNCTION();

    connect(&mTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    mTimer.setSingleShot(false);

    // connect to capture sound signal in order to monitor
    // warning tone changes
    connect(&mSettings,
            SIGNAL(settingValueChanged(long int, unsigned long int, QVariant)),
            this, SLOT(enableSound(long int, unsigned long int, QVariant)));

    // get initial warning tone value from profile
    QVariant value(0);

#ifdef Q_OS_SYMBIAN
    // get current profile setting for using camerasound
    // camera sound follows warning tone setting
    unsigned long int key = KProEngActiveWarningTones;
    long int uid = KCRUidProfileEngine.iUid;
    mSettings.get(uid, key, Cxe::Repository, value);
#endif

    // possible values are:
    // 0 -> warning tones off
    // 1 -> warning tones on
    mUseSound = (value.toInt() == 1);
    CX_DEBUG(("Warning tones enabled [%d]", value.toInt()));

    CX_DEBUG_EXIT_FUNCTION();
}

CxuiSelfTimer::~CxuiSelfTimer()
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
    Get pointers to the UI widgets from the documentloader.

    \param documentLoader Pointer to a CxuiDocumentLoader instance that has
    loaded the docml containing selftimer widgets.
 */
void CxuiSelfTimer::loadSelftimerWidgets(CxuiDocumentLoader *documentLoader) {

    if (documentLoader) {

        QGraphicsWidget *widget = NULL;
        
        widget = documentLoader->findWidget(STILL_PRE_CAPTURE_SELFTIMER_CONTAINER);
        mWidgetContainer = qobject_cast<HbWidget *>(widget);
        CX_DEBUG_ASSERT(mWidgetContainer);

        widget = documentLoader->findWidget(STILL_PRE_CAPTURE_SELFTIMER_COUNTER);
        mTimerLabel = qobject_cast<HbLabel *>(widget);
        CX_DEBUG_ASSERT(mTimerLabel);

        widget = documentLoader->findWidget(STILL_PRE_CAPTURE_SELFTIMER_CANCEL_BUTTON);
        mCancelButton = qobject_cast<HbPushButton *>(widget);
        CX_DEBUG_ASSERT(mCancelButton);

        widget = documentLoader->findWidget(STILL_PRE_CAPTURE_SELFTIMER_START_BUTTON);
        mStartButton = qobject_cast<HbPushButton *>(widget);
        CX_DEBUG_ASSERT(mStartButton);

        connect(mCancelButton, SIGNAL(released()), this, SLOT(cancel()));
        connect(mStartButton, SIGNAL(released()), this, SLOT(startTimer()));

        updateWidgets();
    }
}

/*!
    Method for checking if selftimer is enabled.
    Selftimer is enabled/disabled using slots cancel() and changeTimeout()
    \return True if selftimer is enabled

    \sa changeTimeOut(int seconds)
    \sa cancel()
 */
bool CxuiSelfTimer::isEnabled()
{
    return (mDelay >= 0);
}

/*!
    Method for checking if selftimer countdown is ongoing.
    Countdown is started with by calling startTimer().
    \return True if selftimer is active.

    \sa startTimer()
 */
bool CxuiSelfTimer::isOngoing()
{
    return mTimer.isActive();
}

/*!
   Returns current timeout value of selftimer.
 */
int CxuiSelfTimer::getTimeout() const
{
    return mDelay;
}

/*! Slot for canceling the selftimer.
    Disables selftimer, sets back the postcapturetimeout if it
    has been changed by selftimer and emits signal to notify interested
    parties that cancel has been called.

    \sa startTimer()
 */
void CxuiSelfTimer::cancel()
{
    CX_DEBUG_ENTER_FUNCTION();

    // show postcapture setting is changed when selftimer is started
    // if it has been changed, then change it back
    if (mOldPostCaptureTimeOut != UNKNOWN) {
        mSettings.set(CxeSettingIds::STILL_SHOWCAPTURED, mOldPostCaptureTimeOut);
        mOldPostCaptureTimeOut = UNKNOWN;
    }

    // Reset counter and stop any ongoing action.
    // Start button back to enabled.
    reset(false);
    // Disable self timer
    mDelay = -1;

    // Update UI components.
    updateWidgets();

    // Signal others about cancellation.
    emit cancelled();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Slot for handling timer timouts. Counter will be updated and when
    selftimer countdown is finished, timerFinished() signal is emitted.
 */
void CxuiSelfTimer::timeout()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Update seconds passed.
    ++mCounter;

    // Update now to have better looking user experience,
    // especially when counter elapses: "0" is updated before emitting event,
    // so the UI seems to update smoother.
    updateWidgets();

    playSound();

    // Check if timer ran out
    if (mCounter >= mDelay) {
        mTimer.stop();
        mSound.stop();
        hideWidgets();
        emit timerFinished();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Play selftimer sound.
 */
void CxuiSelfTimer::playSound()
{
    CX_DEBUG_ENTER_FUNCTION();

    // play sounds only if warning tones are enabled
    if (mUseSound) {
        CX_DEBUG(("play"));

        int timeLeft = mDelay - mCounter;

        if (timeLeft <= 3) {
            // play as fast as we can
            if (mSound.isFinished()) {
                mSound.setLoops(-1);
                mSound.play();
            }
        } else if (timeLeft <= 10) {
            // play every second
            mSound.setLoops(1);
            mSound.play();
        } else {
            // play once every two seconds
            if (mCounter%2) {
                mSound.setLoops(1);
                mSound.play();
            }
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
     Slot for starting the countdown. Show postcapture setting is set to
     continuous and previous value stored. Timer is started.

     \sa cancel()
 */
void CxuiSelfTimer::startTimer()
{
    CX_DEBUG_ENTER_FUNCTION();

    // get the current postcapture timeout
    try {
        mOldPostCaptureTimeOut = mSettings.get<int>(CxeSettingIds::STILL_SHOWCAPTURED);
        // set continuous postcapture (view is visible until dismissed)
        mSettings.set(CxeSettingIds::STILL_SHOWCAPTURED, CONTINUOUS_POSTCAPTURE);
    } catch (CxeException &e) {
        // if there was an error, don't modify the postcapture setting
        mOldPostCaptureTimeOut = UNKNOWN;
    }

    if (mStartButton) {
        mStartButton->setEnabled(false);
    }

    // start countdown
    mCounter = 0;
    playSound();
    mTimer.start(1000); // 1000 milliseconds == 1 second

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Slot for receiving changes to the selftimer countdown timeout.

    \param seconds Amount of seconds for the selftimer countdown. If the value is
    0 then selftimer is disabled.
 */
void CxuiSelfTimer::changeTimeOut(int seconds)
{
    CX_DEBUG_ENTER_FUNCTION();

    // make sure that timer is not running (should not be)
    reset(false);

    // set the new timer value
    // if incoming value is 0, it means that the selftimer is disabled
    if (seconds > 0) {
        mDelay = seconds;
    } else {
        //value == 0
        mDelay = -1;
    }

    updateWidgets();

    CX_DEBUG_EXIT_FUNCTION();
}

/*! Resets the selftimer countdown.
    Countdown is stopped, and set back to starting value. Start button is set enabled.
    @param update Update the UI widgets if true, skip update if false.
    Can be used to by-pass the UI update if planned to do shortly again.
 */
void CxuiSelfTimer::reset(bool update)
{
    // Stop timer and reset counter.
    mTimer.stop();
    mSound.stop();
    mCounter = 0;

    // Set start buttonback to enabled.
    if (mStartButton) {
        mStartButton->setEnabled(true);
    }

    // Update UI if requested.
    if(update) {
        updateWidgets();
    }
}

/*! Updates the selftimer widgets
    Changes the visibility of the selftimer widgets (counter and cancel button)
    based on the timeoutValue. Updates the counter value.
 */
void CxuiSelfTimer::updateWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();

    int remaining(mDelay - mCounter);
    CX_DEBUG(("remaining time %d", remaining));

    if (mTimerLabel) {
        mTimerLabel->setPlainText(QString::number(remaining));
    }

    if (isEnabled()) {
        showWidgets();
    } else {
        hideWidgets();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Shows the selftimer widgets.
 */
void CxuiSelfTimer::showWidgets()
{
    if (mWidgetContainer){
        mWidgetContainer->show();
    }


}

/*!
    Hides the selftimer widgets.
 */
void CxuiSelfTimer::hideWidgets()
{
    if (mWidgetContainer){
        mWidgetContainer->hide();
    }

}

/*!
 * Enables or disables the selftimer sound.
 * \param uid UID of the changed setting
 * \param key Key of the changed setting
 * \param value New setting value
 */
void CxuiSelfTimer::enableSound(long int uid, unsigned long int key, QVariant value)
{
#ifdef Q_OS_SYMBIAN
    // selftimer is only interested in warning tones
    if (uid == KCRUidProfileEngine.iUid && key == KProEngActiveWarningTones) {
        CX_DEBUG_IN_FUNCTION();
        // possible values are:
        // 0 -> warning tones off
        // 1 -> warning tones on
        mUseSound = (value.toInt() == 1);
    }
#else
    Q_UNUSED(uid);
    Q_UNUSED(key);
    Q_UNUSED(value);
#endif
}

