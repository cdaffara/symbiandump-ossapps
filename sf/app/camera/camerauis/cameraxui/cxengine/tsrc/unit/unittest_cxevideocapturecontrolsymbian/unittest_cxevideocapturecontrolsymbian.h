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
#ifndef UNITTEST_CXESTILLIMAGESYMBIAN_H
#define UNITTEST_CXESTILLIMAGESYMBIAN_H

#include <QObject>
#include "cxevideocapturecontrolsymbian.h"
#include "cxevideocapturecontrolsymbianunit.h"

class CxeFakeCameraDevice;
class CxeFakeViewfinderControl;
class CxeSnapshotControl;
class CxeFakeCameraDeviceControl;
class CxeFakeSettings;
class CxeFakeFilenameGenerator;
class QSignalSpy;
class CxeFakeQualityPresets;
class CxeDiskMonitor;

class UnitTestCxeVideoCaptureControlSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeVideoCaptureControlSymbian();
    ~UnitTestCxeVideoCaptureControlSymbian();

private slots:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void testState();
    void testRecord();
    void testPause();
    void testStop();
    void testRemainingTime();
    void testElapsedTime();
    void testFilename();
    void testReset();
    void testSnapshot();
    void testHandleSnapshotReady();
    void testInit();
    void testDeinit();
    void testHandleSoundPlayed();
    void testMvruoOpenComplete();
    void testMvruoPrepareComplete();
    void testMvruoRecordComplete();
    void testMvruoEvent();

private:
    // Helper methods for test cases
    void doPrepareStuff();

private:
    CxeVideoCaptureControlSymbianUnit *mCxeVideoCaptureControlSymbian;
    CxeFakeCameraDevice *mCameraDevice;
    CxeFakeViewfinderControl *mViewfinderControl;
    CxeSnapshotControl *mSnapshotControl;
    CxeFakeCameraDeviceControl *mCameraDeviceControl;
    CxeFakeSettings *mSettings;
    CxeFakeFilenameGenerator *mFilenameGeneratorSymbian;
    QSignalSpy *mSpyState;
    CxeFakeQualityPresets *mFakeQualityPresets;
    CxeDiskMonitor *mDiskMonitor;
};

#endif // UNITTEST_CXESTILLIMAGESYMBIAN_H
