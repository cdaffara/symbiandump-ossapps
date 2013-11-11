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
 * Description: This test class contains unit tests for two cxengine classes:
 * CxeSettings and CxeSettingsImp. CxeSettings is mostly a pure virtual
 * class containing couple of overloaded versions of get() methods, and so it is
 * tested together with CxeSettingsImp, which implements CxeSettings
 *
 */
#ifndef UNITTEST_CXESETTINGSIMP_H
#define UNITTEST_CXESETTINGSIMP_H

#include <QObject>
#include "cxesettingsimp.h"

class CxeFakeSettingsStore;
class CxeSettingsImp;

class UnitTestCxeSettingsImp : public QObject
{
Q_OBJECT

public:
    UnitTestCxeSettingsImp();
    virtual ~UnitTestCxeSettingsImp();

public slots:
    void testSlot();

private slots:
    void init();
    void cleanup();

    void testLoadImageSettings();
    void testLoadVideoSettings();
    void testLoadImageAndVideoSettings();

    void testGet();
    void testGet2();
    void testGet3();
    void testGetGarbage();
    void testSet();

    void testReset();
    void testGetVariationSetting();
    void testListenForSetting();

private:

    CxeFakeSettingsStore *mSettingsStore;
    CxeSettingsImp *mSettingsImp;
};

#endif

