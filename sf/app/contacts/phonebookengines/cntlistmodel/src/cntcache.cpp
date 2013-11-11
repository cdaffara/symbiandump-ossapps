/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Class for asynchronously fetching and caching basic
*              contact info (see CntContactInfo) for list views.
*
*/

#include <hbapplication.h>
#include <qtcontacts.h>
#include <qcontactmanager.h>
#include <QTimer>

#include <cntdebug.h>
#include "cntcache.h"
#include "cntnamefetcher.h"
#include "cntcache_p.h"

// set the singleton instance pointer to NULL
CntCache* CntCache::mInstance = NULL;

// value for first cache order to be assigned
static const int CacheOrderStartValue = 1;
// for avoiding wrap around with cache orders
static const int MaxCacheOrderValue = 10000000;
// number of items to read quickly when a new instance is requested or cache is cleared
static const int ItemsToReadUrgently = 13;
// number of items to read ahead into cache; this number is for one direction
static const int ItemsToCacheAhead = 24;
// cache size for info items (name, text, icon1name, icon2name)
static const int InfoCacheSize = 128;
// cache size for icon items (iconName and HbIcon)
static const int IconCacheSize = 50;
// number of icons in a CntContactInfo object
static const int IconsInCntContactInfo = 2;
// default empty text info field for a contact; it cannot be empty
// as the listview will then ignore it, causing rendering problems
static const QString EmptyTextField = " ";

/*!
    Provides a pointer to the CntCache singleton instance.
 */
CntCache* CntCache::instance(QContactManager *manager)
{
    if (!mInstance) {
        mInstance = new CntCache(manager);
    }

    // whenever a client requests an instance the client will want to get all info
    // for the first couple of contacts (~a screenfull) as fast as possible
    mInstance->mUrgentContacts = ItemsToReadUrgently;

    return mInstance;
}

/*! 
    Fetches information about a contact: name, text (e.g. phone number or
    social status) and two icons (e.g. avatar, presence). Previously cached
    content - at the very least the name - will be returned immediately.
    Availability of more information will be checked asynchronously and
    sent to clients via contactInfoUpdated() signals.
      
    The function takes a row and a list rather than just a contact id because
    of read ahead caching - contacts near the requested contacts are expected
    to be needed soon and are therefore also scheduled for caching.

    \param row the row of the contact to fetch
    \param idList a list with all the IDs in the list
    \return a contact with some details filled in
 */
CntContactInfo CntCache::fetchContactInfo(int row, const QList<QContactLocalId>& idList)
{
    CNT_ENTRY_ARGS(row << "/" << idList.count())

    Q_ASSERT(row >= 0 && row < idList.count());

    QString name;
    QString text = EmptyTextField;
    HbIcon icons[IconsInCntContactInfo];

    int contactId = idList.at(row);

    if (contactId != mEmittedContactId) {
        // this request comes from the UI when a new view is created or in response to
        // some scrolling activity; in the former case, the client should
        // have set urgencymode on, but in the latter case:
        // 1) postpone all jobs so the UI can use as much of the CPU as possible
        // 2) update read ahead cache to contain all IDs of all items near this item
        if (mUrgentContacts > 0) {
            --mUrgentContacts;
        } else {
            mWorker->postponeJobs(150);
        }
        updateReadAheadCache(row, idList);
    }

    // fetch contact
    if (mInfoCache.contains(contactId)) {
        // the item is in the cache
        CntInfoCacheItem* infoItem = mInfoCache.value(contactId);
        for (int i = 0; i < IconsInCntContactInfo; ++i) {
            QString iconName = infoItem->icons[i];
            if (!iconName.isEmpty()) {
                if (mIconCache.contains(iconName)) {
                    CntIconCacheItem* iconItem = mIconCache.value(iconName);
                    iconItem->cacheOrder = mNextIconCacheOrder++;
                    icons[i] = iconItem->icon;
                    if (!iconItem->isFetched) {
                        // if icon has not yet been received from backend, add
                        // this id to the list of contacts that want to be
                        // notified when the icon is received
                        iconItem->contactIds.insert(contactId);
                        // also reschedule it
                        mWorker->scheduleIconJob(iconName, row);
                    }
                } else {
                    // needed icon is not in cache, so schedule it for retrieval
                    CntIconCacheItem* iconItem = createIconCacheItem(iconName);
                    iconItem->contactIds.insert(contactId);
                    mWorker->scheduleIconJob(iconName, row);
                }
            }
        }

        // set return text
        text = infoItem->text;

        // update cache order
        infoItem->cacheOrder = mNextInfoCacheOrder++;
        infoItem->latestRow = row;
    } else {
        // the contact info is not in cache, schedule it for retrieval
        if (contactExists(contactId)) {
            // contact found, so add new entry to cache
            CntInfoCacheItem* item = createInfoCacheItem(contactId);
            item->text = text;
            item->latestRow = row;

            // ask the worker thread to fetch the information asynchronously
            mWorker->scheduleInfoJob(contactId, row);
        }
    }

    name = contactName(contactId);
    CNT_EXIT_ARGS("name:" << name << "sec:" << text)

    return CntContactInfo(contactId, name, text, icons[0], icons[1]);
}

