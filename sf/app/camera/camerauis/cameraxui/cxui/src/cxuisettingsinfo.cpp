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

#include <QSize>
#include <QMetaType>
#include <QVariant>
#include <hbparameterlengthlimiter.h>

#include "cxutils.h"
#include "cxuienums.h"
#include "cxeengine.h"
#include "cxesettings.h"
#include "cxenamespace.h"
#include "cxuisettingsinfo.h"
#include "cxequalitydetails.h"
#include "cxestillcapturecontrol.h"
#include "cxevideocapturecontrol.h"
#include "cxecameradevicecontrol.h"
#include "cxuisettingradiobuttonlist.h"
#include "cxuisettingxmlreader.h"

using namespace Cxe;
using namespace CxUiSettings;

namespace
{
    const QSize KResVGA = QSize(640, 480);
    const QSize KResHD = QSize(1280, 720);
    const QSize KResWideVGA = QSize(864, 480);
    const QSize KResSharing = QSize(176,144);
}

RadioButtonListParams& RadioButtonListParams::operator=(const RadioButtonListParams& other)
{
    CX_DEBUG_ENTER_FUNCTION();
    // Protect against self-assignment
    if (this != &other)
        {
        mHeading = other.mHeading;
        mSettingId = other.mSettingId;
        mHeadingIcon = other.mHeadingIcon;
        mSettingPairList = other.mSettingPairList;
        mPreview = other.mPreview;
        mListboxType = other.mListboxType;
        }
    CX_DEBUG_EXIT_FUNCTION();
    return *this;
}

SliderParams& SliderParams::operator=(const SliderParams& other)
{
    CX_DEBUG_ENTER_FUNCTION();
    // Protect against self-assignment
    if (this != &other)
        {
        mHeading = other.mHeading;
        mSettingId = other.mSettingId;
        mHeadingIcon = other.mHeadingIcon;
        mSettingStrings = other.mSettingStrings;
        mRange = other.mRange;
        mMinorStep = other.mMinorStep;
        mMajorStep = other.mMajorStep;
        }
    CX_DEBUG_EXIT_FUNCTION();
    return *this;
}


/*!
* CxuiSettingsInfo::CxuiSettingsInfo
*/
CxuiSettingsInfo::CxuiSettingsInfo(CxeEngine *engine)
    : mMode(-1), mEngine(engine)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(engine);

    mXmlReader = new CxuiSettingXmlReader();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* CxuiSettingsInfo::~CxuiSettingsInfo
