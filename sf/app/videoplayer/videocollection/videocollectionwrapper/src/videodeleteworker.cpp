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
* Description: VideoDeleteWorker class implementation
* 
*/

// Version : %version: %

// INCLUDE FILES
#include <qtimer.h>

#include "videocollectioncommon.h"
#include "videocollectionclient.h"
#include "videodeleteworker.h"
#include "videocollectiontrace.h"

// ================= MEMBER FUNCTIONS =======================
//

/**
 * private global inline hash function for TMPXItemId keys in QSet
 */
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 

    return qHash(keyPair);
}

// -----------------------------------------------------------------------------
// VideoDeleteWorker()
// -----------------------------------------------------------------------------
//
VideoDeleteWorker::VideoDeleteWorker(VideoCollectionClient &collection, QObject *parent) :
QObject(parent),
mCollectionClient(collection),
mRequestWaitTimer(0),
mLastStatus(0),
mLastStatusData(QVariant())
{
	FUNC_LOG;
    
}

// -----------------------------------------------------------------------------
// ~VideoDeleteWorker()
// -----------------------------------------------------------------------------
//
VideoDeleteWorker::~VideoDeleteWorker()
{
	FUNC_LOG;
    if(mRequestWaitTimer && mRequestWaitTimer->isActive())
    {
        mRequestWaitTimer->stop();
    }
    // if we're still doing some background deletion, 
    // dump all deletes to collection and stop timer
    flushAll();
}

// -----------------------------------------------------------------------------
// requestDelete()
// -----------------------------------------------------------------------------
//
void VideoDeleteWorker::requestDelete(const QList<TMPXItemId> &indexList)
{
	FUNC_LOG;
    if(!mRequestWaitTimer)
    {
        mRequestWaitTimer = new QTimer();
        connect(mRequestWaitTimer, SIGNAL(timeout()), this, SLOT(execDeleteBlockSlot()));
    }
    
    mRemoveBuffer.unite(QSet<TMPXItemId>::fromList(indexList));

    if(!mRequestWaitTimer->isActive())
    {
        // first startup throught zero timer, after that
        // deletion is chained. Next delete block starts from
        // modelChangedSlot
        mRequestWaitTimer->setSingleShot(true);
        mRequestWaitTimer->start(0);
    }
}

// -----------------------------------------------------------------------------
// removeFromRequest()
// -----------------------------------------------------------------------------
//
int VideoDeleteWorker::removeFromRequest(TMPXItemId &itemId)
{
	FUNC_LOG;
    mRemoveBuffer.remove(itemId);
    return mRemoveBuffer.count();
}
   
// -----------------------------------------------------------------------------
// isDeleting()
// -----------------------------------------------------------------------------
//
bool VideoDeleteWorker::isDeleting()
{
	FUNC_LOG;
    return mRemoveBuffer.count() ? true : false;
}

// -----------------------------------------------------------------------------
// updateStatus()
// -----------------------------------------------------------------------------
//
void VideoDeleteWorker::updateStatus(int status, QVariant data)
{
	FUNC_LOG;
	INFO_1("VideoDeleteWorker::updateStatus() status: %d", status);
	
    // do not update invalid status
    if(status != VideoCollectionCommon::statusDeleteSucceed && 
       status != VideoCollectionCommon::statusSingleDeleteFail &&
       status != VideoCollectionCommon::statusMultipleDeleteFail)
    {
        return;
    }
    
    if(!mLastStatus || mLastStatus == VideoCollectionCommon::statusDeleteSucceed)
    {
        mLastStatus = status;
        mLastStatusData = data;
        return;    
    }
    if(status == VideoCollectionCommon::statusDeleteSucceed)
    {
        return;
    }
    int count = 0;
    if(mLastStatus == VideoCollectionCommon::statusSingleDeleteFail)
    {
        // old status was single fail
        mLastStatus = VideoCollectionCommon::statusMultipleDeleteFail;
        count = 2;
        if(status == VideoCollectionCommon::statusMultipleDeleteFail)
        {
            count = data.toInt() + 1;
        }
        // count of failed is now 2
        mLastStatusData = count;
        return;
    }
    // all other cases mean multi
    count = data.toInt();
    mLastStatus = VideoCollectionCommon::statusMultipleDeleteFail;
    count ? mLastStatusData = mLastStatusData.toInt() + count : 
                        mLastStatusData = mLastStatusData.toInt() + 1;
}

// -----------------------------------------------------------------------------
// getStatus()
// -----------------------------------------------------------------------------
//
int VideoDeleteWorker::getLastStatus(QVariant &data)
{
	FUNC_LOG;
    data = mLastStatusData;
    return mLastStatus;
}

// -----------------------------------------------------------------------------
// clearStatus()
// -----------------------------------------------------------------------------
//
void VideoDeleteWorker::clearStatus()
{
	FUNC_LOG;
    mLastStatus = 0;
    mLastStatusData = QVariant();
}

// -----------------------------------------------------------------------------
// continueSlot()
// -----------------------------------------------------------------------------
//
void VideoDeleteWorker::continueSlot()
{
    FUNC_LOG;
    if(!mRequestWaitTimer || !mRemoveBuffer.count())
    {
        return;
    }
    if(!mRequestWaitTimer->isActive())
    {
        mRequestWaitTimer->setSingleShot(true);
        mRequestWaitTimer->start(0);
    }
}

// -----------------------------------------------------------------------------
// doBackgroundDeleteSlot()
// -----------------------------------------------------------------------------
//
void VideoDeleteWorker::execDeleteBlockSlot()
{
	FUNC_LOG;
    if(!mRemoveBuffer.count())
    {
        return;
    }
    QList<TMPXItemId> deleteBlock;
    
    // create block of max 3 items
    int counter = 0;
    QSet<TMPXItemId>::iterator iter = mRemoveBuffer.begin();
    while(iter != mRemoveBuffer.end() && counter < 3)
    {
        deleteBlock.append((*iter));
        iter = mRemoveBuffer.erase(iter);
        counter++;
    }
    
    // need to handle errors somehow
    if(mCollectionClient.deleteVideos(&deleteBlock) != 0)
    {
        // signal block delete startup failed
        emit deleteStartupFailed(deleteBlock);
    } 
}

// -----------------------------------------------------------------------------
// flushAll()
// -----------------------------------------------------------------------------
//
void VideoDeleteWorker::flushAll()
{
	FUNC_LOG;
    if(!mRemoveBuffer.count())
    {
        return;
    }
    QList<TMPXItemId> ids = mRemoveBuffer.toList();
    mCollectionClient.deleteVideos(&ids);
    mRemoveBuffer.clear();
}

// End of file
