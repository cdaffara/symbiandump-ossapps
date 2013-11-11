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
#include "logsevent.h"
#include "logsengdefs.h"
#include "logsreader.h"
#include "logslogger.h"
#include "logsremove.h"
#include "logscommondata.h"
#include <logcli.h>
#include <f32file.h>
#include <QApplication>
// CONSTANTS



// ----------------------------------------------------------------------------
// LogsDbConnector::LogsDbConnector
// ----------------------------------------------------------------------------
//
LogsDbConnector::LogsDbConnector( 
    QList<LogsEvent*>& events, bool checkAllEvents, bool resourceControl ) 
: QObject(), 
  mModelEvents( events ), 
  mCheckAllEvents( checkAllEvents ),
  mResourceControl( resourceControl ),
  mLogClient( 0 ), 
  mReader( 0 ),
  mLogsRemove( 0 ),
  mCompressionEnabled(false),
  mFirstReadCompleted(false)
{
    LOGS_QDEBUG( "logs [ENG] <-> LogsDbConnector::LogsDbConnector()" )
    mFsSession = new RFs();
}

// ----------------------------------------------------------------------------
// LogsDbConnector::~LogsDbConnector
// ----------------------------------------------------------------------------
//
LogsDbConnector::~LogsDbConnector()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::~LogsDbConnector()" )
    
    delete mReader;
    delete mLogsRemove;
    delete mLogClient;
    if ( mFsSession ){
        mFsSession->Close();
    }
    delete mFsSession;
    
    qDeleteAll( mEvents );
    qDeleteAll( mDuplicatedEvents );
    
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::~LogsDbConnector()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::init
// ----------------------------------------------------------------------------
//
int LogsDbConnector::init()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::init()" )
    
    TRAPD( err, initL() );
    handleTemporaryError(err);
    
    LOGS_QDEBUG_2( "logs [ENG] <- LogsDbConnector::init(), err:", err )
    
    return err;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::start
// ----------------------------------------------------------------------------
//
int LogsDbConnector::start()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::start()" )

#ifdef LOGSDBCONNECTOR_SIMULATION
    
    LOGS_QDEBUG( "logs [ENG]    Simulation enabled" )
    int numEvents = 16;
    QList<int> indexes;
    for ( int i = 0; i < numEvents; i++ ){
        QString number = QString("1223456%1").arg(i);
        LogsEvent* event = new LogsEvent;
        if ( i % 3 == 0 ){
            event->setDirection(LogsEvent::DirMissed);
            number += QString("DirMissed");        
        }
        else if ( i % 2 == 0 ){
            event->setDirection(LogsEvent::DirIn);
            number += QString("DirIn");
        } else {
            event->setDirection(LogsEvent::DirOut);
            number += QString("DirOut");
        }
        event->setNumber(number);
        mEvents.append(event);
        mModelEvents.append(event);
        indexes.append(i);
    }
    emit dataAdded( indexes );
    return 0;
    
#endif
    
    if ( !mReader ){
        LOGS_QDEBUG( "logs [ENG]    Not initialized, failure" )
        return -1;
    }
    int err = mReader->start();
    handleTemporaryError(err);
    
    LOGS_QDEBUG_2( "logs [ENG] <- LogsDbConnector::start(), err:", err )
    return err;
}


// ----------------------------------------------------------------------------
// LogsDbConnector::updateDetails
// ----------------------------------------------------------------------------
//
int LogsDbConnector::updateDetails(bool clearCached)
{
    if ( !mReader ){
        LOGS_QDEBUG( "logs [ENG]    Not initialized, failure" )
        return -1;
    }
    mReader->updateDetails(clearCached);    
    return 0;    
}

