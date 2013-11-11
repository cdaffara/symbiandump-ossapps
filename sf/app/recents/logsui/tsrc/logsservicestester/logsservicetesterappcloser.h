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

#ifndef LOGSSERVICETESTERAPPCLOSER_H
#define LOGSSERVICETESTERAPPCLOSER_H

#include <e32base.h>
#include <apgtask.h>
#include <QTimer>


class CAppCloseWatcher;

/**
 * 
 */
class MCloseOberver
{
public:
    virtual void AppClosed(TInt aError) = 0;
};


/**
 * 
 */
class LogsServiceTesterAppCloser : public QObject, public MCloseOberver 
{
    Q_OBJECT
public:
    
    explicit LogsServiceTesterAppCloser(QObject *parent=0);
    ~LogsServiceTesterAppCloser();
    
    enum AppCloserErrors {
        ErrorAppNotRunning = 1,
        ErrorClosingOngoing,
        ErrorClosingTimeout,
        ErrorAppRunning
    };
    
public slots:

    void closeApp(TApaTask& aTask);
    void closeDialerApp();
    void startDialerAtBg();

signals:
        
    void closed();
    void closeError(int error);
    
private slots:
    
    void closeAppTimeout();
    
private:
    
    // from MCloseOberver
    virtual void AppClosed(TInt aError);
    
    bool isDialerRunning(bool close = false);
    
private:
    QTimer mTimer;
    CAppCloseWatcher* mAppCloseWatch;
};


/**
 * 
 */
class CAppCloseWatcher : public CActive
    {
public:
    
    CAppCloseWatcher(MCloseOberver& aObserver);
    ~CAppCloseWatcher();
    
    // Start watching application death
    void Start(const TApaTask& aTask);
    
private:
    
    void DoCancel();
    void RunL();
    
private: //data

    //A handle to the main thread of the application to be closed.
    RThread iThread;
    
    //A backup of the orginal priority of the application's process
    TProcessPriority iOriginalProcessPriority;
    
    MCloseOberver& iObserver;
    };

#endif // LOGSSERVICETESTERAPPCLOSER_H

