/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QObject>
#include <QTest>
#include <QSignalSpy>

#include "unittest_cxezoomcontrolsymbian.h"
#include "cxezoomcontrolsymbian.h"
#include "cxefakecameradevice.h"
#include "cxefakesettings.h"
#include "cxefakefeaturemanager.h"
#include "cxefakecameradevicecontrol.h"
#include "cxeerror.h"
#include "cxezoomcontrolsymbian.h"
#include "cxetestutils.h"


UnitTestCxeZoomControlSymbian::UnitTestCxeZoomControlSymbian()
: mZoomControl(NULL), mFakeCameraDevice(NULL), mFakeCameraDeviceControl(NULL), mFakeSettings(NULL), mFakeFeatureManager(NULL)
{
    qRegisterMetaType<CxeError::Id>("CxeError::Id");
    qRegisterMetaType<CxeZoomControl::State>("CxeZoomControl::State");
}

UnitTestCxeZoomControlSymbian::~UnitTestCxeZoomControlSymbian()
{

}


void UnitTestCxeZoomControlSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    mFakeCameraDevice = new CxeFakeCameraDevice;
    mFakeCameraDevice->newCamera(Cxe::PrimaryCameraIndex,
                                 mFakeCameraDeviceControl);
    mFakeCameraDeviceControl = new CxeFakeCameraDeviceControl;
    mFakeCameraDeviceControl->setState(CxeCameraDeviceControl::Ready);
    
    mFakeFeatureManager = new CxeFakeFeatureManager();
    
    mFakeSettings = new CxeFakeSettings();

    mZoomControl = new CxeZoomControlSymbian(*mFakeCameraDevice,
                                             *mFakeCameraDeviceControl,
                                             *mFakeSettings,
                                             *mFakeFeatureManager);
    
    // init zoom control
    connect(this, SIGNAL(prepareZoom(int)), mZoomControl, SLOT(prepareZoomForStill(int)));

    // make sure that after creation we are in correct state
    QCOMPARE(mZoomControl->state(), CxeZoomControl::Idle);

    // after construction the initial values are 0
    // other cases for min and max are tested as part of other testcases
    QCOMPARE(mZoomControl->min(),0);
    QCOMPARE(mZoomControl->max(),0);
    
    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeZoomControlSymbian::cleanup()
{
    delete mZoomControl;
    mZoomControl = NULL;

    delete mFakeCameraDeviceControl;
    mFakeCameraDeviceControl = NULL;

    delete mFakeCameraDevice;
    mFakeCameraDevice = NULL;

    delete mFakeSettings;
    mFakeSettings = NULL;

    delete mFakeFeatureManager;
    mFakeFeatureManager = NULL;
}

