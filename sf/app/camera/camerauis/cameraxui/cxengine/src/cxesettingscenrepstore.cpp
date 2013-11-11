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
/*
* Symbian specific settings handling
*/

#include <QVariant>
#include <QList>
#include <QMultiMap>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QVariant>
#ifdef Q_OS_SYMBIAN
#include <ProfileEngineSDKCRKeys.h>
#endif

#include "xqsettingsmanager.h" // Settings Manager API
#include "xqsettingskey.h"
#include "cxesettingscenrepstore.h"
#include "cxenamespace.h"
#include "cxutils.h"
#include "cxenamespace.h"
#include "cxeerror.h"
#include "cxecenrepkeys.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxesettingscenrepstoreTraces.h"
#endif


using namespace CxeSettingIds;


/*!
* CxeSettingsCenRepStore::CxeSettingsCenRepStore
*/
CxeSettingsCenRepStore::CxeSettingsCenRepStore()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXESETTINGSCENREPSTORE_1, "msg: e_CX_SETTINGSSTORE_CREATE 1");

    // map keys of type "string" to cenrep ids
    mapKeys();

	// we create settings handle, scope user::scope
    mSettingsManager = new XQSettingsManager(this);

    bool ok = connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
                      this, SLOT(handleValueChanged(XQSettingsKey, QVariant)));
    CX_ASSERT_ALWAYS(ok);

    CX_DEBUG(("CxeSettingsCenRepStore - mSettingsManager ptr = %d", mSettingsManager));

    OstTrace0(camerax_performance, CXESETTINGSCENREPSTORE_2, "msg: e_CX_SETTINGSSTORE_CREATE 0");
	CX_DEBUG_EXIT_FUNCTION();
}



/*
* CxeSettingsCenRepStore::~CxeSettingsCenRepStore()
*/
CxeSettingsCenRepStore::~CxeSettingsCenRepStore()
{
    CX_DEBUG_ENTER_FUNCTION();
    delete mSettingsManager;
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Generates XQSettingsKey from given setting/runtime key
* \param key Name of the setting from which to generate the XQSettingsKey
* \param[out] error Error code. CxeError::None if everything went fine.
*/
XQSettingsKey
CxeSettingsCenRepStore::generateXQSettingsKey(const QString& key, CxeError::Id& error)
{
    CX_DEBUG_ENTER_FUNCTION();

    error = CxeError::None;
	long int uid = 0;
	unsigned long int keyId = 0;

	if(mKeyMapping.contains(key)) {
	    uid = mKeyMapping[key].mRepositoryUid;
		keyId = mKeyMapping[key].mKeyId;
	} else {
	    error = CxeError::NotFound;
	}

	CX_DEBUG(( "CxeSettingsCenRepStore::generateXQSettingsKey uid = %x keyId = %d", uid, keyId ));
	XQSettingsKey newKey(XQSettingsKey::TargetCentralRepository, uid, keyId);

	CX_DEBUG_EXIT_FUNCTION();

	return newKey;
}



/*!
* Reads/loads all run-time variation settings values from cenrep
* @param QList<QString> contains list of all variation key ids which we use to load values from cenrep.
* returns: QHash container, "contains" values associated with each key that are read from cenrep
* NOTE: loading variation settings should be done only ONCE at start-up.
*/
QHash<QString, QVariantList> CxeSettingsCenRepStore::loadVariationSettings(QList<QString>& variationKeys)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXESETTINGSCENREPSTORE_LOADRUNTIME_1, "msg: e_CX_SETTINGSSTORE_LOAD_RUNTIME 1");

    QHash<QString, QVariantList> settings;
	CxeError::Id err = CxeError::None;
	QVariantList list;
	QVariant data;

    // parsing through the list of run-time variation keys and reading values from cenrep.
    foreach (QString key, variationKeys) {

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

    OstTrace0(camerax_performance, CXESETTINGSCENREPSTORE_LOADRUNTIME_2, "msg: e_CX_SETTINGSSTORE_LOAD_RUNTIME 0");
    return settings;
}



