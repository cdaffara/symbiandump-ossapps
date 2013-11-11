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

#ifndef LOGSREADER_H
#define LOGSREADER_H

//  INCLUDES
#include <QList>
#include <QHash>
#include <e32base.h>
#include <logclientchangeobserver.h>
#include <logviewchangeobserver.h>
#include "logsworker.h"
#include "logsreaderstatecontext.h"
#include "logsreaderstates.h"

// FORWARD DECLARATION
class CLogViewRecent;
class RFs;
class CLogClient;
class CLogViewEvent;
class LogsReaderObserver;
class LogsEvent;
class LogsEventStrings;
class LogsReaderStateFiltering;

// CLASS DECLARATION



/**
 * LogsReader is used to read events from database
 */
class LogsReader : public LogsWorker,
                   public MLogClientChangeObserver,
                   public MLogViewChangeObserver,
                   public LogsReaderStateContext
    {

        friend class UT_LogsReader;
        friend class UT_LogsDbConnector;

    public:
        
        /**
         *  Constructor
         *  @param fsSession, connected fileserver session
         *  @param logClient
         *  @param strings, commonly used strings
         *  @param events, filled with events once reading progresses
         *  @param observer, information about read progress is notified
         *      via the interface
         *  @param readAllEvents, true if all events in db should be read,
         *      otherwise only recent events are read
         */
        LogsReader( RFs& fsSession,
                    CLogClient& logClient,
                    LogsEventStrings& strings,
                    QList<LogsEvent*>& events,
                    LogsReaderObserver& observer,
                    bool readAllEvents = false );
  
       /**
        *   Destructor.
        */
        virtual ~LogsReader();
        
        

    public:

        /**
         * Start reading
         * @return 0 if reading started succesfully
         */
        int start();
        
        /**
         * Stop reading
         */
        void stop();
        
        /**
         * Synchronously update details (rematches from phonebook) for 
         * already read events
         * @param clearCached if true, cached matches will be cleaned before
         * rematching
         */
        void updateDetails(bool clearCached);
                
        /**
         * Starts modifying the event and all its duplicates as read
         * @param eventId, id of the event to be removed
         * @return 0 if removing started succesfully
         */
        int markEventSeen(int eventId);
        
        /**
         * Starts reading duplicates of the event
         * @param eventId
         * @return 0 if removing started succesfully
         */
        int readDuplicates(int eventId);

    public: // From LogsWorker
        
        virtual int lock(bool locked);
        
    protected: // From CActive

        TInt RunError(TInt error);

    private: // From MLogClientChangeObserver

        void HandleLogClientChangeEventL( TUid aChangeType, 
                                          TInt aChangeParam1, 
                                          TInt aChangeParam2, 
                                          TInt aChangeParam3 );
    private: // From MLogViewChangeObserver
        
        void HandleLogViewChangeEventAddedL( 
                TLogId aId, TInt aViewIndex, 
                TInt aChangeIndex, TInt aTotalChangeCount);

        void HandleLogViewChangeEventChangedL(   
                TLogId aId, TInt aViewIndex, 
                TInt aChangeIndex, TInt aTotalChangeCount);

        void HandleLogViewChangeEventDeletedL( 
                TLogId aId, TInt aViewIndex, 
                TInt aChangeIndex, TInt aTotalChangeCount);
    
    private: // From LogsReaderStateContext

        inline QList<LogsEvent*>& events();
        inline LogsEventStrings& strings();
        inline LogsReaderObserver& observer();
        inline QHash<QString, ContactCacheEntry>& contactCache();
        inline QList<LogsEvent*>& duplicatedEvents();
        
    private:

        void startL();
        void markEventSeenL(int eventId);
        void readDuplicatesL(int eventId);
        
        /**
         * Cancel all outstanding requests if possible. In case of deleting is
         * in progress, it will leave with error
         */
        void cancelCurrentRequestL();
        void initializeReadStates();
        void initializeModifyingStates();
        void initializeDuplicateReadingStates();
        void handleViewChange( int totalChangeCount = 1 );
        void handleError(int error);
        void createLogViewsL();
        void deleteLogViews();
        LogsReaderStateFiltering* createFilteringState();
        void prepareReadingL();
        void setGlobalObserver();
        void clearGlobalObserver();
        void cleanContactCache();
        
    private: // data
        
        RFs& mFsSession;
        LogsEventStrings& mStrings;
        QList<LogsEvent*>& mEvents;
        LogsReaderObserver& mObserver;

        QList<LogsStateBase*> mReadStates;
        QList<LogsStateBase*> mModifyingStates;
        QList<LogsStateBase*> mDuplicateReadingStates;
    
        QHash<QString, ContactCacheEntry> mContactCache;
        QList<LogsEvent*> mDuplicatedEvents;

        bool mGlobalObserverSet;
        bool mPendingRead;
    };

#endif      // LOGSREADER_H


// End of File
      

        
       