void UnitTestCxeZoomControlSymbian::testZoomTo()
{
    CX_DEBUG_ENTER_FUNCTION();
    
    QSignalSpy spyStateChange(mZoomControl, SIGNAL(stateChanged(CxeZoomControl::State,
                                                                CxeError::Id)));
    QSignalSpy spyZoomLevelChange(mZoomControl,
                                  SIGNAL(zoomLevelChanged(int)));
    QList<QVariant> spyArguments;

    // before prepare, this method should not do anything
    mZoomControl->zoomTo(mZoomControl->max());
    QVERIFY(!CxeTestUtils::waitForSignal(spyZoomLevelChange, 1000));
    QCOMPARE(mZoomControl->max(), 0);
    QCOMPARE(mZoomControl->min(), 0);
    QCOMPARE(mZoomControl->state(), CxeZoomControl::Idle);

    // call fakeclass to emit a signal to get the zoomcontrol initialized
    // and get zoom values
    mFakeCameraDeviceControl->setState(CxeCameraDeviceControl::Ready);
    emit prepareZoom(0);
    int min = mZoomControl->min();
    int max = mZoomControl->max();
    int middle = min + (max-min)/2;

    // clear signal spies (just to be sure)
    spyStateChange.clear();
    spyZoomLevelChange.clear();

    // zoom to middle
    mZoomControl->zoomTo(middle);
    // verify statechanges
    QVERIFY(CxeTestUtils::waitForSignal(spyStateChange, 1000, 2, false));
    spyArguments = spyStateChange.takeFirst(); // first signal
    QCOMPARE(qvariant_cast<CxeZoomControl::State>(spyArguments.at(0)),
             CxeZoomControl::ZoomingIn);
    spyArguments = spyStateChange.takeFirst(); // second signal
    QCOMPARE(qvariant_cast<CxeZoomControl::State>(spyArguments.at(0)),
             CxeZoomControl::Ready);
    // verify current zoomlevel change
    QVERIFY(CxeTestUtils::waitForSignal(spyZoomLevelChange, 1000));
    spyArguments = spyZoomLevelChange.takeFirst();
    QCOMPARE(spyArguments.at(0).toInt(), middle); // mCurrentZoomLevel

    // clear signal spies
    spyStateChange.clear();
    spyZoomLevelChange.clear();

    // zoom to min
    mZoomControl->zoomTo(min);
    // verify statechanges
    QVERIFY(CxeTestUtils::waitForSignal(spyStateChange, 1000, 2, false));
    spyArguments.clear();
    spyArguments = spyStateChange.takeFirst(); // first signal
    QCOMPARE(qvariant_cast<CxeZoomControl::State>(spyArguments.at(0)),
             CxeZoomControl::ZoomingOut);
    spyArguments.clear();
    spyArguments = spyStateChange.takeFirst(); // second signal
    QCOMPARE(qvariant_cast<CxeZoomControl::State>(spyArguments.at(0)),
             CxeZoomControl::Ready);
    // verify current zoomlevel change
    QVERIFY(CxeTestUtils::waitForSignal(spyZoomLevelChange, 1000));
    spyArguments.clear();
    spyArguments = spyZoomLevelChange.takeFirst();
    QCOMPARE(spyArguments.at(0).toInt(), min); // mCurrentZoomLevel

    // clear signal spies
    spyStateChange.clear();
    spyZoomLevelChange.clear();

    // zoom to max
    mZoomControl->zoomTo(max);
    // verify statechanges
    QVERIFY(CxeTestUtils::waitForSignal(spyStateChange, 1000, 2, false));
    spyArguments = spyStateChange.takeFirst(); // first signal
    QCOMPARE(qvariant_cast<CxeZoomControl::State>(spyArguments.at(0)),
             CxeZoomControl::ZoomingIn);
    spyArguments = spyStateChange.takeFirst(); // second signal
    QCOMPARE(qvariant_cast<CxeZoomControl::State>(spyArguments.at(0)),
             CxeZoomControl::Ready);
    // verify current zoomlevel change
    QVERIFY(CxeTestUtils::waitForSignal(spyZoomLevelChange, 1000));
    spyArguments = spyZoomLevelChange.takeFirst();
    QCOMPARE(spyArguments.at(0).toInt(), max); // mCurrentZoomLevel

    // clear signal spies
    spyStateChange.clear();
    spyZoomLevelChange.clear();

    // zoom to max+1 (nothing should happen, check that no signal is emitted)
    mZoomControl->zoomTo(max+1);
    QVERIFY(!CxeTestUtils::waitForSignal(spyZoomLevelChange, 1000));
    QVERIFY(!CxeTestUtils::waitForSignal(spyStateChange, 1000));


    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeZoomControlSymbian::testPrepareZoomControl()
{
    QSignalSpy spyZoomLevelChange(mZoomControl, SIGNAL(zoomLevelChanged(int)));

    // setting the state of mFakeCameraDeviceControl emits stateChanged signal
    // that CxeZoomControlSymbian connects to prepareZoomControl slot

    // signal parameters that are not supposed to change zoom values
    mFakeCameraDeviceControl->setState(CxeCameraDeviceControl::Idle);
    emit prepareZoom(0);
    // zoomcontrol is in initial state and zoom values should not change
    QVERIFY(!CxeTestUtils::waitForSignal(spyZoomLevelChange, 1000));
    QCOMPARE(mZoomControl->max(),0);
    QCOMPARE(mZoomControl->min(),0);
    QCOMPARE(mZoomControl->state(), CxeZoomControl::Idle);

    spyZoomLevelChange.clear();

    // check that the zoom min and max values are updated
    // when slot is called with the Ready parameter
    mFakeCameraDeviceControl->setState(CxeCameraDeviceControl::Ready);
    emit prepareZoom(0);
    QVERIFY(CxeTestUtils::waitForSignal(spyZoomLevelChange, 1000));
    QList<QVariant> spyArguments = spyZoomLevelChange.takeFirst();
    QCOMPARE(spyArguments.at(0).toInt(), 0); // mCurrentZoomLevel should not change when prepare is called
    QVERIFY(mZoomControl->max() > 0); // Must be positive, or zero if not supported.
    QCOMPARE(mZoomControl->min(), 0); // Must be negative, or zero if macro zoom not supported
    QCOMPARE(mZoomControl->state(), CxeZoomControl::Ready);
}

void UnitTestCxeZoomControlSymbian::testPrepareForCameraDelete()
{
    QSignalSpy spyZoomLevelChange(mZoomControl, SIGNAL(zoomLevelChanged(int)));

    // zoomcontrol needs to be prepared in order to delete camera
    // this emits a signal that calls prepareZoomControl
    mFakeCameraDeviceControl->setState(CxeCameraDeviceControl::Ready);
    emit prepareZoom(0);

    // call fakeclass method that emits a signal,
    // which is connected to prepareForCameraDelete slot in contructor
    // of CxeZoomControlSymbian
    mFakeCameraDevice->deleteCamera();

    // check that after prepareForCameraDelete default values are set
    QCOMPARE(mZoomControl->max(),0);
    QCOMPARE(mZoomControl->min(),0);
    QCOMPARE(mZoomControl->state(),CxeZoomControl::Idle);
    // check signal is emitted, and current zoom level is reset
    QVERIFY(CxeTestUtils::waitForSignal(spyZoomLevelChange, 1000));
    QList<QVariant> spyArguments = spyZoomLevelChange.takeFirst();
    QCOMPARE(spyArguments.at(0).toInt(), 0); // mCurrentZoomLevel
}

QTEST_MAIN(UnitTestCxeZoomControlSymbian);
