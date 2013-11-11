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

#ifndef XQSETTINGSMANAGER_STUB_H
#define XQSETTINGSMANAGER_STUB_H

#include <QObject>
#include <QVariant>

//#include "xqsettingskey.h"
class XQSettingsKey;

class XQSettingsManager : public QObject
{
    Q_OBJECT
//    Q_ENUMS(Error)
//    Q_ENUMS(Type)

public:

    enum Type
    {
        TypeVariant = 0,
        TypeInt,
        TypeDouble,
        TypeString,
        TypeByteArray
    };

    enum Error 
    {
        NoError = 0,
        OutOfMemoryError,
        NotFoundError,
        AlreadyExistsError,
        PermissionDeniedError,
        BadTypeError,
        NotSupportedError,
        UnknownError = -1
    };

    XQSettingsManager(QObject* parent = 0);
    virtual ~XQSettingsManager();

    QVariant readItemValue(const XQSettingsKey& key, XQSettingsManager::Type type = XQSettingsManager::TypeVariant);
    bool writeItemValue(const XQSettingsKey& key, const QVariant& value);
    bool startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type = XQSettingsManager::TypeVariant);
    bool stopMonitoring(const XQSettingsKey& key);
    
    XQSettingsManager::Error error() const;

Q_SIGNALS:
    void valueChanged(const XQSettingsKey& key, const QVariant& value);
    void itemDeleted(const XQSettingsKey& key);

private:
};

#endif // XQSETTINGSMANAGER_STUB_H
