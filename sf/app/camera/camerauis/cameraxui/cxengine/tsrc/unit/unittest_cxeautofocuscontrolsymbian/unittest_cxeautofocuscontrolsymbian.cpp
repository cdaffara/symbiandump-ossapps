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
#include <QDate>
#include <QTest>
#include <QDebug>
#include <QSignalSpy>
#include <QThread>

#include "cxetestutils.h"
#include "cxefakecameradevicecontrol.h"
#include "cxefakecameradevice.h"
#include "cxefakesettings.h"
#include "cxeautofocuscontrolsymbianunit.h"
#include "unittest_cxeautofocuscontrolsymbian.h"
#include "cxenamespace.h"

UnitTestCxeAutoFocusControlSymbian::UnitTestCxeAutoFocusControlSymbian()
    : mAutoFocusControl(NULL),
      mCameraDeviceControl(NULL),
      mCameraDevice(NULL),
      mFakeSettings(NULL)
{
    qRegisterMetaType<CxeError::Id>("CxeError::Id");
    qRegisterMetaType<CxeAutoFocusControl::State>("CxeAutoFocusControl::State");
    qRegisterMetaType<CxeAutoFocusControl::Mode>("CxeAutoFocusControl::Mode");
}

UnitTestCxeAutoFocusControlSymbian::~UnitTestCxeAutoFocusControlSymbian()
{
}

// Run before each individual test case
void UnitTestCxeAutoFocusControlSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    mFakeSettings = new CxeFakeSettings();

    mCameraDeviceControl = new CxeFakeCameraDeviceControl();
    mCameraDevice = new CxeFakeCameraDevice();
    mCameraDevice->newCamera(mCameraDeviceControl->cameraIndex(), mCameraDeviceControl);

    mAutoFocusControl = new CxeAutoFocusControlSymbianUnit(*mCameraDevice, *mFakeSettings);

    // make sure that initialization is correct
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Unknown);
    QCOMPARE(mAutoFocusControl->mode(), CxeAutoFocusControl::Hyperfocal);
    QCOMPARE(mAutoFocusControl->isSoundEnabled(), false);
    QCOMPARE(mAutoFocusControl->supported(), true);

    CX_DEBUG_EXIT_FUNCTION();
}

// Run after each individual test case
void UnitTestCxeAutoFocusControlSymbian::cleanup()
{
    CX_DEBUG_ENTER_FUNCTION();

    delete mAutoFocusControl;
    mAutoFocusControl = NULL;

    delete mCameraDeviceControl;
    mCameraDeviceControl = NULL;

    delete mCameraDevice;
    mCameraDevice = NULL;

    delete mFakeSettings;
    mFakeSettings = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}


void UnitTestCxeAutoFocusControlSymbian::testStart()
{
    // start takes a boolean input and returns CxeError
    // functionality depends on current state and current autofocus mode

    QSignalSpy stateSpy(mAutoFocusControl,
                        SIGNAL(stateChanged(CxeAutoFocusControl::State,
                                            CxeError::Id)));

    CxeError::Id returnValue = CxeError::None;
    // 1) Default input after initialisation (fixed focus) -> nothing should happen
    returnValue = mAutoFocusControl->start();
    // verifying the result:
    QVERIFY(returnValue == CxeError::None);
    QCOMPARE(mAutoFocusControl->isSoundEnabled(), true); // check for input paramete
    // no state changes
    QVERIFY(!CxeTestUtils::waitForSignal(stateSpy, 1000));
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Unknown);

    // 2) Input parameter false is handled correctly
    // prerequisites : not fixed mode & unknown state -> normal functionality
    mAutoFocusControl->setMode(CxeAutoFocusControl::Auto); // non-fixed
    // function call
    returnValue = mAutoFocusControl->start(false);
    // verifying the result:
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::InProgress);
    QCOMPARE(returnValue, CxeError::None);
    QCOMPARE(mAutoFocusControl->isSoundEnabled(), false); // check for input parameter

    // 3) Autofocus is not ready (state is cancelling or in progress)
    // prerequisites :
    mAutoFocusControl->setMode(CxeAutoFocusControl::Macro); // non-fixed mode
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::InProgress);
    // start should return CxeError::InUse
    returnValue = mAutoFocusControl->start();
    QCOMPARE(returnValue, CxeError::InUse);

    // 4) Camera has been released
    // Cannot be tested like this since assertion crashes the function
    // AFTER prepareForCameraDelete CALL.
    /*mAutoFocusControl->prepareForCameraDelete();
    stateSpy.clear();
    returnValue = mAutoFocusControl->start();
    // result: no signal should be emitted
    QVERIFY(!CxeTestUtils::waitForSignal(stateSpy, 1000));
    QCOMPARE(returnValue, CxeError::None); // should there be and error here?
    */
}

