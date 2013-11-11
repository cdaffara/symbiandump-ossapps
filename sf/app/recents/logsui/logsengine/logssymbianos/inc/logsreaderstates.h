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

#ifndef LOGSREADERSTATES_H
#define LOGSREADERSTATES_H

//  INCLUDES
#include "logsstatebase.h"
#include <e32std.h>
#include <logclientchangeobserver.h>
#include <logviewchangeobserver.h>
#include "logsduplicatelookup.h"
#include "logsreaderstatecontext.h"

// FORWARD DECLARATION
class LogsReaderStateContext;
class LogsEvent;
class CLogViewEvent;
class CLogFilterList;
class CLogFilter;
class CLogEvent;

// CLASS DECLARATION

/**
 * Reader state base
 */
class LogsReaderStateBase : public LogsStateBase {

    friend class UT_LogsReaderStates;
    friend class UT_LogsReader;
    
    public:
        
        /**
         * Destructor
         */
        virtual ~LogsReaderStateBase();
    
    protected:
        
        /**
         * Constructor
         */
        LogsReaderStateBase(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        
        /**
         * Fill dest event with source event data and insert to specified
         * place in events list.
         * @param source event 
         * @param dest event, ownership is transferred
         * @param index, increased if event was added
         * @param events, event is possibly added to this list
         * @return true if dest event was inserted, false if discarded and deleted
         */
        bool constructAndInsertEventL(
                const CLogEvent& source, LogsEvent* dest, int &eventIndex, QList<LogsEvent*>& events );
        
        /**
         * Fill already used dest event with source event data and insert
         * to specified place in events list.
         * @param source event 
         * @param dest event, ownership is transferred
         * @param index, increased if event was added
         * @return true if dest event was inserted, false if discarded and deleted
         */
        bool updateAndInsertEventL(
                const CLogEvent& source, LogsEvent* dest, int &eventIndex );
        
        /**
         * Reset current events
         */
        void resetEvents();
        
        /**
         * Try to find matching event and remove it from
         * events list.
         * @param event, symbian log event
         * @return LogsEvent or 0 if no matching found, ownership is transferred
         */
        LogsEvent* takeMatchingEvent(const CLogEvent& event);
        
        /**
         * Finds specified event from the event list. Event not removed from the list.
         * @param eventId, id of the event to be found
         * @return pointer to event or 0 if not found
         */
        LogsEvent* eventById(int eventId);

    protected:   
        LogsReaderStateContext& mContext;
};

/**
 * Initialize reading state
 */
class LogsReaderStateInitReading : public LogsReaderStateBase {

     friend class UT_LogsReaderStates;
     
    public:
     LogsReaderStateInitReading(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        virtual ~LogsReaderStateInitReading(){}

    public: // From LogsReaderStateBase
        virtual bool enterL();
 };

/**
 * Filtering recent state
 */
class LogsReaderStateFiltering : public LogsReaderStateBase {

     friend class UT_LogsReaderStates;
     
    public:
        LogsReaderStateFiltering(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        virtual ~LogsReaderStateFiltering();

    public: // From LogsReaderStateBase
        virtual bool enterL();
        virtual bool continueL();
    
    protected:
        /**
         * Setting filter for view, derived class may add special filtering
         * at this stage.
         */
        virtual bool setFilterL(CLogFilterList& filterList);
        
    protected:
        CLogFilterList* mFilterList;
 };

/**
 * Filtering all state
 */
class LogsReaderStateFilteringAll : public LogsReaderStateFiltering {

     friend class UT_LogsReaderStates;
     
    public:
        LogsReaderStateFilteringAll(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        virtual ~LogsReaderStateFilteringAll();

    protected: // From LogsReaderStateFiltering
        virtual bool setFilterL(CLogFilterList& filterList);
 };

/**
 * Reading state
 */
class LogsReaderStateReading : public LogsReaderStateBase {

    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateReading(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        virtual ~LogsReaderStateReading();

    public: // From LogsReaderStateBase
        virtual bool enterL();
        virtual bool continueL();
    
    protected:
        bool handleMissedL(LogsEvent& parsedEvent);
        void updateReadSizeCounter(LogsEvent& event);
        bool canContinueReadingL(int index) const;     
        
    protected:
        CLogFilter* mDuplicateMissedFilter;
        bool mCheckingMissed;
        int mEventIndex;
        int mReadSizeCounter;
};

/**
 * Fill missing information state
 */
class LogsReaderStateFillDetails : public LogsReaderStateBase {
    public:
        LogsReaderStateFillDetails(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        virtual ~LogsReaderStateFillDetails();
        
        /**
         * Synchronously fills details from phonebook
         */
        void fillDetails(); 

    public: // From LogsReaderStateBase
        virtual bool enterL();
    private:
        void mergeDuplicates( LogsEvent& usedEvent, LogsEvent& discardedEvent ) const;
        void searchMatchForNumber( QHash<QString, ContactCacheEntry>& contactMappings, 
                                   QSet<QString>& numbersWithoutMatch, 
                                   LogsEvent& event, 
                                   const QString& num);
        
    private:
        LogsDuplicateLookup mDuplicateLookup;
        
    private: // For testing
        
        friend class UT_LogsReaderStates;
};

/**
 * Completed state
 */
class LogsReaderStateDone : public LogsReaderStateBase {
    public:
        LogsReaderStateDone(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        virtual ~LogsReaderStateDone();
        
    public: // From LogsReaderStateBase
        virtual bool enterL();
};

/**
 * Finding duplicate events state
 */
class LogsReaderStateFindingDuplicates : public LogsReaderStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateFindingDuplicates(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        virtual ~LogsReaderStateFindingDuplicates();

    public: // From LogsReaderStateBase
        virtual bool enterL();  
        virtual bool continueL();
        
    protected:
        CLogFilter* mDuplicateFilter;
};

/**
 * Marking duplicate events state
 */
class LogsReaderStateMarkingDuplicates : public LogsReaderStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateMarkingDuplicates(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        virtual ~LogsReaderStateMarkingDuplicates(){}

    public: // From LogsReaderStateBase
        virtual bool enterL();
        virtual bool continueL(); 
        
    protected:
        bool mGettingDuplicates;
};

/**
 * Reading duplicate events state
 */
class LogsReaderStateReadingDuplicates : public LogsReaderStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateReadingDuplicates(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        virtual ~LogsReaderStateReadingDuplicates(){}

    public: // From LogsReaderStateBase
        virtual bool enterL();   
        virtual bool continueL();
};

/**
 * Merging duplicate events state
 */
class LogsReaderStateMergingDuplicates : public LogsReaderStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
    LogsReaderStateMergingDuplicates(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        virtual ~LogsReaderStateMergingDuplicates(){}

    public: // From LogsReaderStateBase
        virtual bool enterL();   
};

/**
 * Modifying done state
 */
class LogsReaderStateModifyingDone : public LogsReaderStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateModifyingDone(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        virtual ~LogsReaderStateModifyingDone(){}

    public: // From LogsReaderStateBase
        virtual bool enterL();
};

/**
 * Reading duplicates done state
 */
class LogsReaderStateReadingDuplicatesDone : public LogsReaderStateBase 
{
    friend class UT_LogsReaderStates;
    
    public:
        LogsReaderStateReadingDuplicatesDone(LogsStateBaseContext& context, LogsReaderStateContext& readerContext);
        virtual ~LogsReaderStateReadingDuplicatesDone(){}

    public: // From LogsReaderStateBase
        virtual bool enterL();
};



#endif      // LOGSREADERSTATES_H


// End of File
      

        
       
