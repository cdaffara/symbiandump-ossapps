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
#include "logsmatchesmodel.h"
#include "logsmodel.h"

#include "logsevent.h"
#include "logslogger.h"
#include "logsengdefs.h"
#include "logsdbconnector.h"
#include "logscntfinder.h"
#include "logsdetailsmodel.h"
#include "logscall.h"
#include "logsmessage.h"
#include "logscontact.h"
#include "logseventdata.h"
#include "logsthumbnailmanager.h"
#include "logscommondata.h"
#include <hblineedit.h>
#include <QStringList>
#include <qcontactphonenumber.h>
#include <qcontactmanager.h>

Q_DECLARE_METATYPE(LogsEvent *)
Q_DECLARE_METATYPE(LogsCall *)
Q_DECLARE_METATYPE(LogsMessage *)
Q_DECLARE_METATYPE(LogsContact *)
Q_DECLARE_METATYPE(LogsDetailsModel *)

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsMatchesModel::LogsMatchesModel( 
        LogsAbstractModel& parentModel, LogsDbConnector& dbConnector ) 
    : LogsAbstractModel(),
      mParentModel(parentModel),
      mLogsCntFinder(0),
      mIconManager(0),
      mResultCount(0)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::LogsMatchesModel()" )
    
    mDbConnector = &dbConnector;
    initPredictiveSearch();
    
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::LogsMatchesModel()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsMatchesModel::~LogsMatchesModel()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::~LogsMatchesModel()" )
    
    delete mLogsCntFinder;
    qDeleteAll(mMatches);
	delete mIconManager;
    
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::~LogsMatchesModel()" )
}

// -----------------------------------------------------------------------------
// From QAbstractItemModel
// -----------------------------------------------------------------------------
//
int LogsMatchesModel::rowCount(const QModelIndex & /* parent */) const
{
    return mResultCount;
}

