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

#include "cxescenemodestore.h"
#include "cxutils.h"
#include "cxenamespace.h"
#include "cxeautofocuscontrol.h"
#include "cxeexception.h"

using namespace Cxe;

/*!
 * Constructor.
 */
CxeSceneModeStore::CxeSceneModeStore()
{
    CX_DEBUG_ENTER_FUNCTION();
    loadImageScenes();
    loadVideoScenes();

    mCurrentImageScene = mImageSceneModes[Cxe::IMAGE_SCENE_AUTO];
    mCurrentVideoScene = mVideoSceneModes[Cxe::VIDEO_SCENE_AUTO];

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Destructor.
 */
CxeSceneModeStore::~CxeSceneModeStore()
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
* Returns scene setting value from current scene.
* @param cameraMode Camera mode used to determine which scene mode to use
* @param key Settings key
* @param[out] value Value associated with the key
* @return Error id. CxeError::None if no errors.
*/
CxeError::Id CxeSceneModeStore::sceneSettingValue(Cxe::CameraMode cameraMode, const QString &key, QVariant &value) const
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeScene scene;
    CxeError::Id err = CxeError::None;

    if(cameraMode == Cxe::ImageMode) {
        CX_DEBUG(( "CxeSceneModeStore::sceneSettingValue - Image mode Setting"));
        scene = mCurrentImageScene;
    } else {
        CX_DEBUG(( "CxeSceneModeStore::sceneSettingValue - Video mode Setting"));
        scene = mCurrentVideoScene;
    }

    if (scene.contains(key)) {
        value = scene[key];
    } else {
        err = CxeError::NotFound;
    }


    CX_DEBUG_EXIT_FUNCTION();

    return err;
}

/*!
* Sets new value to settings specific to the scene to the current scene.
* @param cameraMode Camera mode used to determine which scene mode to use
* @param key - setting id.
* @param newValue - new setting value
* @return Error id. CxeError::None if no errors.
*/
CxeError::Id CxeSceneModeStore::setSceneSettingValue(Cxe::CameraMode cameraMode, const QString &key, const QVariant &newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeError::Id err = CxeError::None;
    CxeScene *scene(0);

    if (cameraMode == Cxe::ImageMode) {
        CX_DEBUG(( "CxeSettingsImp::setSceneSettingValue - Image mode Setting"));
        scene = &mCurrentImageScene;
    } else {
        CX_DEBUG(( "CxeSettingsImp::setSceneSettingValue - Video mode Setting"));
        scene = &mCurrentVideoScene;
    }

    if (scene && scene->contains(key)) {
        CX_DEBUG(( "CxeSettingsImp::setSceneSettingValue KEY found, writing value"));
        scene->insert(key, newValue);
    } else {
        err = CxeError::NotFound;
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}

/*!
 * Returns id of current scene.
 * @param cameraMode Camera mode used to determine which scene mode to use
 * @return id of current scene mode
 */
QString CxeSceneModeStore::currentSceneId(Cxe::CameraMode cameraMode) const
{
    if (cameraMode == Cxe::ImageMode) {
        return mCurrentImageScene[CxeSettingIds::SCENE_ID].toString();
    } else {
        return mCurrentVideoScene[CxeSettingIds::SCENE_ID].toString();
    }
}
/*!
 * Returns current scene mode.
 * @param cameraMode Camera mode used to determine which scene mode to use
 * @return Current scene mode
 */
CxeScene& CxeSceneModeStore::currentScene(Cxe::CameraMode cameraMode)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (cameraMode == Cxe::ImageMode) {
        CX_DEBUG_EXIT_FUNCTION();
        return mCurrentImageScene;
    } else {
        CX_DEBUG_EXIT_FUNCTION();
        return mCurrentVideoScene;
    }
}

/*!
 * Returns current scene mode. Overloaded const version.
 */
const CxeScene& CxeSceneModeStore::currentScene(Cxe::CameraMode cameraMode) const
{
    CX_DEBUG_ENTER_FUNCTION();
    if (cameraMode == Cxe::ImageMode) {
        CX_DEBUG_EXIT_FUNCTION();
        return mCurrentImageScene;
    } else {
        CX_DEBUG_EXIT_FUNCTION();
        return mCurrentVideoScene;
    }
}
/*!
 * Set current scene mode.
 * @param cameraMode Camera mode used to determine which scene mode to use
 * @sceneId id of the scene mode to set
 */
