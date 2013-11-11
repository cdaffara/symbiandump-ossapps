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
#include <QVariant>
#include <QMetaType>
#include <QGraphicsLayout>

#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbtoolbar.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbtransparentwindow.h>
#include <hbslider.h>
#include <hblistwidget.h>
#include <hbdialog.h>
#include <hblistwidgetitem.h>
#include <hbtoolbarextension.h>
#include <hbtoolbutton.h>
#include <hbdeviceprofile.h> // HbDeviceProfile
#include <hbmenu.h>
#include <hbicon.h>
#include <afactivitystorage.h>
#include <hbextendedlocale.h>

#include "cxuiselftimer.h"
#include "cxeengine.h"
#include "cxeviewfindercontrol.h"
#include "cxecameradevicecontrol.h"
#include "cxestillcapturecontrol.h"
#include "cxeautofocuscontrol.h"
#include "cxuicapturekeyhandler.h"
#include "cxutils.h"
#include "cxenamespace.h" // CxeSettingIds
#include "cxuienums.h"
#include "cxesettings.h"
#include "cxefeaturemanager.h" // mEngine->featureManager()
#include "cxuidocumentloader.h"

#ifdef Q_OS_SYMBIAN
#include "OstTraceDefinitions.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxuistillprecaptureviewTraces.h"
#endif

#endif //Q_OS_SYMBIAN

#include "cxuistillprecaptureview.h"
#include "cxuiserviceprovider.h"
#include "cxuisettingdialog.h"
#include "cxuisettingradiobuttonlist.h"
#include "cxuizoomslider.h"

using namespace Cxe;
using namespace CxUiLayout;
using namespace CxUiInternal;

const int CXUI_CAPTURE_PENDING_TIMEOUT = 5000; // 5 seconds
const int CXUI_IMAGES_LEFT_LIMIT = 99; // images left is shown only if below this limit

CxuiStillPrecaptureView::CxuiStillPrecaptureView( QGraphicsItem *parent) :
    CxuiPrecaptureView(parent),
    mSelfTimer(NULL),
    mFlashSetting(NULL),
    mStillSettingsPopup(NULL),
    mSceneModePopup(NULL),
    mCapturePending(false),
    mImagesLeft(NULL),
    mImagesLeftContainer(NULL),
    mPendingAfCanceling(false)
{
    CX_DEBUG_IN_FUNCTION();
}

CxuiStillPrecaptureView::~CxuiStillPrecaptureView()
{
    CX_DEBUG_IN_FUNCTION();
    delete mSelfTimer;
}

/*!
 * Construct-method handles initialisation tasks for this class. Needs to be called
 * before the instance of this class is used.
 * @param mainwindow
 * @param engine
 * @param documentLoader
 * @param keyHandler
 */
void CxuiStillPrecaptureView::construct(HbMainWindow *mainwindow, CxeEngine *engine,
                                        CxuiDocumentLoader *documentLoader,
                                        CxuiCaptureKeyHandler *keyHandler)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXUISTILLPRECAPTUREVIEW_CONSTRUCT, "msg: e_CX_STILLPRECAPVIEW_CONSTRUCT 1" );

    // constuct base class
    CxuiPrecaptureView::construct(mainwindow, engine, documentLoader, keyHandler);

    connect(&mEngine->autoFocusControl(), SIGNAL(stateChanged(CxeAutoFocusControl::State,CxeError::Id)),
            this, SLOT(handleAutoFocusStateChanged(CxeAutoFocusControl::State,CxeError::Id)));
    connect(&mEngine->stillCaptureControl(), SIGNAL(snapshotReady(CxeError::Id, const QImage&, int)),
            this, SLOT(handleSnapshot(CxeError::Id)));
    connect(&mEngine->stillCaptureControl(), SIGNAL(stateChanged(CxeStillCaptureControl::State, CxeError::Id)),
            this, SLOT(handleStillCaptureStateChanged(CxeStillCaptureControl::State, CxeError::Id)));
    connect(&mEngine->stillCaptureControl(), SIGNAL(availableImagesChanged()),
            this, SLOT(updateImagesLeftLabel()));

    mEngine->settings().listenForSetting(CxeSettingIds::IMAGE_SCENE_DATA, this, SLOT(handleSceneChanged(const QVariant&)));

    loadDefaultWidgets();
    hideControls();

    OstTrace0( camerax_performance, DUP1_CXUISTILLPRECAPTUREVIEW_CONSTRUCT, "msg: e_CX_STILLPRECAPVIEW_CONSTRUCT 0" );
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Loads default widgets in layouts xml.
 */