/*! 
    Creates a list of contact ids sorted according the corresponding contact names.

    \param idFilter the IDs to be returned; if NULL, all contact IDs are returned
    \return the list of ids, sorted according the contact name
 */
QList<QContactLocalId> CntCache::sortIdsByName(const QSet<QContactLocalId>* idFilter) const
{
    CNT_ENTRY

    QList<QContactLocalId> sortedIds;
    
    // allocate memory in advance to avoid repeated reallocation during population
    // an extra 16 items are allocated to leave room for a few more contacts
    // before reallocation is needed
    if (!idFilter) {
        sortedIds.reserve(mSortedNames.count() + 16);
    } else {
        sortedIds.reserve(idFilter->count() + 16);
    }

    // the entries in mSortedNames are already sorted, so just pick
    // out the ids from that list in the order that they appear
    if (!idFilter) {
        foreach (CntNameCacheItem* item, mSortedNames) {
            sortedIds.append(item->contactId());
        }
    } else {
        foreach (CntNameCacheItem* item, mSortedNames) {
            if (idFilter->contains(item->contactId())) {
                sortedIds.append(item->contactId());
            }
        }
    }

    CNT_EXIT

    return sortedIds;
}

/*!
    Overloaded version of the function for string based searching of contact names.
    Currently for multi part names only space and dash variations are used for filtering,
    e.g. "Axx Bxx" or "Axx-Bxx" are the only possible matches along with the original string. 
    
    \param searchList list of strings which are used for search
    \return the list of ids, sorted according the contact name
 */
QList<QContactLocalId> CntCache::sortIdsByName(const QStringList searchList) const
{
    CNT_ENTRY
    
    QList<QContactLocalId> sortedIds;
    int iterNames = 0;
    int iterList = 0;
    QString firstName = 0;
    QString lastName = 0;
    QString tempString = 0;
    QString tempDash = 0;
    QString tempSpace = 0;
    int matchesFound = 0;
    const QChar dash = '-';
    const QChar space = ' ';
    QStringList searchVariations;
    
    for (iterList = 0; iterList < searchList.size(); iterList++)
    {
        tempString = searchList.at(iterList);
        tempDash = tempString;
        tempSpace = tempString;
        tempDash.insert(0, dash);
        tempSpace.insert(0, space);
        
        searchVariations.append(tempString);
        searchVariations.append(tempDash);
        searchVariations.append(tempSpace);
    }
    
    for (iterNames = 0; iterNames < mSortedNames.size(); iterNames++)
    {
        matchesFound = 0;
        firstName = (mSortedNames.at(iterNames))->firstName();
        lastName = (mSortedNames.at(iterNames))->lastName();
        for (iterList = 0; iterList < searchVariations.size(); iterList += 3)
        {
            // if the current name doesn't contain any of the possible variations then it can be skipped
            if ( !( firstName.startsWith(searchVariations.at(iterList), Qt::CaseInsensitive) ||
                    lastName.startsWith(searchVariations.at(iterList), Qt::CaseInsensitive) ||
                    firstName.contains(searchVariations.at(iterList+1), Qt::CaseInsensitive) ||
                    lastName.contains(searchVariations.at(iterList+1), Qt::CaseInsensitive) ||
                    firstName.contains(searchVariations.at(iterList+2), Qt::CaseInsensitive) ||
                    lastName.contains(searchVariations.at(iterList+2), Qt::CaseInsensitive) ) )
            {
                break;
            }
        }
        if (iterList == searchVariations.size())
        {
            sortedIds.append(mSortedNames.at(iterNames)->contactId());
        }
    }
    
    CNT_EXIT

    return sortedIds;
}

