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

#include <QtTest/QtTest>

#include "mt_cnthistorymodel.h"
#include "cnthistorymodel_p.h"
#include "cnthistorymodel.h"

void TestCntHistoryModel::init()
{
    qRegisterMetaType<QModelIndex>("QModelIndex");
    cm = new QContactManager("symbian");
    model = new CntHistoryModel(1, cm);
    model->d_ptr->m_List.clear();
    model->d_ptr->m_logsMap.clear();
    model->d_ptr->m_msgMap.clear();
}

void TestCntHistoryModel::cleanup()
{
    delete cm;
    delete model;
}

void TestCntHistoryModel::testLogsInsertions()
{   
    QSignalSpy spy( model, SIGNAL(modelReset()));
    model->d_ptr->logsRowsInserted(QModelIndex(), 0, 0);
    
    QVERIFY(model->rowCount() == 1);
    QVERIFY(model->d_ptr->m_logsMap.count() == 1);
    QVERIFY(spy.count() == 1);
}

void TestCntHistoryModel::testLogsRemovals()
{
    model->d_ptr->logsRowsInserted(QModelIndex(), 0, 0);
    
    QSignalSpy spy( model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
    model->d_ptr->logsRowsRemoved(QModelIndex(), 0, 0);
    QVERIFY(model->rowCount() == 0);
    QVERIFY(model->d_ptr->m_logsMap.count() == 0);
    QVERIFY(spy.count() == 1);
    
    // Remove the same item
    QSignalSpy spy1( model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
    model->d_ptr->logsRowsRemoved(QModelIndex(), 0, 0);
    QVERIFY(spy1.count() == 0);
}

void TestCntHistoryModel::testLogsUpdates()
{
    model->d_ptr->logsRowsInserted(QModelIndex(), 0, 0);
    
    QSignalSpy spy( model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)));
    QModelIndex in = model->d_ptr->m_AbstractLogsModel->index(0,0);
    model->d_ptr->logsDataChanged(in, in);
    QVERIFY(spy.count() == 1);
}

void TestCntHistoryModel::testMsgInsertions()
{
    QList<MsgItem> msgs;
    MsgItem msg1;
    msg1.setId(111);
    MsgItem msg2;
    msg2.setId(222);
    MsgItem msg3;
    msg3.setId(333);
    msgs.append(msg1);
    msgs.append(msg2);
    msgs.append(msg3);
    
    QSignalSpy spy( model, SIGNAL(modelReset()));
    model->d_ptr->messagesReady(msgs);
    
    QVERIFY(model->rowCount() == 3);
    QVERIFY(model->d_ptr->m_msgMap.count() == 3);
    QVERIFY(model->d_ptr->m_msgMap.contains(111));
    QVERIFY(model->d_ptr->m_msgMap.contains(222));
    QVERIFY(model->d_ptr->m_msgMap.contains(333));
    QVERIFY(spy.count() == 1);
    
    MsgItem msg;
    QSignalSpy spy2( model, SIGNAL(rowsInserted(const QModelIndex &, int, int)));
    model->d_ptr->messageAdded(msg);
    QVERIFY(model->rowCount() == 4);
    QVERIFY(model->d_ptr->m_msgMap.count() == 4);
    QVERIFY(spy2.count() == 1);
    
}

void TestCntHistoryModel::testMsgRemovals()
{
    QList<MsgItem> msgs;
    MsgItem msg1;
    msg1.setId(111);
    MsgItem msg2;
    msg2.setId(222);
    MsgItem msg3;
    msg3.setId(333);
    msgs.append(msg1);
    msgs.append(msg2);
    msgs.append(msg3);
    model->d_ptr->messagesReady(msgs);
    
    QSignalSpy spy( model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)));
    model->d_ptr->messageDeleted(msg2);
    QVERIFY(!model->d_ptr->m_msgMap.contains(222));
    QVERIFY(model->rowCount() == 2);
    QVERIFY(spy.count() == 1);
    
    // Increase code coverage
    HItemPointer p;
    model->d_ptr->m_msgMap.insert(333, p);
    model->d_ptr->messageDeleted(msg3);
}

void TestCntHistoryModel::testMsgUpdates()
{
    QList<MsgItem> msgs;
    MsgItem msg1;
    msg1.setId(111);
    MsgItem msg2;
    msg2.setId(222);
    MsgItem msg3;
    msg3.setId(333);
    msgs.append(msg1);
    msgs.append(msg2);
    msgs.append(msg3);
    model->d_ptr->messagesReady(msgs);
    
    QSignalSpy spy( model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)));
    QString s("test message");
    msg2.setBody(s);
    model->d_ptr->messageChanged(msg2);
    QVERIFY(model->d_ptr->m_msgMap.value(222)->message == s);
    QVERIFY(spy.count() == 1);
    
    // Increase code coverage
    HItemPointer p;
    model->d_ptr->m_msgMap.insert(333, p);
    model->d_ptr->messageChanged(msg3);
}

void TestCntHistoryModel::testReadLogEvent()
{
    LogsEvent event;
    HItemPointer item(new HistoryItem);
    model->d_ptr->readLogEvent(&event, *item);
    
    QVERIFY(event.time() == item->timeStamp);
    QVERIFY(item->flags & CntIncoming);
    QVERIFY(event.number() == item->number);
    QVERIFY(item->flags & CntCallLog);
    QVERIFY(!item->message.isEmpty());
    
    // Increase code coverage
    model->d_ptr->m_isMyCard = true;
    model->d_ptr->readLogEvent(&event, *item);
}

