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

#ifndef CNTHISTORYMODELPRIVATE_H
#define CNTHISTORYMODELPRIVATE_H

#include <QSharedData>
#include <QSharedPointer>
#include <QMap>
#include <QDateTime>
#include <qtcontacts.h>

#ifdef PBK_UNIT_TEST
#include "stub_classes.h"
#else
#include <logsevent.h>
#include <logsmodel.h>
#include <logscustomfilter.h>
#endif
#include <msghistory.h>
#include <msgitem.h>
#include <hbextendedlocale.h>
#include "cnthistorymodel.h"
#include "cnthistorymodelconsts.h"

QTM_USE_NAMESPACE

class HistoryItem
{   
public:   
    HistoryItem() : flags(0)
    {};
    
    inline const HistoryItem& operator=(const HistoryItem& other)
    {
        flags = other.flags;
        number = other.number;
        title = other.title;
        message = other.message;
        timeStamp = other.timeStamp;
        return *this;
    }
    
    inline bool operator==(const HistoryItem& other) const
    {
         return timeStamp == other.timeStamp;
    }
    
public:
    int flags;
    QString number;
    QString title;
    QString message;
    QDateTime timeStamp;
};

typedef QSharedPointer<HistoryItem> HItemPointer;

class CntHistoryModelPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CntHistoryModel)
    
public:
    CntHistoryModelPrivate(QContactLocalId contactId, QContactManager* manager);
    ~CntHistoryModelPrivate();

public:
    QVariant data(const QModelIndex& index, int role) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    void sort(int column = 0, Qt::SortOrder order = Qt::AscendingOrder);
    void clearHistory();
    void markAllAsSeen();
    
public:
    CntHistoryModel *q_ptr;
    
public:
    void initializeModel();
    QVariant displayRoleData(const HistoryItem& item) const;
    QVariant decorationRoleData(const HistoryItem& item) const;
    QVariant backgroundRoleData(const HistoryItem& item) const;
    QVariant conversationIdRoleData(const int row) const;
    
    // Utility finctions
    void readLogEvent(LogsEvent* event, HistoryItem& item);
    void readMsgEvent(MsgItem& event, HistoryItem& item);
    void initializeLogsModel();
    void initializeMsgModel();
    bool validateRowIndex(const int index) const;
    QList< QList<int> > findIndices( const QList< int >& indices );
    
public slots:
    // Logs model slots
    void logsRowsInserted(const QModelIndex& parent, int first, int last);
    void logsRowsRemoved(const QModelIndex& parent, int first, int last);
    void logsDataChanged(const QModelIndex& first, const QModelIndex& last);
    void handleLogsReset();
    
    // Messaging model slots
    void messagesReady(QList<MsgItem>& msgs);
    void messageAdded(MsgItem& msg);
    void messageChanged(MsgItem& msg);
    void messageDeleted(MsgItem& msg);
    
public:
    LogsModel* m_logsModel;
    LogsCustomFilter* m_logsFilter;
    QAbstractItemModel* m_AbstractLogsModel;
    MsgHistory* m_msgHistory;
	QContactLocalId m_contactId;
    QContactManager* m_contactManager;
    bool m_isMyCard;
    bool m_isMarkedAsSeen;
    bool m_initLogs;
    //this contains merged calls and messages history
    QList<HItemPointer> m_List;
    QMap<int, HItemPointer> m_logsMap;
    QMap<int, HItemPointer> m_msgMap;
    const HbExtendedLocale m_extendedLocale;
};

#endif

