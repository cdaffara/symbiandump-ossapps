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
#include "ut_logsmainwindow.h"
#include "ut_logsapplication.h"
#include "ut_logscomponentrepository.h"
#include "ut_logsviewmanager.h"
#include "ut_logsbaseview.h"
#include "ut_logsrecentcallsview.h"
#include "ut_logsdetailsview.h"
#include "ut_logsmatchesview.h"
#include "ut_logseffecthandler.h"
#include "ut_logsservicehandler.h"
#include "ut_logspageindicator.h"
#include "ut_logspageindicatoritem.h"
#include "ut_logsappsettings.h"
#include "testrunner.h"

#include <QtTest/QtTest>

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
    
    TestRunner testRunner("LogsApp", combinedOutputFileName);
    
    UT_LogsMainWindow ut_logsMainWindow;
    testRunner.runTests(ut_logsMainWindow);
    
    UT_LogsComponentRepository ut_logsRepository;
    testRunner.runTests(ut_logsRepository);
    
    UT_LogsViewManager ut_logsViewManager;
    testRunner.runTests(ut_logsViewManager);
    
    UT_LogsBaseView ut_logsBaseView;
    testRunner.runTests(ut_logsBaseView);
    
    UT_LogsRecentCallsView ut_logsRecentCallsView;
    testRunner.runTests(ut_logsRecentCallsView);
    
    UT_LogsDetailsView ut_logsDetailsView;
    testRunner.runTests(ut_logsDetailsView);
    
    UT_LogsMatchesView ut_logsMatchesView;
    testRunner.runTests(ut_logsMatchesView);
    
    UT_LogsEffectHandler ut_logsEffectHandler;
    testRunner.runTests(ut_logsEffectHandler);
    
    UT_LogsServiceHandler ut_logsServiceHandler;
    testRunner.runTests(ut_logsServiceHandler);
    
    UT_LogsPageIndicator ut_logsPageIndicator;
    testRunner.runTests(ut_logsPageIndicator);
    
    UT_LogsPageIndicatorItem ut_logsPageIndicatorItem;
    testRunner.runTests(ut_logsPageIndicatorItem);
    
    UT_LogsAppSettings ut_logsAppSettings;
    testRunner.runTests(ut_logsAppSettings);
        
    //qApp is deleted after this testcase is run
    //keep this testcase last
    UT_LogsApplication ut_logsApplication;
    testRunner.runTests(ut_logsApplication);
    
    testRunner.printResults();
    
    return 0;   
}

