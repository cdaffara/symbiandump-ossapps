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
* Description: Default Screensaver runtime.
*
*/

#include <QGraphicsLinearLayout>
#include <QGraphicsSceneResizeEvent>
#include <QPainter>
#include <QDebug>
#include <QServiceManager>
#include <QServiceFilter>
#include <QServiceInterfaceDescriptor>
#include <QTimer>
#ifdef Q_OS_SYMBIAN
#include <XQSettingsManager>
#include <screensaverdomaincrkeys.h>
#endif //Q_OS_SYMBIAN
#include <HbMainWindow>
#include <HbIndicatorInterface>
#include <HbEffect>

#include <screensaver.h>

#include "snsrdevicedialog.h"
#include "snsrdevicedialogdisplaycontrol.h"

const char *gBigClockInterfaceName = "com.nokia.screensaver.ISnsrBigClockScreensaver";

/*!
    \class SnsrDeviceDialog
    \ingroup group_snsrdevicedialogplugin
    \brief Screensaver Device Dialog.
 */

extern const char *lViewType;

const char *SnsrDeviceDialog::dataKeyUnlock = "unlock";

QTM_USE_NAMESPACE

/*!
    Constructor.
    \param viewType Initial view type (active or standby).
    \param parent Parent.
 */
SnsrDeviceDialog::SnsrDeviceDialog(const QVariantMap &parameters, QGraphicsItem *parent) :
        HbPopup(parent),
        mScreensaver(0),
        mLayout(0),
        mDisplayControl(0),
        mDisplayModeTimer(0),
        mHasFocus(false)
{
    qDebug("SnsrDeviceDialog::SnsrDeviceDialog()");

    
    // load screensaver plugin
    // Workaround to QtSF bug. Create in stack after the bug is fixed.
    QServiceManager manager;
    QServiceFilter filter(gBigClockInterfaceName);
    QList<QServiceInterfaceDescriptor> interfaces = manager.findInterfaces(
        filter);
    Q_ASSERT(interfaces.count());

    mScreensaver = manager.loadLocalTypedInterface<Screensaver>(
        interfaces.first().interfaceName() );
    if (mScreensaver) {
        mScreensaver->setParent(this);
    } else {
        // TODO: error state
    }
    connect(mScreensaver, SIGNAL(viewChanged(QGraphicsWidget*)),
        SLOT(changeView(QGraphicsWidget*)));
    connect(mScreensaver, SIGNAL(faulted()),
        SLOT(screensaverFaulted()));
    connect( mScreensaver, SIGNAL(unlockRequested()),
        SLOT(requestUnlock()) );
    connect( mScreensaver, SIGNAL(activeAreaMoved()),
        SLOT(updateDisplayMode()) );
    
    mScreensaver->initialize();

    setBackgroundItem(0);

    setDismissPolicy(HbPopup::NoDismiss);
    setTimeout(HbPopup::NoTimeout);
    
    // Disable all the effects for the screensaver dialog. Otherwise, the
    // automatic fade effect would be applied to the screensaver when screen
    // orientation changes, revealing the UI beneath (because device dialog
    // application server has a transparent background). This both looks ugly 
    // and poses a potential security risk, as in the device locked case 
    // screensaver is used as a layer to hide any confidential information.
    // Screensaver containers can still implement custom effects for orientation
    // swithc etc.
    HbEffect::disable(this);

    setDeviceDialogParameters( parameters );
    
    mDisplayControl = new SnsrDeviceDialogDisplayControl();
    
    mDisplayModeTimer = new QTimer(this);
    connect( mDisplayModeTimer, SIGNAL(timeout()), SLOT(updateDisplayMode()) );
}

/*!
    Destructor.
 */
SnsrDeviceDialog::~SnsrDeviceDialog()
{
    qDebug("SnsrDeviceDialog::~SnsrDeviceDialog()");
    QServiceManager serviceManager;
    serviceManager.removeService(gBigClockInterfaceName);

    // We should get the FocusOut event when closing, but deactivate the
    // power save also here in case we haven't got the event for some reason.
    if ( mDisplayControl ) {
        mDisplayControl->setDisplayFullPower();
        delete mDisplayControl;
    }
}

/*!
    Sets device dialog parameters.
    \param parameters Device Dialog's parameters to be set.
    \ret return true if parameters are correct.
 */
