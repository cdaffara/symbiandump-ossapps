/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Unit test class
 *
 */

#ifndef UNITTEST_CXESCENEMODESTORE_H
#define UNITTEST_CXESCENEMODESTORE_H

#include <QObject>

// Tested class
#include "cxescenemodestore.h"

class UnitTestCxeSceneModeStore : public QObject
{
    Q_OBJECT

public:
    UnitTestCxeSceneModeStore();
    ~UnitTestCxeSceneModeStore();

private slots:
    void init();
    void cleanup();

    //actual tests
    void testSceneSettingValue();
    void testSceneSettingValueNotFound();
    void testSetSceneSettingValue();
    void testCurrentSceneId();
    void testCurrentScene();
    void testCurrentScene2();
    void testSetCurrentScene();
    void testGetScene();

private:
    // Tested class
    CxeSceneModeStore *mSceneModeStore;

};


#endif // UNITTEST_CXESCENEMODESTORE_H
