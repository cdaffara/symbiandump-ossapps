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
* Description: VideoListDataModelPrivate class implementation
* 
*/

// Version : %version: 38.1.7 %

// INCLUDE FILES
#include <hbglobal.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediavideodefs.h>
#include <thumbnailmanager_qt.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <qvariant.h>
#include <hbextendedlocale.h>
#include <qfileinfo.h>

#include "videolistdatamodel.h"
#include "videocollectionclient.h"
#include "videodeleteworker.h"
#include "videolistdatamodel_p.h"
#include "videothumbnaildata.h"
#include "vcxmyvideosdefs.h"
#include "videocollectionutils.h"
#include "videocollectioncommon.h"
#include "videocollectiontrace.h"

/**
 * global qHash function required fo creating hash values for TMPXItemId -keys
 */
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2); 
    return qHash(keyPair);
}

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoListDataModelPrivate
// -----------------------------------------------------------------------------
//
VideoListDataModelPrivate::VideoListDataModelPrivate(VideoListDataModel *model) :
q_ptr(model),
mVideoThumbnailData(0),
mInitialized(false),
mCurrentAlbum(TMPXItemId::InvalidId())
{
	FUNC_LOG;
}
	
// -----------------------------------------------------------------------------
// ~VideoListDataModelPrivate
// -----------------------------------------------------------------------------
//
VideoListDataModelPrivate::~VideoListDataModelPrivate()
{
	FUNC_LOG;
    QHash<TMPXItemId, QSet<TMPXItemId> >::iterator iter = mAlbumData.begin();
    while(iter != mAlbumData.end())
    {
        iter->clear();
        ++iter;
    }
    mAlbumData.clear();
    mMediaData.clear();
}

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
//
int VideoListDataModelPrivate::initialize()
{
	FUNC_LOG;
    if(mInitialized)
    {
        return 0;
    }
    mVideoThumbnailData = &(VideoThumbnailData::instance());
    if(!connect( mVideoThumbnailData, SIGNAL(thumbnailsFetched( QList<TMPXItemId>& )), 
                  this, SLOT(thumbnailsFetchedSlot( QList<TMPXItemId>& ))))
    {
        return -1;
    }

    mInitialized = true;
    return 0;
}

// -----------------------------------------------------------------------------
// getVideoCount
// -----------------------------------------------------------------------------
//
int VideoListDataModelPrivate::getVideoCount()
{
    return mMediaData.count();
}

// -----------------------------------------------------------------------------
// getMediaIdFromIndex
// -----------------------------------------------------------------------------
//
const TMPXItemId& VideoListDataModelPrivate::getMediaIdFromIndex(const int &index ) const
{
    return mMediaData.idFromIndex(index);
}

// -----------------------------------------------------------------------------
// getVideoNameFromIndex
// -----------------------------------------------------------------------------
//
const QString VideoListDataModelPrivate::getVideoNameFromIndex(const int &index )  const
{
    CMPXMedia* media = mMediaData.fromIndex(index);
    QString text;

    TMPXItemId mpxId( 0, 0 );
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>(media, KMPXMediaGeneralId, mpxId );
    
    //Localize default categories
    if(mpxId.iId2 == KVcxMvcMediaTypeCategory)
    {
        switch(mpxId.iId1)
        {
            case KVcxMvcCategoryIdDownloads:
                text = hbTrId("txt_videos_dblist_downloaded");
                break;
            case KVcxMvcCategoryIdCaptured:
                text = hbTrId("txt_videos_dblist_captured");
                break;
        }
    }
    else
    {
        VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralTitle, text );
    }
    return text;
}
 
// -----------------------------------------------------------------------------
// getVideoThumbnailFromIndex
// -----------------------------------------------------------------------------
//
const QIcon* VideoListDataModelPrivate::getVideoThumbnailFromIndex(const int &index ) const
{
    if(mVideoThumbnailData)
    {
        return mVideoThumbnailData->getThumbnail( mMediaData.idFromIndex(index) );
    }
    return 0;
}