/*!
    Creates the CntCache singleton instance.
 */
CntCache::CntCache(QContactManager *manager)
    : mContactManager(manager),
      mWorker(new CntCacheThread()),
      mNameFetcher(new CntNameFetcher()),
      mNextInfoCacheOrder(CacheOrderStartValue),
      mNextIconCacheOrder(CacheOrderStartValue),
      mEmittedContactId(-1),
      mUrgentContacts(0),
      mHasModifiedNames(false),
      mAllNamesFetchStarted(false)
{
    CNT_ENTRY

    // listen to name fetcher
    connect(mNameFetcher, SIGNAL(nameFormatChanged(CntNameOrder)), this, SLOT(reformatNames(CntNameOrder)));
    connect(mNameFetcher, SIGNAL(databaseAccessComplete()), mWorker, SLOT(resumeJobs()));
    connect(mNameFetcher, SIGNAL(namesAvailable(QList<CntNameCacheItem *>)), this, SLOT(setNameList(QList<CntNameCacheItem *>)));

    // listen to info fetcher
    connect(mWorker, SIGNAL(infoFieldUpdated(int, const ContactInfoField&, const QString&)),
            this, SLOT(onNewInfo(int, const ContactInfoField&, const QString&)));
    connect(mWorker, SIGNAL(infoCancelled(int)), this, SLOT(onInfoCancelled(int)));

    // listen to icon fetcher
    connect(mWorker, SIGNAL(iconUpdated(const QString&, const HbIcon&)),
            this, SLOT(onNewIcon(const QString&, const HbIcon&)));
    connect(mWorker, SIGNAL(iconCancelled(const QString&)), this, SLOT(onIconCancelled(const QString&)));
    connect(mWorker, SIGNAL(allJobsDone()), this, SLOT(scheduleOneReadAheadItem()));

    // listen to contact manager
    connect(mContactManager, SIGNAL(contactsChanged(const QList<QContactLocalId>&)), this, SLOT(updateContacts(const QList<QContactLocalId>&)));
    connect(mContactManager, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), this, SLOT(removeContacts(const QList<QContactLocalId>&)));
    connect(mContactManager, SIGNAL(contactsAdded(const QList<QContactLocalId>&)), this, SLOT(addContacts(const QList<QContactLocalId>&)));

    // listen to application -- shut down cache only when the whole application quits
    connect(HbApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(onShutdown()));

    // load all names to RAM
    loadNames();

    CNT_EXIT
}

/*!
    Destructs the CntCache singleton instance.
 */
CntCache::~CntCache()
{
    CNT_ENTRY

    if (mHasModifiedNames) {
        mNameFetcher->writeNamesToCache(mSortedNames);
    }

    delete mWorker;
    delete mNameFetcher;
    
    qDeleteAll(mInfoCache);
    mInfoCache.clear();

    qDeleteAll(mIconCache);
    mIconCache.clear();

    qDeleteAll(mNameCache);
    mNameCache.clear();
    mSortedNames.clear();

    CNT_EXIT
}

