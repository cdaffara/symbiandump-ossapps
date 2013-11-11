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

//USER
#include "logscall.h"
#include "logsmodel.h"
#include "logsevent.h"
#include "logslogger.h"

//SYSTEM

bool logsCallToNumberCalled = false;
QString logsLastCalledFunction = QString();
QString logsLastCalledNumber = QString();

bool LogsCall::isCallToNumberCalled()
{
    return logsCallToNumberCalled;
}

QString LogsCall::lastCalledFunction()
{
    return logsLastCalledFunction;
}

QString LogsCall::lastCalledNumber()
{
    return logsLastCalledNumber;
}

void LogsCall::resetTestData()
{
    logsCallToNumberCalled = false;
    logsLastCalledFunction = QString();
    logsLastCalledNumber = QString();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsCall::LogsCall(LogsEvent& event)
:mNumber(event.number()),
 mDefaultCall( TypeLogsCallNotAvailable ),
 mEventType(event.eventType()),
 mTestLastCallType(-1)
{
    if (event.eventType() == LogsEvent::TypeVoiceCall){
        mDefaultCall = TypeLogsVoiceCall;
    } else if (event.eventType() == LogsEvent::TypeVideoCall) {
        mDefaultCall = TypeLogsVideoCall;
    } else if (event.eventType() == LogsEvent::TypeVoIPCall) {
        mDefaultCall = TypeLogsVoIPCall;
    }
}

// -----------------------------------------------------------------------------
// Stub constructor
// -----------------------------------------------------------------------------
//
LogsCall::LogsCall() :
    mDefaultCall( TypeLogsCallNotAvailable ),
    mEventType(LogsEvent::TypeVoiceCall),
    mTestLastCallType(-1)
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsCall::~LogsCall()
{
}


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsCall::callToNumber(LogsCall::CallType callType, const QString& number,
	unsigned int serviceId)
{
    Q_UNUSED(callType);
    Q_UNUSED(number);
    Q_UNUSED(serviceId);
    logsCallToNumberCalled = true;
    logsLastCalledFunction = QString("callToNumber");
    logsLastCalledNumber = number;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsCall::isAllowedCallType()
{
    return (mEventType == LogsEvent::TypeVoiceCall);
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
LogsCall::CallType LogsCall::defaultCallType()
{
    return mDefaultCall;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
QList<LogsCall::CallType> LogsCall::allowedCallTypes()
{
    QList<LogsCall::CallType> list;
    list.append(LogsCall::TypeLogsVoiceCall);
    list.append(LogsCall::TypeLogsVideoCall);
    list.append(LogsCall::TypeLogsVoIPCall);
    return list;
}    
    
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsCall::call(LogsCall::CallType callType)
{
    mTestLastCallType = callType;
    logsLastCalledFunction = QString("call");
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void LogsCall::initiateCallback()
    {
    if (mEventType == LogsEvent::TypeVoiceCall)
        {
        call(TypeLogsVoiceCall);
        }
    else if (mEventType == LogsEvent::TypeVideoCall)
        {
        call(TypeLogsVideoCall);
        }
    logsLastCalledFunction = QString("initiateCallback");
    }

// End of file

