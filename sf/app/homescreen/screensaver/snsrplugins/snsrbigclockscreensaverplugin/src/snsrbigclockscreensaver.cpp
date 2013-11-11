/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Big clock Screensaver.
*
*/

#include "snsrbigclockscreensaver.h"

#ifdef Q_OS_SYMBIAN
#include <e32std.h>
#include <XQSettingsManager> 
#include <screensaverdomaincrkeys.h>
#endif // Q_OS_SYMBIAN

#include <QDebug>
#include <QTime>
#include <QTimer>
#include <HbInstance>
#include <HbMainWindow>

#include "snsranalogclockcontainer.h"
#include "snsrdigitalclockcontainer.h"
#include "snsroledanalogclockcontainer.h"
#include "snsroleddigitalclockcontainer.h"
#include "snsrblankcontainer.h"
#include "snsrindicatormodel.h"

/*!
    \class SnsrBigClockScreensaver
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Screensaver with big digital clock.
 */

/*!
    Constructs a new SnsrBigClockScreensaver.
 */
SnsrBigClockScreensaver::SnsrBigClockScreensaver() :
    mMainWindow(0),
    mCurrentContainer(0),
    mIndicatorModel(0)
{
    mMainWindow = HbInstance::instance()->allMainWindows().at(0);
    // for nice looking clock hand transformations
    mMainWindow->setRenderHint(QPainter::SmoothPixmapTransform);
    
    // This model holds indicator status information and must exist as
    // long as screensaver does.
    mIndicatorModel = new SnsrIndicatorModel(this);
}

/*!
    Destructs the class.
 */
SnsrBigClockScreensaver::~SnsrBigClockScreensaver()
{
    // TODO: unlock orientation once the locking mechanism is available and used
    // mCurrentContainer, mIndicatorModel - deleted by the parent
}

/*!
    @copydoc Screensaver::onInitialize()
*/
bool SnsrBigClockScreensaver::onInitialize()
{
    qDebug() << "SnsrBigClockScreensaver::onInitialize()";
    return true;
}

/*!
    @copydoc Screensaver::onForeground()
 */
bool SnsrBigClockScreensaver::onForeground()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockScreensaver::onForeground")
    qDebug() << "SnsrBigClockScreensaver::onForeground()";

    removeCurrentContainer();

    SnsrBigClockContainer* newContainer( 0 );
    if (clockFormat() == ClockFormatAnalog) {
        newContainer = new SnsrAnalogClockContainer();
    }
    else {
        newContainer = new SnsrDigitalClockContainer();
    }

    setCurrentContainer( newContainer );
    
    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockScreensaver::onForeground")
    return true;
}

/*!
    @copydoc Screensaver::onPartialForeground()
 */
bool SnsrBigClockScreensaver::onPartialForeground()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockScreensaver::onPartialForeground")
    qDebug() << "SnsrBigClockScreensaver::onPartialForeground()";

    removeCurrentContainer();
    
    SnsrBigClockContainer* newContainer( 0 );

#ifdef Q_OS_SYMBIAN  
    // Check ScreensaverStatus from repository
    XQSettingsManager::Error error;
    XQCentralRepositorySettingsKey settingsKey(
            KCRUidScreensaverSettings.iUid, KScreensaverStatus ); // TUid as same repository used in control panel via Symbian APIs 
    XQSettingsManager setManager;
    int screensaverOn = setManager.readItemValue(settingsKey, XQSettingsManager::TypeInt).toInt();
    error = setManager.error();
    if (error != XQSettingsManager::NoError) {
        qDebug("Error reading value from XQSettingsManager.. error = %d", error);
    }
#else
    int screensaverOn = 1;
#endif // Q_OS_SYMBIAN
    
    if (screensaverOn) {
        if (clockFormat() == ClockFormatAnalog) {
            newContainer = new SnsrOledAnalogClockContainer();
        }
        else {
            newContainer = new SnsrOledDigitalClockContainer();
        }
    }
    else {
        newContainer = new SnsrBlankContainer();
    }
    
    setCurrentContainer( newContainer );

    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockScreensaver::onPartialForeground")
    return true;
}

/*!
    @copydoc Screensaver::onBackground()
 */
bool SnsrBigClockScreensaver::onBackground()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockScreensaver::onBackground")
    qDebug() << "SnsrBigClockScreensaver::onBackground()";

    removeCurrentContainer();

    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockScreensaver::onBackground")
    return true;
}

/*!
    @copydoc Screensaver::onPowerSave()
 */
bool SnsrBigClockScreensaver::onPowerSave()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockScreensaver::onPowerSave")
    qDebug() << "SnsrBigClockScreensaver::onPowerSave()";

    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockScreensaver::onPowerSave")
    return false;
}

/*!
    @copydoc Screensaver::onClose()
 */
bool SnsrBigClockScreensaver::onClose()
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrBigClockScreensaver::onClose")
    qDebug() << "SnsrBigClockScreensaver::onClose()";

    bool ret(false);
    if (onBackground()) {
        delete mCurrentContainer;
        mCurrentContainer = 0;
        ret = true;
    }

    SCREENSAVER_TEST_FUNC_EXIT("SnsrBigClockScreensaver::onClose")
    return ret;
}

/*!
    @copydoc Screensaver::onHandleActiveIndicators
 */