// -----------------------------------------------------------------------------
// From QAbstractItemModel
// -----------------------------------------------------------------------------
//
QVariant LogsMatchesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= mResultCount || index.row() < 0 ) {
        return QVariant();
    }

    LogsMatchesModelItemContainer* item = 0;
    if ( index.row() < mMatches.count() ){
        item = mMatches.at(index.row());
    } else {
        item = const_cast<LogsMatchesModel*>(this)->addSearchResult(index.row());
    }
   
    updateSearchResult(*item);
    if (role == Qt::DisplayRole){
        return( QVariant( item->texts() ) );
    } else if (role == Qt::DecorationRole) {
        return( QVariant( item->icons(index.row()) ) );
    } else if ( role == RoleDetailsModel ) {
        LOGS_QDEBUG( "logs [ENG]    LogsMatchesModel::data() RoleDetailsModel" )
        LogsDetailsModel* detailsModel = 0;
        LogsEvent* event = item->event();
        if ( event ) {
            detailsModel = new LogsDetailsModel( *mDbConnector, *event );
        }
        QVariant var = qVariantFromValue( detailsModel );
        return var;
    }
    return doGetData(role, *item);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::contactSavingCompleted(bool modified)
{
    Q_UNUSED(modified);
    forceSearchQuery();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QVariant LogsMatchesModel::createCall(const LogsModelItemContainer& item) const
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::createCall()" )     
    LogsEvent* event = item.event();
    if ( event ){
        return LogsAbstractModel::createCall(item);
    }
    const LogsMatchesModelItemContainer& matchItem = 
        static_cast<const LogsMatchesModelItemContainer&>( item ); 
    LogsCall* logscall = new LogsCall(matchItem.contact(), matchItem.number());
    QVariant var = qVariantFromValue(logscall);
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::createCall()" )
    return var;         
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QVariant LogsMatchesModel::createMessage(const LogsModelItemContainer& item) const
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::createMessage()" )   
    LogsEvent* event = item.event();
    if ( event ){
        return LogsAbstractModel::createMessage(item);
    }
    const LogsMatchesModelItemContainer& matchItem = 
        static_cast<const LogsMatchesModelItemContainer&>( item ); 
    LogsMessage* logsMessage = new LogsMessage(
        matchItem.contact(), matchItem.number(),matchItem.contactNameSimple());
    if (!logsMessage->isMessagingAllowed()) {
        delete logsMessage;
        logsMessage = 0;
    }
    QVariant var = qVariantFromValue(logsMessage);
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::createMessage()" )
    return var;      
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QVariant LogsMatchesModel::createContact(const LogsModelItemContainer& item) const
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::createContact()" )
    LogsEvent* event = item.event();
    if ( event ){
        return LogsAbstractModel::createContact(item);
    }
    const LogsMatchesModelItemContainer& matchItem = 
        static_cast<const LogsMatchesModelItemContainer&>( item ); 
    LogsContact* logsContact = 
        new LogsContact(matchItem.number(), *mDbConnector, matchItem.contact());
    if ( !logsContact->isContactRequestAllowed() ) {
        delete logsContact;
        logsContact = 0;
    } else {
        connect(logsContact, SIGNAL(saveCompleted(bool)), this, SLOT(contactUpdated(bool)));
        connect(logsContact, SIGNAL(openCompleted(bool)), this, SLOT(contactUpdated(bool)));
    }
    QVariant var = qVariantFromValue(logsContact);
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::createContact()" )
    return var;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsMatchesModel::doSetPredictiveSearch(bool enabled)
{    
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::doSetPredictiveSearch()" )
    int err = LogsCommonData::getInstance().setPredictiveSearch(enabled);    
    if (!err) {
       //in case of search is turned off, getLogMatches will only reset 
       //previous search results
       forceSearchQuery();
    }     
    LOGS_QDEBUG_2( "logs [ENG] <- LogsMatchesModel::doSetPredictiveSearch(), err: ", err )
    return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::queryReady()
{
	LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::queryReady()" );
	
	qDeleteAll(mMatches);
    mMatches.clear();     
    // Just store number of matches at this point, real data is read only
    // once it is asked first time from model. 
    mResultCount = mLogsCntFinder->resultsCount();	
	reset();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::eventsUpdated(const QModelIndex& first, const QModelIndex& last)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::eventsUpdated()" );
    bool updated( false );
    QMap<LogsCntEntryHandle*, LogsEvent*>::const_iterator i;
    for (i = mSearchEvents.constBegin(); i != mSearchEvents.constEnd(); ++i){
        if ( i.value()->index() >= first.row() && i.value()->index() <= last.row() ){
            LogsCntEntry* entry = mLogsCntFinder->getEntry(*i.key());
            if ( entry ){
                updateSearchEntry(*entry, *i.value());
                updated = true;
            }
        }
    }
    
    if ( updated ){
        forceSearchQuery();
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::eventsUpdated()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::eventsAdded(const QModelIndex& parent, int first, int last)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::eventsAdded()" );
    Q_UNUSED(parent);
    readEvents(first, last);
    forceSearchQuery();
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::eventsAdded()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::eventsRemoved(const QModelIndex& parent, int first, int last)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::eventsRemoved()" );
    Q_UNUSED(parent);
    bool removed( false );
    QMap<LogsCntEntryHandle*, LogsEvent*>::iterator i;
    for (i = mSearchEvents.begin(); i != mSearchEvents.end(); ++i){
        if ( i.value()->index() >= first && i.value()->index() <= last &&
             !i.value()->isInView() ){
            mLogsCntFinder->deleteEntry(*i.key());
            i = mSearchEvents.erase(i);
            removed = true;
        }
    }
    
    if ( removed ){
        forceSearchQuery();
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::eventsRemoved()" );
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::eventsResetted()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::eventsResetted()" );
    
    QMap<LogsCntEntryHandle*, LogsEvent*> unusedEvents = mSearchEvents;
    for ( int i = 0; i < mParentModel.rowCount(); ++i ){
        LogsEvent* event = qVariantValue<LogsEvent*>( 
            mParentModel.data( mParentModel.index(i, 0), LogsModel::RoleFullEvent ) );
        if ( event ){
            QObject* key = mSearchEvents.key(event);
            if ( key ){
                unusedEvents.remove(key);
            } else {
                addEventForSearching(i, *event);
            }
        }
    }
    
    QMap<LogsCntEntryHandle*, LogsEvent*>::iterator unusedIt;
    for (unusedIt = unusedEvents.begin(); unusedIt != unusedEvents.end(); ++unusedIt){
        mLogsCntFinder->deleteEntry(*unusedIt.key());
        mSearchEvents.remove(unusedIt.key());
    }
    
    forceSearchQuery();
    
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::eventsResetted()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::initPredictiveSearch()
{
    int searchStatus = LogsCommonData::getInstance().predictiveSearchStatus();
    //searchStatus equal to 0 means that search should be permanently disabled
    if (searchStatus != 0) {
        mLogsCntFinder = new LogsCntFinder(LogsCommonData::getInstance().contactManager(),
                true);
        connect(mLogsCntFinder, SIGNAL(queryReady()),this, SLOT(queryReady()));
        
        connect( &mParentModel, SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)), 
                 this, SLOT(eventsUpdated(const QModelIndex&,const QModelIndex&)));
        connect( &mParentModel, SIGNAL(rowsInserted(const QModelIndex&,int,int)), 
                 this, SLOT(eventsAdded(const QModelIndex&,int,int)));
        connect( &mParentModel, SIGNAL(rowsRemoved(const QModelIndex&,int,int)), 
                 this, SLOT(eventsRemoved(const QModelIndex&,int,int)));
        connect( &mParentModel, SIGNAL(modelReset()), this, SLOT(eventsResetted()));
        readEvents(0, mParentModel.rowCount());
        mIconManager = new LogsThumbIconManager();
        connect(mIconManager, SIGNAL(contactIconReady(int)),
                this, SLOT(updateContactIcon(int)));
    }
}

// -----------------------------------------------------------------------------
// Add result container but don't get real data yet. Get data once it is
// really needed.
// -----------------------------------------------------------------------------
//
LogsMatchesModelItemContainer* LogsMatchesModel::addSearchResult(int resultIndex)
{
    LogsMatchesModelItemContainer* item = new LogsMatchesModelItemContainer(
                    mParentModel, *mIconManager, resultIndex);
    mMatches.append(item);
    return item;
}

// -----------------------------------------------------------------------------
// Fill result container with real data if it does not yet contain it 
// -----------------------------------------------------------------------------
//
bool LogsMatchesModel::updateSearchResult(LogsMatchesModelItemContainer& item) const
{
    bool updated( false );
    int resIndex = item.resultIndex();
    if ( item.isNull() && resIndex >= 0 && mLogsCntFinder 
         && resIndex < mLogsCntFinder->resultsCount() ){
        const LogsCntEntry& result = mLogsCntFinder->resultAt(resIndex);
        if ( result.type() == LogsCntEntry::EntryTypeHistory ){
            LogsEvent* event = mSearchEvents.value(result.handle());
            if ( event ){
                item.setEvent(*event);  
                item.updateData(result);
                updated = true;
            }
        } else {
            item.setContact(result.contactId());     
            item.updateData(result);
            updated = true;
        }
    }
    return updated;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::readEvents(int first, int last)
{
   for ( int i = first; i < mParentModel.rowCount() && i <= last; ++i ){
        LogsEvent* event = qVariantValue<LogsEvent*>( 
                mParentModel.data( mParentModel.index(i, 0), LogsModel::RoleFullEvent ) );
        if ( event ){
            addEventForSearching(i, *event);
        }
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::logsMatches(const QString& pattern)
{
    // Do user inputted searches in async manner to avoid from
    // blocking next input. This also decreases amount of queries when
    // user types fast.
    getLogsMatches(pattern, true);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::getLogsMatches(const QString& pattern, 
                                      bool async, 
                                      bool force )
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsMatchesModel::getLogsMatches(), pattern:", pattern );
    mCurrentSearchPattern = pattern;
    if ( force ){
        // Clear previous results immeditely as some data associated with results
        // might be already gone.
        doModelReset();
        mPrevSearchPattern.clear();
    }
    if ( async ){
        QMetaObject::invokeMethod(this, "doSearchQuery", Qt::QueuedConnection );
    } else {
        doSearchQuery();   
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::getLogsMatches()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::doSearchQuery()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::doSearchQuery()" );
    if (mCurrentSearchPattern != mPrevSearchPattern &&
            LogsCommonData::getInstance().predictiveSearchStatus() == 1) {
        mPrevSearchPattern = mCurrentSearchPattern;
        if (mCurrentSearchPattern.length() > 0) {
            LOGS_QDEBUG( "logs [ENG]    do search" );
            mLogsCntFinder->predictiveSearchQuery( mCurrentSearchPattern );
        } else {
            // Clear old results for empty search pattern
            doModelReset();
        }
    } else {
        LOGS_QDEBUG( "logs [ENG] pattern hasn't changed or search is OFF" );
    }
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::doSearchQuery()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::doModelReset()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::doModelReset()" );
    qDeleteAll(mMatches);
    mMatches.clear();
    mResultCount = 0;
    reset();
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::doModelReset()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::forceSearchQuery()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::forceSearchQuery()" );
    getLogsMatches( mCurrentSearchPattern, true, true ); 
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::forceSearchQuery()" );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::contactUpdated(bool /*updated*/)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::contactUpdated()" )
    // At the moment phonebook doesnt notify properly when contact was updated
    // Let's reset all results (to clear cached contacts) for now, in the
    // future do it only if the contact was changed for real
    mLogsCntFinder->resetResults();
    forceSearchQuery();
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::contactUpdated()" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::updateSearchEntry(LogsCntEntry& entry, LogsEvent& event)
{
    if ( event.remoteParty().length() > 0 ) {
        entry.setFirstName(event.remoteParty());
    } else if ( event.number().length() > 0 ) {
        entry.setPhoneNumber(stripPhoneNumber(event.number()));
    } else if ( event.eventType() == LogsEvent::TypeVoIPCall && event.logsEventData() ){
        if ( event.logsEventData()->isCsCompatible() ){
            entry.setPhoneNumber(stripPhoneNumber(event.logsEventData()->remoteUrl()));
        } else {
            entry.setFirstName(event.logsEventData()->remoteUrl());
        }
    }
}

// -----------------------------------------------------------------------------
//Update the icon for the contact with the given index (row)
//param index of the contact
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::updateContactIcon(int index)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsMatchesModel::updateContactIcon()" );
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex);
    LOGS_QDEBUG( "logs [ENG] <- LogsMatchesModel::updateContactIcon()" );   
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QString LogsMatchesModel::stripPhoneNumber(const QString& phoneNumber) const
{
    if ( phoneNumber.startsWith(QLatin1Char('+')) ){
        return phoneNumber.mid(1);
    }
    return phoneNumber;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModel::addEventForSearching(int index, LogsEvent& event)
{
     QObject* entryHandle = new QObject(this);
     LogsCntEntry* entry = new LogsCntEntry(*entryHandle, 0);
     updateSearchEntry(*entry, event);
     mLogsCntFinder->insertEntry(index, entry);
     mSearchEvents.insert(entryHandle, &event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsMatchesModelItemContainer::LogsMatchesModelItemContainer(
        LogsAbstractModel& parentModel,
		LogsThumbIconManager& thumbIconManager, 
        int resultIndex) : 
    LogsModelItemContainer(),
    mParentModel(parentModel),
    mContactId( 0 ),
	mIconManager(thumbIconManager),
    mResultIndex(resultIndex)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsMatchesModelItemContainer::~LogsMatchesModelItemContainer()
{
    delete mEvent;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModelItemContainer::setEvent(const LogsEvent& event)
{
    delete mEvent;
    mEvent = 0;
    mEvent = new LogsEvent(event);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModelItemContainer::setContact(unsigned int contactId)
{
    mContactId = contactId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
unsigned int LogsMatchesModelItemContainer::contact() const
{
    return mContactId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QString LogsMatchesModelItemContainer::number() const
{
    QString number;
    if ( mEvent ){
        number = mEvent->getNumberForCalling();
    } else if ( mContactId > 0 ) {
        number = mContactNumber;
    }
    return number;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QString LogsMatchesModelItemContainer::contactName() const
{
   return mContactName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QString LogsMatchesModelItemContainer::contactNameSimple() const
{
   return mContactNameSimple;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsMatchesModelItemContainer::isNull() const
{
    return ( !mEvent && !mContactId );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool LogsMatchesModelItemContainer::isEventMatch() const
{
    return ( mEvent != 0 );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QStringList LogsMatchesModelItemContainer::texts()
{
    QStringList list;
    if ( mEvent ){
        list << mFormattedCallerId;
        list << mParentModel.dateAndTimeString( 
                            mEvent->time().toTimeSpec(Qt::LocalTime) );
    } else if ( mContactId > 0 ) {
        list << mContactName;
        list << mParentModel.phoneNumString(mContactNumber);    
    }
    return list;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QList<QVariant> LogsMatchesModelItemContainer::icons(int row)
{
    QList<QVariant> icons;
    if ( mEvent ) {
        mParentModel.getDecorationData(*mEvent, icons);
    } else if ( mContactId > 0 ) {
        QIcon& icon = mIconManager.contactIcon( mAvatarPath, row );    
        icons.append(icon);
    }
    return icons;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModelItemContainer::updateData(const LogsCntEntry& entry)
{
    if ( mEvent ){
        mFormattedCallerId = getFormattedCallerId(entry);
    } else if ( mContactId > 0 ){
        getFormattedContactInfo(entry, mContactName, mContactNameSimple, mContactNumber);
        mAvatarPath.clear();
        mAvatarPath = entry.avatarPath();
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int LogsMatchesModelItemContainer::resultIndex() const
{
    return mResultIndex;
}

// -----------------------------------------------------------------------------
// Note: Mapping of search result entry into caller ID is depended on
// updateSearchEntry() implemention.
// -----------------------------------------------------------------------------
//
QString LogsMatchesModelItemContainer::getFormattedCallerId(
        const LogsCntEntry& entry) const
{    
    QString callerId;
    getFormattedName(callerId, entry.firstName());
    
    if  ( callerId.length() == 0 ) {
        callerId = mParentModel.phoneNumString(entry.phoneNumber().richText());
    }

    return callerId.trimmed();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsMatchesModelItemContainer::getFormattedContactInfo( 
        const LogsCntEntry& entry,
        QString& contactName,
        QString& contactNameSimple,
        QString& contactNumber ) const
{
    contactName.clear();
    
    getFormattedName(contactName, contactNameSimple, entry.firstName());
    getFormattedName(contactName, contactNameSimple, entry.lastName());

    contactName = contactName.trimmed();
    contactNameSimple = contactNameSimple.trimmed();
    contactNumber = entry.phoneNumber().text();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void  LogsMatchesModelItemContainer::getFormattedName(
    QString& formattedName, const QList<LogsCntText>& list) const
{
    foreach( LogsCntText name, list ) {
        if ( name.text().length() > 0 ) {
            formattedName.append(name.richText());   
            formattedName.append(" ");
        }
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void  LogsMatchesModelItemContainer::getFormattedName(
    QString& formattedName, QString& formattedNameSimple,
    const QList<LogsCntText>& list) const
{
    foreach( LogsCntText name, list ) {
        if ( name.text().length() > 0 ) {
            formattedName.append(name.richText());   
            formattedName.append(" ");
            formattedNameSimple.append(name.text());
            formattedNameSimple.append(" ");
        }
    }
}
