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

#include "cpservicemainwindow.h"
#include <QCoreApplication>
#include <cpbasesettingview.h>
#include "cplauncherservice.h"
#include "cpsplogger.h"

CpServiceMainWindow::CpServiceMainWindow(QWidget *parent /* = 0*/)
: HbMainWindow(parent), mLauncherService(0), mPreviousView(0)
{
    CPSP_LOG_FUNC_ENTRY("CpServiceMainWindow::CpServiceMainWindow")
        
    mLauncherService = new CpLauncherService(this);
}

CpServiceMainWindow::~CpServiceMainWindow()
{
    CPSP_LOG_FUNC_ENTRY("CpServiceMainWindow::~CpServiceMainWindow")
}

void CpServiceMainWindow::setSettingView(CpBaseSettingView *settingView)
{
    CPSP_LOG_FUNC_ENTRY("CpServiceMainWindow::setSettingView")
    
    mSettingViewPointer = settingView;
    
    mPreviousView = currentView();
    
    connect(settingView, SIGNAL(aboutToClose()), this, SLOT(quit()));    
    addView(settingView);
    setCurrentView(settingView);
}

void CpServiceMainWindow::quit()
{
    CPSP_LOG_FUNC_ENTRY("CpServiceMainWindow::quit");
    
    closeSettingView();
    
    connect(mLauncherService, SIGNAL(returnValueDelivered()), qApp, SLOT(quit()));
    mLauncherService->complete();
  
    /*
    hide();
    mLauncherService->complete();
    if (mSettingViewPointer) {
        removeView(mSettingViewPointer);
        mSettingViewPointer->deleteLater();
    }  
    */
}

void CpServiceMainWindow::closeSettingView()
{
    CPSP_LOG_FUNC_ENTRY("CpServiceMainWindow::closeSettingView")
    
    if (mSettingViewPointer) {
        removeView(mSettingViewPointer);
        mSettingViewPointer->deleteLater();
    }
    
    setCurrentView(mPreviousView);
}

//End of File
