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
* Description:   VideoOperatorServicePrivate stub class implementation.
*
*/

#include "videooperatorservice_p.h"
#include "videooperatorservice_pdata.h"
    
bool VideoOperatorServicePrivateData::mLoadReturnValue = true;
QString VideoOperatorServicePrivateData::mTitleReturnValue = QString();
QString VideoOperatorServicePrivateData::mIconResourceReturnValue = QString();
int VideoOperatorServicePrivateData::mLaunchServiceCallCount = 0;
    
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoOperatorServicePrivate::VideoOperatorServicePrivate()
{
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoOperatorServicePrivate::~VideoOperatorServicePrivate()
{
}

// ---------------------------------------------------------------------------
// load
// ---------------------------------------------------------------------------
//
bool VideoOperatorServicePrivate::load(int titleKey, int iconKey, int uriKey, int uidKey)
{
    Q_UNUSED(titleKey);
    Q_UNUSED(iconKey);
    Q_UNUSED(uriKey);
    Q_UNUSED(uidKey);
    return VideoOperatorServicePrivateData::mLoadReturnValue;
}

// ---------------------------------------------------------------------------
// title
// ---------------------------------------------------------------------------
//
const QString VideoOperatorServicePrivate::title() const
{
    return VideoOperatorServicePrivateData::mTitleReturnValue;
}

// ---------------------------------------------------------------------------
// iconResource
// ---------------------------------------------------------------------------
//
const QString VideoOperatorServicePrivate::iconResource() const
{
    return VideoOperatorServicePrivateData::mIconResourceReturnValue;
}

// ---------------------------------------------------------------------------
// launchService
// ---------------------------------------------------------------------------
//
void VideoOperatorServicePrivate::launchService()
{
    VideoOperatorServicePrivateData::mLaunchServiceCallCount++;
}

// ---------------------------------------------------------------------------
// getApplicationFilenameL
// ---------------------------------------------------------------------------
//
QString VideoOperatorServicePrivate::getApplicationFilenameL(const TUid uid)
{
    Q_UNUSED(uid);
    // Not stubbed.
    return QString();
}

// ---------------------------------------------------------------------------
// bringApplicationToForeground
// ---------------------------------------------------------------------------
//
bool VideoOperatorServicePrivate::bringApplicationToForeground(const TUid uid)
{
    Q_UNUSED(uid);
    // Not stubbed.
    return true;
}

// ---------------------------------------------------------------------------
// startApplicationL
// ---------------------------------------------------------------------------
//
void VideoOperatorServicePrivate::startApplicationL(const TUid uid)
{
    Q_UNUSED(uid);
    // Not stubbed.
}

// ---------------------------------------------------------------------------
// processFinished
// ---------------------------------------------------------------------------
//   
void VideoOperatorServicePrivate::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
    // Not stubbed.
}

// ---------------------------------------------------------------------------
// processError
// ---------------------------------------------------------------------------
//
void VideoOperatorServicePrivate::processError(QProcess::ProcessError error)
{
    Q_UNUSED(error);
    // Not stubbed.
}

// End of file.
