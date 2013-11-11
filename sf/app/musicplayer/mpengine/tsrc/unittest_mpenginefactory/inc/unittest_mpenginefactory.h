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
* Description: Unit test for mpenginefactory
*
*/

#ifndef TESTMPENGINEFACTORY_H
#define TESTMPENGINEFACTORY_H

#include <QtTest/QtTest>

class MpEngineFactory;

class TestMpEngineFactory : public QObject
{
    Q_OBJECT

public:

    TestMpEngineFactory();
    ~TestMpEngineFactory();

public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testInstance();
    void testCreateSharedEngine();
    void testCreateIsolatedEngine();
    void testSharedEngine();
    void testClose(); 

private:

    MpEngineFactory *mTest;
};

#endif  // TESTMPENGINEFACTORY_H




