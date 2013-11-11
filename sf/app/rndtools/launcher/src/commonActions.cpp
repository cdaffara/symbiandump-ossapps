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

#include <hbapplication.h>
#include <hbinstance.h>

#include "hbaction.h"
#include "hbmenu.h"
#include "commonActions.h"
#include "notifications.h"
#include "enginewrapper.h"

// ---------------------------------------------------------------------------

CommonActions::CommonActions(EngineWrapper *engine, HbApplication *application)
    : mEngine(engine), mApplication(application)
{

    
    mActionStopLaunch = new HbAction("Stop launch", this);
    mActionSendLog = new HbAction("Send log", this);
    mActionDeleteLog = new HbAction("Delete log", this);
    mActionSendDllInfo = new HbAction("Send list of DLLs", this);
    mActionCompareDllInfo = new HbAction("Dll BC analysis", this);
    mActionAbout = new HbAction("About", this);
    mActionExit = new HbAction("Exit", this);


    connectSignalsAndSlots();
}

// ---------------------------------------------------------------------------

CommonActions::~CommonActions()
{
}

// ---------------------------------------------------------------------------

void CommonActions::showAbout()
{
    Notifications::about();
}

// ---------------------------------------------------------------------------

void CommonActions::exit()
{
    mApplication->exit();
}

// ---------------------------------------------------------------------------

void CommonActions::sendDllInfo()
{
    if (!mEngine->sendListOfDlls()) {
        Notifications::showErrorNote("Unable to send list");
    }

}

// ---------------------------------------------------------------------------

void CommonActions::compareDllInfo()
{
    mEngine->compareDlls();
}

// ---------------------------------------------------------------------------

void CommonActions::sendLog()
{
    if (!mEngine->sendLog()) {
        Notifications::showErrorNote("Unable to send log");
    }
}

// ---------------------------------------------------------------------------

void CommonActions::deleteLog()
{
    if (mEngine->deleteLog()) {
        QString text = "Log file(s) deleted";
        Notifications::showInformationNote(text);
    }
    else {
        QString text = "Unable to delete";
        Notifications::showErrorNote(text);
    }
}

// ---------------------------------------------------------------------------

void CommonActions::stopLaunch()
{
    if (!mEngine->stopLaunching()) {
        Notifications::showErrorNote("Unable to stop launching");
    }
}

// ---------------------------------------------------------------------------

void CommonActions::addActionsToMenu(HbMenu *menu)
{
    menu->addAction(mActionStopLaunch);
    menu->addAction(mActionSendLog);
    menu->addAction(mActionDeleteLog);
    HbMenu *menuDllInfo = menu->addMenu("Dll Info");
    menuDllInfo->addAction(mActionSendDllInfo);
    menuDllInfo->addAction(mActionCompareDllInfo);
    menu->addAction(mActionAbout);
    menu->addAction(mActionExit);
}

// ---------------------------------------------------------------------------

void CommonActions::connectSignalsAndSlots()
{
    connect(mActionStopLaunch, SIGNAL(triggered()), this, SLOT(stopLaunch()));
    connect(mActionSendDllInfo, SIGNAL(triggered()), this, SLOT(sendDllInfo()));
    connect(mActionCompareDllInfo, SIGNAL(triggered()), this, SLOT(compareDllInfo()));
    connect(mActionSendLog, SIGNAL(triggered()), this, SLOT(sendLog()));
    connect(mActionDeleteLog, SIGNAL(triggered()), this, SLOT(deleteLog()));
    connect(mActionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(mActionExit, SIGNAL(triggered()), this, SLOT( exit() ) );
}


// ---------------------------------------------------------------------------
