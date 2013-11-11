/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for presence cache reader and writer.
*
*/
#include "entitytests.h"
#include <qtest.h>
#include <stdio.h>

#include <qdebug.h>
#include <qsqldatabase.h>
#include <qfile.h>


//QTEST_MAIN(TestOfTest);
int main(int argc, char *argv[])
{
    bool promptOnExit(0);
    for (int i=0; i<argc; i++) {
        if (QString(argv[i]) == "-noprompt")
            promptOnExit = false;
    }
    printf("Running tests...\n");

    QApplication app(argc, argv);

    QString entityTestsFileName = "c:/testPresence_qt.xml";
   
    QStringList args( "entitytests");
    args << "-xml" << "-o" << entityTestsFileName;
    EntityTests et;
    QTest::qExec(&et, args);

    if (promptOnExit) {
        printf("Press any key...\n");
        getchar();
    }
    return 0;
}
