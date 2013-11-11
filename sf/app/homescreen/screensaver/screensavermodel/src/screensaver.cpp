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
* Description:  Base class for all screensavers.
*
*/

#include "screensaver.h"
#include "screensaver_p.h"

/*!
    \enum ScreensaverState
    Lists states that the Screensaver can be in.
*/

/*
    \var ScreensaverState ScreensaverStateConstructed
    Screensaver is in this state right after construction.
*/

/*
    \var ScreensaverState ScreensaverStateInitialized

    All Screensaver resources are initialized.
    Screensaver is set to Initialized state after a call to initialize 
    (if previously Constructed or Closed) method.
*/

/*
    \var Screensaver ScreensaverStateBackground
    Screensaver is in background, its operations are suspended.
    Screensaver is set to Background after a call to background method.
*/

/*
    \var Screensaver ScreensaverStateForeground 
    Screensaver is in foreground and fully operational, showing the main visualization.
    Screensaver is set to Foreground after a call to foreground method.
*/

/*
    \var Screensaver ScreensaverStatePartialForeground 
    Screensaver has limited foreground (in OLED display cases).
    Screensaver is set to PartialForeground after a call to partialForeground method.
*/

/*
    \var Screensaver ScreensaverStatePowerSave 
    Device is in power save mode. Screensaver should limit all processing.
    Screensaver is set to PowerSave after a call to powerSave method.
*/

/*
    \var Screensaver ScreensaverStateClosed 
    Screensaver is closed. All resources should be frees.
    Screensaver is set to Closed after a call to close method.
*/

/*!
    \class Screensaver
    \brief Base class for all screensavers.

    Screensaver plug-ins provide the visualizations for different screensaver application states.
    A Screensaver is notified about state changes and in consequence it should emit a signal
    viewChanged() carrying a QGraphicsWidget which will be set as the current view.
    The application takse care about tracing device status so the Screensaver should be only
    concerned about the GUI.
 */

/*!
    Constructs a new Screensaver with \a parent.
 */
Screensaver::Screensaver(QObject *parent) :
    QObject(parent), m_d(new ScreensaverPrivate(this))
{
}

/*!
    Destructs the class.
 */
Screensaver::~Screensaver()
{
    delete m_d;
}

/*!
    \fn void Screensaver::faulted()

    This signal is emitted if a fault occurs when changing Screensaver's state.
 */

/*!
    \fn void Screensaver::viewChanged(QGraphicsWidget *widget)

    This signal should be emitted when the Screensaver needs to change its visualization
    after a state change.
    \param widget The graphics widget container holding the current visualization.
 */

/*!
    \fn void Screensaver::unlockRequested()

    This signal is emitted if screensaver wants the device to be unlocked.
 */

/*!
    \fn void Screensaver::activeAreaMoved()

    This signal is emitted when active area of power-save mode screensaver has moved.
 */

/*!
    Returns the state that the Screensaver is currently in.
    \return The current state.
 */
ScreensaverState Screensaver::currentState()
{
    return m_d->currentState();
}

/*!
    \fn virtual Screensaver::ScreenPowerMode currentPowerMode() = 0

    Inherited screensavers must implement this function to return the
    display power mode to be used in the current state.
 */

/*!
    \fn virtual void getActiveScreenRows(int *firstActiveRow, int *lastActiveRow) = 0

    Inherited screensavers must implement this function to return the rows where 
    the screensaver wants to draw graphics. This is called when screen is set to the
    power save mode. The values returned do not matter in case the screensaver doesn't
    ask for power save mode.
    Note that the returned rows should reflect the rows in the natural orientation
    of the screen device. That is, if default orientation of the screen is portrait,
    then the return value in landscape mode must reflect the columns between which
    the graphics are drawn.
 */

/*!
    \fn virtual void updateLayout() = 0

    Inherited screensavers must implement this function to reload the screen layout
    for the current screen orientation.
 */

/*!
    Initializes the Screensaver
 */
void Screensaver::initialize()
{
    m_d->initialize();
}

/*!
    Called when the application is in foreground.
 */
void Screensaver::foreground()
{
    m_d->foreground();
}

/*!
    Called when the application gains limited foreground as with OLED display cases.
 */
void Screensaver::partialForeground()
{
    m_d->partialForeground();
}

/*!
    Called when the application goes to background.
 */
void Screensaver::background()
{
    m_d->background();
}

/*!
    Called when device enters power save mode.
 */
void Screensaver::powerSave()
{
    m_d->powerSave();
}

/*!
    Stops Screensaver's processing.
 */
void Screensaver::close()
{
    m_d->close();
}

/*!
    If there is any active universal indicator when screensaver
    is launched, then this method is called.
    Note that indicators are implemented only for Symbian/S60 OS.
    \param activeIndicators List of currently active indicators in 
    the chronological order according to their arrival time.
 */
void Screensaver::handleActiveIndicators(
    const QList<HbIndicatorInterface*> &activeIndicators)
{
    // TODO: need to go through private class?
    m_d->handleActiveIndicators(activeIndicators);
}

/*!
    Called when some universal indicator is activated.
    \param activatedIndicator Activated indicator. Ownership is not transferred.
 */
void Screensaver::handleActivatedIndicator(
    HbIndicatorInterface* activatedIndicator)
{
    m_d->handleActivatedIndicator(activatedIndicator);
}

/*!
    Called when some universal indicator is deactivated.
    \param activatedIndicator Deactivated indicator. Ownership is not transferred.
 */
void Screensaver::handleDeactivatedIndicator(
    HbIndicatorInterface* deactivatedIndicator)
{
    m_d->handleDeactivatedIndicator(deactivatedIndicator);
}


/*!
    \fn virtual bool Screensaver::onForeground() = 0

    After a call the Screensaver should emit the foreground state visualization.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    \return Indicates if the operation succeeded.
 */

/*!
    \fn virtual bool Screensaver::onPartialForeground() = 0

    After a call the Screensaver should emit the partial foreground state visualization.
    This is valid for OLED display cases when the screensaver is displayed at all times
    with limited functionality.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    \return Indicates if the operation succeeded.
 */

/*!
    \fn virtual bool Screensaver::onBackground() = 0

    After a call the Screensaver should limit its processing.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    \return Indicates if the operation succeeded.
 */

/*!
    \fn virtual bool Screensaver::onPowerSave() = 0

    After a call the Screensaver should limit its processing as much as possible.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    \return Indicates if the operation succeeded.
 */

/*!
    After a call it should initialize the Screensaver.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    The default implementation does nothing and always returns true.
    \return Indicates if the operation succeeded.
 */
bool Screensaver::onInitialize()
{
    return true;
}

/*!
    After a call it should close the Screensaver.
    The Screensaver should also free all resources.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    The default implementation does nothing and always returns true.
    \return Indicates if the operation succeeded.
 */
bool Screensaver::onClose()
{
    return true;
}

/*!
    The default implementation is empty.
 */
void Screensaver::onHandleActiveIndicators(
        const QList<HbIndicatorInterface*> &activeIndicators)
{
    Q_UNUSED(activeIndicators);
}

/*!
    The default implementation is empty.
 */
void Screensaver::onHandleActivatedIndicator(
        HbIndicatorInterface *activatedIndicator)
{
    Q_UNUSED(activatedIndicator);
}

/*!
    The default implementation is empty.
 */
void Screensaver::onHandleDeactivatedIndicator(
        HbIndicatorInterface *deactivatedIndicator)
{
    Q_UNUSED(deactivatedIndicator);
}