void TestCntHistoryModel::testReadMsgEvent()
{
    MsgItem msg;
    QString body("Some message");
    msg.setBody(body);
    msg.setDirection(MsgItem::MsgDirectionIncoming);
    msg.setPhoneNumber("123456");
    QDateTime d = QDateTime(QDate(2012, 12, 12), QTime(12, 23, 10));
    msg.setTimeStamp(d.toTime_t());
    
    HItemPointer item(new HistoryItem);
    model->d_ptr->readMsgEvent(msg, *item);
    
    QVERIFY(item->timeStamp == d);
    QVERIFY(item->message == body);
    QVERIFY(item->flags & CntMessage);
    QVERIFY(item->number == QString("123456"));
    QVERIFY(item->flags & CntIncoming);
}

void TestCntHistoryModel::testRoles()
{
    MsgItem msg;
    model->d_ptr->messageAdded(msg);
    
    QModelIndex modelIndex = model->index(0, 0);
    QVERIFY(modelIndex.isValid());
    
    // HbIcon
    QVariant var = modelIndex.data(Qt::DecorationRole);
    QVERIFY(var.isValid());
    
    // FlagsRole
    var = modelIndex.data(CntFlagsRole);
    QVERIFY(var.type() == QVariant::Int);
    
    // Display role
    var = modelIndex.data(Qt::DisplayRole);
    QVERIFY(var.type() == QVariant::StringList);
    
    // PhoneNumberRole
    var = modelIndex.data(CntPhoneNumberRole);
    QVERIFY(var.type() == QVariant::String);

    // BackgroundRole
    var = modelIndex.data(Qt::BackgroundRole);
    QVERIFY(var.type() > (QVariant::UserType - 1));
    
    // ConversationRole
    var = modelIndex.data(CntConverstaionIdRole);
    QVERIFY(var.type() == QVariant::Int);
    
    // Invalid role
    var = modelIndex.data(-10);
    QVERIFY(var.type() == QVariant::Invalid);
    
    // Invalid indices
    modelIndex = model->index(-10, 0);
    QVERIFY(!modelIndex.isValid());
    var = model->data(modelIndex, Qt::DisplayRole);
    QVERIFY(var.type() == QVariant::Invalid);
    
    modelIndex = model->index(100, 0);
    QVERIFY(!modelIndex.isValid());
    
    // Stored history item is null
    model->d_ptr->m_List.first().clear();
    modelIndex = model->index(0, 0);
    var = model->data(modelIndex, Qt::DisplayRole);
    QVERIFY(var.type() == QVariant::Invalid);
}

void TestCntHistoryModel::testSorting()
{
    // Create random data
    QList<int> delta;
    delta.append(23);
    delta.append(12);
    delta.append(45);
    delta.append(32);
    delta.append(67);
    delta.append(45);
    delta.append(92);
    delta.append(12);
    
    QDateTime dt = QDateTime::currentDateTime();
    
    for(int i=0; i<delta.count(); i++) {
        HItemPointer p(new HistoryItem());
        dt = dt.addMSecs(delta.at(i));
        p.data()->timeStamp = dt.toLocalTime();
        if ( i % 2 )
            p.data()->flags |= CntCallLog;
        else
            p.data()->flags |= CntMessage;

        model->d_ptr->m_List.append(p);
    } 
    
    // Sort ascending
    QSignalSpy spy( model, SIGNAL(modelReset()));
    model->sortAndRefresh(Qt::AscendingOrder);
    
    QVERIFY(spy.count() == 1);    
    HItemPointer current = model->d_ptr->m_List.first();
    for(int i=1; i<model->d_ptr->m_List.count(); i++) {
        QVERIFY(current.data()->timeStamp <= model->d_ptr->m_List.at(i).data()->timeStamp);
        current = model->d_ptr->m_List.at(i);
    }
    
    // Sort descending
    QSignalSpy spy1( model, SIGNAL( modelReset()));
    model->sortAndRefresh(Qt::DescendingOrder);
    
    QVERIFY(spy1.count() == 1);    
    current = model->d_ptr->m_List.last();
    for(int i=model->d_ptr->m_List.count(); i<1; i--) {
        QVERIFY(current.data()->timeStamp >= model->d_ptr->m_List.at(i).data()->timeStamp);
        current = model->d_ptr->m_List.at(i);
    }
}

void TestCntHistoryModel::testClear()
{
    model->d_ptr->logsRowsInserted(QModelIndex(), 0, 0);
    
    // Clear history with log
    QSignalSpy spy( model, SIGNAL( rowsRemoved(const QModelIndex &, int, int)));
    model->clearHistory();
    QVERIFY( model->rowCount() == 0 );
    QVERIFY(spy.count() == 1);
    
    // Clear already empty history view
    QSignalSpy spy1( model, SIGNAL( rowsRemoved(const QModelIndex &, int, int)));
    model->clearHistory();
    QVERIFY(spy1.count() == 0);
    
    // test reset
    QSignalSpy spy2( model, SIGNAL(modelReset()));
    model->d_ptr->handleLogsReset();
    QVERIFY(spy2.count() == 1);
}

void TestCntHistoryModel::testMarkSeen()
{
    // Mark history as seen
    QVERIFY(!model->d_ptr->m_isMarkedAsSeen);
    model->markAllAsSeen();
    QVERIFY(model->d_ptr->m_isMarkedAsSeen);
    
    // NOT TESTING! Increasing code coverage
    model->markAllAsSeen();
    model->d_ptr->m_isMarkedAsSeen = false;
    model->d_ptr->m_isMyCard = true;
    model->d_ptr->initializeMsgModel();
    model->markAllAsSeen();
    model->clearHistory();
}
