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

#include <QProcess>
#include <QApplication>
#include <hbmainwindow.h>
#include <hbslider.h>
#include <hbtoolbar.h>
#include <hblabel.h>
#include <hbtoolbarextension.h>
#include <hbtransparentwindow.h>
#include <hbmenu.h>
#include <hbdialog.h>
#include <hbdeviceprofile.h>
#include <hbnotificationdialog.h>
#include <hbmessagebox.h>
#include <hbaction.h>
#include <hbstyle.h>
#include <hbframeitem.h>
#include <hbwidget.h>
#include <hbeffect.h>
#include <hbinstance.h>

#include "cxeengine.h"
#include "cxeviewfindercontrol.h"
#include "cxezoomcontrol.h"
#include "cxecameradevicecontrol.h"
#include "cxenamespace.h"
#include "cxefeaturemanager.h"
#include "cxestillcapturecontrol.h"
#include "cxevideocapturecontrol.h"
#include "cxesettings.h"
#include "cxutils.h"
#include "cxuienums.h"
#include "cxuidocumentloader.h"
#include "cxuicapturekeyhandler.h"
#include "cxuiprecaptureview.h"
#include "cxuisettingdialog.h"
#include "cxuisettingradiobuttonlist.h"
#include "cxuisettingslider.h"
#include "cxuisettingsinfo.h"

#ifdef Q_OS_SYMBIAN
#include "OstTraceDefinitions.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxuiprecaptureviewTraces.h"
#endif

#endif //Q_OS_SYMBIAN

#include "cxuifullscreenpopup.h"
#include "cxuiserviceprovider.h"
#include "cxuizoomslider.h"

using namespace CxUiLayout;
using namespace CxUiSettings;
using namespace CxUiInternal;

const int CXUI_GEOTAGGING_BLINK_INTERVAL = 500; //milliseconds, for geotagging icon


// ---------------------------------------------------------------------------
// CxuiPrecaptureView::CxuiPrecaptureView
// Constructor
// ---------------------------------------------------------------------------
//
CxuiPrecaptureView::CxuiPrecaptureView(QGraphicsItem *parent) :
    CxuiView(parent),
    mViewfinder(0),
    mSettingsGrid(0),
    mWidgetsLoaded(false),
    mSettingsDialog(NULL),
    mSettingsDialogList(NULL),
    mQualityIcon(NULL),
    mGeoTaggingIndicatorIcon(NULL),
    mFaceTrackingIcon(NULL),
    mStandbyPopup(NULL),
    mSettingsDialogHeading(NULL),
    mSliderSettingsDialog(NULL),
    mSliderSettingsDialogHeading(NULL),
    mSettingsSlider(NULL),
    mSettingsInfo(NULL),
    mSettingsDialogOkAction(NULL),
    mSliderSettingsDialogOkAction(NULL)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

