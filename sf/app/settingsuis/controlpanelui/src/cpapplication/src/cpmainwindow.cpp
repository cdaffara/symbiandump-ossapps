/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
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

#include "cpmainwindow.h"
#include "cpmainview.h"
#include <cpplugininterface.h>
#include <cpcategorysettingformitemdata.h>
#include <hbapplication.h>
#include <cpevent.h>
#include <QTimer>
//CpMainWindow implementation
CpMainWindow::CpMainWindow(QWidget *parent /*= 0*/, Hb::WindowFlags windowFlags /*= Hb::WindowFlagNone*/)
: HbMainWindow(parent,windowFlags),mStartupTimer(0)
{
    // start a zero interval timer to identify the startup of control panel is finished.
    mStartupTimer = new QTimer(this);         
    connect(mStartupTimer, SIGNAL(timeout()), this, SLOT(onStartupFininshed()));
    mStartupTimer->start();
}

CpMainWindow::~CpMainWindow()
{
    CpMainView *mainView = qobject_cast<CpMainView *>(views().first());
    if(mainView != 0) {
        mainView->saveActivity();
    }
    if(mStartupTimer != 0) {
        delete mStartupTimer;
    } 
}

bool CpMainWindow::event(QEvent *e)
{
    if (e->type() == (QEvent::Type)(CpCreatePluginItemDataEvent::CreatePluginItemData)) {
        CpCreatePluginItemDataEvent *event = static_cast<CpCreatePluginItemDataEvent*>(e);
        ::createCpPluginItemData(event);
        e->accept();
    }
    return HbMainWindow::event(e);
}

/**
 * Emit applicationReady signal when start up process is finished.
 */
void CpMainWindow::onStartupFininshed()
{
    // emit applicationReady signal when the all initilizing event has been deal with.
    // only emit it once
    emit applicationReady();
    if (mStartupTimer != 0) {
        mStartupTimer->stop();
        delete mStartupTimer;
        mStartupTimer = 0;
    }    
}
//End of File


