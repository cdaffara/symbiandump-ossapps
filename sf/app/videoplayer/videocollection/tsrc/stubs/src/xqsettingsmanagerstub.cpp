/**
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

#include "xqsettingsmanagerstub.h"

int XQSettingsManager::mReadItemSucceedCounter = 0;
int XQSettingsManager::mWriteItemSucceedCounter = 0;
QHash<int, QVariant> XQSettingsManager::mWrittenValueHash = QHash<int, QVariant>();

QVariant XQSettingsManager::readItemValue(XQCentralRepositorySettingsKey &key, int type)
{
    Q_UNUSED(type);
    mReadItemSucceedCounter--;
    if(mReadItemSucceedCounter >= 0)
    {
        return mWrittenValueHash.value(key.mKey);
    }
    return QVariant();
}

bool XQSettingsManager::writeItemValue(const XQCentralRepositorySettingsKey& key, const QVariant& value)
{
    mWriteItemSucceedCounter--;
    if(mWriteItemSucceedCounter >= 0)
    {
        mWrittenValueHash.insert(key.mKey, value);
        return true;
    }    
    return false;
}

XQSettingsManager::XQSettingsManager()
{
    
}

XQSettingsManager::~XQSettingsManager()
{
}
