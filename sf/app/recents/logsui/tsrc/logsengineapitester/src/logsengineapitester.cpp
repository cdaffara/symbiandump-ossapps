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

#include <QDebug>
#include <hblistview.h>
#include <hbmenu.h>
#include <hbaction.h>
#include <hblabel.h>
#include <hbmessagebox.h>
#include <QGraphicsLinearLayout>
#include <logsmodel.h>
#include <logsevent.h>
#include <logscustomfilter.h>
#include <centralrepository.h>
#include <logsdomaincrkeys.h>
#include <logsfilter.h>
#include "logsengineapitester.h"

Q_DECLARE_METATYPE(LogsEvent *)

#define LOGSAPITEST_PRINT_ALLOC_SIZE { \
    User::CompressAllHeaps(); \
    TInt allocsize; \
    User::AllocSize(allocsize); \
    qDebug() << "LogsApiTest alloc size:" << allocsize; } \

TestView::TestView() : HbView(), mList(0), mModel(0), mLogsModel(0), mFilter(0), mShowAll(true)
{
    qDebug() << "LogsApiTest::LogsApiTest ->";
    
    LOGSAPITEST_PRINT_ALLOC_SIZE
    
    mRepository = CRepository::NewL( KCRUidLogs );
    
    mLabel = new HbLabel(this);
    mList = new HbListView(this);
    mList->setItemRecycling(true);
    mList->setUniformItemSizes(true);
    mLogsModel = new LogsModel(LogsModel::LogsFullModel);
    mModel = new TestModel(*mLogsModel);
    mList->setModel(mModel);
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->addItem(mLabel);
    layout->addItem(mList);
    setLayout(layout);
    
    mMissedCallsFilter = new LogsFilter(LogsFilter::Missed);
    mMissedCallsFilter->setMaxSize(5);   
    mMissedCallsFilter->setSourceModel(mLogsModel);
    connect(mMissedCallsFilter, SIGNAL(rowsInserted(const QModelIndex &, int, int)), 
            this, SLOT(updateMissedCallsLabel()));
    
    HbAction* filterAction = new HbAction;
    filterAction->setText("Change filter");
    connect(filterAction, SIGNAL(triggered()), this, SLOT(changeFilter()) );
    menu()->addAction(filterAction);
    
    HbAction* addMissedCallAction = new HbAction;
    addMissedCallAction->setText("Add missed call");
    connect(addMissedCallAction, SIGNAL(triggered()), this, SLOT(addMissedCall()) );
    menu()->addAction(addMissedCallAction);
    
    HbAction* clearMissedCallsAction = new HbAction;
    clearMissedCallsAction->setText("Clear missed calls");
    connect(clearMissedCallsAction, SIGNAL(triggered()), this, SLOT(clearMissedCalls()) );
    menu()->addAction(clearMissedCallsAction);
    
    HbAction* deleteAction = new HbAction;
    deleteAction->setText("Delete events");
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteEvents()) );
    menu()->addAction(deleteAction);
    
    updateMissedCallsLabel();
    
    qDebug() << "LogsApiTest::LogsApiTest <-";
  
}


TestView::~TestView()
{
    qDebug() << "LogsApiTest::~LogsApiTest ->";

    delete mMissedCallsFilter;
    mList->setModel(0);
    delete mModel;
    delete mLogsModel;
    
    delete mRepository;
    
    qDebug() << "LogsApiTest::~LogsApiTest <-";
}

void TestView::changeFilter()
{
    quint32 contactId = 2;
    if ( mShowAll ){
        if ( mFilter ){
            delete mFilter;
            mFilter = 0;
        }
        mFilter = new LogsCustomFilter;
        mFilter->setContactId(contactId);
        mFilter->setSourceModel(mLogsModel);
        connect( mFilter, SIGNAL(markingCompleted(int)), this, SLOT(markingCompleted(int)) );
        connect( mFilter, SIGNAL(clearingCompleted(int)), this, SLOT(clearingCompleted(int)) );
        mList->setModel(0);
        delete mModel;
        mModel = 0;
        mModel = new TestModel(*mFilter);
        mList->setModel(mModel);
        mShowAll = false;
    } 
    else {
        mList->setModel(0);
        delete mFilter;
        mFilter = 0;
        delete mModel;
        mModel = 0;
        mModel = new TestModel(*mLogsModel);
        mList->setModel(mModel);
        mShowAll = true;
    }
}

void TestView::addMissedCall()
{
    TInt errorCode( KErrNone );
    TInt value;
    
    errorCode = mRepository->Get( KLogsNewMissedCalls, value );

    if ( errorCode == KErrNone ){
        ++value;
        errorCode = mRepository->Set( KLogsNewMissedCalls, value );
    }
    
    // Do here what some API user might do when it gets
    // notification about missed call counter increase
    updateMissedCallsLabel();
}

