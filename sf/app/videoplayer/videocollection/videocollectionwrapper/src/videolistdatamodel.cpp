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
* Description: VideoListDataModel class implementation
* 
*/

// Version : %version: 52.1.3 %

// INCLUDE FILES
#include <hbglobal.h>
#include <vcxmyvideosdefs.h>
#include <HbStringUtil>

#include "videolistdatamodel.h"
#include "videolistdatamodel_p.h"
#include "videocollectionclient.h"
#include "videocollectioncommon.h"
#include "videocollectionutils.h"
#include "videodeleteworker.h"
#include "videocollectionwrapper.h"
#include "videocollectiontrace.h"

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoListDataModel()
// -----------------------------------------------------------------------------
//
VideoListDataModel::VideoListDataModel( QObject *parent ) 
    : QAbstractItemModel( parent )
    , d_ptr( new VideoListDataModelPrivate( this ) )
    , mCollectionClient( 0 )
    , mDeleteWorker( 0 )
    , mInitialized( false )
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// ~VideoListDataModel()
// -----------------------------------------------------------------------------
//
VideoListDataModel::~VideoListDataModel()
{
	FUNC_LOG;
    delete mDeleteWorker;
    delete d_ptr;
    delete mCollectionClient;
}

// -----------------------------------------------------------------------------
// initialize()
// -----------------------------------------------------------------------------
//
int VideoListDataModel::initialize()
{
	FUNC_LOG;
    if(mInitialized)
    {
        return 0;
    }
    if(!mCollectionClient)
    {
        mCollectionClient = new VideoCollectionClient();
        if(!mCollectionClient || mCollectionClient->initialize(d_ptr) < 0)
        {
            ERROR(-1, "VideoListDataModel::initialize() collection client setup failed.");
            delete mCollectionClient;
            mCollectionClient = 0;
            return -1;
        }
    }

    if(!mDeleteWorker)
    {
        mDeleteWorker = new VideoDeleteWorker(*mCollectionClient);
        if(!mDeleteWorker)
        {
            delete mCollectionClient;
            mCollectionClient = 0;
            return -1;
        }
    }
    
    if( d_ptr->initialize() == -1)            
    {
        ERROR(-1, "VideoListDataModel::initialize() private model init failed.");
        return -1;
    }

    mInitialized = true;
    return 0;
}

// -----------------------------------------------------------------------------
// getCollectionClient()
// -----------------------------------------------------------------------------
//
VideoCollectionClient* VideoListDataModel::getCollectionClient()
{
	FUNC_LOG;
    return mCollectionClient;
}
 
// -----------------------------------------------------------------------------
// connectSignals()
// -----------------------------------------------------------------------------
//
void VideoListDataModel::connectSignals()
{
	FUNC_LOG;

    connect(this, SIGNAL(modelChanged()), mDeleteWorker, SLOT(continueSlot()), Qt::UniqueConnection);

    connect(this, SIGNAL(modelReady()), mDeleteWorker, SLOT(continueSlot()), Qt::UniqueConnection);

    connect(mDeleteWorker, SIGNAL(deleteStartupFailed(QList<TMPXItemId>&)), 
            this, SLOT(deleteStartingFailsSlot(QList<TMPXItemId>&)), Qt::UniqueConnection);
}

// -----------------------------------------------------------------------------
// disconnectSignals()
// -----------------------------------------------------------------------------
//
void VideoListDataModel::disconnectSignals()
{
	FUNC_LOG;
    disconnect(this, SIGNAL(modelChanged()), mDeleteWorker, SLOT(continueSlot()));
    disconnect(this, SIGNAL(modelReady()), mDeleteWorker, SLOT(continueSlot()));
    disconnect(mDeleteWorker, SIGNAL(deleteStartupFailed(QList<TMPXItemId>&)), 
                this, SLOT(deleteStartingFailsSlot(QList<TMPXItemId>&)));
}

// -----------------------------------------------------------------------------
// mediaIdAtIndex()
// -----------------------------------------------------------------------------
//
const TMPXItemId& VideoListDataModel::mediaIdAtIndex(const int &index) const
{
    return d_ptr->getMediaIdFromIndex(index); 
}

