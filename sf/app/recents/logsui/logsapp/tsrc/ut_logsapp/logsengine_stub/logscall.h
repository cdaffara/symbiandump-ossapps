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


#include <xqservicerequest.h>
#include <hbaction.h>
#include <hblineedit.h>
#include <hblabel.h>
#include <QVariant>
#include <QObject>
#include "logsevent.h"
//#include "logseventdata.h"
#include <logsexport.h>
#include <QList>
/**
 * LogsCall can be used to  make a call
 * from logs model.
 */
class LogsCall
{
  
public:
    
    enum CallType {
        TypeLogsCallNotAvailable = 0,
        TypeLogsVoiceCall,
 	   	TypeLogsVideoCall,
 	    TypeLogsVoIPCall
 	};
    
public: 
 		
    explicit LogsCall(LogsEvent& aEvent);
    explicit LogsCall(); // Stub constructor
    
    ~LogsCall();
 		
    QList<LogsCall::CallType> allowedCallTypes();
    
    LogsCall::CallType defaultCallType();
    
    static void callToNumber(LogsCall::CallType callType, const QString& number,
    	unsigned int serviceId=0);
    
    bool isAllowedCallType();
    
public slots:

    void call(LogsCall::CallType aCallType);
    void initiateCallback();
        
private: //data 
    QString mNumber;
    CallType mDefaultCall;
    LogsEvent::LogsEventType mEventType;
    
public: // test data & methods
    int mTestLastCallType;
    
    static bool isCallToNumberCalled();
    static QString lastCalledFunction();
    static QString lastCalledNumber();
    static void resetTestData();
    
};
                  
#endif // LOGSCALL_H