void CxeSceneModeStore::setCurrentScene(Cxe::CameraMode cameraMode, const QString &sceneId)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("Setting scene mode: %s", sceneId.toAscii().data()));
     // @todo check that scene is valid
    if (cameraMode == Cxe::ImageMode) {
        if (!mImageSceneModes.contains(sceneId)) {
            throw CxeException(CxeError::NotFound);
        }
        mCurrentImageScene = mImageSceneModes[sceneId];
    } else {
        if (!mVideoSceneModes.contains(sceneId)) {
            throw CxeException(CxeError::NotFound);
        }
        mCurrentVideoScene = mVideoSceneModes[sceneId];
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Returns scene mode based on id.
 * @param cameraMode Camera mode used to determine which scene mode to use
 * @param sceneId id of the scene mode
 * @return scene mode
 */
CxeScene CxeSceneModeStore::getScene(Cxe::CameraMode cameraMode, const QString &sceneId) const
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("Getting scene mode: %s", sceneId.toAscii().data()));
    if (cameraMode == Cxe::ImageMode) {
        if (!mImageSceneModes.contains(sceneId)) {
            throw CxeException(CxeError::NotFound);
        }
        CX_DEBUG_EXIT_FUNCTION();
        return mImageSceneModes[sceneId];
    } else {
        if (!mVideoSceneModes.contains(sceneId)) {
            throw CxeException(CxeError::NotFound);
        }
        CX_DEBUG_EXIT_FUNCTION();
        return mVideoSceneModes[sceneId];
    }
}

