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
#ifndef UNITTEST_CXEVIEWFINDERCONTROLSYMBIAN_H
#define UNITTEST_CXEVIEWFINDERCONTROLSYMBIAN_H

#include <QObject>

#include "cxenamespace.h"

class CxeViewfinderControlSymbian;
class CxeFakeCameraDeviceControl;
class CxeFakeContainer;
class CxeFakeCameraDevice;

class UnitTestCxeViewfinderControlSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeViewfinderControlSymbian();
    ~UnitTestCxeViewfinderControlSymbian();

private slots:
    void init();
    void cleanup();

    void testSetWindow();
    void testStop();
    void testState();
    void testStart();
    void testDisplayResolution();

private:
    // Helper methods for test cases

private:

    CxeViewfinderControlSymbian *mViewfinderControl;
    CxeFakeCameraDeviceControl *mCameraDeviceControl;
    CxeFakeContainer *mContainer;
    CxeFakeCameraDevice *mCameraDevice;
};

#endif // UNITTEST_CXEVIEWFINDERCONTROLSYMBIAN_H
