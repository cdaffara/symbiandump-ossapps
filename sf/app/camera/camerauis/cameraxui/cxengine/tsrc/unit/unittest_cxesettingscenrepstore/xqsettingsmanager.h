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
#ifndef XQSETTINGSMANAGER_H
#define XQSETTINGSMANAGER_H

// INCLUDES
#include <QObject>
#include <QVariant>
#include "xqsettingskey.h"




// CLASS DECLARATION
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
    bool writeItemValue(const XQSettingsKey& key, const QVariant& value);
    XQSettingsManager::Error error() const;
    bool startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type = XQSettingsManager::TypeVariant);


signals:
    void valueChanged(const XQSettingsKey& key, const QVariant& value);

private:
    XQSettingsManager::Error mError;
    QHash<unsigned long int, QVariant> mStore;

};

#endif // XQSETTINGSMANAGER_H

// End of file
