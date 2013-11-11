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
#include <QCoreApplication>
#include <QProcess>
#include <QThread>
#include <QDebug>
#include <QDir>
#include <QTime>
#include <QFile>

#include "cxetestrunner.h"

CxeTestRunner::CxeTestRunner()
{
}

CxeTestRunner::~CxeTestRunner()
{
}

void CxeTestRunner::runTests()
{
#ifdef __WINSCW__
    QString logFileFolder("c:\\data\\cxtests\\"); // Must end in backslash
#else
    QString logFileFolder("e:\\cxtests\\"); // Must end in backslash
#endif

    QStringList tests;

    tests << "unittest_cxevideocapturecontrolsymbian"
          << "unittest_cxestillimagesymbian"
          << "unittest_cxequalitypresetssymbian"
          << "unittest_cxecameradevicecontrolsymbian"
          << "unittest_cxestatemachine"
          << "unittest_cxestate"
          << "unittest_cxefilenamegeneratorsymbian"
          << "unittest_cxeautofocuscontrolsymbian"
          << "unittest_cxeviewfindercontrolsymbian"
          << "unittest_cxetestutils"
          << "unittest_cxesettmappersymbian"
          << "unittest_cxecameradevice"
          << "unittest_cxeimagedataitemsymbian"
          << "unittest_cxeimagedataqueuesymbian"
          << "unittest_cxeerrormappersymbian"
          << "unittest_cxefilesavethreadsymbian"
          << "unittest_cxesettingscenrepstore"
          << "unittest_cxezoomcontrolsymbian"
          << "unittest_cxestillcapturecontrolsymbian"
          << "unittest_cxefeaturemanagerimp"
          << "unittest_cxesettingsimp"
          << "unittest_cxescenemodestore"
          << "unittest_cxethumbnailmanagersymbian"
          << "unittest_cxeharvestercontrolsymbian"
          << "unittest_cxesettingscontrolsymbian"
          << "unittest_cxesnapshotcontrol"
          << "unittest_cxeenginesymbian"
          << "unittest_cxegeotaggingtrail"
          << "unittest_cxememorymonitor"
          << "unittest_cxediskmonitor";

    QDir dir;
    dir.mkpath(logFileFolder);

    // Delete old log files
    foreach(const QString &test, tests) {
        dir.remove(logFileFolder + test + ".log");
    }
    dir.remove(logFileFolder + "results.txt");

    // Run all tests sequentially
    foreach(const QString &test, tests) {
        QProcess p;
        QString command = test + ".exe";
        QStringList args;
        args << "-o" << (logFileFolder + test + ".log");
        qDebug() << "***** Launching" << command << "*****";
        p.start(command, args, QProcess::ReadOnly);

        p.waitForStarted();
        qDebug() << "***** started *****";
        QThread::yieldCurrentThread();
        QCoreApplication::processEvents();
        p.waitForFinished();
        qDebug() << "*****" << command << "completed, exit code" << p.exitCode() << "*****";

        parseLogFile(logFileFolder + test + ".log");
    }

    QFile results(logFileFolder + "results.txt");
    if (results.open(QIODevice::WriteOnly | QIODevice::Text)) {
        foreach(const QByteArray &line, mResults) {
            results.write(line);
            results.write("\n");
        }
    } else {
        qWarning() << "Cannot write results!";
    }

    QCoreApplication::quit();
}

void CxeTestRunner::parseLogFile(const QString& filename)
{
    QFile log(filename);

    if (!log.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open log file" << filename << "for reading!";
        return;
    }

    mResults.append(filename.toAscii());

    while (!log.atEnd()) {
        QByteArray line = log.readLine();
        if (line.startsWith("Totals:") ||
            line.startsWith("FAIL!")) {
            line = line.replace("\n", "");
            mResults.append("    " + line);
        }
    }

    mResults.append(""); // add empty line to output
}
