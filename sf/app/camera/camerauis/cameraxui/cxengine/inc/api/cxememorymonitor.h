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

#ifndef CXEMEMORYMONITOR_H
#define CXEMEMORYMONITOR_H

#include <QObject>

class CxeFeatureManager;
class CxeMemoryMonitorPrivate;

/*!
* Memory monitor class.
* Memory monitor requests a pre-defined value of startup memory
* at the time it is created, and throws error if it cannot get
* enough memory freed. After creating, it can be set to monitor
* free memory and try to keep it above pre-defined level.
* Pre-defined levels are read from Feature Manager.
*/
class CxeMemoryMonitor : public QObject
{
    Q_OBJECT

public:
    explicit CxeMemoryMonitor(CxeFeatureManager &features);
    ~CxeMemoryMonitor();

public:
    int free() const;

public slots:
    void startMonitoring();
    void stopMonitoring();

private:
    bool requestFreeMemory(int requiredFreeMemory);

private:
    CxeMemoryMonitorPrivate *p;
};

#endif // CXEMEMORYMONITOR_H