/*!
* Loads all Image Scene Modes
*/
void CxeSceneModeStore::loadImageScenes()
{
    CX_DEBUG_ENTER_FUNCTION();

    mImageSceneModes.clear();

    CxeScene imgSceneAuto;

    imgSceneAuto.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_AUTO);
    imgSceneAuto.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Auto);
    imgSceneAuto.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imgSceneAuto.insert(CxeSettingIds::EXPOSURE_MODE, ExposureAuto);
    imgSceneAuto.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imgSceneAuto.insert(CxeSettingIds::CONTRAST, 0);
    imgSceneAuto.insert(CxeSettingIds::SHARPNESS, SharpnessNormal);
    imgSceneAuto.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imgSceneAuto.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imgSceneAuto.insert(CxeSettingIds::BRIGHTNESS, 0);
    imgSceneAuto.insert(CxeSettingIds::FLASH_MODE, FlashAuto);
    imgSceneAuto.insert(CxeSettingIds::FACE_TRACKING, 1);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_AUTO, imgSceneAuto);


    CxeScene imgSceneSports;

    imgSceneSports.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_SPORTS);
    imgSceneSports.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Hyperfocal);
    imgSceneSports.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imgSceneSports.insert(CxeSettingIds::EXPOSURE_MODE, ExposureSport);
    imgSceneSports.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imgSceneSports.insert(CxeSettingIds::CONTRAST, 0);
    imgSceneSports.insert(CxeSettingIds::SHARPNESS, SharpnessNormal);
    imgSceneSports.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imgSceneSports.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imgSceneSports.insert(CxeSettingIds::BRIGHTNESS, 0);
    imgSceneSports.insert(CxeSettingIds::FLASH_MODE, FlashOff);
    imgSceneSports.insert(CxeSettingIds::FACE_TRACKING, 0);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_SPORTS, imgSceneSports);


    CxeScene imgSceneCloseUp;

    imgSceneCloseUp.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_MACRO);
    imgSceneCloseUp.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Macro);
    imgSceneCloseUp.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imgSceneCloseUp.insert(CxeSettingIds::EXPOSURE_MODE, ExposureAuto);
    imgSceneCloseUp.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imgSceneCloseUp.insert(CxeSettingIds::CONTRAST, 0);
    imgSceneCloseUp.insert(CxeSettingIds::SHARPNESS, SharpnessNormal);
    imgSceneCloseUp.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imgSceneCloseUp.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imgSceneCloseUp.insert(CxeSettingIds::BRIGHTNESS, 0);
    imgSceneCloseUp.insert(CxeSettingIds::FLASH_MODE, FlashAuto);
    imgSceneCloseUp.insert(CxeSettingIds::FACE_TRACKING, 0);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_MACRO, imgSceneCloseUp);

    CxeScene imgPortraitscene;

    imgPortraitscene.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_PORTRAIT);
    imgPortraitscene.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Portrait);
    imgPortraitscene.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imgPortraitscene.insert(CxeSettingIds::EXPOSURE_MODE, ExposureBacklight);
    imgPortraitscene.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imgPortraitscene.insert(CxeSettingIds::CONTRAST, 0);
    imgPortraitscene.insert(CxeSettingIds::SHARPNESS, SharpnessSoft);
    imgPortraitscene.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imgPortraitscene.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imgPortraitscene.insert(CxeSettingIds::BRIGHTNESS, 0);
    imgPortraitscene.insert(CxeSettingIds::FLASH_MODE, FlashAntiRedEye);
    imgPortraitscene.insert(CxeSettingIds::FACE_TRACKING, 1);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_PORTRAIT, imgPortraitscene);

    CxeScene imglandscapescene;

    imglandscapescene.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_SCENERY);
    imglandscapescene.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Infinity);
    imglandscapescene.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceSunny);
    imglandscapescene.insert(CxeSettingIds::EXPOSURE_MODE, ExposureAuto);
    imglandscapescene.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imglandscapescene.insert(CxeSettingIds::CONTRAST, 0);
    imglandscapescene.insert(CxeSettingIds::SHARPNESS, SharpnessHard);
    imglandscapescene.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imglandscapescene.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imglandscapescene.insert(CxeSettingIds::BRIGHTNESS, 0);
    imglandscapescene.insert(CxeSettingIds::FLASH_MODE, FlashOff);
    imglandscapescene.insert(CxeSettingIds::FACE_TRACKING, 0);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_SCENERY, imglandscapescene);


    CxeScene imgNightscene;

    imgNightscene.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_NIGHT);
    imgNightscene.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Auto);
    imgNightscene.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imgNightscene.insert(CxeSettingIds::EXPOSURE_MODE, ExposureNight);
    imgNightscene.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imgNightscene.insert(CxeSettingIds::CONTRAST, 0);
    imgNightscene.insert(CxeSettingIds::SHARPNESS, SharpnessNormal);
    imgNightscene.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imgNightscene.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imgNightscene.insert(CxeSettingIds::BRIGHTNESS, 0);
    imgNightscene.insert(CxeSettingIds::FLASH_MODE, FlashOff);
    imgNightscene.insert(CxeSettingIds::FACE_TRACKING, 1);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_NIGHT, imgNightscene);

    CxeScene imgNightpotraitscene;

    imgNightpotraitscene.insert(CxeSettingIds::SCENE_ID, Cxe::IMAGE_SCENE_NIGHTPORTRAIT);
    imgNightpotraitscene.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Portrait);
    imgNightpotraitscene.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    imgNightpotraitscene.insert(CxeSettingIds::EXPOSURE_MODE, ExposureNight);
    imgNightpotraitscene.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    imgNightpotraitscene.insert(CxeSettingIds::CONTRAST, 0);
    imgNightpotraitscene.insert(CxeSettingIds::SHARPNESS, SharpnessNormal);
    imgNightpotraitscene.insert(CxeSettingIds::LIGHT_SENSITIVITY, LightSensitivityAutomatic);
    imgNightpotraitscene.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);
    imgNightpotraitscene.insert(CxeSettingIds::BRIGHTNESS, 0);
    imgNightpotraitscene.insert(CxeSettingIds::FLASH_MODE, FlashAntiRedEye);
    imgNightpotraitscene.insert(CxeSettingIds::FACE_TRACKING, 1);

    mImageSceneModes.insert(Cxe::IMAGE_SCENE_NIGHTPORTRAIT, imgNightpotraitscene);

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Loads all video scene modes
*/
void CxeSceneModeStore::loadVideoScenes()
{
    CX_DEBUG_ENTER_FUNCTION();

    mVideoSceneModes.clear();

    CxeScene vidSceneAuto;

    vidSceneAuto.insert(CxeSettingIds::SCENE_ID, Cxe::VIDEO_SCENE_AUTO);
    vidSceneAuto.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Hyperfocal);
    vidSceneAuto.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    vidSceneAuto.insert(CxeSettingIds::EXPOSURE_MODE, ExposureAuto);
    vidSceneAuto.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    vidSceneAuto.insert(CxeSettingIds::CONTRAST, 0);
    vidSceneAuto.insert(CxeSettingIds::FRAME_RATE, 0);
    vidSceneAuto.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);

    mVideoSceneModes.insert(Cxe::VIDEO_SCENE_AUTO,vidSceneAuto);


    CxeScene vidSceneNight;

    vidSceneNight.insert(CxeSettingIds::SCENE_ID, Cxe::VIDEO_SCENE_NIGHT);
    vidSceneNight.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Hyperfocal);
    vidSceneNight.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    vidSceneNight.insert(CxeSettingIds::EXPOSURE_MODE, ExposureNight);
    vidSceneNight.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    vidSceneNight.insert(CxeSettingIds::CONTRAST, 0);
    vidSceneNight.insert(CxeSettingIds::FRAME_RATE, 0);
    vidSceneNight.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);

    mVideoSceneModes.insert(Cxe::VIDEO_SCENE_NIGHT, vidSceneNight);


    CxeScene vidSceneLowLight;

    vidSceneLowLight.insert(CxeSettingIds::SCENE_ID, Cxe::VIDEO_SCENE_LOWLIGHT);
    vidSceneLowLight.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Hyperfocal);
    vidSceneLowLight.insert(CxeSettingIds::WHITE_BALANCE, WhitebalanceAutomatic);
    vidSceneLowLight.insert(CxeSettingIds::EXPOSURE_MODE, ExposureAuto);
    vidSceneLowLight.insert(CxeSettingIds::COLOR_TONE, ColortoneNormal);
    vidSceneLowLight.insert(CxeSettingIds::CONTRAST, 0);
    vidSceneLowLight.insert(CxeSettingIds::FRAME_RATE, 15); //fps
    vidSceneLowLight.insert(CxeSettingIds::EV_COMPENSATION_VALUE, 0);

    mVideoSceneModes.insert(Cxe::VIDEO_SCENE_LOWLIGHT, vidSceneLowLight);


    CX_DEBUG_EXIT_FUNCTION();
}
