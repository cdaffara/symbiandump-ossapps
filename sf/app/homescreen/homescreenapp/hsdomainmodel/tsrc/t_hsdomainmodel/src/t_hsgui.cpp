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
* Description:
*
*/

#include "t_hsdomainmodel.h"
#include "hsgui.h"


void TestHsDomainModel::testGuiStaticInterface()
{    
    HsGui *stored = HsGui::takeInstance();
    
    HsGui *gui = new HsGui;    
    HsGui::setInstance(gui);
    HsGui::setInstance(gui);
    QVERIFY(HsGui::instance() == gui);    
    HsGui *gui2 = HsGui::takeInstance();    
    QVERIFY(gui == gui2);
    delete gui;
    
    HsGui::setInstance(stored);
}

void TestHsDomainModel::testSetupCleanupIdleUi()
{
    HsGui *gui = HsGui::instance();    

    gui->setupIdleUi();
    QVERIFY(gui->idleWidget());
    QVERIFY(gui->idleView());    
    gui->setupIdleUi();

    gui->cleanupIdleUi();    
    QVERIFY(!gui->idleView());
}


void TestHsDomainModel::testOrientation()
{
    HsGui *gui = HsGui::instance();
    Qt::Orientation orientation = gui->orientation();
    
    if (orientation == Qt::Horizontal){
        gui->setOrientation(Qt::Vertical);
    }
    else {
        gui->setOrientation(Qt::Horizontal);
    }        
    QVERIFY(gui->orientation() != orientation);
    
    gui->setOrientation(orientation);
    QVERIFY(gui->orientation() == orientation);
}


void TestHsDomainModel::testShow()
{
    HsGui *gui = HsGui::instance();
    gui->setupIdleUi();
    gui->show();
    gui->cleanupIdleUi();    
}

void TestHsDomainModel::testPageChangeAnimation()
{
    HsGui *gui = HsGui::instance();
    
    QVERIFY(gui->pageChangeAnimation());
    gui->cleanupIdleUi();    
}

void TestHsDomainModel::testPageCrawlingAnimation()
{
    HsGui *gui = HsGui::instance();
    
    QVERIFY(gui->pageCrawlingAnimation());
    gui->cleanupIdleUi();    
}
