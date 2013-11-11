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
#ifndef UNITTEST_CXECAMERADEVICECONTROLSYMBIAN_H
#define UNITTEST_CXECAMERADEVICECONTROLSYMBIAN_H

#include <QObject>

#include "cxenamespace.h"

class CxeCameraDeviceControlUnit;
class CxeFakeCameraDevice;

class UnitTestCxeCameraDeviceControlSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeCameraDeviceControlSymbian();
    ~UnitTestCxeCameraDeviceControlSymbian();

private slots:
    void init();
    void cleanup();

    void testMode();
    void testSetMode();
    void testRelease();
    void testCameraIndex();
    void testSwitchCamera();
    void testReserve();

private:
    // Helper methods for test cases

private:
    // We actually use the a derived class (CxeCameraDeviceControlUnit) for testing,
    // because we need to be able to replace the CxeCameraDevice with a fake
    // implementation.
    CxeCameraDeviceControlUnit *mDeviceControl; // This will be recreated for each test
    CxeFakeCameraDevice *mFakeCameraDevice;     // This will be recreated for each test
};

#endif // UNITTEST_CXECAMERADEVICECONTROLSYMBIAN_H