void UnitTestCxeAutoFocusControlSymbian::testCancel()
{
    // create signalspy to monitor that state changed signal
    QSignalSpy spy(mAutoFocusControl,
                        SIGNAL(stateChanged(CxeAutoFocusControl::State,
                                            CxeError::Id)));

    // 1) after initialisation cancel does nothing
    mAutoFocusControl->cancel();
    // no state changes
    QCOMPARE(spy.count(), 0);
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Unknown);

    // 2) When in fixed focus mode, cancel does nothing
    mAutoFocusControl->setMode(CxeAutoFocusControl::Auto); // non fixed mode so that start works
    mAutoFocusControl->start();
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::InProgress); //verify start state
    mAutoFocusControl->setMode(CxeAutoFocusControl::Infinity); // fixed mode
    spy.clear();
    mAutoFocusControl->cancel();
    QCOMPARE(spy.count(), 0);
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::InProgress);

    // 3) InProgress state and "normal cancel"
    // set mode back to non fixed mode
    mAutoFocusControl->setMode(CxeAutoFocusControl::Auto);
    mAutoFocusControl->cancel();
    // -> state is changed to Canceling
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Canceling);
    // -> focustype set correctly
    QVERIFY(mAutoFocusControl->focusType()
            == CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff);

    // 4) Canceling state (reached when calling canceling twice in a row,
    // now already in canceling state after previous test)
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Canceling);
    mAutoFocusControl->cancel();
    // -> state or focustype is not changing
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Canceling);
    QVERIFY(mAutoFocusControl->focusType()
            == CCamera::CCameraAdvancedSettings::EAutoFocusTypeOff);

    // 5) Ready state (cancel called after start has been called and focus
    // found)
    // force state to ready in order to test canceling
    mAutoFocusControl->setState(CxeAutoFocusControl::Unknown);
    mAutoFocusControl->start(); // changes to in progress + sets focus type & range
    mAutoFocusControl->setState(CxeAutoFocusControl::Ready);
    mAutoFocusControl->cancel();
    // state is changed to canceling
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Canceling);

}


void UnitTestCxeAutoFocusControlSymbian::testIsFixedFocusMode()
{
    // default value (hyperfocal)
    QCOMPARE(mAutoFocusControl->isFixedFocusMode(mAutoFocusControl->mode()), true);

    // set non fixed focus mode
    mAutoFocusControl->setMode(CxeAutoFocusControl::Auto);
    QCOMPARE(mAutoFocusControl->isFixedFocusMode(mAutoFocusControl->mode()), false);

    // set fixed focus mode
    mAutoFocusControl->setMode(CxeAutoFocusControl::Infinity);
    QCOMPARE(mAutoFocusControl->isFixedFocusMode(mAutoFocusControl->mode()), true);

}

void UnitTestCxeAutoFocusControlSymbian::testSupported()
{
    bool supported = mAutoFocusControl->supported();
    QVERIFY(supported);
}


void UnitTestCxeAutoFocusControlSymbian::testPrepareForCameraDelete()
{
    // prepareCameraDelete calls prepareCameraRelease plus sets advanced
    // settings false (supported() will return false)
    // this test is for both methods

    // 1) initial values
    mAutoFocusControl->prepareForCameraDelete();
    QCOMPARE(mAutoFocusControl->supported(), false);
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Unknown);

    // 2) test prepareForCameraDelete with non-initial values
    mAutoFocusControl->handleCameraAllocated(CxeError::None);
    // force state to be something else than Unknown and check that it will be changed
    mAutoFocusControl->setState(CxeAutoFocusControl::Canceling);
    // force mCancelled to be false and check that it will be changed
    mAutoFocusControl->mCancelled = true;
    mAutoFocusControl->prepareForCameraDelete();
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Unknown);
    QCOMPARE(mAutoFocusControl->mCancelled, false);
    QCOMPARE(mAutoFocusControl->supported(), false);

}

