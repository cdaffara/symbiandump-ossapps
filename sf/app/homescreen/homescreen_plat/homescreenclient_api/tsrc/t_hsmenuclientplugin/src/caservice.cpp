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
 * Description: caservice.cpp
 *
 */

#include <QMetaType>
#include <QCoreApplication>
#include <QString>
#include <QDebug>

#include <caservice.h>
#include <caservice_p.h>
#include <caquery.h>
#include <caentry.h>
#include <canotifier.h>
#include <cadefs.h>

#include "hsapp_defs.h"
#include "t_hsmenuclientplugin.h"
#include "hsmenuclient.h"


// ======== MEMBER FUNCTIONS ========

/*!

 \class CaService.
 \brief This class operates on data, anable creating and inserting new entry
 to DB, removing entry from DB, update entry or get entry from DB, executeC
 command on entry and create notifier to notify client about changes onto DB.

 CaService class uses singleton design pattern, so that it contains static
 method called instance() to get instance of a class.

 \code
 QSharedPointer<CaService> service = CaService::instance();
 \endcode

 For every operations on data is used always one instantiation of a class.
 Below are examples how to create data and work on those ones.

 */

// Initialization of a static member variable.
//QWeakPointer<CaService> CaService::m_instance = QWeakPointer<CaService>();

/*!
 Constructor.
 \param parent pointer to a parent. It defaults to NULL.
 */
CaService::CaService(QObject *parent) :
    QObject(parent), m_d(&CaServicePrivate::instance())
{

}

/*!
 Returns a pointer to the instance of the CaService class.
 \retval A pointer to the instance of the CaService class.
 */
QSharedPointer<CaService> CaService::instance()
{
    static QSharedPointer<CaService> instance (new CaService);
    return instance;
}

/*!
 Destructor.
 */
CaService::~CaService()
{

}

// HELP METHODS

void CaServicePrivate::clearAll()
{
    mEntryToRemove = -1;

    mUpdatedEntry.clear();

    delete mQueryPassedToGetList;
    mQueryPassedToGetList = NULL;

    mCreatedEntries.clear();
    mReturnedEntries.clear();
}

void CaServicePrivate::AddEntriesForUpdate()
{
    QSharedPointer<CaEntry> item0 (new CaEntry());

    // this will be updated:
    QSharedPointer<CaEntry> item1 (new CaEntry());
    item1->setText("ModuleTestTApp");
    item1->setDescription("Unit test app for HsClientPlugin");

    item1->setAttribute(hsitemLaunchUri, "appto://2002DCEC?");
    item1->setAttribute(hsitemPublisherId, "tappModuleTest");
    item1->setAttribute(hsitemLaunchUri, "hsclockwidgetplugin");

    item1->setEntryTypeName(Hs::templatedApplicationTypeName);
    item1->setFlags(VisibleEntryFlag);

    CaIconDescription iconDescription1;
    iconDescription1.setFilename("iconFileName");
    iconDescription1.setSkinId("iconSkinId");
    iconDescription1.setApplicationId("268458321"); //Bluetouth app UID
    item1->setIconDescription(iconDescription1);

    mCreatedEntries << item0;
    mCreatedEntries << item1;
}
QSharedPointer<CaEntry> CaService::getEntry(int entryId) const
{
    return m_d->mCreatedEntries.at(entryId);
}


bool CaService::updateEntry(const CaEntry &entry) const
{
    m_d->mUpdatedEntry.clear();
    m_d->mUpdatedEntry = QSharedPointer<CaEntry>(new CaEntry(entry));
    return true;
}

QSharedPointer<CaEntry> CaService::createEntry(const CaEntry &entry) const
{
    QSharedPointer<CaEntry> newEntry(new CaEntry(entry));
    m_d->mCreatedEntries << newEntry;
    return newEntry;
}


bool CaService::removeEntry(int entryId) const
{
    m_d->mEntryToRemove = entryId;

    return true;
}


QList< QSharedPointer<CaEntry> > CaService::getEntries(
        const QList<int> &entryIdList) const
{
    Q_UNUSED(entryIdList);
    QList< QSharedPointer<CaEntry> > result;
    return result;

}

QList< QSharedPointer<CaEntry> > CaService::getEntries(const CaQuery &query) const
{
    // store the query for verification
    m_d->mQueryPassedToGetList = new CaQuery (query);

    // return mock list of 2 entires
    QSharedPointer<CaEntry> item1 (new CaEntry());
    item1->setText("ModuleTestTApp1");
    item1->setDescription("Unit test app for HsClientPlugin1");
    item1->setAttribute(hsitemLaunchUri, "appto://1002DCEC?");
    item1->setAttribute(hsitemPublisherId, "tappModuleTest1");
    item1->setAttribute(hsitemLaunchUri, "hsclockwidgetplugin1");
    item1->setEntryTypeName(Hs::templatedApplicationTypeName);
    item1->setFlags(VisibleEntryFlag);
    CaIconDescription iconDescription1;
    iconDescription1.setFilename("iconFileName1");
    iconDescription1.setSkinId("iconSkinId1");
    iconDescription1.setApplicationId("168458321");
    item1->setIconDescription(iconDescription1);

    QSharedPointer<CaEntry> item2 (new CaEntry());
    item2->setText("ModuleTestTApp2");
    item2->setDescription("Unit test app for HsClientPlugin2");
    item2->setAttribute(hsitemLaunchUri, "appto://2002DCEC?");
    item2->setAttribute(hsitemPublisherId, "tappModuleTest2");
    item2->setAttribute(hsitemLaunchUri, "hsclockwidgetplugin2");
    item2->setEntryTypeName(Hs::templatedApplicationTypeName);
    item2->setFlags(VisibleEntryFlag);
    CaIconDescription iconDescription2;
    iconDescription2.setFilename("iconFileName2");
    iconDescription2.setSkinId("iconSkinId2");
    iconDescription2.setApplicationId("268458321");
    item2->setIconDescription(iconDescription2);

    // store items for verification
    m_d->mReturnedEntries << item1 << item2;

    // return copies to caller
    QList< QSharedPointer<CaEntry> > resultList;
    resultList << item1;
    resultList << item2;

    return resultList;
}