/*!
* Reads a value from cenrep
* @param key   - setting key
* @param value - setting value read from cenrep
*/

CxeError::Id CxeSettingsCenRepStore::get(const QString& key, QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mSettingsManager);

    CX_DEBUG(("CxeSettingsCenRepStore::get - key: %s", key.toAscii().constData()));

    CxeError::Id err = CxeError::None;
	XQSettingsKey mappedSettingKey = generateXQSettingsKey(key, err);

    if (CxeError::None == err) {
        CX_DEBUG(("reading values from XQSettingsManager.."));
        value = mSettingsManager->readItemValue(mappedSettingKey, mKeyMapping[key].mDataType);
        // checking if reading data from cenrep was successful
        if (mSettingsManager->error() != XQSettingsManager::NoError) {
            CX_DEBUG(("Error reading value from XQSettingsManager.."));
            err = CxeError::General;
        }
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}



/*!
* Reads a value from cenrep and starts the value change monitoring.
* @param key   - setting key
* @param uid   - setting UID of the component that owns the setting key
* @param type  - the type of key cr key or P&S key
* @param value - setting value read from cenrep
*
* @sa handleValueChanged()
*/
void CxeSettingsCenRepStore::startMonitoring(long int uid,
                                 unsigned long int key,
                                 Cxe::SettingKeyType type,
                                 QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mSettingsManager);

    XQSettingsKey::Target keyType;

    if (type == Cxe::PublishAndSubscribe) {
        keyType = XQSettingsKey::TargetPublishAndSubscribe;
    } else {
        keyType = XQSettingsKey::TargetCentralRepository;
    }

	XQSettingsKey settingsKey(keyType, uid, key);
    CX_DEBUG(("reading value from XQSettingsManager.."));
    value = mSettingsManager->readItemValue(settingsKey);

    // start monitoring changes for the key
    // both P&S and Repository keys are monitored
    bool ok = mSettingsManager->startMonitoring(settingsKey);
    if (!ok) {
        XQSettingsManager::Error error = mSettingsManager->error();
        CX_DEBUG(("CxeSettingsCenRepStore - got error %d trying to start listening", error));
        // If we try to start listening one key more than once,
        // we get this error. We can safely ignore it.
        ok = (XQSettingsManager::AlreadyExistsError == error);
    }
    CX_ASSERT_ALWAYS(ok);

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Sets a new value to cenrep
* @param key   - setting key
* @param newValue - new value set to the key in cenrep
*/
CxeError::Id CxeSettingsCenRepStore::set(const QString& key, const QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mSettingsManager);

	CxeError::Id err = CxeError::None;
    // generating key for xqsettingsmanager
    XQSettingsKey mappedSettingKey = generateXQSettingsKey(key, err);

    if (CxeError::None == err) {
        // key found
        // check if the key can be set any value or read-only
        if(mKeyMapping[key].mReadOnly) {
           err = CxeError::NotFound;
        } else {
            mSettingsManager->writeItemValue(mappedSettingKey, newValue);
            // check from xqsettingsmanager if "Set" operation was successful
            if (mSettingsManager->error() != XQSettingsManager::NoError) {
                CX_DEBUG(("Error writing value to XQSettingsManager.."));
                err = CxeError::General;
            }
        }
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}