// -----------------------------------------------------------------------------
// getCategoryVideoCountFromIndex
// -----------------------------------------------------------------------------
//
quint32 VideoListDataModelPrivate::getCategoryVideoCountFromIndex(const int &index ) const
{
    quint32 count(0);
    CMPXMedia *media = mMediaData.fromIndex(index);
    VideoCollectionUtils::instance().mediaValue<quint32>(media, KVcxMediaMyVideosCategoryItemCount, count );
    return count;
}

// -----------------------------------------------------------------------------
// getVideoSizeFromIndex
// -----------------------------------------------------------------------------
//
quint32 VideoListDataModelPrivate::getVideoSizeFromIndex(const int &index ) const
{
    quint32 size(0);
    CMPXMedia *media = mMediaData.fromIndex(index);
    VideoCollectionUtils::instance().mediaValue<quint32>(media, KMPXMediaGeneralSize, size );    
    return size;
}

// -----------------------------------------------------------------------------
// getVideoAgeProfileFromIndex
// ---------------------------------------------------------------------------
//
quint32 VideoListDataModelPrivate::getVideoAgeProfileFromIndex(const int &index ) const
{
    quint32 ageProfile(0);
    CMPXMedia *media = mMediaData.fromIndex(index);
    VideoCollectionUtils::instance().mediaValue<quint32>(media, KVcxMediaMyVideosAgeProfile, ageProfile );    
    return ageProfile; 
}

// -----------------------------------------------------------------------------
// getVideodurationFromIndex
// -----------------------------------------------------------------------------
//
quint32 VideoListDataModelPrivate::getVideodurationFromIndex(const int &index ) const
{
    CMPXMedia *media = mMediaData.fromIndex(index);
    return getVideoDuration(media);
}

// -----------------------------------------------------------------------------
// getVideodurationFromIndex
// -----------------------------------------------------------------------------
//
quint32 VideoListDataModelPrivate::getVideoDuration(CMPXMedia* media) const
{
    quint32 returnDuration(0);
    float duration(0);
    VideoCollectionUtils::instance().mediaValue<float>(media, KVcxMediaMyVideosDuration, duration );
    returnDuration = static_cast<quint32>(duration);
    return returnDuration;
}

// -----------------------------------------------------------------------------
// getVideoDateFromIndex
// -----------------------------------------------------------------------------
//
QDateTime VideoListDataModelPrivate::getVideoDateFromIndex(const int &index ) const
{
    return getVideoDate(mMediaData.fromIndex(index));
}

// -----------------------------------------------------------------------------
// getVideoDate
// -----------------------------------------------------------------------------
//
QDateTime VideoListDataModelPrivate::getVideoDate(CMPXMedia* media) const
{
    QDateTime date;
    quint64 dateTimeValue;
    if(!VideoCollectionUtils::instance().mediaValue<quint64>(media, KMPXMediaGeneralDate, dateTimeValue)) 
    {
        return date;
    }
    TDateTime temp = TTime( dateTimeValue ).DateTime();
    date = QDateTime(QDate(temp.Year(), temp.Month()+1, temp.Day()+1), 
                       QTime(temp.Hour(), temp.Minute(), temp.Second(), temp.MicroSecond()));
    return date;
}

// -----------------------------------------------------------------------------
// markVideosRemoved
// -----------------------------------------------------------------------------
//
const TMPXItemId VideoListDataModelPrivate::markVideoRemoved(const QModelIndex &itemIndex)
{
    return mMediaData.markItemRemoved(itemIndex.row());
}

// -----------------------------------------------------------------------------
// restoreRemoved
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::restoreRemoved(QList<TMPXItemId> *idList)
{
	FUNC_LOG;
    int startIndex = mMediaData.count();
    int restored = mMediaData.restoreRemovedItems(idList);
    if(restored > 0)
    {
        q_ptr->beginInsertRows(QModelIndex(), startIndex, startIndex + (restored - 1));
        q_ptr->endInsertRows();
    }
}

