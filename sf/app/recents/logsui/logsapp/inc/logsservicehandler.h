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
#ifndef LOGSSERVICEHANDLER_H
#define LOGSSERVICEHANDLER_H

#include <xqserviceprovider.h>
#include <xqaiwdecl.h>

class LogsServiceHandler : public XQServiceProvider
    {
        Q_OBJECT
    public:
        
        explicit LogsServiceHandler();
        ~LogsServiceHandler();
    
    public slots:

        int show(const QVariantMap& params);
        
    signals:
    
        void activateView(XQService::LogsViewIndex activatedView, 
                bool showDialpad, QString dialpadText);
        void activateView(QString dialpadText);
    
    public:
        
        bool isStartedUsingService() const;
        
        
    private:

        bool mIsAppStartedUsingService;
        
    private:
        friend class UT_LogsServiceHandler;
        friend class UT_LogsViewManager;
    };

#endif //LOGSSERVICEHANDLER_H