*/
CxuiSettingsInfo::~CxuiSettingsInfo()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mXmlReader;

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Fetches the settings content for radio button list
* @param key specifies the settings key
* @param settings on return contains all the data required for radiobutton list
* @return Was the setting supported or not
*/
bool CxuiSettingsInfo::getSettingsContent(const QString &key,
                                          RadioButtonListParams &settings)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Check that right content is loaded. Load now if not.
    checkMode();

    bool found(false);

    if (key == CxeSettingIds::IMAGE_QUALITY ||
        key == CxeSettingIds::VIDEO_QUALITY) {
        handleQualitySettings(key, settings);
        found = true;
    } else {
        // load settings from xml loader
        if (mXmlReader) {
            settings = mXmlReader->getListSettingInfo(key, found);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
    return found;
}



/*!
* Fetches the settings content for slider
* @Param key specifies the settings key
* @param settings on return contains all the data required for slider
*/
bool CxuiSettingsInfo::getSettingsContent(const QString &key,
                                          SliderParams &settings)
{
    CX_DEBUG_ENTER_FUNCTION();

    // Check that right content is loaded. Load now if not.
    checkMode();

    bool found(false);

    if (mXmlReader) {
        settings = mXmlReader->getSliderSettingInfo(key, found);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return found;
}





/*!
* Helper function to prepare image/video quality settings list
*/
void CxuiSettingsInfo::handleQualitySettings(const QString &key,
                                             CxUiSettings::RadioButtonListParams &settings)
{
    CX_DEBUG_ENTER_FUNCTION();

    settings.mSettingId = key;
    settings.mPreview = false;
    settings.mListboxType = CxuiSettingRadioButtonList::TwoLine;

    if (key == CxeSettingIds::IMAGE_QUALITY) {
        settings.mHeading = hbTrId("txt_cam_title_image_quality");
        getImageQualitySettings(settings);
    } else {
        settings.mHeading = hbTrId("txt_cam_title_video_quality");
        getVideoQualitySettings(settings);
    }

    CX_DEBUG_EXIT_FUNCTION();

}



/*!
* Prepares Image quality settings.
*/
void CxuiSettingsInfo::getImageQualitySettings(RadioButtonListParams &settings)
{
    CX_DEBUG_ENTER_FUNCTION();

    settings.mSettingPairList.clear();

    QList<CxeImageDetails> list =
                mEngine->stillCaptureControl().supportedImageQualities();

    int index = 0;

    // get the localized possible strings for each image quality setting
    QString vga = "txt_cam_dblist_vga";
    QString normal = "txt_cam_dblist_ln_mpix";
    QString imagesLeft = "txt_cam_dblist_hd_720p_val_ln_images_left";
    QString widescreen = "txt_cam_dblist_ln_mpix_widescreen";

    foreach(CxeImageDetails quality, list) {
        // mapping the right value for each quality

        QString settingString;
        QString qualityIcon = "";
        qreal pxCount = quality.mMpxCount.toDouble();

        if (quality.mWidth == KResVGA.width() && quality.mHeight == KResVGA.height()) {
            settingString.append(hbTrId(vga.toAscii().constData()));
            settingString.append(" ");
        }

        if (quality.mAspectRatio == Cxe::AspectRatio16to9) {
             settingString.append(hbTrId(widescreen.toAscii().constData()).arg(pxCount,0,'g',3));
        } else {
            settingString.append(hbTrId(normal.toAscii().constData()).arg(pxCount,0,'g',3));
        }

        settingString.append(NEW_LINE_CHAR);
        settingString.append(HbParameterLengthLimiter(imagesLeft.toAscii().constData(), quality.mPossibleImages));
        CX_DEBUG(( "Image quality setting string: %s", settingString.toAscii().constData()));

        if (quality.mMpxCount == "12") {
            qualityIcon = "qtg_mono_12mp";
        } else if (quality.mMpxCount == "9") {
            qualityIcon = "qtg_mono_9mp";
        } else if (quality.mMpxCount == "3.1") {
            qualityIcon = "qtg_mono_3mp";
        } else if (quality.mMpxCount == "1.2") {
            qualityIcon = "qtg_mono_1_3mp";
        } else if (quality.mMpxCount == "0.3") {
            qualityIcon = "qtg_mono_0_3mp";
        }

        SettingItem setting;
        setting.mItem = settingString;
        setting.mValue = index;
        setting.mIcon = qualityIcon;
        settings.mSettingPairList.append(setting);

        index++;
    }

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Prepares video quality settings.
*/
void CxuiSettingsInfo::getVideoQualitySettings(RadioButtonListParams &settings)
{
    CX_DEBUG_ENTER_FUNCTION();

    settings.mSettingPairList.clear();

    QList<CxeVideoDetails> list =
                mEngine->videoCaptureControl().supportedVideoQualities();

    int index = 0;

    // get the localized possible strings for each video quality setting
    QString timeleft = hbTrId("txt_cam_dblist_hd_720p_val_ln_recording_time_left");
    QString hdString = hbTrId("txt_cam_dblist_hd_720p_169_widescreen");
    QString vga = hbTrId("txt_cam_dblist_vga_43");
    QString wideVga = hbTrId("txt_cam_dblist_vga_169_widescreen"); //VGA 16:9 widescreen
    QString sharing = hbTrId("txt_cam_dblist_qcif_sharing");

    foreach(CxeVideoDetails quality, list) {
        // mapping the right value for each quality

        QString settingString = "";
        QString time = "";
        QString qualityIcon = "";

        if (quality.mWidth == KResHD.width() &&
            quality.mHeight == KResHD.height()) {
            settingString.append(hdString);
            qualityIcon = "qtg_mono_hd";
        } else if (quality.mWidth == KResVGA.width() &&
                   quality.mHeight == KResVGA.height()) {
            settingString.append(vga);
            qualityIcon = "qtg_mono_vga";
        } else if (quality.mWidth == KResSharing.width() &&
                   quality.mHeight == KResSharing.height()){
            settingString.append(sharing);
            qualityIcon = "qtg_mono_qcif";
        } else if (quality.mWidth == KResWideVGA.width() &&
                  quality.mHeight == KResWideVGA.height()) {
            settingString.append(wideVga);
            qualityIcon = "qtg_mono_vga_wide";
        }

        settingString.append(NEW_LINE_CHAR);
        time.sprintf("%02d:%02d", quality.mRemainingTime / 60, quality.mRemainingTime % 60);
        settingString.append(timeleft.arg(time));



        CX_DEBUG(( "video quality setting string: %s", settingString.toAscii().constData()));
        SettingItem setting;
        setting.mItem = settingString;
        setting.mValue = index;
        setting.mIcon = qualityIcon;
        settings.mSettingPairList.append(setting);

        index++;
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Check that the right content for current mode is loaded.
*
* UI gets "settings changed" / "scene changed" events quite
* early when switching mode (image / video). We cannot rely
* on CxeCameraDeviceControl::initModeComplete signal since
* that would be coming too late. (We would be using setting XML
* for the old mode, and setting keys/values/icons would not
* be found.) Better check this whenever UI requests for
* content for a setting, and reload the XML when needed.
* @see CxuiSettingsInfo::getSettingsContent(const QString &, RadioButtonListParams &)
* @see CxuiSettingsInfo::getSettingsContent(const QString &, SliderParams &)
* @see CxeCameraDeviceControl::initModeComplete(CxeError::Id)
*/
void CxuiSettingsInfo::checkMode()
{
    CX_DEBUG_ENTER_FUNCTION();

    int oldMode = mMode;
    mMode = mEngine->cameraDeviceControl().mode();
    if (mMode != oldMode) {
        // Select the setting XML file based on mode.
        QString sourceXml(mEngine->cameraDeviceControl().mode() == Cxe::ImageMode
                         ? CxUiSettings::IMAGE_SETTING_MAPPING_FILE
                         : CxUiSettings::VIDEO_SETTING_MAPPING_FILE);

        mXmlReader->setXmlSource(sourceXml);
    }

    CX_DEBUG_EXIT_FUNCTION();
}


// end of file
