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
#include "ut_logscustomfilter.h"
#include "logscustomfilter.h"
#include "logsmodel.h"
#include "logsevent.h"
#include "logseventdata.h"
#include <QStringListModel>

#include <QtTest/QtTest>

void UT_LogsCustomFilter::initTestCase()
{
    mFilter = new LogsCustomFilter;
}

void UT_LogsCustomFilter::cleanupTestCase()
{
    delete mFilter;
    mFilter = 0;
}


void UT_LogsCustomFilter::init()
{

}

void UT_LogsCustomFilter::cleanup()
{

}

void UT_LogsCustomFilter::testConstructor()
{
    QVERIFY( mFilter );
}

void UT_LogsCustomFilter::testFilterAcceptsRow()
{
    // No source model
    QVERIFY( mFilter->filterAcceptsRow(0, QModelIndex()) == false );
    
    LogsModel* logsModel = new LogsModel(LogsModel::LogsFullModel);
    mFilter->setSourceModel(logsModel);
    
    // No any events
    QVERIFY( mFilter->filterAcceptsRow(0, QModelIndex()) == false );
    
    // No search term
    LogsEvent* event = new LogsEvent;
    logsModel->mEvents.append(event);
    QVERIFY( mFilter->filterAcceptsRow(0, QModelIndex()) == false );
    
    // Search term but no matching event
    mFilter->setContactId(50);
    QVERIFY( mFilter->filterAcceptsRow(0, QModelIndex()) == false );
    
    // Search term and matching event
    LogsEvent* event2 = new LogsEvent;
    LogsEventData* eventData = new LogsEventData;
    eventData->setContactLocalId(50);
    event2->setLogsEventData(eventData);
    logsModel->mEvents.append(event2);
    QVERIFY( mFilter->filterAcceptsRow(0, QModelIndex()) == false );
    QVERIFY( mFilter->filterAcceptsRow(1, QModelIndex()) == true );
    
    mFilter->setSourceModel(0);
    delete logsModel;
}

void UT_LogsCustomFilter::testClearEvents()
{
    // No source model
    mFilter->setSourceModel(0);
    QVERIFY( !mFilter->clearEvents() );
    
    // Wrong type of source model
    QStringListModel wrongModel;
    mFilter->setSourceModel(&wrongModel);
    QVERIFY( !mFilter->clearEvents() );

    // Nothing to clear
    LogsModel* logsModel = new LogsModel(LogsModel::LogsFullModel);
    mFilter->setSourceModel(logsModel);
    QVERIFY( !mFilter->clearEvents() );
    
    // Something to clear
    LogsEvent* event2 = new LogsEvent;
    LogsEventData* eventData = new LogsEventData;
    eventData->setContactLocalId(50);
    event2->setLogsEventData(eventData);
    logsModel->mEvents.append(event2);
    mFilter->setSourceModel(0);
    mFilter->setSourceModel(logsModel);
    QVERIFY( mFilter->clearEvents() );  
}

void UT_LogsCustomFilter::testMarkEventsSeen()
{
    // No source model
    mFilter->setSourceModel(0);
    QVERIFY( !mFilter->markEventsSeen() );
    
    // Wrong type of source model
    QStringListModel wrongModel;
    mFilter->setSourceModel(&wrongModel);
    QVERIFY( !mFilter->markEventsSeen() );

    // Nothing to clear
    LogsModel* logsModel = new LogsModel(LogsModel::LogsFullModel);
    mFilter->setSourceModel(logsModel);
    QVERIFY( !mFilter->markEventsSeen() );
    
    // Something to clear
    LogsEvent* event2 = new LogsEvent;
    LogsEventData* eventData = new LogsEventData;
    eventData->setContactLocalId(50);
    event2->setLogsEventData(eventData);
    logsModel->mEvents.append(event2);
    mFilter->setSourceModel(0);
    mFilter->setSourceModel(logsModel);
    QVERIFY( mFilter->markEventsSeen() );  
    
    // Clearing for the event already ongoing, don't try again
    event2->markedAsSeenLocally(true);
    QVERIFY( !mFilter->markEventsSeen() );  
}