void SnsrBigClockScreensaver::onHandleActiveIndicators(
        const QList<HbIndicatorInterface*> &activeIndicators)
{
    mIndicatorModel->handleActiveIndicators(activeIndicators);
}

/*!
    @copydoc Screensaver::onHandleActivatedIndicator
 */
void SnsrBigClockScreensaver::onHandleActivatedIndicator(
        HbIndicatorInterface *activatedIndicator)
{
    mIndicatorModel->handleActivatedIndicator(activatedIndicator);
}

/*!
    @copydoc Screensaver::onHandleDeactivatedIndicator
 */
void SnsrBigClockScreensaver::onHandleDeactivatedIndicator(
        HbIndicatorInterface *deactivatedIndicator)
{
    mIndicatorModel->handleDeactivatedIndicator(deactivatedIndicator);
}

/*!
    @copydoc Screensaver::currentPowerMode
 */
Screensaver::ScreenPowerMode SnsrBigClockScreensaver::currentPowerMode()
{
    Screensaver::ScreenPowerMode mode( Screensaver::ScreenModeFullPower );
    if ( mCurrentContainer ) {
        mode = mCurrentContainer->displayPowerMode();
    }
    else {
        qWarning() << "No current container when current power mode queried.";
    }
    return mode;
}

/*!
    @copydoc Screensaver::getActiveScreenRows
 */
void SnsrBigClockScreensaver::getActiveScreenRows(int *firstActiveRow, int *lastActiveRow)
{
    if ( mCurrentContainer ) {
        mCurrentContainer->getActiveScreenRows( firstActiveRow, lastActiveRow );
    }
    else {
        qWarning() << "No current container when active rows queried.";
    }
}

/*!
    @copydoc Screensaver::updateLayout
 */
void SnsrBigClockScreensaver::updateLayout()
{
    if ( mCurrentContainer ) {
        if ( mCurrentContainer->isOrientationLocked() ) {
            // TODO: There is currently no way for device dialog to lock the display orientation.
            // Orbit team has a backlog item about "global orientation lock" which could be
            // used here when it is available.
        }
        else {
            // TODO: There is currently no way for device dialog to lock the display orientation.
            // Orbit team has a backlog item about "global orientation lock" which could be
            // used here when it is available.
        }
        mCurrentContainer->changeLayout( mMainWindow->orientation() );
    }
    else {
        qWarning() << "No current container when updateLayout called.";
    }
}

/*!
    Update clock time when timer is triggered
 */
void SnsrBigClockScreensaver::updateTime()
{
    if ( mCurrentContainer ) {
        mCurrentContainer->update();
        
        // Move the active area of display device if current container uses the power save mode.
        if ( mCurrentContainer->displayPowerMode() == Screensaver::ScreenModeLowPower ) {
            emit activeAreaMoved();
        }
        
        // Ensure correct update interval is used. First timed update may be done
        // with timeout differing from the normal update rate.
        int updateInterval = mCurrentContainer->updateIntervalInMilliseconds();
        if ( updateInterval > 0 ) {
            mTimer.setInterval( updateInterval );
        }
    }
}

/*!
    Determines the curent clock format settings.
    \retval ClockFormat.
 */
SnsrBigClockScreensaver::ClockFormat SnsrBigClockScreensaver::clockFormat()
{
#ifdef Q_OS_SYMBIAN
    if (TLocale().ClockFormat() == EClockAnalog) {
        return ClockFormatAnalog;
    } else {
        return ClockFormatDigital;
    }
#else
    // windows build - change the format every 30 seconds for testing purposes
    if (QTime::currentTime().second() < 30) {
        return ClockFormatAnalog;
    } else {
        return ClockFormatDigital;
    }
#endif // Q_OS_SYMBIAN
}

void SnsrBigClockScreensaver::removeCurrentContainer()
{
    if ( mCurrentContainer ) {
        disconnect(
            &mTimer, SIGNAL(timeout()),
            this, SLOT(updateTime())
            );
        disconnect( 
            mCurrentContainer, SIGNAL(unlockRequested()), 
            this, SIGNAL(unlockRequested()) );
        if (mTimer.timerId()!= -1) {
            mTimer.stop();
        }
        emit viewChanged(0);
        
        delete mCurrentContainer;
        mCurrentContainer = 0;
    }
}

void SnsrBigClockScreensaver::setCurrentContainer( SnsrBigClockContainer* newContainer )
{
    mCurrentContainer = newContainer;
    mCurrentContainer->setParent(this);
    connect( &mTimer, SIGNAL(timeout()), SLOT(updateTime()) );
    connect( mCurrentContainer, SIGNAL(unlockRequested()), SIGNAL(unlockRequested()) );
    
    mCurrentContainer->setIndicatorModel(*mIndicatorModel);

    int updateInterval = mCurrentContainer->updateIntervalInMilliseconds(); 

    // In case the container needs an update seldomly (at maximum once per minute),
    // align these updates with the even minutes by doing the first update on the
    // next even minute.
    const int minute = 60*1000;
    if ( updateInterval >= minute ) {
        updateInterval = minute - QTime::currentTime().second()*1000;
    }
    // container can use negative updateInteval value to indicate that no recurring
    // updates are needed
    if ( updateInterval > 0 ) {
        mTimer.start(updateInterval);
    }
    
    emit viewChanged(mCurrentContainer);
}

