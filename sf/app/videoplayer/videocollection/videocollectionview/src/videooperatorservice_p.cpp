/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VideoOperatorServicePrivate class implementation.
*
*/

// Version : 

// INCLUDE FILES
#include "videooperatorservice_p.h"
#include "videocollectionviewutils.h"
#include "videocollectioncenrepdefs.h"
#include "videocollectiontrace.h"

#include <qdesktopservices.h>
#include <qurl.h>
#include <e32base.h>
#include <coemain.h>
#include <apgcli.h>
#include <apaid.h>
#include <apgtask.h>

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoOperatorServicePrivate::VideoOperatorServicePrivate() :
    mProcess(0)
{
    
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoOperatorServicePrivate::~VideoOperatorServicePrivate()
{   
    if(mProcess)
    {
        disconnect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(processFinished(int, QProcess::ExitStatus)));
        
        disconnect(mProcess, SIGNAL(error(QProcess::ProcessError)), 
                this, SLOT(processError(QProcess::ProcessError)));        
        mProcess->close();
    }
    delete mProcess;
}

// ---------------------------------------------------------------------------
// load
// ---------------------------------------------------------------------------
//
bool VideoOperatorServicePrivate::load(int titleKey, int iconKey, int serviceUriKey, int appUidKey)
{
    FUNC_LOG;
    VideoCollectionViewUtils& utils = VideoCollectionViewUtils::instance();
    
    mTitle = utils.getCenRepStringValue(titleKey);
    mIconResource = utils.getCenRepStringValue(iconKey);
    mServiceUri = utils.getCenRepStringValue(serviceUriKey);
    mApplicationUid = utils.getCenRepIntValue(appUidKey);
    
    // Icon is required, either service uri or application uid is required.
    if(mIconResource.isEmpty() || (mServiceUri.isEmpty() && mApplicationUid <= 0))
    {
        return false;
    }
    
    return true;
}

// ---------------------------------------------------------------------------
// title
// ---------------------------------------------------------------------------
//
const QString VideoOperatorServicePrivate::title() const
{
    return mTitle;
}

// ---------------------------------------------------------------------------
// iconResource
// ---------------------------------------------------------------------------
//
const QString VideoOperatorServicePrivate::iconResource() const
{
    return mIconResource;
}

// ---------------------------------------------------------------------------
// launchService
// ---------------------------------------------------------------------------
//
void VideoOperatorServicePrivate::launchService()
{
    FUNC_LOG;
    
    if(!mServiceUri.isEmpty())
    {
        INFOQSTR_1("VideoOperatorServicePrivate::launchService() starting url: %S", mServiceUri);
        QDesktopServices::openUrl(QUrl(mServiceUri));
    }
    else
    {
        INFO_1("VideoOperatorServicePrivate::launchService() starting application 0x%x", mApplicationUid);
        TRAPD(err, startApplicationL(TUid::Uid(mApplicationUid)));
        if(err)
        {
            INFO_1("VideoOperatorServicePrivate::launchService() failed to start, error: %d", err);
        }
    }
}

// ---------------------------------------------------------------------------
// getApplicationFilenameL
// ---------------------------------------------------------------------------
//
QString VideoOperatorServicePrivate::getApplicationFilenameL(const TUid uid)
{
    FUNC_LOG;

    // TApaAppInfo size is greater then 1024 bytes
    // so its instances should not be created on the stack.
    TApaAppInfo* appInfo = new (ELeave) TApaAppInfo;
    CleanupStack::PushL(appInfo);
    RApaLsSession appArcSession;
    User::LeaveIfError(appArcSession.Connect());
    CleanupClosePushL<RApaLsSession>(appArcSession);

    User::LeaveIfError(appArcSession.GetAppInfo(*appInfo, uid));

    QString fullName((QChar*)appInfo->iFullName.Ptr(), appInfo->iFullName.Length()); 
    
    CleanupStack::PopAndDestroy(&appArcSession);
    CleanupStack::PopAndDestroy(appInfo);
    
    return fullName;
}

// ---------------------------------------------------------------------------
// bringApplicationToForeground
// ---------------------------------------------------------------------------
//
bool VideoOperatorServicePrivate::bringApplicationToForeground(const TUid uid)
{
    FUNC_LOG;
    
    CCoeEnv* coe = CCoeEnv::Static();
    bool ret = false;
    if(coe)
    {
        TApaTaskList taskList(coe->WsSession());
        TApaTask task(taskList.FindApp(uid));
        
        if(task.Exists())
        {
            INFO("VideoOperatorServicePrivate::processFinished() task found.");
            task.BringToForeground();
            ret = true;
        }
    }
    return ret;
}

// ---------------------------------------------------------------------------
// startApplicationL
// ---------------------------------------------------------------------------
//
void VideoOperatorServicePrivate::startApplicationL(const TUid uid)
{
    FUNC_LOG;
    
    // Process handle already exists, try to bring app to foreground.
    if(mProcess)
    {
        if(bringApplicationToForeground(uid))
        {
            // App brought to foreground, all done.
            return;
        }
        // Application is not running, cleanup previous process.
        disconnect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                this, SLOT(processFinished(int, QProcess::ExitStatus)));
        
        disconnect(mProcess, SIGNAL(error(QProcess::ProcessError)), 
                this, SLOT(processError(QProcess::ProcessError)));

        delete mProcess;
        mProcess = 0;
    }
    
    // Create new process and start it.
    mProcess = new QProcess();

    if(!connect(mProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(processFinished(int, QProcess::ExitStatus)), Qt::UniqueConnection) ||
       !connect(mProcess, SIGNAL(error(QProcess::ProcessError)), 
            this, SLOT(processError(QProcess::ProcessError)), Qt::UniqueConnection))
    {
        INFO("VideoOperatorServicePrivate::startApplicationL() failed to connect signals");
        User::Leave(KErrGeneral);
    }

    QString appName = getApplicationFilenameL(uid);
    if(!appName.isEmpty())
    {
        mProcess->start(appName);
    }
}

// ---------------------------------------------------------------------------
// processFinished
// ---------------------------------------------------------------------------
//
void VideoOperatorServicePrivate::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    INFO_2("VideoOperatorServicePrivate::processFinished() code: %d, status: %d", exitCode, exitStatus);
}

// ---------------------------------------------------------------------------
// processError
// ---------------------------------------------------------------------------
//
void VideoOperatorServicePrivate::processError(QProcess::ProcessError error)
{
    Q_UNUSED(error);
    INFO_1("VideoOperatorServicePrivate::processError() error: %d", error);
}

// End of file.
