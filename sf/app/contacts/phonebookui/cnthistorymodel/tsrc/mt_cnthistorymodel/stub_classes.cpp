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

#include "cnthistorymodel_p.h"
#include "stub_classes.h"

Q_DECLARE_METATYPE(LogsEvent *)

QVariant LogsModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 && index.row() >= rowCount())
        return QVariant();
    
    if (role == LogsModel::RoleFullEvent && index.row() < m_events.count()) {
        LogsEvent* event = m_events.at(index.row()).data();
        return qVariantFromValue(event);
    }
    if (role == Qt::DecorationRole) {
        return QVariant(QString(icon_path).arg(index.row()));
    }
    return QVariant();
}

QVariant LogsCustomFilter::data(const QModelIndex& index, int role) const
{    
    if (index.row() < 0 && index.row() >= rowCount())
            return QVariant();
    
    if (role == LogsModel::RoleFullEvent) {
        LogsEvent* event = m_events.at(index.row()).data();
        return qVariantFromValue(event);
    }
    if (role == Qt::DecorationRole) {
        return QVariant(QString(icon_path).arg(index.row()));
    }
    return QVariant();
}

LogsEvent::LogsEvent()
{
    m_num = "1234567";
    m_remoteParty = "Caller's name";
    m_dir = DirMissed;
}

LogsModel::LogsModel(LogsModelType modelType)
{
    if (modelType != LogsModel::LogsFullModel)
        return;
    
    for(int i = 0; i < ALL_EVENTS; i++) {
        LogSharedPtr p(new LogsEvent());
        p.data()->m_time = constDateTime.addSecs(5*i);
        m_events.append(p);
    }
    beginInsertRows(QModelIndex(), 0, m_events.count());
    endInsertRows();
}

void LogsModel::updateEvents(LogsModel::ClearType /*cleartype*/)
{
    LogSharedPtr p = m_events.at(1);
    p.data()->m_time = p.data()->m_time.addSecs(5);
    p.data()->m_dir = LogsEvent::DirIn;
    
    LogSharedPtr p1 = m_events.at(2);
    p1.data()->m_time = p.data()->m_time.addSecs(5);
    p1.data()->m_dir = LogsEvent::DirOut;
    emit dataChanged(index(1,1), index(2,1));
}

LogsCustomFilter::LogsCustomFilter()
{
    for(int i = 0; i < FILTERED_EVENTS; i++) {
        LogSharedPtr p(new LogsEvent());
        p.data()->m_time = constDateTime.addSecs(5*i);
        m_events.append(p);
    }
    beginInsertRows(QModelIndex(), 0, m_events.count());
    endInsertRows();
}

bool MsgHistory::getMessages(int cntId){}

bool MsgHistory::subscribe(int cntId){}

bool MsgHistory::markRead(int cntId){
    return true;
}