// -----------------------------------------------------------------------------
// indexOfId()
// -----------------------------------------------------------------------------
//
QModelIndex VideoListDataModel::indexOfId(const TMPXItemId &id)
{
    int rowIndex = d_ptr->mMediaData.indexOfId(id);
    return index(rowIndex, 0, QModelIndex());     
}

// -----------------------------------------------------------------------------
// mediaFilePathForId()
// -----------------------------------------------------------------------------
//
QString VideoListDataModel::mediaFilePathForId(const TMPXItemId &mediaId) const
{
    return d_ptr->getFilePathForId(mediaId);;
}

// -----------------------------------------------------------------------------
// rowCount()
// -----------------------------------------------------------------------------
//
int VideoListDataModel::rowCount(const QModelIndex &parent ) const
{
    // according to Qt documentation if parent is valid this should return 0 if
    // implementing a table based implementation like this.
    if (parent.isValid())
    {
        return 0;
    }
    
    return d_ptr->getVideoCount();
}

// -----------------------------------------------------------------------------
// itemData()
// -----------------------------------------------------------------------------
//
QMap<int, QVariant> VideoListDataModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> itemData;
    
    if (index.isValid()) 
    {
        // returns only basic data of the item
        itemData.insert(Qt::DisplayRole, data(index, Qt::DisplayRole));
        itemData.insert(Qt::DecorationRole, data(index, Qt::DecorationRole)); 
        itemData.insert(Qt::BackgroundRole, data(index, Qt::BackgroundRole)); 
    }
    return itemData;

}

// -----------------------------------------------------------------------------
// prepareDetailRow()
// -----------------------------------------------------------------------------
//
QString VideoListDataModel::prepareDetailRow(int &index) const
{
    // TODO: download -status?

    QString retString;
    
    TMPXItemId itemId = mediaIdAtIndex(index);
    
    if(itemId.iId2 != KVcxMvcMediaTypeVideo) //category || album
    {
        retString = prepareVideoCountString(index);
    }
    else //video
    {
        retString = doDetailRow(index);
    }
    return retString; 
}

// -----------------------------------------------------------------------------
// prepareVideoCountString()
// -----------------------------------------------------------------------------
//
QString VideoListDataModel::prepareVideoCountString(int &index) const
{
    QString videoCountString("");

    quint32 items = d_ptr->getCategoryVideoCountFromIndex(index);
    if(items > 0)
    {
        videoCountString = hbTrId("txt_videos_dblist_val_ln_videos", items);
    }
    else
    {
        videoCountString = hbTrId("txt_videos_info_no_videos");
    }

    return videoCountString;
}

// -----------------------------------------------------------------------------
// prepareSizeString()
// -----------------------------------------------------------------------------
//
QString VideoListDataModel::prepareSizeString(int &index) const
{
    QString sizeStr("");

    quint32 size = d_ptr->getVideoSizeFromIndex(index);
    sizeStr = VideoCollectionUtils::instance().prepareSizeString(size);

    return sizeStr;
}

// -----------------------------------------------------------------------------
// VideoListDataModel::doDetailRow()
// -----------------------------------------------------------------------------
//
QString VideoListDataModel::doDetailRow(int &index) const
{
	QString detailStr("");
	
    quint32 size = d_ptr->getVideoSizeFromIndex(index);
    
    quint32 total = d_ptr->getVideodurationFromIndex(index);
	
    
	const int secondsInMinute( 60 );
    const int secondsInHour( 3600 );

    const int videoSizeGB( 0x40000000 );
	const int videoSizeHalfGB( 0x20000000 );
	const int videoSizeMB( 0x100000 );
	const int videoSizeHalfMB( 0x80000 );
	const int videoSizeKB( 0x400 );

	quint32 dispSize = 0;

    int hour = total / secondsInHour;
    total = total % secondsInHour;
    int minutes = total / secondsInMinute;
    total = total % secondsInMinute;
    int second = total;
	
    QString hrs(HbStringUtil::convertDigits(QString::number(hour)));
    if(hour < 10)
    {
    	hrs.prepend(HbStringUtil::convertDigits(QString::number(0)));
    }
    QString mins(HbStringUtil::convertDigits(QString::number(minutes)));
    if(minutes < 10)
    {
    	mins.prepend(HbStringUtil::convertDigits(QString::number(0)));
    }
    QString secs(HbStringUtil::convertDigits(QString::number(second)));
    if(second < 10)
    {
    	secs.prepend(HbStringUtil::convertDigits(QString::number(0)));
    }
	
    const char* loc = "txt_videos_dblist_captured_val_l1_l2_gb";

    if ( size >= videoSizeGB )
	{
		dispSize  = size + videoSizeHalfGB;
		dispSize /= videoSizeGB;
	}
	else if ( size >= videoSizeMB )
	{
		dispSize  = size + videoSizeHalfMB;
		dispSize /= videoSizeMB;
		loc = "txt_videos_dblist_captured_val_l1_l2_mb";
	}
	else
	{
		dispSize  = size + videoSizeKB;
		dispSize /= videoSizeKB;
		loc = "txt_videos_dblist_captured_val_l1_l2_kb";
	}
	
    detailStr = hbTrId(loc).arg(hrs).arg(mins).arg(secs).arg(
                       HbStringUtil::convertDigits(QString::number(dispSize)));
	
	return detailStr;
}

