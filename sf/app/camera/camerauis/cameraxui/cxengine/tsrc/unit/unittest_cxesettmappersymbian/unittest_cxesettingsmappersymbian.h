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
#ifndef UNITTEST_CXESETTINGSMAPPERSYMBIAN_H
#define UNITTEST_CXESETTINGSMAPPERSYMBIAN_H

#include <QObject>

/*
* class which implements unit test cases for CxeSettingsMapperSymbian
*/
class UnitTestCxeSettingsMapperSymbian : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeSettingsMapperSymbian();
    ~UnitTestCxeSettingsMapperSymbian();

private slots:
    void init();
    void cleanup();

    void testMap2CameraWb();
    void testMap2CameraFlash();
    void testMap2CameraExposureMode();
    void testMap2CameraEffect();
    void testMap2CameraAutofocus();
    void testMap2CameraOrientation();
    void testMap2UseCaseHintVideoParameters();

};

#endif // UNITTEST_CXESETTINGSMAPPERSYMBIAN_H
