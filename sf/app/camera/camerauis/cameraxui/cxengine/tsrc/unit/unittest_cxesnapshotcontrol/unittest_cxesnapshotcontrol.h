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
#ifndef UNITTEST_CXESNAPSHOTCONTROL_H
#define UNITTEST_CXESNAPSHOTCONTROL_H

#include <QObject>

class CxeSnapshotControl;
class CxeFakeCameraDevice;
class CxeFakeCameraDeviceControl;

class UnitTestCxeSnapshotControl : public QObject
{
    Q_OBJECT

public:

    UnitTestCxeSnapshotControl();
    virtual ~UnitTestCxeSnapshotControl();

private slots:

    void init();
    void cleanup();

    void testState();
    void testCalculateSnapshotSize();
    void testStart();
    void testStop();
    void testHandleCameraEvent();

private:

    CxeSnapshotControl *mSnapshotControl;
    CxeFakeCameraDevice *mCameraDevice;
    CxeFakeCameraDeviceControl *mCameraDeviceControl;
};

#endif /* UNITTEST_CXESNAPSHOTCONTROL_H */
