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


#include <QtTest/QtTest>

#include "at_logsengine.h"
#include "testresultxmlparser.h"


int main(int argc, char *argv[]) 
{
    bool promptOnExit(true);
    for (int i=0; i<argc; i++) {
        if (QString(argv[i]) == "-noprompt")
            promptOnExit = false;
    }
    printf("Running tests...\n");
    
    QApplication app(argc, argv);
    TestResultXmlParser parser;
    
    AT_LogsEngine test;
    QString resultFileName = "c:/at_logs_logsEngine.xml";
    QStringList args_test( "at_logsEngine");
    args_test << "-xml" << "-o" << resultFileName;
    QTest::qExec(&test, args_test);
    parser.parseAndPrintResults(resultFileName,true); 

    if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }
    return 0;   
}



 
