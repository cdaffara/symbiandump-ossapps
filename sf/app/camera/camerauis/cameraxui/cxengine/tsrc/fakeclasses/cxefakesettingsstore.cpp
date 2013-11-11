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
#include <QList>
#include <QHash>

#include "cxenamespace.h"
#include "cxefakesettingsstore.h"

CxeFakeSettingsStore::CxeFakeSettingsStore()
{
    resetSettings();
}

CxeFakeSettingsStore::~CxeFakeSettingsStore()
{
}

void CxeFakeSettingsStore::resetSettings()
{
    // This part should be synchronized with the current settings
    // supported by camera and defined in cxenamespace
    mSettingKeyHash.clear();
    mSettingKeyHash.insert(CxeSettingIds::FNAME_FOLDER_SUFFIX, QVariant("_Nokia"));
    mSettingKeyHash.insert(CxeSettingIds::FNAME_MONTH_FOLDER,  QVariant("08042009"));
    mSettingKeyHash.insert(CxeSettingIds::FNAME_IMAGE_COUNTER, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::FNAME_VIDEO_COUNTER, QVariant(0));

    mSettingKeyHash.insert(CxeSettingIds::CAMERA_MODE, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::FLASH_MODE, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::IMAGE_SCENE,  QVariant("image_scene_auto"));
    mSettingKeyHash.insert(CxeSettingIds::VIDEO_SCENE, QVariant("video_scene_auto"));
    mSettingKeyHash.insert(CxeSettingIds::STILL_SHOWCAPTURED, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::VIDEO_SHOWCAPTURED, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::VIDEO_MUTE_SETTING,  QVariant(0));

    mSettingKeyHash.insert(CxeSettingIds::GEOTAGGING, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::GEOTAGGING_DISCLAIMER, QVariant(1));
    mSettingKeyHash.insert(CxeSettingIds::FACE_TRACKING, QVariant(1));
    mSettingKeyHash.insert(CxeSettingIds::IMAGE_QUALITY,  QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::VIDEO_QUALITY, QVariant(0));

    mSettingKeyHash.insert(CxeSettingIds::FOCAL_RANGE, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::WHITE_BALANCE, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::LIGHT_SENSITIVITY, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::EXPOSURE_MODE, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::SHARPNESS, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::COLOR_TONE, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::EV_COMPENSATION_VALUE, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::CONTRAST, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::BRIGHTNESS, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::FRAME_RATE, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::SCENE_ID, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::SECONDARY_CAMERA, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::SELF_TIMER, QVariant(0));
    mSettingKeyHash.insert(CxeSettingIds::CAPTURE_SOUND_ALWAYS_ON, QVariant(0));
}

/*!
* Reads a value from cenrep
* @param "key"   - setting key
* @param "value" - setting value read from cenrep
*/
CxeError::Id CxeFakeSettingsStore::get(const QString& key, QVariant &value)
{
    CxeError::Id error = CxeError::None;
    if(mSettingKeyHash.contains(key)) {
        value = mSettingKeyHash[key];
    } else if(mVariationKeyHash.contains(key)) {
        value = mVariationKeyHash[key];
    } else {
        error = CxeError::NotFound;
    }

    return error;
}

/*!
* Reads a value from cenrep
* @param "uid"   - UID of the component that own setting key
* @param "key"   - setting key id
* @param "type"  - type of setting key
* @param "value" - setting value read from cenrep
*/
void CxeFakeSettingsStore::startMonitoring(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value)
{
    Q_UNUSED(uid);
    Q_UNUSED(key);
    Q_UNUSED(type);

    // Instead of emulating cenrep functionality
    // we just return a test value for verification
    value = 42;
}

/*!
* Reads/loads all variation settings values from cenrep
* @param QList<QString> contains list of all variation key ids which we use to load values from cenrep.
* returns: QHash container, "contains" values associated with each key that are read from cenrep
* NOTE: loading variation settings should be done only ONCE at start-up.
*/
QHash<QString, QVariantList> CxeFakeSettingsStore::loadVariationSettings(QList<QString> &settingKeys)
{
    QVariant data;
    CxeError::Id err = CxeError::None;
    QVariantList list;
    QHash<QString, QVariantList> settings;
    mVariationKeyHash.clear();

    foreach (QString key, settingKeys) {

        // before we read from get function we set values to the key
        mVariationKeyHash.insert(key, QVariant(1));

        // read the data from cenrep
        err = get(key, data);
        // clear the list
        list.clear();
        if (CxeError::None == err) {
           list.append(data);
        }
        // append the values associated with the key to the list.
        settings.insert(key, list);
    } // end for

    return settings;
}


/*!
* Sets a new value to cenrep
* @param "key"   - setting key
* @param "newValue" - new value set to the key in cenrep
*/
CxeError::Id CxeFakeSettingsStore::set(const QString& key, const QVariant newValue)
{
    CxeError::Id error = CxeError::NotFound;
    if(mSettingKeyHash.contains(key)) {
       mSettingKeyHash[key] = newValue;
       error =  CxeError::None;
    }
    return error;
}

/*!
* resets the cenrep store
*/
void CxeFakeSettingsStore::reset()
{
    resetSettings();
}

