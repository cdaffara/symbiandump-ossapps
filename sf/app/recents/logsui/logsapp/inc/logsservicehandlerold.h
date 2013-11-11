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
#ifndef LOGSSERVICEHANDLEROLD_H
#define LOGSSERVICEHANDLEROLD_H

#include <xqserviceprovider.h>
#include <xqaiwdecl.h>

class LogsServiceHandlerOld : public XQServiceProvider
    {
        Q_OBJECT
    public:
        explicit LogsServiceHandlerOld();
        ~LogsServiceHandlerOld();
    
    public slots:
        int start(int activatedView, bool showDialpad);
        int startWithNum(int activatedView, bool showDialpad, QString dialpadText);
        
    signals:
    
        void activateView(XQService::LogsViewIndex activatedView, bool showDialpad, QString dialpadText);
        void activateView(QString dialpadText);
    
    public:
        
        bool isStartedUsingService() const;
        
    private:
        bool mIsAppStartedUsingService;
        
    private:
        friend class UT_LogsServiceHandler;
        friend class UT_LogsViewManager;
    };

#endif //LOGSSERVICEHANDLEROLD_H
