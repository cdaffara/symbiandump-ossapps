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

#include <QCoreApplication>
#include <QVariant>
#include <QList>
#include <QMetaType>
#include <QObject>

#include "cxesettingsimp.h"
#include "cxutils.h"
#include "cxenamespace.h"
#include "cxeerror.h"
#include "cxesettingsstore.h"
#include "cxeexception.h"

#ifdef Q_OS_SYMBIAN
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxesettingsimpTraces.h"
#endif
#endif // Q_OS_SYMBIAN


// signatures for setting listener slots
const char *SETTING_LISTENER_SIGNATURE1 = "settingChanged(const QString &, const QVariant &)";
const char *SETTING_LISTENER_SIGNATURE2 = "settingChanged(const QVariant &)";


/*
* CxeSettingsImp::CxeSettingsImp
*/
CxeSettingsImp::CxeSettingsImp(CxeSettingsStore *settingStore)
: mSettingStore(settingStore),
  mVariationSettings(),
  mSceneModeStore(),
  mCameraMode(Cxe::ImageMode)
{
    CX_DEBUG_ENTER_FUNCTION();
    loadVariationSettings();

    CX_DEBUG_EXIT_FUNCTION();
}

/*
* CxeSettingsImp::close
*/
CxeSettingsImp::~CxeSettingsImp()
{
    CX_DEBUG_ENTER_FUNCTION();
    delete mSettingStore;

    mVariationSettings.clear();
    mSettingListeners.clear();
    CX_DEBUG_EXIT_FUNCTION();
}

/*
* Reads a value from cenrep
* @param key   - setting key
* @param uid   - setting UID of the component that owns the setting key
* @param type  - the type of key cr key or P&S key (constantly monitoring value)
* @param value - setting value read from cenrep
*/

void CxeSettingsImp::get(long int uid,
                         unsigned long int key,
                         Cxe::SettingKeyType type,
                         QVariant &value) const
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mSettingStore);

    mSettingStore->startMonitoring(uid, key, type, value);

    CX_DEBUG_EXIT_FUNCTION();

}


/*!
    Reset static settings (persistent settings)
*/
void CxeSettingsImp::reset()
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG_ASSERT(mSettingStore);
    mSettingStore->reset();

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Get the configured run-time value associated with the key.
* @Param key - Setting key Id ( refer to CxeSettingIds in cxenums.h )
* @Param value - contains the value associated with the key.
* @returns CxeError::None if successful or any CxeError specific error code.
*/
CxeError::Id CxeSettingsImp::getVariationValue(const QString &key, QVariant &value)
{
    CX_DEBUG_ENTER_FUNCTION();

    CxeError::Id err = CxeError::None;

    // read run-time configuration value
    if ( mVariationSettings.contains(key) ) {
        value = qVariantFromValue<QVariantList > (mVariationSettings.value(key));
    } else {
        err = CxeError::NotFound;
    }

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}


/*!
 * Add listener for changes in one setting. When the value of the setting changes, the given
 * slot is invoked on given object.
 * The validity of the settingKey is not checked.
 *
 * @param settingKey Setting to listen to
 * @param target Object that the slot will be invoked for
 * @param slot Slot that will be invoked. The slot can have either of these two signatures:
 *   slotName(const QVariant&)    only new value of setting is passed as parameter
 *   slotName(const QString&, const QVariant&) both setting key and new value are passed as parameter
 *
 * @return boolean to indicate success
 *
 */
