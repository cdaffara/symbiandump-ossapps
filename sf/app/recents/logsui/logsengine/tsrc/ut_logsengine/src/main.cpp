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

#include "ut_logsmodel.h"
#include "ut_logsdetailsmodel.h"
#include "ut_logsmatchesmodel.h"
#include "ut_logsfilter.h"
#include "ut_logscustomfilter.h"
#include "ut_logscall.h"
#include "ut_logscontact.h"
#include "ut_logsmessage.h"
#include "ut_logsevent.h"
#include "ut_logseventdata.h"
#include "ut_logsthumbnailmanager.h"
#include "ut_logscommondata.h"
#include "ut_logsconfigurationparams.h"
#include "ut_logsduplicatelookup.h"
#include "testrunner.h"


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
    TestRunner testRunner("LogsEngine", combinedOutputFileName);
    
    UT_LogsModel ut_logsModel;
    testRunner.runTests(ut_logsModel);
    
    UT_LogsDetailsModel ut_logsDetailsModel;
    testRunner.runTests(ut_logsDetailsModel);
    
    UT_LogsMatchesModel ut_logsMatchesModel;
    testRunner.runTests(ut_logsMatchesModel);
    
    UT_LogsThumbnailManager ut_logsThumbnailManager;
    testRunner.runTests(ut_logsThumbnailManager);

    UT_LogsFilter ut_logsFilter;
    testRunner.runTests(ut_logsFilter);
    
    UT_LogsCustomFilter ut_logsCustomFilter;
    testRunner.runTests(ut_logsCustomFilter);

    UT_LogsCall ut_logscall;
    testRunner.runTests(ut_logscall);
    
    UT_LogsContact ut_logsContact;
    testRunner.runTests(ut_logsContact);
    
    UT_LogsMessage ut_logsMessage;
    testRunner.runTests(ut_logsMessage);
 
    UT_LogsEvent ut_logsEvent;
    testRunner.runTests(ut_logsEvent);
    
    UT_LogsEventData ut_logsEventdata;
    testRunner.runTests(ut_logsEventdata);
    
    UT_LogsCommonData ut_logsCommonData;
    testRunner.runTests(ut_logsCommonData);
    
    UT_LogsConfigurationParams ut_logsConfigurationParams;
    testRunner.runTests(ut_logsConfigurationParams);
    
    UT_LogsDuplicateLookup ut_logsLookup;
    testRunner.runTests(ut_logsLookup);
    
    testRunner.printResults();
    
    return 0;   
}



 