/*! 
    Processes a new info field that has arrived from the worker thread.
    If the contact is in the info cache, then the info cache is updated
    accordingly.
    
    A contactInfoUpdated() signal is usually also emitted. The exception
    is if the info is the name of an icon and that icon is not in the icon
    cache. In this case the icon is fetched before a signal is emitted.
 */
void CntCache::onNewInfo(int contactId, const ContactInfoField& infoField, const QString& infoValue)
{
    CNT_ENTRY_ARGS( "id:" << contactId   << "infotype:" << infoField   << "infovalue:" << infoValue )

    Q_ASSERT(infoField == ContactInfoTextField || infoField == ContactInfoIcon1Field || infoField == ContactInfoIcon2Field);

    bool hasNewInfo;

    if (!mInfoCache.contains(contactId)) {
        // contact is not in cache, so nothing needs to be done
        // except notify clients that this contact has (possibly)
        // been changed
        hasNewInfo = true;
    }
    else if (infoField == ContactInfoTextField) {
        // update cache with new text for contact
        mInfoCache.value(contactId)->text = infoValue;
        hasNewInfo = true;
    }
    else {
        // update cache with new icon name for contact
        int iconIndex = (infoField == ContactInfoIcon1Field ? 0 : 1);

        CntInfoCacheItem* item = mInfoCache.value(contactId);
        QString iconName = infoValue;
        if (item->icons[iconIndex] != iconName) {
            item->icons[iconIndex] = iconName;
            if (iconName.isEmpty()) {
                hasNewInfo = true;
            }
            else if (mIconCache.contains(iconName)) {
                CntIconCacheItem* iconItem = mIconCache.value(iconName);
                if (!iconItem->isFetched) {
                    iconItem->contactIds.insert(contactId);
                    hasNewInfo = false;
                }
                else {
                    hasNewInfo = true;
                }
            }
            else if (iconName.startsWith("qtg_", Qt::CaseInsensitive)) {
                createIconCacheItem(iconName);
                onNewIcon(iconName, HbIcon(iconName)); 
                hasNewInfo = true;
            }
            else {
                CntIconCacheItem* iconItem = createIconCacheItem(iconName);
                iconItem->contactIds.insert(contactId);
				if (mInfoCache.contains(contactId)) {
                	mWorker->scheduleIconJob(iconName, mInfoCache.value(contactId)->latestRow);
				}
				else {
					// less important icon, since this contact is not in cache
                	mWorker->scheduleIconJob(iconName, 100000);
				}
                hasNewInfo = false;
            }
        }
        else {
            hasNewInfo = false;
        }
    }

    if (hasNewInfo) {
        emitContactInfoUpdated(contactId);
    }

    CNT_EXIT
}

/*! 
    Handle the case where a request for contact info is cancelled by the
    worker because of too many subsequent requests.
 */
void CntCache::onInfoCancelled(int contactId)
{
    CNT_ENTRY_ARGS( "id:" << contactId )

    if (mInfoCache.contains(contactId)) {
        CntInfoCacheItem* item = mInfoCache.take(contactId);
        delete item;
    }

    emitContactInfoUpdated(contactId);

    CNT_EXIT
}

/*! 
    Processes a new icon that has arrived from the worker thread.
    The icon cache is updated and a contactInfoUpdated() signal is
    emitted for all contacts that use this icon.
 */
void CntCache::onNewIcon(const QString& iconName, const HbIcon& icon)
{
    CNT_ENTRY_ARGS( iconName )

    QSet<int> contactsToNotify;

    if (mIconCache.contains(iconName)) {
        CntIconCacheItem* item = mIconCache.value(iconName);
        item->icon = icon;
        item->isFetched = true;
        contactsToNotify = item->contactIds;
        item->contactIds.clear();
    }

    foreach (int contactId, contactsToNotify) {
        emitContactInfoUpdated(contactId);
    }

    CNT_EXIT
}

