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

#ifndef XQSETTINGSMANAGER_H
#define XQSETTINGSMANAGER_H

#include "xqsettingskey.h"

// CLASS DECLARATION
class XQSettingsManager
{
public:

    enum Type
    {
        TypeVariant = 0,
        TypeInt,
        TypeDouble,
        TypeString,
        TypeByteArray
    };

    XQSettingsManager();
    ~XQSettingsManager();

    QVariant readItemValue(const XQSettingsKey& key, XQSettingsManager::Type type = XQSettingsManager::TypeVariant);
    bool writeItemValue(const XQSettingsKey& key, const QVariant& value);
    
public:
    static bool mFailed;
    static int mCurrentVal;
};

#endif // XQSETTINGSMANAGER_H

// End of file