// ----------------------------------------------------------------------------
// LogsDbConnector::stateChanged
// ----------------------------------------------------------------------------
//
void LogsDbConnector::initL()
{
    if ( mReader ){
        // Already initialized
        return;
    }  
    User::LeaveIfError( mFsSession->Connect() );
    mLogClient = CLogClient::NewL( *mFsSession );
    mLogsRemove = new LogsRemove( *this, mCheckAllEvents );
    
    TLogString logString;
    //Texts in LOGWRAP.RLS / LOGWRAP.RSS        
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_DIR_IN ) );
    mLogEventStrings.iInDirection = DESC_TO_QSTRING( logString );
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_DIR_OUT ) );
    mLogEventStrings.iOutDirection = DESC_TO_QSTRING( logString );
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_DIR_MISSED ) );
    mLogEventStrings.iMissedDirection = DESC_TO_QSTRING( logString );
    //"Unknown" (Logwrap.rls)
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_REMOTE_UNKNOWN ) ); 
    mLogEventStrings.iUnKnownRemote = DESC_TO_QSTRING( logString );
    //"Incoming on alternate line"
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_DIR_IN_ALT ) ); 
    mLogEventStrings.iInDirectionAlt = DESC_TO_QSTRING( logString );
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_DIR_OUT_ALT ) );
    mLogEventStrings.iOutDirectionAlt = DESC_TO_QSTRING( logString );
    User::LeaveIfError( mLogClient->GetString( logString, R_LOG_DIR_FETCHED) );
    mLogEventStrings.iFetched = DESC_TO_QSTRING( logString );
    
    mReader = new LogsReader( 
        *mFsSession, *mLogClient, mLogEventStrings, mEvents, *this, mCheckAllEvents );
    
    if ( mResourceControl ){
        LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::initL(), resource control enabled" )
        LogsCommonData::getInstance().configureReadSize(
                logsReadSizeCompressEnabled, LogsEvent::DirUndefined);
        mCompressionEnabled = true;
    }
    
    //Get number of digits used to match   
    int matchLen;
    LogsCommonData::getInstance().getTelNumMatchLen(matchLen);
    LOGS_QDEBUG_2( "logs [ENG]    Tel number match length", matchLen )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::clearList
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::clearList(LogsModel::ClearType cleartype)
{
    bool clearingStarted(false);
	if ( mLogsRemove ){
		clearingStarted = mLogsRemove->clearList(cleartype);
	}
	return clearingStarted;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::clearEvent
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::clearEvents(const QList<LogsEvent*>& events)
{
    bool asyncClearingStarted(false);
    if ( mLogsRemove ){
        bool async(false);
        int err = mLogsRemove->clearEvents(events, async);
        asyncClearingStarted = ( !err && async );
    } 
    
    if ( asyncClearingStarted ){
        // Lock reader while removing events one-by-one as reading
        // might have chance to run while removing is still in progress
        // which looks bad at UI layer.
        mReader->lock(true);
    }
    
    return asyncClearingStarted;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::markEventsSeen
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::markEventsSeen(const QList<LogsEvent*>& events)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::markEventsSeen()" )
    
    if ( !mReader ){
        return false;
    }
    
    foreach( LogsEvent* ev, events ){
        if ( !mEventsSeen.contains(*ev) ){
            mEventsSeen.append(*ev);
            foreach ( const LogsEvent& mergedEv, ev->mergedDuplicates() ){
                if ( !mEventsSeen.contains(mergedEv) ){
                    mEventsSeen.append(mergedEv);
                }
            }
        }
    }
    
    int err = doMarkEventSeen();
    LOGS_QDEBUG_2( "logs [ENG] <- LogsDbConnector::markEventsSeen(), marking err:", 
                   err )
    return ( err == 0 );
}

// ----------------------------------------------------------------------------
// LogsDbConnector::readDuplicates
// ----------------------------------------------------------------------------
//
int LogsDbConnector::readDuplicates(int eventId)
{
    LOGS_QDEBUG_2( "logs [ENG] -> LogsDbConnector::readDuplicates(), id", eventId )
    if ( !mReader ){
        return -1;
    }
    qDeleteAll(mDuplicatedEvents);
    mDuplicatedEvents.clear();
    return mReader->readDuplicates(eventId);
}

// ----------------------------------------------------------------------------
// LogsDbConnector::takeDuplicates
// ----------------------------------------------------------------------------
//
QList<LogsEvent*> LogsDbConnector::takeDuplicates()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::takeDuplicates()" )
    QList<LogsEvent*> duplicates = mDuplicatedEvents;
    mDuplicatedEvents.clear();
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::takeDuplicates()" )
    return duplicates;
} 