/*! 
    Handle the case where a request for an icon is cancelled by the worker because
    of too many subsequent requests.
 */
void CntCache::onIconCancelled(const QString& iconName)
{
    CNT_ENTRY_ARGS( iconName )

    QSet<int> contactsToNotify;

    if (mIconCache.contains(iconName)) {
        CntIconCacheItem* item = mIconCache.take(iconName);
        contactsToNotify = item->contactIds;
        item->contactIds.clear();
        delete item;
    }

    foreach (int contactId, contactsToNotify) {
        emitContactInfoUpdated(contactId);
    }

    CNT_EXIT
}

/*!
    Fetch the names of all contacts.
 */
void CntCache::loadNames()
{
    CNT_ENTRY
    
    // read names from file cache
    mNameFetcher->readNamesFromCache(mSortedNames);

    // insert the names into the id-to-name map
    foreach (CntNameCacheItem* item, mSortedNames) {
        mNameCache.insert(item->contactId(), item);
    }

    // if there are no names in file cache, start the asynch
    // read of all names immediately (normally it is done
    // after secondary info has been read)
    if (mSortedNames.count() == 0) {
        mWorker->postponeJobs();
        mAllNamesFetchStarted = true;
        mNameFetcher->readAllNamesAsynch();
    }

    CNT_EXIT
}

/*!
    Checks whether a contact exists.
 */
bool CntCache::contactExists(QContactLocalId contactId) const
{
    return mNameCache.contains(contactId);
}

/*!
    Fetch the name of one contact.
 */
QString CntCache::contactName(QContactLocalId contactId) const
{
    CNT_ENTRY

    QString name;

    QHash<QContactLocalId, CntNameCacheItem*>::const_iterator i = mNameCache.find(contactId);
    if (i != mNameCache.end()) {
        name = i.value()->name();
    }

    CNT_EXIT

    return name;
}

/*! 
    Collects all contact IDs near the latest fetch from the UI. These will be fetched and
    precached when UI activity slows down.

    \param mostRecentRow the row of the contact that was most recently fetched
    \param idList a list with all the IDs in the list
 */
void CntCache::updateReadAheadCache(int mostRecentRow, const QList<QContactLocalId>& idList)
{
    CNT_ENTRY_ARGS( mostRecentRow )

    int row;

    mReadAheadCache.clear();
    
    // step through the area near to last fetch item and make sure all
    // contacts in it are also in cache or in the read ahead list
    for (int i = 1; i <= ItemsToCacheAhead; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (j == 0) {
                row = mostRecentRow - i;
                if (row <= 0) {
                    continue;
                }
            }
            else {
                row = mostRecentRow + i;
                if (row >= idList.count()) {
                    continue;
                }
            }
            
            int contactId = idList.at(row);
            if (!mInfoCache.contains(contactId)) {
                // contact is not in cache, so put the id to items to read into cache
                mReadAheadCache.append(QPair<int,int>(contactId,row));
            }
            else {
                // contact is in cache; update cache order as we want to keep this item in cache
                mInfoCache.value(contactId)->cacheOrder = mNextInfoCacheOrder++;
            }
        }
    }

   CNT_EXIT
}

/*! 
    Schedules one uncached item in the read-ahead list for retrieval.
 */
void CntCache::scheduleOneReadAheadItem()
{
    CNT_ENTRY

    QString name;

    // fetch all names from the database if it hasn't been done yet
    if (!mAllNamesFetchStarted) {
        mWorker->postponeJobs();
        mAllNamesFetchStarted = true;
        mNameFetcher->readAllNamesAsynch();
    }

    // pick the first contact from the read ahead cache and schedule it
    while (mReadAheadCache.count() > 0) {
        int contactId = mReadAheadCache.first().first;
        int contactRow = mReadAheadCache.takeFirst().second;
        if (!mInfoCache.contains(contactId)) {
            // contact is not in cache, so schedule it for retreival
            if (contactExists(contactId)) {
                // contact found, so add new entry to cache
                CntInfoCacheItem* item = createInfoCacheItem(contactId);
                item->text = EmptyTextField;
                item->latestRow = contactRow;
    
                // schedule the info
                mWorker->scheduleInfoJob(contactId, contactRow);
                break;
            }
        }
    }

    CNT_EXIT
}

