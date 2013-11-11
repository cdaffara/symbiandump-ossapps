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
 * Description:main() for messaging application.
 *
 */

#include <hbapplication.h>
#include <QTranslator>
#include <QLocale>
#include <QFile>
#include "debugtraces.h"
#include <QDateTime>
#include <QPointer>
#include <HbSplashScreen>

#include "msgmainwindow.h"
#include "msgapplication.h"

//Localised constants
#define LOC_TITLE hbTrId("txt_messaging_title_messaging")

const QString debugFileName("c:/art2_app_log.txt");
const QString activityParam("-activity");
const int INVALID_MSGID = -1;

#ifdef _DEBUG_TRACES_
void debugInit(QtMsgType type, const char *msg)
{

    QFile ofile(debugFileName);
    if (!ofile.open(QIODevice::Append | QIODevice::Text))
    {
        qFatal("error opening results file");
        return;
    }
    QDateTime dt = QDateTime::currentDateTime();

    QTextStream out(&ofile);
    switch (type)
    {
        case QtDebugMsg:
            out << " DEBUG:";
            out << msg;
            break;
        case QtWarningMsg:
            out << " WARN:";
            out << msg;
            break;
        case QtCriticalMsg:
            out << "\n ";
            out << dt.toString("dd/MM/yyyy hh:mm:ss.zzz:ap");
            out << " CRITICAL:";
            out << msg;
            break;
        case QtFatalMsg:
            out << " FATAL:";
            out << msg;
            abort();
            break;
        default:
            out << " No Log Selection Type:";
            out << msg;
            break;

    }
}

#endif
int main(int argc, char *argv[])
{
    
    QCRITICAL_WRITE("MsgApp start.");
    
    QString firstArg(argv[1]);
    bool serviceRequest = false;
    // check for argc is greater than 1 and its not from activity
    if(argc >1 && firstArg != activityParam )
    {
        serviceRequest = true;
        HbSplashScreen::setScreenId("dummy");
    }
    else
    {
        HbSplashScreen::setScreenId("clv");   
    }
    
    // Application
    //MsgApplication class dervied from HbApplication is create in order to 
    //emit applicationReady signal for Matti tool
    MsgApplication app(argc, argv);
    

    //TODO: Uncomment the lines when actual translation files are available in sdk and remove loading locally.
    QString locale = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
    QTranslator translator;
    QTranslator translator_comm;
    translator.load(path + QString("messaging_") + locale);
    translator_comm.load(path + QString("common_") + locale);
    app.installTranslator(&translator);
    app.installTranslator(&translator_comm);

    app.setApplicationName(LOC_TITLE);
    
#ifdef _DEBUG_TRACES_
    //Debug Logs
    QFile ofile;
    if (ofile.exists(debugFileName))
    {
        ofile.remove(debugFileName);
    }
    qInstallMsgHandler(debugInit);
#endif
    
    // Main window
    QPointer<MsgMainWindow> mainWindow = 
            new MsgMainWindow(serviceRequest);
    app.initViewReady();    
    mainWindow->show();

    // Event loop
    int error = app.exec();
    HbApplication::processEvents();
    
    // delete main window and return error
    delete mainWindow;
    
    return error;
}

// End of file
