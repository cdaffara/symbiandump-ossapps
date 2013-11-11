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
* Description:
*
*/
//USER
#include "logsappsettings.h"
#include "logsapplication.h"
#include "logsmainwindow.h"
#include "logsviewmanager.h"
#include "logsservicehandler.h"
#include "logsservicehandlerold.h"
#include "logslogger.h"
#include "logsdefs.h"

//SYSTEM
#include <QObject>
#include <hbmainwindow.h>
#include <hbapplication.h>
#include <hbstyleloader.h>
#include <hbtranslator.h>
#include <QTranslator>

// TODO: Use QTranslator for now as HbTranslator has some bugs regarding
// language fallback handling, start using HbTranslator once those problems
// are fixed 
#define LOGS_USE_QTRANSLATOR

int main(int argc, char *argv[])
{
    LOGS_QDEBUG( "logs [UI] -> main()" )

    LogsAppSettings appSettings(argc, argv);
    LogsApplication app(argc, argv, appSettings);

#ifdef LOGS_USE_QTRANSLATOR
    QString lang = QLocale::system().name();
    QTranslator trans;
    QString path = "z:/resource/qt/translations/";
    if ( trans.load(path + "qt_" + lang) ){
        app.installTranslator(&trans);
    }
    QTranslator trans2;
    if ( trans2.load(path + "dialer_" + lang) ){
        app.installTranslator(&trans2);
    }
    QTranslator trans3;
    if ( trans3.load(path + "common_" + lang) ){
        app.installTranslator(&trans3);
    }
    
#else
    HbTranslator translator("dialer");
    translator.loadCommon();
#endif
    
    LogsMainWindow window;
    //Application ready signal is emitted after startup when UI is fully loaded and functional
    QObject::connect( &window, SIGNAL(viewReady()), &app, SLOT(testLogsHandleAppViewReady()) );  
    QObject::connect( &window, SIGNAL(appLostForeground()), &app, SLOT(testLogsResetAppReady()) );
    HbStyleLoader::registerFilePath(":/logslayouts");
    
    // Create service handler asap so that services are published fast.
    // Servicehandler cannot be created before HbApplication creation.
    // This can cause problem of service request not coming through if
    // HbApplication creation takes long time.
    LogsServiceHandler service;
    LogsServiceHandlerOld serviceOld;
    LogsViewManager viewManager(window, service, serviceOld, appSettings);
    
    // Don't show window yet as app might be started at background
    int err = app.exec();
    
    HbStyleLoader::unregisterFilePath(":/logslayouts");
    
    LOGS_QDEBUG( "logs [UI] <- main()" )
    return err;
}
