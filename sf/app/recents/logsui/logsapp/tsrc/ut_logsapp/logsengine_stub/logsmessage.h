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
#ifndef LOGSMESSAGE_H
#define LOGSMESSAGE_H


#include <QObject>
#include <logsexport.h>
#include "logsevent.h"

/**
 * LogsMessage can be used to send message.
 */
class LogsMessage  
{
  
    
public: 
 		
    explicit LogsMessage();
    ~LogsMessage();
    
    bool isMessagingAllowed();
    
public slots:

    /**
     * Send message
     * @return true if sent succesfully
     */
    bool sendMessage();
    static bool sendMessageToNumber(
            const QString& number, const QString& displayName = QString(), unsigned int contactId = 0);
        
private: //data 
    LogsEvent mEvent;

public: // test helpers
    static bool isMessageSent();
    static void resetTestData();
    static bool isDefaultMessageSent();
    static QString sentToNumber();
    
private:
    friend class UT_LogsMessage;
    
};
                  
#endif // LOGSMESSAGE_H
