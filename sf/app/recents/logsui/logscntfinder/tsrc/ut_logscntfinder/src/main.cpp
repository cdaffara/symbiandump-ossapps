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

#include "ut_logscntfinder.h"
#include "ut_logspredictivetranslator.h"
#include "ut_logspredictive12keytranslator.h"
#include "ut_logspredictivelatin12keytranslator.h"
#include "ut_logspredictivethai12keytranslator.h"
#include "ut_logscntentry.h"

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
    
    TestRunner testRunner("LogsCntFinder", combinedOutputFileName);
    
    UT_LogsPredictiveTranslator ut_logsPredictiveTranslator;
    testRunner.runTests(ut_logsPredictiveTranslator);
    
    UT_LogsPredictive12KeyTranslator ut_logsPredictive12KeyTranslator;
    testRunner.runTests(ut_logsPredictive12KeyTranslator);
    
    UT_LogsPredictiveLatin12KeyTranslator ut_logsPredictiveLatin12KeyTranslator;
    testRunner.runTests(ut_logsPredictiveLatin12KeyTranslator);
    
    UT_LogsPredictiveThai12KeyTranslator ut_logsPredictiveThai12KeyTranslator;
    testRunner.runTests(ut_logsPredictiveThai12KeyTranslator);
    
    UT_LogsCntEntry ut_logsCntEntry;
    testRunner.runTests(ut_logsCntEntry);
    
    UT_LogsCntFinder ut_logsCntFinder;
    testRunner.runTests(ut_logsCntFinder);
    
    testRunner.printResults();
    
    return 0;   
}



 
