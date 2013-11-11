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

#include "logscontact.h"
//SYSTEM

static LogsContact::RequestType logstTestRequestType = LogsContact::TypeLogsContactSave;
bool LogsContact::mServiceRequestCanceled = false;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsContact::LogsContact()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsContact::LogsContact(const QString& number): mNumber(number)
{
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsContact::~LogsContact()
{
}
    
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
LogsContact::RequestType LogsContact::allowedRequestType()
{
    return logstTestRequestType;
}    

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::open()
{
    return false;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::addNew()
{
    return false;
}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsContact::updateExisting()
{
    return false;
}

void LogsContact::setNextRequestType(LogsContact::RequestType type)
{
    logstTestRequestType = type;
}
void LogsContact::reset()
{
    logstTestRequestType = LogsContact::TypeLogsContactSave;
    mServiceRequestCanceled = false;
}

void LogsContact::cancelServiceRequest()
{
    mServiceRequestCanceled = true;
}
