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

class XQServiceRequest;

/**
 * LogsMessage can be used to send message.
 */
class LogsMessage : public QObject 
{
    Q_OBJECT
    
public: 
 		
    explicit LogsMessage(LogsEvent& aEvent);
    explicit LogsMessage(unsigned int contactId, const QString& number, const QString& displayName);
    LOGSENGINE_EXPORT ~LogsMessage();
    
    LOGSENGINE_EXPORT static bool sendMessageToNumber(
            const QString& number, const QString& displayName = QString(), unsigned int contactId = 0);
    
    bool isMessagingAllowed();
    
public slots:

    /**
     * Send message
     * @return true if sent succesfully
     */
    LOGSENGINE_EXPORT bool sendMessage();
    
protected slots:
    void requestCompleted(const QVariant& value);
    void requestError(int err);    

private:
    static bool doSendMessageToNumber(
            XQServiceRequest& request, const QString& number, 
            const QString& displayName, unsigned int contactId);
    
private: //data 
    
    bool mIsAllowed;
    QString mNumber;
    unsigned int mContactId;
    QString mDisplayName;
    XQServiceRequest* mService;
private:
    friend class UT_LogsMessage;
    friend class UT_LogsMatchesModel;
    
};
                  
#endif // LOGSMESSAGE_H
