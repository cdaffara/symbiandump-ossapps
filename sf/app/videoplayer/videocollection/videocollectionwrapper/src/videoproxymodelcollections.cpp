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
* Description: VideoProxyModelCollections implementation
*
*/

// Version : 

// INCLUDE FILES
#include <vcxmyvideosdefs.h>
#include "videoproxymodelgeneric.h"
#include "videoproxymodelcollections.h"
#include "videolistdatamodel.h"
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// VideoProxyModelCollections::VideoProxyModelCollections
// -----------------------------------------------------------------------------
//
VideoProxyModelCollections::VideoProxyModelCollections(QObject *parent) :
VideoProxyModelGeneric(parent)
{
	FUNC_LOG_ADDR(this);
	mDefaultSortRole = VideoCollectionCommon::KeyTitle;
}

// -----------------------------------------------------------------------------
// VideoProxyModelCollections::~VideoProxyModelCollections
// -----------------------------------------------------------------------------
//
VideoProxyModelCollections::~VideoProxyModelCollections()
{
	FUNC_LOG_ADDR(this);
}

// -----------------------------------------------------------------------------
// VideoProxyModelCollections::lessThan
// -----------------------------------------------------------------------------
//
bool VideoProxyModelCollections::lessThan(const QModelIndex &left,
    const QModelIndex &right) const
{
    if (!mModel)
    {
        return false;
    }
    int index = left.row();
    TMPXItemId leftId = mModel->mediaIdAtIndex(index);
    index = right.row();
    TMPXItemId rightId = mModel->mediaIdAtIndex(index);
    
    // Default categories are always first in the following order:
    // Recently played (missing currently)
    // Captured
    // Downloaded
    // Podcasts (missing currently)
    bool lessThan(false);
    bool proceedDataSorting(false);
    if(leftId.iId2  == KVcxMvcMediaTypeCategory &&
       rightId.iId2 == KVcxMvcMediaTypeCategory)
    {
        if(leftId.iId1 == KVcxMvcCategoryIdCaptured)
        {
            lessThan = true;
        }
        else if(leftId.iId1 == KVcxMvcCategoryIdDownloads)
        {
            if(rightId.iId1 != KVcxMvcCategoryIdCaptured)
            {
                lessThan = true;
            }
        }
    }
    else if(leftId.iId2 == KVcxMvcMediaTypeCategory ||
            rightId.iId2 == KVcxMvcMediaTypeCategory)
    {
        lessThan = (leftId.iId2 == KVcxMvcMediaTypeCategory);
    }
    else
    {
        proceedDataSorting = true;
    }

    if(!proceedDataSorting)
    {
        return mWantedSortOrder == Qt::AscendingOrder ? lessThan : !lessThan;
    }
    
    // It wasn't collection or they were in correct order, continue sorting with generic rules.
    lessThan = VideoProxyModelGeneric::lessThan(left, right);
    
    return lessThan;
}
// -----------------------------------------------------------------------------
// VideoProxyModelCollections::filterAcceptsRow
// -----------------------------------------------------------------------------
//
bool VideoProxyModelCollections::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
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

    if(mLevel == VideoCollectionCommon::ELevelCategory && id.iId2 != KVcxMvcMediaTypeVideo)
    {
        return true;
    }       

    return false;
}

// End of file
