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
 * Description: Message Notifier server startup
 *
 */

//SYSTEM INCLUDES
#include <QCoreApplication>
#include <QTranslator>
#include <QLocale>
#include <QFile>
#include <QDateTime>
#include <QPointer>

#include "msgnotifier.h"
#include "debugtraces.h"

const QString debugFileName("c:/msgnotifier.txt");

#ifdef _DEBUG_TRACES_
void debugInit(QtMsgType type, const char *msg)
{

    QFile ofile(debugFileName);
    if (!ofile.open(QIODevice::Append | QIODevice::Text)) {
        qFatal("error opening results file");
        return;
    }
    QDateTime dt = QDateTime::currentDateTime();

    QTextStream out(&ofile);
    switch (type) {
    case QtDebugMsg:
        out << "\n DEBUG:";
        out << msg;
        break;
    case QtWarningMsg:
        out << "\n WARN:";
        out << msg;
        break;
    case QtCriticalMsg:
        out << "\n ";
        out << dt.toString("dd/MM/yyyy hh:mm:ss.zzz:ap");
        out << " CRITICAL:";
        out << msg;
        break;
    case QtFatalMsg:
        out << "\n FATAL:";
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
    QCoreApplication app(argc, argv);
    QString locale = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
    QTranslator translator;
    QTranslator translator_comm;
    translator.load(path + QString("messaging_") + locale);
    translator_comm.load(path + QString("common_") + locale);
    app.installTranslator(&translator);
    app.installTranslator(&translator_comm);
    
#ifdef _DEBUG_TRACES_
    //Debug Logs
    QFile ofile;
    if (ofile.exists(debugFileName)) {
        ofile.remove(debugFileName);
    }
    qInstallMsgHandler(debugInit);
#endif
	  // TODO: Get notifications at startup time.handled later
    QPointer<MsgNotifier> msgNotifier = new MsgNotifier();
    int r = app.exec();
    delete msgNotifier;
    return r;
}

