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
#ifndef UT_LOGSREADERSTATES_H
#define UT_LOGSREADERSTATES_H

#include <QObject>
#include <QList>
#include <e32base.h>
#include "logsengdefs.h"
#include "logsstatebasecontext.h"
#include "logsreaderstatecontext.h"
#include "logsreaderobserver.h"

class CLogClient;
class LogsEvent;
class CLogView;
class CLogViewRecent;
class CLogViewEvent;
class LogsReaderStateBase;
class CLogViewDuplicate;

class UT_LogsReaderStates : public QObject, 
                            public LogsStateBaseContext, 
                            public LogsReaderStateContext, 
                            public LogsReaderObserver              
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
     
    void testStateBase();
    void testStateInitReading();
    void testStateFiltering();
    void testStateFilteringAll();
    void testStateReading();
    void testStateReading2();
    void testStateFillDetails();
    void testStateFillDetails2();
    void testStateDone();
    void testStateSearchingEvent();
    void testStateFindingDuplicates();
    void testStateMarkingDuplicates();
    void testStateReadingDuplicates();
    void testStateMergingDuplicates();
    void testStateReadingDuplicatesDone();
    void testStateModifyingDone();

protected: // From LogsStateBaseContext
    
      void setCurrentState(const LogsStateBase& state);
      CLogView& logView();
      CLogViewDuplicate& duplicatesView();
      int& index();
      TRequestStatus& reqStatus();
      int currentEventId();
      CLogClient& logClient();
      bool isRecentView();
      
protected: // From LogsReaderStateContext
      
      QList<LogsEvent*>& events();
      LogsEventStrings& strings();
      LogsReaderObserver& observer();
      QHash<QString, ContactCacheEntry>& contactCache();
      QList<LogsEvent*>& duplicatedEvents();
      
protected: // From LogsReaderObserver
    
      void readCompleted();
      void errorOccurred(int err);
      void temporaryErrorOccurred(int err);
      void eventModifyingCompleted();
      void duplicatesReadingCompleted(QList<LogsEvent*> duplicates);

          
private:
    
    void reset();

    
private:

      CLogClient* mLogClient;
      const LogsStateBase* mCurrentState;
      CLogViewRecent* mLogView;
      CLogViewEvent* mLogViewEvent;
      CLogViewDuplicate* mDuplicatesView;
      QList<LogsEvent*> mEvents;
      int mIndex;
      LogsEventStrings mStrings;
      TRequestStatus mReqStatus;
      bool mIsRecentView;
      
      int mError;
      int mTemporaryError;
      bool mReadCompleted;
      bool mModifyCompleted;
      int mReadCount;
      int mDuplicatesReadingCompletedCount;
      
      QHash<QString, ContactCacheEntry> mContactCache;
      
      int mCurrentEventId;
      QList<LogsEvent*> mDuplicatedEvents;
};


#endif //UT_LOGSREADERSTATES_H