QList<int> CaService::getEntryIds(const CaQuery &query) const
{
    Q_UNUSED(query);
    QList<int> result;
    return result;

}



bool CaService::removeEntry(const CaEntry &entry) const
{
    return removeEntry(entry.id());
}


bool CaService::removeEntries(const QList<int> &entryIdList) const
{
    Q_UNUSED(entryIdList);
    return true;
}


bool CaService::removeEntries(
        const QList< QSharedPointer<CaEntry> > &entryList) const
{
    Q_UNUSED(entryList);
    return true;
}

bool CaService::touch(const CaEntry &entry) const
{
    Q_UNUSED(entry);
    return true;
}


bool CaService::insertEntryIntoGroup(int groupId, int entryId,
                                     int beforeEntryId) const
{
    Q_UNUSED(groupId);
    Q_UNUSED(entryId);
    Q_UNUSED(beforeEntryId);
    return true;
}


bool CaService::insertEntryIntoGroup(
        const CaEntry &group, const CaEntry &entry, int beforeEntryId) const
{
    Q_UNUSED(group);
    Q_UNUSED(entry);
    Q_UNUSED(beforeEntryId);
    return true;
}


bool CaService::insertEntriesIntoGroup(int groupId,
       const QList<int> &entryIdList, int beforeEntryId) const
{
    Q_UNUSED(groupId);
    Q_UNUSED(entryIdList);
    Q_UNUSED(beforeEntryId);
    return true;
}


bool CaService::insertEntriesIntoGroup(const CaEntry &group,
       const QList< QSharedPointer<CaEntry> > &entryList,
       int beforeEntryId) const
{
    Q_UNUSED(group);
    Q_UNUSED(entryList);
    Q_UNUSED(beforeEntryId);
    return true;
}


bool CaService::removeEntryFromGroup(int groupId, int entryId) const
{
    Q_UNUSED(groupId);
    Q_UNUSED(entryId);
    return true;
}


bool CaService::removeEntryFromGroup(const CaEntry &group,
        const CaEntry &entry) const
{
    Q_UNUSED(group);
    Q_UNUSED(entry);
    return true;
}


bool CaService::removeEntriesFromGroup(int groupId,
       const QList<int> &entryIdList) const
{
    Q_UNUSED(groupId);
    Q_UNUSED(entryIdList);
    return true;
}


bool CaService::removeEntriesFromGroup(const CaEntry &group,
        const QList< QSharedPointer<CaEntry> > &entryList) const
{
    Q_UNUSED(group);
    Q_UNUSED(entryList);
    return true;
}


bool CaService::appendEntryToGroup(int groupId, int entryId) const
{
    Q_UNUSED(groupId);
    Q_UNUSED(entryId);
    return true;
}


bool CaService::appendEntryToGroup(const CaEntry &group,
                                   const CaEntry &entry) const
{
    Q_UNUSED(group);
    Q_UNUSED(entry);
    return true;
}


bool CaService::appendEntriesToGroup(int groupId,
        const QList<int> &entryIdList) const
{
    Q_UNUSED(groupId);
    Q_UNUSED(entryIdList);
    return true;
}


bool CaService::appendEntriesToGroup(const CaEntry &group,
        const QList< QSharedPointer<CaEntry> > &entryList) const
{
    Q_UNUSED(group);
    Q_UNUSED(entryList);
    return true;
}


bool CaService::prependEntryToGroup(int groupId, int entryId) const
{
    Q_UNUSED(groupId);
    Q_UNUSED(entryId);
    return true;
}


bool CaService::prependEntryToGroup(const CaEntry &group,
                                    const CaEntry &entry) const
{
    Q_UNUSED(group);
    Q_UNUSED(entry);
    return true;
}

bool CaService::prependEntriesToGroup(int groupId,
                                      const QList<int> &entryIdList) const
{
    Q_UNUSED(groupId);
    Q_UNUSED(entryIdList);
    return true;
}

bool CaService::prependEntriesToGroup(const CaEntry &group,
        const QList< QSharedPointer<CaEntry> > &entryList) const
{
    Q_UNUSED(group);
    Q_UNUSED(entryList);
    return true;
}

int CaService::executeCommand(int entryId, const QString &command,
        QObject* receiver, const char* member) const
{
    Q_UNUSED(entryId);
    Q_UNUSED(command);
    Q_UNUSED(receiver);
    Q_UNUSED(member);
    return 0;
}

int CaService::executeCommand(const CaEntry &entry, const QString &command,
        QObject* receiver, const char* member) const
{
    Q_UNUSED(entry);
    Q_UNUSED(command);
    Q_UNUSED(receiver);
    Q_UNUSED(member);
    return 0;
}

CaNotifier *CaService::createNotifier(const CaNotifierFilter &filter) const
{
    Q_UNUSED(filter);
    return NULL;
}


bool CaService::customSort(int groupId, QList<int> &entryIdList) const
{
    Q_UNUSED(entryIdList);
    return true;
}

ErrorCode CaService::lastError() const
{
    return NoErrorCode;
}
