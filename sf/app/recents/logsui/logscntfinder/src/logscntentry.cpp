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

#include <QListIterator>
#include <QStringList>
#include <QVector>

#include "logscntentry.h"
#include "logspredictivetranslator.h"
#include "logslogger.h"



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
// LogsCntEntry::LogsCntEntry()
// -----------------------------------------------------------------------------
//
LogsCntEntry::LogsCntEntry( LogsCntEntryHandle& handle, 
                            quint32 cid )
    : mType( EntryTypeHistory ), mCid( cid ), 
      mCached( true ),mHandle(&handle)
{
    LogsCntText empty;
    mFirstName.append( empty );
    mLastName.append( empty );
    mAvatarPath = "";
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
    mAvatarPath = "";
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
      mPhoneNumber(entry.mPhoneNumber),
      mAvatarPath(entry.mAvatarPath)
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
// LogsCntEntry::firstName()
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
// LogsCntEntry::speedDial()
// -----------------------------------------------------------------------------
//
const QString& LogsCntEntry::speedDial() const
{
    return mSpeedDial;
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
// LogsCntEntry::setAvatarPath()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setAvatarPath( const QString& avatarpath ) 
{
    mCached=true;
    mAvatarPath.clear();
    mAvatarPath = avatarpath;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::setPhoneNumber()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setPhoneNumber( const QString& number )
{
    LogsPredictiveTranslator* translator = LogsPredictiveTranslator::instance();
    
    mCached=true;
    mPhoneNumber.mText = number;
    mPhoneNumber.mTranslatedText = translator->translateText( mPhoneNumber.mText );
    
}


// -----------------------------------------------------------------------------
// LogsCntEntry::doSetText()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::doSetText( const QString& text, LogsCntTextList& textlist ) 
{
    LogsPredictiveTranslator* translator = LogsPredictiveTranslator::instance();
    
    QListIterator<QString> iter( translator->nameTokens( text ) );

    while( iter.hasNext() ) {
        LogsCntText txt;
        txt.mText = iter.next();
        txt.mTranslatedText = translator->translateText( txt.mText );
        textlist.append( txt );
    }
    if ( textlist.count() == 0 ) {
        textlist.append( LogsCntText() );
    }
}


// -----------------------------------------------------------------------------
// LogsCntEntry::resetHighlights()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::resetHighlights( LogsCntTextList& nameArray )
{
    QMutableListIterator<LogsCntText> names( nameArray ); 
    while( names.hasNext() ) {
        names.next().mHighlights = 0;
    }
    
}


// -----------------------------------------------------------------------------
// LogsCntEntry::setHighlights()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setHighlights( const QString& pattern )
{
    resetHighlights( mFirstName );
    resetHighlights( mLastName );
    
    mPhoneNumber.mHighlights =
              mPhoneNumber.mTranslatedText.startsWith( pattern ) &&
              mPhoneNumber.mTranslatedText.length() >= pattern.length() ?
            pattern.length(): 0;
    
    doSetHighlights( pattern, mFirstName );
    doSetHighlights( pattern, mLastName );
    
}

// -----------------------------------------------------------------------------
// LogsCntEntry::doSetHighlights()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::doSetHighlights( const QString& pattern, 
                                    LogsCntTextList& nameArray )
{
    
    LogsPredictiveTranslator* translator = LogsPredictiveTranslator::instance();
    QMutableListIterator<LogsCntText> names( nameArray );
    QString modifiedPattern = pattern;
    modifiedPattern = translator->trimPattern( modifiedPattern, true );
    
    bool hasSeparators = translator->hasPatternSeparators( modifiedPattern );
    
    //simple
    while( names.hasNext() ) {
        LogsCntText& nameItem = names.next();
        //must use non-optimized version with whole pattern
        nameItem.mHighlights = startsWith( nameItem, modifiedPattern );
    }

    if ( hasSeparators ) {
        //complex
        QListIterator<QString> patternArray( translator->patternTokens( modifiedPattern ) );
        while( patternArray.hasNext() ) {
            QString patternItem = patternArray.next();
            doSetHighlights( patternItem, names );
            translator->trimPattern( patternItem );
            doSetHighlights( patternItem, names );
        }
    }
    
}

// -----------------------------------------------------------------------------
// LogsCntEntry::doSetHighlights()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::doSetHighlights( const QString& patternItem, 
                                    QMutableListIterator<LogsCntText>& names )
                                            
{
    names.toFront();
    while( names.hasNext() ) {
        LogsCntText& nameItem = names.next();
        int matchSize = startsWith( nameItem, patternItem );
        nameItem.mHighlights = matchSize > nameItem.mHighlights ?
                               matchSize : nameItem.mHighlights; 
    }

}

// -----------------------------------------------------------------------------
// LogsCntEntry::startsWith()
// -----------------------------------------------------------------------------
//
int LogsCntEntry::startsWith( const LogsCntText& nameItem, 
                              const QString& pattern ) const
{
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntEntry::startsWith()" )
    //assumed that text has found based on pattern, thus only checking with
    //first char is enough, if mightContainZeroes eq false
    const QString& text = nameItem.mTranslatedText;
    
    int matchCount = pattern.length();
    if ( text.isEmpty() || matchCount > text.length() ) {
        matchCount = 0;
    } else {
        matchCount = text.startsWith( pattern ) ? matchCount : 0; 
    }
    LOGS_QDEBUG( "logs [FINDER] -> LogsCntEntry::startsWith()" )
    return matchCount;
}


// -----------------------------------------------------------------------------
// LogsCntEntry::setSpeedDial()
// -----------------------------------------------------------------------------
//
void LogsCntEntry::setSpeedDial( const QString& number )
{
    mSpeedDial = number;
}



// -----------------------------------------------------------------------------
// LogsCntEntry::match()
// -----------------------------------------------------------------------------
//
bool LogsCntEntry::match( const QString& pattern ) const
{
    bool match = false;
    if ( pattern.length() > 0 ) {
        LogsPredictiveTranslator* translator = LogsPredictiveTranslator::instance();
        
        QString modifiedPattern = pattern;
        modifiedPattern = translator->trimPattern( modifiedPattern, true );
        
        //direct match with phone number is enough
        match = ( type() == EntryTypeHistory && 
                  mPhoneNumber.mTranslatedText.startsWith( pattern ) ) ||
                doSimpleMatch( modifiedPattern );
        
        if (!match && translator->hasPatternSeparators( modifiedPattern ) ) {
            QStringList patternArray = translator->patternTokens( modifiedPattern );
            match = doComplexMatch( patternArray );
            if (!match ) {
                for(int i=0;i<patternArray.length();i++ ) {
                    translator->trimPattern( patternArray[i] );
                }
                match = doComplexMatch( patternArray );
            }
        }
    }
    
    return match;
}

// -----------------------------------------------------------------------------
// LogsCntEntry::doSimpleMatch()
// -----------------------------------------------------------------------------
//
bool LogsCntEntry::doSimpleMatch( const QString& pattern ) const
{
    LogsCntTextList nameArray = mFirstName + mLastName; //with empties
    QListIterator<LogsCntText> names( nameArray ); 
    int matchCount = 0;

    while( names.hasNext() && !matchCount ) {
        matchCount = (int)names.next().mTranslatedText.startsWith( pattern );
    }

    return matchCount > 0;
}


// -----------------------------------------------------------------------------
// LogsCntEntry::doComplexMatch()
// -----------------------------------------------------------------------------
//
bool LogsCntEntry::doComplexMatch( const QStringList& patternArray ) const
{
    const bool zero = false;

    LogsCntTextList nameArray = mFirstName + mLastName; //with empties

    int targetMatchCount = patternArray.count();
    int namesCount = nameArray.count();

    //if pattern has more tokens than name(s), it is a missmatch
    if ( namesCount < targetMatchCount ) {
        return false;
    }

    QListIterator<LogsCntText> names( nameArray ); 
    QListIterator<QString> patterns( patternArray );
    QVector<bool> matchVector(targetMatchCount, zero );
    int currentPattern = 0;
    int matchCount = 0;
    bool match = false;
    
    while( names.hasNext() && matchCount < targetMatchCount ) {
        LogsCntText name = names.next();
        currentPattern = 0;
        patterns.toFront();
        match = false;
        while ( !name.mText.isEmpty() && 
                 patterns.hasNext() && !match ) {
            QString pattern = patterns.next();
            //unique match check
            if ( !matchVector.at( currentPattern ) ) {
                match = matchVector[ currentPattern ] 
                      = name.mTranslatedText.startsWith( pattern );
                matchCount = match ? matchCount+1 : matchCount;
            }
            currentPattern++;
        }
    }
    return matchCount >= targetMatchCount;

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



