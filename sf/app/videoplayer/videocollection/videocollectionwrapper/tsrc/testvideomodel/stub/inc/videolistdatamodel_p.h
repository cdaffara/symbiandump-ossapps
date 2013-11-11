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
* Description:  stubclass of videolistdata for testing VideoListDataModel class methods*
*/


#ifndef __VIDEOLISTDATA_H__
#define __VIDEOLISTDATA_H__


// INCLUDES
#include <QObject>
#include <QDate>
#include <QIcon>
#include <QSet>
#include <qabstractitemmodel.h>
#include "videodatasignalreceiver.h"


// FORWARD DECLARATIONS
class CMPXMediaArray;
class VideoThumbnailData;


class VideoListDataModel;
class CMPXMediaArray;
class DummyData;




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
    
    friend class DummyData;
    
public:

    /**
     * Default constructor
     */
    VideoListDataModelPrivate(VideoListDataModel *model); 
    
    /**
     * Destructor
     */
    ~VideoListDataModelPrivate();  
	
	/**
     * returns 0 or -1 based on global init failure flag
     * if flag is true, return 0
     * 
     * @return int 0 or -1
     */
	int initialize();
	
	/**
	 * calls model's sidconnect -method (implemented for coverity's sake)
	 */
	void callModelDisconnect();
	
	/**
	 * calls model object's reportAsyncStatus
	 */
	void callModelAsyncReport(int status, QVariant data);
	
signals:

    /**
     * Not emitted from here but had to be defined in order to 
     * get stub into use to testable object 
     */
    void dataChanged(const QModelIndex &startIndex, const QModelIndex &endIndex);

    /**
     * Not emitted from here but had to be defined in order to 
     * get stub into use to testable object 
     */
    void videoDetailsReady(QVariant&);

    
public: // services

    /**
     * return count of mMediaArray
     * 
     * @rerurn int
     */
    int getVideoCount();
   
    /**
     * return video id of the item at given index
     * 
     * @param index
     * @return int
     */
    TMPXItemId& getMediaIdFromIndex(const int &index) const;
    
    /**
    * return video path of the item with given media id
    * 
    * @param mediaId
    * @return int
    */
    const QString getFilePathForId(const TMPXItemId &mediaId) const;
           
    /**
     * return name of the item at given index
     * 
     * @param index
     * @return QString
     */
    const QString getVideoNameFromIndex(const int &index )  const;
   
    /**
     * return default tn always
     * 
     * @param index
     * @return QIcon
     */
    const QIcon* getVideoThumbnailFromIndex(const int &index ) const;
   
    /**
     * return video count of the item at given index
     * 
     * @param index
     * @return quint32
     */
    quint32 getCategoryVideoCountFromIndex(const int &index ) const;
    
    /**
     * return video size of the item at given index
     * 
     * @param index
     * @return quint32
     */
    quint32 getVideoSizeFromIndex(const int &index ) const;
   
    /**
     * return video age profile of the item at given index
     * 
     * @param index
     * @return quint32
     */
    quint32 getVideoAgeProfileFromIndex(const int &index ) const;
   
    /**
     * return video duration of the item at given index
     * 
     * @param index
     * @return quint32
     */
    quint32 getVideodurationFromIndex(const int &index ) const;
   
    /**
     * return video creation datetime of the item at given index
     * 
     * @param index
     * @return QDateTime
     */
    QDateTime getVideoDateFromIndex(const int &index ) const;
   
    /**
     * Returns video status
     * 
     * @param index: item position where client wants the date from.
     * 
     * @return int status code
     */
    int getVideoStatusFromIndex(const int &index) const;
    
    /**
     * marks videos to be removed: it's id and index are saved to 
     * remove buffer in data container
     * 
     * @param itemIndex: index of item to be removed
     * @return TMPXItemId: id of the item marked
     *
     */
    TMPXItemId markVideoRemoved(const QModelIndex &itemIndex);

    /**
     * Removes provided ids from the remove -list
     * 
     * @param itemIds ids of the items to be removed
     */
    void restoreRemoved(QList<TMPXItemId> *idList);
	
	/**
     * Returns the file path of the video.
     * 
     * @param index: item position where client wants the file path from.
     */
    const QString getFilePathFromIndex(const int &index) const;
    
    /**
     * returns mBelongsToAlbum;
     */
    bool belongsToAlbum(const TMPXItemId &itemId, TMPXItemId albumId);
    
    /**
     * returns mBelongsToAlbum;
     */
    void setAlbumInUse(const TMPXItemId &albumId);
    
    /**
     * returns mRemoveFrAlbumReturn
     */
    int removeItemsFromAlbum(const TMPXItemId &albumId, const QList<TMPXItemId> &ids);
    
    
