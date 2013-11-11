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
* Description: Unit test for mpequalizerwidget
*
*/

#ifndef TESTMPEQUALIZERWIDGET_H
#define TESTMPEQUALIZERWIDGET_H

#include <QtTest/QtTest>

class QTranslator;
class MpEqualizerWidget;

class TestMpEqualizerWidget: public QObject
{
    Q_OBJECT

public:

    TestMpEqualizerWidget();
    virtual ~TestMpEqualizerWidget();

signals:

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testInitialize();
    void testPrepareDialog();
    void testPresetSelected();
    void testCancelSelected();
    void testOkSelected();
    void testGetLocalizedString();

private:

    MpEqualizerWidget *mTest;
    QTranslator       *mMpTranslator; // Own

};

#endif  // TESTMPEQUALIZERWIDGET_H