// -----------------------------------------------------------------------------
// getFilePathFromIndex
// -----------------------------------------------------------------------------
//
const QString VideoListDataModelPrivate::getFilePathFromIndex(const int &index) const
{
    QString filePath;
    VideoCollectionUtils::instance().mediaValue<QString>(mMediaData.fromIndex(index), 
            KMPXMediaGeneralUri, filePath );
    return filePath;
}

// -----------------------------------------------------------------------------
// getFilePathForId
// -----------------------------------------------------------------------------
//
const QString VideoListDataModelPrivate::getFilePathForId(const TMPXItemId &mediaId)
{
    QString filePath;
    int index = mMediaData.indexOfId(mediaId);
    VideoCollectionUtils::instance().mediaValue<QString>(mMediaData.fromIndex(index), 
            KMPXMediaGeneralUri, filePath );
    return filePath;
}

// -----------------------------------------------------------------------------
// belongsToAlbum
// -----------------------------------------------------------------------------
//
bool VideoListDataModelPrivate::belongsToAlbum(const TMPXItemId &itemId, TMPXItemId albumId) const
{
    bool contains(false);
    QHash<TMPXItemId, QSet<TMPXItemId> >::const_iterator iter;
    if(albumId == TMPXItemId::InvalidId())
    {    
        // album id not specified, use the current album
        iter = mAlbumData.constFind(mCurrentAlbum);
    }
    else
    {
        iter = mAlbumData.constFind(albumId);
    }     
    if (iter != mAlbumData.constEnd())
    {
        contains = iter->contains(itemId);
    }
    return contains;
}

// -----------------------------------------------------------------------------
// setAlbumInUse
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::setAlbumInUse(const TMPXItemId &albumId)
{
	FUNC_LOG;
    mCurrentAlbum = albumId;
}

// -----------------------------------------------------------------------------
// removeItemsFromAlbum
// -----------------------------------------------------------------------------
//
int VideoListDataModelPrivate::removeItemsFromAlbum(const TMPXItemId &albumId, 
                                        const QList<TMPXItemId> &items)
{
	FUNC_LOG;
    QHash<TMPXItemId, QSet<TMPXItemId> >::iterator iter = mAlbumData.find(albumId);
    if(iter == mAlbumData.end())
    {
        return 0;
    }
    int removeCount = 0;
    int count = items.count();
    for(int i = 0; i < count; ++i)
    {
        if(iter->remove(items.at(i)))
        {
            ++removeCount;
        }
    }   
    return removeCount;
}

// -----------------------------------------------------------------------------
// getMediaId
// -----------------------------------------------------------------------------
//
TMPXItemId VideoListDataModelPrivate::getMediaId(const CMPXMedia *media ) const
{
    TMPXItemId mediaId = TMPXItemId::InvalidId();
    
    VideoCollectionUtils::instance().mediaValue<TMPXItemId>(media, KMPXMediaGeneralId, mediaId );

    return mediaId;
}

// -----------------------------------------------------------------------------
// appendDataToContainerL
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::appendDataToContainerL(
    CMPXMediaArray *videoArray,
    unsigned int startIndex)
{
	FUNC_LOG;
	
    if (!videoArray ||
        startIndex >= videoArray->Count())
    {
        return;
    }
    
    INFO_2("VideoListDataModelPrivate::appendDataToContainerL() array count: %d, start index: %d", videoArray->Count(), startIndex);
    
    CMPXMedia *newMedia = 0;
    CMPXMedia *mediaFromArray = 0;
    TMPXItemId itemId = TMPXItemId::InvalidId();
    int count = videoArray->Count();
    
    for(int i = startIndex; i < count; ++i)
    {
        mediaFromArray = videoArray->AtL(i);
        itemId = getMediaId(mediaFromArray);
        if (isValid(*mediaFromArray, itemId))
        {
            newMedia = CMPXMedia::NewL(*mediaFromArray);
            mMediaData.append(newMedia);
            newMedia = 0;
        }
    }
}