void CxuiStillPrecaptureView::loadDefaultWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mDocumentLoader);

    // get pointer to the viewfinder
    QGraphicsWidget *widget = NULL;
    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_VIEWFINDER);
    mViewfinder = qobject_cast<HbTransparentWindow *>(widget);
    CX_ASSERT_ALWAYS(mViewfinder);

    reloadIndicatorWidgets();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Loads default indicators from docml and modifies the visibility
 * according to current settings.
 */
void CxuiStillPrecaptureView::reloadIndicatorWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mDocumentLoader);

    // delete old indicator container and indicators before creating new ones
    delete mIndicators;
    mIndicators = NULL;

    bool ok = false;
    mDocumentLoader->load(STILL_1ST_XML, STILL_PRE_CAPTURE_INDICATORS_SECTION, &ok);
    CX_ASSERT_ALWAYS(ok);

    QGraphicsWidget *widget = NULL;
    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_QUALITY_ICON);
    mQualityIcon = qobject_cast<HbLabel *>(widget);
    CX_ASSERT_ALWAYS(mQualityIcon);

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_FLASHBLINK_INDICATOR_ICON);
    HbLabel *flashBlinkingIcon = qobject_cast<HbLabel *>(widget);
    CX_ASSERT_ALWAYS(flashBlinkingIcon);

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_GEOTAGGING_INDICATOR_ICON);
    mGeoTaggingIndicatorIcon = qobject_cast<HbLabel *>(widget);
    CX_ASSERT_ALWAYS(mGeoTaggingIndicatorIcon);

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_FACE_TRACKING_ICON);
    mFaceTrackingIcon = qobject_cast<HbLabel *>(widget);
    CX_ASSERT_ALWAYS(mFaceTrackingIcon);

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_INDICATOR_CONTAINER);
    mIndicators = qobject_cast<HbWidget *>(widget);
    CX_ASSERT_ALWAYS(mIndicators);

    QGraphicsLayout *layout = mIndicators->layout();
    QGraphicsLayoutItem *graphicsLayoutItem = NULL;
    QGraphicsItem *graphicsItem = NULL;
    QString key = "";
    int currentSettingValue = -1;
    bool isSettingOff = false;
    // Go through the items in the layout to check whether they should be
    // shown or not in the indicator pane. Start from the last towards
    // the first, so that removing items from between works correctly.
    for (int i = layout->count() - 1; i >= 0; i--) {
        graphicsLayoutItem = layout->itemAt(i);
        isSettingOff = false;
        if (graphicsLayoutItem) {
            graphicsItem = graphicsLayoutItem->graphicsItem();
            currentSettingValue = -1;
            if (graphicsItem == mGeoTaggingIndicatorIcon) {
                key = CxeSettingIds::GEOTAGGING;
                currentSettingValue = mEngine->settings().get(key, currentSettingValue);
                if (currentSettingValue == Cxe::GeoTaggingOff) {
                    isSettingOff = true;
                }
            } else if (graphicsItem == mFaceTrackingIcon) {
                key = CxeSettingIds::FACE_TRACKING;
                currentSettingValue = mEngine->settings().get(key, currentSettingValue);
                // facetracking implementation does not use
                // enum for on/off values but instead
                // 0 for off and 1 for on.
                if (currentSettingValue == 0) {
                    isSettingOff = true;
                }
            } else if (graphicsItem == flashBlinkingIcon) {
                //remove flash indicator
                isSettingOff = true;
            }
            if (isSettingOff) {
                layout->removeAt(i);
            }
        }
    }

    // create background for indicator container
    createWidgetBackgroundGraphic(mIndicators, TRANSPARENT_BACKGROUND_GRAPHIC);
    
    // Update the icons that are possibly changed during reloading
    updateQualityIcon();
    updateFaceTrackingIcon();
    updateGeotaggingIcon();
                
    mIndicators->setVisible(true);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Loads widgets that are not part of the default section in layouts xml.
 * Widgets are created at the time they are first loaded.
 */