bool CxeSettingsImp::listenForSetting(const QString &settingKey, QObject *target, const char *slot)
{

    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(target && slot);

    // SLOT() macro adds '1' to the beginning of string so we use slot+1 to get the name of the slot
    CX_DEBUG(("Adding listener %s::%s for key %s", target->metaObject()->className(), slot+1, settingKey.toAscii().data()));
    QByteArray normalizedSlotName = QMetaObject::normalizedSignature(slot+1);

    int slotIndex = target->metaObject()->indexOfSlot(normalizedSlotName);

    bool ok = false;
    if (slotIndex > -1) {
        CX_DEBUG(("Slot found, checking signature"));
        // verify that slot is correct type
        if (QMetaObject::checkConnectArgs(QMetaObject::normalizedSignature(SETTING_LISTENER_SIGNATURE1), normalizedSlotName) ||
            QMetaObject::checkConnectArgs(QMetaObject::normalizedSignature(SETTING_LISTENER_SIGNATURE2), normalizedSlotName)) {

            CX_DEBUG(("Slot signature ok, adding listener"));

            // check if list for given key already exists
            if (!mSettingListeners.contains(settingKey)) {
                mSettingListeners.insert(settingKey, CxeSettingListenerList());
            }

            // get QMetaMethod object
            QMetaMethod method = target->metaObject()->method(slotIndex);
            // add listener to the list
            CxeSettingListenerList& list = mSettingListeners[settingKey];
            list.append(CxeSettingListener(target, method));

            // connect to destroyed() signal so we can remove listener if it's deleted
            connect(target, SIGNAL(destroyed(QObject*)), this, SLOT(handleListenerDestroyed(QObject*)));

            ok = true;
        } else {
            CX_DEBUG(("Slot signature doesn't match, can't add listener"));
        }

    } else {
        CX_DEBUG(("Slot not found, can't add listener"));
    }

    CX_DEBUG_EXIT_FUNCTION();
    return ok;
}

