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
#include "logsservicehandler.h"
#include "logslogger.h"
#include "logsdefs.h"
#include <xqserviceutil.h>

//SYSTEM

// -----------------------------------------------------------------------------
// LogsServiceHandler::LogsServiceHandler
// -----------------------------------------------------------------------------
//
LogsServiceHandler::LogsServiceHandler()
    : XQServiceProvider(QLatin1String("logs.com.nokia.symbian.ILogsView"),0)
{
    LOGS_QDEBUG( "logs [UI] -> LogsServiceHandler::LogsServiceHandler(), ILogsView" )
    
    publishAll();
    mIsAppStartedUsingService = XQServiceUtil::isService();
    
    LOGS_QDEBUG_2( 
        "logs [UI] <- LogsServiceHandler::LogsServiceHandler(), is service",
        mIsAppStartedUsingService )
}

// -----------------------------------------------------------------------------
// LogsServiceHandler::~LogsServiceHandler
// -----------------------------------------------------------------------------
//
LogsServiceHandler::~LogsServiceHandler()
{
    LOGS_QDEBUG( "logs [UI] <-> LogsServiceHandler::~LogsServiceHandler()" )
}

// -----------------------------------------------------------------------------
// LogsServiceHandler::show
// -----------------------------------------------------------------------------
//
int LogsServiceHandler::show(const QVariantMap& params)
{
    LOGS_QDEBUG( "logs [UI] -> LogsServiceHandler::show()" )
    int activatedView = params.value(logsViewIndexParam, 
                            QVariant((int)XQService::LogsViewAll)).toInt();
    bool showDialpad = params.value(logsShowDialpadParam, 
                            QVariant(false)).toBool();
    QString dialpadText = params.value(logsDialpadTextParam, 
                            QVariant(QString())).toString();

    LOGS_QDEBUG_2( "logs [UI] activatedView:", activatedView )
    LOGS_QDEBUG_2( "logs [UI] showDialpad:", showDialpad )
    LOGS_QDEBUG_2( "logs [UI] dialpadText:", dialpadText )
    
    if (params.contains(XQLOGS_VIEW_INDEX)) {
        activatedView = params.value(XQLOGS_VIEW_INDEX).toInt();
        LOGS_QDEBUG_2( "logs [UI] new activatedView:", activatedView )        
    }
    if (params.contains(XQLOGS_SHOW_DIALPAD)) {
        showDialpad = params.value(XQLOGS_SHOW_DIALPAD).toBool();
        LOGS_QDEBUG_2( "logs [UI] new showDialpad:", showDialpad )        
    }
    if (params.contains(XQLOGS_DIALPAD_TEXT)) {
        dialpadText = params.value(XQLOGS_DIALPAD_TEXT).toString();
        LOGS_QDEBUG_2( "logs [UI] new dialpadText:", dialpadText )        
    }
    
    if ( activatedView < XQService::LogsViewAll || 
         activatedView > XQService::LogsViewMissed ){
        LOGS_QDEBUG( "logs [UI] <- LogsServiceHandler::show(), incorrect view" )
        return -1;
    }
    
    mIsAppStartedUsingService = true;
    
    if (!dialpadText.isEmpty()) {
        emit activateView(dialpadText);
    } else {
        emit activateView((XQService::LogsViewIndex)activatedView, showDialpad, dialpadText);
    }
    LOGS_QDEBUG( "logs [UI] <- LogsServiceHandler::show()" )
    return 0;
}

// -----------------------------------------------------------------------------
// LogsServiceHandler::isStartedUsingService
// -----------------------------------------------------------------------------
//
bool LogsServiceHandler::isStartedUsingService() const
{
    return mIsAppStartedUsingService;
}
