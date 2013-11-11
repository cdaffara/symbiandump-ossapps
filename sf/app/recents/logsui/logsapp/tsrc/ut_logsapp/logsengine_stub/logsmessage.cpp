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

#include "logsmessage.h"
//SYSTEM

bool logsTestMessageSent = false;
bool defaultMessageSent = false;
QString lostTestMessageNumber;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsMessage::LogsMessage()
{
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
LogsMessage::~LogsMessage()
{
 
}
    
// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//
bool LogsMessage::isMessagingAllowed()
{
    return false;
}    

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsMessage::sendMessage()
{
    defaultMessageSent = true;
    return false;

}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
bool LogsMessage::sendMessageToNumber(
            const QString& number, const QString& displayName, unsigned int contactId)
{
    Q_UNUSED(number);
    Q_UNUSED(displayName);
    Q_UNUSED(contactId);
    logsTestMessageSent = true;
    lostTestMessageNumber = number;
}

bool LogsMessage::isMessageSent()
{
    return logsTestMessageSent;
}

void LogsMessage::resetTestData()
{
    logsTestMessageSent = false;
    defaultMessageSent = false;
}

bool LogsMessage::isDefaultMessageSent()
{
    return defaultMessageSent;
}

QString LogsMessage::sentToNumber()
{
    return lostTestMessageNumber;
}

// End of file

