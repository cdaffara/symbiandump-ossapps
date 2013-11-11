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
#ifndef UNITTEST_CXEMEMORYMONITOR_H
#define UNITTEST_CXEMEMORYMONITOR_H

#include <QObject>

#include "cxenamespace.h"

class CxeMemoryMonitor;
class CxeFakeFeatureManager;

class UnitTestCxeMemoryMonitor : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeMemoryMonitor();
    ~UnitTestCxeMemoryMonitor();

private slots:
    void init();
    void cleanup();

    void testFree();
    void testStartMonitoring();
    void testStopMonitoring();
    void testStartStopMonitoring();

private:
    CxeFakeFeatureManager *mFakeFeatureManager;
    CxeMemoryMonitor *mMemoryMonitor;
};

#endif // UNITTEST_CXEMEMORYMONITOR_H
