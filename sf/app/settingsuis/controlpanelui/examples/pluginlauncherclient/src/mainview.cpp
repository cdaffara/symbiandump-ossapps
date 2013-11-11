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
#include "mainview.h"
#include <hbmenu.h>
#include <hbaction.h>
#include <qcoreapplication.h>
#include <cppluginlauncher.h>
#include <QStringList>
#include <cpbasesettingview.h>
#include <hbmessagebox.h>
#include <xqaiwrequest.h>
#include <XQServiceRequest.h>
#include <QTimer>

MainView::MainView(QGraphicsItem *parent/* = 0*/)
: HbView(parent),mRequest(0)
{
	init();
}

MainView::~MainView()
{
    delete mRequest;
}

void MainView::init()
{
  setTitle(tr("CpPlugin Launcher"));  
  
  HbMenu *menu = new HbMenu();
  setMenu(menu);
  
  HbAction *action = menu->addAction(tr("Launch View(in process)"));
  connect(action, SIGNAL(triggered()), this, SLOT(launchInProcessProfileView()));

  action = menu->addAction(tr("Launch View(QtHighway)"));
  connect(action, SIGNAL(triggered()), this, SLOT(launchQtHighwayProfileView()));
}

void MainView::launchInProcessProfileView()
{
    CpBaseSettingView *settingView = CpPluginLauncher::launchSettingView("cppersonalizationplugin.dll","profile_view");
    if (settingView) {
        connect(settingView,SIGNAL(returnValueDelivered(QVariant)),this,SLOT(handleReturnValue(QVariant)));
    }
}

void MainView::launchQtHighwayProfileView()
{
    if (mRequest) {
        delete mRequest;
        mRequest = 0;
    }
    
    mRequest = mAppMgr.create("com.nokia.symbian.ICpPluginLauncher", "launchSettingView(QString,QVariant)", true);

    if (!mRequest)
    {
        return;
    }
    else
    {
        connect(mRequest, SIGNAL(requestOk(QVariant)), SLOT(handleReturnValue(QVariant)));
        connect(mRequest, SIGNAL(requestError(int,QString)), SLOT(handleError(int,QString)));
    }


    // Set arguments for request 
    QList<QVariant> args;
    args << QVariant( "cppersonalizationplugin.dll" );
    args << QVariant ( "profile_view" );
    mRequest->setArguments(args);

    mRequest->setSynchronous(false);
    
    QTimer::singleShot(20* 1000, this, SLOT(closeSettingView()));
    
    // Make the request
    if (!mRequest->send())
    {
        //report error     
    }
    
}

void MainView::handleReturnValue(const QVariant &returnValue)
{
    HbMessageBox::information( QString("Return value:") + returnValue.toString());
}

void MainView::handleError(int errorCode,const QString &errorMessage)
{
    HbMessageBox::information( QString("handle error:") + errorMessage);
}

void MainView::closeSettingView()
{   
    if (mRequest) {
        delete mRequest;
        mRequest = 0;
    }
}

//End of File
