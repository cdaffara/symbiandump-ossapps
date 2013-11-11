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
* Description:   VideoProxyModelAllVideos implementation
*
*/

// Version : 

// INCLUDE FILES
#include <vcxmyvideosdefs.h>
#include "videoproxymodelgeneric.h"
#include "videoproxymodelallvideos.h"
#include "videolistdatamodel.h"
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// VideoProxyModelAllVideos::VideoProxyModelGeneric
// -----------------------------------------------------------------------------
//
VideoProxyModelAllVideos::VideoProxyModelAllVideos(QObject *parent) :
VideoProxyModelGeneric(parent)
{
	FUNC_LOG_ADDR(this);
}

// -----------------------------------------------------------------------------
// VideoProxyModelAllVideos::~VideoProxyModelAllVideos
// -----------------------------------------------------------------------------
//
VideoProxyModelAllVideos::~VideoProxyModelAllVideos()
{
	FUNC_LOG_ADDR(this);
}

// -----------------------------------------------------------------------------
// VideoProxyModelAllVideos::filterAcceptsRow
// -----------------------------------------------------------------------------
//
bool VideoProxyModelAllVideos::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
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

    if(id.iId2 == KVcxMvcMediaTypeVideo)
    {
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------
// VideoProxyModelAllVideos::getOpenItem()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoProxyModelAllVideos::getOpenItem() const
{
	FUNC_LOG_ADDR(this);
    TMPXItemId itemId = TMPXItemId::InvalidId();
    
    if(mModel && mCollectionClient)
    {
        INFO_1("VideoProxyModelAllVideos::getOpenItem() [0x%x] all videos is open.", this);
        itemId.iId1 = KVcxMvcCategoryIdAll;
        itemId.iId2 = KVcxMvcMediaTypeCategory;
    }

    return itemId;
}

// End of file