// -----------------------------------------------------------------------------
// data()
// -----------------------------------------------------------------------------
//
QVariant VideoListDataModel::data(const QModelIndex & index, int role) const
{
    QVariant returnValue = QVariant();

    if (index.isValid()) 
    {
        int rowIndex = index.row();

        if (role == Qt::DisplayRole)
        {
            QStringList itemTxt;
            itemTxt.append(d_ptr->getVideoNameFromIndex(rowIndex));
                
            // Item's 2nd row: details
            itemTxt.append(prepareDetailRow(rowIndex));
            returnValue = itemTxt;
        }
        else if (role == Qt::DecorationRole)
        {
            const QIcon *thumbnail = d_ptr->getVideoThumbnailFromIndex(rowIndex); 
            QVariant tnVariant(QVariant::Icon, thumbnail);
            returnValue = tnVariant;
        }
        else if (role == VideoCollectionCommon::KeyDateTime)
        {
            QDateTime date = d_ptr->getVideoDateFromIndex(rowIndex);
            returnValue = date;
        }
        else if(role == VideoCollectionCommon::KeySizeValue)
        {
            quint32 size = d_ptr->getVideoSizeFromIndex(rowIndex);
            returnValue = size;
        }
        else if (role == VideoCollectionCommon::KeyFilePath)
        {
            QString path = d_ptr->getFilePathFromIndex(rowIndex);
            if(!(path.isNull()) && !(path.isEmpty())) { 
                returnValue = path;
            }
        }
        else if (role == VideoCollectionCommon::KeyNumberOfItems)
        {
            quint32 numberOfItems = d_ptr->getCategoryVideoCountFromIndex(rowIndex);
            returnValue = numberOfItems;
        }
        else if (role == VideoCollectionCommon::KeyTitle)
        {
            QString title = d_ptr->getVideoNameFromIndex(rowIndex);
            returnValue = title;
        }
    }
    
    return returnValue;
}

