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
#ifndef UNITTEST_CXEZOOMCONTROLSYMBIAN_H_
#define UNITTEST_CXEZOOMCONTROLSYMBIAN_H_

#include <QObject>


class CxeZoomControlSymbian;
class CxeFakeCameraDeviceControl;
class CxeFakeCameraDevice;
class CxeFakeSettings;
class CxeFakeFeatureManager;

class UnitTestCxeZoomControlSymbian : public QObject
{
    Q_OBJECT

public:

    UnitTestCxeZoomControlSymbian();
    virtual ~UnitTestCxeZoomControlSymbian();

private slots:

    void init();
    void cleanup();

    void testZoomTo();
    void testPrepareZoomControl();
    void testPrepareForCameraDelete();

signals:
    void prepareZoom(int index);

private:

    CxeZoomControlSymbian *mZoomControl;
    CxeFakeCameraDevice *mFakeCameraDevice;
    CxeFakeCameraDeviceControl *mFakeCameraDeviceControl;
    CxeFakeSettings *mFakeSettings;
    CxeFakeFeatureManager *mFakeFeatureManager;
};

#endif /* UNITTEST_CXEZOOMCONTROLSYMBIAN_H_ */
