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
* Description:
*
*/
#ifndef CXEENGINETEST_H
#define CXEENGINETEST_H

// Traces are mandatory for automated testing
#ifndef _DEBUG
    #define _DEBUG
#endif

#include <QObject>
#include <hbmainwindow.h>

class CxeEngine;

class TestCxeEngine : public QObject
{
    Q_OBJECT

private:
    void init();
    void cleanup();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void createAndDestroyEngine();
    void initStillImageMode();

    void testAutoFocusBasic();
    void testImageCapture();
    void testBatchCapture();

    void initVideoMode();
    void testVideoCapture();

private:
    CxeEngine *mEngine; // This will be recreated for each test
    HbMainWindow *mWindow;
};

#endif // CXEENGINETEST_H
