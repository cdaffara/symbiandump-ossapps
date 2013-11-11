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
* Description: Unit test for MpPlaybackViewPlugin
*
*/

#ifndef TESTMPPLAYBACKVIEWPLUGIN_H
#define TESTMPPLAYBACKVIEWPLUGIN_H

#include <QtTest/QtTest>

class MpPlaybackViewPlugin;

class TestMpPlaybackViewPlugin : public QObject
{
    Q_OBJECT

public:

    TestMpPlaybackViewPlugin();
    virtual ~TestMpPlaybackViewPlugin();

signals:


public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void testConstructor();
    void testCreateView();
    void testdestroyView();
    void testActivateView();
    void testDeactivateView();
    void testGetView();
private:

    MpPlaybackViewPlugin          *mTest;

};

#endif  // TESTMPPLAYBACKVIEWPLUGIN_H
