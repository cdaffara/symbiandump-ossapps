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
#include "logseventdata.h"
#include "logsevent.h"
#include "logseventdataparser.h"
#include "logslogger.h"

#include <QRegExp>
#include <QRegExpValidator>
#include <QDataStream>

// ----------------------------------------------------------------------------
// LogsEventData::LogsEventData
// ----------------------------------------------------------------------------
//
LogsEventData::LogsEventData() :
    mIsCNAP( false ),
    mIsVT( false ),
    mIsPoC( false ),
    mIsVoIP( false ),
    mIsEmerg( false ),
    mDataSent( 0 ),
    mDataReceived( 0 ),
    mMsgPartsNumber( 0 ),
    mServiceId( 0 ),
    mContactLocalId( 0 ),
    mRemoteUrl(),
    mLocalUrl()
{
}

// ----------------------------------------------------------------------------
// LogsEventData::LogsEventData
// ----------------------------------------------------------------------------
//
LogsEventData::LogsEventData( const LogsEventData& data )
{
    mIsCNAP = data.mIsCNAP;
    mIsVT = data.mIsVT;
    mIsPoC = data.mIsPoC;
    mIsVoIP = data.mIsVoIP;
    mIsEmerg = data.mIsEmerg;
    mDataSent = data.mDataSent;
    mDataReceived = data.mDataReceived;        
    mMsgPartsNumber = data.mMsgPartsNumber;            
    mServiceId = data.mServiceId;
    mContactLocalId = data.mContactLocalId;
    mRemoteUrl = data.mRemoteUrl;
    mLocalUrl = data.mLocalUrl;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
LogsEventData::LogsEventData( QDataStream& serializedEvent )
{
    LOGS_QDEBUG( "logs [ENG] -> LogsEventData::LogsEventData deserialize")
        
    serializedEvent >> mIsCNAP;
    serializedEvent >> mIsVT;
    serializedEvent >> mIsPoC;
    serializedEvent >> mIsVoIP;
    serializedEvent >> mIsEmerg;
    serializedEvent >> mDataSent;
    serializedEvent >> mDataReceived;        
    serializedEvent >> mMsgPartsNumber;            
    serializedEvent >> mServiceId;
    serializedEvent >> mContactLocalId;
    serializedEvent >> mRemoteUrl;
    serializedEvent >> mLocalUrl;
    
    LOGS_QDEBUG( "logs [ENG] <- LogsEventData::LogsEventData deserialize")
    
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsEventData::serialize( QDataStream& serializeDestination )
{
    LOGS_QDEBUG( "logs [ENG] -> LogsEventData::serialize")
        
    serializeDestination << mIsCNAP;
    serializeDestination << mIsVT;
    serializeDestination << mIsPoC;
    serializeDestination << mIsVoIP;
    serializeDestination << mIsEmerg;
    serializeDestination << mDataSent;
    serializeDestination << mDataReceived;        
    serializeDestination << mMsgPartsNumber;            
    serializeDestination << mServiceId;
    serializeDestination << mContactLocalId;
    serializeDestination << mRemoteUrl;
    serializeDestination << mLocalUrl;
    
    LOGS_QDEBUG( "logs [ENG] <- LogsEventData::serialize")
    return true;
}

// ----------------------------------------------------------------------------
// LogsEventData::~LogsEventData
// ----------------------------------------------------------------------------
//
LogsEventData::~LogsEventData() 
{
}

// ----------------------------------------------------------------------------
// LogsEventData::parse
// ----------------------------------------------------------------------------
//
int LogsEventData::parse( const CLogEvent& source ) 
{  
    return LogsEventDataParser::parse(source, *this);
}

// ----------------------------------------------------------------------------
// LogsEventData::isCNAP
// ----------------------------------------------------------------------------
//
bool LogsEventData::isCNAP() const
{
    return mIsCNAP;
}

// ----------------------------------------------------------------------------
// LogsEventData::isVT
// ----------------------------------------------------------------------------
//
bool LogsEventData::isVT() const
{
    return mIsVT;  
}

// ----------------------------------------------------------------------------
// LogsEventData::isPoC
// ----------------------------------------------------------------------------
//
bool LogsEventData::isPoC() const
{
    return mIsPoC;
}

// ----------------------------------------------------------------------------
// LogsEventData::isVoIP
// ----------------------------------------------------------------------------
//
bool LogsEventData::isVoIP() const
{
    return mIsVoIP;
}

// ----------------------------------------------------------------------------
// LogsEventData::dataReceived
// ----------------------------------------------------------------------------
//
long long LogsEventData::dataReceived() const
{
    return mDataReceived;
}

// ----------------------------------------------------------------------------
// LogsEventData::dataSent
// ----------------------------------------------------------------------------
//
long long LogsEventData::dataSent() const
{
    return mDataSent;
}

// ----------------------------------------------------------------------------
// LogsEventData::isEmerg
// ----------------------------------------------------------------------------
//
bool LogsEventData::isEmerg() const
{
    return mIsEmerg;
}

// ----------------------------------------------------------------------------
// LogsEventData::MsgPartsNumber
//
// Msg parts. This info is meaningful only for sms events
// ----------------------------------------------------------------------------
//
int LogsEventData::msgPartsNumber()  const
{
    return mMsgPartsNumber;
}

// ----------------------------------------------------------------------------
// LogsEventData::ServiceId
//
// Returns the Service ID of the log event.
// ----------------------------------------------------------------------------
//
unsigned int LogsEventData::serviceId() const
{
    return mServiceId;
}

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
QString LogsEventData::remoteUrl() const
{
    return mRemoteUrl;
}

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
QString LogsEventData::localUrl() const
{
    return mLocalUrl;
}

// ----------------------------------------------------------------------------
// Checks whether event data can be used in CS context
// ----------------------------------------------------------------------------
//
bool LogsEventData::isCsCompatible()
{
    bool compatible( true );
    if ( !mRemoteUrl.isEmpty() ){
        // Url can start with plus but after that can contain only digits
        // to be CS compatible.
        QRegExp rx( "\\+{0,1}\\d*$" );
        QRegExpValidator validator(rx, 0);
        int pos( 0 );
        compatible = ( validator.validate( mRemoteUrl, pos ) == QValidator::Acceptable );
    }
    return compatible;
}


// ----------------------------------------------------------------------------
// LogsEventData::contactLocalId
// ----------------------------------------------------------------------------
//
unsigned int LogsEventData::contactLocalId() const
{
    return mContactLocalId;
}

// ----------------------------------------------------------------------------
// LogsEvent::setContactLocalId
// ----------------------------------------------------------------------------
//
void LogsEventData::setContactLocalId(unsigned int id)
{
    mContactLocalId = id;
}
