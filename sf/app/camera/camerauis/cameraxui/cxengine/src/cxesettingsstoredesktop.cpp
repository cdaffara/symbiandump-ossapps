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
* Description: Desktop specific settings handling
* 
*/

#include <QVariant>
#include <QList>
#include <QMultiMap>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QVariant>

#include "cxesettingsstoredesktop.h"
#include "cxenamespace.h"
#include "cxutils.h"
#include "cxenamespace.h"
#include "cxeerror.h"

using namespace Cxe;
using namespace CxeSettingIds;


/*!
* Constructor
*/
CxeSettingsStoreDesktop::CxeSettingsStoreDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();

    initSettings();

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Destructor
*/
CxeSettingsStoreDesktop::~CxeSettingsStoreDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Initalizes settings with default values.
*/
void CxeSettingsStoreDesktop::initSettings()
{
    mSettings[CAMERA_MODE] = QVariant(0);
    mSettings[FLASH_MODE] = QVariant(0);
    mSettings[WHITE_BALANCE] = QVariant(0);
    mSettings[LIGHT_SENSITIVITY] = QVariant(0);
    mSettings[EXPOSURE_MODE] = QVariant(0);
    mSettings[SHARPNESS] = QVariant(0);
    mSettings[COLOR_TONE] = QVariant(0);
    mSettings[EV_COMPENSATION_VALUE] = QVariant(0);
    mSettings[CONTRAST] = QVariant(0);
    mSettings[BRIGHTNESS] = QVariant(0);
    mSettings[SCENE_ID] = QVariant(IMAGE_SCENE_AUTO);
    mSettings[IMAGE_QUALITY] = QVariant(0);
    mSettings[VIDEO_QUALITY] = QVariant(0);
    mSettings[SELF_TIMER] = QVariant(-1);
    mSettings[FACE_TRACKING] = QVariant(0);
    mSettings[GEOTAGGING] = QVariant(0);
    mSettings[IMAGE_SCENE] = QVariant(IMAGE_SCENE_AUTO);
    mSettings[VIDEO_SCENE] = QVariant(VIDEO_SCENE_AUTO);
    mSettings[STILL_SHOWCAPTURED] = QVariant(2000);
    mSettings[VIDEO_SHOWCAPTURED] = QVariant(2000);
}



/*!
* Reads all run-time settings values from the internal hash
* \param QList<QString> contains list of all runtime key ids.
* \return QHash containing value associated to the keys
* \note loading runtime settings should be done only ONCE at start-up.
*/
QHash<QString, QVariantList> CxeSettingsStoreDesktop::loadVariationSettings(QList<QString>& runtimeKeys)
{
    CX_DEBUG_ENTER_FUNCTION();

    QHash<QString, QVariantList> settings;
    CxeError::Id err = CxeError::None;
    QVariantList list;
    QVariant data;

    // parsing through the list of run-time keys and reading values from the settings array.
    foreach (QString key, runtimeKeys) {

        // read the data from cenrep
        err = get(key, data);

        // clear the list
        list.clear();

        if (CxeError::None == err) {
            if (data.type() == QMetaType::QString ) {
                QString str = data.toString();
                QVariant strListVariant = qVariantFromValue(str.split(","));
                //generate qvariantlist from strListVariant
                list = strListVariant.toList();
            } else {
                // if its of any other type, then just append to the list
                list.append(data);
            }
        }
        // append the values associated with the key to the list.
        settings.insert(key, list);

    } // end for

    CX_DEBUG_EXIT_FUNCTION();

    return settings;
}



/*!
* Reads a setting from the internal hash
* \param "key"   - setting key
* \param "value" - setting value read from cenrep
* \return Error code
*/

CxeError::Id CxeSettingsStoreDesktop::get(const QString& key, QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("CxeSettingsStoreDesktop::get - key: %s", key.toAscii().constData()));

    if (mSettings.contains(key) )
    {
        value.setValue(mSettings.value(key));
    }

    CxeError::Id err = CxeError::None;

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}

/*!
* Reads a value from the settings store and starts monitoring it.
* \param "uid"   - UID of the component that own setting key
* \param "key"   - setting key id
* \param "type"  - type of setting key
* \param "value" - setting value read from cenrep
* \note Not only dummy implementation in desktop.
*/
void CxeSettingsStoreDesktop::startMonitoring(long int uid, unsigned long int key, Cxe::SettingKeyType type, QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();
    value = QVariant::Invalid;
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Sets a new value to the internal hash
* \param "key"   - setting key
* \param "newValue" - new value set to the key in cenrep
* \return Error code, in this case, always CxeError::None
*/
CxeError::Id CxeSettingsStoreDesktop::set(const QString& key, const QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mSettings.contains(key) )
    {
        mSettings[key] = newValue;
    }
    CxeError::Id err = CxeError::None;

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}



/*!
* resets the cenrep store
*/
void CxeSettingsStoreDesktop::reset()
{
    CX_DEBUG_ENTER_FUNCTION();
    //!@Todo: How to reset the repository.
    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
