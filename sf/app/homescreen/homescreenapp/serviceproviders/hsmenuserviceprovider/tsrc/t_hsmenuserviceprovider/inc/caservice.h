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
 * Description: caservice.h
 *
 */

#ifndef CASERVICE_H
#define CASERVICE_H

#include <QObject>
#include <QSharedPointer>
#include <QMetaType>
#include <QCoreApplication>
#include <QString>
#include <QDebug>

#include "caservice.h"
#include "caquery.h"
#include "caentry.h"
#include "canotifier.h"
#include "canotifierfilter.h"
#include "cadefs.h"

typedef QList< QSharedPointer<CaEntry> > CaEntryList;

// mocks Content Storage's CaService
class CaService: public QObject
{
    Q_OBJECT

    explicit CaService(QObject *parent = 0);

    ~CaService();
public:


    static QSharedPointer<CaService> instance();

    // mock only those methods of original CaService that are used in the module
    QSharedPointer<CaEntry> getEntry(int entryId) const;
    QList< QSharedPointer<CaEntry> > getEntries(const CaQuery &query) const;
    QList<int> getEntryIds(const CaQuery &query) const;

    QSharedPointer<CaEntry> createEntry(const CaEntry &entry) const;

    bool removeEntry(int entryId) const;
    bool updateEntry(const CaEntry &entry) const;
    bool touch(const CaEntry &entry) const;
    bool removeEntryFromGroup(int groupId, int entryId) const;
    bool appendEntryToGroup(int groupId, int entryId) const;
    bool appendEntriesToGroup(int groupId,
        const QList<int> &entryIdList) const;

    int executeCommand(int entryId, const QString &command = 
        caCmdOpen, QObject* receiver = NULL, const char* member = NULL) const;
    int executeCommand(const CaEntry &entry, const QString &command =
        caCmdOpen, QObject* receiver = NULL, const char* member = NULL) const;

    
    bool customSort(int groupId, QList<int> &entryIdList) const;
    
    CaNotifier * createNotifier(const CaNotifierFilter &filter) const;
    
    mutable int mGetEntryId;
    mutable QSharedPointer<CaEntry> mGetEntryResult;
    mutable CaQuery mGetEntriesQuery;
    mutable QList< QSharedPointer<CaEntry> > mGetEntriesResult;
    mutable CaQuery mGetEntryIdQuery;
    mutable QList<int> mGetEntryIdsResult;
    mutable CaEntry mCreatedEntry;
    mutable QSharedPointer<CaEntry>mCreateEntryResult;

    mutable int mRemoveEntryId;
    mutable bool mRemoveEntryForIdResult;

    mutable CaEntry mUpdateEntry;
    mutable bool mUpdateEntryResult;

    mutable CaEntry mTouchEntry;
    mutable QList<int> mTouchEntriesId;
    mutable bool mTouchResult;

    mutable int mRemoveGroupId;
    mutable int mRemoveEntryFromGroupId;
    mutable bool mRemoveEntryFromGroupResult;

    mutable int mAppendGroupId;
    mutable int mAppenEntryId;
    mutable bool mAppendEntryToGroupResult;

    mutable int mAppendListGroupId;
    mutable QList<int> mAppendEntryIdList;
    mutable bool mAppendEntriesToGroupResult;

    mutable int mExecuteEntryId;
    mutable QString mExecuteCommand;
    mutable int mExecuteCommandResult;
    
    mutable CaEntry mExecuteEntry;
    
    mutable int mCustomSortGroupId;
    mutable QList<int> mCustomSortEntryIdList;
    mutable bool mCustomSortResult;
    
    mutable CaNotifierFilter mCreateNotifierFilter;
    mutable CaNotifier *mCreateNotifierResult;

    // helper for tests, not belonging to CaService
    void clear()
    {
        mGetEntryId = 0;
        mGetEntryResult.clear();
        mGetEntriesQuery.clear();
        mGetEntriesResult.clear();
        mGetEntryIdQuery.clear();
        mGetEntryIdsResult.clear();
        mCreatedEntry.clear();
        mCreateEntryResult.clear();
        mRemoveEntryId = 0;
        mRemoveEntryForIdResult = false;
        
        mUpdateEntry.clear();
        mUpdateEntryResult = false;
        
        mTouchEntry.clear();
        mTouchEntriesId.clear();
        mTouchResult = false;
        
        mRemoveGroupId = 0;
        mRemoveEntryFromGroupId = 0;
        mRemoveEntryFromGroupResult = false;
        
        mAppendGroupId = 0;
        mAppenEntryId = 0;
        mAppendEntryToGroupResult = false;
        
        mAppendListGroupId = 0;
        mAppendEntryIdList.clear();
        mAppendEntriesToGroupResult = false;
        
        mExecuteEntryId = 0;
        mExecuteCommand.clear();
        mExecuteCommandResult = -333;
        
        mCustomSortGroupId = 0;
        mCustomSortEntryIdList.clear();
        mCustomSortResult = false;
        
        mCreateNotifierFilter = CaNotifierFilter();
        mCreateNotifierResult = 0;
    }
private:
    Q_DISABLE_COPY(CaService)
    friend class QtSharedPointer::ExternalRefCount<CaService>;
};

