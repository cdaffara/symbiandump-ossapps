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
* Description:  Stub for FM Radio home screen widget unit testing.
*
*/

#ifndef XQSETTINGSMANAGER_H
#define XQSETTINGSMANAGER_H

// System includes
#include <QObject>
#include <QVariant>

// Forward declarations
class XQSettingsKey;

class XQSettingsManager : public QObject
{
    Q_OBJECT

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
    ~XQSettingsManager();

    QVariant readItemValue(const XQSettingsKey& key, XQSettingsManager::Type type = XQSettingsManager::TypeVariant);
    bool startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type = XQSettingsManager::TypeVariant);
    bool stopMonitoring(const XQSettingsKey& key);
    
signals:
    void valueChanged(const XQSettingsKey& key, const QVariant& value);
    void itemDeleted(const XQSettingsKey& key);

};

#endif // XQSETTINGSMANAGER_H
