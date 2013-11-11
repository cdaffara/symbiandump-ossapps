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

#include <qcontactdetailfilter.h>
#include <qcontactphonenumber.h>
#include <qcontactname.h>
#include <qcontactmanager.h>
#include <qcontactavatar.h>
#include <qcontactaction.h>

#include "logscntentry.h"
#include "logscntfinder.h"
#include "logspredictivetranslator.h"
#include "logslogger.h"

const int MaxPredSearchPatternLen = 15;



// -----------------------------------------------------------------------------
// LogsCntFinder::LogsCntFinder()
// -----------------------------------------------------------------------------
//
LogsCntFinder::LogsCntFinder(bool preferDefaultNumber)
    : mPreferDefaultNumber(preferDefaultNumber), mCachedCounter(0)
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::LogsCntFinder()" )
    
    // Create manager ourselves, object takes care of deletion when registering
    // as parent.
    QMap<QString, QString> dummyParams;
    mContactManager = new QContactManager("symbian", dummyParams, this);
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::LogsCntFinder()" )
}

// -----------------------------------------------------------------------------
// LogsCntFinder::LogsCntFinder()
// -----------------------------------------------------------------------------
//
LogsCntFinder::LogsCntFinder(QContactManager& contactManager, bool preferDefaultNumber)
    : mPreferDefaultNumber(preferDefaultNumber), mCachedCounter(0)
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::LogsCntFinder(), cntmgr from client" )
    
    mContactManager = &contactManager;
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::LogsCntFinder()" )
}

// -----------------------------------------------------------------------------
// LogsCntFinder::~LogsCntFinder()
// -----------------------------------------------------------------------------
//
LogsCntFinder::~LogsCntFinder()
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::~LogsCntFinder()" )

    qDeleteAll( mResults );
    qDeleteAll( mHistoryEvents );
    LogsPredictiveTranslator::deleteInstance();
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::~LogsCntFinder()" )
}

// -----------------------------------------------------------------------------
// LogsCntFinder::isProgressivePattern
// -----------------------------------------------------------------------------
//
bool LogsCntFinder::isProgressivePattern( const QString& pattern ) const
{
    //"" -> XXX not progressive
    //XX -> YY not progressive
    //XXX -> YY not progressive
    int prevPatternLen = mCurrentInputPattern.length();
    return prevPatternLen > 0 &&  
           pattern.length() - prevPatternLen > 0;
}


// -----------------------------------------------------------------------------
// LogsCntFinder::predictiveSearchQuery
// -----------------------------------------------------------------------------
//
void LogsCntFinder::predictiveSearchQuery( const QString& pattern )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::predictiveSearchQuery()" )
    LOGS_QDEBUG_2( "logs [FINDER] pattern= ", pattern )

    if ( pattern.length() > MaxPredSearchPatternLen ) {
        LOGS_QDEBUG( "logs [FINDER] too long pattern. Exit quietly.")
        LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::predictiveSearchQuery()" )
        return;
    }
        
    //in this point mCurrentPredictivePattern is previous
    bool patternIsProgressive = isProgressivePattern( pattern );
    bool resultsAreAllCached = resultsCount() > 0 && 
                               mCachedCounter == resultsCount();
    bool nothingToDo = resultsCount() == 0 &&
                       patternIsProgressive;
    int patternSeparators = 
            LogsPredictiveTranslator::instance()->hasPatternSeparators( 
                                                        mCurrentInputPattern );
    bool doCacheQuery =  patternSeparators != mCurrentInputPattern.length() && 
                        ( ( patternIsProgressive && resultsAreAllCached ) || nothingToDo );
            
    LOGS_QDEBUG_2( "logs [FINDER] patternIsProgressive = ", patternIsProgressive )
    LOGS_QDEBUG_2( "logs [FINDER] resultsAreAllCached = ", resultsAreAllCached )
    LOGS_QDEBUG_2( "logs [FINDER] nothingToDo = ", nothingToDo )
    LOGS_QDEBUG_2( "logs [FINDER] cachedCounter = ", mCachedCounter )
    LOGS_QDEBUG_2( "logs [FINDER] patternSeparators = ", patternSeparators )
    LOGS_QDEBUG_2( "logs [FINDER] doCacheQuery = ", doCacheQuery )
    
    setCurrentPattern( pattern );
    
    if ( mCurrentPredictivePattern.isEmpty() ) {
        qDeleteAll( mResults );
        mResults.clear();
        mCachedCounter = 0;
    } else if ( doCacheQuery ) {
        doPredictiveCacheQuery();
    } else {        
        mCachedCounter = 0;
        LogsCntEntryList recentResults = mResults;
        mResults.clear();
        doPredictiveHistoryQuery();
        doPredictiveContactQuery( recentResults );
        qDeleteAll( recentResults );
    }
    emit queryReady();
 
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::predictiveSearchQuery()" )
}

