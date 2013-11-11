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

#ifndef XQSETTINGSMANAGER_H
#define XQSETTINGSMANAGER_H

#include <QObject>
#include <QVariant>
#include <stub/inc/xqsettingskey.h>

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

    explicit XQSettingsManager(QObject* parent = 0);
    virtual ~XQSettingsManager();
    
    QVariant readItemValue(const XQSettingsKey& key, XQSettingsManager::Type type = XQSettingsManager::TypeVariant);
    bool writeItemValue(const XQSettingsKey& key, const QVariant& value);
    bool startMonitoring(const XQSettingsKey& key, XQSettingsManager::Type type = XQSettingsManager::TypeVariant);
        
    int readItemValueRequestCount(int key);
    int writeItemValueRequestCount(int key);
    int startMonitoringRequestCount(int key);
    QVariant XQSettingsManager::writeRequestValue(int key);
    void clear();
    void emitValueChanged(const XQSettingsKey& key, const QVariant& value);

signals:
    void valueChanged(const XQSettingsKey& key, const QVariant& value);

private:
    QMap <int,int> mReadCount;
    QMap <int,int> mWriteCount;
    QMap <int,QVariant> mWriteValue;
    QMap <int,int> mStartMonitoringCount;

};

#endif  // XQSETTINGSMANAGER_H