/*!
* resets the cenrep store
*/
void CxeSettingsCenRepStore::reset()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mSettingsManager);
    //!@Todo: How to reset the repository.
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Maps CxeSettingIds to cenrep key ids that XQSettingsManager understands
*/
void CxeSettingsCenRepStore::mapKeys()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXESETTINGSCENREPSTORE_MAPKEYS_1, "msg: e_CX_SETTINGSSTORE_INIT_MAPPING 1");

    mKeyMapping.clear();

    // mapping setting keys
    addKeyMapping(CxeSettingIds::FNAME_FOLDER_SUFFIX,
                  FileNameSuffixCr,
                  XQSettingsManager::TypeString);

    addKeyMapping(CxeSettingIds::FNAME_MONTH_FOLDER,
                  FolderNameCr,
                  XQSettingsManager::TypeString);

    addKeyMapping(CxeSettingIds::FNAME_IMAGE_COUNTER,
                  FileNameImageCounterCr,
                  XQSettingsManager::TypeInt);

    addKeyMapping(CxeSettingIds::FNAME_VIDEO_COUNTER,
                  FileNameVideoCounterCr,
                  XQSettingsManager::TypeInt);

    addKeyMapping(CxeSettingIds::STILL_SHOWCAPTURED,
                  StillShowCapturedCr,
                  XQSettingsManager::TypeInt);

    addKeyMapping(CxeSettingIds::VIDEO_SHOWCAPTURED,
                  VideoShowCapturedCr,
                  XQSettingsManager::TypeInt);

    addKeyMapping(CxeSettingIds::IMAGE_QUALITY,
                  ImageQualityCr,
                  XQSettingsManager::TypeInt);

    addKeyMapping(CxeSettingIds::VIDEO_QUALITY,
                  VideoQualityCr,
                  XQSettingsManager::TypeInt);

    addKeyMapping(CxeSettingIds::VIDEO_MUTE_SETTING,
                  AudioMuteCr,
                  XQSettingsManager::TypeInt);

    addKeyMapping(CxeSettingIds::GEOTAGGING,
                  GeoTaggingCr,
                  XQSettingsManager::TypeInt);

    addKeyMapping(CxeSettingIds::FACE_TRACKING,
                  FacetrackingCr,
                  XQSettingsManager::TypeInt);

    addKeyMapping(CxeSettingIds::IMAGE_SCENE,
                  SceneModeStillCr,
                  XQSettingsManager::TypeString);

    addKeyMapping(CxeSettingIds::VIDEO_SCENE,
                  SceneModeVideoCr,
                  XQSettingsManager::TypeString);

    addKeyMapping(CxeSettingIds::FLASH_MODE,
                  FlashModeStillCr,
                  XQSettingsManager::TypeInt);

    addKeyMapping(CxeSettingIds::CAPTURE_SOUND_ALWAYS_ON,
                  CaptureSoundAlwaysOnCr,
                  XQSettingsManager::TypeInt);

    addKeyMapping(CxeSettingIds::CAMERA_MODE,
                  CameraModeCr,
                  XQSettingsManager::TypeInt);

    addKeyMapping(CxeSettingIds::GEOTAGGING_DISCLAIMER,
                  GeoTaggingDisclaimerCr,
                  XQSettingsManager::TypeInt);

    // mapping run-time variation keys
    addKeyMapping(CxeVariationKeys::FREE_MEMORY_LEVELS,
                  FreeMemoryLevelsCr,
                  XQSettingsManager::TypeString,
                  true);

    addKeyMapping(CxeVariationKeys::STILL_MAX_ZOOM_LIMITS,
                  StillMaxZoomLimitsCr,
                  XQSettingsManager::TypeString,
                  true);

    addKeyMapping(CxeVariationKeys::VIDEO_MAX_ZOOM_LIMITS,
                  VideoMaxZoomLimitsCr,
                  XQSettingsManager::TypeString,
                  true);

    OstTrace0(camerax_performance, CXESETTINGSCENREPSTORE_MAPKEYS_2, "msg: e_CX_SETTINGSSTORE_INIT_MAPPING 0");
	CX_DEBUG_EXIT_FUNCTION();
}