// -----------------------------------------------------------------------------
// LogsCntFinder::doPredictiveHistoryQuery
// -----------------------------------------------------------------------------
//
void LogsCntFinder::doPredictiveHistoryQuery()
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::doPredictiveHistoryQuery()" )
    
    QListIterator<LogsCntEntry*> iter(mHistoryEvents);
    
    while( iter.hasNext() ) {
        LogsCntEntry* e = iter.next();
        if ( e->match( mCurrentPredictivePattern ) ) {
            LogsCntEntry* entry = new LogsCntEntry( *e );
            addResult( entry );
        }
    }
        
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::doPredictiveHistoryQuery()" )
}

// -----------------------------------------------------------------------------
// LogsCntFinder::doPredictiveContactQuery
// -----------------------------------------------------------------------------
//
void LogsCntFinder::doPredictiveContactQuery( LogsCntEntryList& recentResults )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::doPredictiveContactQuery()" )
    QContactDetailFilter df;
    df.setDetailDefinitionName( QContactName::DefinitionName, QContactName::FieldFirstName );
    df.setMatchFlags( QContactFilter::MatchKeypadCollation );
    df.setValue( mCurrentInputPattern );
    QList<QContactLocalId> cntIds;
    LOGS_QDEBUG( "logs [FINDER] about to call contacts manager" )
    
    cntIds = mContactManager->contactIds( df );
    LOGS_QDEBUG_2( "logs [FINDER] number of matched contacts =", cntIds.count() )
    int index = 0;
    while( index < cntIds.count() ) {
        addResult( cntIds.at( index++ ), recentResults );
    }
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::doPredictiveContactQuery()" )
    
}

// -----------------------------------------------------------------------------
// LogsCntFinder::doPredictiveCacheQuery()
// -----------------------------------------------------------------------------
//
void LogsCntFinder::doPredictiveCacheQuery()
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::doPredictiveCacheQuery()" )
    QMutableListIterator<LogsCntEntry*> iter(mResults);
    while( iter.hasNext() ) {
        LogsCntEntry* entry = iter.next();
        if ( !entry->match( mCurrentPredictivePattern ) ) {
            mCachedCounter = 
                    entry->isCached() ? mCachedCounter-1 : mCachedCounter;
            iter.remove();
            delete entry;
        } else {
            entry->setHighlights( mCurrentPredictivePattern );
        }
    }
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::doPredictiveCacheQuery()" )
    
}

// -----------------------------------------------------------------------------
// LogsCntFinder::addResult()
// -----------------------------------------------------------------------------
//
void LogsCntFinder::addResult( quint32 cntId, LogsCntEntryList& recentResults )
{
    QMutableListIterator<LogsCntEntry*> iter(recentResults);
    bool reused = false;
    while( iter.hasNext() && !reused ) {
        LogsCntEntry* entry = iter.next();
        if ( entry->contactId() == cntId ) {
            LOGS_QDEBUG_4( "logs [FINDER] LogsCntFinder::addResult() - \
re-using entry. contact id ", cntId, "cached=", entry->isCached() );
            iter.remove();
            addResult( entry );
            reused = true;
        }
    }
    
    if ( !reused ) {
        LogsCntEntry* entry = new LogsCntEntry( cntId );
        addResult( entry );
    }
}


// -----------------------------------------------------------------------------
// LogsCntFinder::addResult()
// -----------------------------------------------------------------------------
//
void LogsCntFinder::addResult( LogsCntEntry* entry )
{
    updateResult( entry );
    mResults.append( entry );
}

// -----------------------------------------------------------------------------
// LogsCntFinder::updateResult()
// -----------------------------------------------------------------------------
//
void LogsCntFinder::updateResult( LogsCntEntry* entry )
{
    if ( entry->isCached() ) {
        entry->setHighlights( mCurrentPredictivePattern );
        mCachedCounter++;
    }
}


// -----------------------------------------------------------------------------
// LogsCntFinder::setCurrentPattern()
// -----------------------------------------------------------------------------
//
void LogsCntFinder::setCurrentPattern( const QString& pattern )
{
    LogsPredictiveTranslator* translator = 
            LogsPredictiveTranslator::instance();
    
    mCurrentInputPattern = pattern;
    mCurrentPredictivePattern = translator->translatePattern( mCurrentInputPattern );    
}

// -----------------------------------------------------------------------------
// LogsCntFinder::phoneNumber
// -----------------------------------------------------------------------------
//
QString LogsCntFinder::phoneNumber(const QContact& contact) const
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::phoneNumber()" )
    QString number;
    if (mPreferDefaultNumber) {
            
        QContactActionDescriptor callActionName("call");
        
        number = contact.preferredDetail( callActionName.actionName() ).value(
                    QContactPhoneNumber::FieldNumber );
        
        if ( number.isEmpty() ) {
            number = contact.detailWithAction( 
                    QContactAction::action( callActionName ) ).value(
                    QContactPhoneNumber::FieldNumber );
        }
    }
    
    if (number.isEmpty()) {
        QContactPhoneNumber contactPhoneNumber = 
            contact.detail( QContactPhoneNumber::DefinitionName );
        number = contactPhoneNumber.value( QContactPhoneNumber::FieldNumber );
    }
    LOGS_QDEBUG_2( "logs [FINDER] -> LogsCntFinder::phoneNumber(): ", number )
    return number;
}

