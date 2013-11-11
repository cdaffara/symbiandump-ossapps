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

#include "unittest_cxecameradevice.h"

#include <QTest>
#include <QSignalSpy>
#include <ECamOrientationCustomInterface2.h>

#include "cxecameradevice.h"
#include "cxedummycamera.h"
#include "cxetestutils.h"

UnitTestCxeCameraDevice::UnitTestCxeCameraDevice()
: mCameraDevice(NULL)
{
    qRegisterMetaType<CxeError::Id>("CxeError::Id");
}


UnitTestCxeCameraDevice::~UnitTestCxeCameraDevice()
{
    cleanup();
}


void UnitTestCxeCameraDevice::init()
{
    mCameraDevice = new CxeCameraDevice;

    // check that the getters return default values after construction
    QVERIFY(mCameraDevice->camera() == NULL);
    QVERIFY(mCameraDevice->advancedSettings() == NULL);
    QVERIFY(mCameraDevice->imageProcessor() == NULL);
    QVERIFY(mCameraDevice->cameraOrientation() == NULL);
    QVERIFY(mCameraDevice->cameraSnapshot() == NULL);
}

void UnitTestCxeCameraDevice::cleanup()
{
    delete mCameraDevice;
    mCameraDevice = NULL;
}

void UnitTestCxeCameraDevice::testNewCamera()
{
    CX_DEBUG_ENTER_FUNCTION();

    CCamera *camera = NULL;
    CCamera::CCameraAdvancedSettings *advancedSettings = NULL;
    CCamera::CCameraImageProcessing * imageProcessor = NULL;
    MCameraOrientation *cameraOrientation = NULL;
    CxeError::Id error = CxeError::None;

    // test newCamera when camera is not set
    error = mCameraDevice->newCamera(Cxe::PrimaryCameraIndex, this);

    camera = mCameraDevice->camera();
    advancedSettings = mCameraDevice->advancedSettings();
    imageProcessor = mCameraDevice->imageProcessor();
    cameraOrientation = mCameraDevice->cameraOrientation();

    QCOMPARE(error,CxeError::None);
    QVERIFY(camera);
    QVERIFY(advancedSettings);
    QVERIFY(imageProcessor);
    QVERIFY(mCameraDevice->cameraSnapshot());
    QVERIFY(cameraOrientation == camera->CustomInterface(KCameraOrientationUid));

    // test newCamera when camera is set
    error = mCameraDevice->newCamera(Cxe::SecondaryCameraIndex, this);
    QVERIFY(mCameraDevice->camera());
    QVERIFY(mCameraDevice->camera() != camera);
    QCOMPARE(error,CxeError::None);

    camera = mCameraDevice->camera();
    // test newCamera with NULL as observer
    error = mCameraDevice->newCamera(Cxe::PrimaryCameraIndex, NULL);
    QVERIFY(mCameraDevice->camera());
    QVERIFY(mCameraDevice->camera() != camera);
    QCOMPARE(error,CxeError::None);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeCameraDevice::testReleaseCamera()
{
    CX_DEBUG_ENTER_FUNCTION();
    QSignalSpy spy(mCameraDevice, SIGNAL(prepareForRelease()));

    // releaseCamera when camera is not set
    // -> signal is not emitted
    // -> camera is still NULL
    mCameraDevice->releaseCamera();
    QVERIFY(!CxeTestUtils::waitForSignal(spy, 1000));
    QVERIFY(mCameraDevice->camera() == NULL);

    mCameraDevice->newCamera(Cxe::PrimaryCameraIndex, this);

    // releaseCamera when camera set
    // -> signal is emitted
    // -> camera stays the same (not deleted)
    CCamera *camera = mCameraDevice->camera();
    mCameraDevice->releaseCamera();
    QVERIFY(CxeTestUtils::waitForSignal(spy, 1000));
    QVERIFY(mCameraDevice->camera() == camera);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeCameraDevice::testDeleteCamera()
{
    CX_DEBUG_ENTER_FUNCTION();
    QSignalSpy spy(mCameraDevice, SIGNAL(prepareForCameraDelete()));

    // deleteCamera when camera is not set
    mCameraDevice->deleteCamera();
    QVERIFY(CxeTestUtils::waitForSignal(spy, 1000));
    QVERIFY(mCameraDevice->advancedSettings() == NULL);
    QVERIFY(mCameraDevice->imageProcessor() == NULL);
    QVERIFY(mCameraDevice->cameraOrientation() == NULL);
    QVERIFY(mCameraDevice->camera() == NULL);
	QVERIFY(mCameraDevice->cameraSnapshot() == NULL);

    mCameraDevice->newCamera(Cxe::PrimaryCameraIndex, this);

    // deleteCamera when camera is set
    mCameraDevice->deleteCamera();
    QVERIFY(CxeTestUtils::waitForSignal(spy, 1000));
    QVERIFY(mCameraDevice->advancedSettings() == NULL);
    QVERIFY(mCameraDevice->imageProcessor() == NULL);
    QVERIFY(mCameraDevice->cameraOrientation() == NULL);
    QVERIFY(mCameraDevice->camera() == NULL);
    QVERIFY(mCameraDevice->cameraSnapshot() == NULL);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeCameraDevice::testSetCamera()
{
    CX_DEBUG_ENTER_FUNCTION();
    QSignalSpy spy(mCameraDevice, SIGNAL(cameraAllocated(CxeError::Id)));

    // create new dummycamera for testing the input
    CxeDummyCamera *camera = 0;
    TRAPD(error, camera = CxeDummyCamera::NewL(*this, Cxe::PrimaryCameraIndex, 100, 2));
    QCOMPARE(error, KErrNone);
    QVERIFY(camera);

    // test when camera is not set
    mCameraDevice->setCamera(camera);
    QVERIFY(mCameraDevice->camera() == camera);
    QVERIFY(mCameraDevice->advancedSettings());
    QVERIFY(mCameraDevice->imageProcessor());
    QVERIFY(mCameraDevice->cameraOrientation() == camera->CustomInterface(KCameraOrientationUid));
    QVERIFY(mCameraDevice->cameraSnapshot());
    QVERIFY(CxeTestUtils::waitForSignal(spy, 1000));

    // setting same camera twice
    mCameraDevice->setCamera(camera);
    QVERIFY(mCameraDevice->camera() == camera);
    QVERIFY(mCameraDevice->advancedSettings());
    QVERIFY(mCameraDevice->imageProcessor());
    QVERIFY(mCameraDevice->cameraOrientation() == camera->CustomInterface(KCameraOrientationUid));
    QVERIFY(mCameraDevice->cameraSnapshot());
    QVERIFY(CxeTestUtils::waitForSignal(spy, 1000));

    // set a new camera
    CxeDummyCamera *camera2 = 0;
    TRAPD(error2, camera2 = CxeDummyCamera::NewL(*this, Cxe::PrimaryCameraIndex, 100, 2));
    QCOMPARE(error2,KErrNone);
    QVERIFY(camera2 != 0);

    mCameraDevice->setCamera(camera2);
    QVERIFY(mCameraDevice->camera() == camera2);
    QVERIFY(mCameraDevice->advancedSettings());
    QVERIFY(mCameraDevice->imageProcessor());
    QVERIFY(mCameraDevice->cameraSnapshot());
    QVERIFY(mCameraDevice->cameraOrientation() == camera2->CustomInterface(KCameraOrientationUid));
    QVERIFY(CxeTestUtils::waitForSignal(spy, 1000));

    CX_DEBUG_EXIT_FUNCTION();
}



QTEST_MAIN(UnitTestCxeCameraDevice);
