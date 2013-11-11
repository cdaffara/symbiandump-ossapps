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
#ifndef UNITTEST_CXESTILLCAPTURECONTROLSYMBIAN_H
#define UNITTEST_CXESTILLCAPTURECONTROLSYMBIAN_H

#include <QObject>

#include "cxenamespace.h"

class CxeSensorEventHandler;
class CxeStillCaptureControlSymbian;
class CxeFakeCameraDeviceControl;
class CxeStillCaptureControlSymbian;
class CxeFakeViewfinderControl;
class CxeSnapshotControl;
class CxeFakeFilenameGenerator;
class CxeFilenameGenerator;
class CxeFakeSensorEventHandler;
class CxeFakeAutoFocusControl;
class QSignalSpy;
class CxeFakeCameraDevice;
class CxeFakeAutoFocusControl;
class CxeFakeSettings;
class CxeFakeQualityPresets;
class CxeFakeFileSaveThread;
class CxeDiskMonitor;

class UnitTestCxeStillCaptureControlSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeStillCaptureControlSymbian();
    ~UnitTestCxeStillCaptureControlSymbian();

private slots:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();
    void testSupportedImageQualities();
    void testCapture();
    void testState();
    void testImageCount();
    void testReset();
    void testCancelAll();
    void testSetMode();
    void testMode();
    void testImageDataQueue();
    void testInit();
    void testDeinit();

private:
    CxeFakeAutoFocusControl *mFakeAutoFocusControl;
    CxeFakeCameraDeviceControl *mFakeCameraDeviceControl;
    CxeFakeCameraDevice *mFakeCameraDevice;
    CxeFakeViewfinderControl *mViewfinderControl;
    CxeSnapshotControl *mSnapshotControl;
    CxeFakeFilenameGenerator *mFilenameGenerator;
    CxeFakeSensorEventHandler *mSensorEventHandler;
    CxeStillCaptureControlSymbian *mStillCaptureControl;
    CxeFakeAutoFocusControl *mAutoFocuscontrol;
    CxeFakeSettings *mFakeSettings;
    QSignalSpy *mSpyState;
    CxeFakeQualityPresets *mFakeQualityPresets;
    CxeFakeFileSaveThread *mFakeFileSaveThread;
    CxeDiskMonitor *mDiskMonitor;
};

#endif // UNITTEST_CXESTILLCAPTURECONTROLSYMBIAN_H
