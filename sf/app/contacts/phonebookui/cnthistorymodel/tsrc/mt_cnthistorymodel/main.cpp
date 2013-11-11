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

#include "testrunner.h"

#include "mt_cnthistorymodel.h"

#include <QtTest/QtTest>

int main(int argc, char *argv[]) 
{
    bool promptOnExit(true);
    for (int i=0; i<argc; i++) {
        if (QString(argv[i]) == "-noprompt")
            promptOnExit = false;
    }
    printf("Running tests...\n");
            
    QApplication app(argc, argv);
    
    QTranslator translator;
    QString lang = QLocale::system().name();
    QString path = "z:/resource/qt/translations/";
    translator.load(path + "contacts_" + lang);
    app.installTranslator(&translator);
    
    TestRunner testRunner("mt_cnthistorymodel");

    TestCntHistoryModel historyModelTests;
    testRunner.runTests( historyModelTests );
    
    testRunner.printResults();

    /*if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }*/
    return 0;   
}