// -----------------------------------------------------------------------------
// columnCount()
// -----------------------------------------------------------------------------
//
int VideoListDataModel::columnCount(const QModelIndex & parent) const
{
    // according to Qt documentation if parent is valid this should return 0 if
    // implementing a table based implementation like this.
    if (parent.isValid())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// -----------------------------------------------------------------------------
// index()
// -----------------------------------------------------------------------------
//
QModelIndex VideoListDataModel::index(int row, int column, const QModelIndex & /*parent*/) const
{
    if(row >= 0 && row < d_ptr->getVideoCount())
    {
        return createIndex(row, column);
    }

    return QModelIndex();
}

// -----------------------------------------------------------------------------
// parent()
// -----------------------------------------------------------------------------
//
QModelIndex VideoListDataModel::parent(const QModelIndex & /*index*/) const
{
    return QModelIndex();
}

// -----------------------------------------------------------------------------
// removeRows()
// -----------------------------------------------------------------------------
//
bool VideoListDataModel::removeRows(const QModelIndexList &indexList)
{
	FUNC_LOG;
    if(!mCollectionClient || !mDeleteWorker)
    {
        return false;
    }
    // generate list of deletable item ids
    TMPXItemId id = TMPXItemId::InvalidId();
    QList<TMPXItemId> removedIds;
    QModelIndexList sortable(indexList);
    qSort(sortable);
    
    // since items are removed from actual container when being marked
    // causing total item count to be decreased,
    // we need to mark items starting from biggest index, otherwise we might 
    // end up to mark invalid items or we are referring into null item
    QModelIndexList::const_iterator iter = sortable.constEnd();
    while(iter != sortable.constBegin())
    {
        iter--;
        beginRemoveRows(QModelIndex(), (*iter).row(), (*iter).row());
        // marking might fail if either because
        // item does no exist or index is incorrect.
        // Both reasons mean that item is incorrect and should be removed
        // from the view. No need to restain them
        id = d_ptr->markVideoRemoved((*iter));
        endRemoveRows();
        if(id != TMPXItemId::InvalidId())
        {
            removedIds.append(id);
        }
    }
    emit modelChanged();
    if(removedIds.count() > 0 )
    {    
        connectSignals();
        mDeleteWorker->requestDelete(removedIds);
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------------
// belongsToAlbum()
// -----------------------------------------------------------------------------
//
bool VideoListDataModel::belongsToAlbum(const TMPXItemId &itemId,
      TMPXItemId albumId ) const
{
    return d_ptr->belongsToAlbum(itemId, albumId);
}

// -----------------------------------------------------------------------------
// setAlbumInUse()
// -----------------------------------------------------------------------------
//
void VideoListDataModel::setAlbumInUse(const TMPXItemId &albumId)
{
	FUNC_LOG;
    d_ptr->setAlbumInUse(albumId);
}

// -----------------------------------------------------------------------------
// albumInUse()
// -----------------------------------------------------------------------------
//
const TMPXItemId& VideoListDataModel::albumInUse() const
{
	FUNC_LOG;
    return d_ptr->mCurrentAlbum;
}

// -----------------------------------------------------------------------------
// removeItemsFromAlbum()
// -----------------------------------------------------------------------------
//
int VideoListDataModel::removeItemsFromAlbum(const TMPXItemId &albumId, const QList<TMPXItemId> &items)
{
	FUNC_LOG;
    int removeCount = d_ptr->removeItemsFromAlbum(albumId, items);
    if(removeCount)
    {
        if(mCollectionClient->removeItemsFromAlbum(albumId, items) < 0)
        {
            ERROR(-1, "VideoListDataModel::removeItemsFromAlbum() remove failed.");
            return -1;
        }
        emit albumChanged();
    }
    return removeCount;
}   

// -----------------------------------------------------------------------------
// deleteStartingFailsSlot()
// -----------------------------------------------------------------------------
//
void VideoListDataModel::deleteStartingFailsSlot(QList<TMPXItemId>& ids)
{
	FUNC_LOG;
    if(ids.count())
    {        
        d_ptr->restoreRemoved(&ids);
        QVariant data = ids.count();
        reportAsyncStatus(VideoCollectionCommon::statusMultipleDeleteFail, data);
    }    
    // signal for worker to proceed
    emit modelChanged();
}

// -----------------------------------------------------------------------------
// reportAsyncStatus()
// -----------------------------------------------------------------------------
//
void VideoListDataModel::reportAsyncStatus(int statusCode, QVariant &additional)
{
	FUNC_LOG;
    bool report = true;
    if(statusCode == VideoCollectionCommon::statusSingleDeleteFail ||
       statusCode ==  VideoCollectionCommon::statusMultipleDeleteFail ||
       statusCode ==  VideoCollectionCommon::statusDeleteSucceed)
    {
        report = false;
        if(!mDeleteWorker)
        {
            return;
        }
        mDeleteWorker->updateStatus(statusCode, additional);  
        statusCode = mDeleteWorker->getLastStatus(additional);
        if(!mDeleteWorker->isDeleting())
        {
            report = true;
            // delete procedure ends, reset statuses
            mDeleteWorker->clearStatus();
            disconnectSignals();
        }
    }
    if(report)
    {
        VideoCollectionWrapper::instance().sendAsyncStatus(statusCode, additional); 
    }
}

// End of file
