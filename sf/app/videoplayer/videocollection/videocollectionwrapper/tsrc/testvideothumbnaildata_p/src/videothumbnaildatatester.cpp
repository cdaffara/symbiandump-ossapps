/**
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
* Description:   helper class to test VideoThumbnailData
*
*/

// INCLUDES
#define private public
#include "videothumbnaildata_p.h"
#undef private

#include "videothumbnaildatatester.h"

// -----------------------------------------------------------------------------
// VideoTnDataTester::VideoTnDataTester()
// -----------------------------------------------------------------------------
//
VideoThumbnailDataTester::VideoThumbnailDataTester()
{
    connect(this, SIGNAL(doBackgroundFetchingSignal()), this, SLOT(doBackgroundFetching()));
    connect(this, SIGNAL(reportThumbnailsReadySignal()), this, SLOT(reportThumbnailsReadySlot()));
    connect(this, SIGNAL(aboutToQuitSignal()), this, SLOT(aboutToQuitSlot()));
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::~VideoThumbnailDataTester()
// -----------------------------------------------------------------------------
//
VideoThumbnailDataTester::~VideoThumbnailDataTester()
{
    disconnect(this, SIGNAL(doBackgroundFetchingSignal()), this, SLOT(doBackgroundFetching()));
    disconnect(this, SIGNAL(reportThumbnailsReadySignal()), this, SLOT(reportThumbnailsReadySlot()));
    disconnect(this, SIGNAL(aboutToQuitSignal()), this, SLOT(aboutToQuitSlot()));
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::emitDoBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::emitDoBackgroundFetching()
{
    emit doBackgroundFetchingSignal();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::emitDoBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::emitReportThumbnailsReady()
{
    emit reportThumbnailsReadySignal();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::emitAboutToQuit()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::emitAboutToQuit()
{
    emit aboutToQuitSignal();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::initialize()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataTester::initialize()
{
    return VideoThumbnailDataPrivate::initialize();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::cleanup()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::cleanup()
{
    VideoThumbnailDataPrivate::cleanup();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::connectSignals()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::connectSignals()
{
    VideoThumbnailDataPrivate::connectSignals();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::disconnectSignals()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::disconnectSignals()
{
    VideoThumbnailDataPrivate::disconnectSignals();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::defaultThumbnail()
// -----------------------------------------------------------------------------
//
const QIcon* VideoThumbnailDataTester::defaultThumbnail(TMPXItemId mediaId)
{
    return VideoThumbnailDataPrivate::defaultThumbnail(mediaId);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::continueBackgroundFetch()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataTester::continueBackgroundFetch()
{
    VideoThumbnailDataPrivate::continueBackgroundFetch();
}


// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::startFetchingThumbnails()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataTester::startFetchingThumbnails(const QList<QModelIndex> &indexes, int priority)
{
    return VideoThumbnailDataPrivate::startFetchingThumbnails(indexes, priority);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataTester::continueBackgroundFetch()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataTester::startFetchingThumbnail(TMPXItemId mediaId, int priority)
{
    return VideoThumbnailDataPrivate::startFetchingThumbnail(mediaId, priority);
}

// End of file

