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

#include "logsevent.h"
#include "logseventparser.h"
#include "logseventdata.h"
#include "logsengdefs.h"
#include "logslogger.h"

// CONSTANTS

// ----------------------------------------------------------------------------
// logsEvent::logsEvent
// ----------------------------------------------------------------------------
//
LogsEvent::LogsEvent() 
 :
    mLogId(-1),
    mDirection(DirUndefined),
    mEventType(TypeUndefined),
    mUid(0),
    mLogsEventData(0),
    mDuplicates(0),                
    mRingDuration(0),
    mIsRead(false),
    mIsALS(false),
    mDuration(0),
    mIndex(0),
    mIsInView(false),
    mContactMatched(false),
    mIsPrivate(false),
    mEventState(EventAdded)
{
}

// ----------------------------------------------------------------------------
// LogsEvent::LogsEvent
// ----------------------------------------------------------------------------
//
LogsEvent::LogsEvent( const LogsEvent& event )
{
    mLogId = event.mLogId;        
    mDirection = event.mDirection;
    mEventType = event.mEventType;
    mUid = event.mUid;
    if ( event.mLogsEventData ){
        mLogsEventData = new LogsEventData( *event.mLogsEventData );
    } else {
        mLogsEventData = 0;
    }
    mRemoteParty = event.mRemoteParty;
    mNumber = event.mNumber;
    mDuplicates = event.mDuplicates;               
    mTime = event.mTime;       
    mRingDuration = event.mRingDuration;
    mIsRead = event.mIsRead;  
    mIsALS = event.mIsALS;
    mDuration = event.mDuration;
    
    mIndex = event.mIndex;
    mIsInView = event.mIsInView;
    mContactMatched = event.mContactMatched;
    mEventState = event.mEventState;
}

