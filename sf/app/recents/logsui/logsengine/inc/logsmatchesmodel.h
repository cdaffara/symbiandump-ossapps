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

#ifndef LOGSMATCHESMODEL_H
#define LOGSMATCHESMODEL_H

#include <logsexport.h>
#include <logsabstractmodel.h>
#include "logsengdefs.h"

class LogsEvent;
class LogsDbConnector;
class LogsCntFinder;
class LogsCntEntry;
class LogsCntText;
class LogsContact;
class LogsMatchesModelItemContainer;
class LogsThumbIconManager;
typedef QObject LogsCntEntryHandle;

/**
 * Model for log event details.
 *
 */
class LogsMatchesModel : public LogsAbstractModel 
{
    Q_OBJECT
    
public:
    
    explicit LogsMatchesModel( LogsAbstractModel& parentModel,
                               LogsDbConnector& dbConnector );
    
public: // The exported API
  
    LOGSENGINE_EXPORT ~LogsMatchesModel();
    LOGSENGINE_EXPORT void logsMatches(const QString& pattern);
        
public: // From QAbstractItemModel
    
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

protected slots:

    virtual void contactSavingCompleted(bool modified);
    
protected: // From LogsAbstractModel
    
    virtual QVariant createCall(const LogsModelItemContainer& item) const;
    virtual QVariant createMessage(const LogsModelItemContainer& item) const;
    virtual QVariant createContact(const LogsModelItemContainer& item) const;
    virtual int doSetPredictiveSearch(bool enabled);
   
    
private slots:

    void queryReady();
    void updateContactIcon(int index);  

    void eventsUpdated(const QModelIndex& first, const QModelIndex& last);
    void eventsAdded(const QModelIndex& parent, int first, int last);
    void eventsRemoved(const QModelIndex& parent, int first, int last);
    void eventsResetted();
	
    void doSearchQuery();
    void doModelReset();
    void forceSearchQuery();
    void contactUpdated(bool updated);
    
private:

	void initPredictiveSearch();
	
	LogsMatchesModelItemContainer* addSearchResult(int resultIndex);
    bool updateSearchResult(LogsMatchesModelItemContainer& item) const;
    void readEvents(int first, int last);
    void getLogsMatches( const QString& pattern, bool async = false, bool force = false );  
    void updateSearchEntry(LogsCntEntry& entry, LogsEvent& event);
    QString stripPhoneNumber(const QString& phoneNumber) const;
    void addEventForSearching(int index, LogsEvent& event);
    
private: //data 
    
    LogsAbstractModel& mParentModel;
    LogsCntFinder* mLogsCntFinder;
    
    QList<LogsMatchesModelItemContainer*> mMatches;
    QMap<LogsCntEntryHandle*, LogsEvent*> mSearchEvents;
    
    QString mCurrentSearchPattern;
    QString mPrevSearchPattern;
    LogsThumbIconManager   *mIconManager;
    int mResultCount;
    
private:
        
    friend class UT_LogsModel;
    friend class UT_LogsMatchesModel;
    
};

/**
 * Contains log event or contact match
 */
class LogsMatchesModelItemContainer : public LogsModelItemContainer {
public:
    LogsMatchesModelItemContainer(LogsAbstractModel& parentModel,
								  LogsThumbIconManager& mIconManager,
                                  int resultIndex);
    virtual ~LogsMatchesModelItemContainer();
    
    void setEvent(const LogsEvent& event);
    void setContact(unsigned int contactId);
    unsigned int contact() const;
    QString number() const;
    QString contactName() const;
    QString contactNameSimple() const;
    bool isNull() const;
    bool isEventMatch() const;
    QStringList texts();
    QList<QVariant> icons(int row);
    void updateData(const LogsCntEntry& entry);
    int resultIndex() const;
    
private:
    
    QString getFormattedCallerId(const LogsCntEntry& entry) const;
    void getFormattedContactInfo( 
            const LogsCntEntry& entry, QString& contactName, 
            QString& contactNameSimple, QString& contactNumber ) const;
    void getFormattedName(QString& formattedName, const QList<LogsCntText>& list) const;
    void getFormattedName(QString& formattedName, QString& formattedNameSimple,
            const QList<LogsCntText>& list) const;
    
private:

    LogsAbstractModel& mParentModel;
    unsigned int mContactId;
    QString mContactName;
    QString mContactNameSimple;
    QString mContactNumber;
    QString mAvatarPath;
    QString mFormattedCallerId;
	LogsThumbIconManager& mIconManager;
	int mResultIndex;
	
private:	
    
	friend class UT_LogsMatchesModel;
};

#endif //LOGSMATCHESMODEL_H
