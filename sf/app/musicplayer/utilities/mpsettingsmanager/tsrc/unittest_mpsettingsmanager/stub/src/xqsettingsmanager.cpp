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
* Description: xqsettingsmanager stub for testing MpSettingsManager.
*
*/

#include <qglobal.h>
#include "stub/inc/xqsettingsmanager.h"

/*!
 Constructs the information bar widget stub.
 */
XQSettingsManager::XQSettingsManager(QObject* parent)
{
    Q_UNUSED(parent);
}

/*!
 Destructs the widget stub.
 */
XQSettingsManager::~XQSettingsManager()
{
}

/*!
internal
*/
QVariant XQSettingsManager::readItemValue(const XQSettingsKey& key, XQSettingsManager::Type type)
{

    if ( mReadCount.contains( key.key() ) ) {
        mReadCount.insert(key.key(),  mReadCount.take ( key.key() ) + 1 ); 
    }
    else {
        mReadCount.insert(key.key(), 1); 
    }
    
    QVariant result;
    switch ( type ) {
        case TypeString :
            result = QString("testText") ;
            break;
        default:
            result = int(1);
            break;
    }
    return result;
}

/*!
internal
*/
bool XQSettingsManager::writeItemValue(const XQSettingsKey& key, const QVariant& value)
{
    if (mWriteCount.contains( key.key() )) {
        mWriteCount.insert(key.key(),  mWriteCount.take ( key.key() ) + 1); 
        mWriteValue.remove( key.key());
    }
    else {
        mWriteCount.insert(key.key(), 1); 
    }
    mWriteValue.insert(key.key(), value ); 
    return true;
}

/*!
internal
*/
bool XQSettingsManager::startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type)
{
    Q_UNUSED(type);
    if (mStartMonitoringCount.contains( key.key() )) {
        mStartMonitoringCount.insert(key.key(),  mStartMonitoringCount.take( key.key() ) + 1 ); 
    }
    else {
        mStartMonitoringCount.insert( key.key(), 1 ); 
    }
    return true;
}

/*!
internal
*/
int XQSettingsManager::readItemValueRequestCount(int key)
{
   return mReadCount.value( key );
}

/*!
internal
*/
int XQSettingsManager::writeItemValueRequestCount(int key)
{
   return mWriteCount.value( key );
}

/*!
internal
*/
int XQSettingsManager::startMonitoringRequestCount(int key)
{
   return mStartMonitoringCount.value( key );
}

/*!
internal
*/
QVariant XQSettingsManager::writeRequestValue(int key)
{
    return mWriteValue.value( key );
}

/*!
internal
*/
void XQSettingsManager::clear()
{
    mReadCount.clear();
    mWriteCount.clear();
    mWriteValue.clear();
    mStartMonitoringCount.clear();
}

void XQSettingsManager::emitValueChanged(const XQSettingsKey& key, const QVariant& value)
{
    emit valueChanged( key, value );
}
 