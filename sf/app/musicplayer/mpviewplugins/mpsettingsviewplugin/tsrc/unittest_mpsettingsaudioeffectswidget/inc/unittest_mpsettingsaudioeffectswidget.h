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
* Description: Unit test for mpaudioeffectswidget
*
*/

#ifndef TESTMPSETTINGSAUDIOEFFECTSWIDGET_H
#define TESTMPSETTINGSAUDIOEFFECTSWIDGET_H

#include <QtTest/QtTest>

class QTranslator;
class MpSettingsAudioEffectsWidget;

class TestMpSettingsAudioEffectsWidget: public QObject
{
    Q_OBJECT

public:

    TestMpSettingsAudioEffectsWidget();
    virtual ~TestMpSettingsAudioEffectsWidget();

signals:

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testInitializeSlider();
    void testInitializeLoudnessOn();
    void testInitializeLoudnessOff();
    void testFilterSignals();

private:

    MpSettingsAudioEffectsWidget *mTest;
    QTranslator       *mMpTranslator; // Own

};

#endif  // TESTMPSETTINGSAUDIOEFFECTSWIDGET_H




