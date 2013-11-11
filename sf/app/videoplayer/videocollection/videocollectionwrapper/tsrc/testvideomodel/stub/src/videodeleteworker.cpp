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
* Description:  stubclass of videodeleteworker for testing VideoListDataModel class methods*
*/



// INCLUDE FILES
#include "videodeleteworker.h"

int VideoDeleteWorker::mCreateCount = 0;

// -----------------------------------------------------------------------------
// VideoDeleteWorker
// -----------------------------------------------------------------------------
//
VideoDeleteWorker::VideoDeleteWorker(VideoCollectionClient &collection, QObject *parent) :
mIsDeleting(false),
mStatus(-1),
mStatusData(QVariant())
{
    Q_UNUSED(collection);
    Q_UNUSED(parent);
    mCreateCount++;
}


// -----------------------------------------------------------------------------
// ~VideoDeleteWorker
// -----------------------------------------------------------------------------
//
VideoDeleteWorker::~VideoDeleteWorker()
{
    mRequestList.clear();
}

// -----------------------------------------------------------------------------
// requestDelete
// -----------------------------------------------------------------------------
//
void VideoDeleteWorker::requestDelete(const QList<TMPXItemId> &itemList)
{
    mRequestList.append(itemList);    
}
 
// -----------------------------------------------------------------------------
// isDeleting
// -----------------------------------------------------------------------------
//
bool VideoDeleteWorker::isDeleting()
{
    return mIsDeleting;
}
  
// -----------------------------------------------------------------------------
// updateStatus
// -----------------------------------------------------------------------------
//
void VideoDeleteWorker::updateStatus(int status, QVariant data)
{
    mStatus = status;
    mStatusData = data;
}

// -----------------------------------------------------------------------------
// clearStatus
// -----------------------------------------------------------------------------
//
void VideoDeleteWorker::clearStatus()
{
    mStatus = -1;
    mStatusData = QVariant();
}

// -----------------------------------------------------------------------------
// getLastStatus
// -----------------------------------------------------------------------------
//
int VideoDeleteWorker::getLastStatus(QVariant &data)
{
    data = mStatusData;
    return mStatus;
}
    


