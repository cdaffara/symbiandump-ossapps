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
* Description:   VideoProxyModelContent implementation
*
*/

// Version : 

// INCLUDE FILES
#include "videoproxymodelgeneric.h"
#include "videoproxymodelcontent.h"
#include "videolistdatamodel.h"
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// VideoProxyModelContent::VideoProxyModelGeneric
// -----------------------------------------------------------------------------
//
VideoProxyModelContent::VideoProxyModelContent(QObject *parent) :
VideoProxyModelGeneric(parent)
{
	FUNC_LOG_ADDR(this);
}

// -----------------------------------------------------------------------------
// VideoProxyModelContent::~VideoProxyModelContent
// -----------------------------------------------------------------------------
//
VideoProxyModelContent::~VideoProxyModelContent()
{
	FUNC_LOG_ADDR(this);
}

// -----------------------------------------------------------------------------
// VideoProxyModelContent::connectSignals
// -----------------------------------------------------------------------------
//
bool VideoProxyModelContent::connectSignals()
{
    FUNC_LOG_ADDR(this);
    
    if(!VideoProxyModelGeneric::connectSignals())
    {
        return false;
    }

    if(!connect(mModel, SIGNAL(albumChanged()), 
            this, SLOT(albumChangedSlot()))) 
    {
        return false;
    }

    return true;
}
   
// -----------------------------------------------------------------------------
// VideoProxyModelContent::disconnectSignals
// -----------------------------------------------------------------------------
//
void VideoProxyModelContent::disconnectSignals()
{
    FUNC_LOG_ADDR(this);
    
    VideoProxyModelGeneric::disconnectSignals();
    
    disconnect(mModel, SIGNAL(albumChanged()), 
            this, SLOT(albumChangedSlot()));
}

// -----------------------------------------------------------------------------
// VideoProxyModelContent::filterAcceptsRow
// -----------------------------------------------------------------------------
//
bool VideoProxyModelContent::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);
    
    if(!sourceModel())
    {
        return false;
    }

    if(source_row < 0 || source_row >= sourceModel()->rowCount())
    {
        return false;
    }
    
    TMPXItemId id = mModel->mediaIdAtIndex(source_row);

    // Accept item if it belongs to the open album.
    if(mModel->belongsToAlbum(id))
    {
        return true;
    }
    
    return false;
}

// -----------------------------------------------------------------------------
// VideoProxyModelContent::getOpenItem()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoProxyModelContent::getOpenItem() const
{
	FUNC_LOG_ADDR(this);
    TMPXItemId itemId = TMPXItemId::InvalidId();
    
    if(mModel && mCollectionClient)
    {
        INFO_1("VideoProxyModelContent::getOpenItem() [0x%x] category or album is open.", this);
        itemId = mModel->albumInUse();
    }

    return itemId;
}

// -----------------------------------------------------------------------------
// VideoProxyModelContent::albumChangedSlot()
// -----------------------------------------------------------------------------
//
void VideoProxyModelContent::albumChangedSlot()
{
    FUNC_LOG_ADDR(this);
    // Sort and invalidate filtering, otherwise newly created album content won't sort.
    invalidateFilter();
    setSortRole(mWantedSortRole);
    sort(0, mWantedSortOrder);
}

// End of file
