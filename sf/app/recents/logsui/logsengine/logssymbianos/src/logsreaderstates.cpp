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

// INCLUDE FILES
#include <QList>
#include <logview.h>
#include <logwraplimits.h>
#include "logsreaderstates.h"
#include "logsstatebasecontext.h"
#include "logsevent.h"
#include "logseventdata.h"
#include "logsengdefs.h"
#include "logslogger.h"
#include "logsreaderobserver.h"
#include "logscommondata.h"

// CONSTANTS

// ----------------------------------------------------------------------------
// LogsReaderStateBase::LogsReaderStateBase
// ----------------------------------------------------------------------------
//
LogsReaderStateBase::LogsReaderStateBase(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext) 
 : LogsStateBase(context),
   mContext(readerContext)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::~LogsReaderStateBase
// ----------------------------------------------------------------------------
//
LogsReaderStateBase::~LogsReaderStateBase()
{

}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::updateAndInsertEventL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateBase::updateAndInsertEventL(
    const CLogEvent& source, LogsEvent* dest, int& eventIndex)
{
    Q_ASSERT( dest );
    dest->initializeEventL( source, mContext.strings() );
    mContext.events().insert(eventIndex, dest);
    eventIndex++;
    return true;
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::constructAndInsertEventL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateBase::constructAndInsertEventL(
    const CLogEvent& source, LogsEvent* dest, int& eventIndex, QList<LogsEvent*>& events)
{
    Q_ASSERT( dest );
    dest->initializeEventL( source, mContext.strings() );
    if ( !dest->validate() ){
        LOGS_QDEBUG( "LogsReaderStateBase::constructAndInsertEventL, event discarded" )
        delete dest;
        return false;
    } 
    events.insert(eventIndex, dest);
    eventIndex++;
    return true;
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::resetEvents
// ----------------------------------------------------------------------------
//
void LogsReaderStateBase::resetEvents()
{
    QList<LogsEvent*> &events = mContext.events();
    for ( int i = 0; i < events.count(); i++ ){
        events.at(i)->setIsInView(false);
    }
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::takeMatchingEvent
// ----------------------------------------------------------------------------
//
LogsEvent* LogsReaderStateBase::takeMatchingEvent(const CLogEvent& event)
{
    QList<LogsEvent*> &events = mContext.events();
    for ( int i = 0; i < events.count(); i++ ){
        if ( events.at(i)->logId() == event.Id() ){
            return events.takeAt(i);
        }
    }
    return 0;
}

// ----------------------------------------------------------------------------
// LogsReaderStateBase::eventById
// ----------------------------------------------------------------------------
//
LogsEvent* LogsReaderStateBase::eventById(int eventId)
{
    LogsEvent* event = 0;
    QList<LogsEvent*> &events = mContext.events();
    for ( int i = 0; i < events.count() && !event; i++ ){
        if ( events.at(i)->logId() == eventId ){
            event = events.at(i);
        }
    }
    return event;
}

// ----------------------------------------------------------------------------
// LogsReaderStateInitReading::LogsReaderStateInitReading
// ----------------------------------------------------------------------------
//
LogsReaderStateInitReading::LogsReaderStateInitReading(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext ) 
 : LogsReaderStateBase(context, readerContext)
{
}

// ----------------------------------------------------------------------------
// LogsReaderInitReadingState::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateInitReading::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateInitReading::enterL" );
    resetEvents();
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::LogsReaderStateFiltering
// ----------------------------------------------------------------------------
//
LogsReaderStateFiltering::LogsReaderStateFiltering(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext ) 
 : LogsReaderStateBase(context, readerContext),
   mFilterList(0)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::~LogsReaderStateFiltering
// ----------------------------------------------------------------------------
//
LogsReaderStateFiltering::~LogsReaderStateFiltering()
{
    if ( mFilterList ){
        mFilterList->ResetAndDestroy();
    }
    delete mFilterList;
}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFiltering::enterL()
{
    // Filtering all recent calls (max number of recent events is configurable,
    // see TLogConfig in logcli.h
    //
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateFiltering::enterL" );
    
    CLogFilterList* filterList = new ( ELeave ) CLogFilterList;
    CleanupStack::PushL(filterList);
    CLogFilter* filter = CLogFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetEventType( KLogCallEventTypeUid );
    filterList->AppendL( filter );
    CleanupStack::Pop( filter );
    
    if ( mFilterList ){
        mFilterList->ResetAndDestroy();
        delete mFilterList;
        mFilterList = 0;
    }
    mFilterList = filterList;
    CleanupStack::Pop(filterList);
    
    if ( setFilterL( *filterList ) ) {
        return true;
    }
    
    // Not possible to continue with filtering
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::continueL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFiltering::continueL()
{
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::setFilterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFiltering::setFilterL(CLogFilterList& filterList){
    __ASSERT_ALWAYS( mBaseContext.isRecentView(), User::Leave( KErrNotFound ) );
    return static_cast<CLogViewRecent&>( mBaseContext.logView() ).SetRecentListL( 
            KLogNullRecentList, filterList, mBaseContext.reqStatus() );
}
    
// ----------------------------------------------------------------------------
// LogsReaderStateFiltering::LogsReaderStateFiltering
// ----------------------------------------------------------------------------
//
LogsReaderStateFilteringAll::LogsReaderStateFilteringAll(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext ) 
 : LogsReaderStateFiltering(context, readerContext)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateFilteringAll::~LogsReaderStateFilteringAll
// ----------------------------------------------------------------------------
//
LogsReaderStateFilteringAll::~LogsReaderStateFilteringAll()
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateFilteringAll::setFilterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFilteringAll::setFilterL(CLogFilterList& filterList){
    __ASSERT_ALWAYS( !mBaseContext.isRecentView(), User::Leave( KErrNotFound ) );
    return static_cast<CLogViewEvent&>( mBaseContext.logView() ).SetFilterL( 
            filterList, mBaseContext.reqStatus() );
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::LogsReaderStateReading
// ----------------------------------------------------------------------------
//
LogsReaderStateReading::LogsReaderStateReading(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext ) 
 : LogsReaderStateBase(context, readerContext),
   mDuplicateMissedFilter(0),
   mCheckingMissed(false),
   mEventIndex(0),
   mReadSizeCounter(0)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::~LogsReaderStateReading
// ----------------------------------------------------------------------------
//
LogsReaderStateReading::~LogsReaderStateReading()
{
    delete mDuplicateMissedFilter;
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateReading::enterL()
{
    mCheckingMissed = false;
    mEventIndex = 0;
    mReadSizeCounter = 0;
    
    if ( !mDuplicateMissedFilter ){
        // Interested only about duplicates which are not marked as read
        mDuplicateMissedFilter = CLogFilter::NewL();
        mDuplicateMissedFilter->SetFlags(KLogEventRead);
        mDuplicateMissedFilter->SetNullFields(ELogFlagsField);
    }
    
    if ( viewCountL() > 0 && mBaseContext.logView().FirstL( mBaseContext.reqStatus() ) ){
        return true;
    }
    
    // Not possible to continue with reading
    return enterNextStateL();  
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::continueL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateReading::continueL()
{
    int& index = mBaseContext.index();  
    QList<LogsEvent*> &events = mContext.events();
        
    if ( mCheckingMissed ) {
        events.at(mEventIndex-1)->setDuplicates( 
            mBaseContext.duplicatesView().CountL() );
        mCheckingMissed = false;
    } 
    else {
        const CLogEvent& sourceEvent = event();
        LogsEvent* event = takeMatchingEvent(sourceEvent);
        bool inserted = false;
        if ( event ){
            // Matching event is updated and put to new position
            inserted = updateAndInsertEventL( sourceEvent, event, mEventIndex );
        }
        else {
            // Create new entry
            event = new LogsEvent;
            inserted = constructAndInsertEventL( 
                    sourceEvent, event, mEventIndex, mContext.events() );
        }
        
        if ( inserted  ){
            updateReadSizeCounter(*event);
            if ( handleMissedL(*event) ){
                mCheckingMissed = true;
                return true;
            }
        }
    }

    index++;    
    if ( canContinueReadingL(index) ){
        return mBaseContext.logView().NextL( mBaseContext.reqStatus() );
    }           
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::handleMissedL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateReading::handleMissedL(LogsEvent& parsedEvent)
{
    bool handled = false;
    if ( parsedEvent.direction() == LogsEvent::DirMissed ){
        handled = duplicatesL(mDuplicateMissedFilter); 
    }
    return handled;
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::updateReadSizeCounter
// ----------------------------------------------------------------------------
//
void LogsReaderStateReading::updateReadSizeCounter(LogsEvent& event)
{
    if ( LogsCommonData::getInstance().maxReadSize() >= 0 ){
        LogsEvent::LogsDirection dir = 
            LogsCommonData::getInstance().maxReadSizeDirection();
        if ( dir == LogsEvent::DirUndefined || dir == event.direction() ){
            mReadSizeCounter++;
        }
    }
}

// ----------------------------------------------------------------------------
// LogsReaderStateReading::canContinueReadingL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateReading::canContinueReadingL(int index) const
{
    bool canContinue( index < viewCountL() );
    int maxReadSize = LogsCommonData::getInstance().maxReadSize();
    if ( canContinue && maxReadSize >= 0 ){
        canContinue = ( mReadSizeCounter < maxReadSize );
    }
    return canContinue;
}

// ----------------------------------------------------------------------------
// LogsReaderStateFillDetails::LogsReaderStateFillDetails
// ----------------------------------------------------------------------------
//
LogsReaderStateFillDetails::LogsReaderStateFillDetails(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext ) 
 : LogsReaderStateBase(context, readerContext)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateFillDetails::~LogsReaderStateFillDetails
// ----------------------------------------------------------------------------
//
LogsReaderStateFillDetails::~LogsReaderStateFillDetails()
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateFillDetails::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFillDetails::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateFillDetails::enterL()" )
    fillDetails();
    LOGS_QDEBUG( "logs [ENG] <- LogsReaderStateFillDetails::enterL()" )
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateFillDetails::fillDetails
// ----------------------------------------------------------------------------
//
void LogsReaderStateFillDetails::fillDetails()
{
    mDuplicateLookup.invalidate();
    QSet<QString> numbersWithoutMatch;
    QHash<QString, ContactCacheEntry>& contactMappings = mContext.contactCache();
    QList<LogsEvent*> &events = mContext.events();
    foreach ( LogsEvent* event, events ){  
        if ( event->isInView() ){
            const QString& num = event->getNumberForCalling();
            if ( !event->remoteParty().isEmpty() ){
                // NOP
            } else if ( numbersWithoutMatch.contains(num) ) {
                event->setRemoteParty( "", true ); // No contact match found
            } else {
                // No remote party name, search match from contact cache or phonebook 
                searchMatchForNumber(contactMappings, numbersWithoutMatch, *event, num);
            }
            if ( mBaseContext.isRecentView() ){
                LogsEvent* duplicateEvent = mDuplicateLookup.findDuplicate(*event);
                if ( duplicateEvent ){
                    mergeDuplicates( *duplicateEvent, *event ); 
                } else {
                    mDuplicateLookup.addLookupEntry(*event);
                }
            }
        }
    } 
    
    mDuplicateLookup.cleanup();
}

// ----------------------------------------------------------------------------
// LogsReaderStateFillDetails::searchMatchForNumber
// ----------------------------------------------------------------------------
//
void LogsReaderStateFillDetails::searchMatchForNumber(
    QHash<QString, ContactCacheEntry>& contactMappings, 
    QSet<QString>& numbersWithoutMatch, 
    LogsEvent& event, 
    const QString& num)
{
    if ( contactMappings.contains(num) ) {
        // Matching cached contact found, use that
        LOGS_QDEBUG_2( "logs [ENG]    Use existing contact for num:", num )
        ContactCacheEntry entry = contactMappings.value(num);
        event.setRemoteParty( entry.mRemoteParty, true );
        event.setContactLocalId( entry.mContactLocalId );
    } else {
        QString contactNameStr;
        if (event.updateRemotePartyFromContacts(
                LogsCommonData::getInstance().contactManager(), contactNameStr)){
            LOGS_QDEBUG_3( "LogsReaderStateFillDetails, (name, num):", 
                           contactNameStr, num );
            // Cache the new contact name
            ContactCacheEntry contactEntry(contactNameStr, event.contactLocalId());
            contactMappings.insert( num, contactEntry );
        } else {
            // Avoid searching match again for the same number at this round
            event.setRemoteParty( "", true ); // No contact match found
            numbersWithoutMatch.insert( num );
        }
    }
}
 
// ----------------------------------------------------------------------------
// LogsReaderStateFillDetails::mergeDuplicates
// ----------------------------------------------------------------------------
//
void LogsReaderStateFillDetails::mergeDuplicates( 
    LogsEvent& usedEvent, LogsEvent& discardedEvent ) const
{
    usedEvent.merge(discardedEvent);
    discardedEvent.setIsInView(false);
}

// ----------------------------------------------------------------------------
// LogsReaderStateDone::LogsReaderStateDone
// ----------------------------------------------------------------------------
//
LogsReaderStateDone::LogsReaderStateDone(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext ) 
 : LogsReaderStateBase(context, readerContext)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateDone::~LogsReaderStateDone
// ----------------------------------------------------------------------------
//
LogsReaderStateDone::~LogsReaderStateDone()
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateDone::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateDone::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateDone::enterL" );
    
    mContext.observer().readCompleted();

    LOGS_QDEBUG( "logs [ENG] <- LogsReaderStateDone::enterL" );
    
    return false;
} 

// ----------------------------------------------------------------------------
// LogsReaderStateFindingDuplicates::LogsReaderStateFindingDuplicates
// ----------------------------------------------------------------------------
//
LogsReaderStateFindingDuplicates::LogsReaderStateFindingDuplicates(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext ) 
 : LogsReaderStateBase(context, readerContext),
   mDuplicateFilter(0)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateFindingDuplicates::~LogsReaderStateFindingDuplicates
// ----------------------------------------------------------------------------
//
LogsReaderStateFindingDuplicates::~LogsReaderStateFindingDuplicates()
{
    delete mDuplicateFilter;
}

// ----------------------------------------------------------------------------
// LogsReaderStateFindingDuplicates::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFindingDuplicates::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateFindingDuplicates::enterL" );
    
    if ( !mDuplicateFilter ){
        // Interested only about duplicates which are not marked as read
        mDuplicateFilter = CLogFilter::NewL();
        mDuplicateFilter->SetFlags(KLogEventRead);
        mDuplicateFilter->SetNullFields(ELogFlagsField);
    }
    
    if ( duplicatesL(mDuplicateFilter) ) {
        LOGS_QDEBUG( "logs [ENG] duplicates exist!");
        return true;
    }
    
    // Not possible to continue with finding
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateFindingDuplicates::continueL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateFindingDuplicates::continueL()
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsReaderStateFindingDuplicates::continueL" );
    
    return enterNextStateL();
}


// ----------------------------------------------------------------------------
// LogsReaderStateMarkingDuplicates::LogsReaderStateMarkingDuplicates
// ----------------------------------------------------------------------------
//
LogsReaderStateMarkingDuplicates::LogsReaderStateMarkingDuplicates(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext ) 
 : LogsReaderStateBase(context, readerContext)
{
}
    
// ----------------------------------------------------------------------------
// LogsReaderStateMarkingDuplicates::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateMarkingDuplicates::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateMarkingDuplicates::enterL" );
    
    mGettingDuplicates = false;
    if ( event().Id() == mBaseContext.currentEventId() ) {
        // Mark event read
        event().SetFlags( event().Flags() | KLogEventRead ); 
        mBaseContext.logClient().ChangeEvent(event(), mBaseContext.reqStatus());
        return true;
    }
    
    // Not possible to continue with marking
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateMarkingDuplicates::continueL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateMarkingDuplicates::continueL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateMarkingDuplicates::continueL" );

    if ( !mGettingDuplicates ){
        if ( duplicatesL() ) {
            LOGS_QDEBUG( "logs [ENG] duplicates exist!");
            mGettingDuplicates = true;
            return true;
        }
    } else {
        // Mark duplicate events read
        mBaseContext.duplicatesView().SetFlagsL(
            mBaseContext.duplicatesView().Event().Flags() | KLogEventRead ); 
    }

    LOGS_QDEBUG( "logs [ENG] <- LogsReaderStateMarkingDuplicates::continueL" );
    
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateReadingDuplicates::LogsReaderStateReadingDuplicates
// ----------------------------------------------------------------------------
//
LogsReaderStateReadingDuplicates::LogsReaderStateReadingDuplicates(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext ) 
 : LogsReaderStateBase(context, readerContext)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateReadingDuplicates::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateReadingDuplicates::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateReadingDuplicates::enterL" );
    if ( mBaseContext.duplicatesView().CountL() > 0 && 
        mBaseContext.duplicatesView().FirstL(mBaseContext.reqStatus()) ){
         LOGS_QDEBUG( "logs [ENG] duplicates exist!");
         return true;
    }
    
    // Not possible to continue with deletion
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateReadingDuplicates::continueL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateReadingDuplicates::continueL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateReadingDuplicates::continueL" );

    int nextIndex = mContext.duplicatedEvents().count();
    const CLogEvent& sourceEvent = mBaseContext.duplicatesView().Event();
    LogsEvent* event = new LogsEvent;
    constructAndInsertEventL( 
            sourceEvent, event, nextIndex, mContext.duplicatedEvents() );
    if ( mBaseContext.duplicatesView().NextL(mBaseContext.reqStatus()) ) {
        return true;
    } 

    LOGS_QDEBUG( "logs [ENG] <- LogsReaderStateReadingDuplicates::continueL" );
    
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateMergingDuplicates::LogsReaderStateMergingDuplicates
// ----------------------------------------------------------------------------
//
LogsReaderStateMergingDuplicates::LogsReaderStateMergingDuplicates(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext ) 
 : LogsReaderStateBase(context, readerContext)
{
}

// ----------------------------------------------------------------------------
// LogsReaderStateMergingDuplicates::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateMergingDuplicates::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateMergingDuplicates::enterL" );

    QList<LogsEvent*>& duplicates = mContext.duplicatedEvents();
    
    LogsEvent* event = eventById( mBaseContext.currentEventId() );
    if ( event ){
        for ( int i = 0; i < event->mergedDuplicates().count(); i++ ){
            const LogsEvent& mergedDupl = event->mergedDuplicates().at(i);
            if ( !mergedDupl.isSeenLocally() ){
                // Search backwards duplicates list until later occured event is
                // found and insert the merged duplicate after that.
                // Event is added to beginning of the list if there's no
                // later events.
                int insertIndex = 0;
                int lastIndex = duplicates.count() - 1;
                for ( int j = lastIndex; j >= 0 && insertIndex == 0; j-- ) {
                    if ( duplicates.at(j)->time() >= mergedDupl.time() ){
                        insertIndex = j + 1; // break the loop
                    }
                }
                duplicates.insert(insertIndex, new LogsEvent(mergedDupl) );
            }
        }
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsReaderStateMergingDuplicates::enterL" );   
    return enterNextStateL();
}

// ----------------------------------------------------------------------------
// LogsReaderStateModifyingDone::LogsReaderStateModifyingDone
// ----------------------------------------------------------------------------
//
LogsReaderStateModifyingDone::LogsReaderStateModifyingDone(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext ) 
 : LogsReaderStateBase(context, readerContext)
{

}
    
// ----------------------------------------------------------------------------
// LogsReaderStateModifyingDone::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateModifyingDone::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateModifyingDone::enterL" );
    
    LogsEvent* modifiedEvent = eventById(mBaseContext.currentEventId());
    if ( modifiedEvent ){
        // Update modified event to know that it has been marked. Real
        // update of the event happens soon when db notifies the change.
        modifiedEvent->markedAsSeenLocally(true);
    }
    mContext.observer().eventModifyingCompleted();

    LOGS_QDEBUG( "logs [ENG] <- LogsReaderStateModifyingDone::enterL" );
    
    return false;
}

// ----------------------------------------------------------------------------
// LogsReaderStateReadingDuplicatesDone::LogsReaderStateReadingDuplicatesDone
// ----------------------------------------------------------------------------
//
LogsReaderStateReadingDuplicatesDone::LogsReaderStateReadingDuplicatesDone(
    LogsStateBaseContext& context, LogsReaderStateContext& readerContext ) 
 : LogsReaderStateBase(context, readerContext)
{

}
    
// ----------------------------------------------------------------------------
// LogsReaderStateReadingDuplicatesDone::enterL
// ----------------------------------------------------------------------------
//
bool LogsReaderStateReadingDuplicatesDone::enterL()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsReaderStateReadingDuplicatesDone::enterL" );
    
    QList<LogsEvent*> duplicates = mContext.duplicatedEvents();
    mContext.duplicatedEvents().clear();
    mContext.observer().duplicatesReadingCompleted(duplicates);

    LOGS_QDEBUG( "logs [ENG] <- LogsReaderStateReadingDuplicatesDone::enterL" );
    
    return false;
}
