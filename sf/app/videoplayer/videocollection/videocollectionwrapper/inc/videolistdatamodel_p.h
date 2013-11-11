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
* Description:   VideoListDataModelPrivate class definition*
*/

#ifndef __VIDEOLISTDATAMODEL_P_H__
#define __VIDEOLISTDATAMODEL_P_H__


// INCLUDES
#include <QObject>
#include <qicon.h>
#include <qdatetime.h>
#include <qhash.h>
#include <qset.h>
#include <qabstractitemmodel.h>
#include <e32const.h>
#include <mpxitemid.h>
#include "videodatasignalreceiver.h"
#include "videodatacontainer.h"

// FORWARD DECLARATIONS
class VideoListDataModel;
class CMPXMediaArray;
class CMPXMedia;
class VideoThumbnailData;

class VideoListDataModelPrivate : public VideoDataSignalReceiver
{    
    /**
     * define to be able to use signals and slots
     */
    Q_OBJECT
    
    /**
     * disable copy-constructor and assignment operator
     */
	Q_DISABLE_COPY(VideoListDataModelPrivate) 
    
	/**
	 * we're friend of the actual model object
	 */
    friend class VideoListDataModel;

public: // Constructor

	/**
	 * Default constructor
     */
    VideoListDataModelPrivate(VideoListDataModel *model);	
	
	/**
     * Destructor
     */
	~VideoListDataModelPrivate();	
		
	/**
	 * Method initializes class members:
	 * Creates thumbnail data -object and connect it's signals.
	 * 
	 * - VideoThumbnailData: thumbnailsFetched
	 * ---> VideoListData: thumbnailsFetchedSlot
	 * 
	 * @return int, 0 if ok.
	 */
	int initialize();

private slots:

    /**
     * This slot is connected to VideoThumbnailData::thumbnailsFetched -signal
     * to get notified when thumbnail(s) for some particular item(s) is ready.
     * 
     * The indexes of the items are checked and videoDataChanged signal is emitted.
     * 
     * @param mediaIds: media ids of the items  
     * 
     */
    void thumbnailsFetchedSlot(QList<TMPXItemId> &mediaIds);
    
private slots: //slots from VideoDataSignalReceiver    
    
    /**
     * Signaled by the collection client when there is new video list available.
     * Method emits videosInserted -signal.
     * 
     * @param newVideoList: new list of videos
     * 
     */
    void newVideoListSlot(CMPXMediaArray *newVideoList);
      
    /**
     * Signaled by the collection client when there are new videos appended
     * into the existing videolist. 
     * Checks if medias at provided array does not exist at the existing
     * array and appends them if they do not.
     * 
     * @param videoList: list of videos provided for appending
     */
    void appendVideoListSlot(CMPXMediaArray* videoList);
        
    /**
     * Signaled by the collection client when new videodata is fetched.
     * Given object is inserted into correct position in the 
     * media array and videosInserted -signal is emitted
     * 
     * @param aVideo data of the new video.
     */
    void newVideoAvailableSlot(CMPXMedia *newVideo);
    
    /**
     * Signaled by the collection client when
     * item removal has detected.
     * 
     * @param itemId id of the removed item
     */
    void itemDeletedSlot(TMPXItemId &itemId);

    /**
     * Signaled by the collection client video deletion request
     * has completed
     * 
     * @param overallCount count of ALL videos in this delete procedure
     * @param failedMediaIds list of item ids that failed to be removed.
     */
    void videoDeleteCompletedSlot(int overallCount, QList<TMPXItemId> *failedMediaIds);
     
    
    /**
     * Signaled when album remove command is completed and some albums were not removed.
     * 
     * @param failedMediaIds media-array containing failed medias
     */
    void albumRemoveFailureSlot(QList<TMPXItemId> *failedMediaIds);
    
    /**
     * Signaled by the collection client when video details fetching
     * has completed.
     * 
     */
    void videoDetailsCompletedSlot(CMPXMedia* media);      
    
    /**
     * Signalled when album items are recieved.
     * 
     * @param albumId, Album which items are received.
     * @param albumItems, Items belonging to the current album.
     */
    void albumListAvailableSlot(TMPXItemId &albumId, CMPXMediaArray *albumItems);
    
    /**
     * Signalled when item has been mofidied.
     * 
     * @param itemId, item that has been modified.
     */
    void itemModifiedSlot(const TMPXItemId &itemId);
    
    /**
     * Signalled when video list is complete.
     */
    void videoListCompleteSlot();
    
    /**
     * Signalled when album list is complete.
     */
    void albumListCompleteSlot();

public: // services 
    
    /**
     * Returns video count
     * 
     * @return int count of videos
     */
    int getVideoCount();
    
    /**
     * Returns the MPX media item id (TMPXItemId.id1)
     * from the given index.
     * 
     * @param index: item position where client wants the id from 
     * 
     * @return TMPXItemId item id or invalid TMPXItemId if id not found at that index
     */
    const TMPXItemId& getMediaIdFromIndex(const int &index) const;
        
    /**
     * Returns name of the video from the given index.
     * If there is no item at that index, empty string is returned.
     * 
     * @param index: item position where client wants the name from 
     * 
     * @return QString name of the item
     */
    const QString getVideoNameFromIndex(const int &index)  const;
      
    /**
     * Returns the thumbnail of the item from given index.
     * If there is no item at that index or thumbnail is not yet fetched,
     * default thumbnail is returned.
     * 
     * @param index: item position where client wants the tn from 
     * 
     * @return QIcon thumbnail of the item
     */
    const QIcon* getVideoThumbnailFromIndex(const int &index) const;
    
