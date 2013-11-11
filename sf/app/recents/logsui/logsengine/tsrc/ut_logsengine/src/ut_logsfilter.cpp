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
#include "ut_logsfilter.h"
#include "logsfilter.h"
#include "logsmodel.h"
#include "logsevent.h"
#include "logscommondata.h"

#include <QtTest/QtTest>

void UT_LogsFilter::initTestCase()
{
    mFilter = new LogsFilter();
}

void UT_LogsFilter::cleanupTestCase()
{
    delete mFilter;
    mFilter = 0;
}


void UT_LogsFilter::init()
{

}

void UT_LogsFilter::cleanup()
{

}

void UT_LogsFilter::testConstructor()
{
    QVERIFY( mFilter );
    LogsFilter* filt = new LogsFilter( LogsFilter::Received );
    QVERIFY( filt->filterType() == LogsFilter::Received );
    delete filt;
}

void UT_LogsFilter::testFilterAcceptsRow()
{
    // No source model
    QVERIFY( mFilter->filterAcceptsRow(0, QModelIndex()) == false );
    
    LogsModel* logsModel = new LogsModel();
    mFilter->setSourceModel(logsModel);
    
    // No any events
    QVERIFY( mFilter->filterAcceptsRow(0, QModelIndex()) == false );
    
    // Matching event ("accept all" filter)
    LogsEvent* event = new LogsEvent;
    logsModel->mEvents.append(event);
    QVERIFY( mFilter->filterAcceptsRow(0, QModelIndex()) == true );
    
    // Matching event ("accept received" filter)
    mFilter->mFilterType = LogsFilter::Received;
    LogsEvent* event2 = new LogsEvent();
    event2->setDirection(LogsEvent::DirIn);
    logsModel->mEvents.append(event2);
    QVERIFY( mFilter->filterAcceptsRow(1, QModelIndex()) == true );
    
    // Not mathing event ("accept received" filter)
    QVERIFY( mFilter->filterAcceptsRow(0, QModelIndex()) == false );
    
    // Called filter
    mFilter->mFilterType = LogsFilter::Called;
    QVERIFY( mFilter->filterAcceptsRow(1, QModelIndex()) == false );
    event2->setDirection(LogsEvent::DirOut);
    QVERIFY( mFilter->filterAcceptsRow(1, QModelIndex()) == true );
    
    // Missed filter
    mFilter->mFilterType = LogsFilter::Missed;
    QVERIFY( mFilter->filterAcceptsRow(1, QModelIndex()) == false );
    event2->setDirection(LogsEvent::DirMissed);
    QVERIFY( mFilter->filterAcceptsRow(1, QModelIndex()) == true );
    
    // Unknown filter
    mFilter->mFilterType =  (LogsFilter::FilterType)999;
    QVERIFY( mFilter->filterAcceptsRow(1, QModelIndex()) == false );
}

void UT_LogsFilter::testClearType()
{
    
    LogsModel* logsModel = new LogsModel();
    mFilter->setSourceModel(logsModel);
    
    // No any events
    QVERIFY( mFilter->clearType() == LogsModel::TypeLogsClearAll );
    
    // Matching event ("accept all" filter)
    LogsEvent* event = new LogsEvent;
    logsModel->mEvents.append(event);
    QVERIFY( mFilter->clearType() == LogsModel::TypeLogsClearAll );
    
    // Matching event ("accept received" filter)
    mFilter->mFilterType = LogsFilter::Received;
    LogsEvent* event2 = new LogsEvent();
    event2->setDirection(LogsEvent::DirIn);
    logsModel->mEvents.append(event2);
    QVERIFY( mFilter->clearType() == LogsModel::TypeLogsClearReceived );
    
    // Not mathing event ("accept received" filter)
    QVERIFY( mFilter->filterAcceptsRow(0, QModelIndex()) == false );
    
    // Called filter
    mFilter->mFilterType = LogsFilter::Called;
    QVERIFY( mFilter->filterAcceptsRow(1, QModelIndex()) == false );
    event2->setDirection(LogsEvent::DirOut);
    QVERIFY( mFilter->clearType() == LogsModel::TypeLogsClearCalled );
    
    // Missed filter
    mFilter->mFilterType = LogsFilter::Missed;
    QVERIFY( mFilter->filterAcceptsRow(1, QModelIndex()) == false );
    event2->setDirection(LogsEvent::DirMissed);
    QVERIFY( mFilter->clearType() == LogsModel::TypeLogsClearMissed );
    
    // Unknown filter
    mFilter->mFilterType =  (LogsFilter::FilterType)999;
    QVERIFY( mFilter->clearType() == LogsModel::TypeLogsClearAll );
}

void UT_LogsFilter::testSetMaxSize()
{
    // Max size for all
    mFilter->mFilterType = LogsFilter::All;
    mFilter->setMaxSize(10);
    QVERIFY( LogsCommonData::getInstance().maxReadSize() == 10 );
    QVERIFY( LogsCommonData::getInstance().maxReadSizeDirection() == LogsEvent::DirUndefined );
    
    // Max size for missed
    mFilter->mFilterType = LogsFilter::Missed;
    mFilter->setMaxSize(5);
    QVERIFY( LogsCommonData::getInstance().maxReadSize() == 5 );
    QVERIFY( LogsCommonData::getInstance().maxReadSizeDirection() == LogsEvent::DirMissed );
    
    // Max size for called
    mFilter->mFilterType = LogsFilter::Called;
    mFilter->setMaxSize(100);
    QVERIFY( LogsCommonData::getInstance().maxReadSize() == 100 );
    QVERIFY( LogsCommonData::getInstance().maxReadSizeDirection() == LogsEvent::DirOut );
    
    // Max size for received
    mFilter->mFilterType = LogsFilter::Received;
    mFilter->setMaxSize(8);
    QVERIFY( LogsCommonData::getInstance().maxReadSize() == 8 );
    QVERIFY( LogsCommonData::getInstance().maxReadSizeDirection() == LogsEvent::DirIn );
}