void UnitTestCxeAutoFocusControlSymbian::testHandleCameraAllocated()
{
    // create signalspy to monitor that state changed signal
    QSignalSpy stateSpy(mAutoFocusControl,
                        SIGNAL(stateChanged(CxeAutoFocusControl::State,
                                            CxeError::Id)));

    // 1) initial setup without an error (camera is allocated already)
    mAutoFocusControl->handleCameraAllocated(CxeError::None);
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Unknown);
    QCOMPARE(mAutoFocusControl->supported(), true);

    // 2) release camera and call with an error code
    // -> supported should return false since initializeResources is not called in error case
    // -> state is not changed
    mAutoFocusControl->prepareForCameraDelete();
    stateSpy.clear();
    mAutoFocusControl->handleCameraAllocated(CxeError::General);
    QVERIFY(!CxeTestUtils::waitForSignal(stateSpy, 1000));
    QCOMPARE(mAutoFocusControl->supported(), false);

    // 3) release camera and call without an error
    // (prepareForCameraDelete has been called in previous test)
    // -> resources are initialized i.e. supported returns true
    // first force state to something else so state change can be verified
    mAutoFocusControl->setState(CxeAutoFocusControl::Canceling);
    mAutoFocusControl->handleCameraAllocated(CxeError::None);
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Unknown);
    QCOMPARE(mAutoFocusControl->supported(), true);

}

void UnitTestCxeAutoFocusControlSymbian::testPrepareForRelease()
{
    // see testPrepareForCameraDelete
}

void UnitTestCxeAutoFocusControlSymbian::testHandleCameraEventOptimalFocus()
{
    // handleCameraEvent handles callbacks and gets information about
    // focus events. handleCameraEvent calls private method handleAfEvent
    // for the focusing events that CxeAutoFocusControlSymbian needs

    // testing handleAfEvent is done by calling handleCameraEvent
    // and monitoring state changes after each "event"

    QSignalSpy stateSpy(mAutoFocusControl,
                        SIGNAL(stateChanged(CxeAutoFocusControl::State,
                                            CxeError::Id)));


    // Input 1 ----------------------------------------------------
    // Optimal focus was reached or couldn't be found
    int eventUid = KUidECamEventCameraSettingsOptimalFocusUidValue;
    int symbianError = 0; //KErrNone

    // AutofocusControl class is not InProgress or Canceling state
    // => event ignored, nothing is changed
    mAutoFocusControl->handleCameraEvent(eventUid, symbianError);
    QVERIFY(!CxeTestUtils::waitForSignal(stateSpy, 1000));

    // AutoFocusControl is InProgress state (waiting for the focus)
    mAutoFocusControl->setMode(CxeAutoFocusControl::Auto); // not fixed focus
    mAutoFocusControl->start();
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::InProgress); //verify start state
    mAutoFocusControl->handleCameraEvent(eventUid, symbianError);
    // => state should change to Ready
    QVERIFY(CxeTestUtils::waitForSignal(stateSpy, 1000));
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Ready);

    // InProgress state but focus fails
    mAutoFocusControl->start();
    stateSpy.clear();
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::InProgress); //verify start state
    mAutoFocusControl->handleCameraEvent(eventUid, -18); // KErrNotReady
    // => state should change to Failed
    QVERIFY(CxeTestUtils::waitForSignal(stateSpy, 1000));
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Failed);

    // AutoFocusControl is in Canceling state
    mAutoFocusControl->start();
    mAutoFocusControl->cancel();
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Canceling); //verify start state
    stateSpy.clear();
    mAutoFocusControl->handleCameraEvent(eventUid, symbianError);
    // => state should change to Unknown
    QVERIFY(CxeTestUtils::waitForSignal(stateSpy, 1000));
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Unknown);
}

void UnitTestCxeAutoFocusControlSymbian::testHandleCameraEventAutofocusChanged()
{
    QSignalSpy stateSpy(mAutoFocusControl,
                        SIGNAL(stateChanged(CxeAutoFocusControl::State,
                                            CxeError::Id)));
    // Input 2 -----------------------------------------------------
    // Notifies a change in autofocus type
    int eventUid = KUidECamEventCameraSettingAutoFocusType2UidValue;
    int symbianError = -18; // == KErrNotReady

    // InProgress: event is ignored
    mAutoFocusControl->setMode(CxeAutoFocusControl::Auto); // not fixed focus
    mAutoFocusControl->start();
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::InProgress); //verify start state
    stateSpy.clear();
    mAutoFocusControl->handleCameraEvent(eventUid, symbianError);
    QVERIFY(!CxeTestUtils::waitForSignal(stateSpy, 1000));

    // Canceling:
    // error parameter is ignored, state is not changed
    mAutoFocusControl->cancel();
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Canceling); //verify start state
    stateSpy.clear();
    mAutoFocusControl->handleCameraEvent(eventUid, symbianError);
    QVERIFY(!CxeTestUtils::waitForSignal(stateSpy, 1000));

}

