/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsysteminfo.h"
#include "xqserviceprovider.h"
#include "xqsettingskey.h"
#include "xqsettingsmanager.h"
#include "afactivitystorage.h"
#include "radio_global.h"

/*!
 *
 */
QtMobility::QSystemDeviceInfo::QSystemDeviceInfo( QObject* parent ) :
    QObject( parent )
{
}

/*!
 *
 */
QtMobility::QSystemDeviceInfo::~QSystemDeviceInfo()
{
}

/*!
 *
 */
XQServiceProvider::XQServiceProvider( const QString&, QObject* parent ) :
    QObject( parent )
{
}

/*!
 *
 */
XQServiceProvider::~XQServiceProvider()
{
}


/*!
 *
 */
XQSettingsKey::XQSettingsKey( XQSettingsKey::Target, long int uid, unsigned long int key ) :
    mUid( uid ),
    mKey( key )
{
}

/*!
 *
 */
XQSettingsKey::~XQSettingsKey()
{
}

/*!
 *
 */
XQSettingsKey::Target XQSettingsKey::target() const
{
    return TargetCentralRepository;
}

/*!
 *
 */
long int XQSettingsKey::uid() const
{
    return mUid;
}

/*!
 *
 */
unsigned long int XQSettingsKey::key() const
{
    return mKey;
}

/*!
 *
 */
XQSettingsManager::XQSettingsManager( QObject* parent ) :
    QObject( parent )
{
}

/*!
 *
 */
XQSettingsManager::~XQSettingsManager()
{
}

/*!
 *
 */
QVariant XQSettingsManager::readItemValue( const XQSettingsKey& key, XQSettingsManager::Type )
{
    if ( key.uid() == CENREP_CORE_APPLICATION_UIS && key.key() == ID_NETWORK_CONNECTION_ALLOWED ) {
        return NetworkAllowed;
    }
    return QVariant();
}

/*!
 *
 */
bool XQSettingsManager::writeItemValue( const XQSettingsKey&, const QVariant& )
{
    return true;
}

/*!
 *
 */
bool XQSettingsManager::startMonitoring( const XQSettingsKey&, XQSettingsManager::Type )
{
    return true;
}

/*!
 *
 */
bool XQSettingsManager::stopMonitoring( const XQSettingsKey& )
{
    return true;
}

/*!
 *
 */
XQSettingsManager::Error XQSettingsManager::error() const
{
    return NoError;
}

/*!
 *
 */
AfActivityStorage::AfActivityStorage( QObject* )
{
}

/*!
 *
 */
AfActivityStorage::~AfActivityStorage()
{
}

/*!
 *
 */
bool AfActivityStorage::saveActivity( const QString&, const QVariant&, const QVariantHash& )
{
    return true;
}

/*!
 *
 */
bool AfActivityStorage::removeActivity( const QString& )
{
    return true;
}