    /**
     * Returns the value for item count attribute from given index. 
	 * If there is no item at that index, 0 is returned.
     * 
     * @param index: item position where client wants the count from 
     * 
     * @return guint32 size in bytes
     */
    quint32 getCategoryVideoCountFromIndex(const int &index ) const;
    
    /**
     * Returns the size of the video from given index.
     * If there is no item at that index, 0 is returned
     * 
     * @param index: item position where client wants the size from 
     * 
     * @return guint32 size in bytes
     */
    quint32 getVideoSizeFromIndex(const int &index) const;
    
    /**
     * Returns the age profile of the video from given index
     * If there is no item at that index, 0 is returned
     * 
     * @param index: item position where client wants the size from 
     * 
     * @return quint32 age profile in bytes
     */
    quint32 getVideoAgeProfileFromIndex(const int &index) const;
    
    /**
     * Returns the duration of the video from given index.
     * If there is no item at that index, 0 is returned
     * 
     * @param index: item position where client wants the duration from 
     * 
     * @return guint32 duration
     */
    quint32 getVideodurationFromIndex(const int &index) const;
    
    /**
     * Returns the duration of the video from given media.
     * 
     * @param media The video media object. 
     * 
     * @return guint32 duration
     */
    quint32 getVideoDuration(CMPXMedia* media) const;
    
    /**
     * Returns the creation/download date of the video.
     * 
     * @param index: item position where client wants the date from.
     * 
     * @return QDate date
     */
    QDateTime getVideoDateFromIndex(const int &index) const;
    
    /**
     * Returns the creation/download date of the video
     * 
     * @param media The video media object.
     */
    QDateTime getVideoDate(CMPXMedia* media) const;
    
    /**
     * marks videos to be removed: it's id and index are saved to 
     * remove buffer in data container
     * 
     * @param itemIndex: index of item to be removed
     * @return TMPXItemId: id of the item marked
     *
     */
    const TMPXItemId markVideoRemoved(const QModelIndex &itemIndex);
    
    /**
     * Removes provided ids from the remove -list
     * and restores them into actual container in case
     * our current collection level is correct.
     * 
     */
    void restoreRemoved(QList<TMPXItemId> *idList);
	
	/**
     * Returns the file path of the video.
     * 
     * @param index: item position where client wants the file path from.
     */
    const QString getFilePathFromIndex(const int &index) const;
    
	/**
     * Returns the file path of the video.
     * 
     * @param mediaId: id for the item
     */
    const QString getFilePathForId(const TMPXItemId &mediaId);
    
    /**
     * Checks if the specified item belongs to currently open album.
     * 
     * @albumId, Album where the item is to be checked.
     * @param itemId, Item to be checked.
     * @return true if item belongs to the album.
     */
    bool belongsToAlbum(const TMPXItemId &itemId, TMPXItemId albumId = TMPXItemId::InvalidId()) const;
    
    /**
     * Set album in use.
     * 
     * @param albumId, Album to set in use.
     * @return None.
     */
    void setAlbumInUse(const TMPXItemId &albumId);
    
    /**
     * removes provided items from album provided
     * 
     * @param albumId album from where to remove
     * @param items to remove
     * 
     * @return count of item removed;
     */
    int removeItemsFromAlbum(const TMPXItemId &albumId, const QList<TMPXItemId> &items);

private: // private methods
  
    /**
     * private default constructor definition
     */
    VideoListDataModelPrivate();
    
    /**
     * Method clears map containing media objects
     */
    void clearVideoData();
    
    /**
     * Method used to parse media id from given object.
     * Id id data is not available or given object is null.
     * method returns -1. 
     * The id is CMPXMedia's TMPXItemId.iId1
     * 
     * @param media: where to get id from 
     * 
     * @return TMPXItemId: id of the item
     */
    TMPXItemId getMediaId(const CMPXMedia *media ) const;
    
    /**
     * Leaving helper method for appending data from media array into 
     * data container for reducing amount of TRAPs.
     * 
     * Leaves if CMPXMedia object creation fails.
     * 
     * @param videoArray array from where to take data
     * @param startIndex startindex from where start appending.
     *  
     */
    void appendDataToContainerL(CMPXMediaArray *videoArray, unsigned int startIndex = 0);
    
    /**
     * Check that a media item is valid.
     * 
     * @param itemId, Item to be checked.
     * @return true if valid.
     */
    bool isValid(const CMPXMedia &media, const TMPXItemId &itemId) const;
    
    /**
     * Album data changed, resets album data from array provided
     * 
     * @param albumId id of album
     * @param videoarray array of videos in album 
     */
    void albumDataChangedL(TMPXItemId &albumId, CMPXMediaArray *videoArray);

    /**
     * Called when an album has been removed.
     * 
     * @param albumId, Album which items are received.
     */
    void albumRemoved(TMPXItemId &albumId);
   
    /**
     * Called when a video has been removed.
     * 
     * @param videoId id of the removed video
     */
    void videoDeleted(TMPXItemId &videoId);

private:
    
    /**
     * Pointer to the public class
     * Not own.
     */
    VideoListDataModel *q_ptr;
    
   /**
    * media object container
    */
    VideoDataContainer mMediaData;  
    
    /**
     * Thumbnail handler object, not owned
     */
    VideoThumbnailData *mVideoThumbnailData;
   
	/**
	 * Object is initialized.
	 */
	bool mInitialized;

	/** 
	 * "wrapped" album container, contains item ids belongint into a particular album
	 * or category 
	 */
	QHash<TMPXItemId, QSet<TMPXItemId> > mAlbumData;
	
	/** 
	 * currently open album 
	 */
	TMPXItemId mCurrentAlbum;
};

#endif  // __VIDEOLISTDATAMODEL_P_H__

// End of file
