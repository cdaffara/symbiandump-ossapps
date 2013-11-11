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
#ifndef UT_LOGSREMOVESTATES_H
#define UT_LOGSREMOVESTATES_H

#include <QObject>
#include <QList>
#include <e32base.h>
#include "logsengdefs.h"
#include "logsstatebasecontext.h"
#include "logsremovestatecontext.h"
#include "logsremoveobserver.h"
#include "logsevent.h"

class CLogClient;
class LogsEvent;
class CLogViewRecent;
class CLogViewEvent;
class LogsReaderStateBase;

class UT_LogsRemoveStates : public QObject, 
                            public LogsStateBaseContext, 
                            public LogsRemoveStateContext, 
                            public LogsRemoveObserver              
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

    void testClearAll();
    void testInit();
    void testDelete();
    void testDeleteDuplicates();
    void testDone();

protected: // From LogsStateBaseContext
    
    void setCurrentState(const LogsStateBase& state);
    CLogView& logView();
    CLogViewDuplicate& duplicatesView();
    int& index();
    TRequestStatus& reqStatus();
    int currentEventId();
    CLogClient& logClient();
    bool isRecentView();

protected: // From LogsRemoveStateContext
    
    LogsRemoveObserver& observer();
    QList<LogsEvent>& removedEvents(); 
    int clearType();

protected: // From LogsRemoveObserver
    
    void removeCompleted();
    void logsRemoveErrorOccured(int err);
          
private:
    
    void reset();

    
private:

    CLogClient* mLogClient;
    const LogsStateBase* mCurrentState;
    CLogViewRecent* mLogView;
    CLogViewEvent* mLogViewEvent;
    CLogViewDuplicate* mDuplicatesView;
    int mIndex;
    TRequestStatus mReqStatus;
    bool mIsRecentView;
    
    int mError;
    bool mRemoveCompleted;
    
    int mCurrentEventId;
    QList<LogsEvent> mRemovedEvents;
    int mClearType;
};


#endif //UT_LOGSREMOVESTATES_H
