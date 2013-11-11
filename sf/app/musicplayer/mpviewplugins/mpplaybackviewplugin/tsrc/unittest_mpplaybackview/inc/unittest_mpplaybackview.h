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
* Description: Unit test for mpplaybackview
*
*/

#ifndef TESTMPPLAYBACKVIEW_H
#define TESTMPPLAYBACKVIEW_H

#include <QtTest/QtTest>
#include "mpengine.h"
#include "mpviewbase.h"
#include "mpequalizerwidget.h"


class MpPlaybackView;

class TestMpPlaybackView: public QObject
{
    Q_OBJECT

public:

    TestMpPlaybackView();
    virtual ~TestMpPlaybackView();

signals:

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
//Test Cases
    void testInitializeView();
    void testActivateView();
    void testDeactivateView();
    void testShuffleEnabled();
    void testRepeatEnabled();
    void testPlaybackStatus();

    void testStartSettingsView();
    void testBack();
    void testExit();

    void testPlaybackStateChanged();
    void testFlip();
    void testShuffle();
    void testRepeat();

    void testHandleSongSelected();
    void testShowEqualizerDialog();
    void testCloseEqualizerDialog();
    void testShowCorruptedNote();

    void testSeekForward();
    void testSeekRewind();
    void testEndRewind();
    void testEndForward();

private:
   MpPlaybackView *mTest;
   MpPlaybackData *mPlaybackData;
   MpViewBase     *mViewBase;
   MpEqualizerWidget *mEqualizerWidget;

};

#endif  // TESTMPPLAYBACKVIEW_H




