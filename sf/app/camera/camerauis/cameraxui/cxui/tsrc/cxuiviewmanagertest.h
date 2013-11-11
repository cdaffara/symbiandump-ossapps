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
#ifndef CXUIVIEWMANAGERTEST_H
#define CXUIVIEWMANAGERTEST_H

#include <QObject>
#include <hbview.h>
#include "cxuifakeengine.h"

class CxuiFakeEngine;
class CXUIViewfinderWidget;
class QGraphicsWidget;
class HbMainWindow;

class CxuiViewmanagerTest : public QObject
{
    Q_OBJECT

private slots:
    // Called before each test case ( not yet needed...)
    //void init();
    // Called first before first test case
    void initTestCase();
    
    //void initStillImageMode();
    
    void cleanup();

    void createAndDestroyEngine();
    
    void cleanupTestCase();
    
signals:
    void toPrecaptureView();
    void toPostcaptureView();

private:
    HbMainWindow *mMainWindow;
    //CxeEngine *mEngine;  
    CxuiFakeEngine *mEngineFake;
};

#endif // CXEENGINETEST_H