/*! 
    Creates a new item in the info cache. If the cache is full,
    then the least recently accessed item is removed from cache.
    
    /param contactId id of contact for which to create the new cache item
    /return the newly created cache item
 */
CntInfoCacheItem* CntCache::createInfoCacheItem(int contactId)
{
    CNT_ENTRY_ARGS( contactId )

    if (mInfoCache.count() >= InfoCacheSize) {
        // cache is full, so remove the oldest contact
        int minCacheOrder = mNextInfoCacheOrder;
        CntInfoCacheItem* oldestItem = NULL;
        foreach (CntInfoCacheItem* i, mInfoCache) {
            if (i->cacheOrder < minCacheOrder) {
                minCacheOrder = i->cacheOrder;
                oldestItem = i;
            }
        }
        
        if (oldestItem) {
            mInfoCache.remove(oldestItem->contactId);
            delete oldestItem;
        }
        
        // cache maintenance: if the cache ids become too large,
        // reduce all of them by MaxCacheOrderValue
        if (mNextInfoCacheOrder >= MaxCacheOrderValue) {
            mNextInfoCacheOrder -=  MaxCacheOrderValue;
            foreach (CntInfoCacheItem* i, mInfoCache) {
                i->cacheOrder -= MaxCacheOrderValue;
            }
        }
    }
    
    // create and insert the new item
    CntInfoCacheItem* item = new CntInfoCacheItem();
    item->cacheOrder = mNextInfoCacheOrder++;
    item->contactId = contactId;
    mInfoCache.insert(contactId, item);
    
    CNT_EXIT

    return item;
}

/*! 
    Creates a new item in the icon cache. If the cache is full,
    then the least recently accessed item is removed from cache.
    
    /param iconName name of the icon for which to create the new cache item
    /return the newly created cache item
 */
CntIconCacheItem* CntCache::createIconCacheItem(const QString& iconName)
{
    CNT_ENTRY_ARGS( iconName )

    if (mIconCache.count() >= IconCacheSize) {
        // cache is full, so remove the oldest icon
        int minCacheOrder = mNextIconCacheOrder;
        CntIconCacheItem* oldestItem = NULL;
        foreach (CntIconCacheItem* i, mIconCache) {
            if (i->cacheOrder < minCacheOrder) {
                minCacheOrder = i->cacheOrder;
                oldestItem = i;
            }
        }
        mIconCache.remove(oldestItem->iconName);
        delete oldestItem;

        // cache maintenance: if the cache orders become too large,
        // reduce all of them by MaxCacheOrderValue
        if (mNextIconCacheOrder >= MaxCacheOrderValue) {
            mNextIconCacheOrder -=  MaxCacheOrderValue;
            foreach (CntIconCacheItem* i, mIconCache) {
                i->cacheOrder -=  MaxCacheOrderValue;
            }
        }
    }

    // create and insert the new item
    CntIconCacheItem* item = new CntIconCacheItem();
    item->cacheOrder = mNextIconCacheOrder++;
    item->iconName = iconName;
    item->isFetched = false;
    mIconCache.insert(iconName, item);

    CNT_EXIT

    return item;
}

/*! 
    Notifies clients that a contact might have changed.
    Clients can then request the info via fetchContactInfo() 
    if they are interested.
 */
void CntCache::emitContactInfoUpdated(int contactId)
{
	CNT_ENTRY_ARGS( contactId )

    mEmittedContactId = contactId;
    emit contactInfoUpdated(contactId);
    mEmittedContactId = -1;

	CNT_EXIT
}

/*! 
    Deletes the cache.
 */
