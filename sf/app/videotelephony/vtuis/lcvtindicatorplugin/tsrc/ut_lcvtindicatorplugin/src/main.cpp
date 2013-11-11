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

#include "ut_lcvtindicatorplugin.h"
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

    UT_LcVtIndicatorPlugin ut_lcvtindicatorplugin;
    QString resultFileName = "c:/ut_logs_ut_lcvtindicatorplugin.xml";
    QStringList args_ut_lcvtindicatorplugin( "ut_ut_lcvtindicatorplugin");
    args_ut_lcvtindicatorplugin << "-xml" << "-o" << resultFileName;
    QTest::qExec(&ut_lcvtindicatorplugin, args_ut_lcvtindicatorplugin);
    parser.parseAndPrintResults(resultFileName,true); 


    if (promptOnExit) {
        printf("Press any key...\n");
        getchar(); 
    }
    return 0;   
}



 
