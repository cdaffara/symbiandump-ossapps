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
* Description: Unit test for MpSettingsManager
*
*/

#ifndef TESTMPSETTINGSMANAGER_H
#define TESTMPSETTINGSMANAGER_H

#include <QtTest/QtTest>

class TestMpSettingsManager : public QObject
{
    Q_OBJECT

public:

    TestMpSettingsManager();
    virtual ~TestMpSettingsManager();

signals:


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testConstructor(); //don't change the order, this should be the first test.
    void testInstanceConstruction();
    void testFirstStartupGet();
    void testShuffleGetAndSet();
    void testRepeatGetAndSet();
    void testPresetGetAndSet();
    void testInspireMeGetAndSet();
    void testSongDetailsGbGetAndSet();
    void testShowMtpInfoGetAndStop();
    void testMtpInfoUrlGet();
    void testValueChangedShuffleCase();
    void testValueChangedRepeatCase();
    void testValueChangedPresetCase();
    void testValueChangedInspireMe();
    void testValueChangedSongDetailsGb();
    void testValueChangedShowMtpInfo();

};

#endif  // TESTMPSETTINGSMANAGER_H