/*!
    Load image/video specific settings during mode change or startup
*/
void CxeSettingsImp::loadSettings(Cxe::CameraMode mode)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXESETTINGSIMP_LOADSETTINGS_IN, "msg: e_CX_SETTINGS_LOADSETTINGS 1");

    mCameraMode = mode;

    // inform the settings model for the change in mode.
    if (mode == Cxe::ImageMode) {
        restoreImageSettings();
        notifyListeners(CxeSettingIds::IMAGE_SCENE_DATA, mSceneModeStore.currentScene(Cxe::ImageMode));
        notifyListeners(CxeSettingIds::IMAGE_SCENE, mSceneModeStore.currentSceneId(Cxe::ImageMode));
        emit settingValueChanged(CxeSettingIds::IMAGE_SCENE, mSceneModeStore.currentSceneId(Cxe::ImageMode));
    } else {
        restoreVideoSettings();
        notifyListeners(CxeSettingIds::VIDEO_SCENE_DATA, mSceneModeStore.currentScene(Cxe::VideoMode));
        notifyListeners(CxeSettingIds::VIDEO_SCENE, mSceneModeStore.currentSceneId(Cxe::VideoMode));
        emit settingValueChanged(CxeSettingIds::VIDEO_SCENE, mSceneModeStore.currentSceneId(Cxe::VideoMode));
    }

    OstTrace0(camerax_performance, CXESETTINGSIMP_LOADSETTINGS_OUT, "msg: e_CX_SETTINGS_LOADSETTINGS 0");
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Get setting value associated with the key.
* @Param key - Setting key Id ( refer to CxeSettingIds in cxenums.h )
* @Param value - contains the value associated with the key.
* @returns CxeError::None if successful or any CxeError specific error code.
*/
void CxeSettingsImp::getValue(const QString &key, QVariant &value) const
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mSettingStore);


    CxeError::Id err = CxeError::None;

    // check if getting scene mode
    if(key == CxeSettingIds::IMAGE_SCENE_DATA) {
        value = mSceneModeStore.currentScene(Cxe::ImageMode);
    } else if(key == CxeSettingIds::VIDEO_SCENE_DATA) {
        value = mSceneModeStore.currentScene(Cxe::VideoMode);
    } else {
        // Try first to find the item from cenrep store.
        err = mSettingStore->get(key, value);

        // If setting is not in cenrep store, try fetching it from scene settings.
        if (!err) {
            CX_DEBUG(("Got value %s from settings store", value.toString().toAscii().data()));
        } else {
            // setting not found in setting store, try finding if its scene specific setting.
            CX_DEBUG(( "fetching value from scene settings" ));
            err = mSceneModeStore.sceneSettingValue(mCameraMode, key, value);
        }
        if (err) {
            throw CxeException(err);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();

}

/*!
* Set a value to the key.
* @Param key - Setting key Id ( refer to CxeSettingIds in cxenums.h )
* @Param value - contains the value associated with the key.
* @returns CxeError::None if successful or any CxeError specific error code.
*/
void CxeSettingsImp::setValue(const QString &key, const QVariant &newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT(mSettingStore);

    CxeError::Id err;
    // check if setting scene mode
    if(key == CxeSettingIds::IMAGE_SCENE) {
        setImageScene(newValue.toString());
    } else if(key == CxeSettingIds::VIDEO_SCENE) {
        setVideoScene(newValue.toString());
    } else if(key == CxeSettingIds::IMAGE_SCENE_DATA || key == CxeSettingIds::VIDEO_SCENE_DATA) {
        // setting whole scene by value is not supported
        throw CxeException(CxeError::NotSupported);
    } else {
        // Try storing new value to cenrep
        err = mSettingStore->set(key, newValue);

        if (err) {
            CX_DEBUG(( "Key not found in cenrepstore, writing value to scene settings" ));
            err = mSceneModeStore.setSceneSettingValue(mCameraMode, key, newValue);
        }

        if (err) {
            throw CxeException(err);
        }

        // send notifications
        emit settingValueChanged(key, newValue);
        notifyListeners(key, newValue);
    }

    CX_DEBUG_EXIT_FUNCTION();

}

/*!
* Restores image settings, during mode change or during startup.
*/
void CxeSettingsImp::restoreImageSettings()
{
    CX_DEBUG_ENTER_FUNCTION();

    QString currentSceneInUse = mSceneModeStore.currentSceneId(Cxe::ImageMode);

    // get the image scene value from cenrep and load the scene settings
    QString key(CxeSettingIds::IMAGE_SCENE);
    QString cenrepSceneValue = CxeSettings::get<QString>(key);

    bool ok2LoadSceneSettings = (cenrepSceneValue != currentSceneInUse);

    if (ok2LoadSceneSettings) {
        // loading scene settings
        mSceneModeStore.setCurrentScene(Cxe::ImageMode, cenrepSceneValue);
    }

    // Updating Flash setting from cenrep
    key = CxeSettingIds::FLASH_MODE;
    int flashMode = CxeSettings::get<int>(key);

    // update local datastructure with flash setting value from cenrep.
    CX_DEBUG(( "flash setting value %d", flashMode));
    mSceneModeStore.setSceneSettingValue(Cxe::ImageMode, key, flashMode);

    // Updating Face Tracking setting from cenrep
    key = CxeSettingIds::FACE_TRACKING;
    int faceTracking = CxeSettings::get<int>(key);

    // update local datastructure with flash setting value from cenrep.
    CX_DEBUG(( "Face Tracking setting value %d", faceTracking));
    mSceneModeStore.setSceneSettingValue(Cxe::ImageMode, key, faceTracking);


    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Restores video settings, during mode change or during startup.
*/
void CxeSettingsImp::restoreVideoSettings()
{
    CX_DEBUG_ENTER_FUNCTION();

    QString currentSceneInUse = mSceneModeStore.currentSceneId(Cxe::VideoMode);

    // get the video scene value from cenrep and load the scene settings
    QString cenrepSceneValue = CxeSettings::get<QString>(CxeSettingIds::VIDEO_SCENE);

    bool ok2LoadSceneSettings = (cenrepSceneValue != currentSceneInUse);

    if (ok2LoadSceneSettings) {
        // loading video scene settings
        mSceneModeStore.setCurrentScene(Cxe::VideoMode, cenrepSceneValue);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Set new Image scene mode.
* @returns CxeError::None if successful or any CxeError specific error code.
*/
void CxeSettingsImp::setImageScene(const QString &newScene)
{
    CX_DEBUG_ENTER_FUNCTION();

    // load scene specific settings
    mSceneModeStore.setCurrentScene(Cxe::ImageMode, newScene);

    // saving current image scene to cenrep
    CxeError::Id err = mSettingStore->set(CxeSettingIds::IMAGE_SCENE, newScene);
    CxeException::throwIfError(err);

    // saving flash value from scene to cenrep
    // call CxeSettingStore::set directly because we don't want to send notifications
    // about these changes
    QString key(CxeSettingIds::FLASH_MODE);
    err = mSettingStore->set(key, mSceneModeStore.currentScene(Cxe::ImageMode)[key].toInt());

    CxeException::throwIfError(err);
    // saving face tracking value from scene to cenrep
    key = CxeSettingIds::FACE_TRACKING;
    err = mSettingStore->set(key, mSceneModeStore.currentScene(Cxe::ImageMode)[key].toInt());
    CxeException::throwIfError(err);

    // send notifications
    emit settingValueChanged(CxeSettingIds::IMAGE_SCENE, mSceneModeStore.currentSceneId(Cxe::ImageMode));
    notifyListeners(CxeSettingIds::IMAGE_SCENE, mSceneModeStore.currentSceneId(Cxe::ImageMode));
    notifyListeners(CxeSettingIds::IMAGE_SCENE_DATA, mSceneModeStore.currentScene(Cxe::ImageMode));

    CX_DEBUG_EXIT_FUNCTION();

}

/*!
* Set new video scene mode.
* @returns CxeError::None if successful or any CxeError specific error code.
*/
void CxeSettingsImp::setVideoScene(const QString &newScene)
{
    CX_DEBUG_ENTER_FUNCTION();

    mSceneModeStore.setCurrentScene(Cxe::VideoMode, newScene);

    // video scene set successfully, store the scene value to cenrep
    CxeError::Id err = mSettingStore->set(CxeSettingIds::VIDEO_SCENE, newScene);
    CxeException::throwIfError(err);

    emit settingValueChanged(CxeSettingIds::VIDEO_SCENE, mSceneModeStore.currentSceneId(Cxe::VideoMode));
    notifyListeners(CxeSettingIds::VIDEO_SCENE, mSceneModeStore.currentSceneId(Cxe::VideoMode));
    notifyListeners(CxeSettingIds::VIDEO_SCENE_DATA, mSceneModeStore.currentScene(Cxe::VideoMode));

    CX_DEBUG_EXIT_FUNCTION();

}

/*!
* Loads all run-time variation settings
*/
void CxeSettingsImp::loadVariationSettings()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT( mSettingStore );

    QList<QString> variationKeys;
    // all supported runtime variation keys are fetched from here.
    variationKeys.append(CxeVariationKeys::FREE_MEMORY_LEVELS);
    variationKeys.append(CxeVariationKeys::STILL_MAX_ZOOM_LIMITS);
    variationKeys.append(CxeVariationKeys::VIDEO_MAX_ZOOM_LIMITS);

    // load all run-time setting values from cenrep.
    mVariationSettings = mSettingStore->loadVariationSettings(variationKeys);

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Notify setting listeners that setting has changed
 * @param settingKey Setting that has changed
 * @param newValue New value of the setting
 */
void CxeSettingsImp::notifyListeners(const QString &settingKey, const QVariant &newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    CX_DEBUG(("CxeSettingsImp::notifyListeners settingKey=%s", settingKey.toAscii().data()));

    if (mSettingListeners.contains(settingKey)) {
        CX_DEBUG(("Listeners found"));
        // get list of listener
        CxeSettingListenerList& list = mSettingListeners[settingKey];

        // iterate through the list and call all listeners
        CxeSettingListenerList::const_iterator i = list.constBegin();
        for(; i != list.constEnd(); ++i) {

            QObject *object = (*i).first;
            QMetaMethod slot = (*i).second;

            CX_DEBUG_ASSERT(object);

            // invoke the slot
            CX_DEBUG(("Calling slot %s::%s", object->metaObject()->className(), slot.signature()));

            bool ok = false;
            if (slot.parameterTypes().size() == 2) {
                // slot has two parameters, send settingKey as well
                ok = slot.invoke(object,
                                  Qt::AutoConnection,
                                  Q_ARG(QString, settingKey),
                                  Q_ARG(QVariant, newValue));
            } else {
                // don't send settingKey as parameter
                ok = slot.invoke(object,
                                 Qt::AutoConnection,
                                 Q_ARG(QVariant, newValue));
            }

            if (!ok) {
                CX_DEBUG(("QMetaMethod::invoke() failed, listener not notified"))
            }

        }

    } else {
        CX_DEBUG(("NO listeners found"));
    }



    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Handle deletion of registered listener.
 */
void CxeSettingsImp::handleListenerDestroyed(QObject *object)
{
    CX_DEBUG_ENTER_FUNCTION();

    QList<QString> keyList = mSettingListeners.keys();

    for (int i = 0; i < keyList.size(); i++) {

        QString key = keyList[i];
        CxeSettingListenerList& list = mSettingListeners[key];

        for (int j = 0; j < list.size(); j++) {
            CxeSettingListener &listener = list[j];
            if (listener.first == object) {
                list.removeAt(j--);
            }
        }

        // removed last listener for this key
        if (list.size() == 0) {
            mSettingListeners.remove(key);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

// end of file
