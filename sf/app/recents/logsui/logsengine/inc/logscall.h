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
#ifndef LOGSCALL_H
#define LOGSCALL_H

#include <QObject>
#include <QList>
#include <logsexport.h>

#include "logsevent.h"


/**
 * LogsCall can be used to  make a call
 * from logs model.
 */
class LogsCall : public QObject 
{
    Q_OBJECT
  
public:
    
    enum CallType {
        TypeLogsCallNotAvailable = 0,
        TypeLogsVoiceCall,
        TypeLogsVideoCall,
        TypeLogsVoIPCall
    };
    
public: 
 		
    explicit LogsCall(LogsEvent& aEvent);
    explicit LogsCall(unsigned int contactId, const QString& number);
    
    LOGSENGINE_EXPORT ~LogsCall();
 		
    LOGSENGINE_EXPORT QList<LogsCall::CallType> allowedCallTypes();
    
    LOGSENGINE_EXPORT LogsCall::CallType defaultCallType();
    
    LOGSENGINE_EXPORT static void callToNumber(LogsCall::CallType callType, const QString& number,
            unsigned int serviceId = 0, unsigned int contactId = 0);
    
    bool isAllowedCallType();
    
public slots:

    LOGSENGINE_EXPORT void call(LogsCall::CallType callType);
    LOGSENGINE_EXPORT void initiateCallback();
    
private: 
    
    static void createcall(QString service, QString type, QString num, bool sync);
    static void createCallWithService(QString service, QString type, QString num, 
            bool sync, unsigned int serviceId );
        
private: //data 
    
    QList<LogsCall::CallType> mCalls;
    CallType mDefaultCall;
    QString mNumber;
    unsigned int mServiceId;
    unsigned int mContactId;
    
private:
    friend class UT_LogsCall;
    
};
                  
#endif // LOGSCALL_H
