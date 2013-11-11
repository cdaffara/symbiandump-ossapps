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

#include <hbapplication.h>
#include <cpmainwindow.h>
#include <cpmainview.h>
#include <hbstyleloader.h>
#include <hbtranslator.h>
#include <QLocale>
#include <QLatin1String>
#include <QDir>
#include <cplogger.h>

int main(int argc, char **argv)
{
    HbApplication app(argc, argv);
    
    //used by QSettings
    QCoreApplication::setOrganizationName("nokia");
    QCoreApplication::setOrganizationDomain("Orbit");
    QCoreApplication::setApplicationName("ControlPanel");

#ifdef ENABLE_CPFW_LOG
    INIT_LOGGER(CPFW_LOGGER_NAME,CP_LOGGER_CONFIG_PATH)
#endif
    
#ifdef ENABLE_CPPERF_LOG
    INIT_LOGGER(CPPERF_LOGGER_NAME,CP_LOGGER_CONFIG_PATH)
#endif

    CPFW_LOG("Entering ControlPanel.exe...");
    CPPERF_LOG("Entering ControlPanel.exe...");
       
    HbTranslator translator("control_panel");
    translator.loadCommon();
        
    HbStyleLoader::registerFilePath(":/widgetml/cpdataformlistentryviewitem.css");
    HbStyleLoader::registerFilePath(":/widgetml/cpdataformlistentryviewitem_color.css");
    HbStyleLoader::registerFilePath(":/widgetml/cpdataformlistentryviewitem.widgetml");

    CpMainWindow mainWindow;
    
    CpMainView *mainView = new CpMainView(&mainWindow);
    QObject::connect(mainView,SIGNAL(aboutToClose()),qApp,SLOT(quit()));
    mainWindow.addView(mainView);
    mainWindow.setCurrentView(mainView);
    
    mainWindow.show();
    
    int ret = app.exec();
    
    CPFW_LOG("Exiting ControlPanel.exe.");

    return ret;
}

// End of File
