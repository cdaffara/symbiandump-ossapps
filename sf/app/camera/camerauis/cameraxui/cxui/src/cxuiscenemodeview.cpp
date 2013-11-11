/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <hbframeitem.h>
#include <afactivitystorage.h>

#include "cxenamespace.h"
#include "cxesettings.h"
#include "cxuiscenemodeview.h"
#include "cxuienums.h"
#include "cxutils.h"
#include "cxuicapturekeyhandler.h"
#include "cxuisettingsinfo.h"
#include "cxeviewfindercontrol.h"
#include "cxecameradevicecontrol.h"
#include "cxuidocumentloader.h"

using namespace Cxe;
using namespace CxUi;
using namespace CxUiLayout;
using namespace CxUiSettings;
using namespace CxeSettingIds;

const int CXUI_SCENES_TRANSITION_TIME = 350; // 350 milliseconds
const int CXUI_SCENES_CAMERA_TIMEOUT = 60000; // 60 seconds for the camera hw release timeout
const QString CXUI_SCENES_AUTOMATIC_IMAGE=":/camerax/scene_automatic.png";
const QString CXUI_SCENES_LANDSCAPE_IMAGE=":/camerax/scene_landscape.png";
const QString CXUI_SCENES_CLOSEUP_IMAGE=":/camerax/scene_close-up.png";
const QString CXUI_SCENES_PORTRAIT_IMAGE=":/camerax/scene_portrait.png";
const QString CXUI_SCENES_SPORT_IMAGE=":/camerax/scene_sport.png";
const QString CXUI_SCENES_NIGHT_IMAGE=":/camerax/scene_night.png";
const QString CXUI_SCENES_NIGHT_PORTRAIT_IMAGE=":/camerax/scene_nightportrait.png";
const QString CXUI_SCENES_LOW_LIGHT_IMAGE=":/camerax/scene_lowlight.png";


/*!
* Constructor
*/
CxuiSceneModeView::CxuiSceneModeView(QGraphicsItem *parent) :
    CxuiView(parent),
    mSettingsInfo(NULL),
    mScenesBackground(NULL),
    mScenesHeading(NULL)
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
* Destructor
*/
CxuiSceneModeView::~CxuiSceneModeView()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Construct-method handles initialisation tasks for this class. Needs to be called
* before the instance of this class is used.
* @param mainwindow
* @param engine
* @param documentLoader
* @param keyHandler
*/
void CxuiSceneModeView::construct(HbMainWindow *mainwindow,
                               CxeEngine *engine,
                               CxuiDocumentLoader *documentLoader,
                               CxuiCaptureKeyHandler *keyHandler)
{
    CX_DEBUG_ENTER_FUNCTION();
    CxuiView::construct(mainwindow, engine, documentLoader, keyHandler);

    mSettingsInfo = new CxuiSettingsInfo(mEngine);
    setContentFullScreen(true);
    loadDefaultWidgets();

    mCameraReleaseTimer.setInterval(CXUI_SCENES_CAMERA_TIMEOUT);
    mCameraReleaseTimer.setSingleShot(true);
    connect(&mCameraReleaseTimer, SIGNAL(timeout()), this, SLOT(releaseCamera()), Qt::UniqueConnection);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Load widgets from DocML.
*/
void CxuiSceneModeView::loadDefaultWidgets()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mDocumentLoader);

    QGraphicsWidget *widget = NULL;

    widget = mDocumentLoader->findWidget(SCENE_MODE_VIEW_CONTAINER);
    mScenesContainer = qobject_cast<HbWidget *> (widget);

    mScenesHeading = qobject_cast<HbLabel *>(
        mDocumentLoader->findWidget(SCENE_MODE_VIEW_HEADING_WIDGET));
    CX_ASSERT_ALWAYS(mScenesHeading);

    //Now let's retreive the pointer to icon widget
    widget = mDocumentLoader->findWidget(SCENE_MODE_VIEW_BG_IMAGE);
    mScenesBackground = qobject_cast<HbLabel *> (widget);

    widget = mDocumentLoader->findWidget(SCENE_MODE_VIEW_BG_IMAGE2);
    mScenesBackground2 = qobject_cast<HbLabel *> (widget);

    //Assuming that the automatic scene mode is always the default one
    CX_DEBUG(("CxuiSceneModeView::loadDefaultWidgets -> Now setting default image"));
    HbIcon background(CXUI_SCENES_AUTOMATIC_IMAGE);
    background.setMirroringMode(HbIcon::LayoutDirection);
    mScenesBackground->setIcon(background);
    mScenesBackground2->setIcon(background);

    widget = mDocumentLoader->findWidget(SCENE_MODE_VIEW_RADIOBUTTONS);
    mScenesList = qobject_cast<CxuiSettingRadioButtonList *> (widget);

    mTransitionAnimation = new QPropertyAnimation(mScenesBackground2, "opacity");
    mTransitionAnimation->setDuration(CXUI_SCENES_TRANSITION_TIME);

    createWidgetBackgroundGraphic(mScenesContainer, TRANSPARENT_BACKGROUND_GRAPHIC);

    //!@todo: View flags property is missing from HbView,
    //        so can't properly hide title bar / status bar there.
    hideControls();

    connectSignals();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Helper function to connect signals needed in this class.
