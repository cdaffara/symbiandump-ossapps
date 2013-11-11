/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef STUB_CLASSES_H
#define STUB_CLASSES_H

#include <QtTest/QtTest>
#include <QAbstractItemModel>
#include "cnthistorymodel.h"

#define ALL_EVENTS 7
#define FILTERED_EVENTS 5

const QString own_msg = QString("Own card view message %1 added");
const QString cc_msg = QString("Contact message %1 added");
const QString icon_path = QString("Icon %1 path");
const QDateTime constDateTime = QDateTime(QDate(2010, 2, 15), QTime(12, 10, 10));

class LogsEvent
{
public:
    enum LogsDirection
    {
        DirUndefined = -1, // undefined
        DirIn =  0,        // incoming
        DirOut = 1,        // outgoing
        DirMissed = 2      // missed
    };
public:
    LogsEvent();
    ~LogsEvent(){};
    bool isRead(){return false;}
    QDateTime time() const{return m_time;}
    const QString& number() const{return m_num;}
    const QString& remoteParty() const{return m_remoteParty;}
    LogsDirection direction() const{return m_dir;}
    
public:
    QDateTime m_time;
    QString m_num;
    QString m_remoteParty;
    LogsDirection m_dir;
};

typedef QSharedPointer<LogsEvent> LogSharedPtr;

class LogsModel : public QAbstractItemModel
{ 
    Q_OBJECT
    
public:
    
    enum LogsModelType
    {
        RoleFullEvent,
        LogsFullModel
    };
    
    enum ClearType {
        TypeLogsClearAll = 0,
        TypeLogsClearReceived,
        TypeLogsClearCalled,
        TypeLogsClearMissed
    };
    
public:
    LogsModel(LogsModelType /*modelType*/);
    ~LogsModel(){}
    bool clearList(LogsModel::ClearType /*cleartype*/)
        {
        emit clearingCompleted(0);
        return true;
        }
    void markEventsSeen(LogsModel::ClearType /*cleartype*/)
        {
        emit dataChanged(index(1,1), index(2,1));
        }
    void updateEvents(LogsModel::ClearType /*cleartype*/);
    QVariant data(const QModelIndex& index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex& /*parent*/ = QModelIndex()) const
        {return createIndex(row, column);}
    QModelIndex parent(const QModelIndex& /*index*/) const
        {return QModelIndex();}
    int rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const
        {return m_events.count();}
    int columnCount(const QModelIndex& /*parent*/ = QModelIndex()) const
        {return 1;}
signals:
    void clearingCompleted(int err);
    
private:
    QList<LogSharedPtr> m_events;
};

class LogsCustomFilter : public QAbstractItemModel
{
    Q_OBJECT
    
public:
    LogsCustomFilter();
    ~LogsCustomFilter(){}
    void setContactId(int /*cntId*/){};
    void setSourceModel(QAbstractItemModel* /*srcModel*/){};
    bool clearEvents()
        {
        emit clearingCompleted(0);
        return true;
        }
    void markEventsSeen()
        {
        emit dataChanged(index(1,1), index(3,1));
        }
    
    QVariant data(const QModelIndex& index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex& /*parent*/ = QModelIndex()) const
        {return createIndex(row, column);}
    QModelIndex parent(const QModelIndex& /*index*/) const
        {return QModelIndex();}
    int rowCount(const QModelIndex& /*parent*/ = QModelIndex()) const
        {return m_events.count();}
    int columnCount(const QModelIndex& /*parent*/ = QModelIndex()) const
        {return 1;};
signals:
    void clearingCompleted(int err);
            
private:
    QList<LogSharedPtr> m_events;
};

#endif
