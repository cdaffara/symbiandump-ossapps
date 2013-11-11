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

#include "logscntfinder.h"
#include "logslogger.h"

#include <QStringList>
#include <qcontactmanager.h>
#include <qcontact.h>
#include <qcontactname.h>
#include <qcontactphonenumber.h>

QString testHighlights;

// -----------------------------------------------------------------------------
// LogsCntEntry::richText()
// -----------------------------------------------------------------------------
//
QString LogsCntText::richText( QString startTag, 
                               QString endTag ) const
{
    QString str = text();
    if ( str.length() > 0 && highlights() > 0 ) {
        str.insert( highlights() , endTag );
        str.insert( 0, startTag );
    }

    return str;
    
}

// -----------------------------------------------------------------------------
// LogsCntEntry::highlights()
// -----------------------------------------------------------------------------
//
int LogsCntText::highlights() const 
{
    return testHighlights.length() ;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::LogsCntEntry()
// -----------------------------------------------------------------------------
//
LogsCntEntry::LogsCntEntry( LogsCntEntryHandle& handle, 
                            quint32 cid )
    : mType( EntryTypeHistory ), mCid( cid ), 
      mCached( false ),mHandle(&handle)
{
    LogsCntText empty;
    mFirstName.append( empty );
    mLastName.append( empty );
    mAvatarPath = "c:\\data\\images\\bg_1.png";
}

// -----------------------------------------------------------------------------
// LogsCntEntry::LogsCntEntry()
// -----------------------------------------------------------------------------
//
LogsCntEntry::LogsCntEntry( quint32 cid )
    : mType( EntryTypeContact ), mCid( cid ), 
      mCached( false ),mHandle(0)
{
    LogsCntText empty;
    mFirstName.append( empty );
    mLastName.append( empty );
}

// -----------------------------------------------------------------------------
// copy LogsCntEntry::LogsCntEntry()
// -----------------------------------------------------------------------------
//
LogsCntEntry::LogsCntEntry( const LogsCntEntry& entry )
    : mType(entry.mType),
      mCid(entry.mCid),
      mFirstName(entry.mFirstName),
      mLastName(entry.mLastName),
      mCached(entry.mCached),
      mHandle(entry.mHandle),
      mPhoneNumber(entry.mPhoneNumber)
{
}

// -----------------------------------------------------------------------------
// LogsCntEntry::~LogsCntEntry()
// -----------------------------------------------------------------------------
//
LogsCntEntry::~LogsCntEntry()
{
}

// -----------------------------------------------------------------------------
// LogsCntEntry::LogsCntEntry()
// -----------------------------------------------------------------------------
//
const LogsCntTextList& LogsCntEntry::firstName() const 
{
    return mFirstName;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::lastName()
// -----------------------------------------------------------------------------
//
const LogsCntTextList& LogsCntEntry::lastName() const
{
    return mLastName;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::avatarPath()
// -----------------------------------------------------------------------------
//
const QString& LogsCntEntry::avatarPath() const
{
    return mAvatarPath;
}


// -----------------------------------------------------------------------------
// LogsCntEntry::phoneNumber()
// -----------------------------------------------------------------------------
//
const LogsCntText& LogsCntEntry::phoneNumber() const
{
    return mPhoneNumber;
}


// -----------------------------------------------------------------------------
// LogsCntEntry::contactId()
// -----------------------------------------------------------------------------
//
quint32 LogsCntEntry::contactId() const
{
    return mCid;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::handle()
// -----------------------------------------------------------------------------
//
LogsCntEntryHandle* LogsCntEntry::handle() const
{
    return mHandle;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::setFirstName()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setFirstName( const QString& name ) 
{
    mCached=true;
    mFirstName.clear();
    doSetText( name, mFirstName );
}

// -----------------------------------------------------------------------------
// LogsCntEntry::setLastName()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setLastName( const QString& name ) 
{
    mCached=true;
    mLastName.clear();
    doSetText( name, mLastName );
}

// -----------------------------------------------------------------------------
// LogsCntEntry::setPhoneNumber()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setPhoneNumber( const QString& number )
{
    mPhoneNumber.mText = number;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::doSetText()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::doSetText( const QString& text, LogsCntTextList& textlist ) 
{
    const QChar separ(' ');
    QStringList parts = text.split( separ, QString::SkipEmptyParts );
    for( int i=0;i<parts.count();i++) {
        LogsCntText txt;
        txt.mText = parts[i];
        if ( type() == EntryTypeHistory  ) {
            txt.mTranslatedText = txt.mText;
        }
        textlist.append( txt );
    }
}


// -----------------------------------------------------------------------------
// LogsCntEntry::setHighlights()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setHighlights( const QString& pattern )
{
    testHighlights = pattern;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::match()
// -----------------------------------------------------------------------------
//
bool LogsCntEntry::match( const QString& pattern ) const
{
    return doMatch( pattern, mFirstName ) ||
           doMatch( pattern, mLastName );
}

// -----------------------------------------------------------------------------
// LogsCntEntry::doMatch()
// -----------------------------------------------------------------------------
//
bool LogsCntEntry::doMatch( const QString& pattern, 
                            const LogsCntTextList& textlist ) const
{
     //direct match with phone number is enough
    if ( mPhoneNumber.text().startsWith( pattern ) ) {
        return true;
    }
    
    bool found = false;
    int index=0;
    while( index < textlist.count() && !found ) {
        found = textlist.at( index++ ).mTranslatedText.startsWith( pattern );
    }
    return found;
    
}

// -----------------------------------------------------------------------------
// LogsCntEntry::isCached()
// -----------------------------------------------------------------------------
//
bool LogsCntEntry::isCached() const
{
    return mCached;
}


// -----------------------------------------------------------------------------
// LogsCntEntry::type()
// -----------------------------------------------------------------------------
//
LogsCntEntry::EntryType LogsCntEntry::type() const
{
    return mType;
}

// -----------------------------------------------------------------------------
// LogsCntFinder::LogsCntFinder()
// -----------------------------------------------------------------------------
//
LogsCntFinder::LogsCntFinder(bool /*preferDefaultNumber*/)
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
LogsCntFinder::LogsCntFinder(QContactManager& contactManager,bool /*preferDefaultNumber*/)
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
    
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::~LogsCntFinder()" )
}


// -----------------------------------------------------------------------------
// LogsCntFinder::predictiveSearchQuery
// -----------------------------------------------------------------------------
//
void LogsCntFinder::predictiveSearchQuery( const QString& pattern )
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::predictiveSearchQuery()" )
    LOGS_QDEBUG_2( "logs [FINDER] pattern= ", pattern )
    
    bool patternChanged = pattern != mCurrentPredictivePattern;
    mCurrentPredictivePattern = pattern;
    
    if ( !mCurrentPredictivePattern.isEmpty() && patternChanged ) {
        qDeleteAll( mResults );
        mResults.clear();
        doPredictiveHistoryQuery();
        doPredictiveContactQuery();
    }
    //emit queryReady(); // commented off to ease testing
 
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
            mResults.append( entry );
        }
    }
        
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::doPredictiveHistoryQuery()" )
}

// -----------------------------------------------------------------------------
// LogsCntFinder::doPredictiveContactQuery
// -----------------------------------------------------------------------------
//
void LogsCntFinder::doPredictiveContactQuery()
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntFinder::doPredictiveContactQuery()" )
/*
    int index = 0;
    while( index < cntIds.count() ) {
      LogsCntEntry* entry = new LogsCntEntry( cntIds.at( index++ ) );
      mResults.append( entry );
    }
    */
    LOGS_QDEBUG( "logs [FINDER] <- LogsCntFinder::doPredictiveContactQuery()" )
    
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
        QContact contact = mContactManager->contact( entry->contactId() );
        QContactName contactName = contact.detail( QContactName::DefinitionName );
        entry->setFirstName( contactName.value( QContactName::FieldFirstName ) );
        entry->setLastName( contactName.value( QContactName::FieldLastName ) );
        QContactPhoneNumber contactPhoneNumber = 
              contact.detail( QContactPhoneNumber::DefinitionName );
        entry->setPhoneNumber( 
              contactPhoneNumber.value( QContactPhoneNumber::FieldNumber ) );
        
        entry->setHighlights( mCurrentPredictivePattern );
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
// LogsCntFinder::resetResults
// -----------------------------------------------------------------------------
//
void LogsCntFinder::resetResults()
{
    qDeleteAll( mResults );
    mResults.clear();
}



