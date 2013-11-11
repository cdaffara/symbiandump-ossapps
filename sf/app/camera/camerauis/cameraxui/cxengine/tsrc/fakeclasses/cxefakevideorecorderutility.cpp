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
#include <QTest>
#include "cxetestutils.h"
#include "cxefakevideorecorderutility.h"

CxeFakeVideoRecorderUtility::CxeFakeVideoRecorderUtility(MVideoRecorderUtilityObserver& aObserver)
    : mObserver( &aObserver )
{
    CX_DEBUG_IN_FUNCTION();
}

CxeFakeVideoRecorderUtility::~CxeFakeVideoRecorderUtility()
{
    CX_DEBUG_IN_FUNCTION();
}

CxeFakeVideoRecorderUtility::CxeFakeVideoRecorderUtility(MVideoRecorderUtilityObserver& aObserver,
                              TInt aPriority,
                              TMdaPriorityPreference aPref)
                              : mObserver( &aObserver )
{
    CX_DEBUG_IN_FUNCTION();
    Q_UNUSED(aPriority);
    Q_UNUSED(aPref);
}

void CxeFakeVideoRecorderUtility::open(int cameraHandle,
                                       const QString &/*filename*/,
                                       const QString &/*fileMimeType*/,
                                       const QString &/*supplier*/,
                                       const QString &/*videoType*/,
                                       const QString &/*audioType*/)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(cameraHandle);
    mObserver->MvruoOpenComplete(KErrNone);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFakeVideoRecorderUtility::setVideoFrameSize(const QSize &/*size*/)
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeVideoRecorderUtility::setVideoFrameRate(int rate)
{
    CX_DEBUG_IN_FUNCTION();
    Q_UNUSED(rate);
}

void CxeFakeVideoRecorderUtility::setVideoBitRate(int rate)
{
    CX_DEBUG_IN_FUNCTION();
    Q_UNUSED(rate);
}

void CxeFakeVideoRecorderUtility::setAudioEnabled(bool enabled)
{
    CX_DEBUG_IN_FUNCTION();
    Q_UNUSED(enabled);
}

void CxeFakeVideoRecorderUtility::setVideoMaxSize(int sizeInBytes)
{
    CX_DEBUG_IN_FUNCTION();
    Q_UNUSED(sizeInBytes);
}

void CxeFakeVideoRecorderUtility::close()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeVideoRecorderUtility::prepare()
{
    CX_DEBUG_ENTER_FUNCTION();
    mObserver->MvruoPrepareComplete(KErrNone);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFakeVideoRecorderUtility::record()
{
    CX_DEBUG_IN_FUNCTION();
}

void CxeFakeVideoRecorderUtility::stop(bool asynchronous)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(asynchronous);
    mObserver->MvruoRecordComplete(KErrNone);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeFakeVideoRecorderUtility::pause()
{
    CX_DEBUG_IN_FUNCTION();
}

int CxeFakeVideoRecorderUtility::availableRecordingTime()
{
    CX_DEBUG_IN_FUNCTION();
    // 100 s
    return 100;
}

int CxeFakeVideoRecorderUtility::duration()
{
    CX_DEBUG_IN_FUNCTION();
    // 10 s
    return 10;
}



// end of file
