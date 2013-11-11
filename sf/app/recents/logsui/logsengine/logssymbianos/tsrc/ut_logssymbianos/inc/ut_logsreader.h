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
#ifndef UT_LOGSREADER_H
#define UT_LOGSREADER_H

#include <QObject>
#include <f32file.h>
#include "logsengdefs.h"
#include "logsreaderobserver.h"

class LogsDbConnector;
class LogsEvent;
class CLogClient;
class LogsReader;

class UT_LogsReader : public QObject, public LogsReaderObserver             
{
     Q_OBJECT
    
private slots:

/*
 * In addition, there are four private slots that are not treated as testfunctions. 
 * They will be executed by the testing framework and can be used to initialize and clean up 
 * either the entire test or the current test function.
 * 
 * initTestCase() will be called before the first testfunction is executed.
 * cleanupTestCase() will be called after the last testfunction was executed.
 * init() will be called before each testfunction is executed.
 * cleanup() will be called after every testfunction.
*/
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots: //test methods
     
    void testConstructor();
    void testStart();
    void testStart2();
    void testStop();
    void testMarkEventSeen();
    void testReadDuplicates();
    void testRunL();
    void testRunError();
    void testStateContext();
    void testViewChange();
    void testUpdateDetails();
    void testLock();

protected: // From LogsReaderObserver
    
    void readCompleted();
    void errorOccurred(int err);
    void temporaryErrorOccurred(int err);
    void eventModifyingCompleted();
    void duplicatesReadingCompleted(QList<LogsEvent*> duplicates);
        
private:
  
    LogsReader* mReader;
    
    bool mErrorOccurred;
    int mError;
    
    QList<LogsEvent*> mEvents;
    RFs mFs;
    CLogClient* mLogClient;
    LogsEventStrings mStrings;

};


#endif //UT_LOGSREADER_H
