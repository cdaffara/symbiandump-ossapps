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
#ifndef UNITTEST_CXESETTINGSMODELIMP_H
#define UNITTEST_CXESETTINGSMODELIMP_H

#include <QObject>
#include "cxenamespace.h"

// forward declaration
class CxeSettingsModelImp;

class UnitTestCxeSettingsModelImp : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeSettingsModelImp();
    ~UnitTestCxeSettingsModelImp();

private slots:
    void init();
    void cleanup();
    void testSettingValues();
    void testRuntimeValues();
    void testImageScene();
    void testVideoScene();
    void testImageSceneSettingValues();
    void testVideoSceneSettingValues();


private:
    CxeSettingsModelImp *mSettingsModel;
};

#endif // UNITTEST_CXESETTINGSMODELIMP_H