bool SnsrDeviceDialog::setDeviceDialogParameters(const QVariantMap &parameters)
{
    qDebug("SnsrDeviceDialog::setDeviceDialogParameters()");

    // default to initial state if no other state given
    int viewType( ViewTypeInitial );
        
    QVariantMap::const_iterator it = parameters.find(lViewType);
    if (it != parameters.end() && !it.value().isNull() && it.value().isValid()) {
        bool ok(false);
        viewType = it.value().toInt(&ok);
        if ( !ok ) {
            qWarning() << "SnsrDeviceDialog::setDeviceDialogParameters() - int cast failed";
            return false;
        }
    }
    
#ifdef Q_OS_SYMBIAN
    // Check initial view from repository
    if (viewType == ViewTypeInitial ) {
        XQSettingsManager::Error error;
        int startupView = 0; 
        XQCentralRepositorySettingsKey settingsKey(
                 KCRUidScreensaverSettings.iUid, KScreensaverStartupView ); // TUid as same repository used in control panel via Symbian APIs 
        XQSettingsManager settingsManager;
        startupView = settingsManager.readItemValue(settingsKey, XQSettingsManager::TypeInt).toInt();
        error = settingsManager.error();
        if (error == XQSettingsManager::NoError) {
            viewType = startupView;
        }
    }
#else
    if (viewType == ViewTypeInitial ) {
        viewType = ViewTypeStandby;
	}
#endif //Q_OS_SYMBIAN
    
    switch (viewType) {
    case SnsrDeviceDialog::ViewTypeActive:
        mScreensaver->foreground();
        return true;
    case SnsrDeviceDialog::ViewTypeStandby:
        mScreensaver->partialForeground();
        return true;
    default:
        qWarning() << QString("SnsrDeviceDialog::setDeviceDialogParameters() "
          "- unspecefied view type: %1").arg(viewType);
    }
    
    return false;
}

/*!
    Returns error code.
    \ret error code.
 */
int SnsrDeviceDialog::deviceDialogError() const
{
    return 0;
}

/*!
    Closes device dialog.
    \param byClient If dialog is close by client.
 */
void SnsrDeviceDialog::closeDeviceDialog(bool byClient)
{
    qDebug("SnsrDeviceDialog::closeDeviceDialog()");
    Q_UNUSED(byClient)
    hide();
    emit deviceDialogClosed();
}

/*!
    Returns device dialog instance.
    \ret Device dialog instance.
 */
HbPopup *SnsrDeviceDialog::deviceDialogWidget() const
{
    return const_cast<SnsrDeviceDialog*>(this);
}

/*!
    Shows widget.
    \param event.
 */
void SnsrDeviceDialog::showEvent(QShowEvent *event)
{
    qDebug("SnsrDeviceDialog::showEvent()");

    HbPopup::showEvent(event);
    
    // workaround for duplicating connections and emiting two signals
    // showEvent is called for HbDeviceDialogManager showDeviceDialog()
    // and for HbPopup itemChange(), both events are set to QEvent::Show type
    disconnect(mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
        this, SLOT(handleOrientationChange(Qt::Orientation)));
    connect(mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),
        this, SLOT(handleOrientationChange(Qt::Orientation)));

    changeLayout();
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Hides widget.
    \param event.
 */
void SnsrDeviceDialog::hideEvent(QHideEvent *event)
{
    qDebug("SnsrDeviceDialog::hideEvent()");

    HbPopup::hideEvent(event);
    mScreensaver->background();
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

/*!
    Close widget.
    \param event.
 */
void SnsrDeviceDialog::closeEvent(QCloseEvent *event)
{
    qDebug("SnsrDeviceDialog::closeEvent()");

    HbPopup::closeEvent(event);

    disconnect(mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)), 
        this, SLOT(handleOrientationChange(Qt::Orientation)));
    mScreensaver->close();
}

/*!
    Handle focus in/out events and just pass through the rest of the events
 */
bool SnsrDeviceDialog::event(QEvent *event)
{
    if ( event->type() == QEvent::FocusOut ) {
        mHasFocus = false;
        // Some other dialog came on top of us (or dialog was closed).
        // Set screen to full power.
        mDisplayModeTimer->stop();
        mDisplayControl->setDisplayFullPower();
    }
    else if ( event->type() == QEvent::FocusIn ) {
        mHasFocus = true;
        // We became (again) the top-most dialog. Switch screen to
        // applicable power mode.
        updateDisplayModeDeferred();
    }
    return HbPopup::event(event);
}