// ----------------------------------------------------------------------------
// LogsEvent::~LogsEvent
// ----------------------------------------------------------------------------
//
LogsEvent::~LogsEvent() 
{
    delete mLogsEventData;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
LogsEvent::LogsEvent( QDataStream& serializedEvent )
{
    serializedEvent >> mLogId;
    mLogsEventData = 0;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsEvent::serialize( QDataStream& serializeDestination )
{
    serializeDestination << mLogId;

    return true;
}

// ----------------------------------------------------------------------------
// LogsEvent::initializeEventL
// ----------------------------------------------------------------------------
//
void LogsEvent::initializeEventL( 
    const CLogEvent& source, 
    const LogsEventStrings& strings )
{
    Q_UNUSED(source)
    Q_UNUSED(strings)
}
    

// ----------------------------------------------------------------------------
// LogsEvent::isEmergencyNumber
// Checks wether the number is an emergency number
// ----------------------------------------------------------------------------

bool LogsEvent::isEmergencyNumber(const QString& number)
{
    return ( number == logsEmergencyCall911 || number == logsEmergencyCall );             
}

// ----------------------------------------------------------------------------
// LogsEvent::setDuration()
// ----------------------------------------------------------------------------
//
void LogsEvent::setDuration( int duration )
{
    mDuration = duration;
}

// ----------------------------------------------------------------------------
// LogsEvent::setALS()
// ----------------------------------------------------------------------------
//
void LogsEvent::setALS( bool isALS ) 
{
	mIsALS = isALS;	
}

// ----------------------------------------------------------------------------
// LogsEvent::setLogId
// ----------------------------------------------------------------------------
//
void LogsEvent::setLogId( int logId )
{
    mLogId = logId;
}

// ----------------------------------------------------------------------------
// LogsEvent::setNumber
// ----------------------------------------------------------------------------
//
bool LogsEvent::setNumber( const QString& number )
    {
    bool changed( mNumber != number );
    mNumber = number;
    return changed;
    }

// ----------------------------------------------------------------------------
// LogsEvent::setRemoteParty
// ----------------------------------------------------------------------------
//
void LogsEvent::setRemoteParty( const QString& remoteParty )
{
    if ( mEventState == EventNotUpdated && mRemoteParty != remoteParty ){
        LOGS_QDEBUG( "logs [ENG] <-> LogsEvent::setRemoteParty, event updated")
        mEventState = LogsEvent::EventUpdated;
    }
    mRemoteParty = remoteParty;
}

bool LogsEvent::isRemotePartyPrivate() const
{
   return mIsPrivate;
}

// ----------------------------------------------------------------------------
// LogsEvent::setContactMatched
// ----------------------------------------------------------------------------
//
void LogsEvent::setContactMatched( bool value )
{
    mContactMatched = value;
}

// ----------------------------------------------------------------------------
// LogsEvent::contactMatched
// ----------------------------------------------------------------------------
//
bool LogsEvent::contactMatched()
{
   return mContactMatched;
}

// ----------------------------------------------------------------------------
// LogsEvent::validate
// ----------------------------------------------------------------------------
//
bool LogsEvent::validate()
{
    return ( !mNumber.isEmpty() || !mRemoteParty.isEmpty() || 
           ( mLogsEventData && !mLogsEventData->remoteUrl().isEmpty() ) );
}

// ----------------------------------------------------------------------------
// LogsEvent::directionAsString
// ----------------------------------------------------------------------------
//
QString LogsEvent::directionAsString() const
{
    QString dir;
    if ( mDirection == DirIn ) {
        dir = QString("Incoming call");
    } else if ( mDirection == DirOut ) {
        dir = QString("Outgoing call");
    } else if ( mDirection == DirMissed ) {
        dir = QString("Missed call");
    } else {
        dir = QString("Direction unknown");
    }
    return dir;
}

// ----------------------------------------------------------------------------
// LogsEvent::typeAsString
// ----------------------------------------------------------------------------
//
QString LogsEvent::typeAsString() const
{
    QString type;
    if ( mEventType == TypeVoiceCall ) {
        type = QString("Voice call");
    } else if ( mEventType == TypeVideoCall ) {
        type = QString("Video call");
    } else if ( mEventType == TypeVoIPCall ) {
        type = QString("VoIP call");
    } else {
        type = QString("Type unknown");
    }
    return type;
}

// ----------------------------------------------------------------------------
// LogsEvent::getNumberForCalling
// ----------------------------------------------------------------------------
//
QString LogsEvent::getNumberForCalling()
{
    if (mNumber.isEmpty() && mEventType == TypeVoIPCall && mLogsEventData ) {
        return mLogsEventData->remoteUrl();
    }
    return mNumber;
}

// ----------------------------------------------------------------------------
// LogsEvent::setDirection
// ----------------------------------------------------------------------------
//
bool LogsEvent::setDirection( LogsDirection direction )
{
    bool changed( mDirection != direction );
    mDirection = direction;
    return changed;
}

// ----------------------------------------------------------------------------
// LogsEvent::setEventUid
// ----------------------------------------------------------------------------
//
void LogsEvent::setEventUid( int uid )
{
    mUid = uid;
}

// ----------------------------------------------------------------------------
// LogsEvent::setEventType
// ----------------------------------------------------------------------------
//
bool LogsEvent::setEventType( LogsEventType eventType )
{
    bool changed( mEventType != eventType );
    mEventType = eventType;
    return changed;
}

// ----------------------------------------------------------------------------
// LogsEvent::ALS()
// ----------------------------------------------------------------------------
//
bool LogsEvent::ALS() const
{
	return mIsALS;
}
	
// ----------------------------------------------------------------------------
// LogsEvent::logId
// ----------------------------------------------------------------------------
//
int LogsEvent::logId() const
    {
    return mLogId;
    }

// ----------------------------------------------------------------------------
// LogsEvent::setIsRead
// ----------------------------------------------------------------------------    
void LogsEvent::setIsRead(bool isRead)
{
    mIsRead = isRead;
}


// ----------------------------------------------------------------------------
// LogsEvent::Number
// ----------------------------------------------------------------------------
//
const QString& LogsEvent::number() const
{
    return mNumber;
}

// ----------------------------------------------------------------------------
// LogsEvent::RemoteParty
// ----------------------------------------------------------------------------
//
const QString& LogsEvent::remoteParty() const
{
    return mRemoteParty;
}

// ----------------------------------------------------------------------------
// LogsEvent::Direction
// ----------------------------------------------------------------------------
//
LogsEvent::LogsDirection LogsEvent::direction() const
{
    return mDirection;
}

// ----------------------------------------------------------------------------
// LogsEvent::eventUid
// ----------------------------------------------------------------------------
//
int LogsEvent::eventUid() const
{
    return mUid;
}

// ----------------------------------------------------------------------------
// LogsEvent::eventType
// ----------------------------------------------------------------------------
//
LogsEvent::LogsEventType LogsEvent::eventType() const
{
    return mEventType;
}

// ----------------------------------------------------------------------------
// LogsEvent::isRead
// ----------------------------------------------------------------------------
 bool LogsEvent::isRead() const
{
	return mIsRead;
}

// ----------------------------------------------------------------------------
// LogsEvent::RingDuration
//
// For ring duation feature
// ----------------------------------------------------------------------------
//
int LogsEvent::ringDuration() const
{
    return mRingDuration;
}

// ----------------------------------------------------------------------------
// LogsEvent::duration
// ----------------------------------------------------------------------------
//
int LogsEvent::duration() const
{
    return mDuration;
}

// ----------------------------------------------------------------------------
// LogsEvent::SetRingDuration
// ----------------------------------------------------------------------------
//
void LogsEvent::setRingDuration( int ringDuration )
{
    mRingDuration = ringDuration;
}

// ----------------------------------------------------------------------------
// LogsEvent::LogsEventData
// ----------------------------------------------------------------------------
//
LogsEventData* LogsEvent::logsEventData() const
{
    return mLogsEventData;
}

// ----------------------------------------------------------------------------
// LogsEvent::setLogsEventData
// ----------------------------------------------------------------------------
//
void LogsEvent::setLogsEventData( LogsEventData* logsEventData )
{
    delete mLogsEventData;    
    mLogsEventData = logsEventData;
}

// ----------------------------------------------------------------------------
// LogsEvent::setTime
//
// Time needed in recent views and in detail view (not needed in event view)
// ----------------------------------------------------------------------------
//
bool LogsEvent::setTime( const QDateTime& time )
{
    bool changed( mTime != time );
    mTime = time;
    return changed;
}

// ----------------------------------------------------------------------------
// LogsEvent::time
// ----------------------------------------------------------------------------
//
QDateTime LogsEvent::time() const
{ 
    return mTime;        
}

// ----------------------------------------------------------------------------
// LogsEvent::setDuplicates
//
// Duplicates needed only in missed calls view
// ----------------------------------------------------------------------------
//
void LogsEvent::setDuplicates( int duplicates )
{
    mDuplicates = duplicates;
}

// ----------------------------------------------------------------------------
// LogsEvent::duplicates
// ----------------------------------------------------------------------------
//
int LogsEvent::duplicates() const
{
    return mDuplicates;
}

// ----------------------------------------------------------------------------
// LogsEvent::setIndex
// ----------------------------------------------------------------------------
//
void LogsEvent::setIndex(int index)
{
    mIndex = index;
}

// ----------------------------------------------------------------------------
// LogsEvent::index
// ----------------------------------------------------------------------------
//
int LogsEvent::index() const
{
    return mIndex;
}

// ----------------------------------------------------------------------------
// LogsEvent::setIsInView
// ----------------------------------------------------------------------------
//
void LogsEvent::setIsInView(bool isInView)
{
    mIsInView = isInView;
}

// ----------------------------------------------------------------------------
// LogsEvent::isInView
// ----------------------------------------------------------------------------
//
bool LogsEvent::isInView() const
{
    return mIsInView;
}

// ----------------------------------------------------------------------------
// LogsEvent::eventState
// ----------------------------------------------------------------------------
//
LogsEvent::LogsEventState LogsEvent::eventState() const
{
    return mEventState;
}


// End of file