void CxuiStillPrecaptureView::loadWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mDocumentLoader);

    if (mWidgetsLoaded) {
        CX_DEBUG(("Widgets already loaded"));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    OstTrace0( camerax_performance, CXUISTILLPRECAPTUREVIEW_LOADWIDGETS, "msg: e_CX_STILLPRECAPTUREVIEW_LOADWIDGETS 1" );

    bool ok = false;

    OstTrace0( camerax_performance, DUP4_CXUISTILLPRECAPTUREVIEW_LOADWIDGETS, "msg: e_CX_DOCUMENTLOADER_LOAD 1" );
    mDocumentLoader->load(STILL_1ST_XML, STILL_PRE_CAPTURE_WIDGETS_SECTION, &ok);
    CX_ASSERT_ALWAYS(ok);
    if (CxuiServiceProvider::isCameraEmbedded()) {
        mDocumentLoader->load(STILL_1ST_XML, STILL_PRE_CAPTURE_EMBEDDED_SECTION, &ok);
    } else {
        mDocumentLoader->load(STILL_1ST_XML, STILL_PRE_CAPTURE_STANDALONE_SECTION, &ok);
    }
    OstTrace0( camerax_performance, DUP5_CXUISTILLPRECAPTUREVIEW_LOADWIDGETS, "msg: e_CX_DOCUMENTLOADER_LOAD 0" );
    CX_ASSERT_ALWAYS(ok);

    // get pointers to ui components from the layout data
    QGraphicsWidget *widget = NULL;
    QObject *object = NULL;

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_ZOOM_SLIDER);
    mSlider = qobject_cast<CxuiZoomSlider *>(widget);
    CX_ASSERT_ALWAYS(mSlider);
    mSlider->addZoomButtons();
    createWidgetBackgroundGraphic(mSlider, TRANSPARENT_BACKGROUND_GRAPHIC);

    // create background for selftimer containers
    HbWidget *container = NULL;
    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_SELFTIMER_CONTAINER);
    container = qobject_cast<HbWidget *>(widget);
    CX_ASSERT_ALWAYS(container);
    createWidgetBackgroundGraphic(container, TRANSPARENT_BACKGROUND_GRAPHIC);

    // connect selftimer start button to hide controls
    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_SELFTIMER_START_BUTTON);
    HbPushButton *startButton = qobject_cast<HbPushButton *>(widget);
    CX_ASSERT_ALWAYS(startButton);
    connect(startButton, SIGNAL(released()), this, SLOT(hideControls()));

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_TOOLBAR);
    mToolbar = qobject_cast<HbToolBar *>(widget);
    CX_ASSERT_ALWAYS(mToolbar);

    object = mDocumentLoader->findObject(STILL_PRE_CAPTURE_FLASH_ACTION);
    mFlashSetting = qobject_cast<HbAction *>(object);
    CX_ASSERT_ALWAYS(mFlashSetting);

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_IMAGES_LEFT_LABEL);
    mImagesLeft = qobject_cast<HbLabel *>(widget);
    CX_ASSERT_ALWAYS(mImagesLeft);

    widget = mDocumentLoader->findWidget(STILL_PRE_CAPTURE_IMAGES_LEFT_CONTAINER);
    mImagesLeftContainer = qobject_cast<HbWidget *>(widget);
    CX_ASSERT_ALWAYS(mImagesLeftContainer);
    createWidgetBackgroundGraphic(mImagesLeftContainer, TRANSPARENT_BACKGROUND_GRAPHIC);
    updateImagesLeftLabel();

    // Create self timer.
    // Let selftimer class get needed selftimer related widgets from the documentloader
    mSelfTimer = new CxuiSelfTimer(mEngine->settings());
    CX_ASSERT_ALWAYS(mSelfTimer);
    connect(mSelfTimer, SIGNAL(timerFinished()), this, SLOT(focusAndCapture()));
    mSelfTimer->loadSelftimerWidgets(mDocumentLoader);

    if (CxuiServiceProvider::isCameraEmbedded()) {
        CX_DEBUG(("EMBEDDED: camera in embedded mode"));
        setTitle(CxuiServiceProvider::instance()->windowTitle());

        if (!CxuiServiceProvider::instance()->allowQualityChange()) {

            CX_DEBUG(("EMBEDDED: don't allow quality change"));
            HbAction* iq = qobject_cast<HbAction*>(mDocumentLoader->findObject(STILL_PRE_CAPTURE_IMAGE_QUALITY_ACTION));
            if (iq) {
                CX_DEBUG(("EMBEDDED: setting image quality setting to disabled"));
                iq->setEnabled(false);
            }
        }
        if (!CxuiServiceProvider::instance()->allowModeSwitching()) {
            CX_DEBUG(("EMBEDDED: don't allow mode switching"));

            HbAction *video = qobject_cast<HbAction*>(mDocumentLoader->findObject(STILL_PRE_CAPTURE_GOTO_VIDEO_ACTION));
            if (video) {
                CX_DEBUG(("EMBEDDED: setting mode switch to disabled"));
                video->setEnabled(false);
            }
        }
        if (!CxuiServiceProvider::instance()->allowCameraSwitching()) {
            CX_DEBUG(("EMBEDDED: don't allow camera switching"));
        }

    }

    // Setting widgets loaded here so updating icons works.
    mWidgetsLoaded = true;

    // Update toolbar flash mode icon
    int flash = mEngine->settings().get<int>(CxeSettingIds::FLASH_MODE);
    handleSettingValueChanged(CxeSettingIds::FLASH_MODE, flash);

    // Update toolbar scene mode icon
    updateSceneIcon(mEngine->settings().get<QString>(CxeSettingIds::IMAGE_SCENE));

    hideControls();

    // Check if we need to show the geotagging disclaimer for first time use.
    Cxe::GeoTaggingDisclaimer value = mEngine->settings().get<Cxe::GeoTaggingDisclaimer>(CxeSettingIds::GEOTAGGING_DISCLAIMER, Cxe::GeoTaggingDisclaimerDisabled);
    if (value == Cxe::GeoTaggingDisclaimerEnabled) {
        launchGeoTaggingDisclaimerDialog();
    }

    // View is ready. Needed for startup performance automated testing.
    emit viewReady();

    OstTrace0( camerax_performance, DUP1_CXUISTILLPRECAPTUREVIEW_LOADWIDGETS, "msg: e_CX_STILLPRECAPTUREVIEW_LOADWIDGETS 0" );
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Restore view state from activity.
 */
