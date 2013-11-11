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

#include <HbAction>

#include "mainwindow.h"

#include "enginewrapper.h"
#include "mainview.h"
#include "settingsview.h"

MainWindow::MainWindow(EngineWrapper &engine, QWidget *parent)
    : HbMainWindow(parent)
    , mEngine(engine)
{
    mMainView = new MainView(mEngine);
    connect(mMainView, SIGNAL(settingsCommandInvoked()), this, SLOT(showSettings()));

    addView(mMainView);
    
    mSettingsView = new SettingsView(mEngine);
    connect(mSettingsView, SIGNAL(finished(bool)), this, SLOT(showMainView()));
    
    HbAction *action = new HbAction(Hb::BackNaviAction, mSettingsView);        
    connect(action, SIGNAL(triggered()), mSettingsView, SLOT(reject()));
    mSettingsView->setNavigationAction(action);

    addView(mSettingsView);
    
    showMainView();
}

void MainWindow::showMainView()
{
    setCurrentView( mMainView );
}

void MainWindow::showSettings()
{
    setCurrentView( mSettingsView );
}
