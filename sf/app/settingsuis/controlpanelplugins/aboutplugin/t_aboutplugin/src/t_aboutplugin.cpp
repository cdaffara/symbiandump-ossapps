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
* Description:  Main test class for about plugin library.
*
*/


#include <QtTest/QtTest>
#include "t_aboutplugin.h"
#include "cpaboutplugin.h"
#include "cpaboutview.h"
#include "cpaboutthirdpartyview.h"
#include "cpaboutopensourceview.h"
#include <xqsysinfo.h>

#include <CpItemDataHelper.h>
#include <hbInstance>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestAboutPlugin::init()
{
    mPlugin = new CpAboutPlugin;
    QVERIFY(mPlugin); 
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestAboutPlugin::cleanup()
{
    if (mPlugin) {
        delete mPlugin;
        mPlugin = 0;
        QVERIFY(mPlugin == 0);
    }
    
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestAboutPlugin::testCreateSettingFormItemData()
{    
	CpItemDataHelper itemDataHelper;
    QList<CpSettingFormItemData*> itemData = mPlugin->createSettingFormItemData(itemDataHelper); 
    QVERIFY(itemData.count() == 1);        
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestAboutPlugin::testCpAboutView()
{
    CpAboutView *view = new CpAboutView();
    QVERIFY(view);
    delete view;
    view = 0;
    QVERIFY(view == 0);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestAboutPlugin::testCpAboutViewOpenThirdPartyView()
{
    CpAboutView *view = new CpAboutView();
    QVERIFY(view);
    
    HbMainWindow *mainWindow = new HbMainWindow();
    mainWindow->addView(view);
    
    view->openThirdPartyView();
    const QString title = hbTrId("txt_cp_title_control_panel");
    QVERIFY(view->mThirdPartyView->title() == title);   
    view->handleThirdPartyViewBackAction();
    
    delete view;
    view = 0;
    QVERIFY(view == 0);
    delete mainWindow;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestAboutPlugin::testCpAboutViewOpenOpenSourceView()
{
    CpAboutView *view = new CpAboutView();
    QVERIFY(view);
    
    HbMainWindow *mainWindow = new HbMainWindow();
    mainWindow->addView(view);
    
    view->openOpenSourceView();
    const QString title = hbTrId("txt_cp_title_control_panel");
    QVERIFY(view->mOpenSourceView->title() == title);   
    view->handleOpenSourceViewBackAction();
    
    delete view;
    view = 0;
    QVERIFY(view == 0);
    delete mainWindow;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//

void TestAboutPlugin::testCpAboutThirdPartyView()
{
    CpAboutThirdPartyView *view = new CpAboutThirdPartyView();
    QVERIFY(view);
    delete view;
    view = 0;
    QVERIFY(view == 0);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestAboutPlugin::testCpAboutOpenSourceView()
{
    CpAboutOpenSourceView *view = new CpAboutOpenSourceView();
    QVERIFY(view);
    delete view;
    view = 0;
    QVERIFY(view == 0);
}
QTEST_MAIN(TestAboutPlugin)
