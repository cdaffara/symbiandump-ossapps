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

#include <logscntfinderexport.h>
#include <QObject>
#include <QList>
#include <QString>
#include <qmobilityglobal.h>

QTM_BEGIN_NAMESPACE
class QContactManager;
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

#include "logscntentry.h"

    
/**
 * Log events and contacts finder
 *
 */
class LogsCntFinder : public QObject 
{

    Q_OBJECT
    
public: // The exported API

    LOGSCNTFINDER_EXPORT LogsCntFinder(bool preferDefaultNumber = false);
    LOGSCNTFINDER_EXPORT LogsCntFinder(QContactManager& contactManager,
            bool preferDefaultNumber = false);
    LOGSCNTFINDER_EXPORT ~LogsCntFinder();

    /**
    * Starts/continues predictive query based on pattern. If
    * there is a previously executed query with same pattern, 
    * call is treated as continue query.
    * @param pattern the predictive pattern, containing digit(s)
    */
    LOGSCNTFINDER_EXPORT 
        void predictiveSearchQuery( const QString& pattern );
                                      
    /**
    * returns number of results
    * @return number of results
    */
    LOGSCNTFINDER_EXPORT 
      int resultsCount() const;
    
    /**
    * returns result at index
    * @param index the index
    */
    LOGSCNTFINDER_EXPORT 
      const LogsCntEntry& resultAt( int index );
    
    /**
    * Used for adding entiries to be part of a query
    * Ownership is transfered
    * @param entry the entry
    */
    LOGSCNTFINDER_EXPORT 
      void insertEntry( int index, LogsCntEntry* entry ); 
    
    /**
    * Used for updating entiries
    * @param handle the handle
    */
    LOGSCNTFINDER_EXPORT 
        LogsCntEntry* getEntry( const LogsCntEntryHandle& handle ) const; 
    
    /**
    * Used for updating entiries
    * @param entry the entry
    */
    LOGSCNTFINDER_EXPORT 
        void deleteEntry( const LogsCntEntryHandle& handle );

    
    /**
     * Used to define which phone number will be returned in search result
     * in case a contact has multiple numbers
     * @param preferDefault if set true, the number set as default for calling
     *        will be used, otherwise the first number from the number list
     *        will be used
     */
    LOGSCNTFINDER_EXPORT void setPreferDefaultNumber(bool preferDefault);

    /**
     * Used for checking whether default number for calling will be used in
     * search results, if contact has multiple phone numbers set
     * @return whether default number is used in search results
     */
    LOGSCNTFINDER_EXPORT bool preferDefaultNumber() const;

    
    /**
     * Used for resetting search results
     */
    LOGSCNTFINDER_EXPORT void resetResults();
    
    
signals:

    /**
    * emitted when query is ready
    */
    void queryReady();
    
    
private:

    void doPredictiveHistoryQuery();
    void doPredictiveContactQuery( LogsCntEntryList& recentResults );
    void doPredictiveCacheQuery();
    
    LogsCntEntry* doGetEntry( const LogsCntEntryList& list, 
                              const LogsCntEntryHandle& handle ) const;
    
    void addResult( quint32 cntId, LogsCntEntryList& recentResults );
    void addResult( LogsCntEntry* entry );
    void updateResult( LogsCntEntry* entry );
    bool isProgressivePattern( const QString& pattern ) const;
    void setCurrentPattern( const QString& pattern );
    QString phoneNumber(const QContact& contact) const;
    
private:
    
    QString mCurrentPredictivePattern;
    QString mCurrentInputPattern;
    LogsCntEntryList mResults;
    QContactManager* mContactManager;
    LogsCntEntryList mHistoryEvents;
    bool mPreferDefaultNumber;
    int mCachedCounter;
    
    friend class UT_LogsCntFinder;
    
};

#endif //LOGSCNTFINDER_H
