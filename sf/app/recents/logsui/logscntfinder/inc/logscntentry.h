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

#ifndef LOGSCNTENTRY_H
#define LOGSCNTENTRY_H

#include <logscntfinderexport.h>
#include <QObject>
#include <QList>
#include <QString>


class LogsCntEntry;
typedef QObject LogsCntEntryHandle;


class LogsCntText
{
public:

    inline LogsCntText() : mHighlights(0){}
    inline const QString& text() const {return mText;}
    inline int highlights() const {return mHighlights;}
    LOGSCNTFINDER_EXPORT QString richText( QString startTag = QString("<b><u>"), 
                                           QString endTag = QString("</u></b>")) const;
    
    
private:
    
    QString mText;
    QString mTranslatedText;
    int mHighlights;
    
    friend class LogsCntEntry;
    friend class UT_LogsCntEntry;
    friend class UT_LogsCntFinder;
};

typedef QList<LogsCntText> LogsCntTextList;

class LogsCntEntry
{
public:

    enum EntryType {
        EntryTypeHistory,
        EntryTypeContact
    };
    
    LOGSCNTFINDER_EXPORT LogsCntEntry( LogsCntEntryHandle& handle, 
                                       quint32 cid );
    LogsCntEntry( quint32 cid );
    LogsCntEntry( const LogsCntEntry& entry );
    ~LogsCntEntry();
    LOGSCNTFINDER_EXPORT EntryType type() const;
    LOGSCNTFINDER_EXPORT const LogsCntTextList& firstName() const;
    LOGSCNTFINDER_EXPORT const LogsCntTextList& lastName() const;
    LOGSCNTFINDER_EXPORT quint32 contactId() const;
    LOGSCNTFINDER_EXPORT LogsCntEntryHandle* handle() const;
    LOGSCNTFINDER_EXPORT void setFirstName( const QString& name );
    LOGSCNTFINDER_EXPORT void setLastName( const QString& name );
    LOGSCNTFINDER_EXPORT const LogsCntText& phoneNumber() const;
    LOGSCNTFINDER_EXPORT const QString& avatarPath() const;
    LOGSCNTFINDER_EXPORT void setPhoneNumber( const QString& number );
    LOGSCNTFINDER_EXPORT void setAvatarPath( const QString& avatarpath );
    LOGSCNTFINDER_EXPORT const QString& speedDial() const;
    
    bool isCached() const;
    void setHighlights( const QString& pattern );
    void setSpeedDial( const QString& number );
    bool match( const QString& pattern ) const;

private:
    
    void doSetText( const QString& text, LogsCntTextList& textlist ); 
    void doSetHighlights( const QString& pattern, LogsCntTextList& nameArray );
    void doSetHighlights( const QString& patternItem, QMutableListIterator<LogsCntText>& names );
    
    void resetHighlights( LogsCntTextList& nameArray );
    bool doSimpleMatch( const QString& pattern ) const;
    bool doComplexMatch( const QStringList& patternArray ) const;
    int startsWith( const LogsCntText& nameItem, const QString& pattern ) const; 

private:
    
    EntryType mType;
    quint32 mCid;
    LogsCntTextList mFirstName;
    LogsCntTextList mLastName;
    bool mCached;
    LogsCntEntryHandle* mHandle;
    LogsCntText mPhoneNumber;
    QString mAvatarPath;
    QString mSpeedDial;
    
    friend class UT_LogsCntEntry;
    friend class UT_LogsCntFinder;
};

typedef QList<LogsCntEntry*> LogsCntEntryList;
    

#endif //LOGSCNTENTRY_H