void CntCache::onShutdown()
{
	CNT_ENTRY

    mInstance = NULL;

    disconnect(mContactManager, SIGNAL(contactsChanged(const QList<QContactLocalId>&)), this, SLOT(updateContacts(const QList<QContactLocalId>&)));
    disconnect(mContactManager, SIGNAL(contactsRemoved(const QList<QContactLocalId>&)), this, SLOT(removeContacts(const QList<QContactLocalId>&)));
    disconnect(mContactManager, SIGNAL(contactsAdded(const QList<QContactLocalId>&)), this, SLOT(addContacts(const QList<QContactLocalId>&)));

    deleteLater();

	CNT_EXIT
}

/*! 
    Updates the names in cache according to newFormat.

    This slot is called when name fetcher signals that the format of
    names has been changed.
 */
void CntCache::reformatNames(CntNameOrder newFormat)
{
    foreach (CntNameCacheItem* item, mSortedNames) {
        item->setNameFormat(newFormat);
    }

    mNameFetcher->sortNames(mSortedNames);

    mNameFetcher->writeNamesToCache(mSortedNames);
    mHasModifiedNames = false;

    emit dataChanged();
}

/*! 
    Replaces the names in cache with the ones in this list.
    
    \param newSortedNames the sorted list with names; this list will be cleared and
                          ownership will be taken of the items in the list
 */
void CntCache::setNameList(QList<CntNameCacheItem *> newSortedNames)
{
    CNT_ENTRY
    
    bool hasModifiedContacts = false;
    int count = newSortedNames.count();

    // check if there have been any changes
    if (mSortedNames.count() != count) {
        hasModifiedContacts = true;
    } else {
        for (int i = 0; i < count; ++i) {
            CntNameCacheItem *oldItem = mSortedNames.at(i);
            CntNameCacheItem *newItem = newSortedNames.at(i);
            if (oldItem->contactId() != newItem->contactId() || oldItem->name() != newItem->name()) {
                hasModifiedContacts = true;
                break;
            }
        }
    }

    // the list has changed, so use the new list instead
    if (hasModifiedContacts) {
        qDeleteAll(mSortedNames);
        mNameCache.clear();
        mSortedNames.clear();
        
        foreach (CntNameCacheItem* item, newSortedNames) {
            mSortedNames.append(item);
            mNameCache.insert(item->contactId(), item);
        }
        
        // write names to file cache
        mNameFetcher->writeNamesToCache(mSortedNames);
        
        // notify clients that the list of names has changed
        emit dataChanged();
    } else {
        qDeleteAll(newSortedNames);
    }
    
    CNT_EXIT
}

/*! 
    Updates data in response to some contacts having changed and
    then notifies observers that these contacts have changed.
 */
void CntCache::updateContacts(const QList<QContactLocalId> &changedContacts)
{
    QString name;
    QList<CntNameCacheItem*> items;

    // reloads the names of the changed contacts and updates the
    // list of sorted names accordingly
    foreach (QContactLocalId contactId, changedContacts) {
        CntNameCacheItem *newItem = mNameFetcher->readOneName(contactId);
        if (newItem != NULL) {
            CntNameCacheItem *oldItem = mNameCache.value(contactId);
            if (oldItem->name() != newItem->name()) {
                QList<CntNameCacheItem*>::iterator oldPos = qLowerBound(mSortedNames.begin(), mSortedNames.end(), oldItem, CntNameFetcher::compareNames);
                while (*oldPos != oldItem && oldPos != mSortedNames.end()) {
                     ++oldPos;
                }
                QList<CntNameCacheItem*>::iterator newPos = qUpperBound(mSortedNames.begin(), mSortedNames.end(), newItem, CntNameFetcher::compareNames);
                if (oldPos < newPos) {
                    mSortedNames.move(oldPos - mSortedNames.begin(), (newPos - mSortedNames.begin()) - 1);
                } else {
                    mSortedNames.move(oldPos - mSortedNames.begin(), newPos - mSortedNames.begin());
                }
                *oldItem = *newItem;
                mHasModifiedNames = true;
            }
        }
    }

    // if any of the changed items have cached info, the info
    // is scheduled for refreshing
    foreach (QContactLocalId contactId, changedContacts) {
        if (mInfoCache.contains(contactId)) {
            CntInfoCacheItem* infoItem = mInfoCache.value(contactId);
            mWorker->scheduleInfoJob(contactId, infoItem->latestRow);
        }
    }

    // inform clients about these changes
    emit contactsChanged(changedContacts);
}

