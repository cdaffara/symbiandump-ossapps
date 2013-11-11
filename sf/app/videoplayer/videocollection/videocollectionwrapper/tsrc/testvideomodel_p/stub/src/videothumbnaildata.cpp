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
* Description: CVideoThumbnailData class implementation
* 
*/
// INCLUDE FILES
#include <QPixmap>
#include <mpxmediageneraldefs.h>

#include "..\inc\videothumbnaildata.h"

bool gFailInitilize = false;
bool gFailFetchTn = false;
bool gFailRemoveTn = false;
int gStartFetchingCounter = 0;
int gRemoveCounter = 0;

// static helper methods
// -----------------------------------------------------------------------------
// setInitializationFail
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::setInitializationFail(bool fail)
{
    gFailInitilize = fail;
}

// -----------------------------------------------------------------------------
// resetStartFetchingCount
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::resetStartFetchingCount()
{
    gStartFetchingCounter = 0;
}
    
// -----------------------------------------------------------------------------
// getStartFetchingCount
// -----------------------------------------------------------------------------
//
int VideoThumbnailData::getStartFetchingCount()
{
    return gStartFetchingCounter;
}

// -----------------------------------------------------------------------------
// resetRemoveCount
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::resetRemoveCount()
{
    gRemoveCounter = 0;
}

// -----------------------------------------------------------------------------
// getRemoveCount
// -----------------------------------------------------------------------------
//
int VideoThumbnailData::getRemoveCount()
{
    return gRemoveCounter;
}

// Stub methods.

// -----------------------------------------------------------------------------
// CVideoThumbnailData::instance()
// -----------------------------------------------------------------------------
//
VideoThumbnailData &VideoThumbnailData::instance() 
{
    static VideoThumbnailData _thumbnailData;
    return _thumbnailData;
}

// -----------------------------------------------------------------------------
// CVideoThumbnailData::CVideoThumbnailData()
// -----------------------------------------------------------------------------
//
VideoThumbnailData::VideoThumbnailData() 
{
}

// -----------------------------------------------------------------------------
// CVideoThumbnailData::~CVideoThumbnailData()
// -----------------------------------------------------------------------------
//
VideoThumbnailData::~VideoThumbnailData()
{
   
}

// -----------------------------------------------------------------------------
// CVideoThumbnailData::initialize()
// -----------------------------------------------------------------------------
//
int VideoThumbnailData::initialize()
{  
    if(gFailInitilize)
    {
        return -1;
    }
    return 0;    
}

// -----------------------------------------------------------------------------
// CVideoThumbnailData::getThumbnail()
// -----------------------------------------------------------------------------
//
const QIcon* VideoThumbnailData::getThumbnail( TMPXItemId mediaId ) 
{
    if(mediaId == TMPXItemId::InvalidId())
    {
        return 0;
    }
    return &emptyIcon;
}

// -----------------------------------------------------------------------------
// CVideoThumbnailData::startFetchingThumbnail()
// -----------------------------------------------------------------------------
//
int VideoThumbnailData::startFetchingThumbnail( int mediaId, int priority )
{
    Q_UNUSED(mediaId);
    Q_UNUSED(priority);
    gStartFetchingCounter++;
    if(gFailFetchTn)
    {
        return -1;
    }
    return 0;    
}

// -----------------------------------------------------------------------------
// CVideoThumbnailData::defaultThumbnail()
// -----------------------------------------------------------------------------
//
const QIcon* VideoThumbnailData::defaultThumbnail()
{
    return 0;
}

// -----------------------------------------------------------------------------
// CVideoThumbnailData::removeThumbnail()
// -----------------------------------------------------------------------------
//
int VideoThumbnailData::removeThumbnail( TMPXItemId mediaId )
{
    Q_UNUSED(mediaId);
    gRemoveCounter++;
    if(gFailRemoveTn)
    {
        return -1;
    }
    return 1;
  
}

// End of file