void CxuiStillPrecaptureView::restoreActivity(const QString &activityId, const QVariant &data)
{
    Q_UNUSED(activityId);
    Q_UNUSED(data);

    CX_DEBUG_ENTER_FUNCTION();
    // no need to restore any state
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Save view state to activity.
 */
void CxuiStillPrecaptureView::saveActivity()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVariantMap data;
    QVariantHash params;
    AfActivityStorage activityStorage;

    HbIcon activityScreenshot("qtg_graf_taskswitcher_camera");
    QPixmap screenshot = activityScreenshot.pixmap();
    params.insert("screenshot", screenshot);

    activityStorage.removeActivity(
            CxuiActivityIds::STILL_PRECAPTURE_ACTIVITY);
    activityStorage.saveActivity(CxuiActivityIds::STILL_PRECAPTURE_ACTIVITY,
                                  data, params);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Clear activity from activity storage.
 */
void CxuiStillPrecaptureView::clearActivity()
{
    CX_DEBUG_ENTER_FUNCTION();
    AfActivityStorage activityStorage;
    activityStorage.removeActivity(CxuiActivityIds::STILL_PRECAPTURE_ACTIVITY);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Initializing settings grid
*/
void CxuiStillPrecaptureView::initializeSettingsGrid()
{
    if(!mSettingsGrid) {
        HbAction* action(NULL);

        // Initialize settings grid
        mSettingsGrid = new HbToolBarExtension;

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_exposure"), hbTrId("txt_cam_button_exposure_compensation"), this, SLOT(launchSliderSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::EV_COMPENSATION_VALUE);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_iso"), hbTrId("txt_cam_button_iso"), this, SLOT(launchSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::LIGHT_SENSITIVITY);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_small_rgb"), hbTrId("txt_cam_button_color_tone"), this, SLOT(launchSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::COLOR_TONE);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_white_balance"), hbTrId("txt_cam_button_white_balance"), this, SLOT(launchSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::WHITE_BALANCE);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_sharpness"), hbTrId("txt_cam_grid_sharpness"), this, SLOT(launchSliderSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::SHARPNESS);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_contrast"), hbTrId("txt_cam_button_contrast"), this, SLOT(launchSliderSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::CONTRAST);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        action = mSettingsGrid->addAction(HbIcon("qtg_mono_face_tracking"), hbTrId("txt_cam_button_face_tracking"), this, SLOT(launchSetting()));
        action->setProperty(PROPERTY_KEY_SETTING_ID, CxeSettingIds::FACE_TRACKING);
        action->setProperty(PROPERTY_KEY_SETTING_GRID, PROPERTY_KEY_TRUE);

        connect(mCaptureKeyHandler, SIGNAL(autofocusKeyPressed()), mSettingsGrid, SLOT(close()));
    }
}

