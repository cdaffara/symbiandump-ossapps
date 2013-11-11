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

#ifndef LOGSCNTFINDER_H
#define LOGSCNTFINDER_H

#include <QObject>
#include <QList>
#include <QString>
#include <qmobilityglobal.h>

QTM_BEGIN_NAMESPACE
class QContactManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class LogsCntEntry;
typedef QObject LogsCntEntryHandle;


class LogsCntText
{
public:

    inline LogsCntText() : mHighlights(0){}
    inline const QString& text() const {return mText;}
    int highlights() const;
    QString richText( QString startTag = QString("<b><u>"), 
                      QString endTag = QString("</u></b>")) const;
    
private:
    
    QString mText;
    QString mTranslatedText;
    int mHighlights;
    
    friend class LogsCntEntry;
    friend class UT_LogsCntEntry;
    friend class UT_LogsCntFinder;
    friend class UT_LogsMatchesModel;
};

typedef QList<LogsCntText> LogsCntTextList;

class LogsCntEntry
{
public:

    enum EntryType {
        EntryTypeHistory,
        EntryTypeContact
    };
    
    LogsCntEntry( LogsCntEntryHandle& handle, 
                                       quint32 cid );
    LogsCntEntry( quint32 cid );
    LogsCntEntry( const LogsCntEntry& entry );
    ~LogsCntEntry();
    EntryType type() const;
    const LogsCntTextList& firstName() const;
    const LogsCntTextList& lastName() const;
    const QString& avatarPath() const;
    quint32 contactId() const;
    LogsCntEntryHandle* handle() const;
    void setFirstName( const QString& name );
    void setLastName( const QString& name );
    const LogsCntText& phoneNumber() const;
    void setPhoneNumber( const QString& number );
    
    bool isCached() const;
    void setHighlights( const QString& pattern );
    bool match( const QString& pattern ) const;

private:
    
    void doSetText( const QString& text, LogsCntTextList& textlist ); 
    bool doMatch( const QString& pattern, 
                  const LogsCntTextList& textlist ) const;
    
private:
    
    EntryType mType;
    quint32 mCid;
    LogsCntTextList mFirstName;
    LogsCntTextList mLastName;
    bool mCached;
    LogsCntEntryHandle* mHandle;
    LogsCntText mPhoneNumber;
    QString mAvatarPath;
    
    friend class UT_LogsCntEntry;
    friend class UT_LogsCntFinder;
    friend class UT_LogsMatchesModel;
};

typedef QList<LogsCntEntry*> LogsCntEntryList;
    
/**
 * Log events and contacts finder
 *
 */
class LogsCntFinder : public QObject 
{

    Q_OBJECT
    
public: // The exported API

    LogsCntFinder(bool preferDefaultNumber = false);
    LogsCntFinder(QContactManager& contactManager, bool preferDefaultNumber = false);
    ~LogsCntFinder();

    /**
    * Starts/continues predictive query based on pattern. If
    * there is a previously executed query with same pattern, 
    * call is treated as continue query.
    * @param pattern the predictive pattern, containing digit(s)
    */
    void predictiveSearchQuery( const QString& pattern );
                                      
    /**
    * returns number of results
    * @return number of results
    */
    int resultsCount() const;
    
    /**
    * returns result at index
    * @param index the index
    */
    const LogsCntEntry& resultAt( int index );
    
    /**
    * Used for adding entiries to be part of a query
    * Ownership is transfered
    * @param entry the entry
    */
    void insertEntry( int index, LogsCntEntry* entry ); 
    
    /**
    * Used for updating entiries
    * @param handle the handle
    */
    LogsCntEntry* getEntry( const LogsCntEntryHandle& handle ) const; 
    
    /**
    * Used for updating entiries
    * @param entry the entry
    */
    void deleteEntry( const LogsCntEntryHandle& handle );
    
    void resetResults();
    
signals:

    /**
    * emitted when query is ready
    */
    void queryReady();
    
private:

    void doPredictiveHistoryQuery();
    void doPredictiveContactQuery();
    
    LogsCntEntry* doGetEntry( const LogsCntEntryList& list, 
                              const LogsCntEntryHandle& handle ) const;
    
    
private:
    
    QString mCurrentPredictivePattern;
    LogsCntEntryList mResults;
    QContactManager* mContactManager;
    LogsCntEntryList mHistoryEvents;
    
    friend class UT_LogsMatchesModel;
    
};

#endif //LOGSCNTFINDER_H