// -----------------------------------------------------------------------------
// isValid
// -----------------------------------------------------------------------------
//
bool VideoListDataModelPrivate::isValid(const CMPXMedia &media,
    const TMPXItemId &itemId) const
{
    if(itemId == TMPXItemId::InvalidId())
    {
        return false;
    }    
    // temporary to remove all items with out a title
    QString title;
    VideoCollectionUtils::instance().mediaValue<QString>(&media,
        KMPXMediaGeneralTitle,
        title);
    if (!title.length())
    {
        return false;
    }
    // recently played and podcasts are not yet supported
    if(itemId.iId2 == KVcxMvcMediaTypeCategory && 
        (itemId.iId1 != KVcxMvcCategoryIdDownloads &&
         itemId.iId1 != KVcxMvcCategoryIdCaptured))
    {
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// albumDataChangedL
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::albumDataChangedL(TMPXItemId &albumId,
    CMPXMediaArray *videoArray)
{
	FUNC_LOG;
    if (!videoArray || albumId == TMPXItemId::InvalidId())
    {
        return;
    }
    QSet<TMPXItemId> items;
    
    int oldCount = mAlbumData[albumId].count();
    
    // remove existing
    mAlbumData.remove(albumId);
    
    int videoCount = videoArray->Count();
    CMPXMedia *media = 0;
    TMPXItemId id = TMPXItemId::InvalidId();
    for (int i = videoCount - 1; i >= 0; i--)
    {
        media = videoArray->AtL(i);
        id = getMediaId(media);
        if (id != TMPXItemId::InvalidId() && id.iId2 == KVcxMvcMediaTypeVideo)
        {          
            items.insert(id);
        }
    }  
 
    mAlbumData[albumId] = items;
    
    if(oldCount != items.count())
    {
        // signal that album has been updated
        emit q_ptr->albumChanged();

        // signal that model has changed.
        emit q_ptr->modelChanged();
    }
}


// -----------------------------------------------------------------------------
// thumbnailsFetchedSlot
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::thumbnailsFetchedSlot(QList<TMPXItemId> &mediaIds)
{
    TMPXItemId id;
    QModelIndex rowIndex;
    
    while(!mediaIds.isEmpty())
    {
        id = mediaIds.takeFirst();
        rowIndex = q_ptr->index(mMediaData.indexOfId(id), 0);
		if(rowIndex.isValid())
		{
	        emit q_ptr->dataChanged( rowIndex, rowIndex);
		}
    }
}

// -----------------------------------------------------------------------------
// newVideoListSlot
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::newVideoListSlot( CMPXMediaArray *newVideoList )
{
	FUNC_LOG;
    if( !newVideoList )
    {
        return;
    }
    
    int startIndex = mMediaData.count();

    TRAP_IGNORE(appendDataToContainerL(newVideoList));
	
    int endIndex = mMediaData.count() - 1;
    if(startIndex <= endIndex)
    {
        q_ptr->beginInsertRows(QModelIndex(), startIndex, endIndex);
        q_ptr->endInsertRows();
        emit q_ptr->modelChanged();
    }
}

// -----------------------------------------------------------------------------
// appendVideoListSlot
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::appendVideoListSlot(CMPXMediaArray* videoList)
{
	FUNC_LOG;
    if(!videoList)
    {
        return;
    }
    if(videoList->Count() == 0 || videoList->Count() <= mMediaData.count())
    {
        // nothing to append or count matches, no changes -> nothing to append
        return;
    }
            
    // array gotten is in same order as existing data from the start,
    // so we only need to go throught the appendding array from the index
    // at position where first new items is excepted to exists.    
    int startIndex = mMediaData.count();
    
    q_ptr->beginInsertRows(QModelIndex(), startIndex, videoList->Count() - 1);

    TRAP_IGNORE(appendDataToContainerL(videoList, startIndex));

    q_ptr->endInsertRows();
    emit q_ptr->modelChanged();
}

// -----------------------------------------------------------------------------
// newVideoAvailable
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::newVideoAvailableSlot(CMPXMedia *newVideo)
{
	FUNC_LOG;
    if(!newVideo ) 
    {
        return;        
    }

    // try to get id of the video, if not found consider invalid clip
    TMPXItemId mediaId = getMediaId(newVideo);
    if( mediaId == TMPXItemId::InvalidId())
    {
        return;
    }

    // append video into list, ownership of provided mediaobject is not transferred, 
    // so we need to create copy of CMPXMedia
    if (isValid(*newVideo, mediaId))
    {
        CMPXMedia *videoToAppend = 0;
        TRAP_IGNORE(videoToAppend = CMPXMedia::NewL(*newVideo));  
    
        q_ptr->beginInsertRows(QModelIndex(), mMediaData.count(), mMediaData.count());
    
        mMediaData.append(videoToAppend);    
  
        q_ptr->endInsertRows();
    
        emit q_ptr->modelChanged();
    }
}

// -----------------------------------------------------------------------------
// itemDeletedSlot
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::itemDeletedSlot(TMPXItemId &itemId)
{
	FUNC_LOG;
	if(itemId == TMPXItemId::InvalidId())
    {
        return;
    }    

	if(itemId.iId2 == KVcxMvcMediaTypeVideo)
    {
    	videoDeleted(itemId);
    }
    else if (itemId.iId2 == KVcxMvcMediaTypeAlbum)
    {
    	albumRemoved(itemId);
    }
}

// -----------------------------------------------------------------------------
// albumRemoved 
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::albumRemoved(TMPXItemId &albumId)
{
	FUNC_LOG;
    if (albumId == TMPXItemId::InvalidId() || albumId.iId2 != KVcxMvcMediaTypeAlbum)
    {
        return;
    }
    
    bool changed(false);

    if(mVideoThumbnailData)
    {
        mVideoThumbnailData->removeThumbnail( albumId );
    }

    int index = mMediaData.indexOfId(albumId);
    if(index >= 0)
    {
        q_ptr->beginRemoveRows(QModelIndex(), index, index);
        mMediaData.remove(albumId);            
        q_ptr->endRemoveRows();
        changed = true;
    }

    //remove album data
    QHash<TMPXItemId, QSet<TMPXItemId> >::iterator iter;
    iter =  mAlbumData.find(albumId);
    if(iter != mAlbumData.end())
    {
        iter->clear();
        mAlbumData.remove(albumId);
        changed = true;
    }

    if(changed)
    {
    	emit q_ptr->modelChanged();
    }
}

// -----------------------------------------------------------------------------
// videoDeleted
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::videoDeleted(TMPXItemId &videoId)
{
	FUNC_LOG;
    if(videoId == TMPXItemId::InvalidId())
    {
        return;
    }    
    // check if provided id is in the deleted -container and remove 
    // it from there if it is.
    QList<TMPXItemId> ids;
    ids.append(videoId);
    if(!mMediaData.clearRemoved(&ids))
    {
        q_ptr->mDeleteWorker->removeFromRequest(videoId);
        if(mVideoThumbnailData)
        {
            mVideoThumbnailData->removeThumbnail( videoId );
        }
        // media removed was not in the deleted -container, so we have external 
        // removal: in this case we need to indicate UI         
        int index = mMediaData.indexOfId(videoId);
        if(index >= 0)
        {
            q_ptr->beginRemoveRows(QModelIndex(), index, index);
            mMediaData.remove(videoId);
            q_ptr->endRemoveRows();
            emit q_ptr->modelChanged();
        }
    }     
}

// -----------------------------------------------------------------------------
// videoDeleteCompleted
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::videoDeleteCompletedSlot(int overallCount, QList<TMPXItemId> *failedMediaIds)
{    
	FUNC_LOG;
    Q_UNUSED(overallCount);
    if(!failedMediaIds)
    {
        // invalid signal received, do nothing
        return;
    }
    int status(VideoCollectionCommon::statusDeleteSucceed);
    QVariant data;
    if(failedMediaIds->count() > 0)
    {     
        QString nameOfFirstFailed("");
        CMPXMedia *media = mMediaData.getRemovedMedia(failedMediaIds->first());
        VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralTitle, nameOfFirstFailed );
        if(failedMediaIds->count() == 1)
        {
            status = VideoCollectionCommon::statusSingleDeleteFail;
            data = nameOfFirstFailed;
        }
        else
        {
            status = VideoCollectionCommon::statusMultipleDeleteFail;
            data = failedMediaIds->count();
        }  
        restoreRemoved(failedMediaIds);
    }
    q_ptr->reportAsyncStatus(status, data);
    emit q_ptr->modelChanged();
}

// -----------------------------------------------------------------------------
// albumRemoveFailureSlot
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::albumRemoveFailureSlot(QList<TMPXItemId> *failedMediaIds)
{
	FUNC_LOG;
    if(!failedMediaIds)
    {
        return;
    }
    int status(VideoCollectionCommon::statusRemoveSucceed);
    QVariant data;
    if(failedMediaIds->count() > 0)
    {     
        QString nameOfFirstFailed("");
        CMPXMedia *media = mMediaData.getRemovedMedia(failedMediaIds->first());
        VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralTitle, nameOfFirstFailed );
        if(failedMediaIds->count() == 1)
        {
            status = VideoCollectionCommon::statusSingleRemoveFail;
            data = nameOfFirstFailed;
        }
        else
        {
            status = VideoCollectionCommon::statusMultiRemoveFail;
            data = failedMediaIds->count();
        }  
    }   
    q_ptr->reportAsyncStatus(status, data);
}

