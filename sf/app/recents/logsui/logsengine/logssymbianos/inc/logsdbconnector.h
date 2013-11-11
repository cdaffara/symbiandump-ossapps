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

#ifndef LOGSDBCONNECTOR_H
#define LOGSDBCONNECTOR_H

//  INCLUDES
#include <QObject>
#include "logsengdefs.h"
#include "logsreaderobserver.h"
#include "logsremoveobserver.h"
#include "logsmodel.h"
// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CLogClient;
class LogsReader;
class LogsEvent;
class LogsRemove;
class RFs;

// CLASS DECLARATIONS


/**
 * LogsDbConnector is used to initialize database access
 */
class LogsDbConnector :  
    public QObject, public LogsReaderObserver, public LogsRemoveObserver
    {
    Q_OBJECT
    
    public:

        /**
         * Constructor
         * @param events, connector fills the array with events
         *  read from database. Ownership of events is not transferred.
         * @param checkAllEvents, true if interested in all events of the database,
         *  false if interested only in recent events
         */
        LogsDbConnector( QList<LogsEvent*>& events, bool checkAllEvents = false, bool resourceControl = false );
        ~LogsDbConnector();
        
    signals:
    
        /**
         * Signaled once some new data has been read from database.
         * Added data is placed to event array already when this signal
         * is emitted.
         * @param addedIndexes, indexes for new events
         */
        void dataAdded(QList<int> addedIndexes);   
        
        /**
         * Signaled once some data has been updated.
         * Corresponding events have been already updated at event array
         * when this signal is emitted.
         * @param updatedIndexes, indexes for updated events
         */
        void dataUpdated(QList<int> updatedIndexes);
        
        /**
         * Signaled once some data has been removed.
         * Corresponding events have been already removed from event array
         * when this signal is emitted.
         * @param removedIndexes, indexes for removed events
         */
        void dataRemoved(QList<int> removedIndexes);
        
        /**
         * Signaled once significant changes in data has occured.
         * Corresponding changes have been already done to event array
         * when this signal is emitted.
         */
        void dataReset();
        
        /**
         * Signaled once asycn event clearing has completed.
         * @param err, 0 if marking completed succesfully
         */
        void clearingCompleted(int err);
        
        /**
         * Signaled once asycn event marking has completed.
         * @param err, 0 if marking completed succesfully
         */
        void markingCompleted(int err);
        
        /**
         * Signaled once duplicate reading has completed.
         */
        void duplicatesRead();

    public:
        
        /**
         * Initialize database access
         * @return 0 if initialized succesfully
         */
		int init();
		
		/**
         * Start reading data from database
         * @return 0 if reading started succesfully
         */
		int start();
		
        /**
		 * Synchronously updates details (rematches from phonebook) for 
         * already read events
         * @param clearCached if true, cached matches will be cleaned before
         * rematching
         */
        int updateDetails(bool clearCached);
		
        /**
         * Clear events of defined type. Clearing is async and completion is
         * indicated by clearingCompleted signal.
         * @return true if async clearing started 
         */
		bool clearList(LogsModel::ClearType cleartype);
		
        /**
         * Starts removing events and all their duplicates. Clearing
         * can be sync or async. In case of async, completion is
         * indicated by clearingCompleted signal.
         * @param events, events to be removed, events are removed only from
         *  database, passed event objects themselves are not touched.
         * @return true if async clearing started
         */
        bool clearEvents(const QList<LogsEvent*>& events);
        
        /**
         * Mark events as seen. Completion is indicated by
         * markingCompleted signal.
         * @param events, events to be marked
         * @return true if marking started
         */
        bool markEventsSeen(const QList<LogsEvent*>& events);
        
        /**
         * Read duplicates for specified event
         * @param eventId
         * @return 0 if reading started succesfully
         */
        int readDuplicates(int eventId);
        
        /**
         * Take current duplicates.
         * @return list of duplicate events, ownership is transferred
         */
        QList<LogsEvent*> takeDuplicates();
        
        int refreshData();
        int compressData();
        
        
    protected: // From LogsReaderObserver
        
        virtual void readCompleted();
        virtual void errorOccurred(int err);
		virtual void temporaryErrorOccurred(int err);
		virtual void eventModifyingCompleted();
		virtual void duplicatesReadingCompleted(QList<LogsEvent*> duplicates);
		
	protected: // From LogsRemoveObserver
		virtual void removeCompleted();
		virtual void logsRemoveErrorOccured(int err);
        
    private:
		void initL();
		void handleTemporaryError(int& error);
		void deleteInvalidEvents(int newEventCount);
		int doMarkEventSeen();
		bool handleModifyingCompletion(int err=0);
		void releaseDbConnections();

    private: // data

        QList<LogsEvent*>& mModelEvents;
        bool mCheckAllEvents;
        bool mResourceControl;
        CLogClient* mLogClient;
        LogsEventStrings mLogEventStrings;
        RFs* mFsSession;
        LogsReader* mReader;
        LogsRemove* mLogsRemove;
        bool mCompressionEnabled;
        bool mFirstReadCompleted;
                
        QList<LogsEvent*> mEvents;
        QList<LogsEvent*> mDuplicatedEvents;
        QList<int> mRemovedEventIndexes;
        QList<int> mUpdatedEventIndexes;
        QList<int> mAddedEventIndexes;
        QList<LogsEvent> mEventsSeen;
        
    private: // Testing related friend definitions
        
        friend class UT_LogsDbConnector;
        friend class UT_LogsRemove;
        friend class UT_LogsModel;
        friend class UT_LogsDetailsModel;
    
    };

            
#endif

// End of File  LOGSDBCONNECTOR_H
