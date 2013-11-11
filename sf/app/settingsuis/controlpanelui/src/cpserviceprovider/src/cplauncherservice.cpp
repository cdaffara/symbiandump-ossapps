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

#include "cplauncherservice.h"
#include <QCoreApplication>
#include <cpservicemainwindow.h>
#include <cplauncherinterface.h>
#include <cppluginloader.h>
#include <cpbasesettingview.h>
#include "cpsplogger.h"

CpLauncherService::CpLauncherService(HbMainWindow *mainWindow /* = 0*/)
: XQServiceProvider("cpserviceprovider.com.nokia.symbian.ICpPluginLauncher",mainWindow),
  mMainWindow(mainWindow),
  mAsyncRequestIndex(-1),
  mReturnValue(false)
{
    CPSP_LOG_FUNC_ENTRY("CpLauncherService::CpLauncherService")
    
    publishAll();
    connect(this,SIGNAL(clientDisconnected()),this,SLOT(handleClientDisconnected()));
}

CpLauncherService::~CpLauncherService()
{
    CPSP_LOG_FUNC_ENTRY("CpLauncherService::~CpLauncherService")
}

bool CpLauncherService::complete()
{
    CPSP_LOG_FUNC_ENTRY("CpLauncherService::complete")
    
    CPSP_LOG( QString("CpLauncherService::complete() mAsyncRequestIndex = %1, mReturnValue = %2").arg(
            mAsyncRequestIndex).arg(mReturnValue.toBool()) )
    
    bool ret = completeRequest(mAsyncRequestIndex, mReturnValue);
    mAsyncRequestIndex = -1;
    return ret;
}

void CpLauncherService::setReturnValue(const QVariant &returnValue)
{
    CPSP_LOG_FUNC_ENTRY("CpLauncherService::setReturnValue")
    
    mReturnValue = returnValue;
}

bool CpLauncherService::launchSettingView(const QString &pluginFile,const QVariant &hint)
{
    CPSP_LOG_FUNC_ENTRY("CpLauncherService::launchSettingView");
   
    mAsyncRequestIndex = setCurrentRequestAsync();
    
    bool succeed = false;
       
    if (mMainWindow) {
        mMainWindow->show();
        CpLauncherInterface *plugin = CpPluginLoader::loadCpLauncherInterface(pluginFile);
        if (plugin) {
            CpBaseSettingView *settingView = plugin->createSettingView(hint);
            if (settingView) {
                connect(settingView, SIGNAL(returnValueDelivered(QVariant)),this,SLOT(setReturnValue(QVariant)));
                (static_cast<CpServiceMainWindow*>(mMainWindow))->setSettingView(settingView);                
                succeed = true;
            }
            else {
                CPSP_LOG("Create setting view failed.");
            }
        }
        else {
            CPSP_LOG(QLatin1String("Load plugin interface(CpLauncherInterface) failed: ") + pluginFile);
        }
        
        if (!succeed) {
            qApp->quit();  //quit application if loading plugin failed or creating setting view failed.
        }
    }
    
    return succeed;
}

void CpLauncherService::handleClientDisconnected()
{
    CPSP_LOG_FUNC_ENTRY("CpLauncherService::handleClientDisconnected")
        
    qApp->quit();
}

//End of File
