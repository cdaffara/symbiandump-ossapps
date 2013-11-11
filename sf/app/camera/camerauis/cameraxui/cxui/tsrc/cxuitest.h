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
#ifndef CXUITEST_H
#define CXUITEST_H

#include <QObject>
#include <hbview.h>
#include "cxeengine.h"
#include "cxuiviewmanagertest.h"
#include "cxuiviewmanager.h"

class CxeEngine;
class QGraphicsWidget;
class HbMainWindow;
class CxuiViewManager;

class TestCxUi : public QObject
{
    Q_OBJECT

private slots:
    // Called before each test case 
    //void init();    
    // Called first before first test case
    void initTestCase();
    // Called after every testfunction
    void cleanup();
    // Called after the last testfunction was executed.
    void cleanupTestCase();      
    
    // Inits still image mode
    //void initStillImageMode();
    
    // Tests for CxuiViewManager class
    void testViewManager();
    void testCaptureKeyInImageMode();
    // This is implemented in cxuitest.cpp
    void initVideoMode();
    void testCaptureKeyInVideoMode();
    
signals:
    void toPrecaptureView();
    void toPostcaptureView();
    
private:
    void sendCameraKeyEvent(QEvent::Type event = QEvent::None);
    void sendAutofocusKeyEvent(QEvent::Type event);

private:
    HbMainWindow *mMainWindow;
    CxuiCaptureKeyHandler *mKeyHandler;
    CxuiFakeEngine *mEngineFake;
    CxuiViewManager *mViewManager;
};

#endif // CXEENGINETEST_H
