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

// INCLUDE FILES
#include "logsdbconnector.h"
#include "logsdbconnector_stub_helper.h"
#include "logsevent.h"

// CONSTANTS

QString logsLastCalledFunction = "";

void LogsDbConnectorStubHelper::reset()
{
    logsLastCalledFunction = "";
}

QString LogsDbConnectorStubHelper::lastCalledFunction()
{
    return logsLastCalledFunction;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::LogsDbConnector
// ----------------------------------------------------------------------------
//
LogsDbConnector::LogsDbConnector( 
    QList<LogsEvent*>& events, bool checkAllEvents, bool resourceControl ) 
: QObject(), mModelEvents( events ), mCheckAllEvents( checkAllEvents ), 
  mResourceControl( resourceControl )
{
}

// ----------------------------------------------------------------------------
// LogsDbConnector::~LogsDbConnector
// ----------------------------------------------------------------------------
//
LogsDbConnector::~LogsDbConnector()
{
    while ( !mEvents.isEmpty() ){
        delete mEvents.takeFirst();
    }
}

// ----------------------------------------------------------------------------
// LogsDbConnector::init
// ----------------------------------------------------------------------------
//
int LogsDbConnector::init()
{
    logsLastCalledFunction = "init";
    return 0;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::start
// ----------------------------------------------------------------------------
//
int LogsDbConnector::start()
{
    logsLastCalledFunction = "start";
    return 0;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::updateDetails
// ----------------------------------------------------------------------------
//
int LogsDbConnector::updateDetails(bool /*clearCached*/)
{
    logsLastCalledFunction = "updateDetails";
    return 0;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::clearList
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::clearList(LogsModel::ClearType /*cleartype*/)
{
    logsLastCalledFunction = "clearList";
    return true;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::clearEvents
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::clearEvents(const QList<LogsEvent*>& /*events*/)
{    
    logsLastCalledFunction = "clearEvents";
    return true;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::markEventsSeen
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::markEventsSeen(const QList<LogsEvent*>& events)
{
    bool started(false);
    logsLastCalledFunction = "markEventsSeen";
    foreach( LogsEvent* currEv, events ){
        if ( !mEventsSeen.contains(*currEv) ){
            mEventsSeen.append(*currEv);
            started = true;
        }
    }
    return started;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::readDuplicates
// ----------------------------------------------------------------------------
//
int LogsDbConnector::readDuplicates(int eventId)
{
    Q_UNUSED(eventId);
    logsLastCalledFunction = "readDuplicates";
    return 0;
}
    
// ----------------------------------------------------------------------------
// LogsDbConnector::takeDuplicates
// ----------------------------------------------------------------------------
//
QList<LogsEvent*> LogsDbConnector::takeDuplicates()
{
    QList<LogsEvent*> events;
    events = mDuplicatedEvents;
    mDuplicatedEvents.clear();
    return events;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::refreshData
// ----------------------------------------------------------------------------
//
int LogsDbConnector::refreshData()
{
    logsLastCalledFunction = "refreshData";
    return 0;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::compressData
// ----------------------------------------------------------------------------
//
int LogsDbConnector::compressData()
{
    logsLastCalledFunction = "compressData";
    return 0;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::readCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::readCompleted()
{
}

// ----------------------------------------------------------------------------
// LogsDbConnector::errorOccurred
// ----------------------------------------------------------------------------
//
void LogsDbConnector::errorOccurred(int err)
{
    Q_UNUSED(err)
}

// ----------------------------------------------------------------------------
// LogsDbConnector::readCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::removeCompleted()
{
}

// ----------------------------------------------------------------------------
// LogsDbConnector::errorOccurred
// ----------------------------------------------------------------------------
//
void LogsDbConnector::logsRemoveErrorOccured(int err)
{
    Q_UNUSED(err)
}

// ----------------------------------------------------------------------------
// LogsDbConnector::temporaryErrorOccurred
// ----------------------------------------------------------------------------
//
void LogsDbConnector::temporaryErrorOccurred(int err)
{
    Q_UNUSED(err)
}

// ----------------------------------------------------------------------------
// LogsDbConnector::eventModifyingCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::eventModifyingCompleted()
{

}

// ----------------------------------------------------------------------------
// LogsDbConnector::duplicatesReadingCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::duplicatesReadingCompleted(QList<LogsEvent*> duplicates)
{
    Q_UNUSED(duplicates);
}

// ----------------------------------------------------------------------------
// LogsDbConnector::doMarkEventSeen
// ----------------------------------------------------------------------------
//
int LogsDbConnector::doMarkEventSeen()
{
    return 0;
}