// -----------------------------------------------------------------------------
// Do refresh only if compression has been enabled. Monitoring for database
// changes is restored and current database contents are read.
// -----------------------------------------------------------------------------
//
int LogsDbConnector::refreshData()
{
    if ( !mResourceControl ){
        return -1;
    }
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::refreshData()" )
    int err = 0;
    bool prevCompressionEnabled = mCompressionEnabled;
    mCompressionEnabled = false;
    LogsCommonData::getInstance().configureReadSize(
        logsReadSizeUndefined, LogsEvent::DirUndefined);
    if ( mReader ){
        if ( prevCompressionEnabled && !mReader->IsActive() ){
            err = mReader->start();
        }
    }
    if ( !mLogsRemove ){
        mLogsRemove = new LogsRemove( *this, mCheckAllEvents );
    }   
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::refreshData()" )
    return err;
}

// -----------------------------------------------------------------------------
// Compress data, only defined amount of events are kept in memory, others
// are deleted and read back to memory once refreshData is called. Also
// monitoring for database changes is disabled.
// -----------------------------------------------------------------------------
//
int LogsDbConnector::compressData()
{
    if ( !mResourceControl ){
        return -1;
    }
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::compressData()" )
    mCompressionEnabled = true;
    LogsCommonData::getInstance().configureReadSize(
        logsReadSizeCompressEnabled, LogsEvent::DirUndefined);
    if ( mReader ){
        int numEventsLeftInMemory = qMin(mEvents.count(), logsReadSizeCompressEnabled );
        // Simulate disappearing of compressed events
        QList<int> removedIndexes;
        for ( int i = 0; i < mEvents.count(); i++ ){
            if ( i == 0 ){
                mModelEvents.clear();
            }
            if ( i < numEventsLeftInMemory ){
                mModelEvents.append(mEvents.at(i));
            } else {
                removedIndexes.append(mEvents.at(i)->index());
            }
        }
        emit dataRemoved(removedIndexes);
        deleteInvalidEvents( numEventsLeftInMemory );
    }
    
    releaseDbConnections();
    
    mFirstReadCompleted = false; // For testing
    
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::compressData()" )
    return 0;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::handleTemporaryError
// ----------------------------------------------------------------------------
//
void LogsDbConnector::handleTemporaryError(int& error)
{
     if ( error == KErrAccessDenied ){
        LOGS_QDEBUG(
            "logs [ENG] LogsDbConnector::handleTemporaryError, DB temp unavailable" )
        // TODO: handle temporary error in some meaningful way
        error = 0;
    }
}

// ----------------------------------------------------------------------------
// LogsDbConnector::deleteInvalidEvents
// ----------------------------------------------------------------------------
//
void LogsDbConnector::deleteInvalidEvents(int newEventCount)
{
    // Remove events which are not anymore in db nor in model,
    // such events are always at end of list
    while ( mEvents.count() > newEventCount ){
        delete mEvents.takeLast();
    }
}

// ----------------------------------------------------------------------------
// LogsDbConnector::removeCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::removeCompleted()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::removeCompleted()" )
    mReader->lock(false);
    emit clearingCompleted(0);
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::removeCompleted()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::logsRemoveErrorOccured
// ----------------------------------------------------------------------------
//
void LogsDbConnector::logsRemoveErrorOccured(int err)
{
    LOGS_QDEBUG_2( "logs [ENG] <-> LogsDbConnector::logsRemoveErrorOccured(), err:", err )
    
    mReader->lock(false);
    emit clearingCompleted(err);
    // TODO: error handling
    
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::logsRemoveErrorOccured()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::readCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::readCompleted()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::readCompleted()" )
    LOGS_QDEBUG_EVENT_ARR(mEvents)
    
    // Find out updated, added and removed events
    mRemovedEventIndexes.clear();
    mUpdatedEventIndexes.clear();
    mAddedEventIndexes.clear();
    mModelEvents.clear();
    QList<LogsEvent*> toBeDeletedEvents;
    for ( int i = 0; i < mEvents.count(); i++ ){
        LogsEvent* currEvent = mEvents.at(i);
        if ( !currEvent->isInView() ){
            if ( currEvent->index() >= 0 ){
                mRemovedEventIndexes.append( currEvent->index() );
            }
            toBeDeletedEvents.append( mEvents.takeAt(i) );
            i--;
        } else {
            currEvent->setIndex(i);
            if ( currEvent->eventState() == LogsEvent::EventUpdated ) {
                mUpdatedEventIndexes.append(i);
            } else if ( currEvent->eventState() == LogsEvent::EventAdded ) {
                mAddedEventIndexes.append(i);
            }
            mModelEvents.append(currEvent);
        }
    }
    
    int changeCount = 0;
    changeCount += mRemovedEventIndexes.isEmpty() ? 0 : 1;
    changeCount += mAddedEventIndexes.isEmpty() ? 0 : 1;
    changeCount += mUpdatedEventIndexes.isEmpty() ? 0 : 1;
    
    if ( changeCount > 1 ){
        // If having many changes, it is less error prone
        // to just report reset.
        emit dataReset();
    } else {
        if ( !mRemovedEventIndexes.isEmpty() ){
            emit dataRemoved(mRemovedEventIndexes);
        }             
        if ( !mAddedEventIndexes.isEmpty() ){
            emit dataAdded(mAddedEventIndexes);
        }
        if ( !mUpdatedEventIndexes.isEmpty() ){
            emit dataUpdated(mUpdatedEventIndexes);
        }
    }  
    qDeleteAll(toBeDeletedEvents);
	
    if ( qApp && !mFirstReadCompleted){
        // Just used for testing purposes
        QMetaObject::invokeMethod(qApp, "testLogsAppEngineReady" );
        mFirstReadCompleted = true;
    }
    if ( mCompressionEnabled ){
        releaseDbConnections();
    }    
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::readCompleted()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::errorOccurred
// ----------------------------------------------------------------------------
//
void LogsDbConnector::errorOccurred(int err)
{
    LOGS_QDEBUG_2( "logs [ENG] <-> LogsDbConnector::errorOccurred(), err:", err )

    // TODO: error handling
    
    handleModifyingCompletion(err);
    
    Q_UNUSED(err)
}

// ----------------------------------------------------------------------------
// LogsDbConnector::temporaryErrorOccurred
// ----------------------------------------------------------------------------
//
void LogsDbConnector::temporaryErrorOccurred(int err)
{
    LOGS_QDEBUG_2( 
            "logs [ENG] -> LogsDbConnector::temporaryErrorOccurred(), err:", err )
    
    handleTemporaryError(err);
    
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::temporaryErrorOccurred()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::eventModifyingCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::eventModifyingCompleted()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::eventModifyingCompleted()" )
    
    if ( handleModifyingCompletion() ){
        // Possible to continue modifying
        int err = doMarkEventSeen();
        if ( err != 0 ){
            // But failed for some reason
            LOGS_QDEBUG( "logs [ENG]    Couldn't continue modifying" )
            handleModifyingCompletion(err);
        }
    }
    
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::eventModifyingCompleted()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::duplicatesReadingCompleted
// ----------------------------------------------------------------------------
//
void LogsDbConnector::duplicatesReadingCompleted(QList<LogsEvent*> duplicates)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::duplicatesReadingCompleted()" )
    qDeleteAll( mDuplicatedEvents );
    mDuplicatedEvents.clear();
    mDuplicatedEvents = duplicates;
    emit duplicatesRead();
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::duplicatesReadingCompleted()" )
}

// ----------------------------------------------------------------------------
// LogsDbConnector::doMarkEventSeen
// ----------------------------------------------------------------------------
//
int LogsDbConnector::doMarkEventSeen()
{
    int err = -1;
    if ( mEventsSeen.count() > 0 ){
        err = mReader->markEventSeen(mEventsSeen.at(0).logId());
    }
    return err;
}

// ----------------------------------------------------------------------------
// LogsDbConnector::handleModifyingCompletion
// ----------------------------------------------------------------------------
//
bool LogsDbConnector::handleModifyingCompletion(int err)
{
    bool continueModify( false );
    if ( err || mEventsSeen.count() == 1 ){
        mEventsSeen.clear();
        emit markingCompleted(err);
    } else if ( mEventsSeen.count() > 1 ){
        // Item was modified succesfully and more to modify, remove handled
        // item from queue
        mEventsSeen.takeFirst();
        continueModify = true;
    } else {
    
    }
    return continueModify;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsDbConnector::releaseDbConnections()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsDbConnector::releaseDbConnections()" )
    if ( mReader ){
        mReader->stop();
    }
    delete mLogsRemove;
    mLogsRemove = 0;
    LOGS_QDEBUG( "logs [ENG] <- LogsDbConnector::releaseDbConnections()" )
}
