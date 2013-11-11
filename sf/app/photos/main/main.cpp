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
* Description:   ?Description
*
*/


#include <hbmainwindow.h>
#include <glxloggerenabler.h>

#include <hbstyle.h>
#include <QDebug>
#include <QTranslator>
#include <xqserviceutil.h>

#include "glxstatemanager.h"
#include "glxaiwservicehandler.h"
#include "glxapplication.h"
#include "glxlocalisationstrings.h"
#include "hbsplashscreen.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "mainTraces.h"
#endif

int main(int argc, char *argv[])
{
    OstTrace0( TRACE_IMPORTANT, _MAIN, "::main" );
    QApplication::setGraphicsSystem("openvg");
    
    QTime localTime = QTime::currentTime();
    OstTraceExt3( TRACE_NORMAL, DUP1__MAIN, "::main Time at Launch HHMMSS =%d::%d::%d",
            localTime.hour(), localTime.minute(), localTime.second() );

    GlxApplication app(argc, argv, XQServiceUtil::isService(argc, argv) ? Hb::NoSplash : Hb::DefaultApplicationFlags);	
    bool loaded(false);	
    QTranslator translator;
    QString path = "Z:/resource/qt/translations/";
    
    loaded = translator.load("photos_" + QLocale::system().name(), path);
    if(loaded) {
         qApp->installTranslator(&translator);
    }
    
    GlxStateManager* stateMgr = NULL;
    GlxAiwServiceHandler* mainWindow = NULL;

    OstTraceEventStart0( EVENT_DUP1__MAIN_START, "launch" );

    if(!XQServiceUtil::isService()){
        
        stateMgr = new GlxStateManager();
        app.setApplicationName( GLX_TITLE );
        stateMgr->launchApplication();  
    }
    else {
        mainWindow = new GlxAiwServiceHandler();
        mainWindow->show();
    }
    app.initMattiAutomation();
    OstTraceEventStop( EVENT_DUP1__MAIN_STOP, "launch", EVENT_DUP1__MAIN_START );

    int ret = app.exec();
    delete stateMgr;
    delete mainWindow;
    return ret;
}