void TestView::clearMissedCalls()
{
    mRepository->Set( KLogsNewMissedCalls, 0 );
    if ( mFilter ){
        mFilter->markEventsSeen();
    }
    updateMissedCallsLabel();
}

void TestView::deleteEvents()
{
    if ( mFilter ){
        mFilter->clearEvents();
    } else {
        mLogsModel->clearList(LogsModel::TypeLogsClearAll);
    }
}

void TestView::updateMissedCallsLabel()
{
    qDebug() << "LogsApiTest::updateMissedCallsLabel ->";
    
    LOGSAPITEST_PRINT_ALLOC_SIZE
    
    QString lastMissedCallFrom;
    int repeatedMissedCalls = 0;
    TInt value(0);
    mRepository->Get( KLogsNewMissedCalls, value );
    if ( mMissedCallsFilter->rowCount() > 0 ){
        lastMissedCallFrom = mMissedCallsFilter->data(mMissedCallsFilter->index(0,0), Qt::DisplayRole).toStringList().at(0);
        repeatedMissedCalls = 1;
    }
    for( int i = 1; i < value && i < mMissedCallsFilter->rowCount(); i++ ){
        QStringList displayData = mMissedCallsFilter->data(mMissedCallsFilter->index(i,0), Qt::DisplayRole).toStringList();
        if ( lastMissedCallFrom == displayData.at(0) ){
            repeatedMissedCalls++;
        } else {
            i = value; // Break
        }
    }
    
    if ( value == 0 ){
        mLabel->setPlainText( "No missed calls" );
    } else if ( repeatedMissedCalls >= value ){
        mLabel->setPlainText( 
            QString("%1 missed call(s) from %2").arg(value).arg(lastMissedCallFrom) );
    } else {
        mLabel->setPlainText( QString("%1 missed call(s)").arg(value) );
    }
    qDebug() << "LogsApiTest::updateMissedCallsLabel <-";
}

void TestView::markingCompleted(int err)
{
     HbMessageBox::information( QString("Marking completed, err:%1").arg(err) );
}

void TestView::clearingCompleted(int err)
{
    HbMessageBox::information( QString("Clearing completed, err:%1").arg(err) );
}

TestModel::TestModel(QAbstractItemModel& logsModel) : mLogsModel(logsModel)
{
    readEvents();
    connect( &mLogsModel, SIGNAL( dataChanged(const QModelIndex&,const QModelIndex&)), 
             this, SLOT(handleModelUpdated()));
    connect( &mLogsModel, SIGNAL( rowsInserted(const QModelIndex&,int,int)), 
             this, SLOT(handleModelUpdated()));
    connect( &mLogsModel, SIGNAL( rowsRemoved(const QModelIndex&,int,int)), 
             this, SLOT(handleModelUpdated()));
    connect( &mLogsModel, SIGNAL(modelReset()), 
             this, SLOT(handleModelUpdated()));
}
TestModel::~TestModel()
{
}
int TestModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mEvents.count();
}
QVariant TestModel::data(const QModelIndex &index, int role) const
{
    if ( role == Qt::DisplayRole && index.row() >= 0 && index.row() < mEvents.count() ){
        return mEvents.at(index.row());
    }
    return QVariant();
}
void TestModel::handleModelUpdated()
{
    LOGSAPITEST_PRINT_ALLOC_SIZE
    
    // Do it in simple but unefficient way
    readEvents();
    reset();
}

void TestModel::readEvents()
{
   mEvents.clear();
   for ( int i = 0; i < mLogsModel.rowCount(); ++i ){
        LogsEvent* event = qVariantValue<LogsEvent*>( 
                mLogsModel.data( mLogsModel.index(i, 0), LogsModel::RoleFullEvent ) );
        if ( event ){
            
            mEvents.append( directionText(*event) );
            // Can check occurence time etc.
        }
    }
}

QString TestModel::directionText(const LogsEvent& event)
{
    QString direction;
    if ( event.direction() == LogsEvent::DirIn ) {
        direction = QString("Received call from ");
    } else if ( event.direction() == LogsEvent::DirOut ) {
        direction = QString("Called to ");
    } else  if ( event.direction() == LogsEvent::DirMissed ) {
        direction = QString("Missed call from ");
    }
    if ( event.remoteParty().length() > 0 ){
        direction.append( event.remoteParty() );
    }
    else {
        direction.append( event.number() );
    }
    
    if ( event.direction() == LogsEvent::DirMissed ){
        if ( event.isRead() ){
            direction.append( " Seen");
        } else {
            direction.append( " Unseen");
        }   
    }
        
    return direction;
}



