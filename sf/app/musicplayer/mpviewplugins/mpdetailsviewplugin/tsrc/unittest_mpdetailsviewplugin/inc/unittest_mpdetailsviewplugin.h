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
* Description: Unit test for MpDetailsViewPlugin
*
*/

#ifndef TESTMPDETAILSVIEWPLUGIN_H
#define TESTMPDETAILSVIEWPLUGIN_H

#include <QtTest/QtTest>

class MpDetailsViewPlugin;

class TestMpDetailsViewPlugin : public QObject
{
    Q_OBJECT

public:

    TestMpDetailsViewPlugin();
    virtual ~TestMpDetailsViewPlugin();

private slots: // not treated as testfunctions, executed by the testing framework
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

    MpDetailsViewPlugin          *mTest;

};

#endif  // TESTMPDETAILSVIEWPLUGIN_H