/**
* Get if postcapture view should be shown or not.
* Postcapture view may be shown for a predefined time or
* until user dismisses it, or it may be completely disabled.
*/
bool CxuiStillPrecaptureView::isPostcaptureOn() const
{
    CX_DEBUG_ENTER_FUNCTION();
    if (CxuiServiceProvider::isCameraEmbedded()) {
        // always show post capture in embedded mode
        CX_DEBUG_EXIT_FUNCTION();
        return true;
    }

    // Read the value from settings. Ignoring reading error.
    // On error (missing settings) default to "postcapture on".
    int showPostcapture(-1);
    if(mEngine) {
        showPostcapture = mEngine->settings().get<int>(CxeSettingIds::STILL_SHOWCAPTURED, -1);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return showPostcapture != 0; // 0 == no postcapture
}

/*!
* Update the scene mode icon.
* @param sceneId The new scene id.
*/
void CxuiStillPrecaptureView::updateSceneIcon(const QString& sceneId)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mEngine->mode() == Cxe::ImageMode) {
        CX_DEBUG(("CxuiStillPrecaptureView - scene: %s", sceneId.toAscii().constData()));

        // No need to update icon, if widgets are not even loaded yet.
        // We'll update the icon once the widgets are loaded.
        if (mWidgetsLoaded) {

            QString iconObjectName = STILL_PRE_CAPTURE_SCENE_MODE_ACTION;
            QString icon = getSettingItemIcon(CxeSettingIds::IMAGE_SCENE, sceneId);

            CX_DEBUG(("CxuiStillPrecaptureView - icon: %s", icon.toAscii().constData()));

            if (mDocumentLoader) {
                QObject *obj = mDocumentLoader->findObject(iconObjectName);
                CX_ASSERT_ALWAYS(obj);
                qobject_cast<HbAction *>(obj)->setIcon(HbIcon(icon));
            }
        } else {
            CX_DEBUG(("CxuiStillPrecaptureView - widgets not loaded yet, ignored!"));
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Update the quality indicator
*/
void CxuiStillPrecaptureView::updateQualityIcon()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mQualityIcon && mEngine) {
        QString icon = "";
        int currentValue = mEngine->settings().get<int>(CxeSettingIds::IMAGE_QUALITY, -1);
        icon = getSettingItemIcon(CxeSettingIds::IMAGE_QUALITY, currentValue);

        mQualityIcon->setIcon(HbIcon(icon));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::handleSnapshot(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mPendingAfCanceling) {
        mPendingAfCanceling = false;
        mEngine->autoFocusControl().cancel();
    }

    if (error == CxeError::None) {
        // Enging may emit snapshot events even if we don't need the snapshot.
        // Check if postcapture is actually wanted and change view now if so.
        if(isPostcaptureOn()) {
            emit changeToPostcaptureView();
            if (mSlider) {
                mSlider->hide();
            }
        }
    } else {
        emit errorEncountered(error);
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
     Method for starting focusing which will be followed by capture.
     Meant for selftimer purposes. If autofocusing is not supported, capture
     will start without focusing.

     \sa capture()
     \sa handleAutofocusKeyPressed()
     \sa handleCaptureKeyPressed()
     \sa setCapturePending()
 */
void CxuiStillPrecaptureView::focusAndCapture()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!mEngine->autoFocusControl().supported() ||
         mEngine->autoFocusControl().isFixedFocusMode(mEngine->autoFocusControl().mode())) {
        // autofocus is not supported, so start capturing straight away
        capture();
    } else {
        setCapturePending();
        // start focusing
        // Auto-focus can only work if viewfinder is running
        if (mEngine->viewfinderControl().state() == CxeViewfinderControl::Running) {
            mEngine->autoFocusControl().start(false);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::capture()
{
    CX_DEBUG_ENTER_FUNCTION();
    mCapturePending = false;

    if (mEngine->mode() == Cxe::ImageMode) {
        // do not start capturing, if it is already ongoing
        // the user might be repeatedly triggering capture key
        if (mEngine->stillCaptureControl().state() == CxeStillCaptureControl::Ready) {
            // If focusing in progress, cancel it now.
            // Set capture pending and continue once focusing is cancelled.
            if(mEngine->autoFocusControl().state() == CxeAutoFocusControl::InProgress) {
                mEngine->autoFocusControl().cancel();
                setCapturePending();
            } else {
                // Engine uses disk space *estimate* for imagesLeft() so it
                // should not cause significant delay / additional shutter lag.
                if (mEngine->stillCaptureControl().imagesLeft()) {
                    mEngine->stillCaptureControl().reset();  //! @todo: Do not delete snapshots before images are really saved
                    mEngine->stillCaptureControl().capture();
                } else {
                    // Inform that error was encountered.
                    // Error manager will show necessary message to user.
                    emit errorEncountered(CxeError::DiskFull);
                }
            }
        } else {
            setCapturePending();
        }
    }

    // after capturing check what is the new amount for images left
    updateImagesLeftLabel();
    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::setCapturePending()
{
    CX_DEBUG_ENTER_FUNCTION();
    mCapturePending = true;
    QTimer::singleShot(CXUI_CAPTURE_PENDING_TIMEOUT, this, SLOT(resetCapturePendingFlag()));
    CX_DEBUG_EXIT_FUNCTION();
}


void CxuiStillPrecaptureView::goToVideo()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, DUP1_CXUISTILLPRECAPTUREVIEW_GOTOVIDEO, "msg: e_CX_GO_TO_VIDEO_MODE 1" );

    hideControls();
    mEngine->initMode(VideoMode);
    emit changeToPrecaptureView();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    CxuiStillPrecaptureView::handleAutoFocusStateChanged

    \sa focusAndCapture()
    \sa capture()
    \sa handleAutofocusKeyPressed()
*/
void CxuiStillPrecaptureView::handleAutoFocusStateChanged(CxeAutoFocusControl::State newstate, CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    CxeAutoFocusControl::Mode mode = mEngine->autoFocusControl().mode();
    CX_DEBUG( ("CxuiStillPrecaptureView::handleAutoFocusStateChanged <> error: %d mode: %d state %d", error, mode,
            newstate ) );


    switch (newstate) {
    case CxeAutoFocusControl::Failed:
    case CxeAutoFocusControl::Ready:
    case CxeAutoFocusControl::Unknown:
        if (mCapturePending) {
            // focus and capture were grouped together, capture straight away
            CX_DEBUG(("mCapturePending is true, starting capture now"));
            OstTrace0(camerax_performance, CXUISTILLPRECAPTUREVIEW_SHOT_TO_SHOT2, "msg: e_CX_SHOT_TO_SHOT 1");
            OstTrace0(camerax_performance, CXUISTILLPRECAPTUREVIEW_SHOT_TO_SNAP2, "msg: e_CX_SHOT_TO_SNAPSHOT 1");
            OstTrace0(camerax_performance, CXUISTILLPRECAPTUREVIEW_SHOT_TO_SAVE2, "msg: e_CX_SHOT_TO_SAVE 1");
            capture();
        }
        break;
    default:
        break;
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Handle capture key full press.
*/
void CxuiStillPrecaptureView::handleCaptureKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mEngine->mode() == Cxe::ImageMode) {
        OstTrace0(camerax_performance, CXUISTILLPRECAPTUREVIEW_SHOT_TO_SHOT, "msg: e_CX_SHOT_TO_SHOT 1");
        OstTrace0(camerax_performance, CXUISTILLPRECAPTUREVIEW_SHOT_TO_SNAP, "msg: e_CX_SHOT_TO_SNAPSHOT 1");
        OstTrace0(camerax_performance, CXUISTILLPRECAPTUREVIEW_SHOT_TO_SAVE, "msg: e_CX_SHOT_TO_SAVE 1");

        menu()->close();

        // close open dialogs
        closeDialogs();

        if (mSelfTimer && mSelfTimer->isEnabled() ) {
            if (mSelfTimer->isOngoing()) {
                // dismiss selftimer and capture image
                mSelfTimer->cancel();
                capture();
            } else {
                // start selftimer if it is enabled and not yet started
                mSelfTimer->startTimer();
            }
        } else {
            // normal capture
            capture();
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle capture key half pressed (aka "autofocus key press").
*/
void CxuiStillPrecaptureView::handleAutofocusKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXUISTILLPRECAPTUREVIEW_AUTOFOCUS, "msg: e_CX_AUTOFOCUS_LOCK 1" );

    // Base class handling needs to happen first to keep e.g. settings dialogs handling sane.
    CxuiPrecaptureView::handleAutofocusKeyPressed();

    if (!mEngine->autoFocusControl().supported()) {
        CX_DEBUG(("Autofocus not supported, ignoring key press"));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    // in case of selftimer being enabled, autofocuskey does not start focusing
    if (mSelfTimer && mSelfTimer->isEnabled()){
        CX_DEBUG(("Selftimer enabled, ignoring autofocus key press"));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    // Auto-focus can only work if viewfinder is running
    if (mEngine->viewfinderControl().state() == CxeViewfinderControl::Running) {

        mEngine->autoFocusControl().start();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::handleAutofocusKeyReleased()
{
    CX_DEBUG_ENTER_FUNCTION();

    // in case of selftimer being enabled, autofocuskey should not do anything
    if (mSelfTimer && mSelfTimer->isEnabled()){
        CX_DEBUG(("Selftimer enabled, ignoring autofocus key release"));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    if (mEngine->autoFocusControl().supported()) {
        // Check that capture is not in progress, or pending.
        // Otherwise wouldn't want to cancel the focus.
        CxeStillCaptureControl::State state(mEngine->stillCaptureControl().state());
        if (state != CxeStillCaptureControl::Capturing && !mCapturePending) {
            mEngine->autoFocusControl().cancel();
        } else {
            // set pending AF release
            mPendingAfCanceling = true;
        }

    } else {
        CX_DEBUG(("Autofocus not supported, ignoring key release"));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::showEvent(QShowEvent *event)
{
    CxuiPrecaptureView::showEvent(event);

    updateImagesLeftLabel();
    updateQualityIcon();
    updateFaceTrackingIcon();
    updateGeotaggingIcon();


    // cancel selftimer when returning to precapture
    // since selftimer needs to be turned off after capturing an image
    // (with selftimer postcapture is always shown)
    if (mSelfTimer && mSelfTimer->isEnabled()) {
        mSelfTimer->cancel();
    }

    // In case the user presses and holds the auto-focus key (ie. half-press)
    // in post-capture view, we need to start auto-focusing when entering
    // the still precapture view.
    if (event->type() == QEvent::Show &&
            mCaptureKeyHandler && mCaptureKeyHandler->isAutofocusKeyPressed()) {

        CX_DEBUG(("Still pre-capture coming visible and auto-focus key pressed -> starting AF"));
        handleAutofocusKeyPressed();
    }
}

/*!
* Allow showing UI controls?
*/
bool CxuiStillPrecaptureView::allowShowControls() const
{
    // show controls when selftimer counter is not active
    // and when autofocus key is not being pressed
    bool engineOk(mEngine && mEngine->isEngineReady());
    bool selfTimerOk(!mSelfTimer || !mSelfTimer->isOngoing());
    bool keysOk(!mCaptureKeyHandler || !mCaptureKeyHandler->isAutofocusKeyPressed());

    return engineOk && selfTimerOk && keysOk;
}

/*!
* Handle change in viewfinder state.
*/
void CxuiStillPrecaptureView::handleVfStateChanged(
    CxeViewfinderControl::State newState, CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();
    // Call base class to get standby timer and display always visible when needed.
    CxuiPrecaptureView::handleVfStateChanged(newState, error);

    if (newState == CxeViewfinderControl::Running) {

        // the toolbar is created when the viewfinder is started the first time
        // in order to hide the delay caused by the creation
        if(!mWidgetsLoaded) {
            loadWidgets();
        }

        if(mMainWindow->currentView() == this &&
            mCaptureKeyHandler->isAutofocusKeyPressed()) {
            // Viewfinder just started and the user is pressing the auto-focus key.
            // Start focusing.

            CX_DEBUG(("Viewfinder starting and auto-focus key pressed -> starting AF"));
            handleAutofocusKeyPressed();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

void CxuiStillPrecaptureView::handleStillCaptureStateChanged(
    CxeStillCaptureControl::State newState, CxeError::Id /*error*/)
{
    if (newState == CxeStillCaptureControl::Ready) {
        OstTrace0(camerax_performance, DUP1_CXUISTILLPRECAPTUREVIEW_SHOT_TO_SHOT, "msg: e_CX_SHOT_TO_SHOT 0");

        if (mCapturePending) {
            CX_DEBUG(("mCapturePending is true, starting capture now"));
            capture();
        }
    }
}

/*!
 * Signal used to reset mCapturePending after a short timeout. If the image
 * cannot be captured within a given time of the key press, it is better to cancel
 * the whole operation.
 */
void CxuiStillPrecaptureView::resetCapturePendingFlag()
{
    CX_DEBUG_IN_FUNCTION();

    mCapturePending = false;
}

/*
    Slot for handling scene mode change
    \param scene QVariantMap containing settings related to the new scene mode
 */
void CxuiStillPrecaptureView::handleSceneChanged(const QVariant &newSceneData)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mEngine->mode() == Cxe::ImageMode) {

        CxeScene scene = newSceneData.toMap();
        // update toolbar scene mode icon
        updateSceneIcon(scene[CxeSettingIds::SCENE_ID].toString());

        // for now, we are only interested in flashmode change
        if (scene.contains(CxeSettingIds::FLASH_MODE)) {
            CX_DEBUG(("updating flash to: %d", scene[CxeSettingIds::FLASH_MODE].value<int>()));
            handleSettingValueChanged(CxeSettingIds::FLASH_MODE, scene[CxeSettingIds::FLASH_MODE]);
        } else {
            // No flash mode specified within the scene.
            // Check from setting model what is it currently.
            Cxe::FlashMode flashMode = mEngine->settings().get<Cxe::FlashMode>(CxeSettingIds::FLASH_MODE, Cxe::FlashAuto);
            handleSettingValueChanged(CxeSettingIds::FLASH_MODE, QVariant(flashMode));
        }

        // If facetracking is changed, we need to update the indicator icon
        if (scene.contains(CxeSettingIds::FACE_TRACKING)) {
            handleSettingValueChanged(CxeSettingIds::FACE_TRACKING, scene[CxeSettingIds::FACE_TRACKING]);
        }

    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Enter standby mode.
*/
void CxuiStillPrecaptureView::enterStandby()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mSelfTimer && mSelfTimer->isOngoing()) {
        // If self-timer is running, stop and reset the delay now.
        mSelfTimer->reset();
    }

    // Base class handles releasing camera.
    CxuiPrecaptureView::enterStandby();

    CX_DEBUG_EXIT_FUNCTION();
}

/*
    Slot for handling setting value changes. Notice that changing the scene mode
    does not emit settingValueChanged signal.
    \param key      CxSettingIds key defining the changed setting
    \param newValue QVariant containing the new setting value

    \sa CxuiStillPrecaptureView::handleSceneChanged(CxeScene &scene)
 */
void CxuiStillPrecaptureView::handleSettingValueChanged(const QString &key, QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mEngine->mode() == Cxe::ImageMode) {
        // update images left and image quality icons
        if (key == CxeSettingIds::IMAGE_QUALITY) {
            reloadIndicatorWidgets();
            // update images left when quality values are changed
            updateImagesLeftLabel();
        } else if (key == CxeSettingIds::FACE_TRACKING ||
                   key == CxeSettingIds::GEOTAGGING) {
            reloadIndicatorWidgets();
        }

        // update toolbar flash icon
        if (mFlashSetting && key == CxeSettingIds::FLASH_MODE) {
            CX_DEBUG((("flash mode: %d"), newValue.toInt()));
            QString icon = getSettingItemIcon(key, newValue);
            CX_DEBUG((("flash mode icon: %s"), icon.toAscii().constData()))
            mFlashSetting->setIcon(HbIcon(icon));
        }

    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
    Close open dialogs.
*/
void CxuiStillPrecaptureView::closeDialogs()
{

    if (mSettingsDialog && mSettingsDialog->isVisible()) {
        mSettingsDialog->close();
    }

    if (mSceneModePopup && mSceneModePopup->isVisible()) {
        mSceneModePopup->close();
    }

    if (mStillSettingsPopup && mStillSettingsPopup->isVisible()) {
        mStillSettingsPopup->close();
    }

    hideSettingsGrid();
}


/*!
*Helper method to create setting dialog
*/
void CxuiStillPrecaptureView::launchSetting()
{
    CX_DEBUG_ENTER_FUNCTION();

    QObject *action = sender();

    if (action) {
        QString settingsKey = action->property(PROPERTY_KEY_SETTING_ID).toString();
        CX_DEBUG(("settingsKey=%s", settingsKey.toAscii().constData()));
        launchSettingsDialog(action);
        // special case to get value changed event to the selftimer class
        if (settingsKey == CxeSettingIds::SELF_TIMER) {
            // selftimer is not found in settings so set the value now
            mSettingsDialogList->setOriginalSelectedItemByValue(mSelfTimer->getTimeout());
            connect(mSettingsDialogList, SIGNAL(valueSelected(int)),
                    mSelfTimer, SLOT(changeTimeOut(int)));
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Update the images left counter
*/
void CxuiStillPrecaptureView::updateImagesLeftLabel()
{
    CX_DEBUG_ENTER_FUNCTION();

    QString imagesStr;
    HbExtendedLocale locale = HbExtendedLocale::system();

    if (mImagesLeft && mImagesLeftContainer) {
        int images = mEngine->stillCaptureControl().imagesLeft();

        if (images > CXUI_IMAGES_LEFT_LIMIT) {
            mImagesLeftContainer->hide();
        } else {
            mImagesLeftContainer->show();
        }

        CX_DEBUG(("Images left %d", images));
        imagesStr = locale.toString(images);

        mImagesLeft->setPlainText(imagesStr);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Update the face tracking icon
*/
void CxuiStillPrecaptureView::updateFaceTrackingIcon()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mFaceTrackingIcon && mEngine) {
        QString key = "";
        QString icon = "";

        key = CxeSettingIds::FACE_TRACKING;

        int currentValue = mEngine->settings().get<int>(key, -1);
        icon = getSettingItemIcon(key, currentValue);

        mFaceTrackingIcon->setIcon(HbIcon(icon));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Update the geotagging icon
*/
void CxuiStillPrecaptureView::updateGeotaggingIcon()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mGeoTaggingIndicatorIcon && mEngine) {
        QString key = "";
        QString icon = "";

        key = CxeSettingIds::GEOTAGGING;

        int currentValue = mEngine->settings().get<int>(key, -1);
        icon = getSettingItemIcon(key, currentValue);

        mGeoTaggingIndicatorIcon->setIcon(HbIcon(icon));
        mGeoTaggingIndicatorIcon->setVisible(true);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