inline CaService::CaService(QObject *parent) :
        QObject(parent),
        mGetEntryId(0),
        mRemoveEntryId(0),
        mRemoveEntryForIdResult(false),
        mUpdateEntryResult(false),
        mTouchResult(false),
        mRemoveGroupId(0),
        mRemoveEntryFromGroupId(0),
        mRemoveEntryFromGroupResult(false),
        mAppendGroupId(0),
        mAppenEntryId(0),
        mAppendEntryToGroupResult(false),
        mAppendListGroupId(0),
        mAppendEntriesToGroupResult(false),
        mExecuteEntryId(0),
        mExecuteCommandResult(false),
        mCustomSortGroupId(0),
        mCustomSortResult(false),
        mCreateNotifierResult(0)
{
}

inline QSharedPointer<CaService> CaService::instance()
{
    static QSharedPointer<CaService> service(new CaService);
    return service;
}

inline CaService::~CaService()
{
}

inline QSharedPointer<CaEntry> CaService::getEntry(int entryId) const
{
    mGetEntryId = entryId;
    if (!mGetEntryResult.isNull()) {
        mGetEntryResult->mIdResult = entryId;
    }
    return mGetEntryResult;
}


inline QList< QSharedPointer<CaEntry> > CaService::getEntries(const CaQuery &query) const
{
    mGetEntriesQuery = query;
    return mGetEntriesResult;
}

inline QList<int> CaService::getEntryIds(const CaQuery &query) const
{
    mGetEntryIdQuery = query;
    return mGetEntryIdsResult;
}

inline QSharedPointer<CaEntry> CaService::createEntry(const CaEntry &entry) const
{
    mCreatedEntry = entry;
    return mCreateEntryResult;
}

inline bool CaService::removeEntry(int entryId) const
{
    mRemoveEntryId = entryId;
    return mRemoveEntryForIdResult;
}


inline bool CaService::updateEntry(const CaEntry &entry) const
{
    mUpdateEntry = entry;
    return mUpdateEntryResult;
}

inline bool CaService::touch(const CaEntry &entry) const
{
    if (entry.flags() & RemovableEntryFlag &&
        (entry.flags() & UsedEntryFlag) == 0 &&
        entry.role() == ItemEntryRole &&
        entry.entryTypeName() != QString("package")) { //packageTypeName
        mTouchEntry = entry;
        mTouchEntriesId.append(entry.id());
        return mTouchResult;
    } else {
        return true;
    }
}

inline bool CaService::removeEntryFromGroup(int groupId, int entryId) const
{
    mRemoveGroupId = groupId;
    mRemoveEntryFromGroupId = entryId;
    return mRemoveEntryFromGroupResult;
}

inline bool CaService::appendEntryToGroup(int groupId, int entryId) const
{
    mAppendGroupId = groupId;
    mAppenEntryId = entryId;
    return mAppendEntryToGroupResult;
}

inline bool CaService::appendEntriesToGroup(int groupId,
                                     const QList<int> &entryIdList) const
{
    mAppendListGroupId = groupId;
    mAppendEntryIdList = entryIdList;
    return mAppendEntriesToGroupResult;
}

inline int CaService::executeCommand(int entryId, const QString &command, 
        QObject* receiver, const char* member) const
{
    Q_UNUSED(receiver);
    Q_UNUSED(member);
    mExecuteEntryId = entryId;
    mExecuteCommand = command;
    return mExecuteCommandResult;
}

inline int CaService::executeCommand(const CaEntry &entry, const QString &command, 
        QObject* receiver, const char* member) const
{
    Q_UNUSED(receiver);
    Q_UNUSED(member);
    mExecuteEntry = entry;
    mExecuteCommand = command;
    return mExecuteCommandResult;
}

inline bool CaService::customSort(int groupId, QList<int> &entryIdList) const
{
    mCustomSortGroupId = groupId;
    mCustomSortEntryIdList = entryIdList;
    return mCustomSortResult;
}

inline CaNotifier * CaService::createNotifier(const CaNotifierFilter &filter) const
{
    mCreateNotifierFilter = filter;
    return mCreateNotifierResult;
}
    

#endif // CASERVICE_H
