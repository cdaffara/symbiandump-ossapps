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
* Description: VideoThumbnailData class implementation
*
*/

// Version : %version: 26.1.3 %

// INCLUDE FILES
#include "videocollectiontrace.h"
#include "videothumbnaildata.h"
#include "videothumbnaildata_p.h"

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoThumbnailData::VideoThumbnailData()
// -----------------------------------------------------------------------------
//
VideoThumbnailData &VideoThumbnailData::instance()
{
	FUNC_LOG;
    static VideoThumbnailData _thumbnailData;
    return _thumbnailData;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::VideoThumbnailData()
// -----------------------------------------------------------------------------
//
VideoThumbnailData::VideoThumbnailData() :
d_ptr(new VideoThumbnailDataPrivate())
{
	FUNC_LOG;
    connect(d_ptr, SIGNAL(thumbnailsFetched(QList<TMPXItemId>&)), 
            this, SIGNAL(thumbnailsFetched(QList<TMPXItemId>&)));
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::~VideoThumbnailData()
// -----------------------------------------------------------------------------
//
VideoThumbnailData::~VideoThumbnailData()
{
	FUNC_LOG;
    disconnect(d_ptr, SIGNAL(thumbnailsFetched(QList<TMPXItemId>&)), 
            this, SIGNAL(thumbnailsFetched(QList<TMPXItemId>&)));
    delete d_ptr;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::removeThumbnail()
// -----------------------------------------------------------------------------
//
bool VideoThumbnailData::removeThumbnail(const TMPXItemId &mediaId)
{
	INFO_2("VideoThumbnailData::removeThumbnail() mediaId (%d, %d)", mediaId.iId1, mediaId.iId2);
    return d_ptr->removeThumbnail(mediaId);
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::getThumbnail()
// -----------------------------------------------------------------------------
//
const QIcon* VideoThumbnailData::getThumbnail(const TMPXItemId &mediaId)
{
	INFO_2("VideoThumbnailData::getThumbnail() mediaId (%d, %d)", mediaId.iId1, mediaId.iId2);
    return d_ptr->getThumbnail(mediaId);
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::startBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::startBackgroundFetching(VideoProxyModelGeneric *model, int fetchIndex)
{
	INFO_2("VideoThumbnailData::startBackgroundFetching() model: 0x%x, fetchIndex: %d", model, fetchIndex);
    d_ptr->startBackgroundFetching(model, fetchIndex);
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::enableBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::enableBackgroundFetching(bool enable)
{
    INFO_1("VideoThumbnailData::enableBackgroundFetching() enable: %d", enable);
    d_ptr->enableBackgroundFetching(enable);
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::enableThumbnailCreation()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::enableThumbnailCreation(bool enable)
{
    INFO_1("VideoThumbnailData::enableThumbnailCreation() enable: %d", enable);
    d_ptr->enableThumbnailCreation(enable);
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::backgroundFetchingEnabled()
// -----------------------------------------------------------------------------
//
bool VideoThumbnailData::backgroundFetchingEnabled()
{
    return d_ptr->backgroundFetchingEnabled();
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::freeThumbnailData()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::freeThumbnailData()
{
	FUNC_LOG;
    d_ptr->freeThumbnailData();
}

// End of file
