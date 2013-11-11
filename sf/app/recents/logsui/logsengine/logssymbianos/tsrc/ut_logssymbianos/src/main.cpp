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

#include "ut_logsdbconnector.h"
#include "ut_logsreader.h"
#include "ut_logsreaderstates.h"
#include "ut_logseventparser.h"
#include "ut_logseventdataparser.h"
#include "testrunner.h"
#include "ut_logsremove.h"
#include "ut_logsremovestates.h"
#include "ut_logsforegroundwatcher.h"


int main(int argc, char *argv[]) 
{
    printf("Running tests...\n");
            
    QApplication app(argc, argv);
    QStringList args = app.arguments();
    QString combinedOutputFileName;
    for ( int i = 0; i < args.count(); i++ ){
        QString arg = args.at(i);
        if ( arg == QString("-o") && i + 1 < args.count() ){
            i++;
            combinedOutputFileName = args.at(i);
        }
    }
    
    TestRunner testRunner("LogsSymbianOS", combinedOutputFileName);
    
    UT_LogsDbConnector ut_logsDbConnector;
    testRunner.runTests(ut_logsDbConnector);
   
	UT_LogsRemove ut_logsRemove;
    testRunner.runTests(ut_logsRemove);
    
    UT_LogsRemoveStates ut_logsRemoveStates;
    testRunner.runTests(ut_logsRemoveStates);
	
    UT_LogsReader ut_logsReader;
    testRunner.runTests(ut_logsReader);
    
    UT_LogsReaderStates ut_logsReaderStates;
    testRunner.runTests(ut_logsReaderStates);
    
    UT_LogsEventParser ut_logsEvent;
    testRunner.runTests(ut_logsEvent);
    
    UT_LogsEventDataParser ut_logsEventData;
    testRunner.runTests(ut_logsEventData);
    
    UT_LogsForegroundWatcher ut_logsForegroundWatcher;
    testRunner.runTests(ut_logsForegroundWatcher);
    
    testRunner.printResults();
    
    return 0;   
}



 