void UnitTestCxeAutoFocusControlSymbian::testHandleCameraEventFocusRangeChanged()
{
    QSignalSpy stateSpy(mAutoFocusControl,
                        SIGNAL(stateChanged(CxeAutoFocusControl::State,
                                            CxeError::Id)));

    // Input 3 -----------------------------------------------------
    // Focus range have changed
    int eventUid = KUidECamEventCameraSettingFocusRangeUidValue;
    int symbianError = -2; // == KErrGeneral

    // In any other state than InProgress this event is ignored
    stateSpy.clear();
    mAutoFocusControl->handleCameraEvent(eventUid, symbianError);
    QVERIFY(!CxeTestUtils::waitForSignal(stateSpy, 1000));

    // InProgress: because of the error parameter state changes to failed
    // (focus range change failed because of error)
    mAutoFocusControl->setMode(CxeAutoFocusControl::Auto); // not fixed focus
    mAutoFocusControl->start();
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::InProgress); //verify start state
    mAutoFocusControl->handleCameraEvent(eventUid, symbianError);
    // => state should change to Failed
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Failed);
}

void UnitTestCxeAutoFocusControlSymbian::testHandleSceneChanged()
{
    // changes the autofocus settings to match the new scene settings
    QVariantMap scene;

    // changes focus if new scene setting defines fixed focus
    // otherwise sets the autofocus control to Unknown state (==initial state)

    // 1) change to a scene with fixed focus mode (Infinity & Hyperfocal)
    scene.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Infinity);
    mAutoFocusControl->handleSceneChanged(scene);
    // states are changed and mode is set correctly ->Unknown->InProgress
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::InProgress); // end state
    QCOMPARE(mAutoFocusControl->mode(), CxeAutoFocusControl::Infinity);

    // 2) change to non-fixed mode (like Macro)
    scene.clear();
    scene.insert(CxeSettingIds::FOCAL_RANGE, CxeAutoFocusControl::Macro);
    mAutoFocusControl->handleSceneChanged(scene);
    // states are changed and mode is set correctly
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Unknown);
    QCOMPARE(mAutoFocusControl->mode(), CxeAutoFocusControl::Macro);

    // 3) try with error input (scene does not contain focal_range)
    scene.clear();
    // change the autofocuscontrol state from Unknown (last test) to
    // something else calling start() for example
    mAutoFocusControl->start();
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::InProgress); //verify start state
    mAutoFocusControl->handleSceneChanged(scene);
    // state is changed and mode is not changed
    QCOMPARE(mAutoFocusControl->state(), CxeAutoFocusControl::Unknown);
    QCOMPARE(mAutoFocusControl->mode(), CxeAutoFocusControl::Macro);

}

void UnitTestCxeAutoFocusControlSymbian::testHandleSettingValueChanged()
{
    // Autofocus mode needs to be updated when face trackin is actived in
    // scene mod which doesn't support facetracking

    QVariant on = QVariant(1);
    QVariant off = QVariant(0);

    // 1) check that mode does not change when some other setting value
    // than face tracking is given
    mAutoFocusControl->handleSettingValueChanged(CxeSettingIds::LIGHT_SENSITIVITY, on);
    QCOMPARE(mAutoFocusControl->mode(), CxeAutoFocusControl::Hyperfocal);

    // 2) check turning facetracking on, when mode is fixed
    mAutoFocusControl->setMode(CxeAutoFocusControl::Infinity);
    mAutoFocusControl->handleSettingValueChanged(CxeSettingIds::FACE_TRACKING, on);
    QCOMPARE(mAutoFocusControl->mode(), CxeAutoFocusControl::Auto);
    QCOMPARE(mAutoFocusControl->mFaceTrackingOverride, true);

    // 3) check turning facetracking off will return the previous mode
    mAutoFocusControl->handleSettingValueChanged(CxeSettingIds::FACE_TRACKING, off);
    QCOMPARE(mAutoFocusControl->mode(), CxeAutoFocusControl::Infinity);
    QCOMPARE(mAutoFocusControl->mFaceTrackingOverride, false);

    // 4) check that turning facetracking off, when it is not on, won't
    // set the previous mode
    mAutoFocusControl->setMode(CxeAutoFocusControl::Macro);
    mAutoFocusControl->handleSettingValueChanged(CxeSettingIds::FACE_TRACKING, off);
    QCOMPARE(mAutoFocusControl->mode(), CxeAutoFocusControl::Macro);
    QCOMPARE(mAutoFocusControl->mFaceTrackingOverride, false);


}



// main() function - Need event loop for waiting signals,
// so can't use QTEST_APPLESS_MAIN.
QTEST_MAIN(UnitTestCxeAutoFocusControlSymbian);
