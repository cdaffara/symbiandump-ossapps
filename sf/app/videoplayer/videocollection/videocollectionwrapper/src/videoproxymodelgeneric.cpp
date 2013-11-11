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
* Description: VideoProxyModelGeneric implementation
*
*/

// Version : 

// INCLUDE FILES
#include <qstringlist.h>
#include <qset.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <vcxmyvideosdefs.h>

#include "videothumbnaildata.h"
#include "videocollectioncommon.h"
#include "videoproxymodelgeneric.h"
#include "videolistdatamodel.h"
#include "videocollectionclient.h"
#include "videocollectionwrapper.h"
#include "videocollectiontrace.h"

const TMPXItemId INVALID_ID = TMPXItemId::InvalidId();

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::VideoProxyModelGeneric
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric::VideoProxyModelGeneric(QObject *parent) :
QSortFilterProxyModel(parent),
mModel(0),
mCollectionClient(0),
mLevel(VideoCollectionCommon::ELevelInvalid),
mGenericFilterId(TMPXItemId::InvalidId()),
mGenericFilterValue(false),
mIdleSortTimer(0),
mWantedSortRole(VideoCollectionCommon::KeyDateTime),
mWantedSortOrder(Qt::AscendingOrder),
mDefaultSortRole(VideoCollectionCommon::KeyTitle)
{
	FUNC_LOG_ADDR(this);
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::~VideoProxyModelGeneric
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric::~VideoProxyModelGeneric()
{
	FUNC_LOG_ADDR(this);
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::initialize
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::initialize(VideoListDataModel *sourceModel)
{
    FUNC_LOG_ADDR(this);
    if(!sourceModel) 
    {
        return -1;
    }
    mModel = sourceModel;
    if(!connectSignals())
    {
        ERROR_1(-1, "VideoProxyModelGeneric::initialize() [0x%x] failed to connect signals.", this);
        disconnectSignals();
        mModel = 0;
        return -1;
    }
    mCollectionClient = sourceModel->getCollectionClient();
    if(!mCollectionClient)
    {
        mModel = 0;
        return -1;
    }
    setSourceModel(sourceModel);
    return 0;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::connectSignals
// -----------------------------------------------------------------------------
//
bool VideoProxyModelGeneric::connectSignals()
{
    FUNC_LOG_ADDR(this);

    if(!connect(mModel, SIGNAL(modelChanged()),
                    this, SLOT(invalidate()))) 
    {
        return false;
    }

    return true;
}
   
// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::disconnectSignals
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::disconnectSignals()
{
    FUNC_LOG_ADDR(this);
    
    disconnect(mModel, SIGNAL(modelChanged()), this, SLOT(invalidate()));
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::open
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::open(VideoCollectionCommon::TCollectionLevels level)
{
    FUNC_LOG_ADDR(this);
    INFO_2("VideoProxyModelGeneric::open() [0x%x] level: %d", this, level);
    
    if(!mCollectionClient)
    {
        return -1;
    }

    if(mLevel != level) 
    {
       INFO_1("VideoProxyModelGeneric::open() [0x%x] opening different level, invalidating.", this);
       mLevel = level;
       invalidateFilter();
       // sorting call required here to setup correct sort order in cases where source model allready 
       // contains items but proxy is not yet updated. (invalidate -call does not work since it updates proxy and
       // calls sort in different order for us to use)
       sort(0, mWantedSortOrder);
    }
    // need to call open every time to make sure all items are 
    // inserted to UI ( recent open might have been cancelled)
    return mCollectionClient->startOpenCollection(level);
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::doSorting
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::doSorting(int sortingRole, Qt::SortOrder order, 
        bool async)
{
    FUNC_LOG_ADDR(this);
    INFO_3("VideoProxyModelGeneric::doSorting() [0x%x] sortingRole: %d, order: %d", this, sortingRole, order);
    
    if(sortingRole != VideoCollectionCommon::KeyTitle &&
       sortingRole != VideoCollectionCommon::KeySizeValue &&
       sortingRole != VideoCollectionCommon::KeyDateTime &&
       sortingRole != VideoCollectionCommon::KeyNumberOfItems)
    {
        mWantedSortRole = mDefaultSortRole;
    }
    else
    {
        mWantedSortRole = sortingRole;
    }
    
    mWantedSortOrder = order;
    
    if(async)
    {
        if(mIdleSortTimer)
        {
            if(mIdleSortTimer->isActive())
            {
                mIdleSortTimer->stop();
            }
        }
        else
        {
            mIdleSortTimer = new QTimer(this);
            connect(mIdleSortTimer, SIGNAL(timeout()), 
                    this, SLOT(processSortingSlot()));
        }
        mIdleSortTimer->start(0);
    }
    else
    {
        processSorting();
    }
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::getSorting
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::getSorting(int &sortingRole, Qt::SortOrder &order)
{
    FUNC_LOG_ADDR(this);
    sortingRole = mWantedSortRole;
    order       = mWantedSortOrder;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::deleteItems
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::deleteItems(const QModelIndexList &indexList)
{
    FUNC_LOG_ADDR(this);
    
    // Disable thumbnail fetching while items are removed from the model. 
    VideoThumbnailData::instance().enableBackgroundFetching(false);
    
    if(mModel)
    {
        QModelIndexList mappedList;
        for(int i = 0; i < indexList.count(); ++i)
        {
            mappedList.append(mapToSource(indexList.at(i)));
        }
        
        if(mModel->removeRows(mappedList))
        {
            // Start fetching thumbnails at start of the model.
            VideoThumbnailData::instance().enableBackgroundFetching(true);
            VideoThumbnailData::instance().startBackgroundFetching(0, 0);
            return 0;
        }
    }
    
    VideoThumbnailData::instance().enableBackgroundFetching(true);
    return -1;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::openItem
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::openItem(TMPXItemId mediaId)
{
    FUNC_LOG_ADDR(this);
    if(mediaId != TMPXItemId::InvalidId() && mCollectionClient)
    {
        if(mCollectionClient->openItem(mediaId) == 0)
        {
            if(mediaId.iId2 != KVcxMvcMediaTypeVideo)
            { 
                mLevel = VideoCollectionCommon::ELevelAlbum;
                mModel->setAlbumInUse(mediaId);
                INFO_1("VideoProxyModelGeneric::open() [0x%x] opening album or category, invalidating.", this);
                invalidateFilter();
                // sorting call required here to setup correct sort order in cases where source model allready 
                // contains items but proxy is not yet updated. (invalidate -call does not work since it updates proxy and
                // calls sort in different order for us to use)
                sort(0, mWantedSortOrder);
            } 
            return 0;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::back
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::back()
{
    FUNC_LOG_ADDR(this);
    if(mCollectionClient && mCollectionClient->back() == 0)
    {
        if(mLevel == VideoCollectionCommon::ELevelAlbum)
        {
            mLevel = VideoCollectionCommon::ELevelCategory;
        }
        else
        {
            mLevel = VideoCollectionCommon::ELevelVideos;
        }
        return 0;
    }
    return -1;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::fetchItemDetails
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::fetchItemDetails(const QModelIndex &index)
{
    FUNC_LOG_ADDR(this);
    // no need to map index beforehand, because 
    // getMediaIdAtIndex maps it
    TMPXItemId mpxId1 = getMediaIdAtIndex(index);
    if(mpxId1 != TMPXItemId::InvalidId() && mCollectionClient)
    {
        if(mCollectionClient->getVideoDetails(mpxId1) == 0)
        {
            emit shortDetailsReady(mpxId1);
            return 0;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::lessThan
// -----------------------------------------------------------------------------
//
bool VideoProxyModelGeneric::lessThan(const QModelIndex &left,
    const QModelIndex &right) const
{
    if (!mModel)
    {
        return false;
    }
    
    // Do comparisation based on the role:
    //  VideoCollectionCommon::KeyTitle,
    //  VideoCollectionCommon::KeySizeValue,
    //  VideoCollectionCommon::KeyNumberOfItems,
    //  VideoCollectionCommon::KeyDateValue
    // 
    // If role does not match, do not sort
    int sRole = sortRole();
    QVariant leftData = sourceModel()->data(left, sRole);
    QVariant rightData = sourceModel()->data(right, sRole);

    if(!leftData.isValid() || !rightData.isValid())
    {
        return false;
    }
    if(sRole == VideoCollectionCommon::KeyTitle)
    {
        QString leftString = leftData.toString().toUpper();
        QString rightString = rightData.toString().toUpper();
        return QString::localeAwareCompare(leftString, rightString) < 0;
    }
    else if(sRole == VideoCollectionCommon::KeySizeValue ||
            sRole == VideoCollectionCommon::KeyNumberOfItems)
    {
        quint32 leftSize = leftData.toUInt();
        quint32 rightSize = rightData.toUInt();
        return leftSize < rightSize;
    }
    else if(sRole == VideoCollectionCommon::KeyDateTime)
    {
        QDateTime leftDateTime = leftData.toDateTime();
        QDateTime rightDateTime = rightData.toDateTime();
        // datetime sorting role has inverted sorting order compared to other roles 
        return rightDateTime < leftDateTime;
    }
    return false;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::filterAcceptsRow
// -----------------------------------------------------------------------------
//
bool VideoProxyModelGeneric::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
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
    if(id == TMPXItemId::InvalidId())
    {
        return false;
    }

    if(mLevel == VideoCollectionCommon::ELevelVideos && id.iId2 == KVcxMvcMediaTypeVideo)
    {
        //  filter items that belong to that album setted as filter id
        // if there's no filter or setted filter is "all videos", we accept everything
        if(mGenericFilterId == TMPXItemId::InvalidId() || 
           (mGenericFilterId.iId1 == KVcxMvcCategoryIdAll && 
            mGenericFilterId.iId2 == KVcxMvcMediaTypeCategory) ||
            mGenericFilterValue == mModel->belongsToAlbum(id, mGenericFilterId))
        {
            return true;
        }
    }
    else if(mLevel == VideoCollectionCommon::ELevelCategory && id.iId2 == KVcxMvcMediaTypeAlbum)    
    {
        // we do not filter albums yet
        return true;
    }  

    return false;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::getMediaIdAtIndex()
// -----------------------------------------------------------------------------
//
const TMPXItemId& VideoProxyModelGeneric::getMediaIdAtIndex(const QModelIndex &index) const
{
    if(index.isValid())
    {
        int rowIndex = mapToSource(index).row();
        if(mModel)
        {
            return mModel->mediaIdAtIndex(rowIndex);
        }
    }
    return INVALID_ID;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::indexOfId()
// -----------------------------------------------------------------------------
//
QModelIndex VideoProxyModelGeneric::indexOfId(const TMPXItemId &id)
{    
    QModelIndex sourceIndex;
    if(!mModel || id == TMPXItemId::InvalidId())
    {
        return sourceIndex;
    }
    sourceIndex = mModel->indexOfId(id);
    return mapFromSource(sourceIndex);
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::getMediaFilePathForId()
// -----------------------------------------------------------------------------
//
QString VideoProxyModelGeneric::getMediaFilePathForId(const TMPXItemId &mediaId)
{
    QString filePath;
    if(mModel)
    {
        filePath = mModel->mediaFilePathForId(mediaId);
    }
    return filePath;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::addNewAlbum()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoProxyModelGeneric::addNewAlbum(const QString &title)
{
    FUNC_LOG_ADDR(this);
    TMPXItemId id = TMPXItemId::InvalidId();
    
    if (mCollectionClient)
    {
        id = mCollectionClient->addNewAlbum(title);
    }
    
    return id;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::removeAlbums()
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::removeAlbums(const QModelIndexList &indexList)
{
    FUNC_LOG_ADDR(this);
    int err(-1);
    
    if (mCollectionClient)
    {
        TMPXItemId mpxId;
        QList<TMPXItemId> ids;

        for(int i = 0; i < indexList.count(); ++i)
        {
            mpxId = getMediaIdAtIndex(indexList.at(i));
            if(mpxId != TMPXItemId::InvalidId() && 
               mpxId.iId2 == KVcxMvcMediaTypeAlbum)
            {
                ids.append(getMediaIdAtIndex(indexList.at(i)));
            }
        }
        
        if (ids.count())
        {
            err = mCollectionClient->removeAlbums(ids);
        }
    }

    return err;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::resolveAlbumName()
// -----------------------------------------------------------------------------
//
QString VideoProxyModelGeneric::resolveAlbumName(
    const QString& albumName) const
{
    FUNC_LOG_ADDR(this);
    QString resolvedName = albumName.trimmed();

    // for checking names, we need to use collection list proxy model 
    // to get all existing album names including default ones
    VideoProxyModelGeneric *collectionModel = 
                       VideoCollectionWrapper::instance().getCollectionsModel();
    if(!collectionModel || !mModel)
    {
        return resolvedName;
    }    

    int i(0);
    QModelIndex proxyIndex = collectionModel->index(i, 0);
    QSet<QString> names;
    QVariant data;
    // create set of existing names
    while (proxyIndex.isValid())
    {
        data = mModel->data(collectionModel->mapToSource(proxyIndex), 
                VideoCollectionCommon::KeyTitle);
        if (data.isValid())
        {
            names.insert(data.toString());
        }
        proxyIndex = collectionModel->index(++i, 0, QModelIndex());
    }
    i = 0;
    QString firstMatch("");
    // find a name that does not yet exists
    while(names.contains(resolvedName))
    {
        if (!firstMatch.length())
        {
            firstMatch = resolvedName;
        }
        ++i;
        resolvedName = tr("%1 (%2)").arg(firstMatch).arg(i);
    }
    return resolvedName;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::addItemsInAlbum()
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::addItemsInAlbum(TMPXItemId &albumId,
        const QList<TMPXItemId> &mediaIds)
{
    FUNC_LOG_ADDR(this);
    int err(-1);
    
    if (mCollectionClient)
    {        
        // add items in album
        err = mCollectionClient->addItemsInAlbum(albumId, mediaIds);
    }
    
    return err;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::removeItemsFromAlbum()
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::removeItemsFromAlbum(TMPXItemId &albumId, 
        const QList<TMPXItemId> &mediaIds)
{
    FUNC_LOG_ADDR(this);
    int err(-1);

    // Disable thumbnail fetching while items are removed from the model. 
    VideoThumbnailData::instance().enableBackgroundFetching(false);
    
    if (mModel)
    {
        // remove items in album
        err = mModel->removeItemsFromAlbum(albumId, mediaIds);
        if(err > 0)
        {
            // if there really were items to be removed, invalid filter
            INFO_1("VideoProxyModelGeneric::removeItemsFromAlbum() [0x%x] items removed, invaliding.", this);
            invalidateFilter();
            err = 0;
        }
    }
 
    VideoThumbnailData::instance().enableBackgroundFetching(true);
    return err;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::renameItem()
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::renameItem(const TMPXItemId &itemId, const QString &newTitle)
{
    FUNC_LOG_ADDR(this);
    int err(-1);

    if(mCollectionClient)
    {
        return mCollectionClient->renameItem(itemId, newTitle);
    }
    return err;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::getOpenItem()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoProxyModelGeneric::getOpenItem() const
{
    FUNC_LOG_ADDR(this);
    TMPXItemId itemId = TMPXItemId::InvalidId();
    return itemId;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::setGenericIdFilter()
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::setGenericIdFilter(const TMPXItemId &itemId, bool filterValue)
{
    FUNC_LOG_ADDR(this);
    INFO_3("VideoProxyModelGeneric::setGenericIdFilter() [0x%x] itemId.iId2: %d, filterValue: %d", this, itemId.iId2, filterValue);
    mGenericFilterId = itemId;
    mGenericFilterValue = filterValue;
    INFO_1("VideoProxyModelGeneric::setGenericIdFilter() [0x%x] invalidating.", this);
    invalidateFilter();
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::setAlbumInUse()
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::setAlbumInUse(const TMPXItemId &albumId)
{
    FUNC_LOG_ADDR(this);
    if(mModel)
    {
        INFO_1("VideoProxyModelGeneric::setAlbumInUse() [0x%x] invalidating.", this);
        mModel->setAlbumInUse(albumId);
        invalidateFilter();
    }
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::processSortingSlot
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::processSortingSlot()
{
    FUNC_LOG_ADDR(this);
    if(mIdleSortTimer)
    {
        mIdleSortTimer->stop();
    }    
    processSorting();
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::processSorting
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::processSorting()
{
    FUNC_LOG_ADDR(this);

    if(sortRole() != mWantedSortRole)
    {
        setSortRole(mWantedSortRole);
    }
    
    if(sortColumn() == -1 || sortOrder() != mWantedSortOrder)
    {
        // if sorting column is not yet setted, it indicates
        // that we've setted the sorting order for the first time
        // and sorting has not yet been done. After we have called sort once,
        // the sort columnt gets updated and following sorts gets executed
        // by setSortRole -call as long as role changes.
    
        // Another case is when we need to change the sort order, as there is
        // no other way of setting the sort order than calling sort().
        sort(0, mWantedSortOrder);
    }

    emit modelSorted();
}

// End of file
