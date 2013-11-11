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
#ifndef UNITTEST_CXEDISKMONITOR_H
#define UNITTEST_CXEDISKMONITOR_H

#include <QObject>

#include "cxenamespace.h"

class CxeDiskMonitor;
class CxeFakeSettings;

class UnitTestCxeDiskMonitor : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeDiskMonitor();
    ~UnitTestCxeDiskMonitor();

private slots:
    void init();
    void cleanup();

    void testStart();
    void testStop();
    void testIsMonitoring();
    void testFree();
    void testSetLowWarningLevelZero();
    void testSetLowWarningLevelTenMegabytes();
    void testSetLowWarningLevelFiveGigabyte();
    void testLowLevelWarningSignal();
    void testFreeSpaceChangedSignal();

private:
    CxeFakeSettings *mFakeSettings;
    CxeDiskMonitor *mDiskMonitor;
};

#endif // UNITTEST_CXEDISKMONITOR_H

