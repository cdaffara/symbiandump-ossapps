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

#ifndef CXEMEMORYMONITORPRIVATE_H
#define CXEMEMORYMONITORPRIVATE_H

#include <QObject>
#include <QTimer>
#include <QList>
#ifdef Q_OS_SYMBIAN
#include <oommonitorsession.h>
#endif // Q_OS_SYMBIAN

class CxeFeatureManager;

class CxeMemoryMonitorPrivate : public QObject
{
    Q_OBJECT

private:
    explicit CxeMemoryMonitorPrivate(CxeFeatureManager &features);
    ~CxeMemoryMonitorPrivate();

private:
    int free() const;
    bool requestFreeMemory(int requiredFreeMemory);

    void startMonitoring();
    void stopMonitoring();

private slots:
    void checkMemory();

private:
    CxeFeatureManager &mFeatures;
    QTimer mTimer;
    QList<int> mLevels;
#ifdef Q_OS_SYMBIAN
    ROomMonitorSession mOomMonitor;
#endif // Q_OS_SYMBIAN

    friend class CxeMemoryMonitor;
};

#endif // CXEMEMORYMONITORPRIVATE_H

