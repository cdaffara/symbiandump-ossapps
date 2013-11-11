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
* Description:  Offers message creation and sending services.
 *
*/

#include <hbaction.h>
#include <hbdevicedialog.h>
#include <qtdebug>
#include <xqservicerequest.h>
#include "testshareuiview.h"
#include "shareui.h"

TestShareUiView::TestShareUiView(HbMainWindow* parent)
    {
    // TODO Auto-generated constructor stub

    }

TestShareUiView::~TestShareUiView()
    {
    // TODO Auto-generated destructor stub
    delete mDialog;
    }

bool TestShareUiView::init()
    {
    // initialize
    qDebug() << "[DeviceView] " << "init()";
    mToolBar = toolBar();
    
    // clear actions
    mToolBar->clearActions();
    
    // add the actions now
    // when you press this, a device dialog should pop up.
    HbAction* openCvAction = new HbAction( HbIcon(""), tr("Launch Share Ui"), this);
    mToolBar->addAction(openCvAction);
    connect(openCvAction, SIGNAL(triggered()), this, SLOT(openCvPop()));
    
    return true;
    }

void TestShareUiView::openCvPop()
    {
    qDebug() << "[DeviceView] " << "openCvPop() ";
    mDialog = new ShareUi();
    QStringList fileList;
    fileList << QString("c:\\Sunset.jpg");
    mDialog->send(fileList,true);    
    }