// -----------------------------------------------------------------------------
// LogsCntFinder::resultsCount
// -----------------------------------------------------------------------------
//
int LogsCntFinder::resultsCount() const
{
    return mResults.count();
}

// -----------------------------------------------------------------------------
// LogsCntFinder::resultAt
// -----------------------------------------------------------------------------
//
const LogsCntEntry& LogsCntFinder::resultAt( int index )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::resultAt()" )
    LOGS_QDEBUG_2( "logs [FINDER] index=", index )
    
    LogsCntEntry* entry = mResults.at( index );
    if ( !entry->isCached() ) {
        LOGS_QDEBUG_2( "logs [FINDER] caching from DB cid=", entry->contactId() )
        QContact contact = mContactManager->contact( entry->contactId() );
        QContactName contactName = contact.detail( QContactName::DefinitionName );
        entry->setFirstName( contactName.value( QContactName::FieldFirstName ) );
        entry->setLastName( contactName.value( QContactName::FieldLastName ) );
        entry->setPhoneNumber(phoneNumber(contact)); 
        QContactAvatar contactAvatar = contact.detail<QContactAvatar>();
        QString avatar = contactAvatar.value( QContactAvatar::FieldImageUrl );
        entry->setAvatarPath( avatar );
        
        updateResult( entry );
    }
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::resultAt()" )
    return *entry;
}

// -----------------------------------------------------------------------------
// LogsCntFinder::insertEntry
// -----------------------------------------------------------------------------
//
void LogsCntFinder::insertEntry( int index, LogsCntEntry* entry )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::insertEntry()" )
    LOGS_QDEBUG_4( "logs [FINDER] handle=", entry->handle()," to index ", index )
    
    mHistoryEvents.insert( index, entry );
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::insertEntry()" )
}

// -----------------------------------------------------------------------------
// LogsCntFinder::getEntry
// -----------------------------------------------------------------------------
//
LogsCntEntry* LogsCntFinder::getEntry( const LogsCntEntryHandle& handle ) const
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::getEntry()" )
    return doGetEntry( mHistoryEvents, handle );
}

// -----------------------------------------------------------------------------
// LogsCntFinder::doGetEntry
// -----------------------------------------------------------------------------
//
LogsCntEntry* LogsCntFinder::doGetEntry( const LogsCntEntryList& list, 
                                         const LogsCntEntryHandle& handle ) const
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::doGetEntry()" )
    LOGS_QDEBUG_2( "logs [FINDER] handle= ", &handle )
    
    LogsCntEntry* entry = 0;
    QListIterator<LogsCntEntry*> iter(list);
    
    while( iter.hasNext() && !entry ) {
        LogsCntEntry* e = iter.next();
        entry = e->handle() == &handle ? e : 0;
    }
    
    LOGS_QDEBUG_2( "logs [FINDER] found=", (entry!=0) )
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::doGetEntry()" )
    return entry;
}

// -----------------------------------------------------------------------------
// LogsCntFinder::deleteEntry
// -----------------------------------------------------------------------------
//
void LogsCntFinder::deleteEntry( const LogsCntEntryHandle& handle )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::deleteEntry()" )
    LOGS_QDEBUG_2( "logs [FINDER] handle= ", &handle )
    
    LogsCntEntry* toRemoveHistoryEv = doGetEntry( mHistoryEvents, handle );
    mHistoryEvents.removeOne( toRemoveHistoryEv );
    delete toRemoveHistoryEv;
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::deleteEntry()" )
    
}

// -----------------------------------------------------------------------------
// LogsCntFinder::setPreferDefaultNumber
// -----------------------------------------------------------------------------
//
void LogsCntFinder::setPreferDefaultNumber(bool preferDefault)
{
    if (mPreferDefaultNumber != preferDefault) {
        resetResults();
        mPreferDefaultNumber = preferDefault;
    }
}

// -----------------------------------------------------------------------------
// LogsCntFinder::setPreferDefaultNumber
// -----------------------------------------------------------------------------
//
bool LogsCntFinder::preferDefaultNumber() const
{
    return mPreferDefaultNumber;
}


// -----------------------------------------------------------------------------
// LogsCntFinder::resetResults
// -----------------------------------------------------------------------------
//
void LogsCntFinder::resetResults()
{
    qDeleteAll( mResults );
    mResults.clear();
    mCachedCounter = 0;
}



