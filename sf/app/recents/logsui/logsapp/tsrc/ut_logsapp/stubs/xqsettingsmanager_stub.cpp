/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qglobal.h>
#include <QVariant>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>

bool XQSettingsManager::mFailed = false;
int XQSettingsManager::mCurrentVal = 0;
    
//////////////////////////////////////////////////////////////////////////////////////////
//  XQSettingsManager
//////////////////////////////////////////////////////////////////////////////////////////

XQSettingsManager::XQSettingsManager()
{
}

XQSettingsManager::~XQSettingsManager()
{
    
}

QVariant XQSettingsManager::readItemValue(const XQSettingsKey& key, 
                                          XQSettingsManager::Type type)
{
    Q_UNUSED(key);
    Q_UNUSED(type);
    if (mFailed)
        return QVariant();
    else
        return QVariant(mCurrentVal);
}

bool XQSettingsManager::writeItemValue(const XQSettingsKey& key, const QVariant& value)
{
    Q_UNUSED(key);
    if (mFailed) {
        return false;        
    }
    else {
        mCurrentVal = value.toInt();
        return true;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//  XQSettingsKey
//////////////////////////////////////////////////////////////////////////////////////////

XQSettingsKey::XQSettingsKey(XQSettingsKey::Target target, long int uid, unsigned long int key)
{
    m_target = target;
    m_uid = uid;
    m_key = key;
}

XQSettingsKey::~XQSettingsKey()
{
    
}

XQSettingsKey::Target XQSettingsKey::target() const
{
    return m_target;
}

long int XQSettingsKey::uid() const
{
    return m_uid;
}
    
unsigned long int XQSettingsKey::key() const
{
    return m_key;
}
