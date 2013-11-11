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

/*!
* XQSettingsManager class fake implementation
*/

#include <QVariant>
#include <QList>
#include <QMetaType>
#include <QObject>

#include "xqsettingsmanager.h"
#include "xqsettingskey.h"
#include "cxenamespace.h"
#include "cxutils.h"




/*!
* write value to cenrep key
*/
bool XQSettingsManager::writeItemValue(const XQSettingsKey& key, const QVariant& value)
{
    CX_DEBUG_ENTER_FUNCTION();

    mError = XQSettingsManager::NoError;
    mStore[key.key()] = value;

    CX_DEBUG_EXIT_FUNCTION();
    return true;
}



/*!
* Reads the setting value for the given key from cenrep
*/
QVariant XQSettingsManager::readItemValue(const XQSettingsKey& settkey, XQSettingsManager::Type /*type*/)
{
    CX_DEBUG_ENTER_FUNCTION();
    mError = XQSettingsManager::NoError;
    if(mStore.contains(settkey.key())) {
        return mStore[settkey.key()];
    }
    return QVariant(1);
}


XQSettingsManager::Error XQSettingsManager::error() const
{
    return mError;
}


/*!
* XQSettingsManager::XQSettingsManager
*/
XQSettingsManager::XQSettingsManager(QObject* /*parent*/)
{
}



/*!
* XQSettingsManager::close
*/
XQSettingsManager::~XQSettingsManager()
{
}


/*! Instead of monitoring value changes for a P&S key 
*   this fake class implementation emits valueChanged signal for the given key
*/
bool XQSettingsManager::startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type)
{
    emit valueChanged(key, type);
    return true;
}

// end of file