CxuiPrecaptureView::~CxuiPrecaptureView()
{
    CX_DEBUG_ENTER_FUNCTION();
    QCoreApplication::instance()->removeEventFilter(this);
    delete mSettingsGrid;
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * CxuiPrecaptureView::construct
 * Construct-method handles initialisation tasks for this class.
 * @param mainwindow
 * @param engine
 * @param documentLoader
 * @param keyHandler
 */
void CxuiPrecaptureView::construct(HbMainWindow *mainWindow, CxeEngine *engine,
                                   CxuiDocumentLoader *documentLoader,
                                   CxuiCaptureKeyHandler * keyHandler)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXUIPRECAPTUREVIEW_CONSTRUCT_1, "msg: e_CX_PRECAPVIEW_CONSTRUCT 1");

    CxuiView::construct(mainWindow, engine, documentLoader, keyHandler);

    mSettingsInfo = new CxuiSettingsInfo(engine);
    CX_DEBUG_ASSERT(mSettingsInfo);

    setContentFullScreen(true);

    connect(&mEngine->zoomControl(), SIGNAL(stateChanged(CxeZoomControl::State, CxeError::Id)), this,
            SLOT(handleEngineZoomStateChange(CxeZoomControl::State, CxeError::Id)));

    connect(&mEngine->zoomControl(), SIGNAL(zoomLevelChanged(int)), this,
            SLOT(handleZoomLevelChange(int)));


    connect(&mEngine->viewfinderControl(), SIGNAL(stateChanged(CxeViewfinderControl::State, CxeError::Id)),
            this, SLOT(handleVfStateChanged(CxeViewfinderControl::State, CxeError::Id)));

    connect(&mEngine->geoTaggingTrail(), SIGNAL(stateChanged(CxeGeoTaggingTrail::State, CxeError::Id)),
            this, SLOT(updateLocationIndicator(CxeGeoTaggingTrail::State, CxeError::Id)));

    connect(&(mEngine->settings()), SIGNAL(settingValueChanged(const QString&,QVariant)),
            this, SLOT(handleSettingValueChanged(const QString&, QVariant)));

    mGeoTaggingBlinkTimer = new QTimer(this);
    connect(mGeoTaggingBlinkTimer, SIGNAL(timeout()), this, SLOT(blinkGeoTaggingIcon()));

    hideControls();

    if (CxuiServiceProvider::isCameraEmbedded()) {
        // override exit with action that completes the request before exiting
        HbAction* exitAction = new HbAction(Hb::QuitNaviAction, this);
        connect(exitAction, SIGNAL(triggered()), CxuiServiceProvider::instance(), SLOT(sendFilenameToClientAndExit()));
        setNavigationAction(exitAction);
    }

    OstTrace0(camerax_performance, CXUIPRECAPTUREVIEW_CONSTRUCT_2, "msg: e_CX_PRECAPVIEW_CONSTRUCT 0");

    QCoreApplication::instance()->installEventFilter(this);

    HbEffect::add("toolbarextension", "camera_toolbarextension_appear", "toolbarextension_appear");
    HbEffect::add("toolbarextension", "camera_toolbarextension_disappear", "toolbarextension_disappear");

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Is standby mode supported / needed by this view.
* Pre-capture views implement / need standby mode and return true.
* @return True if standby mode is supported, false otherwise.
*/
bool CxuiPrecaptureView::isStandbyModeSupported() const
{
    return true;
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::handleEngineZoomStateChange
// Slot for the initModeComplete signal from device control.
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::handleEngineZoomStateChange(CxeZoomControl::State newState, CxeError::Id /*error*/ )
{
    CX_DEBUG_ENTER_FUNCTION();

    if (newState == CxeZoomControl::Ready) {
        // Set the zoom range (if more than zero!)
        // NOTE: This is & should be done after ECam resolution has been set.
        int min = mEngine->zoomControl().min();
        int max = mEngine->zoomControl().max();

        // only change values if they are acceptable and have changed
        if (mSlider && (max - min > 0) && ((mSlider->maximum() != max)
                || (mSlider->minimum()!= min))) {
            mSlider->setRange(min, max);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::zoomTo
// Responds to valueChanged notification from zoom bar UI
// and asks the engine to set the appropriate zoom level.
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::zoomTo(int value)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(( "Slider value: %d", value ));
    mEngine->zoomControl().zoomTo(value);

    // restarts the timer, if ongoing
    if (mHideControlsTimeout.isActive()) {
        mHideControlsTimeout.start();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::toggleZoom
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::toggleZoom()
{
    if (mZoomVisible) {
        hideZoom();
    } else {
        showZoom();
    }
}


/*
 *  Event filter which filters tool bar mouse events and
 *  application wide mouse events.
 */

bool CxuiPrecaptureView::eventFilter(QObject *object, QEvent *event)
{
    bool eventWasConsumed = false;

    switch (event->type())
    {
    case QEvent::GraphicsSceneMouseRelease:
        if (mStandbyPopup) {
            mStandbyPopup->handleMouseRelease();
        } else {
            mHideControlsTimeout.start();
        }
        break;
    case QEvent::GraphicsSceneMousePress:
        if (object == mStandbyPopup) {
            mStandbyPopup->handleMousePress();
        } else {
            mHideControlsTimeout.stop();
        }
        break;
    default:
        break;
    }

    return eventWasConsumed;
}


// ---------------------------------------------------------------------------
// CxuiPrecaptureView::toggleControls
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::toggleControls()
{
    // This method is put here to demonstrate that widget creation can
    // be postponed using sections in layout xml
    if (!mWidgetsLoaded) {
        // call load widgets to load app DocML and get the pointers to needed widgets
        loadWidgets();
    }
    CxuiView::toggleControls();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::handleZoomLevelChange
// Slot to handle zoom level change signal from zoom control.
// -------------------------------------------------------------------------
//
void CxuiPrecaptureView::handleZoomLevelChange(int zoomValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mSlider && zoomValue != mSlider->value()) {
        CX_DEBUG(( "Zoom level is: %d", zoomValue ));
        mSlider->setValue(zoomValue);
    }

    // reset the timer if it is ongoing
    if (mHideControlsTimeout.isActive()) {
        mHideControlsTimeout.start();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::showEvent
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::showEvent(QShowEvent *event)
{
    CX_DEBUG_ENTER_FUNCTION();

    QGraphicsWidget::showEvent(event);

    if (event->type() == QEvent::Show) {
        QCoreApplication::instance()->installEventFilter(this);
        hideControls();
        event->accept();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::hideEvent
//
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::hideEvent(QHideEvent *event)
{
    CX_DEBUG_ENTER_FUNCTION();

    QGraphicsWidget::hideEvent(event);

    if (event->type() == QEvent::Hide) {
        QCoreApplication::instance()->removeEventFilter(this);
        hideZoom();
        hideToolbar();
        event->accept();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::requestCameraSwitch
// Slot to switch camera. Needed for options menu signals (in DocML).
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::requestCameraSwitch() {
    emit switchCamera();
}

/**
* Handle pressing auto focus key.
*/
void CxuiPrecaptureView::handleAutofocusKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mSettingsDialog) {
        // Prevent us from trying to show grid when dialog closes.
        mSettingsDialog->setStarterAction(NULL);
        mSettingsDialog->close();
    }
    if (mSliderSettingsDialog) {
        mSliderSettingsDialog->setStarterAction(NULL);
        mSliderSettingsDialog->close();
    }

    menu()->close();
    hideControls();

    CX_DEBUG_EXIT_FUNCTION();
}

// ---------------------------------------------------------------------------
// CxuiPrecaptureView::disableControlsTimeout
// Slot
// ---------------------------------------------------------------------------
//
void CxuiPrecaptureView::disableControlsTimeout()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mHideControlsTimeout.isActive()) {
        mHideControlsTimeout.stop();
    }
    CX_DEBUG_EXIT_FUNCTION();
}


// ---------------------------------------------------------------------------
// CxuiPrecaptureView::initCamera
// ---------------------------------------------------------------------------
void CxuiPrecaptureView::initCamera()
{
    CX_DEBUG_ENTER_FUNCTION();
    mEngine->initMode(mEngine->mode());
    CX_DEBUG_EXIT_FUNCTION();
}



/*!
 * Handles viewfinder state changes.
 * @param newState new viewfinder state
 * @param error error code in case of error
 */
void CxuiPrecaptureView::handleVfStateChanged(
    CxeViewfinderControl::State newState, CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(error);

    if (newState == CxeViewfinderControl::Running) {
        // start the standby timer
        emit startStandbyTimer();
    } else {
        // stop the standby timer when vf is not running.
        emit stopStandbyTimer();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot for entering standby mode.
* Show standby canvas in addition to base class implementation.
* @sa CxuiView::enterStandby()
*/
void CxuiPrecaptureView::enterStandby()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Release camera right away to avoid any problems with GPU memory.
    CxuiView::enterStandby();

    // If this view is still the current view, show popup.
    // If we moved to precapture view because (error) standby during capturing,
    // don't try show the popup.
    if (mMainWindow->currentView() == this) {
        // Show standby canvas / popup.
        if (!mStandbyPopup) {
            mStandbyPopup = CxuiFullScreenPopup::create(scene(), hbTrId("txt_cam_info_camera_in_standby_mode"));
        }
        mStandbyPopup->show();
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot for exiting standby mode.
* Hide the standby canvas and re-prepare camera.
* @sa CxuiView::exitStandby()
*/
void CxuiPrecaptureView::exitStandby()
{
    CX_DEBUG_ENTER_FUNCTION();
    CxuiView::exitStandby();

    if (mStandbyPopup) {
        mStandbyPopup->hide();
        delete mStandbyPopup;
        mStandbyPopup = NULL;
    }

    initCamera();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle setting dialog showing up.
* If the setting dialog was launched from the setting grid, hide grid now.
* @param action Action that launched the dialog.
*/
void CxuiPrecaptureView::prepareToShowDialog(HbAction *action)
{
    if (!action) {
        return;
    }

    // Check if the dialog was started from grid, and hide the grid now if needed.
    QString fromGrid = action->property(PROPERTY_KEY_SETTING_GRID).toString();
    if (fromGrid.compare(QString(PROPERTY_KEY_TRUE)) == 0 ) {
        hideSettingsGrid();
    }
}

/*!
* Handle setting dialog closing down.
* If the setting dialog was launched from the setting grid, show grid again now.
* @param action Action that launched the dialog.
*/
void CxuiPrecaptureView::prepareToCloseDialog(HbAction *action)
{
    // Check if the dialog was started from grid, and show the grid now if needed.
    // Autofocus key-press will clear the action to disable showing the grid.
    if (action) {
        QString fromGrid = action->property(PROPERTY_KEY_SETTING_GRID).toString();
        if (fromGrid.compare(QString(PROPERTY_KEY_TRUE)) == 0 ) {
            showSettingsGrid();
        }
        // disable geotagging disclaimer after geotagging setting is triggered from FirstTimeUse dialog
        QString settingId = action->property(PROPERTY_KEY_SETTING_ID).toString();
        CX_DEBUG(("settingsKey=%s", settingId.toAscii().constData()));
        Cxe::GeoTaggingDisclaimer value = mEngine->settings().get<Cxe::GeoTaggingDisclaimer>(CxeSettingIds::GEOTAGGING_DISCLAIMER, Cxe::GeoTaggingDisclaimerDisabled);
        if (settingId == CxeSettingIds::GEOTAGGING && value == Cxe::GeoTaggingDisclaimerEnabled) {
            // disable geotagging first-time-use dialog
            disableGeotaggingDisclaimer();
        }
    }

    // Clear the starter actions to be sure they are not reused.
    if (mSettingsDialog) {
        mSettingsDialog->setStarterAction(NULL);
    }
    if (mSliderSettingsDialog) {
        mSliderSettingsDialog->setStarterAction(NULL);
    }

    // Dialog will be deleted automatically when closed
    // The pointers may become invalid at any time
    mSettingsDialog = NULL;
    mSettingsDialogList = NULL;
    mSettingsDialogHeading = NULL;

    mSliderSettingsDialog = NULL;
    mSliderSettingsDialogHeading = NULL;
    mSettingsSlider = NULL;
}



/*!
*Helper method to create setting dialog
*/
CxuiSettingDialog* CxuiPrecaptureView::createSettingsDialog()
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("Loading popup DocML"));
    CxuiDocumentLoader *documentLoader = new CxuiDocumentLoader(mEngine);

    // Use document loader to create popup
    bool ok = false;
    documentLoader->load(SETTINGS_DIALOG_XML, &ok);
    CX_DEBUG(("load ok=%d", ok));

    CxuiSettingDialog *dialog =
        qobject_cast<CxuiSettingDialog*>(documentLoader->findWidget(SETTINGS_DIALOG_WIDGET));
    CX_ASSERT_ALWAYS(dialog);

    dialog->setTimeout(HbDialog::NoTimeout);
    dialog->setBackgroundFaded(false);

    mSettingsDialogHeading = qobject_cast<HbLabel *>(
                                    documentLoader->findWidget(SETTINGS_DIALOG_HEADING_WIDGET));
    CX_ASSERT_ALWAYS(mSettingsDialogHeading);

    mSettingsDialogList = qobject_cast<CxuiSettingRadioButtonList *>(
                                    documentLoader->findWidget(SETTINGS_DIALOG_CONTENT_WIDGET));
    CX_ASSERT_ALWAYS(mSettingsDialogList);

	// The radio button size policy defaults to "Expanding" which makes it
	// always full screen. Let's override the policy here to make the layout
	// reasonable.
    mSettingsDialogList->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,
	                                               QSizePolicy::Maximum));

    QObject *object = documentLoader->findObject(SETTINGS_DIALOG_OK_ACTION);
    mSettingsDialogOkAction = qobject_cast<HbAction *>(object);
    CX_DEBUG_ASSERT(mSettingsDialogOkAction);

    CX_DEBUG_EXIT_FUNCTION();

    delete documentLoader;
    documentLoader = NULL;

    return dialog;
}



/*!
*Helper method to create setting dialog
*/
void CxuiPrecaptureView::launchSettingsDialog(QObject* action)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!action) {
        return;
    }

    hideControls();

    QString key = action->property(PROPERTY_KEY_SETTING_ID).toString();
    CX_DEBUG(("settingsKey=%s", key.toAscii().constData()));

    RadioButtonListParams data;

    // Load data and check that the setting is supported.
    if (mSettingsInfo && mSettingsInfo->getSettingsContent(key, data)) {

        if (!mSettingsDialog) {
            mSettingsDialog = createSettingsDialog();

            connect(mSettingsDialog, SIGNAL(dialogAboutToShow(HbAction *)),
                    this, SLOT(prepareToShowDialog(HbAction *)));
            connect(mSettingsDialog, SIGNAL(dialogAboutToClose(HbAction *)),
                    this, SLOT(prepareToCloseDialog(HbAction *)));
        }

        if (mSettingsDialogHeading) {
            mSettingsDialogHeading->setPlainText(data.mHeading);
        }

        if (mSettingsDialogList) {
            mSettingsDialogList->init(&data);
        }

        if (mSettingsDialog) {
            // Store action that starts the dialog.
            // Dialog passes this along with it's signals,
            // so we can easily decide if we show grid again or not.
            mSettingsDialog->setStarterAction(qobject_cast<HbAction *>(action));

            if (mSettingsDialogOkAction) {
                // disconnect ok action from dialog so that dialog won't be closed
                // automatically when action is triggered. handleSelectionAccepted
                // is called instead
                mSettingsDialogOkAction->disconnect(mSettingsDialog);
                connect(mSettingsDialogOkAction, SIGNAL(triggered()), mSettingsDialogList, SLOT(handleSelectionAccepted()));

                // Close the dialog when new setting value is committed
                connect(mSettingsDialogList, SIGNAL(selectionCommitted()), mSettingsDialog, SLOT(close()));

                connect(mSettingsDialog, SIGNAL(aboutToClose()), mSettingsDialogList, SLOT(handleClose()));
            }

            // Adjust position and show the dialog.
            mSettingsDialog->setPreferredPos(getDialogPosition(), HbPopup::BottomRightCorner);
            mSettingsDialog->setAttribute(Qt::WA_DeleteOnClose, true);
            mSettingsDialog->show();

        }
    } else {
        launchNotSupportedNotification();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
*Helper method to create slider setting dialog
*/
CxuiSettingDialog* CxuiPrecaptureView::createSliderSettingsDialog()
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("Loading dialog DocML"));
    CxuiDocumentLoader *documentLoader = new CxuiDocumentLoader(mEngine);

    // Use document loader to create popup
    bool ok = false;
    documentLoader->load(SETTINGS_SLIDER_DIALOG_XML, &ok);
    CX_DEBUG(("load ok=%d", ok));

    CxuiSettingDialog *dialog =
        qobject_cast<CxuiSettingDialog *>(documentLoader->findWidget(SETTINGS_SLIDER_DIALOG_WIDGET));
    CX_ASSERT_ALWAYS(dialog);

    dialog->setTimeout(HbDialog::NoTimeout);
    dialog->setBackgroundFaded(false);

    mSliderSettingsDialogHeading = qobject_cast<HbLabel *>(
                                    documentLoader->findWidget(SETTINGS_SLIDER_DIALOG_HEADING_WIDGET));
    CX_ASSERT_ALWAYS(mSliderSettingsDialogHeading);

    mSettingsSlider = qobject_cast<CxuiSettingSlider *>(
                                    documentLoader->findWidget(SETTINGS_SLIDER_DIALOG_CONTENT_WIDGET));
    CX_ASSERT_ALWAYS(mSettingsSlider);

    QObject *object = documentLoader->findObject(SETTINGS_DIALOG_OK_ACTION);
    mSliderSettingsDialogOkAction = qobject_cast<HbAction *>(object);
    CX_DEBUG_ASSERT(mSliderSettingsDialogOkAction);

    CX_DEBUG_EXIT_FUNCTION();

    delete documentLoader;
    documentLoader = NULL;

    return dialog;

}


void CxuiPrecaptureView::launchSliderSetting()
{
    CX_DEBUG_ENTER_FUNCTION();

    QObject *action = sender();
    if (!action) {
         return;
    }

    hideControls();

    QString key = action->property(PROPERTY_KEY_SETTING_ID).toString();
    CX_DEBUG(("settingsKey=%s", key.toAscii().constData()));

    SliderParams data;

    // Load data and check that the setting is supported.
    if (mSettingsInfo && mSettingsInfo->getSettingsContent(key, data)) {

        if (!mSliderSettingsDialog) {
            mSliderSettingsDialog = createSliderSettingsDialog();

            connect(mSliderSettingsDialog, SIGNAL(dialogAboutToShow(HbAction *)),
                    this, SLOT(prepareToShowDialog(HbAction *)));
            connect(mSliderSettingsDialog, SIGNAL(dialogAboutToClose(HbAction *)),
                    this, SLOT(prepareToCloseDialog(HbAction *)));
        }

        if (mSliderSettingsDialogHeading) {
            mSliderSettingsDialogHeading->setPlainText(data.mHeading);
        }

        if (mSettingsSlider) {
            mSettingsSlider->init(&data);
        }

        if(mSliderSettingsDialog && mSettingsSlider) {
            // Store action that starts the dialog.
            // Dialog passes this along with it's signals,
            // so we can easily decide if we show grid again or not.
            mSliderSettingsDialog->setStarterAction(qobject_cast<HbAction *>(action));

            if (mSliderSettingsDialogOkAction) {
                // disconnect ok action from dialog so that dialog won't be closed
                // automatically when action is triggered. handleSelectionAccepted
                // is called instead
                mSliderSettingsDialogOkAction->disconnect(mSliderSettingsDialog);
                connect(mSliderSettingsDialogOkAction, SIGNAL(triggered()), mSettingsSlider, SLOT(handleSelectionAccepted()));

                // Close the dialog when new setting value is committed
                connect(mSettingsSlider, SIGNAL(selectionCommitted()), mSliderSettingsDialog, SLOT(close()));

                connect(mSliderSettingsDialog, SIGNAL(aboutToClose()), mSettingsSlider, SLOT(handleClose()));
            }

            // Adjust position and show the dialog.
            mSliderSettingsDialog->setFrameType(HbPopup::Weak);
            mSliderSettingsDialog->setPreferredPos(getDialogPosition(), HbPopup::BottomRightCorner);
            mSliderSettingsDialog->setAttribute(Qt::WA_DeleteOnClose, true);
            mSliderSettingsDialog->show();

        }
    } else {
        launchNotSupportedNotification();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/**
* Show settings grid.
*/
void CxuiPrecaptureView::showSettingsGrid()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Create grid if it's not shown before.
    initializeSettingsGrid();

    // Show grid modally.
    // This call will still not block until grid is dismissed.
    if(mSettingsGrid) {
        mSettingsGrid->setModal(true);
        mSettingsGrid->show();
        HbEffect::start(mSettingsGrid, "toolbarextension", "toolbarextension_appear");
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Hide the settings grid dialog.
*/
void CxuiPrecaptureView::hideSettingsGrid()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mSettingsGrid) {
        // Hiding of the settingsgrid will be done after effect has finished
        HbEffect::start(mSettingsGrid, "toolbarextension", "toolbarextension_disappear", this, "toolbarExtensionDisappearEffectFinished");
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Returns dialogs bottom right corner
*/
QPointF CxuiPrecaptureView::getDialogPosition()
{
    HbStyle *style = HbInstance::instance()->style();

    if (style) {
        qreal bottomMargin = 0.0f;
        qreal rightMargin = 0.0f;
        bool ok = style->parameter("hb-param-margin-gene-bottom",
                                   bottomMargin);
        CX_DEBUG_ASSERT(ok);
        ok = style->parameter("hb-param-margin-gene-right",
                              rightMargin);
        CX_DEBUG_ASSERT(ok);

        // calculate dialog's bottom right position
        QSize deviceRes =
            mEngine->viewfinderControl().deviceDisplayResolution();
        QPointF point(deviceRes.width() - rightMargin,
                      deviceRes.height() - bottomMargin);

        style = NULL; // owned by HbInstance
        return point;
    } else {
        return QPointF(); // (0, 0)
    }
}


/*!
    Get setting item icon for given value.
    \param key Setting key (CxeSettingIds)
    \param value Setting value
    \return Returns string containing icon locigal name
*/
QString CxuiPrecaptureView::getSettingItemIcon(const QString &key, QVariant value)
{
    CX_DEBUG_ENTER_FUNCTION();
    CxUiSettings::RadioButtonListParams data;
    QString icon = "";
    if (mSettingsInfo && mSettingsInfo->getSettingsContent(key, data)) {
        CxUiSettings::SettingItem setting;
        foreach (setting, data.mSettingPairList) {
            if (setting.mValue == value) {
                icon = setting.mIcon;
                break;
            }
        }
    }

    CX_DEBUG((("Setting icon name [%s]"), icon.toAscii().constData()));
    CX_DEBUG_EXIT_FUNCTION();
    return icon;
}

void CxuiPrecaptureView::handleSettingValueChanged(const QString& key, QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(key);
    Q_UNUSED(newValue);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Slot to keep track of state changes in GeotaggingLocation trail. We use these states to update
 * the UI by enabling right icon.
 */
void CxuiPrecaptureView::updateLocationIndicator(CxeGeoTaggingTrail::State newState, CxeError::Id error)
{
    CX_DEBUG( ("CxuiPrecaptureView::updateLocationIndicator <> error: %d ", error));
    Cxe::GeoTagging currentValue = mEngine->settings().get<Cxe::GeoTagging>(CxeSettingIds::GEOTAGGING);
    if(currentValue == Cxe::GeoTaggingOn && mGeoTaggingIndicatorIcon) {
        if (newState == CxeGeoTaggingTrail::DataAvailable && error == CxeError::None) {
            CX_DEBUG(("CxuiPrecaptureView::updateLocationIndicator GPS data available, showing icon"));
            mGeoTaggingIndicatorIcon->show();
            mGeoTaggingBlinkTimer->stop();
        } else {
            // blinking geotagging indicator while acquiring GPS data
            CX_DEBUG(("CxuiPrecaptureView::updateLocationIndicator waiting for GPS data, blinking icon"));
            mGeoTaggingBlinkTimer->setInterval(CXUI_GEOTAGGING_BLINK_INTERVAL);
            mGeoTaggingBlinkTimer->start();
        }
    }
    else {
        CX_DEBUG(("CxuiPrecaptureView::handleIconChanged GPS data not available"));
        mGeoTaggingBlinkTimer->stop();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Private method for blinking geotagging icon
 */
void CxuiPrecaptureView::blinkGeoTaggingIcon()
{
    if (mGeoTaggingIndicatorIcon) {
        if (mGeoTaggingIndicatorIcon->isVisible()) {
            mGeoTaggingIndicatorIcon->setVisible(false);
        } else {
            mGeoTaggingIndicatorIcon->setVisible(true);
        }
    }
}

void CxuiPrecaptureView::toolbarExtensionAppearEffectFinished(const HbEffect::EffectStatus &status) {
}

void CxuiPrecaptureView::toolbarExtensionDisappearEffectFinished(const HbEffect::EffectStatus &status) {
    if (mSettingsGrid) {
        mSettingsGrid->hide();
    }
}

/*!
* Lauches "Geotagging first-time use" notification to the user
*/
void CxuiPrecaptureView::launchGeoTaggingDisclaimerDialog()
{
    CX_DEBUG_ENTER_FUNCTION();

    HbMessageBox *ftuMessageBox = new HbMessageBox(hbTrId("txt_cam_info_captured_photos_and_videos_will_be_ta"),
                                                   HbMessageBox::MessageTypeInformation);

    HbAction *okAction = new HbAction(hbTrId("txt_common_button_ok"));
    HbAction *settingsAction = new HbAction(hbTrId("txt_cam_info_geotagging_ftu_note_settings_button"));

    // connecting signals for dialog's actions
    connect(okAction,
            SIGNAL(triggered()),
            this,
            SLOT(disableGeotaggingDisclaimer()));

    connect(settingsAction,
            SIGNAL(triggered()),
            this,
            SLOT(launchGeoTaggingSetting()));

    // adding buttons to the information message
    ftuMessageBox->setStandardButtons(HbMessageBox::NoButton);
    ftuMessageBox->addAction(okAction);
    ftuMessageBox->addAction(settingsAction);

    ftuMessageBox->setAttribute(Qt::WA_DeleteOnClose, true);
    ftuMessageBox->show();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot that disables Geotagging first-time use notification.
*/
void CxuiPrecaptureView::disableGeotaggingDisclaimer()
{
    CX_DEBUG_ENTER_FUNCTION();

    // disable geotagging first-time-use dialog
    mEngine->settings().set(CxeSettingIds::GEOTAGGING_DISCLAIMER, Cxe::GeoTaggingDisclaimerDisabled);

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Slot that launches geotagging setting dialog.
*/
void CxuiPrecaptureView::launchGeoTaggingSetting()
{
    CX_DEBUG_ENTER_FUNCTION();

    QObject *action = sender();
    action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::GEOTAGGING);
    launchSettingsDialog(action);

    CX_DEBUG_EXIT_FUNCTION();
}

// end of file

