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

#ifndef LOGSAPPLICATION_H
#define LOGSAPPLICATION_H

#include <QObject>
#include <hbapplication.h>

class LogsAppSettings;

class LogsApplication : public HbApplication
{
    Q_OBJECT

public:
    LogsApplication(int &argc, char *argv[], LogsAppSettings& settings);
    ~LogsApplication();

public slots:

    void testLogsAppEngineReady();
    void testLogsHandleAppViewReady();
    void testLogsResetAppReady();
    
signals:
    /*!
    * Testing framework support.
    * Application ready signal is emitted after startup when UI is fully loaded and functional.
    */
    void applicationReady();
    
private: //data for appplicationReady signal
    bool mViewReady;
    bool mReadCompleted;
    
    friend class UT_LogsApplication;
    friend class UT_LogsViewManager;
};

#endif // LOGSAPPLICATION_H
