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
#ifndef LOGSVIEWMANAGERSTUB_H
#define LOGSVIEWMANAGERSTUB_H

#include <QVariant>
#include <hbmainwindow.h>
#include "logsdefs.h"
#include "logsabstractviewmanager.h"


class LogsViewManagerStub : public LogsAbstractViewManager
{
public: 
    LogsViewManagerStub(HbMainWindow& window) : mMainWindow(window), 
        mViewId( LogsUnknownViewId ), mShowDialer(false), 
        mArgs(QVariant()), mPreviousActivated(false), mExitCalled(false)
    {
    }
    
    virtual ~LogsViewManagerStub()
    {
    }
    
    void reset()
    {
        mViewId = LogsUnknownViewId;
        mShowDialer = false; 
        mArgs = QVariant();
        mPreviousActivated = false;
        mExitCalled = false;
    }

public: // From LogsAbstractViewManager
    
    virtual bool activateView(LogsAppViewId viewId)
    {
        mViewId = viewId;
        return true;
    }
    virtual bool activateView(LogsAppViewId viewId, bool showDialer, QVariant args)
    {
        mViewId = viewId;
        mShowDialer = showDialer;
        mArgs = args;
        return true;
    }
    virtual bool activatePreviousView()
    {
        mPreviousActivated = true;
        return true;
    }
    
    virtual HbMainWindow& mainWindow()
    {
        return mMainWindow;
    }
    
    virtual void exitApplication()
    {
        mExitCalled = true;
    }
    
public:   
    
     HbMainWindow& mMainWindow;
     LogsAppViewId mViewId;
     bool mShowDialer; 
     QVariant mArgs;
     bool mPreviousActivated;
     bool mExitCalled;

};

#endif //LOGSVIEWMANAGERSTUB_H
