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
* Description: Messaging service application startup main().
*
*/

#include <hbapplication.h>
#include <QTranslator>
#include <QLocale>
#include <QFile>
#include <QPointer>
#include <QDateTime>
#include <HbSplashScreen>
#include <xqserviceutil.h>

#include "msgservicewindow.h"
#include "debugtraces.h"

//Localised constants
#define LOC_TITLE hbTrId("txt_messaging_title_messaging")

const QString debugFileName("c:/msgservice_app.txt");
const QString TRANSLATOR_FILE_PATH("z:/resource/qt/translations/");

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

int main(int argc, char **argv)
{
    // Need splashScreen at construction time itself?
    bool needImmdSplashScreen = true;

    // Decision logic to launch the splash screen based on the service called
    QString serviceName = XQServiceUtil::interfaceName( argc, argv);
    if( !serviceName.compare( QString( "com.nokia.symbian.IMessageSend") ) )
        {
        HbSplashScreen::setScreenId( "sendservice" );
        }
    else if ( !serviceName.compare( "com.nokia.symbian.IMessageView") )
        {
        HbSplashScreen::setScreenId( "viewservice" );
        needImmdSplashScreen = false;
        }

    // Appropriate HbApplication initialization
    HbApplication app( argc, argv,
            needImmdSplashScreen? Hb::DefaultApplicationFlags : Hb::NoSplash);

    // Install translator.
    QString locale = QLocale::system().name();
    QTranslator translator;
    QTranslator translator_comm;
    translator.load(TRANSLATOR_FILE_PATH + QString("messaging_") + locale);
    translator_comm.load(TRANSLATOR_FILE_PATH + QString("common_") + locale);
    app.installTranslator(&translator);
    app.installTranslator(&translator_comm);
    app.setApplicationName(LOC_TITLE);

    // Create mainwindow, but don't show it just yet.
    // We shall show the mainwindow at appropriate places later in the code.
    QPointer<MsgServiceWindow> window = new MsgServiceWindow();

    // Start event-loop
    int rv = app.exec();
    delete window;
    return rv;
}