private: // private methods
    
    /**
     * return id of given item
     * 
     * @param CMPXMedia
     * @return TMPXItemId
     */
    TMPXItemId getMediaId(const CMPXMedia *media ) const;
    
    /**
     * return item from given index of given item
     * 
     * @param index
     * @return CMPXMedia
     */
    CMPXMedia* getMediaFromIndex(const int &index) const;
 
    /**
     * return index of item of given id
     * 
     * @param mediaId
     * @return int
     */
    int indexOfMediaId(const TMPXItemId &mediaId) const;
    
      
public slots: 
    
    /**
    * ssaves given list to mMediaArray
    * 
    * @param aVideoList: video list
    * 
    */
    void newVideoListSlot(CMPXMediaArray* aVideoList);
    
    /**
     * not used in stub
     * 
     */
    void appendVideoListSlot(CMPXMediaArray * videoList); 
    
    /**
     * not used in stub
     * 
     */
    void newVideoAvailableSlot(CMPXMedia* aVideo);
          
    /**
     * used to clear mItemsUnderDeletion for this stub
     * 
     */
    void videoDeleteCompletedSlot(int overallCount,QList<TMPXItemId> *failedMediaIds);
    
    /**
     * not used in stub
     * 
     */
    void videoDetailsCompletedSlot(CMPXMedia* media);
    
    /**
     * not used in stub
     */
    void albumListAvailableSlot(TMPXItemId &albumId, CMPXMediaArray *albumItems);
    
    /**
     * not used in stub
     */
    void itemModifiedSlot(const TMPXItemId &itemId);    
    
    /**
     * not used in stub
     */
    void albumRemoveFailureSlot(QList<TMPXItemId> *items);

    /**
     * not used in stub
     */
    void itemDeletedSlot(TMPXItemId &id);
    
    /**
     * not used in stub
     */
    void videoListCompleteSlot();
    
    /**
     * not used in stub
     */
    void albumListCompleteSlot();
    
public:    
    
    /**
    * Pointer to the public class
    * Not own.
    */
    VideoListDataModel *q_ptr;
 
    /**
     * dummy media data wrapper
     */
    DummyData &mMediaData;
    
    /**
     * pointer to shared-array of medias
     */
    CMPXMediaArray *mMediaArray;
    
    /**
     * default icon
     */
    QIcon          mIcon; 
    
    /**
     * marked as removed -map
     */
    QList<TMPXItemId> mItemsUnderDeletion;
    
    /**
     * setted in setAlbumInUse
     */
    TMPXItemId mCurrentAlbum;
    
    /**
     * if true, markVideoRemoved returns invalid id
     */
    static bool mReturnInvalidMarked;
    
    /**
     * if true, initialize fails
     */
    static bool mFailInit;

    /**
     * if true, size returned is mVideoCount
     */
    static bool mOverrideVideoCount;
    
    /**
     * override video count value
     */
    static quint32 mVideoCount;
    
    /**
     * if true, size returned is mSize
     */
    static bool mOverrideSize;
    
    /**
     * overrided size value
     */
    static quint32 mSize;

    /**
     * if true, dureation returned is mDuration
     */
    static bool mOverrideDuration;
    
    /**
     * overrided duration
     */
    static quint32 mDuration;

    /**
     * value from videocount to be returned
     */
    static int mGetVideoCountFail;
    
    /**
     * value from belongsToAlbum to be returned
     */
    static bool mBelongsToAlbum;
    
    /**
     * value returned from removeItemsFromAlbum
     */
    static int mRemoveFrAlbumReturn;
    
    /**
     * value returned from getMediaIdFromIndex
     */
    static TMPXItemId mItemIdToReturn;
    
};

/**
 * dummy data wrapper class
 */
class DummyData
{
public:
    
    int indexOfId(TMPXItemId id)
    {
        if(!mObj->mMediaArray)
        {
            return -1;
        }

        for(int i = 0; i < mObj->getVideoCount(); ++i)
        {
            if(id == mObj->getMediaId(mObj->getMediaFromIndex(i)))
            {
                return i;
            }
        }
        return -1;
    }
    
    VideoListDataModelPrivate *mObj;
};

#endif  // __VIDEOLISTDATA_H__

// End of file
    


