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
 * cxesettings.h
 *
 *  Created on: Dec 30, 2008
 *      
 */
#ifndef CXESETTINGS_H_
#define CXESETTINGS_H_

#include <QObject>
#include <QString>
#include <QVariant>

#include "cxeerror.h"
#include "cxenamespace.h"
#include "cxeexception.h"

/*!
* Class to access all kind of Camera Settings
*
* There are templated get() and set() methods for accessing and modifying settings.
* Supported types are int, qreal, QString, QVariantMap and enums.
*
* It is possible to monitor changes in settings, either by connecting to settingValueChanged() signal
* or registering a slot as listener to a specific setting using listenForSetting(). With the latter
* method you will only be notified of the change in the requested setting, as opposed to the settingValueChanged()
* which will be emitted for any setting change.
*/
class CxeSettings : public QObject
{
    
    Q_OBJECT

public:


    /*!
    * Returns value of external setting item which is not owned by camera
    */
    virtual void get(long int uid,
                     unsigned long int key,
                     Cxe::SettingKeyType type,
                     QVariant &value) const = 0;

    /*!
     * Get a value of a certain setting. Template method that can be used
     * with any enumeration (well actually, anything that can be cast to from int),
     * int, QString and QVariantMap. Will throw CxeException in case of error.
     *
     * An example:
     * \code
     *  try {
     *      Cxe::Whitebalance wb = settings.get<Cxe::Whitebalance>(CxeSettingIds::WHITE_BALANCE);
     *  catch (CxeException &e) {
     *      CX_DEBUG(("Error getting white balance"));
     *  }
     * \endcode
     * @param settingId  Setting key
     * @return           Setting value
     */
    template<typename T>
    inline T get(const QString &key) const {
        QVariant v;
        getValue(key, v);
        return (T)v.value<int>();
    }

    /*!
     * Same as above but with default value in case of error. No exceptions are thrown.
     *
     *An example:
     * \code
     *
     *  Cxe::Whitebalance wb = settings.get<Cxe::Whitebalance>(CxeSettingIds::WHITE_BALANCE, Cxe::WhitebalanceAutomatic);
     *
     * \endcode
     * @param key           Setting key
     * @param defaultValue  Default value returned in case of error
     * @return              Value of the setting
     */
    template<typename T>
    inline T get(const QString &key, const T &defaultValue) const {
        try {
            return get<T>(key);
        } catch (CxeException &e) {
            return defaultValue;
        }
    }



    /*!
    * Set a value of a certain setting. Template method that can be used
    * with any enumeration (well actually, anything that can be cast to from int),
    * int, QString and QVariantMap.
    *
    * An example:
    * \code
    *  try {
    *      Cxe::Whitebalance wb = Cxe::WhiteBalanceAutomatic;
    *      settings.get<Cxe::Whitebalance>(CxeSettingIds::WHITE_BALANCE, wb);
    *  catch (CxeException &e) {
    *      CX_DEBUG(("Error setting white balance"));
    *  }
    * \endcode
    * @param settingId  Setting key
    * @param value     Setting value
    */
    template<typename T>
    inline void set(const QString &key, const T &value) {
       QVariant v;
       v.setValue((int)value);
       setValue(key, v);
    }

    /*!
    * Resets settings to default values.
    */
    virtual void reset() = 0;

    /*!
     * Get value of variation setting.
     */
    virtual CxeError::Id getVariationValue(const QString &key, QVariant &value) = 0;

    /*!
     * Add listener for changes in one setting. When the value of the setting changes, the given
     * slot is invoked on given object.
     *
     * @param settingKey Setting to listen to
     * @param target Object that the slot will be invoked for
     * @param slot Slot that will be invoked. The slot can have either of these two signatures:
     *   slotName(const QVariant&)    only new value of setting is passed as parameter
     *   slotName(const QString&, const QVariant&) both setting key and new value are passed as parameter
     * @return boolean to indicate success
     */
    virtual bool listenForSetting(const QString &settingKey, QObject *target, const char *slot) = 0;

protected:
    /*!
     * returns the setting as QVariant
     */
    virtual void getValue(const QString &key, QVariant &value) const = 0;

    /*!
    * Set new value for the given key
    */
    virtual void setValue(const QString &key, const QVariant &newValue) = 0;

signals:

    /*!
    * to notify engine and ui components for a change in a setting value
    */
    void settingValueChanged(const QString &key, QVariant newValue);

    /*!
    * to notify engine and ui components for a change in a setting value
    */
    void settingValueChanged(long int uid, unsigned long int key, QVariant value);

protected:
    CxeSettings() {}

private:
    Q_DISABLE_COPY( CxeSettings )
};

// include set/get function specializations
#include "cxesettings.inl"

#endif /*CXESETTINGS_H_*/

// end  of file
