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
#include "cxefakesettings.h"


CxeFakeSettings::CxeFakeSettings()
{
    mVariationSettings = loadVariationSettings();
}

CxeFakeSettings::~CxeFakeSettings()
{
}


void CxeFakeSettings::get(long int uid,
         unsigned long int key,
         Cxe::SettingKeyType type,
         QVariant &value) const
{
    Q_UNUSED(uid);
    Q_UNUSED(key);
    Q_UNUSED(type);
    Q_UNUSED(value);
}

bool CxeFakeSettings::listenForSetting(const QString &settingKey, QObject *target, const char *slot)
{
    Q_UNUSED(settingKey);
    Q_UNUSED(target);
    Q_UNUSED(slot);
    return true;
}

CxeError::Id CxeFakeSettings::getVariationValue(const QString &key, QVariant &value)
{
    CxeError::Id err = CxeError::None;

    // read run-time configuration value
    if ( mVariationSettings.contains(key) ) {
        value = qVariantFromValue<QVariantList > (mVariationSettings.value(key));
    } else {
        err = CxeError::NotFound;
    }

    return err;
    //int variation = mVariationKeyHash[key];
    //value = QVariant(variation);
    //return CxeError::None;
}

void CxeFakeSettings::getValue(const QString &key, QVariant &value) const
{
    value = mSettingKeyHash[key];
}

void CxeFakeSettings::setValue(const QString &key, const QVariant &newValue)
{
    mSettingKeyHash[key] = newValue;
    emit settingValueChanged(key, newValue);
}



void CxeFakeSettings::reset()
{
}


void CxeFakeSettings::emulate(long int uid, unsigned long int key, QVariant value)
{
    emit settingValueChanged(uid, key, value);
}


/*
* Load some test settings
*/
QHash<QString, QVariantList> CxeFakeSettings::loadVariationSettings()
{
    QHash<QString, QVariantList> settings;
	QVariantList list;
	QVariant data;

    // clear the list
    list.clear();
    list << QString("32");
    list << QString("32");
    list << QString("32");
    list << QString("32");
    list << QString("32");
    //args << QVariant(QVariant::UInt);
    settings.insert(CxeVariationKeys::STILL_MAX_ZOOM_LIMITS, list);
    // clear the list
    list.clear();
    list << QString("40");
    list << QString("40");
    list << QString("40");
    list << QString("-1");
    list << QString("-1");
    settings.insert(CxeVariationKeys::VIDEO_MAX_ZOOM_LIMITS, list);
    // clear the list
    list.clear();
    list << QString("8000000");
    list << QString("10000000");
    settings.insert(CxeVariationKeys::FREE_MEMORY_LEVELS, list);
    return settings;
}