/*!
* helper method to construct key mappings for each setting key
*/
void CxeSettingsCenRepStore::addKeyMapping(QString key,
                                           unsigned long int keyid,
                                           XQSettingsManager::Type type,
                                           bool readOnly)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeCenRepDefinition cenrepDef;

    long int uid;
    if (readOnly) {
        uid = CxRuntimeFeaturesCrUid;
    } else {
        uid = CxSettingsCrUid;
    }
    cenrepDef.mRepositoryUid = uid;
    cenrepDef.mKeyId         = keyid;
    cenrepDef.mReadOnly      = readOnly;
    cenrepDef.mDataType      = type;

    // insert the key specific mapping to the hash
    mKeyMapping.insert(key, cenrepDef);

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
*  Handles value changed signal from the XQSettingsManager.
*  Emits signals corresponding the changed setting.
*  \sa warningTonesChanged(), settingValueChanged()
*/
void CxeSettingsCenRepStore::handleValueChanged(XQSettingsKey key, QVariant value)
{
    CX_DEBUG_ENTER_FUNCTION();
    emit settingValueChanged(key.uid(), key.key(), value);
    CX_DEBUG_EXIT_FUNCTION();
}

const QHash<QString, CxeSettingsCenRepStore::CxeCenRepDefinition>& CxeSettingsCenRepStore::keyMapping() const
{
    return mKeyMapping;
}

CxeSettingsLocalStore::CxeSettingsLocalStore()
{
    CX_DEBUG_ENTER_FUNCTION();
    // load values from cenrep
    CX_DEBUG(("Loading keys from cenrep..."));
    foreach(QString key, keyMapping().keys()) {

        QVariant value;
        CxeError::Id err = CxeSettingsCenRepStore::get(key, value);
        if (!err) {
            CX_DEBUG(("Adding key %s", key.toAscii().constData()));
            mSettings[key] = value;
        } else {
            CX_DEBUG(("Key %s not found in cenrep", key.toAscii().constData()));
        }
    }
    CX_DEBUG(("Done"));
    CX_DEBUG_EXIT_FUNCTION();
}
CxeSettingsLocalStore::~CxeSettingsLocalStore()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}


CxeError::Id CxeSettingsLocalStore::get(const QString& key, QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (useValueFromCenrep(key)) {
        CX_DEBUG(("Reading key %s directly from cenrep", key.toAscii().constData()));
        return CxeSettingsCenRepStore::get(key, value);
    }

    value = mSettings.value(key);
    if (value.isNull()) {
        CX_DEBUG(("Key %s not found", key.toAscii().constData()));
        return CxeError::NotFound;
    }

    CX_DEBUG(("Key %s, value %s", key.toAscii().constData(), value.toString().toAscii().constData()));

    CX_DEBUG_EXIT_FUNCTION();
    return CxeError::None;
}


CxeError::Id CxeSettingsLocalStore::set(const QString& key, const QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (useValueFromCenrep(key)) {
        CX_DEBUG(("Writing key %s directly to cenrep", key.toAscii().constData()));
        return CxeSettingsCenRepStore::set(key, newValue);
    }
    if (mSettings.value(key).isNull()) {
        // only allow to set keys that are mapped in CxeSettingsCenRepStore
        CX_DEBUG(("Key %s not found", key.toAscii().constData()));
        return CxeError::NotFound;
    }

    mSettings[key] = newValue;

    CX_DEBUG_EXIT_FUNCTION();
    return CxeError::None;
}

/*!
 * Returns true for keys that should be read/written from cenrep instead
 * of using cached copies. This is for some settings that need to be shared
 * between all instances of camera.
 *
 * @param key name
 * @return boolean to indicate wheter to load use cenrep value or not
 */
bool CxeSettingsLocalStore::useValueFromCenrep(const QString &key) const
{
    if (key == CxeSettingIds::FNAME_MONTH_FOLDER ||
        key == CxeSettingIds::FNAME_IMAGE_COUNTER ||
        key == CxeSettingIds::FNAME_VIDEO_COUNTER) {
        return true;
    }

    return false;

}


// end of file
