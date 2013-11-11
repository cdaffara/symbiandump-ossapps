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

#include <QActionGroup>
#include <HbApplication>
#include <hbmainwindow.h>
#include <HbMenu>
#include <HbToolBar>
#include <HbAction>
#include <HbMessageBox>
#include <HbLabel>

#include "mainview.h"
#include "enginewrapper.h"


// ---------------------------------------------------------------------------

MainView::MainView(EngineWrapper &engine) :
    mEngine(engine),
    mValueDataContainer(0),
    mGraphDataContainer(0)
{
    setTitle(tr("Perf. Monitor"));
    mValueDataContainer = new ValueDataContainer(mEngine, this); 
    mGraphDataContainer = new GraphDataContainer(mEngine, this);
    createMenu();
    showValues();
}

// ---------------------------------------------------------------------------

MainView::~MainView()
{
}

// ---------------------------------------------------------------------------

void MainView::createMenu()
{
    HbMenu *menu = this->menu();
    HbToolBar *toolbar = this->toolBar();

    if (menu && toolbar) {
        mSwitchViewAction = new HbAction(toolbar);
        toolbar->addAction(mSwitchViewAction);

        QActionGroup *viewGroup = new QActionGroup(this);
        HbMenu *viewSubmenu = menu->addMenu(tr("View"));
        mValuesAction = viewSubmenu->addAction(tr("Values"), this, SLOT(showValues()));
        mValuesAction->setCheckable(true);
        mValuesAction->setChecked(true);
        viewGroup->addAction(mValuesAction);

        mGraphAction = viewSubmenu->addAction(tr("Graphs"), this, SLOT(showGraphs()));
        mGraphAction->setCheckable(true);
        viewGroup->addAction(mGraphAction);

        mLoggingAction = menu->addAction(QString(), this, SLOT(toggleLogging()));
        toolbar->addAction(mLoggingAction);
        updateLoggingAction();

        menu->addAction(tr("Settings..."), this, SIGNAL(settingsCommandInvoked()));
        menu->addAction(tr("About..."), this, SLOT(showAbout()));
        menu->addAction(tr("Exit"), qApp, SLOT(quit()));

    }
}

void MainView::showValues()
{
    // remove old widget & take ownership
    takeWidget();
    if(scene()){
        scene()->removeItem(mGraphDataContainer);
    }
    mGraphDataContainer->hideContainer();
    mValueDataContainer->showContainer(); 
    // set new widget
    this->setWidget(mValueDataContainer);
    
    mValuesAction->setChecked(true);

    mSwitchViewAction->setText(tr("View Graphs"));
    disconnect(mSwitchViewAction, SIGNAL(triggered(bool)), this, SLOT(showValues()));
    connect(mSwitchViewAction, SIGNAL(triggered(bool)), this, SLOT(showGraphs()));
}

void MainView::showGraphs()
{
    // remove old widget & take ownership
    takeWidget();
    if(scene()){
        scene()->removeItem(mValueDataContainer);
    }
    mValueDataContainer->hideContainer();    
    mGraphDataContainer->showContainer();
    // set new widget
    this->setWidget(mGraphDataContainer);

    mGraphAction->setChecked(true);

    mSwitchViewAction->setText(tr("View Values"));
    disconnect(mSwitchViewAction, SIGNAL(triggered(bool)), this, SLOT(showGraphs()));
    connect(mSwitchViewAction, SIGNAL(triggered(bool)), this, SLOT(showValues()));
}

void MainView::updateLoggingAction()
{
    mLoggingAction->setText(mEngine.settings().loggingEnabled() ?
                            tr("Stop Logging") :
                            tr("Start Logging"));
}

void MainView::toggleLogging()
{
    mEngine.setLoggingEnabled(!mEngine.settings().loggingEnabled());
    updateLoggingAction();
}

void MainView::showAbout()
{
    HbMessageBox *messageBox = new HbMessageBox(HbMessageBox::MessageTypeInformation);
    messageBox->setText("Version 1.2.2 - 24th September 2010. Copyright © 2010 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. Licensed under Eclipse Public License v1.0.");
    HbLabel *header = new HbLabel("About PerfMon", messageBox);
    messageBox->setHeadingWidget(header);
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->setTimeout(HbPopup::NoTimeout);
    messageBox->open();
}
