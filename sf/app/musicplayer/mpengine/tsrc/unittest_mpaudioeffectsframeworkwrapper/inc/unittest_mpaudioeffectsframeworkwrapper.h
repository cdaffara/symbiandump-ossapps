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
* Description: Unit test for mpaudioeffectsframeworkwrapper
*
*/

#ifndef TESTMPAUDIOEFFECTSFRAMEWORKWRAPPER_H
#define TESTMPAUDIOEFFECTSFRAMEWORKWRAPPER_H

#include <QtTest/QtTest>

class MpAudioEffectsFrameworkWrapper;
class MpAudioEffectsFrameworkWrapperPrivate;

class TestMpAudioEffectsFrameworkWrapper : public QObject
{
    Q_OBJECT

public:
    TestMpAudioEffectsFrameworkWrapper();
    virtual ~TestMpAudioEffectsFrameworkWrapper();

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testConstructor();
    void testInit();
    void testBalance();
    void testLoudness();
    void testSetBalance();
    void testSetLoudness();
    void testSaveToFile();
    
private:
    MpAudioEffectsFrameworkWrapper          *mTest;
    MpAudioEffectsFrameworkWrapperPrivate   *mTestPrivate;

};

#endif  // TESTMPAUDIOEFFECTSFRAMEWORKWRAPPER_H
