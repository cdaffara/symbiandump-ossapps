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
#ifndef UNITTEST_CXEAUTOFOCUSCONTROLSYMBIAN_H
#define UNITTEST_CXEAUTOFOCUSCONTROLSYMBIAN_H

#include <QObject>

#include "cxenamespace.h"

class CxeAutoFocusControlSymbianUnit;
class CxeFakeCameraDeviceControl;
class CxeFakeCameraDevice;
class CxeFakeSettings;

class UnitTestCxeAutoFocusControlSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeAutoFocusControlSymbian();
    ~UnitTestCxeAutoFocusControlSymbian();

private slots:
    void init();
    void cleanup();

    void testStart();
    void testCancel();

    void testIsFixedFocusMode();
    void testSupported();

    // tests for protected slots?
    void testPrepareForCameraDelete();
    void testHandleCameraAllocated();
    void testPrepareForRelease();

    void testHandleCameraEventOptimalFocus();
    void testHandleCameraEventAutofocusChanged();
    void testHandleCameraEventFocusRangeChanged();


    void testHandleSceneChanged();
    void testHandleSettingValueChanged();

private:
    // Helper methods for test cases

private:
    // Need to test with derived class to hide missing ECAM.
    CxeAutoFocusControlSymbianUnit *mAutoFocusControl;
    CxeFakeCameraDeviceControl *mCameraDeviceControl;
    CxeFakeCameraDevice *mCameraDevice;
    CxeFakeSettings *mFakeSettings;

};

#endif // UNITTEST_CXEAUTOFOCUSCONTROLSYMBIAN_H
