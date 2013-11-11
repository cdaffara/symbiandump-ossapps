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
#ifndef UNITTEST_CXEENGINESYMBIAN_H
#define UNITTEST_CXEENGINESYMBIAN_H

#include <QObject>

#include "cxenamespace.h"

class CxeEngineSymbianUnit;
class CxeFakeCameraDeviceControl;
class CxeFakeStillCaptureControl;
class CxeFakeVideoCaptureControl;
class CxeFakeViewfinderControl;

class UnitTestCxeEngineSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeEngineSymbian();
    ~UnitTestCxeEngineSymbian();

private slots:
    void init();
    void cleanup();

    void testCameraDeviceControl();
    void testViewfinderControl();
    void testStillCaptureControl();
    void testVideoCaptureControl();
    void testAutoFocusControl();
    void testZoomControl();
    void testSettings();
    void testSensorEventHandler();
    void testFeatureManager();
    void testMode();
    void testInitMode1();
    void testInitMode2();
    void testInitMode3();
    void testInitMode4();

private:
    // Helper methods for test cases

private:
    // We actually use the a derived class (CxeEngineSymbianUnit) for testing,
    // because we need to be able to replace the engine controls with fake
    // implementations.
    CxeEngineSymbianUnit *mEngine; //!< This will be recreated for each test

    CxeFakeCameraDeviceControl *mCameraDeviceControl; //!< Owned by mEngine
    CxeFakeStillCaptureControl *mStillCaptureControl; //!< Owned by mEngine
    CxeFakeVideoCaptureControl *mVideoCaptureControl; //!< Owned by mEngine
    CxeFakeViewfinderControl *mViewfinderControl; //!< Owned by mEngine
};

#endif // UNITTEST_CXEENGINESYMBIAN_H
