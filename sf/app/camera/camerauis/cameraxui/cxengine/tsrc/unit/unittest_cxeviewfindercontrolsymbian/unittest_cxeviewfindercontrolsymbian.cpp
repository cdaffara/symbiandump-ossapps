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

#include <QDate>
#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include <QThread>

#include <coecntrl.h>

#include "cxeerror.h"
#include "cxutils.h"
#include "cxetestutils.h"
#include "cxefakecontainer.h"
#include "cxefakecameradevicecontrol.h"
#include "cxefakecameradevice.h"
#include "cxeviewfindercontrolsymbian.h"
#include "unittest_cxeviewfindercontrolsymbian.h"



/**
* UnitTestCxeViewfinderControlSymbian::UnitTestCxeViewfinderControlSymbian()
*/
UnitTestCxeViewfinderControlSymbian::UnitTestCxeViewfinderControlSymbian()
    : mViewfinderControl(0), mCameraDeviceControl(0), mContainer(0), mCameraDevice(0)
{
}

/**
* UnitTestCxeViewfinderControlSymbian::~UnitTestCxeViewfinderControlSymbian()
*/
UnitTestCxeViewfinderControlSymbian::~UnitTestCxeViewfinderControlSymbian()
{
    cleanup();
}

/**
* UnitTestCxeViewfinderControlSymbian::init()
*/
void UnitTestCxeViewfinderControlSymbian::init()
{
    mContainer = CxeFakeContainer::create();

    mCameraDeviceControl = new CxeFakeCameraDeviceControl();
    mCameraDevice = new CxeFakeCameraDevice();
    mCameraDevice->newCamera(Cxe::PrimaryCameraIndex, mCameraDeviceControl);

    mViewfinderControl =
        new CxeViewfinderControlSymbian(
            *mCameraDevice,
            *mCameraDeviceControl);
}

/**
* UnitTestCxeViewfinderControlSymbian::cleanup()
*/
void UnitTestCxeViewfinderControlSymbian::cleanup()
{
    delete mViewfinderControl;
    mViewfinderControl = 0;

    delete mCameraDeviceControl;
    mCameraDeviceControl = 0;

    delete mContainer;
    mContainer = 0;

    delete mCameraDevice;
    mCameraDevice = 0;
}

/**
* UnitTestCxeViewfinderControlSymbian::testSetWindow()
*/
void UnitTestCxeViewfinderControlSymbian::testSetWindow()
{
    mViewfinderControl->setWindow(mContainer);
}

/**
* UnitTestCxeViewfinderControlSymbian::testStart()
*/
void UnitTestCxeViewfinderControlSymbian::testStart()
{
    mViewfinderControl->setWindow(mContainer);

    QVERIFY(mViewfinderControl->start() == CxeError::None);
    QVERIFY(mViewfinderControl->state() == CxeViewfinderControl::Running);
}

/**
* UnitTestCxeViewfinderControlSymbian::testStop()
*/
void UnitTestCxeViewfinderControlSymbian::testStop()
{
    mViewfinderControl->setWindow(mContainer);
    QVERIFY(mViewfinderControl->start() == CxeError::None);

    mViewfinderControl->stop();
    QVERIFY(mViewfinderControl->state() == CxeViewfinderControl::Ready);
}

/**
* UnitTestCxeViewfinderControlSymbian::testState()
*/
void UnitTestCxeViewfinderControlSymbian::testState()
{
    QVERIFY(mViewfinderControl->state() == CxeViewfinderControl::Uninitialized);
}


/**
* UnitTestCxeViewfinderControlSymbian::testDisplayResolution()
*/
void UnitTestCxeViewfinderControlSymbian::testDisplayResolution()
{
    mViewfinderControl->setWindow(mContainer);
    QVERIFY(mViewfinderControl->start() == CxeError::None);

    QSize displaySize = mViewfinderControl->deviceDisplayResolution();
    QCOMPARE(displaySize.width(), mContainer->Size().iWidth);
    QCOMPARE(displaySize.height(), mContainer->Size().iHeight);
}

// main() function - Need event loop for waiting signals,
// so can't use QTEST_APPLESS_MAIN.
QTEST_MAIN(UnitTestCxeViewfinderControlSymbian);

// end of file