/*!
    Replaces layout's item.
    \param widget New item to be added to layout.
 */
void SnsrDeviceDialog::changeView(QGraphicsWidget *widget)
{
    qDebug("SnsrDeviceDialog::changeView() - widget: 0x%X", (int)widget);
    if (!mLayout) {
        mLayout = new QGraphicsLinearLayout(Qt::Vertical);
        mLayout->setContentsMargins(0,0,0,0);
        setLayout(mLayout);
    }
    
    if (mLayout->count()) {
        mLayout->removeAt(0);
    }
    if (widget) {
        mLayout->addItem(widget);
        
        // Update layout and display mode immediately if view is
        // changed while Screensaver is visible. This is the case when
        // mode changes from standby to active or vice versa. In case of the
        // initial view setting, Screensaver is not yet visible, and the update
        // will happen later on Show/FocusIn events.
        if ( isVisible() ) {
           changeLayout();
           updateDisplayMode();
        }
        
        widget->show();
    }
}

/*!
    Handles faults in loaded screensaver model.
 */
void SnsrDeviceDialog::screensaverFaulted()
{
    qWarning() << "SnsrDeviceDialog::screensaverFaulted()";
    // TODO: some error handling
}

/*!
    Handle orientation change event
    \param orientation New orientation value.
 */
void SnsrDeviceDialog::handleOrientationChange(Qt::Orientation orientation)
{
    Q_UNUSED(orientation)
    changeLayout();
    updateDisplayMode();
}

/*!
    Resize device dialog.
 */
void SnsrDeviceDialog::changeLayout()
{
    QRectF rect = mainWindow()->layoutRect();
    setMinimumSize( rect.size() );
    setPreferredPos( QPointF(0,0) );
    QGraphicsSceneResizeEvent sceneResize;
    sceneResize.setNewSize( rect.size() );
    event( &sceneResize );
    mScreensaver->updateLayout();
}

/*!
    Update power mode of the display device
 */
void SnsrDeviceDialog::updateDisplayMode()
{
    // Cancel any pending deferred update order
    mDisplayModeTimer->stop();
    
    // Check desidred display power mode from screensaver but only when
    // we are the top-most device dialog. If some other dialog is on top of us, 
    // then always default to full power mode.
    Screensaver::ScreenPowerMode mode( Screensaver::ScreenModeFullPower );
    if ( mHasFocus ) {
        mode = mScreensaver->currentPowerMode();
    }
    
    switch ( mode ) {
        case Screensaver::ScreenModeOff: {
            mDisplayControl->setDisplayOff();
            break;
        }
        case Screensaver::ScreenModeLowPower: {
            int firstRow(-1);
            int lastRow(-1);
            mScreensaver->getActiveScreenRows(&firstRow, &lastRow);
            mDisplayControl->setDisplayLowPower(firstRow, lastRow);
            break;
        }
        case Screensaver::ScreenModeFullPower: {
            mDisplayControl->setDisplayFullPower();
            break;
        }
    }
}

/*!
    Update power mode of the display device after a short delay
 */
void SnsrDeviceDialog::updateDisplayModeDeferred()
{
    mDisplayModeTimer->start(200); // milliseconds
}

/*!
    Send unlock signal to autolock.
 */
void SnsrDeviceDialog::requestUnlock()
{
    QVariantMap data;
    data.insert(dataKeyUnlock, 1);
    emit deviceDialogData(data);
}

/*!
    Called when the dialog is created if they are any currently
    activate universal indicators present.
 */
void SnsrDeviceDialog::indicatorsActivated(
        const QList<HbIndicatorInterface*> &activatedIndicators)
{
    mScreensaver->handleActiveIndicators(activatedIndicators);
}

/*!
    Called when some universal indicator is activated.
    Ownership of activatedIndicator is not transferred.
 */
void SnsrDeviceDialog::indicatorActivated(
        HbIndicatorInterface *activatedIndicator)
{
    mScreensaver->handleActivatedIndicator(activatedIndicator);
}

/*!
    Called when some universal indicator is deactivated.
 */
void SnsrDeviceDialog::indicatorDeactivated(
        HbIndicatorInterface *deactivatedIndicator)
{
    mScreensaver->handleDeactivatedIndicator(deactivatedIndicator);
}

// end of file