/*! 
    Updates data in response to some contacts having been removed
    and then notifies observers that the contacts have been removed.
 */
void CntCache::removeContacts(const QList<QContactLocalId> &removedContacts)
{
    // removed the deleted contacts from the name cache and from the
    // list of sorted names
    foreach (QContactLocalId contactId, removedContacts) {
        CntNameCacheItem *item = mNameCache.take(contactId);
        if (item) {
            QList<CntNameCacheItem*>::iterator pos = qLowerBound(mSortedNames.begin(), mSortedNames.end(), item, CntNameFetcher::compareNames);
            while (*pos != item && pos != mSortedNames.end()) {
                ++pos;
            }
            mSortedNames.erase(pos);
            delete item;
            mHasModifiedNames = true;
        }
    }

    // info for these deleted items should be removed from cache
    foreach (QContactLocalId contactId, removedContacts) {
        if (mInfoCache.contains(contactId)) {
            CntInfoCacheItem* item = mInfoCache.take(contactId);
            delete item;
        }
    }

    // inform clients about these deleted contacts
    emit contactsRemoved(removedContacts);
}

/*! 
    Updates data in response to some contacts having been added
    and then notifies observers that the contacts have been added.
 */
void CntCache::addContacts(const QList<QContactLocalId> &addedContacts)
{
    // add the new contacts to the name cache and to the
    // list of sorted names
    foreach (QContactLocalId contactId, addedContacts) {
        CntNameCacheItem *item = mNameFetcher->readOneName(contactId);
        if (item != NULL) {
            mNameCache.insert(contactId, item);
            QList<CntNameCacheItem*>::iterator i = qUpperBound(mSortedNames.begin(), mSortedNames.end(), item, CntNameFetcher::compareNames);
            mSortedNames.insert(i, item);
            mHasModifiedNames = true;
        }
    }

    // inform clients about the new contacts
    emit contactsAdded(addedContacts);
}

/*! 
    Creates an empty CntContactInfo object.
 */
CntContactInfo::CntContactInfo()
    : d(new CntContactInfoData())
{
}

/*! 
    Creates a CntContactInfo object with all info fields set.
 */
CntContactInfo::CntContactInfo(int id, const QString& name, const QString& text, const HbIcon& icon1, const HbIcon& icon2)
    : d(new CntContactInfoData())
{
      d->id = id;
      d->name = name;
      d->text = text;
      d->icon1 = icon1;
      d->icon2 = icon2;
}

/*! 
    Destroys the object.
 */
CntContactInfo::~CntContactInfo()
{
}

/*! 
    Copy constructor.
 */
CntContactInfo::CntContactInfo(const CntContactInfo& other)
    : d(other.d)
{
}

/*! 
    Assignment operator.
 */
CntContactInfo& CntContactInfo::operator=(const CntContactInfo& other)
{
    d = other.d;
    return *this;
}

/*! 
    Getter function for the id.
 */
int CntContactInfo::id() const
{
    return d->id;
}

/*! 
    Getter function for the name.
 */
QString CntContactInfo::name() const
{
    return d->name;
}

/*! 
    Getter function for the text.
 */
QString CntContactInfo::text() const
{
    return d->text;
}

/*! 
    Getter function for the first icon.
 */
HbIcon CntContactInfo::icon1() const
{
    return d->icon1;
}

/*! 
    Getter function for the second icon.
 */
HbIcon CntContactInfo::icon2() const
{
    return d->icon2;
}