*/
void CxuiSceneModeView::connectSignals()
{
    connect(mScenesList, SIGNAL(itemSelected(int)), this, SLOT(handleSceneRadiobuttonPress(int)));
    connect(mTransitionAnimation, SIGNAL(finished()), this, SLOT(finishScenesTransition()));
}

/*!
* This public method assumes that the view is already properly constructed
*/
void CxuiSceneModeView::loadBackgroundImages()
{
     CX_DEBUG_ENTER_FUNCTION();
    RadioButtonListParams data;
    if (mEngine->mode() == Cxe::ImageMode) {
        mSettingsInfo->getSettingsContent(CxeSettingIds::IMAGE_SCENE, data);
    } else {
        mSettingsInfo->getSettingsContent(CxeSettingIds::VIDEO_SCENE, data);
    }
    mSettingPairList = data.mSettingPairList;
    mScenesList->init(&data);

    if (mScenesHeading) {
        mScenesHeading->setPlainText(data.mHeading);
    }

    if (mScenesBackground) {
        QString sceneId = mEngine->settings().get<QString>(data.mSettingId);
        HbIcon background(backgroundForScene(sceneId));
        background.setMirroringMode(HbIcon::LayoutDirection);
        mScenesBackground->setIcon(background);
    } else {
        //First time displaying a list
        //Assuming that the automatic scene mode is always the default one and is on top of the list
        mScenesList->setSelected(0);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Save view state to activity. Scene mode view doesn't have it's own activity, just save
 * correct pre-capture view.
 */
void CxuiSceneModeView::saveActivity()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVariantMap data;
    QVariantHash params;
    AfActivityStorage activityStorage;

    if (mEngine->mode() == Cxe::ImageMode) {
        HbIcon activityScreenshot("qtg_graf_taskswitcher_camera");
        QPixmap screenshot = activityScreenshot.pixmap();
        params.insert("screenshot", screenshot);

        activityStorage.removeActivity(CxuiActivityIds::STILL_PRECAPTURE_ACTIVITY);
        activityStorage.saveActivity(CxuiActivityIds::STILL_PRECAPTURE_ACTIVITY, data, params);
    } else {
        HbIcon activityScreenshot("qtg_graf_taskswitcher_camcorder");
        QPixmap screenshot = activityScreenshot.pixmap();
        params.insert("screenshot", screenshot);

        activityStorage.removeActivity(CxuiActivityIds::VIDEO_PRECAPTURE_ACTIVITY);
        activityStorage.saveActivity(CxuiActivityIds::VIDEO_PRECAPTURE_ACTIVITY, data, params);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle selecting value in scene list.
*/
void CxuiSceneModeView::handleSceneRadiobuttonPress(int index)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxUiSettings::SettingItem item = mSettingPairList.at(index);
    QString sceneId = item.mValue.toString();
    HbIcon background(backgroundForScene(sceneId));
    background.setMirroringMode(HbIcon::LayoutDirection);
    mScenesBackground2->setIcon(background);
    startBackgroundTransition();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Helper method for getting the background graphic name
* for each of the scenes.
*/
QString CxuiSceneModeView::backgroundForScene(const QString& sceneId)
{
    //!@todo: This mapping should be added to the setting xml.
    if (sceneId == Cxe::IMAGE_SCENE_AUTO) {
        return CXUI_SCENES_AUTOMATIC_IMAGE;
    } else if (sceneId == Cxe::IMAGE_SCENE_PORTRAIT) {
        return CXUI_SCENES_PORTRAIT_IMAGE;
    } else if (sceneId == Cxe::IMAGE_SCENE_SCENERY) {
        return CXUI_SCENES_LANDSCAPE_IMAGE;
    } else if (sceneId == Cxe::IMAGE_SCENE_MACRO) {
        return CXUI_SCENES_CLOSEUP_IMAGE;
    } else if (sceneId == Cxe::IMAGE_SCENE_SPORTS) {
        return CXUI_SCENES_SPORT_IMAGE;
    } else if (sceneId == Cxe::IMAGE_SCENE_NIGHT) {
        return CXUI_SCENES_NIGHT_IMAGE;
    } else if (sceneId == Cxe::IMAGE_SCENE_NIGHTPORTRAIT) {
        return CXUI_SCENES_NIGHT_PORTRAIT_IMAGE;
    } else if (sceneId == Cxe::VIDEO_SCENE_AUTO) {
        return CXUI_SCENES_AUTOMATIC_IMAGE;
    } else if (sceneId == Cxe::VIDEO_SCENE_LOWLIGHT) {
        return CXUI_SCENES_LOW_LIGHT_IMAGE;
    } else if (sceneId == Cxe::VIDEO_SCENE_NIGHT) {
        return CXUI_SCENES_NIGHT_IMAGE;
    } else {
        return "";
    }

}

/*!
* Start animation for changing the scene background graphic.
*/
void CxuiSceneModeView::startBackgroundTransition()
{
    mTransitionAnimation->setStartValue(0.0);
    mTransitionAnimation->setEndValue(1.0);
    mTransitionAnimation->start();

}

/*!
* Handle scene background graphic animation finishing.
*/
void CxuiSceneModeView::finishScenesTransition()
{
    mScenesBackground->setIcon(mScenesBackground2->icon());
    mScenesBackground2->setOpacity(0);
    mScenesBackground2->setIcon(HbIcon());
}

/*!
* Handle press of ok button.
*/
void CxuiSceneModeView::handleOkButtonPress()
{
    CX_DEBUG_ENTER_FUNCTION();
    mScenesList->handleSelectionAccepted();
    closeView();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle press of cancel button.
*/
void CxuiSceneModeView::handleCancelButtonPress()
{
    CX_DEBUG_ENTER_FUNCTION();
    closeView();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* This view is about to be shown.
* Update the view to match currently selected scene.
*/
void CxuiSceneModeView::showEvent(QShowEvent *event)
{
    CX_DEBUG_ENTER_FUNCTION();
    mScenesList->scrollTo(mScenesList->currentIndex(), HbAbstractItemView::PositionAtTop);

    mEngine->viewfinderControl().stop();
    mCameraReleaseTimer.start();
    QGraphicsWidget::showEvent(event);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Allow showing UI controls?
* Title bar and other UI chrome is never shown in scene mode view.
*/
bool CxuiSceneModeView::allowShowControls() const
{
    return false;
}

/*!
 * Play feedback when touching view outside of any widget?
 * Feedback is not played in scene mode view.
 */
bool CxuiSceneModeView::isFeedbackEnabled() const
{
    return false;
}

/*!
* Slot to handle capture key full press.
*/
void CxuiSceneModeView::handleCaptureKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();
    closeView();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot to handle capture key half press / autofocus key press.
*/
void CxuiSceneModeView::handleAutofocusKeyPressed()
{
    CX_DEBUG_ENTER_FUNCTION();
    closeView();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Closing the view and reactivating camera hw if needed
*/
void CxuiSceneModeView::closeView()
{
    CX_DEBUG_ENTER_FUNCTION();
    mScenesList->handleClose();
    mScenesBackground->setIcon(HbIcon());
    mScenesHeading = NULL;
    emit viewCloseEvent();
    CX_DEBUG_EXIT_FUNCTION();
}

// end of file