// -----------------------------------------------------------------------------
// VideoDetailsCompleted 
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::videoDetailsCompletedSlot(CMPXMedia* media)
{
	FUNC_LOG;
    if(!media)
    {
        return;
    }
    
    using namespace VideoCollectionCommon;
    QMap<QString, QVariant> map;

    HbExtendedLocale locale = HbExtendedLocale::system();
    
    // MetaKeyDate
    QDateTime date = getVideoDate(media);
    if(date.isValid()) {
        map[MetaKeyDate] = locale.format( date.date(), r_qtn_date_usual );
    }
    
    // MetaKeyDurationString
    quint32 dur = getVideoDuration(media);
    QString duration = VideoCollectionUtils::instance().prepareLengthString(dur);
    if(!duration.isNull() && !duration.isEmpty()) {
        map[MetaKeyDurationString] = duration;
    }
    
    // MetaKeySizeString
    quint32 s;
    VideoCollectionUtils::instance().mediaValue<quint32>(media, KMPXMediaGeneralSize, s );
    QString size = VideoCollectionUtils::instance().prepareSizeString(s);
    if(!size.isNull() && !size.isEmpty()) {
        map[MetaKeySizeString] = size; 
    }
    
    // MetaKeyStarRating
    quint8 rating = 0;
    if(VideoCollectionUtils::instance().mediaValue<quint8>(media, KVcxMediaMyVideosRating, rating)) 
    {
        map[MetaKeyStarRating] = rating;
    }
    
    // MetaKeyDRMInfo
    
    // MetaKeyServiceURL
    
    // MetaKeyDescription
    QString desc;
    if(VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralComment, desc)) {
        map[MetaKeyDescription] = desc;
    }
    
    // MetaKeyModifiedDate
    quint64 dateTimeValue;
    if(VideoCollectionUtils::instance().mediaValue<quint64>(media, KVcxMediaMyVideosModifiedDate, dateTimeValue)) {
        TDateTime temp = TTime( dateTimeValue ).DateTime();
        QDateTime date = QDateTime(QDate(temp.Year(), temp.Month()+1, temp.Day()+1), 
                           QTime(temp.Hour(), temp.Minute(), temp.Second(), temp.MicroSecond()));
        map[MetaKeyModifiedDate] = locale.format( date.date(), r_qtn_date_usual );
    }

    // MetaKeyShotLocation
    
    // MetaKeyAuthor
    QString author;
    if(VideoCollectionUtils::instance().mediaValue<QString>(media, KVcxMediaMyVideosAuthor, author)) {
        map[MetaKeyAuthor] = author;
    }
    
    // MetaKeyCopyright
    QString copyright;
    if(VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralCopyright, copyright)) {
        map[MetaKeyCopyright] = copyright;
    }
    
    // MetaKeyAudioType
    
    // MetaKeyLanguageString
    QString language;
    if(VideoCollectionUtils::instance().mediaValue<QString>(media, KVcxMediaMyVideosAudioLanguage, language)) {
        map[MetaKeyLanguageString] = language;
    }
    
    // MetaKeyKeywords
    
    // MetaKeyVideoResolutionString
    quint16 width;
    quint16 heigth;
    if(VideoCollectionUtils::instance().mediaValue<quint16>(media, KMPXMediaVideoWidth, width) &&
       VideoCollectionUtils::instance().mediaValue<quint16>(media, KMPXMediaVideoHeight, heigth)) 
    {
        map[MetaKeyVideoResolutionString] = hbTrId("txt_videos_list_l1l2").arg(width).arg(heigth);
    }
    
    // MetaKeyBitRate
    qint16 bitrate;
    if(VideoCollectionUtils::instance().mediaValue<qint16>(media, KMPXMediaVideoBitRate, bitrate)) 
    {
        const char* loc = "txt_videos_list_l1_kbps";
        
        if(bitrate > 999)
        {
            loc = "txt_videos_list_l1_mbps";
            bitrate = (double)bitrate / (double)1000 + 0.5;
        }
        
        map[MetaKeyBitRate] = hbTrId(loc).arg(bitrate);
    }
    
    // MetaKeyFormat
    QString format;
    if(VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralMimeType, format)) {
        map[MetaKeyFormat] = format;
    }
    
    // MetaKeyFileName and MetaKeyFilePath
    QString fileName;
    if(VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralUri, fileName)) {
         
        QFileInfo fInfo(fileName);
        if(fInfo.isFile())
        {
            map[MetaKeyFileName] = fInfo.fileName();
            map[MetaKeyFilePath] = fInfo.absolutePath();
        }
    }
    
    // Metakey video title
    QString videoTitle;
    if(VideoCollectionUtils::instance().mediaValue<QString>(media, KMPXMediaGeneralTitle, videoTitle)) {
        map[MetaKeyVideoTitle] = videoTitle;
    }
            
        
    QVariant variant = QVariant(map);
    emit q_ptr->fullVideoDetailsReady(variant);
}

// -----------------------------------------------------------------------------
// albumListAvailableSlot 
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::albumListAvailableSlot(TMPXItemId &albumId,
    CMPXMediaArray *albumItems)
{
	FUNC_LOG;
    // currently only one album is kept in memory
    TRAP_IGNORE(albumDataChangedL(albumId, albumItems));
}

// -----------------------------------------------------------------------------
// itemModifiedSlot 
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::itemModifiedSlot(const TMPXItemId &itemId)
{
	FUNC_LOG;
    int index = mMediaData.indexOfId(itemId);
    if(index != -1)
    {
        QModelIndex rowIndex = q_ptr->index(index, 0);
        if(rowIndex.isValid())
        {            
            emit q_ptr->dataChanged(rowIndex, rowIndex);            
            emit q_ptr->modelChanged();
        }
    }
}

// -----------------------------------------------------------------------------
// videoListCompleteSlot 
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::videoListCompleteSlot()
{
    emit q_ptr->modelReady();
}

// -----------------------------------------------------------------------------
// albumListCompleteSlot 
// -----------------------------------------------------------------------------
//
void VideoListDataModelPrivate::albumListCompleteSlot()
{
    emit q_ptr->albumListReady();
}

// End of file
