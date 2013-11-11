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

#ifndef UNITTEST_CXESETTINGSCONTROLSYMBIAN_H
#define UNITTEST_CXESETTINGSCONTROLSYMBIAN_H

#include <QObject>

class CxeSettingsControlSymbian;
class CxeFakeSettings;
class CxeFakeCameraDevice;
class CxeFakeCameraDeviceControl;



/*!
* Unit test class for CxeSettingsControlSymbian.
*/
class UnitTestCxeSettingsControlSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeSettingsControlSymbian();
    ~UnitTestCxeSettingsControlSymbian();

private slots:
    void init();
    void cleanup();

    void testHandleSettingValueChanged();
    void testHandleSceneChanged();

private:
    CxeSettingsControlSymbian *mSettingsControl;

    CxeFakeSettings *mSettings;
    CxeFakeCameraDevice *mCameraDevice;
    CxeFakeCameraDeviceControl *mCameraDeviceControl;
};

#endif // UNITTEST_CXESETTINGSCONTROLSYMBIAN_H

